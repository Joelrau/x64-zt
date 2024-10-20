#include <std_include.hpp>
#include "techset.hpp"

#include "vertexshader.hpp"
#include "vertexdecl.hpp"
#include "hullshader.hpp"
#include "domainshader.hpp"
#include "pixelshader.hpp"

#include "zonetool/iw6/zonetool.hpp"

#define DEEP_LOOK_TECHNIQUES

namespace zonetool::h1
{
	namespace material_data
	{
#ifdef DEEP_LOOK_TECHNIQUES
		namespace
		{
			struct mdata_s
			{
				std::string name;
				bool has_dir;
				bool has_spot;
				bool has_omni;
			};

			bool is_better_option(mdata_s& first, mdata_s& second)
			{
				auto count1 = first.has_dir + first.has_spot + first.has_omni;
				auto count2 = second.has_dir + second.has_spot + second.has_omni;
				
				return count1 > count2;
			}

			bool is_best_option(mdata_s& data)
			{
				return data.has_dir && data.has_spot && data.has_omni;
			}

			mdata_s get_file_info(const std::string& type, const std::string& file_path)
			{
				mdata_s info{};

				std::filesystem::path file_ = file_path;

				if (type == "state")
				{
					file_.replace_extension("statebits");
				}
				else if (type == "contantbuffer")
				{
					file_.replace_extension("cbi");
				}

				unsigned char data[MaterialTechniqueType::TECHNIQUE_COUNT]{};
				//memset(data, 0xFF, MaterialTechniqueType::TECHNIQUE_COUNT);

				auto file = filesystem::file(file_.string());
				file.open("rb", false);
				auto fp = file.get_fp();

				if (fp)
				{
					fread(data, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
					file.close();
				}

				info.name = file_path;
				info.has_dir = data[MaterialTechniqueType::TECHNIQUE_LIT_DIR] != 0xFF;
				info.has_spot = data[MaterialTechniqueType::TECHNIQUE_LIT_SPOT] != 0xFF;
				info.has_omni = data[MaterialTechniqueType::TECHNIQUE_LIT_OMNI] != 0xFF;

				return info;
			}

			std::optional<mdata_s> find_best_file_with_extension_in_directory(const std::string& directory, const std::string& extension, const std::string& type)
			{
				if (!std::filesystem::exists(directory))
				{
					return {};
				}

				mdata_s best_file{};
				for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
				{
					if (std::filesystem::is_regular_file(entry) && entry.path().extension() == extension)
					{
						auto info = get_file_info(type, entry.path().string());

						if (best_file.name.empty() || is_better_option(info, best_file))
						{
							best_file = info;
						}

						if (is_best_option(best_file))
						{
							return best_file;
						}
					}
				}

				if (!best_file.name.empty())
				{
					return best_file;
				}

				return {};
			}
		}
#endif

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

		std::string get_parse_path(const std::string& type, const std::string& ext, const std::string& techset, const std::string& material, bool* use_path)
		{
			const std::string parent_path = utils::string::va("techsets\\%s\\%s", type.data(), techset.data());
			const std::string file = utils::string::va("%s%s", material.data(), ext.data());
			std::string path = parent_path + "\\" + file;

			if (filesystem::file(path).exists())
			{
				return path;
			}

#ifdef DEEP_LOOK_TECHNIQUES
			// get best one from the directories
			{
				mdata_s best_file{};
				for (const auto& parse_path : filesystem::get_search_paths())
				{
					const std::string dir = parse_path + parent_path;
					const auto best_file_in_dir = find_best_file_with_extension_in_directory(dir, ext, type);
					if (best_file_in_dir.has_value())
					{
						auto best_file_ = best_file_in_dir.value();
						if (best_file.name.empty() || is_better_option(best_file_, best_file))
						{
							best_file = best_file_;
						}

						if (is_best_option(best_file))
						{
							*use_path = false;
							return best_file.name;
						}
					}
				}
				if (!best_file.name.empty())
				{
					if (!is_best_option(best_file))
					{
						//__debugbreak();
					}
					*use_path = false;
					return best_file.name;
				}
			}
#else
			// get a random one from the directory
			{
				for (const auto& parse_path : filesystem::get_search_paths())
				{
					const std::string dir = parse_path + parent_path;
					const auto first_file = find_first_file_with_extension_in_directory(dir, ext);
					if (first_file.has_value() && !first_file.value().empty())
					{
						path = parent_path + "\\" + first_file.value();
						return path;
					}
				}
			}
#endif

			return path;
		}
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

	MaterialTechnique* parse_technique_internal(const std::string& name, zone_memory* mem, std::uint32_t index, bool use_path = true)
	{
		const auto path = name;

		assetmanager::reader reader(mem);
		if (!reader.open(path, use_path))
		{
			//ZONETOOL_FATAL("technique \"%s\" is missing.", name.data());
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

	MaterialTechnique* parse_technique(const std::string& name, zone_memory* mem, std::uint32_t index)
	{
		const auto path = "techsets\\" + name + ".technique";
		return parse_technique_internal(path, mem, index);
	}

	MaterialTechniqueSet* techset::parse(const std::string& name, zone_memory* mem)
	{
#ifdef DEEP_LOOK_TECHNIQUES
		bool use_path = true;
		auto spath = material_data::get_parse_path("state", ".statebits", name, "", &use_path);
		std::string parent_path = spath.substr(0, spath.find("\\state"));
		auto path = parent_path + "\\"s + name + ".techset"s;

		assetmanager::reader reader(mem);
		if (!reader.open(path, use_path))
		{
			path = "techsets\\"s + name + ".techset"s;
			if (!reader.open(path, true))
			{
				return nullptr;
			}
		}
#else
		const auto path = "techsets\\" + name + ".techset";
		assetmanager::reader reader(mem);
		if (!reader.open(path))
			if (!reader.open(path))
			{
				return nullptr;
			}
#endif

		ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

		const auto asset = reader.read_single<MaterialTechniqueSet>();
		asset->name = reader.read_string();

		for (auto i = 0u; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
		{
			if (asset->techniques[i])
			{
				asset->techniques[i] = parse_technique(reader.read_string(), mem, i);
			}
		}

#ifdef DEEP_LOOK_TECHNIQUES
		const auto add = [&](MaterialTechniqueType type, MaterialTechniqueType a2, const std::string& str)
		{
			if (!asset->techniques[type])
			{
				if (!asset->techniques[a2])
				{
					return;
				}

				std::string technique_name = asset->techniques[a2]->hdr.name;

				try
				{
					std::regex pattern("_l.0");  // '.' matches any single character between '_l' and '0'
					std::smatch match;

					if (std::regex_search(technique_name, match, pattern)) {
						// If the pattern is found, get the position of the match
						size_t pos = match.position(0);

						// Replace the matched substring with the new string
						technique_name.replace(pos, 4, str);  // '4' is the length of the pattern "_ln0"
					}
					else
					{
						//return;
					}
				}
				catch (const std::runtime_error& err)
				{
					printf("%s", err.what());
					return;
				}

				auto* technique = parse_technique(technique_name, mem, type);
				if (technique)
				{
					asset->techniques[type] = technique;
				}
			}
		};

		// regular

		add(MaterialTechniqueType::TECHNIQUE_LIT, MaterialTechniqueType::TECHNIQUE_LIT, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_DIR, MaterialTechniqueType::TECHNIQUE_LIT, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_LIT, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_LIT, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_LIT, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT, "_lo1");

		add(MaterialTechniqueType::TECHNIQUE_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG, "_lo1");

		// instanced

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, "_lo1");

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, "_lo1");

		// subdiv

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, "_lo1");

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, "_lo1");

		// displacement

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, "_lo1");

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ln0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ld0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ld1");

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ls0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ls1");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_ls2");

		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_lo0");
		add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, "_lo1");
#endif

		if (asset->techniques[0] && asset->techniques[0]->hdr.name == "textured_3d"s)
		{
			ZONETOOL_WARNING("techset \"%s\" is invalid!", name.data());
		}

		reader.close();

		return asset;
	}

	void techset::parse_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* indexes, zone_memory* mem)
	{
		bool use_path = true;
		const auto path = material_data::get_parse_path("constantbuffer", ".cbi", techset, material, &use_path);
		auto file = filesystem::file(path);
		file.open("rb", use_path);
		auto fp = file.get_fp();

		if (fp)
		{
			fread(indexes, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();

#ifdef DEEP_LOOK_TECHNIQUES
			const auto add = [&](MaterialTechniqueType type, MaterialTechniqueType a2 = MaterialTechniqueType::TECHNIQUE_LIT_DIR)
			{
				if (indexes[type] == 0xFF)
				{
					indexes[type] = indexes[a2];
				}
			};

			// regular

			add(MaterialTechniqueType::TECHNIQUE_LIT, MaterialTechniqueType::TECHNIQUE_LIT);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR, MaterialTechniqueType::TECHNIQUE_LIT);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);

			// instanced

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);

			// subdiv

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);

			// displacement

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);
#endif

			return;
		}

		ZONETOOL_FATAL("constantbufferindexes for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::parse_constant_buffer_def_array(const std::string& techset, const std::string& material, 
		MaterialConstantBufferDef** def_ptr, unsigned char* count, zone_memory* mem)
	{
		bool use_path = true;
		const auto path = material_data::get_parse_path("constantbuffer", ".cbt", techset, material, &use_path);
		assetmanager::reader read(mem);
		if (!read.open(path, use_path))
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
		bool use_path = true;
		const auto path = material_data::get_parse_path("state", ".stateinfo", techset, material, &use_path);
		filesystem::file file(path);
		if (file.exists(use_path))
		{
			file.open("rb", use_path);
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
		bool use_path = true;
		const auto path = material_data::get_parse_path("state", ".statebits", techset, material, &use_path);
		auto file = filesystem::file(path);
		file.open("rb", use_path);
		auto fp = file.get_fp();

		if (fp)
		{
			fread(statebits, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();

#ifdef DEEP_LOOK_TECHNIQUES
			const auto add = [&](MaterialTechniqueType type, MaterialTechniqueType a2 = MaterialTechniqueType::TECHNIQUE_LIT_DIR)
			{
				if (statebits[type] == 0xFF)
				{
					statebits[type] = statebits[a2];
				}
			};

			// regular

			add(MaterialTechniqueType::TECHNIQUE_LIT, MaterialTechniqueType::TECHNIQUE_LIT);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR, MaterialTechniqueType::TECHNIQUE_LIT);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_LIT_DIR_SHADOW_DFOG);

			// instanced

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG);

			// subdiv

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG);

			// displacement

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);

			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG);
			add(MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG, MaterialTechniqueType::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG);
#endif

			return;
		}

		ZONETOOL_FATAL("statebits for techset \"%s\", material \"%s\" are missing!", techset.data(), material.data());
	}

	void techset::parse_statebitsmap(const std::string& techset, const std::string& material, GfxStateBits** map, unsigned char* count,
		std::vector<std::array<std::uint64_t, 10>>* dssb,
		std::vector<std::array<std::uint32_t, 3>>* bsb,
		zone_memory* mem)
	{
		bool use_path = true;
		const auto path = material_data::get_parse_path("state", ".statebitsmap", techset, material, &use_path);
		filesystem::file file(path);
		if (file.exists(use_path))
		{
			file.open("rb", use_path);
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

					std::array<std::uint64_t, 10> temp_bits = { 0 };
					for (int j = 0; j < 10; j++)
					{
						temp_bits[j] = stateMap[i]["depthStencilStateBits"][j].get<std::uint64_t>();
					}
					dssb->push_back(std::move(temp_bits));

					std::array<std::uint32_t, 3> temp_bits2;
					for (int j = 0; j < 3; j++)
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
				ZONETOOL_FATAL("techset \"%s\" not found.", name.data());
			}
		}
	}

	void techset::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto data = this->asset_;
		for (auto technique = 0u; technique < MaterialTechniqueType::TECHNIQUE_COUNT; technique++)
		{
			if (data->techniques[technique])
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
	}

	void techset::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		for (auto technique = 0u; technique < MaterialTechniqueType::TECHNIQUE_COUNT; technique++)
		{
			if (data->techniques[technique])
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

		for (auto technique = 0; technique < MaterialTechniqueType::TECHNIQUE_COUNT; technique++)
		{
			if (!data->techniques[technique])
			{
				continue;
			}

			buf->align(3);

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
						ptr = 0xFDFDFDF300000000 + buf->stream_offset(3) + 1;
						add_vertexdecl_pointer(technique_passes[pass].vertexDecl->name, ptr);
						buf->inc_stream(3, 8);
						buf->pop_stream();

						auto vertexDecl = buf->write(data->techniques[technique]->passArray[pass].vertexDecl);

						buf->push_stream(XFILE_BLOCK_VIRTUAL);
						if (data->techniques[technique]->passArray[pass].vertexDecl->name)
						{
							vertexDecl->name = buf->write_str(data->techniques[technique]->passArray[pass].vertexDecl->name);
						}
						buf->pop_stream();

						buf->pop_stream();

						buf->insert_pointer(&technique_passes[pass].vertexDecl);
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
					buf->align(3);
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

			buf->write_str(technique_header->name);
			buf->clear_pointer(&technique_header->name);

			buf->clear_pointer(&dest->techniques[technique]);
		}

		buf->pop_stream();
	}

	void techset::dump_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* cbi)
	{
		const auto path = "techsets\\constantbuffer\\"s + techset + "\\"s + material + ".cbi";
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
		const auto path = "techsets\\constantbuffer\\"s + techset + "\\"s + material + ".cbt";
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
		const auto path = "techsets\\state\\"s + techset + "\\"s + material + ".stateinfo";

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
		const auto path = "techsets\\state\\"s + techset + "\\"s + material + ".statebits";
		auto file = filesystem::file(path);
		file.open("wb");
		auto fp = file.get_fp();

		if (fp)
		{
			fwrite(statebits, MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
			file.close();
		}
	}

	GfxStateBitsRaw parse_statebits_raw_data(const ordered_json& data, const std::string& name)
	{
		std::uint32_t loadbits0 = 0;
		std::uint32_t loadbits1 = 0;
		std::uint32_t loadbits2 = 0; //0xFFFF;
		std::uint32_t loadbits3 = 0;
		std::uint32_t loadbits4 = 0;
		std::uint32_t loadbits5 = 0;

#define READ_INT_LB_FROM_JSON(x) std::uint32_t x = data[#x].get<std::uint32_t>();
#define READ_BOOL_LB_FROM_JSON(x) bool x = data[#x].get<bool>();
#define READ_STR_LB_FROM_JSON(x) std::string x = data[#x].get<std::string>();

		READ_INT_LB_FROM_JSON(srcBlendRgb);
		READ_INT_LB_FROM_JSON(dstBlendRgb);
		READ_INT_LB_FROM_JSON(blendOpRgb);
		READ_INT_LB_FROM_JSON(srcBlendAlpha);
		READ_INT_LB_FROM_JSON(dstBlendAlpha);
		READ_INT_LB_FROM_JSON(blendOpAlpha);
		READ_INT_LB_FROM_JSON(polygonOffset);

		READ_STR_LB_FROM_JSON(alphaTest);
		READ_STR_LB_FROM_JSON(cullFace);
		READ_STR_LB_FROM_JSON(depthTest);

		READ_BOOL_LB_FROM_JSON(colorWriteR);
		READ_BOOL_LB_FROM_JSON(colorWriteG);
		READ_BOOL_LB_FROM_JSON(colorWriteB);
		READ_BOOL_LB_FROM_JSON(colorWriteAlpha);
		READ_BOOL_LB_FROM_JSON(polymodeLine);

		READ_BOOL_LB_FROM_JSON(gammaWrite);
		READ_BOOL_LB_FROM_JSON(depthWrite);
		READ_BOOL_LB_FROM_JSON(depthWriteOpaque);
		READ_BOOL_LB_FROM_JSON(stencilFrontEnabled);
		READ_BOOL_LB_FROM_JSON(stencilBackEnabled);

		READ_INT_LB_FROM_JSON(stencilFrontPass);
		READ_INT_LB_FROM_JSON(stencilFrontFail);
		READ_INT_LB_FROM_JSON(stencilFrontZFail);
		READ_INT_LB_FROM_JSON(stencilFrontFunc);
		READ_INT_LB_FROM_JSON(stencilBackPass);
		READ_INT_LB_FROM_JSON(stencilBackFail);
		READ_INT_LB_FROM_JSON(stencilBackZFail);
		READ_INT_LB_FROM_JSON(stencilBackFunc);
		READ_INT_LB_FROM_JSON(stencilRefBits);

		loadbits3 |= srcBlendRgb << GFXS3_SRCBLEND_RGB_SHIFT;
		loadbits3 |= dstBlendRgb << GFXS3_DSTBLEND_RGB_SHIFT;
		loadbits3 |= blendOpRgb << GFXS3_BLENDOP_RGB_SHIFT;
		loadbits3 |= srcBlendAlpha << GFXS3_SRCBLEND_ALPHA_SHIFT;
		loadbits3 |= dstBlendAlpha << GFXS3_DSTBLEND_ALPHA_SHIFT;
		loadbits3 |= blendOpAlpha << GFXS3_BLENDOP_ALPHA_SHIFT;

		if (depthTest == "always")
		{
			loadbits1 |= GFXS1_DEPTHTEST_ALWAYS;
		}
		else if (depthTest == "less")
		{
			loadbits1 |= GFXS1_DEPTHTEST_LESS;
		}
		else if (depthTest == "equal")
		{
			loadbits1 |= GFXS1_DEPTHTEST_EQUAL;
		}
		else if (depthTest == "lessequal")
		{
			loadbits1 |= GFXS1_DEPTHTEST_LESSEQUAL;
		}
		else if (depthTest == "great")
		{
			loadbits1 |= GFXS1_DEPTHTEST_GREAT;
		}
		else if (depthTest == "greatequal")
		{
			loadbits1 |= GFXS1_DEPTHTEST_GREATEQUAL;
		}
		else if (depthTest == "disable")
		{
			loadbits1 |= GFXS1_DEPTHTEST_DISABLE;
		}
		else
		{
			__debugbreak();
		}

		loadbits1 |= polygonOffset;

		if (alphaTest == "disable")
		{
			loadbits0 |= GFXS0_ATEST_DISABLE;
		}
		else if (alphaTest == ">0")
		{
			loadbits0 |= GFXS0_ATEST_GT_0;
		}
		else if (alphaTest == "<128")
		{
			loadbits0 |= GFXS0_ATEST_LT_128;
		}
		else if (alphaTest == ">=128")
		{
			loadbits0 |= GFXS0_ATEST_GE_128;
		}
		else
		{
			printf("Invalid alphatest loadbit0 '%s' in material %s\n", alphaTest.data(), name.data());
			return{};
		}

		if (cullFace == "none")
		{
			loadbits0 |= GFXS0_CULL_NONE;
		}
		else if (cullFace == "back")
		{
			loadbits0 |= GFXS0_CULL_BACK;
		}
		else if (cullFace == "front")
		{
			loadbits0 |= GFXS0_CULL_FRONT;
		}
		else
		{
			printf("Invalid cullFace loadbit0 '%s' in material %s\n", cullFace.data(), name.data());
			return{};
		}

		if (gammaWrite)
		{
			loadbits0 |= GFXS0_GAMMAWRITE;
		}

		if (colorWriteAlpha)
		{
			loadbits3 |= GFXS3_BLEND_COLORWRITE_A;
		}
		if (colorWriteR)
		{
			loadbits3 |= GFXS3_BLEND_COLORWRITE_R;
		}
		if (colorWriteG)
		{
			loadbits3 |= GFXS3_BLEND_COLORWRITE_G;
		}
		if (colorWriteB)
		{
			loadbits3 |= GFXS3_BLEND_COLORWRITE_B;
		}

		if (polymodeLine)
		{
			loadbits0 |= GFXS0_POLYMODE_LINE;
		}

		if (depthWrite)
		{
			loadbits1 |= GFXS1_DEPTHWRITE;
		}
		if (depthWriteOpaque)
		{
			loadbits3 |= GFXS3_DEPTHWRITE_OPAQUE;
		}

		if (stencilFrontEnabled)
		{
			loadbits1 |= GFXS1_STENCIL_FRONT_ENABLE;
		}
		if (stencilBackEnabled)
		{
			loadbits1 |= GFXS1_STENCIL_BACK_ENABLE;
		}

		loadbits1 |= stencilFrontPass << GFXS1_STENCIL_FRONT_PASS_SHIFT;
		loadbits1 |= stencilFrontFail << GFXS1_STENCIL_FRONT_FAIL_SHIFT;
		loadbits1 |= stencilFrontZFail << GFXS1_STENCIL_FRONT_ZFAIL_SHIFT;
		loadbits1 |= stencilFrontFunc << GFXS1_STENCIL_FRONT_FUNC_SHIFT;
		loadbits1 |= stencilBackPass << GFXS1_STENCIL_BACK_PASS_SHIFT;
		loadbits1 |= stencilBackFail << GFXS1_STENCIL_BACK_FAIL_SHIFT;
		loadbits1 |= stencilBackZFail << GFXS1_STENCIL_BACK_ZFAIL_SHIFT;
		loadbits1 |= stencilBackFunc << GFXS1_STENCIL_BACK_FUNC_SHIFT;

		loadbits2 |= stencilRefBits;

		GfxStateBitsRaw bits{};
		bits.loadBits[0] = loadbits0;
		bits.loadBits[1] = loadbits1;
		bits.loadBits[2] = loadbits2;
		bits.loadBits[3] = loadbits3;
		bits.loadBits[4] = loadbits4;
		bits.loadBits[5] = loadbits5;

		return bits;
	}

	void generate_statebits_raw_data(GfxStateBits* statebits, ordered_json& data, const std::string& name)
	{
		GfxStateBitsRaw entry{};
		entry.flags.loadbit0 = static_cast<GfxStateBitRawBits0>(statebits->loadBits[GFXS_RAWBITS_CULL_INDEX]);
		entry.flags.loadbit1 = static_cast<GfxStateBitRawBits1>(statebits->loadBits[GFXS_RAWBITS_DEPTH_STENCIL_INDEX]);
		entry.flags.loadbit2 = static_cast<GfxStateBitRawBits2>(statebits->loadBits[GFXS_RAWBITS_UNK2_INDEX]);
		entry.flags.loadbit3 = static_cast<GfxStateBitRawBits3>(statebits->loadBits[GFXS_RAWBITS_BLEND_INDEX]);
		entry.flags.loadbit4 = static_cast<GfxStateBitRawBits4>(statebits->loadBits[GFXS_RAWBITS_UNK4_INDEX]);
		entry.flags.loadbit5 = static_cast<GfxStateBitRawBits5>(statebits->loadBits[GFXS_RAWBITS_UNK5_INDEX]);

		const auto srcBlendRgb = (entry.flags.loadbit3 & GFXS3_SRCBLEND_RGB_MASK) >> GFXS3_SRCBLEND_RGB_SHIFT;
		const auto dstBlendRgb = (entry.flags.loadbit3 & GFXS3_DSTBLEND_RGB_MASK) >> GFXS3_DSTBLEND_RGB_SHIFT;
		const auto blendOpRgb = (entry.flags.loadbit3 & GFXS3_BLENDOP_RGB_MASK) >> GFXS3_BLENDOP_RGB_SHIFT;
		const auto srcBlendAlpha = (entry.flags.loadbit3 & GFXS3_SRCBLEND_ALPHA_MASK) >> GFXS3_SRCBLEND_ALPHA_SHIFT;
		const auto dstBlendAlpha = (entry.flags.loadbit3 & GFXS3_DSTBLEND_ALPHA_MASK) >> GFXS3_DSTBLEND_ALPHA_SHIFT;
		const auto blendOpAlpha = (entry.flags.loadbit3 & GFXS3_BLENDOP_ALPHA_MASK) >> GFXS3_BLENDOP_ALPHA_SHIFT;

		const auto polygonOffset = (entry.flags.loadbit1 & GFXS1_POLYGON_OFFSET_MASK);

		const auto* depthTest = "always";
		if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_LESS)
		{
			depthTest = "less";
		}
		else if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_EQUAL)
		{
			depthTest = "equal";
		}
		else if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_LESSEQUAL)
		{
			depthTest = "lessequal";
		}
		else if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_GREAT)
		{
			depthTest = "great";
		}
		else if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_GREATEQUAL)
		{
			depthTest = "greatequal";
		}
		else if ((entry.flags.loadbit1 & GFXS1_DEPTHTEST_MASK) == GFXS1_DEPTHTEST_DISABLE)
		{
			depthTest = "disable";
		}

		const auto* alphaTest = "disable";
		if ((entry.flags.loadbit0 & GFXS0_ATEST_MASK) == GFXS0_ATEST_GE_128)
		{
			alphaTest = ">=128";
		}
		else if ((entry.flags.loadbit0 & GFXS0_ATEST_MASK) == GFXS0_ATEST_GT_0)
		{
			alphaTest = ">0";
		}
		else if ((entry.flags.loadbit0 & GFXS0_ATEST_MASK) == GFXS0_ATEST_LT_128)
		{
			alphaTest = "<128";
		}
		else
		{
			assert(entry.flags.loadbit0 & GFXS0_ATEST_DISABLE);
		}

		const auto* cullFace = "none";
		if ((entry.flags.loadbit0 & GFXS0_CULL_MASK) == GFXS0_CULL_BACK)
		{
			cullFace = "back";
		}
		else if ((entry.flags.loadbit0 & GFXS0_CULL_MASK) == GFXS0_CULL_FRONT)
		{
			cullFace = "front";
		}
		else
		{
			assert((entry.flags.loadbit0 & GFXS0_CULL_MASK) == GFXS0_CULL_NONE);
		}

		const auto colorWriteR = entry.flags.loadbit3 & GFXS3_BLEND_COLORWRITE_R ? true : false;
		const auto colorWriteG = entry.flags.loadbit3 & GFXS3_BLEND_COLORWRITE_G ? true : false;
		const auto colorWriteB = entry.flags.loadbit3 & GFXS3_BLEND_COLORWRITE_B ? true : false;
		const auto colorWriteAlpha = entry.flags.loadbit3 & GFXS3_BLEND_COLORWRITE_A ? true : false;
		const auto polymodeLine = entry.flags.loadbit0 & GFXS0_POLYMODE_LINE ? true : false;
		const auto gammaWrite = entry.flags.loadbit0 & GFXS0_GAMMAWRITE ? true : false;
		const auto depthWrite = (entry.flags.loadbit1 & GFXS1_DEPTHWRITE) ? true : false;
		const auto depthWriteOpaque = (entry.flags.loadbit3 & GFXS3_DEPTHWRITE_OPAQUE) ? true : false;
		const auto stencilFrontEnabled = (entry.flags.loadbit1 & GFXS1_STENCIL_FRONT_ENABLE) ? true : false;
		const auto stencilBackEnabled = (entry.flags.loadbit1 & GFXS1_STENCIL_BACK_ENABLE) ? true : false;
		const auto stencilFrontPass = (entry.flags.loadbit1 >> GFXS1_STENCIL_FRONT_PASS_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilFrontFail = (entry.flags.loadbit1 >> GFXS1_STENCIL_FRONT_FAIL_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilFrontZFail = (entry.flags.loadbit1 >> GFXS1_STENCIL_FRONT_ZFAIL_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilFrontFunc = (entry.flags.loadbit1 >> GFXS1_STENCIL_FRONT_FUNC_SHIFT) & GFXS_STENCILFUNC_MASK;
		const auto stencilBackPass = (entry.flags.loadbit1 >> GFXS1_STENCIL_BACK_PASS_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilBackFail = (entry.flags.loadbit1 >> GFXS1_STENCIL_BACK_FAIL_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilBackZFail = (entry.flags.loadbit1 >> GFXS1_STENCIL_BACK_ZFAIL_SHIFT) & GFXS_STENCILOP_MASK;
		const auto stencilBackFunc = (entry.flags.loadbit1 >> GFXS1_STENCIL_BACK_FUNC_SHIFT) & GFXS_STENCILFUNC_MASK;

		const auto stencilRefBits = (entry.flags.loadbit2 & GFXS2_STENCIL_REF_MASK);

#define ADD_TO_JSON(x) data[#x] = x;

		ADD_TO_JSON(alphaTest);
		ADD_TO_JSON(blendOpAlpha);
		ADD_TO_JSON(blendOpRgb);
		ADD_TO_JSON(colorWriteAlpha);
		ADD_TO_JSON(colorWriteR);
		ADD_TO_JSON(colorWriteG);
		ADD_TO_JSON(colorWriteB);
		ADD_TO_JSON(cullFace);
		ADD_TO_JSON(depthTest);
		ADD_TO_JSON(depthWrite);
		ADD_TO_JSON(depthWriteOpaque);
		ADD_TO_JSON(dstBlendAlpha);
		ADD_TO_JSON(dstBlendRgb);
		ADD_TO_JSON(gammaWrite);
		ADD_TO_JSON(polygonOffset);
		ADD_TO_JSON(polymodeLine);
		ADD_TO_JSON(srcBlendAlpha);
		ADD_TO_JSON(srcBlendRgb);
		ADD_TO_JSON(stencilBackEnabled);
		ADD_TO_JSON(stencilBackFail);
		ADD_TO_JSON(stencilBackFunc);
		ADD_TO_JSON(stencilBackPass);
		ADD_TO_JSON(stencilBackZFail);
		ADD_TO_JSON(stencilFrontEnabled);
		ADD_TO_JSON(stencilFrontFail);
		ADD_TO_JSON(stencilFrontFunc);
		ADD_TO_JSON(stencilFrontPass);
		ADD_TO_JSON(stencilFrontZFail);
		ADD_TO_JSON(stencilRefBits);
	}

	void generate_statebits_raw_data_iw6(zonetool::iw6::GfxStateBits* statebits, ordered_json& data, const std::string& name)
	{
		zonetool::iw6::GfxStateBitsRaw entry{};
		entry.flags.loadbit0 = static_cast<zonetool::iw6::GfxStateBitRawBits0>(statebits->loadBits[zonetool::iw6::GFXS_RAWBITS_CULL_INDEX]);
		entry.flags.loadbit1 = static_cast<zonetool::iw6::GfxStateBitRawBits1>(statebits->loadBits[zonetool::iw6::GFXS_RAWBITS_DEPTH_STENCIL_INDEX]);
		entry.flags.loadbit2 = static_cast<zonetool::iw6::GfxStateBitRawBits2>(statebits->loadBits[zonetool::iw6::GFXS_RAWBITS_BLEND_INDEX]);

		const auto srcBlendRgb = (entry.flags.loadbit2 & zonetool::iw6::GFXS_SRCBLEND_RGB_MASK) >> zonetool::iw6::GFXS_SRCBLEND_RGB_SHIFT;
		const auto dstBlendRgb = (entry.flags.loadbit2 & zonetool::iw6::GFXS_DSTBLEND_RGB_MASK) >> zonetool::iw6::GFXS_DSTBLEND_RGB_SHIFT;
		const auto blendOpRgb = (entry.flags.loadbit2 & zonetool::iw6::GFXS_BLENDOP_RGB_MASK) >> zonetool::iw6::GFXS_BLENDOP_RGB_SHIFT;
		const auto srcBlendAlpha = (entry.flags.loadbit2 & zonetool::iw6::GFXS_SRCBLEND_ALPHA_MASK) >> zonetool::iw6::GFXS_SRCBLEND_ALPHA_SHIFT;
		const auto dstBlendAlpha = (entry.flags.loadbit2 & zonetool::iw6::GFXS_DSTBLEND_ALPHA_MASK) >> zonetool::iw6::GFXS_DSTBLEND_ALPHA_SHIFT;
		const auto blendOpAlpha = (entry.flags.loadbit2 & zonetool::iw6::GFXS_BLENDOP_ALPHA_MASK) >> zonetool::iw6::GFXS_BLENDOP_ALPHA_SHIFT;

		const auto polygonOffset = (entry.flags.loadbit1 & zonetool::iw6::GFXS1_POLYGON_OFFSET_MASK);

		const auto* depthTest = "disable";
		if ((entry.flags.loadbit1 & zonetool::iw6::GFXS1_DEPTHTEST_DISABLE) == false)
		{
			depthTest = "always";
			if ((entry.flags.loadbit1 & zonetool::iw6::GFXS1_DEPTHTEST_MASK) == zonetool::iw6::GFXS1_DEPTHTEST_LESS)
			{
				depthTest = "less";
			}
			else if ((entry.flags.loadbit1 & zonetool::iw6::GFXS1_DEPTHTEST_MASK) == zonetool::iw6::GFXS1_DEPTHTEST_EQUAL)
			{
				depthTest = "equal";
			}
			else if ((entry.flags.loadbit1 & zonetool::iw6::GFXS1_DEPTHTEST_MASK) == zonetool::iw6::GFXS1_DEPTHTEST_LESSEQUAL)
			{
				depthTest = "lessequal";
			}
		}

		const auto* alphaTest = "disable";
		if ((entry.flags.loadbit0 & zonetool::iw6::GFXS0_ATEST_MASK) == zonetool::iw6::GFXS0_ATEST_GE_128)
		{
			alphaTest = ">=128";
		}
		else if ((entry.flags.loadbit0 & zonetool::iw6::GFXS0_ATEST_MASK) == zonetool::iw6::GFXS0_ATEST_GT_0)
		{
			alphaTest = ">0";
		}
		else if ((entry.flags.loadbit0 & zonetool::iw6::GFXS0_ATEST_MASK) == zonetool::iw6::GFXS0_ATEST_LT_128)
		{
			alphaTest = "<128";
		}
		else
		{
			assert(entry.flags.loadbit0 & zonetool::iw6::GFXS0_ATEST_DISABLE);
		}

		const auto* cullFace = "none";
		if ((entry.flags.loadbit0 & zonetool::iw6::GFXS0_CULL_MASK) == zonetool::iw6::GFXS0_CULL_BACK)
		{
			cullFace = "back";
		}
		else if ((entry.flags.loadbit0 & zonetool::iw6::GFXS0_CULL_MASK) == zonetool::iw6::GFXS0_CULL_FRONT)
		{
			cullFace = "front";
		}
		else
		{
			assert((entry.flags.loadbit0 & zonetool::iw6::GFXS0_CULL_MASK) == zonetool::iw6::GFXS0_CULL_NONE);
		}

		const auto colorWriteR = entry.flags.loadbit2 & zonetool::iw6::GFXS_BLEND_COLORWRITE_R ? true : false;
		const auto colorWriteG = entry.flags.loadbit2 & zonetool::iw6::GFXS_BLEND_COLORWRITE_G ? true : false;
		const auto colorWriteB = entry.flags.loadbit2 & zonetool::iw6::GFXS_BLEND_COLORWRITE_B ? true : false;
		const auto colorWriteAlpha = entry.flags.loadbit2 & zonetool::iw6::GFXS_BLEND_COLORWRITE_A ? true : false;
		const auto polymodeLine = entry.flags.loadbit0 & zonetool::iw6::GFXS0_POLYMODE_LINE ? true : false;
		const auto gammaWrite = entry.flags.loadbit0 & zonetool::iw6::GFXS0_GAMMAWRITE ? true : false;
		const auto depthWrite = (entry.flags.loadbit1 & zonetool::iw6::GFXS1_DEPTHWRITE) ? true : false;
		const auto depthWriteOpaque = (entry.flags.loadbit2 & zonetool::iw6::GFXS2_DEPTHWRITE_OPAQUE) ? true : false;
		const auto stencilFrontEnabled = (entry.flags.loadbit1 & zonetool::iw6::GFXS1_STENCIL_FRONT_ENABLE) ? true : false;
		const auto stencilBackEnabled = (entry.flags.loadbit1 & zonetool::iw6::GFXS1_STENCIL_BACK_ENABLE) ? true : false;
		const auto stencilFrontPass = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_FRONT_PASS_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilFrontFail = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_FRONT_FAIL_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilFrontZFail = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_FRONT_ZFAIL_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilFrontFunc = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_FRONT_FUNC_SHIFT) & zonetool::iw6::GFXS_STENCILFUNC_MASK;
		const auto stencilBackPass = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_BACK_PASS_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilBackFail = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_BACK_FAIL_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilBackZFail = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_BACK_ZFAIL_SHIFT) & zonetool::iw6::GFXS_STENCILOP_MASK;
		const auto stencilBackFunc = (entry.flags.loadbit1 >> zonetool::iw6::GFXS1_STENCIL_BACK_FUNC_SHIFT) & zonetool::iw6::GFXS_STENCILFUNC_MASK;

		const auto stencilRefBits = (entry.flags.loadbit0 & zonetool::iw6::GFXS0_STENCIL_REF_MASK);

#define ADD_TO_JSON(x) data[#x] = x;

		ADD_TO_JSON(alphaTest);
		ADD_TO_JSON(blendOpAlpha);
		ADD_TO_JSON(blendOpRgb);
		ADD_TO_JSON(colorWriteAlpha);
		ADD_TO_JSON(colorWriteR);
		ADD_TO_JSON(colorWriteG);
		ADD_TO_JSON(colorWriteB);
		ADD_TO_JSON(cullFace);
		ADD_TO_JSON(depthTest);
		ADD_TO_JSON(depthWrite);
		ADD_TO_JSON(depthWriteOpaque);
		ADD_TO_JSON(dstBlendAlpha);
		ADD_TO_JSON(dstBlendRgb);
		ADD_TO_JSON(gammaWrite);
		ADD_TO_JSON(polygonOffset);
		ADD_TO_JSON(polymodeLine);
		ADD_TO_JSON(srcBlendAlpha);
		ADD_TO_JSON(srcBlendRgb);
		ADD_TO_JSON(stencilBackEnabled);
		ADD_TO_JSON(stencilBackFail);
		ADD_TO_JSON(stencilBackFunc);
		ADD_TO_JSON(stencilBackPass);
		ADD_TO_JSON(stencilBackZFail);
		ADD_TO_JSON(stencilFrontEnabled);
		ADD_TO_JSON(stencilFrontFail);
		ADD_TO_JSON(stencilFrontFunc);
		ADD_TO_JSON(stencilFrontPass);
		ADD_TO_JSON(stencilFrontZFail);
		ADD_TO_JSON(stencilRefBits);
	}

	void dump_statebits_map_iw6(const std::string& techset, const std::string& material, zonetool::iw6::GfxStateBits* map, unsigned char count)
	{
		const auto path = "techsets\\state\\"s + techset + "\\"s + material + ".statebitsmap";

		ordered_json json_data = {};
		for (unsigned char i = 0; i < count; i++)
		{
			const auto var_x_gfx_globals = get_x_gfx_globals_for_zone<zonetool::iw6::XGfxGlobals>(map[i].zone);
			ordered_json entry;

			ordered_json raw_bits_data;
			generate_statebits_raw_data_iw6(&map[i], raw_bits_data, material);
			auto converted_bits = parse_statebits_raw_data(raw_bits_data, material); // no need to convert raw_bits_data

			entry["loadBits"][0] = converted_bits.loadBits[0];
			entry["loadBits"][1] = converted_bits.loadBits[1];
			entry["loadBits"][2] = converted_bits.loadBits[2]; //0xFFFF;
			entry["loadBits"][3] = converted_bits.loadBits[3]; // maybe wrong still (white flash)
			entry["loadBits"][4] = converted_bits.loadBits[4];
			entry["loadBits"][5] = converted_bits.loadBits[5];

			//convert_depth_stencil_state_bits
			const auto cdssb = [](std::uint64_t bits) -> std::uint64_t
			{
				std::bitset<64> new_bits(bits);
				std::bitset<64> original_bits(bits);

				for (auto idx = 0; idx < 4; idx++)
				{
					new_bits[idx] = 0;
				}

				std::uint64_t h1_bits = static_cast<std::uint64_t>(new_bits.to_ullong());
				std::uint64_t iw6_bits = static_cast<std::uint64_t>(original_bits.to_ullong());

				const auto convert_bit = [&](std::uint64_t mask, std::uint64_t bit1, std::uint64_t bit2)
				{
					if (mask == 0)
						mask = bit1;

					h1_bits |= ((iw6_bits & mask) == bit1) ? bit2 : 0;
				};

				convert_bit(0, zonetool::iw6::GFXS1_DEPTHWRITE, zonetool::h1::GFXS1_DEPTHWRITE);
				convert_bit(zonetool::iw6::GFXS1_DEPTHTEST_MASK, zonetool::iw6::GFXS1_DEPTHTEST_ALWAYS, zonetool::h1::GFXS1_DEPTHTEST_ALWAYS); // irrevelant
				convert_bit(zonetool::iw6::GFXS1_DEPTHTEST_MASK, zonetool::iw6::GFXS1_DEPTHTEST_LESS, zonetool::h1::GFXS1_DEPTHTEST_LESS);
				convert_bit(zonetool::iw6::GFXS1_DEPTHTEST_MASK, zonetool::iw6::GFXS1_DEPTHTEST_EQUAL, zonetool::h1::GFXS1_DEPTHTEST_EQUAL);
				convert_bit(zonetool::iw6::GFXS1_DEPTHTEST_MASK, zonetool::iw6::GFXS1_DEPTHTEST_LESSEQUAL, zonetool::h1::GFXS1_DEPTHTEST_LESSEQUAL);
				//convert_bit(zonetool::iw6::GFXS1_DEPTHTEST_MASK, zonetool::iw6::GFXS1_DEPTHTEST_MDAO, zonetool::h1::GFXS1_DEPTHTEST_MDAO);
				convert_bit(0, zonetool::iw6::GFXS1_DEPTHTEST_DISABLE, zonetool::h1::GFXS1_DEPTHTEST_DISABLE);

				return h1_bits;
			};

			const auto add_ds_entry = [&](std::uint32_t dsi, std::uint32_t dsi2)
			{
				entry["depthStencilStateBits"][dsi] = var_x_gfx_globals ? cdssb(var_x_gfx_globals->depthStencilStateBits[map[i].depthStencilState[dsi2]]) : 0;
			};
			const auto add_ds_entry_val = [&](std::uint32_t dsi, std::uint64_t val)
			{
				entry["depthStencilStateBits"][dsi] = val;
			};

			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_DEFAULT, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_DEFAULT);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_HUD_OUTLINE_ZFAIL, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_HUD_OUTLINE_ZFAIL);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_MOTION_BLUR_HQ, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_MOTION_BLUR_HQ);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_DEPTH_HACK, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_DEPTH_HACK);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_UNK, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_DEFAULT); // 19872279822925 default
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_FORCE_DEPTH_WRITE, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_FORCE_DEPTH_WRITE);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_FORCE_DEPTH_WRITE_HUD_OUTLINE_ZFAIL, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_FORCE_DEPTH_WRITE_HUD_OUTLINE_ZFAIL);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_INCR_SAT, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_INCR_SAT);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_INCR_SAT_CLEAR_DEPTH, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_INCR_SAT_CLEAR_DEPTH);
			add_ds_entry(zonetool::h1::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_FULL_MASK, zonetool::iw6::GFX_DEPTH_STENCIL_MODE_CACHED_SPOT_STENCIL_FULL_MASK);

			entry["blendStateBits"][0] = converted_bits.loadBits[3];
			entry["blendStateBits"][1] = 0;
			entry["blendStateBits"][2] = 0;

			entry["rasterizerState"] = map[i].rasterizerState; // should be the same

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

	void techset::dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count)
	{
		if (game::get_mode() == game::iw6)
		{
			return dump_statebits_map_iw6(techset, material, (zonetool::iw6::GfxStateBits*)map, count);
		}

		const auto path = "techsets\\state\\"s + techset + "\\"s + material + ".statebitsmap";

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
			for (int j = 0; j < 10; j++)
			{
				entry["depthStencilStateBits"][j] = var_x_gfx_globals ? var_x_gfx_globals->depthStencilStateBits[map[i].depthStencilState[j]] : 0;
			}
			for (int j = 0; j < 3; j++)
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
		const auto path = "techsets\\"s + asset->name + ".techset.txt";

		filesystem::file file(path);
		file.open("wb");
		auto fp = file.get_fp();

		for (auto i = 0u; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
		{
			if (asset->techniques[i])
			{
				fprintf(fp, "%i: %s\n", i, asset->techniques[i]->hdr.name);
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
		const auto path = "techsets\\"s + asset->name + ".techset";

		yeet(asset);

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);

		for (auto i = 0u; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
		{
			if (asset->techniques[i])
			{
				dumper.dump_string(asset->techniques[i]->hdr.name);
				techset::dump_technique(asset->techniques[i]);
			}
		}

		dumper.close();
	}
}