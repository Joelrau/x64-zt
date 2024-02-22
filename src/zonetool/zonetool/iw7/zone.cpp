#include <std_include.hpp>
#include "zonetool.hpp"
#include "zone.hpp"
#include "zonetool/utils/utils.hpp"

#include <utils/io.hpp>
#include <utils/cryptography.hpp>

#include <zlib.h>
#include <lz4.h>
#include <lz4hc.h>

#define FF_MAGIC_SIGNED "IWff0100"
#define FF_MAGIC_UNSIGNED "IWffu100"

//#define FF_SIGNED
#ifdef FF_SIGNED
#define FF_MAGIC FF_MAGIC_SIGNED
#else
#define FF_MAGIC FF_MAGIC_UNSIGNED
#endif

#define FF_VERSION 1619

#define CUSTOM_IMAGEFILE_INDEX 431

#define COMPRESS_BLOCK_TYPE_ZLIB 1
#define COMPRESS_BLOCK_TYPE_LZ4 4
#define COMPRESS_BLOCK_TYPE COMPRESS_BLOCK_TYPE_LZ4

#define COMPRESSOR_BLOCK 2
#define COMPRESSOR_PASSTHROUGH 1
#define COMPRESSOR COMPRESSOR_BLOCK

#ifdef FF_SIGNED
static_assert(COMPRESS_BLOCK_TYPE == COMPRESS_BLOCK_TYPE_LZ4, "LZ4 is the only compression type supported with signed fastfiles currently");
static_assert(COMPRESSOR == COMPRESSOR_BLOCK, "Block compressor is the only supported type with signed fastfiles currently");
#else
static_assert(COMPRESSOR == COMPRESSOR_BLOCK || COMPRESSOR == COMPRESSOR_PASSTHROUGH, "Unknown compressor type");
static_assert(COMPRESS_BLOCK_TYPE == COMPRESS_BLOCK_TYPE_LZ4 || COMPRESSOR == COMPRESS_BLOCK_TYPE_ZLIB, "Unsupported block compression type");
#endif

namespace zonetool::iw7
{
	namespace compression::iwc
	{
		size_t align_value(size_t value, unsigned int alignment)
		{
			return (value + alignment - 1) & ~(static_cast<size_t>(alignment) - 1);
		}

		const char* align_value(const char* value, unsigned int alignment)
		{
			return reinterpret_cast<const char*>(align_value(reinterpret_cast<size_t>(value), alignment));
		}

		constexpr auto MAX_BLOCK_SIZE = 0x10000ull;
		constexpr auto BLOCK_SIZE_CHUNK_SIGNED = 0x4000ull;
		constexpr auto BLOCK_SIZE_SIGNED = BLOCK_SIZE_CHUNK_SIGNED - sizeof(XBlockCompressionBlockHeader);
		constexpr auto BLOCK_SIZE_FIRST_SIGNED = BLOCK_SIZE_SIGNED - sizeof(XBlockCompressionDataHeader) - sizeof(XFileCompressorHeader);

		std::vector<std::uint8_t> compress_block_signed(const std::uint8_t* data, const std::size_t size, const int type, std::vector<DB_AuthHash>& chunk_hashes)
		{
			auto* block_buffer = reinterpret_cast<std::uint8_t*>(malloc(BLOCK_SIZE_CHUNK_SIGNED));

			std::vector<std::uint8_t> out_buffer;

			auto bytes_to_compress = size;
			if (bytes_to_compress > std::numeric_limits<unsigned int>::max())
			{
				throw std::runtime_error("cannot compress more than `std::numeric_limits<unsigned int>::max()` bytes");
			}

			auto data_ptr = reinterpret_cast<const char*>(data);

			const auto write = [&](void* data, const size_t len)
			{
				for (auto i = 0ull; i < len; i++)
				{
					out_buffer.push_back(reinterpret_cast<char*>(data)[i]);
				}
			};

			XFileCompressorHeader compress_header{};
			memcpy(compress_header.magic, "IWC", 3);
			compress_header.compressor = DB_COMPRESSOR_BLOCK;
			write(&compress_header, sizeof(XFileCompressorHeader));

			auto first_block = true;

			const auto compress_block_lz4 = [&]()
			{
				memset(block_buffer, 0, sizeof(block_buffer));

				const auto uncompressed_size = static_cast<unsigned int>(std::min(bytes_to_compress, MAX_BLOCK_SIZE));
				memcpy(block_buffer, data_ptr, uncompressed_size);

				const auto block_size = first_block ? BLOCK_SIZE_FIRST_SIGNED : BLOCK_SIZE_SIGNED;
				const auto bound = LZ4_compressBound(static_cast<int>(block_size));
				assert(bound > block_size);

				std::string buffer;
				buffer.resize(block_size);

				const auto compressed_size = LZ4_compress_HC(reinterpret_cast<char*>(block_buffer),
					buffer.data(), uncompressed_size, bound, LZ4HC_CLEVEL_DEFAULT);
				assert(buffer.size() == block_size);

				if (first_block)
				{
					XBlockCompressionDataHeader header{};
					header.uncompressedSize = bytes_to_compress;
					header.blockSizeAndType.blockSize = MAX_BLOCK_SIZE;
					header.blockSizeAndType.compressionType = XBLOCK_COMPRESSION_LZ4;

					write(&header, sizeof(header));
				}

				XBlockCompressionBlockHeader block_header{};
				block_header.compressedSize = compressed_size;
				block_header.uncompressedSize = uncompressed_size;

				write(&block_header, sizeof(block_header));

				write(buffer.data(), buffer.size());

				first_block = false;

				bytes_to_compress -= uncompressed_size;
				data_ptr += uncompressed_size;

				assert(buffer.size() % BLOCK_SIZE_CHUNK_SIGNED);
			};

			const auto compress_block_zlib = [&]()
			{
				__debugbreak();
			};

			while (bytes_to_compress > 0)
			{
				if (type == XBLOCK_COMPRESSION_LZ4)
				{
					compress_block_lz4();
				}
				else
				{
					__debugbreak();
				}

				// calc hash
				DB_AuthHash hash{};
				hash_state state{};
				sha256_init(&state);
				sha256_process(&state, out_buffer.data() + out_buffer.size() - BLOCK_SIZE_CHUNK_SIGNED, BLOCK_SIZE_CHUNK_SIGNED);
				sha256_done(&state, hash.bytes);
				chunk_hashes.push_back(hash);
			}

			free(block_buffer);

			return out_buffer;
		}

		std::vector<std::uint8_t> compress_block(const std::uint8_t* data, const std::size_t size, const int type)
		{
			std::vector<std::uint8_t> out_buffer;

			auto bytes_to_compress = size;
			if (bytes_to_compress > std::numeric_limits<unsigned int>::max())
			{
				throw std::runtime_error("cannot compress more than `std::numeric_limits<unsigned int>::max()` bytes");
			}

			auto data_ptr = reinterpret_cast<const char*>(data);

			const auto write = [&](void* data, const size_t len)
			{
				for (auto i = 0ull; i < len; i++)
				{
					out_buffer.push_back(reinterpret_cast<char*>(data)[i]);
				}
			};

			XFileCompressorHeader compress_header{};
			memcpy(compress_header.magic, "IWC", 3);
			compress_header.compressor = DB_COMPRESSOR_BLOCK;
			write(&compress_header, sizeof(XFileCompressorHeader));

			auto first_block = true;

			const auto compress_block_lz4 = [&]()
			{
				const auto block_size = static_cast<unsigned int>(std::min(bytes_to_compress, MAX_BLOCK_SIZE));
				const auto bound = LZ4_compressBound(block_size);

				std::string buffer;
				buffer.resize(bound);

				const auto compressed_size = LZ4_compress_HC(data_ptr,
					buffer.data(), block_size, bound, LZ4HC_CLEVEL_DEFAULT);
				buffer.resize(align_value(compressed_size, 4));

				if (first_block)
				{
					XBlockCompressionDataHeader header{};
					header.uncompressedSize = bytes_to_compress;
					header.blockSizeAndType.blockSize = MAX_BLOCK_SIZE;
					header.blockSizeAndType.compressionType = XBLOCK_COMPRESSION_LZ4;

					write(&header, sizeof(header));
				}

				XBlockCompressionBlockHeader block_header{};
				block_header.compressedSize = compressed_size;
				block_header.uncompressedSize = block_size;

				write(&block_header, sizeof(block_header));

				write(buffer.data(), buffer.size());

				first_block = false;

				bytes_to_compress -= block_size;
				data_ptr += block_size;
			};

			const auto compress_block_zlib = [&]()
			{
				const auto block_size = static_cast<unsigned int>(std::min(bytes_to_compress, MAX_BLOCK_SIZE));
				auto bound = compressBound(static_cast<unsigned long>(block_size));

				std::string buffer;
				buffer.resize(bound);

				compress2(reinterpret_cast<unsigned char*>(buffer.data()), &bound,
					reinterpret_cast<const unsigned char*>(data_ptr), static_cast<uLong>(size), Z_BEST_COMPRESSION);
				const auto compressed_size = bound;
				buffer.resize(compressed_size);

				if (first_block)
				{
					XBlockCompressionDataHeader header{};
					header.uncompressedSize = bytes_to_compress;
					header.blockSizeAndType.blockSize = MAX_BLOCK_SIZE;
					header.blockSizeAndType.compressionType = XBLOCK_COMPRESSION_ZLIB_SIZE;

					write(&header, sizeof(header));
				}

				XBlockCompressionBlockHeader block_header{};
				block_header.compressedSize = compressed_size;
				block_header.uncompressedSize = block_size;

				write(&block_header, sizeof(block_header));

				write(buffer.data(), buffer.size());

				first_block = false;

				bytes_to_compress -= block_size;
				data_ptr += block_size;
			};

			const auto compress_block_none = [&]()
			{
				const auto block_size = static_cast<unsigned int>(std::min(bytes_to_compress, MAX_BLOCK_SIZE));

				if (first_block)
				{
					XBlockCompressionDataHeader header{};
					header.uncompressedSize = bytes_to_compress;
					header.blockSizeAndType.blockSize = MAX_BLOCK_SIZE;
					header.blockSizeAndType.compressionType = XBLOCK_COMPRESSION_NONE;

					write(&header, sizeof(header));
				}

				XBlockCompressionBlockHeader block_header{};
				block_header.compressedSize = 0;
				block_header.uncompressedSize = block_size;

				write(&block_header, sizeof(block_header));

				write(const_cast<char*>(data_ptr), block_size);

				first_block = false;

				bytes_to_compress -= block_size;
				data_ptr += block_size;
			};

			while (bytes_to_compress > 0)
			{
				if (type == XBLOCK_COMPRESSION_LZ4)
				{
					compress_block_lz4();
				}
				else if (type == XBLOCK_COMPRESSION_ZLIB_SIZE)
				{
					compress_block_zlib();
				}
				else
				{
					compress_block_none(); // not working
				}
			}

			return out_buffer;
		}
	}

	namespace imagefile
	{
		void compress_images(const std::vector<gfx_image*>& images)
		{
			ZONETOOL_INFO("Compressing images...");

			const auto max_threads = std::thread::hardware_concurrency() * 2;
			const auto images_per_thread = std::max(1, static_cast<int>(images.size() / max_threads));

			std::vector<std::thread> threads;

			const auto start_thread = [&](const int index, const int count)
			{
				threads.emplace_back([&, index, count]
				{
					for (auto i = index; i < index + count; i++)
					{
						const auto image = images[i];
						for (auto o = 0; o < 4; o++)
						{
							auto& path = image->image_stream_blocks_paths[o];
							if (!path.has_value())
							{
								continue;
							}

							auto block = utils::io::read_file(path.value());
							const auto compressed = compression::iwc::compress_block(reinterpret_cast<std::uint8_t*>(block.data()), block.size(), XBLOCK_COMPRESSION_LZ4);
							const std::string compressed_str = { compressed.begin(), compressed.end() };
							image->image_stream_blocks[o].emplace(compressed_str);
						}
					}
				});
			};

			auto images_left = static_cast<int>(images.size());
			auto index = 0;
			while (images_left > 0)
			{
				const auto count = std::min(images_left, images_per_thread);
				start_thread(index, count);
				index += count;
				images_left -= count;
			}

			for (auto& thread : threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}

		void generate(const std::string& fastfile, std::uint16_t index, int ff_version, const std::string& ff_magic,
			std::vector<gfx_image*> images, zone_memory* mem)
		{
			compress_images(images);

			if (images.size() == 0)
			{
				return;
			}

			std::string image_file_buffer;

			ZONETOOL_INFO("Writing imagefile...");

			XPakHeader header{};
			std::memcpy(&header.magic, ff_magic.data(), ff_magic.size());
			header.version = ff_version;

			image_file_buffer.append(reinterpret_cast<char*>(&header), sizeof(XPakHeader));

			const auto write_image_file = [&]
			{
				const auto header = reinterpret_cast<XPakHeader*>(image_file_buffer.data());

				const auto hash_start = reinterpret_cast<std::uint8_t*>(image_file_buffer.data() + sizeof(XPakHeader));
				const auto len = image_file_buffer.size() - sizeof(XPakHeader);

				const auto hash = utils::cryptography::sha256::compute(hash_start, len, false);
				std::memcpy(header->hash.bytes, hash.data(), sizeof(header->hash));

				const auto save_path = utils::io::directory_exists("zone") ? "zone/" : "";
				const auto name = utils::string::va("%s%s.pak", save_path, fastfile.data(), index);
				utils::io::write_file(name, image_file_buffer);
			};

			for (auto i = 0; i < 4; i++)
			{
				for (const auto& image : images)
				{
					if (image->image_stream_files[i])
					{
						continue;
					}

					image->image_stream_files[i] = mem->allocate<XStreamFile>();

					if (!image->image_stream_blocks[i].has_value())
					{
						continue;
					}

					const auto& data = image->image_stream_blocks[i].value();
					const auto offset = image_file_buffer.size();
					image_file_buffer.append(reinterpret_cast<const char*>(data.data()), data.size());
					const auto offset_end = image_file_buffer.size();
					image->image_stream_blocks[i].reset();

					image->image_stream_files[i]->fileIndex = index;
					image->image_stream_files[i]->offset = offset;
					image->image_stream_files[i]->offsetEnd = offset_end;
				}
			}

			write_image_file();
		}
	}

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

		const std::uint64_t mask = 0x0000000000000000;
		for (std::size_t idx = 0; idx < m_assets.size(); idx++)
		{
			if (m_assets[idx]->type() == type && (m_assets[idx]->name() == name || m_assets[idx]->name() == ref_name))
			{
				auto ptr = mask | (static_cast<std::uint64_t>(XFILE_BLOCK_VIRTUAL) & 0x0F) << 32; // add stream index
				ptr = (ptr + static_cast<std::uint32_t>((this->m_assetbase + ((16 * idx) + 8) + 1))); // add offset
				return reinterpret_cast<void*>(ptr);
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
			ADD_ASSET(ASSET_TYPE_FX, fx_effect_def);
			ADD_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation);
			ADD_ASSET(ASSET_TYPE_IMAGE, gfx_image);
			ADD_ASSET(ASSET_TYPE_LIGHT_DEF, gfx_light_def);
			ADD_ASSET(ASSET_TYPE_LASER, laser);
			ADD_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, localize);
			ADD_ASSET(ASSET_TYPE_LUA_FILE, lua_file);
			ADD_ASSET(ASSET_TYPE_MATERIAL, material);
			ADD_ASSET(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings);
			ADD_ASSET(ASSET_TYPE_VFX, particle_system);
			ADD_ASSET(ASSET_TYPE_RAWFILE, rawfile);
			ADD_ASSET(ASSET_TYPE_RETICLE, reticle);
			ADD_ASSET(ASSET_TYPE_RUMBLE, rumble);
			ADD_ASSET(ASSET_TYPE_RUMBLE_GRAPH, rumble_graph);
			ADD_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile);
			ADD_ASSET(ASSET_TYPE_STRINGTABLE, string_table);
			ADD_ASSET(ASSET_TYPE_TTF, font_def);
			ADD_ASSET(ASSET_TYPE_VECTORFIELD, vector_field);
			ADD_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment);
			ADD_ASSET(ASSET_TYPE_ANIM_PACKAGE, weapon_anim_package);
			ADD_ASSET(ASSET_TYPE_SFX_PACKAGE, weapon_sfx_package);
			ADD_ASSET(ASSET_TYPE_VFX_PACKAGE, weapon_vfx_package);
			//ADD_ASSET(ASSET_TYPE_WEAPON, weapon_def);
			ADD_ASSET(ASSET_TYPE_XANIMPARTS, xanim_parts);
			ADD_ASSET(ASSET_TYPE_XMODEL, xmodel);
			ADD_ASSET(ASSET_TYPE_XMODEL_SURFS, xsurface);
			
			//ADD_ASSET(ASSET_TYPE_SOUND_GLOBALS, sound_globals);
			ADD_ASSET(ASSET_TYPE_SOUND_BANK, sound_bank);
			//ADD_ASSET(ASSET_TYPE_SOUND_BANK_TRANSIENT, sound_bank_transient);

			ADD_ASSET(ASSET_TYPE_PHYSICSASSET, physics_asset);
			ADD_ASSET(ASSET_TYPE_PHYSICS_FX_PIPELINE, physics_fx_pipeline);
			ADD_ASSET(ASSET_TYPE_PHYSICS_FX_SHAPE, physics_fx_shape);
			ADD_ASSET(ASSET_TYPE_PHYSICSLIBRARY, physics_library);
			ADD_ASSET(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, physics_sfx_event);
			ADD_ASSET(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, physics_vfx_event);

			ADD_ASSET(ASSET_TYPE_COMPUTESHADER, compute_shader);
			ADD_ASSET(ASSET_TYPE_DOMAINSHADER, domain_shader);
			ADD_ASSET(ASSET_TYPE_HULLSHADER, hull_shader);
			ADD_ASSET(ASSET_TYPE_PIXELSHADER, pixel_shader);
			//ADD_ASSET(ASSET_TYPE_VERTEXDECL, vertex_decl);
			ADD_ASSET(ASSET_TYPE_VERTEXSHADER, vertex_shader);

			ADD_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset);
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
				imagefile::generate(filesystem::get_fastfile(),
					CUSTOM_IMAGEFILE_INDEX, FF_VERSION, FF_MAGIC_UNSIGNED, images, this->m_zonemem.get());
			}
		}

		std::uintptr_t following = static_cast<std::uintptr_t>(buf->data_following);
		std::uintptr_t zero = 0; // data_none

		// write asset types to header
		for (std::size_t i = 0; i < m_assets.size(); i++)
		{
			m_assets[i]->prepare(buf, this->m_zonemem.get());
		}

		buf->push_stream(XFILE_BLOCK_TEMP);

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

		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		// write scriptstrings
		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		if (stringcount)
		{
			buf->align(7);

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
				for (auto j = 0; j < 4; j++)
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
#if (COMPRESSOR == COMPRESSOR_BLOCK)
#ifdef FF_SIGNED
		std::vector<DB_AuthHash> chunk_hashes{};
		const auto buf_compressed = compression::iwc::compress_block_signed(buf->buffer(), buf->size(), COMPRESS_BLOCK_TYPE, chunk_hashes);
		const auto buf_output = buf_compressed.data();
		const auto buf_output_size = buf_compressed.size();
#else
		const auto buf_compressed = compression::iwc::compress_block(buf->buffer(), buf->size(), COMPRESS_BLOCK_TYPE);
		const auto buf_output = buf_compressed.data();
		const auto buf_output_size = buf_compressed.size();
#endif
#elif (COMPRESSOR == COMPRESSOR_PASSTHROUGH)
		const auto buf_output = buf->buffer();
		const auto buf_output_size = buf->size();

		XFileCompressorHeader compress_header{};
		memcpy(compress_header.magic, "IWC", 3);
		compress_header.compressor = DB_COMPRESSOR_PASSTHROUGH;
#endif

		// Generate FF header
		XFileHeader header{};
		strcat(header.magic, FF_MAGIC);
		header.version = FF_VERSION;
		header.unused = 0;
		header.has_no_image_fastfile = 1;
		header.has_no_shared_fastfile = 1;
		header.unk1 = 1;
		header.fileTimeLow = 0;
		header.fileTimeHigh = 0;
		header.shared_ff_hash = 0;
		header.shared_ff_count = 0;
		header.image_ff_hash = 0;
		header.image_ff_count = 0;
		header.fileLen = buf_output_size + sizeof(XFileHeader) + sizeof(XFileCompressorHeader);
		header.fileLenUnk1 = 0; // imagefile?
		header.fileLenUnk2 = 0; // shared?

		{
			header.stream_data.unk1 = 0;
			header.stream_data.unk2 = 0;

			std::uint64_t total_block_size = 0;
			for (auto i = 0; i < MAX_XFILE_COUNT; i++)
			{
				header.stream_data.block_size[i] = buf->stream_offset(static_cast<std::uint8_t>(i));
				total_block_size += header.stream_data.block_size[i];
			}
			header.stream_data.size = total_block_size; // not correct
			memset(header.stream_data.unk_arr, 0, sizeof(header.stream_data.unk_arr));
		}

#ifdef FF_SIGNED
		header.fileLen += sizeof(DB_MasterBlock);

		DB_MasterBlock master_block{};
		memset(&master_block, 0, sizeof(DB_MasterBlock));

		// process_master_blocks()
		{
			for (auto i = 0; i < chunk_hashes.size(); i++)
			{
				memcpy(master_block.chunkHashes[i].bytes, chunk_hashes[i].bytes, sizeof(DB_AuthHash));
			}
		}

		header.fileLen += sizeof(DB_AuthHeader);

		DB_AuthHeader auth_header{};
		memset(&auth_header, 0, sizeof(DB_AuthHeader));
		strcat(auth_header.magic, "IWffs100");
		auth_header.reserved = 0;
		strcat(auth_header.subheader.fastfileName, this->name_.data());
		auth_header.subheader.reserved = 0;

		// process_sub_header_master_block_hashes()
		{
			///for (std::uint8_t i = 0; i < 192; i++)
			{
				hash_state state{};
				sha256_init(&state);
				sha256_process(&state, reinterpret_cast<unsigned char*>(&master_block), sizeof(DB_MasterBlock));
				sha256_done(&state, auth_header.subheader.masterBlockHashes[0].bytes);
			}
		}

		// process_sub_header()
		{
			hash_state state{};
			sha256_init(&state);
			sha256_process(&state, reinterpret_cast<unsigned char*>(&auth_header.subheader), sizeof(DB_AuthSubHeader));
			sha256_done(&state, auth_header.subheaderHash.bytes);
		}

		// process_signature
		{
			// we don't know the private key so we can't generate signature?
			constexpr auto DEADBEEF = 0xDEADBEEF;
			memcpy_s(auth_header.signedSubheaderHash.bytes, sizeof(DB_AuthSignature), &DEADBEEF, sizeof(DEADBEEF));
		}
#endif

		const auto image_streamfiles_count = buf->streamfile_count();
		if (image_streamfiles_count)
		{
			header.image_ff_count = static_cast<std::uint32_t>(image_streamfiles_count);
		}

		header.fileLen += sizeof(XStreamFile) * header.image_ff_count;
		header.fileLen += sizeof(XStreamFile) * header.shared_ff_count;

		zone_buffer fastfile(header.fileLen);

		// Do streamfile stuff
		if (image_streamfiles_count > 0)
		{
			const auto offset = sizeof(XFileHeader) - offsetof(XFileHeader, fileLen);

			// Generate fastfile
			fastfile.init_streams(1);
			fastfile.write_stream(&header, sizeof(XFileHeader) - offset);

			// Write stream files
			for (std::size_t i = 0; i < image_streamfiles_count; i++)
			{
				auto* stream = reinterpret_cast<XStreamFile*>(buf->get_streamfile(i));
				fastfile.write_stream(stream, sizeof(XStreamFile));
			}

			fastfile.write_stream(reinterpret_cast<std::uint8_t*>(&header) + sizeof(XFileHeader) - offset, offset);
		}
		else
		{
			// Generate fastfile
			fastfile.init_streams(1);
			fastfile.write_stream(&header, sizeof(XFileHeader));
		}
#ifdef FF_SIGNED
		fastfile.write_stream(&auth_header, sizeof(DB_AuthHeader));
		fastfile.write_stream(&master_block, sizeof(DB_MasterBlock));
#endif
#if (COMPRESSOR == COMPRESSOR_PASSTHROUGH)
		fastfile.write_stream(&compress_header, sizeof(XFileCompressorHeader));
#endif

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
