#include <std_include.hpp>
#include "s2.hpp"
#include "zone.hpp"
#include "loader.hpp"

#include <game.hpp>

namespace zonetool::s2
{
	const game& definition();

	namespace
	{
		bool allows_empty_name(const std::int32_t type)
		{
			// no clue why but common.ff has empty tables
			return type == ASSET_TYPE_IMPACT_FX || type == ASSET_TYPE_SURFACE_FX;
		}

		template <typename T>
		void write_state_array(zone_buffer* buf, T* data, const unsigned int count, T*& field)
		{
			if (!data)
			{
				return;
			}

			buf->align(3);
			buf->write(data, count);
			buf->clear_pointer(&field);
		}

		void write_zone_table(zone_buffer* buf, GfxZoneTableEntry* table, const unsigned int count, GfxZoneTableEntry*& field)
		{
			if (!table)
			{
				return;
			}

			buf->push_stream(XFILE_BLOCK_RUNTIME);
			buf->align(7);
			buf->write(table, count);
			buf->pop_stream();
			buf->clear_pointer(&field);
		}

		void write_script_strings(zone_buffer* buf)
		{
			const auto count = buf->scriptstring_count();
			if (!count)
			{
				return;
			}

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
		}

		// the game fills XFILE_BLOCK_CALLBACK after the zone is read (0x4D5EC0)
		std::uint64_t get_callback_block_size(const auto& assets)
		{
			std::uint64_t size = 0;
			const auto allocate = [&](const std::uint64_t bytes, const std::uint64_t alignment)
			{
				size = ((size + alignment - 1) & ~(alignment - 1)) + bytes;
			};

			for (const auto& asset : assets)
			{
				if (asset->referenced() || !asset->pointer())
				{
					continue;
				}

				switch (asset->type())
				{
				case ASSET_TYPE_MATERIAL:
				{
					// image index per texture (0x4DA970)
					const auto* material = static_cast<const Material*>(asset->pointer());
					if (material->textureCount)
					{
						allocate(4ull * material->textureCount, 4);
					}
					break;
				}
				case ASSET_TYPE_XMODEL_BASE:
				{
					// index per lod surface (using count at 0x26C) (0x4DAAA0)
					const auto* base = static_cast<const XModelBase*>(asset->pointer());
					if (!base->numLods)
					{
						break;
					}

					std::uint64_t count = *reinterpret_cast<const std::uint16_t*>(
						reinterpret_cast<const std::uint8_t*>(base) + 0x26C);
					std::uint64_t surf_count = 0;
					for (auto i = 0; i < base->numLods && i < 6; i++)
					{
						surf_count += base->lodInfo[i].numsurfs;
					}

					allocate(2ull * std::max(count, surf_count), 2);
					break;
				}
				case ASSET_TYPE_XMODEL:
				{
					// index per material for models that are not variants (0x4DAC30)
					const auto* model = static_cast<const XModel*>(asset->pointer());
					if (!(model->flags & 0x200) && model->numMaterials)
					{
						allocate(4ull * model->numMaterials, 2);
					}
					break;
				}
				default:
					break;
				}
			}

			return size ? size + 3 : 0;
		}

		bool has_gfx_globals(zone_buffer* buf)
		{
			return buf->depthstencilstatebit_count() + buf->blendstatebits_count() + buf->ppas_count()
				+ buf->poas_count() + buf->vas_count() + buf->sas_count() > 0;
		}

		void write_gfx_globals(zone_buffer* buf, zone_memory* memory)
		{
			const auto globals = memory->allocate<XGfxGlobals>();

			globals->depthStencilStateCount = static_cast<unsigned int>(buf->depthstencilstatebit_count());
			globals->depthStencilStateBits = memory->allocate<std::uint64_t>(globals->depthStencilStateCount);
			globals->depthStencilStates = memory->allocate<GfxZoneTableEntry>(globals->depthStencilStateCount);
			for (auto i = 0u; i < globals->depthStencilStateCount; i++)
			{
				globals->depthStencilStateBits[i] = buf->get_depthstencilstatebit(i);
			}

			globals->blendStateCount = static_cast<unsigned int>(buf->blendstatebits_count());
			globals->blendStateBits = memory->allocate<GfxBlendStateBits>(globals->blendStateCount);
			globals->blendStates = memory->allocate<GfxZoneTableEntry>(globals->blendStateCount);
			for (auto i = 0u; i < globals->blendStateCount; i++)
			{
				const auto bits = buf->get_blendstatebits(i);
				std::copy_n(bits.begin(), 3, globals->blendStateBits[i]);
			}

			globals->perPrimConstantBufferCount = static_cast<unsigned int>(buf->ppas_count());
			globals->perPrimConstantBufferSizes = memory->allocate<unsigned int>(globals->perPrimConstantBufferCount);
			globals->perPrimConstantBuffers = memory->allocate<GfxZoneTableEntry>(globals->perPrimConstantBufferCount);
			for (auto i = 0u; i < globals->perPrimConstantBufferCount; i++)
			{
				globals->perPrimConstantBufferSizes[i] = buf->get_ppas(i);
			}

			globals->perObjConstantBufferCount = static_cast<unsigned int>(buf->poas_count());
			globals->perObjConstantBufferSizes = memory->allocate<unsigned int>(globals->perObjConstantBufferCount);
			globals->perObjConstantBuffers = memory->allocate<GfxZoneTableEntry>(globals->perObjConstantBufferCount);
			for (auto i = 0u; i < globals->perObjConstantBufferCount; i++)
			{
				globals->perObjConstantBufferSizes[i] = buf->get_poas(i);
			}

			globals->volatileConstantBufferCount = static_cast<unsigned int>(buf->vas_count());
			globals->volatileConstantBufferSizes = memory->allocate<unsigned int>(globals->volatileConstantBufferCount);
			globals->volatileConstantBuffers = memory->allocate<GfxZoneTableEntry>(globals->volatileConstantBufferCount);
			for (auto i = 0u; i < globals->volatileConstantBufferCount; i++)
			{
				globals->volatileConstantBufferSizes[i] = buf->get_vas(i);
			}

			globals->stableConstantBufferCount = static_cast<unsigned int>(buf->sas_count());
			globals->stableConstantBufferSizes = memory->allocate<unsigned int>(globals->stableConstantBufferCount);
			globals->stableConstantBuffers = memory->allocate<GfxZoneTableEntry>(globals->stableConstantBufferCount);
			for (auto i = 0u; i < globals->stableConstantBufferCount; i++)
			{
				globals->stableConstantBufferSizes[i] = buf->get_sas(i);
			}

			buf->align(7);
			buf->write<std::uintptr_t>(&buf->data_following);

			buf->align(7);
			const auto dest = buf->write(globals);

			write_state_array(buf, globals->depthStencilStateBits, globals->depthStencilStateCount, dest->depthStencilStateBits);
			write_state_array(buf, globals->blendStateBits, globals->blendStateCount, dest->blendStateBits);
			write_zone_table(buf, globals->depthStencilStates, globals->depthStencilStateCount, dest->depthStencilStates);
			write_zone_table(buf, globals->blendStates, globals->blendStateCount, dest->blendStates);
			write_state_array(buf, globals->perPrimConstantBufferSizes, globals->perPrimConstantBufferCount, dest->perPrimConstantBufferSizes);
			write_state_array(buf, globals->perObjConstantBufferSizes, globals->perObjConstantBufferCount, dest->perObjConstantBufferSizes);
			write_state_array(buf, globals->volatileConstantBufferSizes, globals->volatileConstantBufferCount, dest->volatileConstantBufferSizes);
			write_state_array(buf, globals->stableConstantBufferSizes, globals->stableConstantBufferCount, dest->stableConstantBufferSizes);
			write_zone_table(buf, globals->perPrimConstantBuffers, globals->perPrimConstantBufferCount, dest->perPrimConstantBuffers);
			write_zone_table(buf, globals->perObjConstantBuffers, globals->perObjConstantBufferCount, dest->perObjConstantBuffers);
			write_zone_table(buf, globals->volatileConstantBuffers, globals->volatileConstantBufferCount, dest->volatileConstantBuffers);
			write_zone_table(buf, globals->stableConstantBuffers, globals->stableConstantBufferCount, dest->stableConstantBuffers);
		}
	}

	asset_interface* zone_interface::find_asset(const std::int32_t type, const std::string& name)
	{
		if (name.empty() && !allows_empty_name(type))
		{
			return nullptr;
		}

		for (const auto& asset : this->m_assets)
		{
			if (asset->type() == type && asset->name() == name)
			{
				return asset.get();
			}
		}

		return nullptr;
	}

	void* zone_interface::get_asset_pointer(const std::int32_t type, const std::string& name)
	{
		if (name.empty() && !allows_empty_name(type))
		{
			return nullptr;
		}

		const auto other_name = name.starts_with(',') ? name.substr(1) : "," + name;
		for (std::size_t index = 0; index < this->m_assets.size(); index++)
		{
			const auto& asset = this->m_assets[index];
			if (asset->type() == type && (asset->name() == name || asset->name() == other_name))
			{
				const auto offset = this->m_assetbase + 16 * index + 8 + 1;
				return reinterpret_cast<void*>((static_cast<std::uint64_t>(XFILE_BLOCK_VIRTUAL) << 32) + static_cast<std::uint32_t>(offset));
			}
		}

		return nullptr;
	}

	void zone_interface::add_asset_of_type_by_pointer(const std::int32_t type, void* pointer)
	{
		if (!pointer)
		{
			return;
		}

		const std::string name = type == ASSET_TYPE_XSURFACE_SHARED
			? surface_shared_key(static_cast<XSurfaceShared*>(pointer))
			: get_asset_name(static_cast<XAssetType>(type), pointer);
		if (this->find_asset(type, name))
		{
			return;
		}

		try
		{
			if (type == ASSET_TYPE_XSURFACE_SHARED)
			{
				auto asset = std::make_shared<xsurface_shared>();
				asset->init(pointer, this->m_zonemem.get());
				this->m_assets.push_back(asset);
			}
			else if (type == ASSET_TYPE_LOCALIZE_ENTRY)
			{
				auto asset = std::make_shared<localize>();
				asset->init(pointer, this->m_zonemem.get());
				asset->load_depending(this);
				this->m_assets.push_back(asset);
			}
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(static_cast<XAssetType>(type)), ex.what());
		}
	}

	void zone_interface::add_asset_of_type(const std::int32_t type, const std::string& input_name)
	{
		auto name = input_name;
		if (name.empty() && !allows_empty_name(type))
		{
			return;
		}

		if (ignore_assets.contains({static_cast<std::uint32_t>(type), name}) && !name.starts_with(','))
		{
			name = ","s + name;
		}

		if (this->get_asset_pointer(type, name))
		{
			return;
		}

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
			S2_ASSETS(ADD_ASSET)
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(static_cast<XAssetType>(type)), ex.what());
		}
#undef ADD_ASSET

		ZONETOOL_WARNING("Asset type %s is not supported for building (\"%s\")",
			type_to_string(static_cast<XAssetType>(type)), name.data());
	}

	std::int32_t zone_interface::get_type_by_name(const std::string& type)
	{
		return type_to_int(type);
	}

	void zone_interface::add_asset_of_type(const std::string& type, const std::string& name)
	{
		this->add_asset_of_type(type_to_int(type), name);
	}

	std::vector<XStreamFileKey> zone_interface::collect_image_stream_keys(std::vector<image_pak::image_streams>& images) const
	{
		std::vector<XStreamFileKey> keys;
		for (const auto& asset : this->m_assets)
		{
			if (asset->type() != ASSET_TYPE_IMAGE || asset->referenced())
			{
				continue;
			}

			const auto* header = static_cast<const GfxImage*>(asset->pointer());
			if (!header->streamed)
			{
				continue;
			}

			const auto* image = static_cast<const gfx_image*>(asset.get());
			const auto image_keys = image->stream_keys();
			if (image_keys.size() != image_stream_count)
			{
				throw std::runtime_error(std::format("streamed image \"{}\" has no stream keys", asset->name()));
			}

			const auto& pixel_paths = image->stream_pixel_paths();
			if (std::ranges::any_of(pixel_paths, [](const auto& path) { return path.has_value(); }))
			{
				auto& streams = images.emplace_back(keys.size());
				for (auto i = 0u; i < image_stream_count; i++)
				{
					if (pixel_paths[i])
					{
						streams.pixels[i] = *pixel_paths[i];
					}
				}
			}

			keys.insert(keys.end(), image_keys.begin(), image_keys.end());
		}

		return keys;
	}

	void zone_interface::build(zone_buffer* buf)
	{
		const auto start_time = GetTickCount64();
		const auto& format = definition().format;

		buf->init_streams(format.block_count);
		buf->set_temp_stream(XFILE_BLOCK_TEMP);
		buf->set_unread_stream(XFILE_BLOCK_RUNTIME_UNREAD);

		ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

		for (const auto& asset : this->m_assets)
		{
			asset->prepare(buf, this->m_zonemem.get());
		}

		std::uintptr_t none{};
		const auto string_count = static_cast<std::uintptr_t>(buf->scriptstring_count());
		const auto asset_count = static_cast<std::uintptr_t>(this->m_assets.size());
		const auto write_globals = has_gfx_globals(buf);

		buf->push_stream(XFILE_BLOCK_TEMP);
		buf->write(&string_count);
		buf->write(string_count ? &buf->data_following : &none);
		buf->write(&asset_count);
		buf->write(asset_count ? &buf->data_following : &none);
		buf->write(write_globals ? &buf->data_following : &none);
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		write_script_strings(buf);
		if (write_globals)
		{
			write_gfx_globals(buf, this->m_zonemem.get());
		}
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		buf->align(7);
		this->m_assetbase = buf->stream_offset(XFILE_BLOCK_VIRTUAL);

		for (const auto& asset : this->m_assets)
		{
			const auto type = static_cast<std::uintptr_t>(asset->type());
			buf->write(&type);
			buf->write(&buf->data_following);
		}

		for (const auto& asset : this->m_assets)
		{
			buf->push_stream(XFILE_BLOCK_TEMP);
			buf->align(7);
			asset->write(this, buf);
			buf->pop_stream();
		}
		buf->pop_stream();

		std::vector<image_pak::image_streams> images;
		xfile::fastfile file{};
		file.version = format.version;
		file.stream_keys = this->collect_image_stream_keys(images);
		file.shared_stream_keys = buf->shared_stream_keys();
		file.block_sizes.resize(format.block_count);
		for (std::size_t index = 0; index < format.block_count; index++)
		{
			file.block_sizes[index] = buf->stream_offset(static_cast<std::uint8_t>(index));
		}

		if (XFILE_BLOCK_CALLBACK < file.block_sizes.size())
		{
			file.block_sizes[XFILE_BLOCK_CALLBACK] += get_callback_block_size(this->m_assets);
		}

		const auto folder = filesystem::get_output_path();
		image_pak::write(this->name_, folder, images, file.stream_keys);

		const auto path = folder + this->name_ + ".ff";
		xfile::write_s2(path, format, file, std::string_view(reinterpret_cast<const char*>(buf->buffer()), buf->size()));

		ZONETOOL_INFO("Fastfile \"%s\" written to %s", this->name_.data(), path.data());
		ZONETOOL_INFO("Compiling took %llu msec.", GetTickCount64() - start_time);
	}

	zone_interface::zone_interface(std::string name)
		: m_assetbase(0)
		, name_(std::move(name))
		, m_zonemem(std::make_shared<zone_memory>(max_zone_size))
	{
	}

	zone_interface::~zone_interface()
	{
		this->m_assets.clear();
	}
}
