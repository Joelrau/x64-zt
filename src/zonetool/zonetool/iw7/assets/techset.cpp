#include <std_include.hpp>
#include "techset.hpp"

#include "vertexshader.hpp"
#include "vertexdecl.hpp"
#include "hullshader.hpp"
#include "domainshader.hpp"
#include "pixelshader.hpp"

//#include "zonetool/iw7/zonetool.hpp"

namespace zonetool::iw7
{
	std::string clean_name(const std::string& name)
	{
		auto new_name = name;

		for (auto i = 0u; i < name.size(); i++)
		{
			switch (new_name[i])
			{
			case '|':
				new_name[i] = '-';
				break;
			}
		}

		return new_name;
	}

	std::unordered_map<std::string, std::uintptr_t> techset::vertexdecl_pointers;

	std::uintptr_t techset::get_vertexdecl_pointer(std::string vertexdecl)
	{
		if (vertexdecl_pointers.find(vertexdecl) != vertexdecl_pointers.end())
		{
			return vertexdecl_pointers[vertexdecl];
		}
		return 0;
	}

	void techset::add_vertexdecl_pointer(std::string name, std::uintptr_t pointer)
	{
		auto ptr = get_vertexdecl_pointer(name);
		if (!ptr)
		{
			vertexdecl_pointers[name] = pointer;
		}
	}

	MaterialTechnique* parse_technique(const std::string& name, zone_memory* mem, std::uint32_t index)
	{
		const auto path = "techsets\\" + name + ".technique";

		assetmanager::reader reader(mem);
		if (!reader.open(path))
		{
			ZONETOOL_FATAL("technique \"%s\" is missing.", name.data());
			return nullptr;
		}

		//ZONETOOL_INFO("Parsing technique \"%s\"...", name.data());

		const auto header = reader.read_single<MaterialTechniqueHeader>();
		const auto passes = reader.read_array<MaterialPass>();

		header->name = reader.read_string();

		const auto asset = mem->manual_allocate<MaterialTechnique>(sizeof(MaterialTechniqueHeader) + (sizeof(MaterialPass) * header->passCount));
		memcpy(&asset->hdr, header, sizeof(MaterialTechniqueHeader));
		memcpy(asset->passArray, passes, sizeof(MaterialPass) * header->passCount);

		for (unsigned short i = 0; i < header->passCount; i++)
		{
			if (asset->passArray[i].vertexShader)
			{
				asset->passArray[i].vertexShader = reader.read_asset<MaterialVertexShader>();
			}

			if (asset->passArray[i].vertexDecl)
			{
				asset->passArray[i].vertexDecl = reader.read_asset<MaterialVertexDeclaration>();
				asset->passArray[i].vertexDecl = vertex_decl::parse(asset->passArray[i].vertexDecl->name, mem);
			}

			if (asset->passArray[i].hullShader)
			{
				asset->passArray[i].hullShader = reader.read_asset<MaterialHullShader>();
			}

			if (asset->passArray[i].domainShader)
			{
				asset->passArray[i].domainShader = reader.read_asset<MaterialDomainShader>();
			}

			if (asset->passArray[i].pixelShader)
			{
				asset->passArray[i].pixelShader = reader.read_asset<MaterialPixelShader>();
			}

			if (asset->passArray[i].args)
			{
				asset->passArray[i].args = reader.read_array<MaterialShaderArgument>();

				for (auto arg = 0; arg < asset->passArray[i].perObjArgCount + asset->passArray[i].perPrimArgCount + asset->passArray[i].stableArgCount; arg++)
				{
					if (asset->passArray[i].args[arg].type == 4)
					{
						asset->passArray[i].args[arg].u.literalConst = reader.read_array<float>();
					}
				}
			}
		}

		reader.close();

		return asset;
	}

	MaterialTechniqueSet* techset::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "techsets\\" + clean_name(name) + ".techset";

		assetmanager::reader reader(mem);
		if (!reader.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

		const auto asset = reader.read_single<MaterialTechniqueSet>();
		asset->name = reader.read_string();

		asset->techniques = mem->allocate<MaterialTechnique*>(asset->techniqueCount);
		for (auto i = 0u; i < asset->techniqueCount; i++)
		{
			asset->techniques[i] = parse_technique(reader.read_string(), mem, i);
		}

		reader.close();

		return asset;
	}

	namespace material_data
	{
		namespace
		{
			std::optional<std::string> find_first_file_with_extension_in_directory(const std::string& directory, const std::string& extension)
			{
				if (!std::filesystem::exists(directory))
				{
					return {};
				}

				std::string stored_path{};
				for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
				{
					if (std::filesystem::is_regular_file(entry) && entry.path().extension() == extension)
					{
						std::string file_path = entry.path().filename().string();
						auto parent_path = entry.path().parent_path();
						if (parent_path != directory)
						{
							file_path = std::filesystem::relative(parent_path, directory).string() + "\\" + file_path;
						}
						if (file_path.starts_with("$") || file_path.contains("default"))
						{
							stored_path = file_path;
						}
						else
						{
							return file_path;
						}
					}
				}

				return stored_path;
			}
		}

		std::string get_parse_path(const std::string& type, const std::string& ext, const std::string& techset, const std::string& material, bool* is_random = nullptr)
		{
			const std::string parent_path = utils::string::va("techsets\\%s\\%s", type.data(), techset.data());
			const std::string file = utils::string::va("%s%s", material.data(), ext.data());
			std::string path = parent_path + "\\" + file;

			if (filesystem::file(path).exists())
			{
				return path;
			}

			// get a random one from the directory
			{
				for (const auto& parse_path : filesystem::get_search_paths())
				{
					const std::string dir = parse_path + parent_path;
					const auto first_file = find_first_file_with_extension_in_directory(dir, ext);
					if (first_file.has_value() && !first_file.value().empty())
					{
						if (is_random)
							*is_random = true;

						path = parent_path + "\\" + first_file.value();
						return path;
					}
				}
			}

			return path;
		}
	}

	void techset::parse_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* indexes, zone_memory* mem)
	{
		const auto path = material_data::get_parse_path("constantbuffer", ".cbi", clean_name(techset), material);
		auto file = filesystem::file(path);
		file.open("rb");
		auto fp = file.get_fp();

		if (fp)
		{
			fread(indexes, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();
			return;
		}

		ZONETOOL_FATAL("constantbufferindexes for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::parse_constant_buffer_def_array(const std::string& techset, const std::string& material, 
		MaterialConstantBufferDef** def_ptr, unsigned char* count, zone_memory* mem)
	{
		const auto path = material_data::get_parse_path("constantbuffer", ".cbt", clean_name(techset), material);
		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			(*def_ptr) = nullptr;
			return;
		}

		*count = static_cast<unsigned char>(read.read_int());
		auto def = read.read_array<MaterialConstantBufferDef>();
		for (int i = 0; i < *count; i++)
		{
			if (def[i].vsData)
			{
				def[i].vsData = read.read_array<unsigned char>();
			}
			if (def[i].hsData)
			{
				def[i].hsData = read.read_array<unsigned char>();
			}
			if (def[i].dsData)
			{
				def[i].dsData = read.read_array<unsigned char>();
			}
			if (def[i].psData)
			{
				def[i].psData = read.read_array<unsigned char>();
			}
			if (def[i].vsOffsetData)
			{
				def[i].vsOffsetData = read.read_array<unsigned short>();
			}
			if (def[i].hsOffsetData)
			{
				def[i].hsOffsetData = read.read_array<unsigned short>();
			}
			if (def[i].dsOffsetData)
			{
				def[i].dsOffsetData = read.read_array<unsigned short>();
			}
			if (def[i].psOffsetData)
			{
				def[i].psOffsetData = read.read_array<unsigned short>();
			}
		}

		read.close();

		(*def_ptr) = def;
	}

	void techset::parse_stateinfo(const std::string& techset, const std::string& material, Material* mat, zone_memory* mem)
	{
		const auto path = material_data::get_parse_path("state", ".stateinfo", clean_name(techset), material);
		filesystem::file file(path);
		if (file.exists())
		{
			file.open("rb");
			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto stateInfo = json::parse(bytes);

			mat->stateFlags = stateInfo["stateFlags"].get<unsigned char>();

			return;
		}
		ZONETOOL_FATAL("stateinfo for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::parse_statebits(const std::string& techset, const std::string& material, unsigned char* statebits, zone_memory* mem)
	{
		const auto path = material_data::get_parse_path("state", ".statebits", clean_name(techset), material);
		auto file = filesystem::file(path);
		file.open("rb");
		auto fp = file.get_fp();

		if (fp)
		{
			fread(statebits, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();
			return;
		}

		ZONETOOL_FATAL("statebits for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::parse_statebitsmap(const std::string& techset, const std::string& material, GfxStateBits** map, unsigned char* count,
		std::vector<std::array<std::uint64_t, 14>>* dssb,
		std::vector<std::array<std::uint32_t, 4>>* bsb,
		zone_memory* mem)
	{
		const auto path = material_data::get_parse_path("state", ".statebitsmap", clean_name(techset), material);
		filesystem::file file(path);
		if (file.exists())
		{
			file.open("rb");
			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto stateMap = json::parse(bytes);
			if (stateMap.size() > 0)
			{
				auto stateBits = mem->allocate<GfxStateBits>(stateMap.size());
				for (int i = 0; i < stateMap.size(); i++)
				{
					stateBits[i].loadBits[0] = stateMap[i]["loadBits"][0].get<unsigned int>();
					stateBits[i].loadBits[1] = stateMap[i]["loadBits"][1].get<unsigned int>();
					stateBits[i].loadBits[2] = stateMap[i]["loadBits"][2].get<unsigned int>();
					stateBits[i].loadBits[3] = stateMap[i]["loadBits"][3].get<unsigned int>();
					stateBits[i].loadBits[4] = stateMap[i]["loadBits"][4].get<unsigned int>();
					stateBits[i].loadBits[5] = stateMap[i]["loadBits"][5].get<unsigned int>();

					std::array<std::uint64_t, 14> temp_bits = { 0 };
					for (int j = 0; j < 14; j++)
					{
						temp_bits[j] = stateMap[i]["depthStencilStateBits"][j].get<std::uint64_t>();
					}
					dssb->push_back(std::move(temp_bits));

					std::array<std::uint32_t, 4> temp_bits2;
					for (int j = 0; j < 4; j++)
					{
						temp_bits2[j] = stateMap[i]["blendStateBits"][j].get<std::uint32_t>();
					}
					bsb->push_back(std::move(temp_bits2));

					stateBits[i].rasterizerState = stateMap[i]["rasterizerState"].get<unsigned char>();
				}
				(*map) = stateBits;
			}
			else
			{
				(*map) = nullptr;
			}
			*count = static_cast<unsigned char>(stateMap.size());
			return;
		}
		ZONETOOL_FATAL("statebitsmap for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).techniqueSet;
			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				auto clean_namee = clean_name(name);
				ZONETOOL_FATAL("techset \"%s\" not found.", name.data());
			}
		}
	}

	void techset::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto data = this->asset_;
		for (auto technique = 0u; technique < data->techniqueCount; technique++)
		{
			for (unsigned short pass = 0; pass < data->techniques[technique]->hdr.passCount; pass++)
			{
				auto& techniquePass = data->techniques[technique]->passArray[pass];
				if (techniquePass.perPrimArgSize)
				{
					techniquePass.perPrimConstantBuffer = buf->write_ppas(techniquePass.perPrimArgSize);
				}
				if (techniquePass.perObjArgSize)
				{
					techniquePass.perObjConstantBuffer = buf->write_poas(techniquePass.perObjArgSize);
				}
				if (techniquePass.stableArgSize)
				{
					techniquePass.stableConstantBuffer = buf->write_sas(techniquePass.stableArgSize);
				}
			}
		}
	}

	void techset::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		for (auto technique = 0u; technique < data->techniqueCount; technique++)
		{
			for (unsigned short pass = 0; pass < data->techniques[technique]->hdr.passCount; pass++)
			{
				auto& techniquePass = data->techniques[technique]->passArray[pass];

				if (techniquePass.vertexShader)
				{
					zone->add_asset_of_type(ASSET_TYPE_VERTEXSHADER, techniquePass.vertexShader->name);
				}

				/*if (techniquePass.vertexDecl)
				{
					zone->add_asset_of_type(ASSET_TYPE_VERTEXDECL, techniquePass.vertexDecl->name);
				}*/

				if (techniquePass.hullShader)
				{
					zone->add_asset_of_type(ASSET_TYPE_HULLSHADER, techniquePass.hullShader->name);
				}

				if (techniquePass.domainShader)
				{
					zone->add_asset_of_type(ASSET_TYPE_DOMAINSHADER, techniquePass.domainShader->name);
				}

				if (techniquePass.pixelShader)
				{
					zone->add_asset_of_type(ASSET_TYPE_PIXELSHADER, techniquePass.pixelShader->name);
				}
			}
		}
	}

	std::string techset::name()
	{
		return this->name_;
	}

	std::int32_t techset::type()
	{
		return ASSET_TYPE_TECHNIQUE_SET;
	}

	void techset::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->techniques)
		{
			buf->align(7);
			auto dest_techniques = buf->write(data->techniques, data->techniqueCount);

			for (auto technique = 0; technique < data->techniqueCount; technique++)
			{
				buf->align(7);

				auto* technique_header = buf->write(&data->techniques[technique]->hdr);
				auto* technique_passes = buf->write(data->techniques[technique]->passArray, technique_header->passCount);

				for (unsigned short pass = 0; pass < technique_header->passCount; pass++)
				{
					if (technique_passes[pass].vertexShader)
					{
						technique_passes[pass].vertexShader =
							reinterpret_cast<MaterialVertexShader*>(zone->get_asset_pointer(
								ASSET_TYPE_VERTEXSHADER, technique_passes[pass].vertexShader->name));
					}

					if (technique_passes[pass].vertexDecl)
					{
						/*technique_passes[pass].vertexDecl =
							reinterpret_cast<MaterialVertexDeclaration*>(zone->get_asset_pointer(
								ASSET_TYPE_VERTEXDECL, technique_passes[pass].vertexDecl->name));*/
						std::uintptr_t ptr = get_vertexdecl_pointer(technique_passes[pass].vertexDecl->name);
						if (ptr)
						{
							technique_passes[pass].vertexDecl = reinterpret_cast<MaterialVertexDeclaration*>(ptr);
						}
						else
						{
							buf->push_stream(XFILE_BLOCK_TEMP);

							buf->push_stream(XFILE_BLOCK_VIRTUAL);
							buf->align(7);
							ptr = reinterpret_cast<std::uintptr_t>(buf->get_zone_pointer<MaterialVertexDeclaration>(buf->current_stream(), buf->stream_offset(XFILE_BLOCK_VIRTUAL)));
							add_vertexdecl_pointer(technique_passes[pass].vertexDecl->name, ptr);
							buf->inc_stream(XFILE_BLOCK_VIRTUAL, 8);
							buf->pop_stream();

							auto vertexDecl = buf->write(data->techniques[technique]->passArray[pass].vertexDecl);

							buf->push_stream(XFILE_BLOCK_VIRTUAL);
							if (data->techniques[technique]->passArray[pass].vertexDecl->name)
							{
								vertexDecl->name = buf->write_str(data->techniques[technique]->passArray[pass].vertexDecl->name);
							}
							buf->pop_stream();

							buf->insert_pointer(&technique_passes[pass].vertexDecl);

							buf->pop_stream();
						}
					}

					if (technique_passes[pass].hullShader)
					{
						technique_passes[pass].hullShader =
							reinterpret_cast<MaterialHullShader*>(zone->get_asset_pointer(
								ASSET_TYPE_HULLSHADER, technique_passes[pass].hullShader->name));
					}

					if (technique_passes[pass].domainShader)
					{
						technique_passes[pass].domainShader =
							reinterpret_cast<MaterialDomainShader*>(zone->get_asset_pointer(
								ASSET_TYPE_DOMAINSHADER, technique_passes[pass].domainShader->name));
					}

					if (technique_passes[pass].pixelShader)
					{
						technique_passes[pass].pixelShader =
							reinterpret_cast<MaterialPixelShader*>(zone->get_asset_pointer(
								ASSET_TYPE_PIXELSHADER, technique_passes[pass].pixelShader->name));
					}

					if (technique_passes[pass].args)
					{
						buf->align(7);
						technique_passes[pass].args = buf->write(data->techniques[technique]->passArray[pass].args,
							technique_passes[pass].perPrimArgCount +
							technique_passes[pass].perObjArgCount +
							technique_passes[pass].stableArgCount);

						for (auto arg = 0; arg <
							technique_passes[pass].perPrimArgCount +
							technique_passes[pass].perObjArgCount +
							technique_passes[pass].stableArgCount; arg++)
						{
							if (technique_passes[pass].args[arg].type == 4)
							{
								if (technique_passes[pass].args[arg].u.literalConst)
								{
									technique_passes[pass].args[arg].u.literalConst = buf->write_s(3,
										technique_passes[pass].args[arg].u.literalConst, 4);
								}
							}
						}
					}
				}

				technique_header->name = buf->write_str(technique_header->name);

				buf->clear_pointer(&dest_techniques[technique]);
			}

			buf->clear_pointer(&dest->techniques);
		}

		buf->pop_stream();
	}

	void techset::dump_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* cbi)
	{
		const auto path = "techsets\\constantbuffer\\"s + clean_name(techset) + "\\"s + material + ".cbi";
		auto file = filesystem::file(path);
		file.open("wb");
		auto fp = file.get_fp();

		if (fp)
		{
			fwrite(cbi, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();
		}
	}

	void techset::dump_constant_buffer_def_array(const std::string& techset, const std::string& material, unsigned char count, MaterialConstantBufferDef* def)
	{
		const auto path = "techsets\\constantbuffer\\"s + clean_name(techset) + "\\"s + material + ".cbt";
		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_int(count);
		dump.dump_array(def, count);
		for (int i = 0; i < count; i++)
		{
			if (def[i].vsData)
			{
				dump.dump_array(def[i].vsData, def[i].vsDataSize);
			}
			if (def[i].hsData)
			{
				dump.dump_array(def[i].hsData, def[i].hsDataSize);
			}
			if (def[i].dsData)
			{
				dump.dump_array(def[i].dsData, def[i].dsDataSize);
			}
			if (def[i].psData)
			{
				dump.dump_array(def[i].psData, def[i].psDataSize);
			}
			if (def[i].vsOffsetData)
			{
				dump.dump_array(def[i].vsOffsetData, def[i].vsOffsetDataSize);
			}
			if (def[i].hsOffsetData)
			{
				dump.dump_array(def[i].hsOffsetData, def[i].hsOffsetDataSize);
			}
			if (def[i].dsOffsetData)
			{
				dump.dump_array(def[i].dsOffsetData, def[i].dsOffsetDataSize);
			}
			if (def[i].psOffsetData)
			{
				dump.dump_array(def[i].psOffsetData, def[i].psOffsetDataSize);
			}
		}

		dump.close();
	}

	void techset::dump_stateinfo(const std::string& techset, const std::string& material, Material* mat)
	{
		const auto path = "techsets\\state\\"s + clean_name(techset) + "\\"s + material + ".stateinfo";

		ordered_json json_data = {};

		json_data["stateFlags"] = mat->stateFlags;

		auto file = filesystem::file(path);
		file.open("wb");
		auto fp = file.get_fp();
		if (fp)
		{
			const auto json_dump = json_data.dump(4);
			file.write(json_dump.data(), json_dump.size(), 1);
			file.close();
		}
	}

	void techset::dump_statebits(const std::string& techset, const std::string& material, unsigned char* statebits)
	{
		const auto path = "techsets\\state\\"s + clean_name(techset) + "\\"s + material + ".statebits";
		auto file = filesystem::file(path);
		file.open("wb");
		auto fp = file.get_fp();

		if (fp)
		{
			fwrite(statebits, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();
		}
	}

	void techset::dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count)
	{
		const auto path = "techsets\\state\\"s + clean_name(techset) + "\\"s + material + ".statebitsmap";

		ordered_json json_data = {};
		for (unsigned char i = 0; i < count; i++)
		{
			const auto var_x_gfx_globals = get_x_gfx_globals_for_zone<XGfxGlobals>(map[i].zone);
			ordered_json entry;
			entry["loadBits"][0] = map[i].loadBits[0];
			entry["loadBits"][1] = map[i].loadBits[1];
			entry["loadBits"][2] = map[i].loadBits[2];
			entry["loadBits"][3] = map[i].loadBits[3];
			entry["loadBits"][4] = map[i].loadBits[4];
			entry["loadBits"][5] = map[i].loadBits[5];
			for (int j = 0; j < 14; j++)
			{
				entry["depthStencilStateBits"][j] = var_x_gfx_globals ? var_x_gfx_globals->depthStencilStateBits[map[i].depthStencilState[j]] : 0;
			}
			for (int j = 0; j < 4; j++)
			{
				entry["blendStateBits"][j] = var_x_gfx_globals ? var_x_gfx_globals->blendStateBits[map[i].blendState][j] : 0;
			}
			entry["rasterizerState"] = map[i].rasterizerState;
			json_data[i] = entry;
		}

		auto file = filesystem::file(path);
		file.open("wb");
		auto fp = file.get_fp();
		if (fp)
		{
			const auto json_dump = json_data.dump(4);
			file.write(json_dump.data(), json_dump.size(), 1);
			file.close();
		}
	}

	void techset::dump_technique(MaterialTechnique* asset)
	{
		const auto path = "techsets\\"s + asset->hdr.name + ".technique";

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(&asset->hdr);
		dumper.dump_array(asset->passArray, asset->hdr.passCount);

		dumper.dump_string(asset->hdr.name);
		for (unsigned short i = 0; i < asset->hdr.passCount; i++)
		{
			if (asset->passArray[i].vertexShader)
			{
				dumper.dump_asset(asset->passArray[i].vertexShader);
				//vertex_shader::dump(asset->passArray[i].vertexShader);
			}

			if (asset->passArray[i].vertexDecl)
			{
				dumper.dump_asset(asset->passArray[i].vertexDecl);
				vertex_decl::dump(asset->passArray[i].vertexDecl);
			}

			if (asset->passArray[i].hullShader)
			{
				dumper.dump_asset(asset->passArray[i].hullShader);
				//hull_shader::dump(asset->passArray[i].hullShader);
			}

			if (asset->passArray[i].domainShader)
			{
				dumper.dump_asset(asset->passArray[i].domainShader);
				//domain_shader::dump(asset->passArray[i].domainShader);
			}

			if (asset->passArray[i].pixelShader)
			{
				dumper.dump_asset(asset->passArray[i].pixelShader);
				//pixel_shader::dump(asset->passArray[i].pixelShader);
			}

			if (asset->passArray[i].args)
			{
				dumper.dump_array(asset->passArray[i].args, asset->passArray[i].perObjArgCount + asset->passArray[i].perPrimArgCount + asset->passArray[i].stableArgCount);
				for (auto arg = 0; arg < asset->passArray[i].perObjArgCount + asset->passArray[i].perPrimArgCount + asset->passArray[i].stableArgCount; arg++)
				{
					if (asset->passArray[i].args[arg].type == 4)
					{
						dumper.dump_array(const_cast<float*>(asset->passArray[i].args[arg].u.literalConst), 4);
					}
				}
			}
		}

		dumper.close();
	}

	void yeet(MaterialTechniqueSet* asset)
	{
		const auto path = "techsets\\"s + clean_name(asset->name) + ".techset.txt";

		filesystem::file file(path);
		file.open("wb");
		auto fp = file.get_fp();

		if (!fp)
		{
			__debugbreak();
		}

		unsigned int index = 0u;
		for (auto i = 0u; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
		{
			if (Material_TechSetHasTechnique(asset, (MaterialTechniqueType)i) && asset->techniques[index])
			{
				fprintf(fp, "%i: %s\n", i, asset->techniques[index++]->hdr.name);
			}
			else
			{
				fprintf(fp, "%i: nullptr\n", i);
			}
		}

		file.close();
	}

	void techset::dump(MaterialTechniqueSet* asset)
	{
		const auto path = "techsets\\"s + clean_name(asset->name) + ".techset";

		yeet(asset);

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);

		for (auto i = 0u; i < asset->techniqueCount; i++)
		{
			dumper.dump_string(asset->techniques[i]->hdr.name);
			techset::dump_technique(asset->techniques[i]);
		}

		dumper.close();
	}

	namespace shader
	{
		const char* parse_debug_name(const char* name, zone_memory* mem)
		{
			const auto path = utils::string::va("techsets\\dbg\\%s", name);

			filesystem::file file(path);
			file.open("rb");
			if (!file.get_fp())
			{
				return name;
			}

			const auto data = file.read_bytes(file.size());
			file.close();

			return mem->duplicate_string({ data.begin(), data.end() });
		}

		void dump_debug_name(const std::string& name, const std::string& debug_name)
		{
			if (name == debug_name)
			{
				return;
			}

			const auto path = utils::string::va("techsets\\dbg\\%s", name.data());

			filesystem::file file(path);
			file.open("wb");
			file.write(debug_name.data(), debug_name.size());
			file.close();
		}
	}
}