#include <std_include.hpp>
#include "s4.hpp"
#include "zone.hpp"
#include "loader.hpp"

#include <game.hpp>

#include <utils/flags.hpp>

namespace zonetool::s4
{
	const game& definition();

	namespace
	{
		void write_asset_list(zone_buffer* buf, const std::size_t asset_count)
		{
			const auto string_count = buf->scriptstring_count();

			XAssetList list{};
			list.stringList.count = static_cast<int>(string_count);
			list.stringList.strings = string_count ? reinterpret_cast<const char**>(buf->data_following) : nullptr;
			list.assetCount = static_cast<unsigned int>(asset_count);
			list.assets = asset_count ? reinterpret_cast<XAsset*>(buf->data_following) : nullptr;

			buf->write(&list);
		}

		std::string resolve_asset_name(const std::int32_t type, const std::string& name)
		{
			if (!name.starts_with(',') && ignore_assets.contains({static_cast<std::uint32_t>(type), name}))
			{
				return renamed_asset_name(","s + name);
			}

			return renamed_asset_name(name);
		}

		void write_script_strings(zone_buffer* buf)
		{
			const auto count = buf->scriptstring_count();
			if (!count)
			{
				return;
			}

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			buf->align(7);

			std::uintptr_t none{};
			for (std::size_t index = 0; index < count; index++)
			{
				buf->write<std::uintptr_t>(buf->get_scriptstring(index) ? &buf->data_following : &none);
			}

			for (std::size_t index = 0; index < count; index++)
			{
				if (const auto string = buf->get_scriptstring(index))
				{
					buf->write_str(string);
				}
			}

			buf->pop_stream();
		}

		std::size_t align_offset(const std::size_t offset, const std::size_t alignment)
		{
			return (offset + alignment) & ~alignment;
		}

		bool opens_preload_region(const std::int32_t type)
		{
			switch (type)
			{
			case ASSET_TYPE_XMODEL_SURFS:
			case ASSET_TYPE_MATERIAL:
			case ASSET_TYPE_COMPUTESHADER:
			case ASSET_TYPE_LIBSHADER:
			case ASSET_TYPE_VERTEXSHADER:
			case ASSET_TYPE_HULLSHADER:
			case ASSET_TYPE_DOMAINSHADER:
			case ASSET_TYPE_PIXELSHADER:
			case ASSET_TYPE_TECHNIQUE_SET:
			case ASSET_TYPE_IMAGE:
			case ASSET_TYPE_SOUND_BANK:
			case ASSET_TYPE_SOUND_BANK_TRANSIENT:
			case ASSET_TYPE_STREAMING_INFO:
			case ASSET_TYPE_STREAM_KEY:
			case ASSET_TYPE_KEY_VALUE_PAIRS:
			case ASSET_TYPE_DECAL_VOLUME_MATERIAL:
			case ASSET_TYPE_DECAL_VOLUME_MASK:
				return true;
			default:
				return false;
			}
		}
	}

	asset_reference::asset_reference(const std::int32_t type, const std::string& name)
		: name_(name)
		, type_(type)
		, data_(get_asset_loader(static_cast<XAssetType>(type)).size)
	{
		*get_asset_loader(static_cast<XAssetType>(type)).name(this->data_.data()) = this->name_.data();
	}

	void asset_reference::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->data_.data(), this->data_.size());

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		*get_asset_loader(static_cast<XAssetType>(this->type_)).name(dest) = buf->write_str(this->name_);
		buf->pop_stream();
	}

	asset_interface* zone_interface::find_asset(const std::int32_t type, const std::string& name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		const auto renamed = renamed_asset_name(name);
		for (const auto& asset : this->m_assets)
		{
			if (asset->type() == type && asset->name() == renamed)
			{
				return asset.get();
			}
		}

		return nullptr;
	}

	void* zone_interface::get_asset_pointer(const std::int32_t type, const std::string& name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		const auto renamed = renamed_asset_name(name);
		const auto full_name = renamed.starts_with(',') ? renamed.substr(1) : renamed;
		const auto reference_name = "," + full_name;

		std::optional<std::size_t> index;
		for (std::size_t i = 0; i < this->m_assets.size(); i++)
		{
			const auto& asset = this->m_assets[i];
			if (asset->type() == type && (asset->name() == full_name || asset->name() == reference_name) && (!index || i < this->m_write_index))
			{
				index = i;
			}
		}

		if (!index)
		{
			return nullptr;
		}

		const auto offset = this->m_assetbase + sizeof(XAsset) * *index + offsetof(XAsset, header) + 1;
		return reinterpret_cast<void*>((static_cast<std::uint64_t>(XFILE_BLOCK_VIRTUAL) << 32) + static_cast<std::uint32_t>(offset));
	}

	void zone_interface::add_asset_of_type_by_pointer(const std::int32_t type, void* pointer)
	{
		if (!pointer)
		{
			return;
		}

		const std::string name = get_asset_name(static_cast<XAssetType>(type), pointer);
		if (this->find_asset(type, name))
		{
			return;
		}

		ZONETOOL_WARNING("Asset type %s cannot be added by pointer (\"%s\")", type_to_string(static_cast<XAssetType>(type)), name.data());
	}

	void zone_interface::add_asset_of_type(const std::int32_t type, const std::string& name)
	{
		const auto asset_name = resolve_asset_name(type, name);
		if (!asset_name.empty() && !this->get_asset_pointer(type, asset_name))
		{
			this->create_asset(type, asset_name);
		}
	}

	void zone_interface::add_listed_asset(const std::int32_t type, const std::string& name)
	{
		const auto asset_name = resolve_asset_name(type, name);
		if (asset_name.empty())
		{
			return;
		}

		const auto exists = asset_name.starts_with(',') ? this->get_asset_pointer(type, asset_name) != nullptr : this->find_asset(type, asset_name) != nullptr;
		if (!exists)
		{
			this->create_asset(type, asset_name);
		}
	}

	void zone_interface::push_preload() // DB_PushPreloadProcessing (0x11B2540)
	{
		if (this->m_preload_depth++ == 0)
		{
			for (std::uint8_t block = 0; block < this->m_preload_positions.size(); block++)
			{
				this->m_preload_positions[block] = this->m_buffer->stream_position(block);
			}
		}
	}

	void zone_interface::pop_preload() // DB_PopPreloadProcessing (0x11B24B0)
	{
		if (--this->m_preload_depth != 0)
		{
			return;
		}

		for (std::uint8_t block = 0; block < this->m_preload_positions.size(); block++)
		{
			if (this->m_buffer->stream_position(block) != this->m_preload_positions[block])
			{
				this->m_preload_walk_size += sizeof(std::uint8_t) + sizeof(std::uint32_t);
			}
		}

		this->m_preload_walk_size += sizeof(std::uint8_t);
	}

	void zone_interface::preload_asset_pointer(const std::int32_t type)
	{
		if (this->m_buffer && opens_preload_region(type))
		{
			this->push_preload();
			this->pop_preload();
		}
	}

	void zone_interface::create_asset(const std::int32_t type, const std::string& name)
	{
#define ADD_ASSET(__type__, __class__, __struct__) \
		if (type == __type__) \
		{ \
			auto asset = std::make_shared<__class__>(); \
			asset->init(name, this->m_zonemem.get()); \
			if (!asset->pointer() && !asset->referenced()) \
			{ \
				ZONETOOL_WARNING("%s \"%s\" not found, adding it as a reference", type_to_string(static_cast<XAssetType>(type)), name.data()); \
				asset->init(","s + name, this->m_zonemem.get()); \
			} \
			asset->load_depending(this); \
			this->m_assets.push_back(asset); \
			return; \
		}

		try
		{
			S4_ASSETS(ADD_ASSET)
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(static_cast<XAssetType>(type)), ex.what());
		}
#undef ADD_ASSET

		if (!get_asset_loader(static_cast<XAssetType>(type)).size)
		{
			ZONETOOL_WARNING("Asset type %s is not supported for building (\"%s\")",
				type_to_string(static_cast<XAssetType>(type)), name.data());
			return;
		}

		ZONETOOL_WARNING("Asset type %s has no writer, adding \"%s\" as a reference", type_to_string(static_cast<XAssetType>(type)), name.data());
		this->m_assets.push_back(std::make_shared<asset_reference>(type, name.starts_with(',') ? name : ","s + name));
	}

	std::int32_t zone_interface::get_type_by_name(const std::string& type)
	{
		return type_to_int(type);
	}

	void zone_interface::add_asset_of_type(const std::string& type, const std::string& name)
	{
		this->add_asset_of_type(type_to_int(type), name);
	}

	void zone_interface::build(zone_buffer* buf)
	{
		const auto start_time = GetTickCount64();
		const auto& format = definition().format;

		buf->init_streams(format.block_count);
		buf->set_temp_stream(XFILE_BLOCK_TEMP);

		ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

		for (const auto& asset : this->m_assets)
		{
			asset->prepare(buf, this->m_zonemem.get());
		}

		this->m_buffer = buf;
		this->m_preload_walk_size = 0;

		buf->push_stream(XFILE_BLOCK_TEMP);
		write_asset_list(buf, this->m_assets.size());

		this->push_preload();
		write_script_strings(buf);
		this->pop_preload();

		std::size_t preload_offset = align_offset(sizeof(XAssetList), 31);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		buf->align(7);
		this->m_assetbase = buf->stream_offset(XFILE_BLOCK_VIRTUAL);

		for (const auto& asset : this->m_assets)
		{
			XAsset entry{};
			entry.type = static_cast<XAssetType>(asset->type());
			entry.header.data = reinterpret_cast<void*>(buf->data_following);
			buf->write(&entry);
		}

		for (this->m_write_index = 0; this->m_write_index < this->m_assets.size(); this->m_write_index++)
		{
			const auto& asset = this->m_assets[this->m_write_index];
			const auto preload_region = opens_preload_region(asset->type());
			if (preload_region)
			{
				this->push_preload();
			}

			buf->push_stream(XFILE_BLOCK_TEMP);
			buf->align(7);
			const auto alignment = asset_alignment(static_cast<XAssetType>(asset->type()));
			buf->align(alignment);
			const auto asset_start = buf->current_stream_offset();
			asset->write(this, buf);
			preload_offset = align_offset(align_offset(preload_offset, alignment) + buf->current_stream_offset() - asset_start, 31);
			buf->pop_stream();

			if (preload_region)
			{
				this->pop_preload();
			}
		}
		buf->pop_stream();
		buf->pop_stream();
		this->m_buffer = nullptr;

		xfile::fastfile file{};
		file.s4_header.preload_walk_size = this->m_preload_walk_size;
		file.is_compressed = !utils::flags::has_flag("uncompressed");
		file.block_sizes.resize(format.block_count);
		for (std::size_t index = 0; index < format.block_count; index++)
		{
			file.block_sizes[index] = buf->stream_offset(static_cast<std::uint8_t>(index));
		}
		file.block_sizes[XFILE_BLOCK_TEMP_PRELOAD] = preload_offset;

		const auto path = filesystem::get_output_path() + this->name_ + ".ff";
		xfile::write_iw8(path, format, file, std::string_view(reinterpret_cast<const char*>(buf->buffer()), buf->size()));

		ZONETOOL_INFO("Fastfile \"%s\" written to %s", this->name_.data(), path.data());
		ZONETOOL_INFO("Compiling took %llu msec.", GetTickCount64() - start_time);
	}

	zone_interface::zone_interface(std::string name)
		: m_assetbase(0)
		, m_write_index(std::numeric_limits<std::size_t>::max())
		, name_(std::move(name))
		, m_zonemem(std::make_shared<zone_memory>(utils::flags::has_flag("more_memory") ? max_zone_size * 2 : max_zone_size))
	{
	}

	zone_interface::~zone_interface()
	{
		this->m_assets.clear();
	}
}
