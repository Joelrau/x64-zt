#include <std_include.hpp>
#include "zonetool.hpp"
#include "zone.hpp"
#include "zonetool/utils/utils.hpp"

#include <utils/io.hpp>

#define FF_VERSION 66

#define COMPRESS_TYPE_LZ4 4
#define COMPRESS_TYPE_ZLIB 1

#define COMPRESS_TYPE COMPRESS_TYPE_ZLIB
static_assert(COMPRESS_TYPE == COMPRESS_TYPE_LZ4 || COMPRESS_TYPE == COMPRESS_TYPE_ZLIB);

namespace zonetool::h1
{
	namespace
	{
		void write_stream_files(std::vector<IGfxImage*> images, ZoneMemory* mem)
		{
			if (images.size() == 0)
			{
				return;
			}

			std::uint16_t current_index = 96; // unused imagefile index for h1
			std::string image_file_buffer;

			const auto init_image_file = [&]
			{
				image_file_buffer.clear();

				XPakHeader header{};
				std::strcat(header.header, "S1ffu100");
				header.version = FF_VERSION;

				image_file_buffer.append(reinterpret_cast<char*>(&header), sizeof(XPakHeader));
			};

			const auto write_image_file = [&]
			{
				const auto& fastfile = filesystem::get_fastfile();
				const auto save_path = utils::io::directory_exists("zone") ? "zone/" : "";
				const auto name = utils::string::va("%s%s.pak", save_path, fastfile.data(), current_index);
				utils::io::write_file(name, image_file_buffer);
				init_image_file();
			};

			init_image_file();

			for (const auto& image : images)
			{
				for (auto i = 0; i < 4; i++)
				{
					if (image->image_stream_files[i])
					{
						continue;
					}

					image->image_stream_files[i] = mem->Alloc<XStreamFile>();

					if (!image->image_stream_blocks[i].has_value())
					{
						continue;
					}

					const auto& data = image->image_stream_blocks[i].value();
					const auto offset = image_file_buffer.size();
					image_file_buffer.append(reinterpret_cast<const char*>(data.data()), data.size());
					const auto offset_end = image_file_buffer.size();
					image->image_stream_blocks[i].reset();

					image->image_stream_files[i]->fileIndex = current_index;
					image->image_stream_files[i]->offset = offset;
					image->image_stream_files[i]->offsetEnd = offset_end;
				}
			}

			write_image_file();
		}
	}

	IAsset* Zone::find_asset(std::int32_t type, const std::string& name)
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

	void* Zone::get_asset_pointer(std::int32_t type, const std::string& name)
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

	void Zone::add_asset_of_type_by_pointer(std::int32_t type, void* pointer)
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

#define ADD_ASSET_PTR(__type__, __interface__) \
		if (type == __type__) \
		{ \
			auto asset = std::make_shared < __interface__ >(); \
			asset->init(pointer, this->m_zonemem.get()); \
			asset->load_depending(this); \
			m_assets.push_back(asset); \
		}

		try
		{
			// declare asset interfaces
			ADD_ASSET_PTR(ASSET_TYPE_MENU, IMenuDef);
			ADD_ASSET_PTR(ASSET_TYPE_LOCALIZE_ENTRY, ILocalize);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(XAssetType(type)), ex.what());
		}
	}

	void Zone::add_asset_of_type(std::int32_t type, const std::string& name)
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

#define ADD_ASSET(__type__, __interface__) \
		if (type == __type__) \
		{ \
			auto asset = std::make_shared < __interface__ >(); \
			asset->init(name, this->m_zonemem.get()); \
			asset->load_depending(this); \
			m_assets.push_back(asset); \
		}

		try
		{
			// declare asset interfaces
			ADD_ASSET(ASSET_TYPE_CLUT, IClut);
			ADD_ASSET(ASSET_TYPE_DOPPLER_PRESET, IDopplerPreset);
			ADD_ASSET(ASSET_TYPE_FX, IFxEffectDef);
			ADD_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, IFxParticleSimAnimation);
			ADD_ASSET(ASSET_TYPE_IMAGE, IGfxImage);
			ADD_ASSET(ASSET_TYPE_LIGHT_DEF, IGfxLightDef);
			ADD_ASSET(ASSET_TYPE_LOADED_SOUND, ILoadedSound);
			ADD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, ILocalize);
			ADD_ASSET(ASSET_TYPE_LPF_CURVE, ILpfCurve);
			ADD_ASSET(ASSET_TYPE_LUA_FILE, ILuaFile);
			ADD_ASSET(ASSET_TYPE_MAP_ENTS, IMapEnts);
			ADD_ASSET(ASSET_TYPE_MATERIAL, IMaterial);
			ADD_ASSET(ASSET_TYPE_NET_CONST_STRINGS, INetConstStrings);
			ADD_ASSET(ASSET_TYPE_RAWFILE, IRawFile);
			ADD_ASSET(ASSET_TYPE_REVERB_CURVE, IReverbCurve);
			ADD_ASSET(ASSET_TYPE_SCRIPTABLE, IScriptableDef);
			ADD_ASSET(ASSET_TYPE_SCRIPTFILE, IScriptFile);
			ADD_ASSET(ASSET_TYPE_SKELETONSCRIPT, ISkeletonScript);
			ADD_ASSET(ASSET_TYPE_SOUND, ISound);
			ADD_ASSET(ASSET_TYPE_SOUND_CONTEXT, ISoundContext);
			ADD_ASSET(ASSET_TYPE_SOUND_CURVE, ISoundCurve);
			ADD_ASSET(ASSET_TYPE_STRINGTABLE, IStringTable);
			ADD_ASSET(ASSET_TYPE_TECHNIQUE_SET, ITechset);
			ADD_ASSET(ASSET_TYPE_TRACER, ITracerDef);
			ADD_ASSET(ASSET_TYPE_TTF, IFont);
			ADD_ASSET(ASSET_TYPE_ATTACHMENT, IWeaponAttachment);
			ADD_ASSET(ASSET_TYPE_WEAPON, IWeaponDef);
			ADD_ASSET(ASSET_TYPE_XANIM, IXAnimParts);
			ADD_ASSET(ASSET_TYPE_XMODEL, IXModel);
			ADD_ASSET(ASSET_TYPE_XMODEL_SURFS, IXSurface);

			ADD_ASSET(ASSET_TYPE_PHYSCOLLMAP, IPhysCollmap);
			ADD_ASSET(ASSET_TYPE_PHYSCONSTRAINT, IPhysConstraint);
			ADD_ASSET(ASSET_TYPE_PHYSPRESET, IPhysPreset);
			ADD_ASSET(ASSET_TYPE_PHYSWATERPRESET, IPhysWaterPreset);
			ADD_ASSET(ASSET_TYPE_PHYSWORLDMAP, IPhysWorld);

			ADD_ASSET(ASSET_TYPE_COMPUTESHADER, IComputeShader);
			ADD_ASSET(ASSET_TYPE_DOMAINSHADER, IDomainShader);
			ADD_ASSET(ASSET_TYPE_HULLSHADER, IHullShader);
			ADD_ASSET(ASSET_TYPE_PIXELSHADER, IPixelShader);
			//ADD_ASSET(ASSET_TYPE_VERTEXDECL, IVertexDecl);
			ADD_ASSET(ASSET_TYPE_VERTEXSHADER, IVertexShader);

			ADD_ASSET(ASSET_TYPE_MENU, IMenuDef);
			ADD_ASSET(ASSET_TYPE_MENULIST, IMenuList);

			ADD_ASSET(ASSET_TYPE_AIPATHS, IAIPaths);
			ADD_ASSET(ASSET_TYPE_COL_MAP_MP, IClipMap);
			ADD_ASSET(ASSET_TYPE_COM_MAP, IComWorld);
			ADD_ASSET(ASSET_TYPE_FX_MAP, IFxWorld);
			ADD_ASSET(ASSET_TYPE_GFX_MAP, IGfxWorld);
			ADD_ASSET(ASSET_TYPE_GLASS_MAP, IGlassWorld);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while adding asset \"%s\" of type %s, exception was: \n%s",
				name.data(), type_to_string(XAssetType(type)), ex.what());
		}
	}

	std::int32_t Zone::get_type_by_name(const std::string& type)
	{
		return type_to_int(type);
	}

	void Zone::add_asset_of_type(const std::string& type, const std::string& name)
	{
		std::int32_t itype = type_to_int(type);
		this->add_asset_of_type(itype, name);
	}

	void Zone::build(ZoneBuffer* buf)
	{
		buf->init_streams(7);

		auto startTime = GetTickCount64();

		ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

		constexpr std::size_t num_streams = 7;
		XZoneMemory<num_streams> mem;

		std::size_t headersize = sizeof XZoneMemory<num_streams>;
		memset(&mem, 0, headersize);

		auto zone = buf->at<XZoneMemory<num_streams>>();

		{
			// write imagefile
			std::vector<IGfxImage*> images;
			for (std::size_t i = 0; i < m_assets.size(); i++)
			{
				if (m_assets[i]->type() == ASSET_TYPE_IMAGE)
				{
					const auto image = static_cast<IGfxImage*>(m_assets[i].get());
					if (image->custom_streamed_image)
					{
						images.emplace_back(image);
					}
				}
			}

			write_stream_files(images, this->m_zonemem.get());
		}

		// write zone header
		buf->write(&mem);

		std::uintptr_t pad = 0xFDFDFDFFFFFFFFFF;
		std::uintptr_t zero = 0;

		// write asset types to header
		for (std::size_t i = 0; i < m_assets.size(); i++)
		{
			m_assets[i]->prepare(buf, this->m_zonemem.get());
		}

		// write scriptstring count
		std::size_t stringcount = buf->scriptstring_count();
		buf->write<std::uintptr_t>(&stringcount);
		buf->write<std::uintptr_t>(stringcount > 0 ? (&pad) : (&zero)); // pointer to scriptstrings

		// write asset count
		std::size_t asset_count = m_assets.size();
		buf->write<std::uintptr_t>(&asset_count);
		buf->write<std::uintptr_t>(asset_count > 0 ? (&pad) : (&zero)); // pointer to assets

		bool write_globals = (buf->depthstencilstatebit_count() + buf->blendstatebits_count()) > 0;
		buf->write<std::uintptr_t>(write_globals ? (&pad) : (&zero)); // pointer to globals

		buf->push_stream(3);

		// write scriptstrings
		buf->push_stream(3);
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
					buf->write<std::uintptr_t>(&pad);
				}
			}

			// write scriptstrings
			for (std::size_t idx = 0; idx < stringcount; idx++)
			{
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
			buf->write<std::uintptr_t>(&pad); // pointer to gfxglobals

			// parse gfxglobals
			auto mem_ = this->m_zonemem.get();
			auto globals = mem_->Alloc<XGfxGlobals>();

			globals->depthStencilStateCount = static_cast<unsigned int>(buf->depthstencilstatebit_count());
			globals->depthStencilStateBits = mem_->Alloc<std::uint64_t>(globals->depthStencilStateCount);
			globals->depthStencilStates = mem_->Alloc<GfxZoneTableEntry>(globals->depthStencilStateCount);

			for (unsigned int i = 0; i < globals->depthStencilStateCount; i++)
			{
				globals->depthStencilStateBits[i] = buf->get_depthstencilstatebit(i);
			}

			globals->blendStateCount = static_cast<unsigned int>(buf->blendstatebits_count());
			globals->blendStateBits = mem_->Alloc<GfxBlendStateBits>(globals->blendStateCount);
			globals->blendStates = mem_->Alloc<GfxZoneTableEntry>(globals->blendStateCount);

			for (unsigned int i = 0; i < globals->blendStateCount; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					globals->blendStateBits[i][j] = buf->get_blendstatebits(i)[j];
				}
			}
			
			globals->perPrimConstantBufferCount = static_cast<unsigned int>(buf->ppas_count());
			globals->perPrimConstantBufferSizes = mem_->Alloc<unsigned int>(globals->perPrimConstantBufferCount);
			globals->perPrimConstantBuffers = mem_->Alloc<GfxZoneTableEntry>(globals->perPrimConstantBufferCount);

			for (unsigned int i = 0; i < globals->perPrimConstantBufferCount; i++)
			{
				globals->perPrimConstantBufferSizes[i] = buf->get_ppas(i);
			}

			globals->perObjConstantBufferCount = static_cast<unsigned int>(buf->poas_count());
			globals->perObjConstantBufferSizes = mem_->Alloc<unsigned int>(globals->perObjConstantBufferCount);
			globals->perObjConstantBuffers = mem_->Alloc<GfxZoneTableEntry>(globals->perObjConstantBufferCount);

			for (unsigned int i = 0; i < globals->perObjConstantBufferCount; i++)
			{
				globals->perObjConstantBufferSizes[i] = buf->get_poas(i);
			}

			globals->stableConstantBufferCount = static_cast<unsigned int>(buf->sas_count());
			globals->stableConstantBufferSizes = mem_->Alloc<unsigned int>(globals->stableConstantBufferCount);
			globals->stableConstantBuffers = mem_->Alloc<GfxZoneTableEntry>(globals->stableConstantBufferCount);

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
				ZoneBuffer::clear_pointer(&dest->depthStencilStateBits);
			}
			if (globals->blendStateBits)
			{
				buf->align(3);
				buf->write(globals->blendStateBits, globals->blendStateCount);
				ZoneBuffer::clear_pointer(&dest->blendStateBits);
			}
			buf->push_stream(2);
			if (globals->depthStencilStates)
			{
				buf->align(3);
				auto dest_0 = buf->write(globals->depthStencilStates, globals->depthStencilStateCount);
				for (unsigned int i = 0; i < globals->depthStencilStateCount; i++)
				{
					if (globals->depthStencilStates[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->depthStencilStates[i].dataPtr, 1);
						ZoneBuffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				ZoneBuffer::clear_pointer(&dest->depthStencilStates);
			}
			buf->pop_stream();
			buf->push_stream(2);
			if (globals->blendStates)
			{
				buf->align(3);
				auto dest_0 = buf->write(globals->blendStates, globals->blendStateCount);
				for (unsigned int i = 0; i < globals->blendStateCount; i++)
				{
					if (globals->blendStates[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->blendStates[i].dataPtr, 1);
						ZoneBuffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				ZoneBuffer::clear_pointer(&dest->blendStates);
			}
			buf->pop_stream();
			if (globals->perPrimConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perPrimConstantBufferSizes, globals->perPrimConstantBufferCount);
				ZoneBuffer::clear_pointer(&dest->perPrimConstantBufferSizes);
			}
			if (globals->perObjConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perObjConstantBufferSizes, globals->perObjConstantBufferCount);
				ZoneBuffer::clear_pointer(&dest->perObjConstantBufferSizes);
			}
			if (globals->stableConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->stableConstantBufferSizes, globals->stableConstantBufferCount);
				ZoneBuffer::clear_pointer(&dest->stableConstantBufferSizes);
			}
			buf->push_stream(2);
			if (globals->perPrimConstantBuffers)
			{
				buf->align(3);
				auto dest_0 = buf->write(globals->perPrimConstantBuffers, globals->perPrimConstantBufferCount);
				for (unsigned int i = 0; i < globals->perPrimConstantBufferCount; i++)
				{
					if (globals->perPrimConstantBuffers[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->perPrimConstantBuffers[i].dataPtr, 1);
						ZoneBuffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				ZoneBuffer::clear_pointer(&dest->perPrimConstantBuffers);
			}
			buf->pop_stream();
			buf->push_stream(2);
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
						ZoneBuffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				ZoneBuffer::clear_pointer(&dest->perObjConstantBuffers);
			}
			buf->pop_stream();
			buf->push_stream(2);
			if (globals->stableConstantBuffers)
			{
				buf->align(3);
				auto dest_0 = buf->write(globals->stableConstantBuffers, globals->stableConstantBufferCount);
				for (unsigned int i = 0; i < globals->stableConstantBufferCount; i++)
				{
					if (globals->stableConstantBuffers[i].voidPtr)
					{
						buf->align(0);
						buf->write(globals->stableConstantBuffers[i].dataPtr, 1);
						ZoneBuffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				ZoneBuffer::clear_pointer(&dest->stableConstantBuffers);
			}
			buf->pop_stream();
		}

		buf->pop_stream();
		buf->push_stream(3);

		// align buffer
		buf->align(3);

		// set asset ptr base
		this->m_assetbase = buf->stream_offset(3);
		ZONETOOL_INFO("m_assetbase: %llu", this->m_assetbase);

		// write asset types to header
		for (std::size_t i = 0; i < asset_count; i++)
		{
			// write asset data to zone
			auto type = static_cast<uintptr_t>(m_assets[i]->type());

			buf->write(&type);
			buf->write(&pad);
		}

		// write assets
		for (auto& asset : m_assets)
		{
#ifdef DEBUG
			ZONETOOL_INFO("writing asset \"%s\" of type %s...", asset->name().data(), type_to_string(XAssetType(asset->type())));
#endif

			// push stream
			buf->push_stream(0);
			buf->align(3);

			// write asset
			asset->write(this, buf);

			// pop stream
			buf->pop_stream();
		}

		// pop stream
		buf->pop_stream();

		// update zone header
		zone->size = static_cast<std::uint64_t>(buf->size() - headersize);
		zone->externalsize = 0;

		// Update stream data
		for (int i = 0; i < num_streams; i++)
		{
			zone->streams[i] = buf->stream_offset(static_cast<std::uint8_t>(i));
		}
		
#ifdef DEBUG
		// Dump zone to disk (for debugging)
		buf->save("zonetool\\_debug\\" + this->name_ + ".zone", false);
#endif

		// Compress buffer
#if (COMPRESS_TYPE == COMPRESS_TYPE_LZ4)
		auto buf_compressed = buf->compress_lz4(); // idk how to compress lz4 fastfiles properly
#elif (COMPRESS_TYPE == COMPRESS_TYPE_ZLIB)
		auto buf_compressed = buf->compress_zlib();
#endif

		// Generate FF header
		auto header = this->m_zonemem->Alloc<XFileHeader>();
		strcat(header->header, "S1ffu100");
		header->version = FF_VERSION;
		header->compress = 1;
		header->compressType = COMPRESS_TYPE; // 0 == INVALID, 1 == ZLIB, 3 == PASSTHROUGH, 4 == LZ4
		header->sizeOfPointer = 8;
		header->sizeOfLong = 4;
		header->fileTimeHigh = 0;
		header->fileTimeLow = 0;
		header->imageCount = 0;
		header->baseFileLen = buf_compressed.size() + sizeof(XFileHeader);
		header->totalFileLen = buf_compressed.size() + sizeof(XFileHeader);

		ZoneBuffer fastfile;

		// Do streamfile stuff
		auto streamfiles_count = buf->streamfile_count();
		if (streamfiles_count > 0)
		{
			if (streamfiles_count > 93056)
			{
				ZONETOOL_ERROR("There was an error writing the zone: Too many streamFiles!");
				return;
			}

			header->imageCount = static_cast<std::uint32_t>(streamfiles_count);
			std::uint64_t base_len = buf_compressed.size() + sizeof(XFileHeader) + (sizeof(XStreamFile) * streamfiles_count);

			// Generate fastfile
			fastfile = ZoneBuffer(base_len);
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
		}
		else
		{
			// Generate fastfile
			fastfile = ZoneBuffer(header->baseFileLen);
			fastfile.init_streams(1);
			fastfile.write_stream(header, sizeof(XFileHeader));
		}

		fastfile.write(buf_compressed.data(), buf_compressed.size());

		std::string path = this->name_ + ".ff";
		fastfile.save(path);

		ZONETOOL_INFO("Successfully compiled fastfile \"%s\"!", this->name_.data());
		ZONETOOL_INFO("Compiling took %llu msec.", (GetTickCount64() - startTime));
	}

	Zone::Zone(std::string name)
	{
		this->name_ = name;

		this->m_assetbase = 0;

		this->m_zonemem = std::make_shared<ZoneMemory>(MAX_ZONE_SIZE);
	}

	Zone::~Zone()
	{
		// wipe all assets
		m_assets.clear();
	}
}
