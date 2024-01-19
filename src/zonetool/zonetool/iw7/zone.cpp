#include <std_include.hpp>
#include "zonetool.hpp"
#include "zone.hpp"
#include "zonetool/utils/utils.hpp"
#include "zonetool/utils/imagefile.hpp"

#include <utils/io.hpp>

//#define FF_SIGNED
#ifdef FF_SIGNED
#define FF_HEADER "IWff0100"
#else
#define FF_HEADER "IWffu100"
#endif

#define FF_VERSION 1619

#define CUSTOM_IMAGEFILE_INDEX 230

#define COMPRESS_TYPE_LZ4 4
#define COMPRESS_TYPE_ZLIB 1
#define COMPRESS_TYPE_NONE 0

#define COMPRESS_TYPE COMPRESS_TYPE_ZLIB
static_assert(COMPRESS_TYPE == COMPRESS_TYPE_LZ4 || COMPRESS_TYPE == COMPRESS_TYPE_ZLIB || COMPRESS_TYPE == COMPRESS_TYPE_NONE);

namespace zonetool::iw7
{
	asset_interface* zone_interface::find_asset(std::int32_t type, const std::string& name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		for (std::size_t idx = 0; idx < m_assets.size(); idx++)
		{
			if (m_assets[idx]->type() == type && m_assets[idx]->name() == name)
			{
				return m_assets[idx].get();
			}
		}

		return nullptr;
	}

	void* zone_interface::get_asset_pointer(std::int32_t type, const std::string& name)
	{
		if (name.empty())
		{
			return nullptr;
		}

		std::string ref_name = "," + name;
		if (name.starts_with(","))
		{
			ref_name = name;
			ref_name.erase(0, 1);
		}

		for (std::size_t idx = 0; idx < m_assets.size(); idx++)
		{
			if (m_assets[idx]->type() == type && (m_assets[idx]->name() == name || m_assets[idx]->name() == ref_name))
			{
				auto ptr = reinterpret_cast<void*>(0xFDFDFDF300000000 + (this->m_assetbase + ((16 * idx) + 8) + 1));
				return ptr;
			}
		}

		return nullptr;
	}

	void zone_interface::add_asset_of_type_by_pointer(std::int32_t type, void* pointer)
	{
		if (!pointer)
		{
			return;
		}

		const std::string& name = get_asset_name(XAssetType(type), pointer);

		// don't add asset if it already exists
		for (std::size_t idx = 0; idx < m_assets.size(); idx++)
		{
			if (m_assets[idx]->type() == type && m_assets[idx]->name() == name)
			{
				return;
			}
		}

#define ADD_ASSET_PTR(__type__, ___) \
		if (type == __type__) \
		{ \
			auto asset = std::make_shared < ___ >(); \
			asset->init(pointer, this->m_zonemem.get()); \
			asset->load_depending(this); \
			m_assets.push_back(asset); \
		}

		try
		{
			// declare asset interfaces
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(XAssetType(type)), ex.what());
		}
	}

	void zone_interface::add_asset_of_type(std::int32_t type, const std::string& name)
	{
		if (name.empty())
		{
			return;
		}

		// don't add asset if it already exists
		if (get_asset_pointer(type, name))
		{
			return;
		}

		// if common asset: add as referenced?
		{

		}

#define ADD_ASSET(__type__, ___) \
		if (type == __type__) \
		{ \
			auto asset = std::make_shared < ___ >(); \
			asset->init(name, this->m_zonemem.get()); \
			asset->load_depending(this); \
			m_assets.push_back(asset); \
		}

		try
		{
			// declare asset interfaces
			ADD_ASSET(ASSET_TYPE_RAWFILE, rawfile);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(XAssetType(type)), ex.what());
		}
	}

	std::int32_t zone_interface::get_type_by_name(const std::string& type)
	{
		return type_to_int(type);
	}

	void zone_interface::add_asset_of_type(const std::string& type, const std::string& name)
	{
		std::int32_t itype = type_to_int(type);
		this->add_asset_of_type(itype, name);
	}

	void zone_interface::build(zone_buffer* buf)
	{
		buf->init_streams(MAX_XFILE_COUNT);

		[[maybe_unused]] const auto start_time = GetTickCount64();

		ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

		/* {
			std::vector<gfx_image*> images;
			for (std::size_t i = 0; i < m_assets.size(); i++)
			{
				if (m_assets[i]->type() == ASSET_TYPE_IMAGE)
				{
					const auto image = static_cast<gfx_image*>(m_assets[i].get());
					if (image->custom_streamed_image)
					{
						images.emplace_back(image);
					}
				}
			}

			if (images.size() > 0)
			{
				imagefile::generate(filesystem::get_fastfile(),
					CUSTOM_IMAGEFILE_INDEX, FF_VERSION, FF_HEADER, images, this->m_zonemem.get());
			}
		}*/

		std::uintptr_t following = static_cast<std::uintptr_t>(buf->data_following);
		std::uintptr_t zero = 0; // data_none

		// write asset types to header
		for (std::size_t i = 0; i < m_assets.size(); i++)
		{
			m_assets[i]->prepare(buf, this->m_zonemem.get());
		}

		// write scriptstring count
		std::size_t stringcount = buf->scriptstring_count();
		buf->write<std::uintptr_t>(&stringcount);
		buf->write<std::uintptr_t>(stringcount > 0 ? (&following) : (&zero)); // pointer to scriptstrings

		// write asset count
		std::size_t asset_count = m_assets.size();
		buf->write<std::uintptr_t>(&asset_count);
		buf->write<std::uintptr_t>(asset_count > 0 ? (&following) : (&zero)); // pointer to assets

		bool write_globals = (buf->depthstencilstatebit_count() + buf->blendstatebits_count()) > 0;
		buf->write<std::uintptr_t>(write_globals ? (&following) : (&zero)); // pointer to globals

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		// write scriptstrings
		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		if (stringcount)
		{
			// write pointer for every scriptstring
			for (std::size_t idx = 0; idx < stringcount; idx++)
			{
				const auto str = buf->get_scriptstring(idx);
				if (str == nullptr)
				{
					buf->write<std::uintptr_t>(&zero);
				}
				else
				{
					buf->write<std::uintptr_t>(&following);
				}
			}

			// write scriptstrings
			for (std::size_t idx = 0; idx < stringcount; idx++)
			{
				buf->align(7);
				const auto str = buf->get_scriptstring(idx);
				if (str != nullptr)
				{
					buf->write_str(str);
				}
			}
		}
		buf->pop_stream();

		// write globals
		if (write_globals)
		{
			buf->align(7);
			buf->write<std::uintptr_t>(&following); // pointer to gfxglobals

			// parse gfxglobals
			auto mem_ = this->m_zonemem.get();
			auto globals = mem_->allocate<XGfxGlobals>();

			globals->depthStencilStateCount = static_cast<unsigned int>(buf->depthstencilstatebit_count());
			globals->depthStencilStateBits = mem_->allocate<std::uint64_t>(globals->depthStencilStateCount);
			globals->depthStencilStates = mem_->allocate<GfxZoneTableEntry>(globals->depthStencilStateCount);

			for (unsigned int i = 0; i < globals->depthStencilStateCount; i++)
			{
				globals->depthStencilStateBits[i] = buf->get_depthstencilstatebit(i);
			}

			globals->blendStateCount = static_cast<unsigned int>(buf->blendstatebits_count());
			globals->blendStateBits = mem_->allocate<GfxBlendStateBits>(globals->blendStateCount);
			globals->blendStates = mem_->allocate<GfxZoneTableEntry>(globals->blendStateCount);

			for (unsigned int i = 0; i < globals->blendStateCount; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					globals->blendStateBits[i][j] = buf->get_blendstatebits(i)[j];
				}
			}
			
			globals->perPrimConstantBufferCount = static_cast<unsigned int>(buf->ppas_count());
			globals->perPrimConstantBufferSizes = mem_->allocate<unsigned int>(globals->perPrimConstantBufferCount);
			globals->perPrimConstantBuffers = mem_->allocate<GfxZoneTableEntry>(globals->perPrimConstantBufferCount);

			for (unsigned int i = 0; i < globals->perPrimConstantBufferCount; i++)
			{
				globals->perPrimConstantBufferSizes[i] = buf->get_ppas(i);
			}

			globals->perObjConstantBufferCount = static_cast<unsigned int>(buf->poas_count());
			globals->perObjConstantBufferSizes = mem_->allocate<unsigned int>(globals->perObjConstantBufferCount);
			globals->perObjConstantBuffers = mem_->allocate<GfxZoneTableEntry>(globals->perObjConstantBufferCount);

			for (unsigned int i = 0; i < globals->perObjConstantBufferCount; i++)
			{
				globals->perObjConstantBufferSizes[i] = buf->get_poas(i);
			}

			globals->stableConstantBufferCount = static_cast<unsigned int>(buf->sas_count());
			globals->stableConstantBufferSizes = mem_->allocate<unsigned int>(globals->stableConstantBufferCount);
			globals->stableConstantBuffers = mem_->allocate<GfxZoneTableEntry>(globals->stableConstantBufferCount);

			for (unsigned int i = 0; i < globals->stableConstantBufferCount; i++)
			{
				globals->stableConstantBufferSizes[i] = buf->get_sas(i);
			}

			// write gfxglobals
			auto dest = buf->write(globals);
			if (globals->depthStencilStateBits)
			{
				buf->align(3);
				buf->write(globals->depthStencilStateBits, globals->depthStencilStateCount);
				buf->clear_pointer(&dest->depthStencilStateBits);
			}
			if (globals->blendStateBits)
			{
				buf->align(3);
				buf->write(globals->blendStateBits, globals->blendStateCount);
				buf->clear_pointer(&dest->blendStateBits);
			}
			buf->push_stream(XFILE_BLOCK_RUNTIME);
			if (globals->depthStencilStates)
			{
				buf->align(7);
				auto dest_0 = buf->write(globals->depthStencilStates, globals->depthStencilStateCount);
				for (unsigned int i = 0; i < globals->depthStencilStateCount; i++)
				{
					if (globals->depthStencilStates[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->depthStencilStates[i].dataPtr, 1);
						buf->clear_pointer(&dest_0->dataPtr);
					}
				}
				buf->clear_pointer(&dest->depthStencilStates);
			}
			buf->pop_stream();
			buf->push_stream(XFILE_BLOCK_RUNTIME);
			if (globals->blendStates)
			{
				buf->align(7);
				auto dest_0 = buf->write(globals->blendStates, globals->blendStateCount);
				for (unsigned int i = 0; i < globals->blendStateCount; i++)
				{
					if (globals->blendStates[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->blendStates[i].dataPtr, 1);
						buf->clear_pointer(&dest_0->dataPtr);
					}
				}
				buf->clear_pointer(&dest->blendStates);
			}
			buf->pop_stream();
			if (globals->perPrimConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perPrimConstantBufferSizes, globals->perPrimConstantBufferCount);
				buf->clear_pointer(&dest->perPrimConstantBufferSizes);
			}
			if (globals->perObjConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perObjConstantBufferSizes, globals->perObjConstantBufferCount);
				buf->clear_pointer(&dest->perObjConstantBufferSizes);
			}
			if (globals->stableConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->stableConstantBufferSizes, globals->stableConstantBufferCount);
				buf->clear_pointer(&dest->stableConstantBufferSizes);
			}
			buf->push_stream(XFILE_BLOCK_RUNTIME);
			if (globals->perPrimConstantBuffers)
			{
				buf->align(7);
				auto dest_0 = buf->write(globals->perPrimConstantBuffers, globals->perPrimConstantBufferCount);
				for (unsigned int i = 0; i < globals->perPrimConstantBufferCount; i++)
				{
					if (globals->perPrimConstantBuffers[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->perPrimConstantBuffers[i].dataPtr, 1);
						buf->clear_pointer(&dest_0->dataPtr);
					}
				}
				buf->clear_pointer(&dest->perPrimConstantBuffers);
			}
			buf->pop_stream();
			buf->push_stream(XFILE_BLOCK_RUNTIME);
			if (globals->perObjConstantBuffers)
			{
				buf->align(3);
				auto dest_0 = buf->write(globals->perObjConstantBuffers, globals->perObjConstantBufferCount);
				for (unsigned int i = 0; i < globals->perObjConstantBufferCount; i++)
				{
					if (globals->perObjConstantBuffers[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->perObjConstantBuffers[i].dataPtr, 1);
						buf->clear_pointer(&dest_0->dataPtr);
					}
				}
				buf->clear_pointer(&dest->perObjConstantBuffers);
			}
			buf->pop_stream();
			buf->push_stream(XFILE_BLOCK_RUNTIME);
			if (globals->stableConstantBuffers)
			{
				buf->align(7);
				auto dest_0 = buf->write(globals->stableConstantBuffers, globals->stableConstantBufferCount);
				for (unsigned int i = 0; i < globals->stableConstantBufferCount; i++)
				{
					if (globals->stableConstantBuffers[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->stableConstantBuffers[i].dataPtr, 1);
						buf->clear_pointer(&dest_0->dataPtr);
					}
				}
				buf->clear_pointer(&dest->stableConstantBuffers);
			}
			buf->pop_stream();
		}
		else
		{
			buf->align(7);
			buf->write<std::uintptr_t>(&zero); // pointer to gfxglobals
		}

		buf->pop_stream();
		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		// align buffer
		buf->align(7);

		// set asset ptr base
		this->m_assetbase = buf->stream_offset(XFILE_BLOCK_VIRTUAL);
		ZONETOOL_INFO("m_assetbase: %llu", this->m_assetbase);

		// write asset types to header
		for (std::size_t i = 0; i < asset_count; i++)
		{
			// write asset data to zone
			auto type = static_cast<uintptr_t>(m_assets[i]->type());

			buf->write(&type);
			buf->write(&following);
		}

		// write assets
		for (auto& asset : m_assets)
		{
#ifdef DEBUG
			ZONETOOL_INFO("writing asset \"%s\" of type %s...", asset->name().data(), type_to_string(XAssetType(asset->type())));
#endif

			// push stream
			buf->push_stream(XFILE_BLOCK_TEMP);
			buf->align(7);

			// write asset
			asset->write(this, buf);

			// pop stream
			buf->pop_stream();
		}

		// pop stream
		buf->pop_stream();
		
#ifdef DEBUG
		// Dump zone to disk (for debugging)
		buf->save("zonetool\\_debug\\" + this->name_ + ".zone", false);
#endif

		// Compress buffer
#if (COMPRESS_TYPE == COMPRESS_TYPE_LZ4)
		const auto buf_compressed = buf->compress_lz4();
		const auto buf_output = buf_compressed.data();
		const auto buf_output_size = buf_compressed.size();
#elif (COMPRESS_TYPE == COMPRESS_TYPE_ZLIB)
		const auto buf_compressed = buf->compress_zlib();
		const auto buf_output = buf_compressed.data();
		const auto buf_output_size = buf_compressed.size();
#else
		const auto buf_output = buf->buffer();
		const auto buf_output_size = buf->size();
#endif

		// Generate FF header
		auto header = this->m_zonemem->allocate<XFileHeader>();
		strcat(header->header, FF_HEADER);
		header->version = FF_VERSION;
		header->unused = 0;
		header->has_no_image_fastfile = 1;
		header->has_no_shared_fastfile = 1;
		header->unk1 = 1;
		header->fileTimeLow = 0;
		header->fileTimeHigh = 0;
		header->shared_ff_hash = 0;
		header->shared_ff_count = 0;
		header->image_ff_hash = 0;
		header->image_ff_count = 0;
		header->fileLen = buf_output_size + sizeof(XFileHeader);
		header->fileLenUnk1 = 0; // imagefile?
		header->fileLenUnk2 = 0; // shared?

		{
			header->stream_data.unk1 = 0;
			header->stream_data.unk2 = 0;

			std::uint64_t total_block_size = 0;
			for (auto i = 0; i < MAX_XFILE_COUNT; i++)
			{
				header->stream_data.block_size[i] = buf->stream_offset(static_cast<std::uint8_t>(i));
				total_block_size += header->stream_data.block_size[i];
			}
			header->stream_data.size = total_block_size; // not correct
			memset(header->stream_data.unk_arr, 0, sizeof(header->stream_data.unk_arr));
		}

#ifdef FF_SIGNED
		header->fileLen += sizeof(XFileSignedInfo);

		XFileSignedInfo secret{};
		strcat(secret.magic, "IWffs100");
		secret.unk = 0;
		memset(secret.key, 0, sizeof(secret.key));
		memset(secret.sha, 0, sizeof(secret.sha));
		strcat(secret.name, this->name_.data());
#endif

		// calculate header->fileLen + streamfiles

		zone_buffer fastfile(header->fileLen);

		// Do streamfile stuff
		/*auto streamfiles_count = buf->streamfile_count();
		if (streamfiles_count > 0)
		{
			if (streamfiles_count > 55168)
			{
				ZONETOOL_ERROR("There was an error writing the zone: Too many streamFiles!");
				return;
			}

			header->imageCount = static_cast<std::uint32_t>(streamfiles_count);
			std::uint64_t base_len = buf_compressed.size() + sizeof(XFileHeader) + (sizeof(XStreamFile) * streamfiles_count);

			// Generate fastfile
			fastfile = zone_buffer(base_len);
			fastfile.init_streams(1);
			fastfile.write_stream(header, sizeof(XFileHeader) - 16);

			// Write stream files
			std::uint64_t total_len = base_len;
			for (std::size_t i = 0; i < streamfiles_count; i++)
			{
				auto* stream = reinterpret_cast<XStreamFile*>(buf->get_streamfile(i));
				fastfile.write_stream(stream, sizeof(XStreamFile));

				total_len += (stream->offsetEnd - stream->offset);
			}

			header->baseFileLen = base_len;
			header->totalFileLen = total_len;

			fastfile.write_stream(&header->baseFileLen, 8);
			fastfile.write_stream(&header->totalFileLen, 8);

#ifdef FF_SIGNED
			fastfile.write_stream(&secret, sizeof(XFileSignedInfo));
#endif
		}
		else*/
		{
			// Generate fastfile
			fastfile.init_streams(1);
			fastfile.write_stream(header, sizeof(XFileHeader));
#ifdef FF_SIGNED
			fastfile.write_stream(&secret, sizeof(XFileSignedInfo));
#endif
		}

		fastfile.write(buf_output, buf_output_size);

		std::string path = this->name_ + ".ff";
		fastfile.save(path);

		ZONETOOL_INFO("Successfully compiled fastfile \"%s\"!", this->name_.data());
		ZONETOOL_INFO("Compiling took %llu msec.", (GetTickCount64() - start_time));
	}

	zone_interface::zone_interface(std::string name)
	{
		this->name_ = name;

		this->m_assetbase = 0;

		this->m_zonemem = std::make_shared<zone_memory>(MAX_ZONE_SIZE);
	}

	zone_interface::~zone_interface()
	{
		// wipe all assets
		m_assets.clear();
	}
}
