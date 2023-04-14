#include <std_include.hpp>
#include "zonetool.hpp"
#include "zone.hpp"
#include "zonetool/utils/utils.hpp"
#include "zonetool/utils/imagefile.hpp"

#include <utils/flags.hpp>
#include <utils/io.hpp>

#define FF_VERSION 130
#define FF_HEADER "S1ffu100"

#define COMPRESS_TYPE_LZ4 4
#define COMPRESS_TYPE_ZLIB 1

#define COMPRESS_TYPE COMPRESS_TYPE_LZ4
static_assert(COMPRESS_TYPE == COMPRESS_TYPE_LZ4 || COMPRESS_TYPE == COMPRESS_TYPE_ZLIB);

namespace zonetool::h2
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
			ADD_ASSET_PTR(ASSET_TYPE_LOCALIZE_ENTRY, localize);
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
			ADD_ASSET(ASSET_TYPE_CLUT, clut);
			ADD_ASSET(ASSET_TYPE_DOPPLER_PRESET, doppler_preset);
			ADD_ASSET(ASSET_TYPE_FX, fx_effect_def);
			ADD_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation);
			ADD_ASSET(ASSET_TYPE_IMAGE, gfx_image);
			ADD_ASSET(ASSET_TYPE_LIGHT_DEF, gfx_light_def);
			ADD_ASSET(ASSET_TYPE_LOADED_SOUND, loaded_sound);
			ADD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, localize);
			ADD_ASSET(ASSET_TYPE_LPF_CURVE, lpf_curve);
			ADD_ASSET(ASSET_TYPE_LUA_FILE, lua_file);
			ADD_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, IAddonMapEnts);
			ADD_ASSET(ASSET_TYPE_MAP_ENTS, map_ents);
			ADD_ASSET(ASSET_TYPE_MATERIAL, material);
			ADD_ASSET(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings);
			ADD_ASSET(ASSET_TYPE_RAWFILE, rawfile);
			ADD_ASSET(ASSET_TYPE_REVERB_CURVE, reverb_curve);
			ADD_ASSET(ASSET_TYPE_SCRIPTABLE, scriptable_def);
			ADD_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile);
			ADD_ASSET(ASSET_TYPE_SKELETONSCRIPT, skeleton_script);
			ADD_ASSET(ASSET_TYPE_SOUND, sound);
			ADD_ASSET(ASSET_TYPE_SOUND_CONTEXT, sound_context);
			ADD_ASSET(ASSET_TYPE_SOUND_CURVE, sound_curve);
			ADD_ASSET(ASSET_TYPE_STRINGTABLE, string_table);
			ADD_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset);
			ADD_ASSET(ASSET_TYPE_TRACER, tracer_def);
			ADD_ASSET(ASSET_TYPE_TTF, IFont);
			ADD_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment);
			ADD_ASSET(ASSET_TYPE_WEAPON, weapon_def);
			ADD_ASSET(ASSET_TYPE_VEHICLE, IVehicleDef);
			ADD_ASSET(ASSET_TYPE_XANIM, xanim_parts);
			ADD_ASSET(ASSET_TYPE_XMODEL, xmodel);
			ADD_ASSET(ASSET_TYPE_XMODEL_SURFS, xsurface);

			ADD_ASSET(ASSET_TYPE_PHYSCOLLMAP, phys_collmap);
			ADD_ASSET(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint);
			ADD_ASSET(ASSET_TYPE_PHYSPRESET, phys_preset);
			ADD_ASSET(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset);
			ADD_ASSET(ASSET_TYPE_PHYSWORLDMAP, phys_world);

			ADD_ASSET(ASSET_TYPE_COMPUTESHADER, compute_shader);
			ADD_ASSET(ASSET_TYPE_DOMAINSHADER, domain_shader);
			ADD_ASSET(ASSET_TYPE_HULLSHADER, hull_shader);
			ADD_ASSET(ASSET_TYPE_PIXELSHADER, pixel_shader);
			//ADD_ASSET(ASSET_TYPE_VERTEXDECL, vertex_decl);
			ADD_ASSET(ASSET_TYPE_VERTEXSHADER, vertex_shader);

			ADD_ASSET(ASSET_TYPE_AIPATHS, IAIPaths);
			ADD_ASSET(ASSET_TYPE_COL_MAP_SP, clip_map);
			ADD_ASSET(ASSET_TYPE_COM_MAP, com_world);
			ADD_ASSET(ASSET_TYPE_FX_MAP, fx_world);
			ADD_ASSET(ASSET_TYPE_GFX_MAP, gfx_world);
			ADD_ASSET(ASSET_TYPE_GLASS_MAP, glass_world);
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
		buf->init_streams(7);

		auto startTime = GetTickCount64();

		ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

		constexpr std::size_t num_streams = 7;
		Xzone_memory<num_streams> mem;

		std::size_t headersize = sizeof Xzone_memory<num_streams>;
		memset(&mem, 0, headersize);

		auto zone = buf->at<Xzone_memory<num_streams>>();

		{
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
				ZONETOOL_INFO("Writing imagefile...");
				imagefile::generate(filesystem::get_fastfile(),
					custom_imagefile_index, FF_VERSION, FF_HEADER, images, this->m_zonemem.get());
			}
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
				zone_buffer::clear_pointer(&dest->depthStencilStateBits);
			}
			if (globals->blendStateBits)
			{
				buf->align(3);
				buf->write(globals->blendStateBits, globals->blendStateCount);
				zone_buffer::clear_pointer(&dest->blendStateBits);
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
						zone_buffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				zone_buffer::clear_pointer(&dest->depthStencilStates);
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
						zone_buffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				zone_buffer::clear_pointer(&dest->blendStates);
			}
			buf->pop_stream();
			if (globals->perPrimConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perPrimConstantBufferSizes, globals->perPrimConstantBufferCount);
				zone_buffer::clear_pointer(&dest->perPrimConstantBufferSizes);
			}
			if (globals->perObjConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->perObjConstantBufferSizes, globals->perObjConstantBufferCount);
				zone_buffer::clear_pointer(&dest->perObjConstantBufferSizes);
			}
			if (globals->stableConstantBufferSizes)
			{
				buf->align(3);
				buf->write(globals->stableConstantBufferSizes, globals->stableConstantBufferCount);
				zone_buffer::clear_pointer(&dest->stableConstantBufferSizes);
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
						zone_buffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				zone_buffer::clear_pointer(&dest->perPrimConstantBuffers);
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
						zone_buffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				zone_buffer::clear_pointer(&dest->perObjConstantBuffers);
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
						zone_buffer::clear_pointer(&dest_0->dataPtr);
					}
				}
				zone_buffer::clear_pointer(&dest->stableConstantBuffers);
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

		ZONETOOL_INFO("Writing assets...");

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

		ZONETOOL_INFO("Compressing buffer...");

		// Compress buffer
#if (COMPRESS_TYPE == COMPRESS_TYPE_LZ4)
		auto buf_compressed = buf->compress_lz4(); // idk how to compress lz4 fastfiles properly
#elif (COMPRESS_TYPE == COMPRESS_TYPE_ZLIB)
		auto buf_compressed = buf->compress_zlib();
#endif

		// Generate FF header
		auto header = this->m_zonemem->allocate<XFileHeader>();
		strcat(header->header, FF_HEADER);
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

		zone_buffer fastfile;

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
		}
		else
		{
			// Generate fastfile
			fastfile = zone_buffer(header->baseFileLen);
			fastfile.init_streams(1);
			fastfile.write_stream(header, sizeof(XFileHeader));
		}

		fastfile.write(buf_compressed.data(), buf_compressed.size());

		std::string output_folder = utils::flags::get_flag("-output", "o", ".");
		std::string path = output_folder + "/" + this->name_ + ".ff";
		fastfile.save(path);

		ZONETOOL_INFO("Successfully compiled fastfile \"%s\" (%s)!", this->name_.data(), path.data());
		ZONETOOL_INFO("Compiling took %llu msec.", (GetTickCount64() - startTime));
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
