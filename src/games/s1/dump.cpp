#include <std_include.hpp>
#include "s1.hpp"
#include "zone/loader.hpp"

#include <game.hpp>

namespace zonetool::s1
{
	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"physpreset",
			"phys_collmap",
			"physwaterpreset",
			"phys_worldmap",
			"physconstraint",
			"xanim",
			"xmodelsurfs",
			"xmodel",
			"material",
			"computeshader",
			"vertexshader",
			"hullshader",
			"domainshader",
			"pixelshader",
			"vertexdecl",
			"techset",
			"image",
			"sound",
			"soundsubmix",
			"sndcurve",
			"lpfcurve",
			"reverbsendcurve",
			"sndcontext",
			"loaded_sound",
			"col_map_mp",
			"com_map",
			"glass_map",
			"aipaths",
			"vehicle_track",
			"map_ents",
			"fx_map",
			"gfx_map",
			"lightdef",
			"ui_map",
			"font",
			"menufile",
			"menu",
			"animclass",
			"localize",
			"attachment",
			"weapon",
			"snddriverglobals",
			"fx",
			"impactfx",
			"surfacefx",
			"aitype",
			"mptype",
			"character",
			"xmodelalias",
			"rawfile",
			"scriptfile",
			"stringtable",
			"leaderboarddef",
			"structureddatadef",
			"tracer",
			"vehicle",
			"addon_map_ents",
			"netconststrings",
			"reverbpreset",
			"luafile",
			"scriptable",
			"equipsndtable",
			"vectorfield",
			"dopplerpreset",
			"particlesimanimation",
			"laser",
			"skeletonscript",
			"clut",
		};

		static_assert(std::size(asset_type_names) == ASSET_TYPE_COUNT);

		void dump_asset(const loaded_asset& asset)
		{
			switch (asset.type)
			{
#define DUMP_ASSET(type, asset_class, struct_name) \
			case type: \
				asset_class::dump(static_cast<struct_name*>(asset.header)); \
				break;
				S1_ASSETS(DUMP_ASSET)
#undef DUMP_ASSET
			default:
				break;
			}
		}

		bool is_dumped_type(const XAssetType type)
		{
			return type != ASSET_TYPE_VERTEXDECL;
		}

		enum class map_zone_dump
		{
			none,
			referenced,
			all,
		};

		struct map_zone
		{
			std::string name;
			map_zone_dump dump;
		};

		struct map_asset_list
		{
			std::vector<loaded_asset> assets;
			std::unordered_map<std::pair<std::uint32_t, std::string>, std::size_t, pair_hash<std::uint32_t, std::string>> indices;

			void add(const loaded_asset& asset)
			{
				const auto [entry, inserted] = this->indices.try_emplace({asset.type, get_asset_name(asset.type, asset.header)},
					this->assets.size());
				if (inserted)
				{
					this->assets.emplace_back(asset);
				}
				else if (!asset.referenced)
				{
					this->assets[entry->second] = asset;
				}
			}

			bool is_last_copy(const loaded_asset& asset) const
			{
				const auto entry = this->indices.find({asset.type, get_asset_name(asset.type, asset.header)});
				return entry != this->indices.end() && this->assets[entry->second].header == asset.header;
			}
		};

		std::string common_zone_suffix(const std::string& map)
		{
			if (map.starts_with("mp_"))
			{
				return "_mp";
			}

			if (map.starts_with("zm_"))
			{
				return "_zm_mp";
			}

			return {};
		}

		std::vector<map_zone> map_zones(const std::string& map)
		{
			const auto suffix = common_zone_suffix(map);
			return
			{
				{"code_post_gfx" + suffix, map_zone_dump::referenced},
				{"patch_code_post_gfx" + suffix, map_zone_dump::referenced},
				{"common_core" + suffix, map_zone_dump::referenced},
				{"patch_common_core" + suffix, map_zone_dump::referenced},
				{"patch_common" + suffix, map_zone_dump::referenced},
				{"common" + suffix, map_zone_dump::referenced},
				{"techsets_common_core_mp", map_zone_dump::referenced},
				{"eng_" + map, map_zone_dump::all},
				{map + "_path", map_zone_dump::all},
				{map + "_load", map_zone_dump::all},
				{map, map_zone_dump::all},
				{"eng_patch_" + map, map_zone_dump::all},
				{"patch_" + map, map_zone_dump::all},
			};
		}

		void add_techset_shaders(const MaterialTechniqueSet* techset, std::unordered_set<const void*>& references)
		{
			for (const auto* technique : techset->techniques)
			{
				if (!technique)
				{
					continue;
				}

				for (auto pass_index = 0u; pass_index < technique->hdr.passCount; pass_index++)
				{
					const auto& pass = technique->passArray[pass_index];
					for (const void* shader : {static_cast<const void*>(pass.vertexShader), static_cast<const void*>(pass.hullShader),
						static_cast<const void*>(pass.domainShader), static_cast<const void*>(pass.pixelShader)})
					{
						if (shader)
						{
							references.insert(shader);
						}
					}
				}
			}
		}

		void add_material_dependencies(const Material* material, std::unordered_set<const void*>& references)
		{
			if (material->techniqueSet && references.insert(material->techniqueSet).second)
			{
				add_techset_shaders(material->techniqueSet, references);
			}

			for (auto i = 0; material->textureTable && i < material->textureCount; i++)
			{
				const auto& texture = material->textureTable[i];
				const auto* image = texture.semantic == TS_WATER_MAP
					? (texture.u.water ? texture.u.water->image : nullptr)
					: texture.u.image;
				if (image)
				{
					references.insert(image);
				}
			}
		}

		void add_xmodel_dependencies(const XModel* model, std::unordered_set<const void*>& references)
		{
			for (auto i = 0; model->materialHandles && i < model->numsurfs; i++)
			{
				if (model->materialHandles[i])
				{
					references.insert(model->materialHandles[i]);
				}
			}

			for (auto i = 0; i < model->numLods; i++)
			{
				if (model->lodInfo[i].modelSurfs)
				{
					references.insert(model->lodInfo[i].modelSurfs);
				}
			}
		}

		void add_fx_dependencies(const FxEffectDef* fx, std::unordered_set<const void*>& references);

		void add_fx_reference(const FxEffectDef* fx, std::unordered_set<const void*>& references)
		{
			if (fx && references.insert(fx).second)
			{
				add_fx_dependencies(fx, references);
			}
		}

		void add_fx_visual_dependencies(const FxElemDef& def, const FxElemVisuals& visual, std::unordered_set<const void*>& references)
		{
			if (!visual.anonymous)
			{
				return;
			}

			switch (def.elemType)
			{
			case FX_ELEM_TYPE_RUNNER:
				add_fx_reference(visual.effectDef.handle, references);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			default:
				references.insert(visual.anonymous);
				break;
			}
		}

		void add_fx_dependencies(const FxEffectDef* fx, std::unordered_set<const void*>& references)
		{
			const auto elem_count = fx->elemDefCountLooping + fx->elemDefCountOneShot + fx->elemDefCountEmission;
			for (auto i = 0; fx->elemDefs && i < elem_count; i++)
			{
				const auto& def = fx->elemDefs[i];
				add_fx_reference(def.effectOnImpact.handle, references);
				add_fx_reference(def.effectOnDeath.handle, references);
				add_fx_reference(def.effectEmitted.handle, references);

				if (def.elemType == FX_ELEM_TYPE_DECAL)
				{
					for (auto j = 0; def.visuals.markArray && j < def.visualCount; j++)
					{
						for (const auto* material : def.visuals.markArray[j].materials)
						{
							if (material)
							{
								references.insert(material);
							}
						}
					}
				}
				else if (def.visualCount > 1)
				{
					for (auto j = 0; def.visuals.array && j < def.visualCount; j++)
					{
						add_fx_visual_dependencies(def, def.visuals.array[j], references);
					}
				}
				else
				{
					add_fx_visual_dependencies(def, def.visuals.instance, references);
				}
			}
		}

		void add_common_dependencies(const std::vector<std::pair<std::unique_ptr<loaded_zone>, map_zone_dump>>& zones,
			std::unordered_set<const void*>& references)
		{
			for (const auto type : {ASSET_TYPE_FX, ASSET_TYPE_XMODEL, ASSET_TYPE_MATERIAL})
			{
				for (const auto& [zone, dump] : zones)
				{
					for (const auto& asset : zone->assets)
					{
						if (dump != map_zone_dump::referenced || asset.referenced || asset.type != type || !references.contains(asset.header))
						{
							continue;
						}

						if (type == ASSET_TYPE_FX)
						{
							add_fx_dependencies(static_cast<const FxEffectDef*>(asset.header), references);
						}
						else if (type == ASSET_TYPE_XMODEL)
						{
							add_xmodel_dependencies(static_cast<const XModel*>(asset.header), references);
						}
						else
						{
							add_material_dependencies(static_cast<const Material*>(asset.header), references);
						}
					}
				}
			}
		}

		std::unordered_set<const void*> collect_map_references(const std::vector<std::pair<std::unique_ptr<loaded_zone>, map_zone_dump>>& zones)
		{
			std::unordered_set<const void*> references;
			for (const auto& [zone, dump] : zones)
			{
				for (const auto& asset : zone->assets)
				{
					if (dump != map_zone_dump::all || !asset.referenced)
					{
						continue;
					}

					references.insert(asset.header);
					if (asset.type == ASSET_TYPE_TECHNIQUE_SET)
					{
						add_techset_shaders(static_cast<const MaterialTechniqueSet*>(asset.header), references);
					}
				}
			}

			add_common_dependencies(zones, references);
			return references;
		}

		bool is_map_zone_asset(const loaded_asset& asset, const map_zone_dump dump, const std::unordered_set<const void*>& references)
		{
			if (asset.referenced || !is_dumped_type(asset.type))
			{
				return false;
			}

			return dump == map_zone_dump::all || (dump == map_zone_dump::referenced && references.contains(asset.header));
		}

		void dump_zone_asset(const loaded_asset& asset, const converter* converter)
		{
			try
			{
				if (converter)
				{
					converter->dump_asset(asset.type, asset.header);
				}
				else
				{
					dump_asset(asset);
				}
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to dump %s \"%s\": %s", type_to_string(asset.type),
					get_asset_name(asset.type, asset.header), e.what());
			}
		}

		std::string asset_csv(const std::vector<loaded_asset>& assets)
		{
			std::string text;
			for (const auto& asset : assets)
			{
				text += std::format("{},{}{}\n", type_to_string(asset.type),
					asset.referenced ? "," : "", get_asset_name(asset.type, asset.header));
			}

			return text;
		}

		void write_asset_csv(const std::string& name, const std::vector<loaded_asset>& assets, const converter* converter)
		{
			const auto text = asset_csv(assets);

			const auto write = [&](const std::string& file_name)
			{
				filesystem::file csv(file_name);
				csv.open("wb");
				csv.write(text);
				csv.close();
			};

			write(name + ".csv");
			if (converter)
			{
				write(std::format("{}.{}.csv", name, converter->source));
			}
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::string& fastfile)
		{
			filesystem::set_zone_folder(zone_folder);
			filesystem::set_fastfile(fastfile);
			filesystem::set_language("english", "eng");
		}
	}

	const char* type_to_string(const XAssetType type)
	{
		return asset_type_names[type];
	}

	std::int32_t type_to_int(const std::string& type)
	{
		if (type == "col_map_sp")
		{
			return ASSET_TYPE_CLIPMAP;
		}

		for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			if (asset_type_names[i] == type)
			{
				return i;
			}
		}

		return -1;
	}

	bool is_valid_asset_type(const std::string& type)
	{
		return type_to_int(type) >= 0;
	}

	XAssetHeader db_find_x_asset_header(const XAssetType type, const char* name, const int create_default)
	{
		return zonetool::db_find_x_asset_header<XAssetHeader>(type, name, create_default);
	}

	XAssetHeader db_find_x_asset_header_safe(const XAssetType type, const std::string& name)
	{
		return zonetool::db_find_x_asset_header_safe<XAssetHeader, XAssetEntry>(type, name);
	}

	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		ZONETOOL_INFO("Loading zone \"%s\"...", zone_path.stem().string().data());
		const auto zone = load_zone(zone_path);

		ZONETOOL_INFO("Dumping %zu assets...", zone->assets.size());
		for (const auto& asset : zone->assets)
		{
			if (asset.referenced || !is_dumped_type(asset.type))
			{
				continue;
			}

			dump_zone_asset(asset, converter);
		}

		write_asset_csv(zone->name, zone->assets, converter);
		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}
		ZONETOOL_INFO("Zone \"%s\" dumped to %s", zone->name.data(), filesystem::get_dump_path().data());
	}

	void dump_map(const std::filesystem::path& zone_folder, const std::string& map, const converter* converter, const bool skip_common)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, map);

		std::vector<std::pair<std::unique_ptr<loaded_zone>, map_zone_dump>> zones;
		for (const auto& [name, dump] : map_zones(map))
		{
			const auto path = game_files::find_zone(name);
			if (!path)
			{
				ZONETOOL_INFO("Zone \"%s\" not found, skipping", name.data());
				continue;
			}

			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());
			zones.emplace_back(load_zone(*path), skip_common && dump != map_zone_dump::all ? map_zone_dump::none : dump);
		}

		const auto references = collect_map_references(zones);
		map_asset_list map_assets;
		for (const auto& [zone, dump] : zones)
		{
			for (const auto& asset : zone->assets)
			{
				if (is_map_zone_asset(asset, dump, references) || dump == map_zone_dump::all)
				{
					map_assets.add(asset);
				}
			}
		}

		for (const auto& [zone, dump] : zones)
		{
			ZONETOOL_INFO("Dumping zone \"%s\"...", zone->name.data());
			for (const auto& asset : zone->assets)
			{
				if (is_map_zone_asset(asset, dump, references) && map_assets.is_last_copy(asset))
				{
					dump_zone_asset(asset, converter);
				}
			}
		}

		write_asset_csv(map, map_assets.assets, converter);
		if (converter && converter->finish)
		{
			converter->finish(map);
		}
		ZONETOOL_INFO("Map \"%s\" dumped to %s", map.data(), filesystem::get_dump_path().data());
	}

	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type_name,
		const std::string& name, const converter* converter)
	{
		const auto type = type_to_int(type_name);
		if (type < 0)
		{
			throw std::runtime_error(std::format("unknown asset type \"{}\"", type_name));
		}

		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		const auto zone = load_zone(zone_path);

		std::vector<const loaded_asset*> assets;
		for (const auto& entry : zone->assets)
		{
			if (entry.type == type && !entry.referenced && (name == "*" || name == get_asset_name(entry.type, entry.header)))
			{
				assets.emplace_back(&entry);
			}
		}

		if (assets.empty())
		{
			throw std::runtime_error(std::format("{} \"{}\" is not in zone \"{}\"", type_name, name, zone->name));
		}

		filesystem::set_dump_folder("assets");
		for (const auto* asset : assets)
		{
			if (converter)
			{
				converter->dump_asset(asset->type, asset->header);
			}
			else
			{
				dump_asset(*asset);
			}
		}

		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}

		ZONETOOL_INFO("%s \"%s\" dumped to %s", type_name.data(), name.data(), filesystem::get_dump_path().data());
	}

	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		load_zone(zone_path);
	}

	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		return asset_csv(load_zone(zone_path)->assets);
	}
}
