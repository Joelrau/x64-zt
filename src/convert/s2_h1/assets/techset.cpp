#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"

#include <io/output_cache.hpp>
#include <utils/io.hpp>

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		constexpr std::string_view global_zones[]
		{
			"code_post_gfx_mp",
			"patch_code_post_gfx_mp",
		};

		const std::unordered_map<std::string_view, std::string_view> s2_prefixes
		{
			{"w", ""},
			{"xv", ""},
			{"xl", ""},
			{"wc", "wc"},
			{"m", "m"},
			{"mo", "m"},
			{"mou", "m"},
			{"mc", "mc"},
			{"mco", "mc"},
			{"mcou", "mc"},
			{"me", "me"},
			{"mec", "mce"},
			{"ml", "ml"},
			{"mlc", "mlc"},
			{"ms", "ms"},
			{"mv", "mv"},
		};

		const std::unordered_set<std::string_view> h1_prefixes
		{
			"m", "mc", "wc", "mv", "mvc", "ml", "mlc", "me", "mce", "ms", "w",
		};

		const std::unordered_map<std::string_view, std::string_view> fallback_prefixes
		{
			{"me", "m"},
			{"ml", "m"},
			{"ms", "m"},
			{"mv", "m"},
			{"mce", "mc"},
			{"mlc", "mc"},
			{"mvc", "mc"},
			{"mc", "m"},
			{"wc", ""},
		};

		const std::unordered_set<std::string_view> model_screen_kinds
		{
			"ui3d", "2d",
		};

		const std::unordered_set<std::string_view> blend_tokens
		{
			"add", "blend", "replace", "atest",
		};

		const std::unordered_map<std::string_view, float> token_weights
		{
			{"l", 20.0f},
			{"sm", 5.0f},
			{"lmpb", 10.0f},
			{"ndw", 8.0f},
			{"add", 10.0f},
			{"blend", 10.0f},
			{"replace", 10.0f},
			{"atest", 10.0f},
			{"frt", 3.0f},
			{"aat", 4.0f},
			{"cltrans", 6.0f},
			{"flag", 15.0f},
			{"flagext", 15.0f},
		};

		struct techset_name
		{
			std::string name;
			std::string prefix;
			std::string kind;
			std::unordered_set<std::string> features;
		};

		struct catalog
		{
			std::unordered_map<std::string, h1_techset_source> sources;
			std::vector<techset_name> names;
			std::unordered_set<std::string> global_images;
			std::unordered_map<std::string, std::filesystem::path> shader_sources;
		};

		struct shader_key
		{
			zonetool::h1::XAssetType type;
			std::string name;
		};

		std::vector<shader_key> missing_shaders;
		std::unordered_set<std::string> missing_shader_keys;
		std::unordered_set<std::string> dumped_shader_keys;

		std::vector<std::pair<std::string, std::uint64_t>> pending_techset_outputs;

		std::filesystem::path h1_folder;

		const catalog& get_catalog();

		bool is_layer_token(const std::string_view token)
		{
			static const std::regex layer(R"(^([a-z]+[0-9]+)+$)");
			static const std::regex excluded(R"(^(wv|z|zdecal)[0-9])");
			const std::string value(token);
			return std::regex_match(value, layer) && !std::regex_search(value, excluded);
		}

		std::string convert_layer_feature(const std::string& feature)
		{
			if (feature == "s0")
			{
				return "sd0";
			}

			if (feature == "sl0")
			{
				return "sld0";
			}

			return feature;
		}

		techset_name parse_name(const std::string& name, const bool is_s2)
		{
			techset_name result{.name = name};

			auto tokens = utils::string::split(name, '_');
			if (tokens.size() > 1)
			{
				if (is_s2)
				{
					if (const auto prefix = s2_prefixes.find(tokens.front()); prefix != s2_prefixes.end())
					{
						result.prefix = prefix->second;
						tokens.erase(tokens.begin());
					}
				}
				else if (h1_prefixes.contains(tokens.front()))
				{
					result.prefix = tokens.front();
					tokens.erase(tokens.begin());
				}
			}

			if (!tokens.empty())
			{
				result.kind = tokens.front();
			}

			// S2 model ui3d and 2d techsets draw an opaque world screen, the H1 techsets of these kinds are code UI techsets without a depth test
			if (is_s2 && result.prefix.starts_with('m') && model_screen_kinds.contains(result.kind))
			{
				result.kind = "unlit";
				result.features.insert({"unlit", "replace", "lin", "nfwpf"});
				tokens.erase(tokens.begin());
			}

			static const std::regex wave_token(R"(^(wv[0-9])+$)");
			static const std::regex layer_feature(R"([a-z]+[0-9]+)");
			for (const auto& token : tokens)
			{
				if (std::regex_match(token, wave_token))
				{
					for (auto i = 0u; i < token.size(); i += 3)
					{
						result.features.insert("W:" + token.substr(i, 3));
					}

					continue;
				}

				if (!is_layer_token(token))
				{
					result.features.insert(token);
					continue;
				}

				for (auto it = std::sregex_iterator(token.begin(), token.end(), layer_feature); it != std::sregex_iterator(); ++it)
				{
					const auto feature = it->str();
					result.features.insert("L:" + (is_s2 ? convert_layer_feature(feature) : feature));
				}
			}

			return result;
		}

		float token_weight(const std::string& token)
		{
			if (const auto weight = token_weights.find(token); weight != token_weights.end())
			{
				return weight->second;
			}

			if (token.starts_with("W:"))
			{
				return 6.0f;
			}

			if (token.starts_with("L:"))
			{
				const auto mode = token[2];
				const auto is_blend_layer = (mode == 'r' || mode == 'b' || mode == 'a') && std::isdigit(token[3]);
				return is_blend_layer ? 8.0f : 3.0f;
			}

			return 1.0f;
		}

		bool has_blend_token(const techset_name& name)
		{
			return std::ranges::any_of(name.features, [](const std::string& token)
			{
				return blend_tokens.contains(token);
			});
		}

		float score(const techset_name& source, const techset_name& candidate)
		{
			const auto source_has_blend = has_blend_token(source);

			auto result = 0.0f;
			for (const auto& token : candidate.features)
			{
				const auto weight = token_weight(token);
				if (source.features.contains(token))
				{
					result += weight;
				}
				else if (source_has_blend && blend_tokens.contains(token))
				{
					result -= 0.5f * weight;
				}
				else
				{
					result -= 1.5f * weight;
				}
			}

			for (const auto& token : source.features)
			{
				if (!candidate.features.contains(token))
				{
					result -= 0.5f * token_weight(token);
				}
			}

			return result;
		}

		std::optional<std::string> select_with_prefix(const techset_name& source, const std::string& prefix)
		{
			const techset_name* best{};
			auto best_score = std::numeric_limits<float>::lowest();
			for (const auto& candidate : get_catalog().names)
			{
				if (candidate.prefix != prefix || candidate.kind != source.kind)
				{
					continue;
				}

				const auto candidate_score = score(source, candidate);
				if (candidate_score > best_score)
				{
					best_score = candidate_score;
					best = &candidate;
				}
			}

			if (!best)
			{
				return {};
			}

			return best->name;
		}

		std::vector<std::filesystem::path> list_techset_zones()
		{
			std::vector<std::filesystem::path> zones;
			for (const auto& zone : global_zones)
			{
				zones.emplace_back(h1_folder / "zone" / (std::string(zone) + ".ff"));
			}

			std::vector<std::filesystem::path> techset_zones;
			for (const auto& entry : std::filesystem::directory_iterator(h1_folder / "zone"))
			{
				const auto filename = entry.path().filename().string();
				if (entry.is_regular_file() && filename.starts_with("techsets_") && filename.ends_with(".ff"))
				{
					techset_zones.emplace_back(entry.path());
				}
			}

			std::ranges::sort(techset_zones);
			zones.insert(zones.end(), techset_zones.begin(), techset_zones.end());
			return zones;
		}

		bool is_global_zone(const std::filesystem::path& zone)
		{
			const auto name = zone.stem().string();
			return std::ranges::find(global_zones, name) != std::end(global_zones);
		}

		bool is_shader_type(const zonetool::h1::XAssetType type)
		{
			return type == zonetool::h1::ASSET_TYPE_VERTEXSHADER || type == zonetool::h1::ASSET_TYPE_HULLSHADER
				|| type == zonetool::h1::ASSET_TYPE_DOMAINSHADER || type == zonetool::h1::ASSET_TYPE_PIXELSHADER;
		}

		std::string get_shader_key(const zonetool::h1::XAssetType type, const std::string& name)
		{
			return std::format("{}:{}", static_cast<int>(type), name);
		}

		void add_zone_to_catalog(catalog& result, const std::filesystem::path& path)
		{
			const auto db = db_create();
			db_scope _(*db);

			const auto zone = zonetool::h1::load_zone(path);
			const auto global = is_global_zone(path);

			std::unordered_set<std::string> zone_techsets;
			for (const auto& asset : zone->assets)
			{
				if (asset.referenced)
				{
					continue;
				}

				if (asset.type == zonetool::h1::ASSET_TYPE_TECHNIQUE_SET)
				{
					zone_techsets.insert(static_cast<zonetool::h1::MaterialTechniqueSet*>(asset.header)->name);
				}
				else if (global && asset.type == zonetool::h1::ASSET_TYPE_IMAGE)
				{
					result.global_images.insert(static_cast<zonetool::h1::GfxImage*>(asset.header)->name);
				}
				else if (is_shader_type(asset.type))
				{
					const auto name = zonetool::h1::get_asset_name(asset.type, asset.header);
					result.shader_sources.try_emplace(get_shader_key(asset.type, name), path);
				}
			}

			for (const auto& asset : zone->assets)
			{
				if (asset.referenced || asset.type != zonetool::h1::ASSET_TYPE_MATERIAL)
				{
					continue;
				}

				const auto material = static_cast<zonetool::h1::Material*>(asset.header);
				if (!material->techniqueSet || !material->techniqueSet->name)
				{
					continue;
				}

				const std::string techset = material->techniqueSet->name;
				if (global || zone_techsets.contains(techset))
				{
					result.sources.try_emplace(techset, h1_techset_source{.zone = path, .global = global});
				}
			}
		}

		const std::filesystem::path catalog_cache_path = "zonetool_cache/h1_techsets.txt";

		std::string zone_signature(const std::vector<std::filesystem::path>& zones)
		{
			auto result = std::format("version {}\n", cache_revision::s2_h1_techset_catalog);
			for (const auto& zone : zones)
			{
				if (std::filesystem::exists(zone))
				{
					result += std::format("z {} {} {}\n", zone.string(), std::filesystem::file_size(zone),
						std::filesystem::last_write_time(zone).time_since_epoch().count());
				}
			}

			return result;
		}

		std::optional<catalog> read_catalog_cache(const std::string& signature)
		{
			std::string data;
			if (!utils::io::read_file(catalog_cache_path.string(), &data) || !data.starts_with(signature))
			{
				return {};
			}

			catalog result;
			std::istringstream stream(data.substr(signature.size()));
			std::string line;
			while (std::getline(stream, line))
			{
				const auto fields = utils::string::split(line, '\t');
				if (fields.size() == 4 && fields[0] == "t")
				{
					result.sources.try_emplace(fields[3], h1_techset_source{.zone = fields[1], .global = fields[2] == "1"});
				}
				else if (fields.size() == 2 && fields[0] == "i")
				{
					result.global_images.insert(fields[1]);
				}
				else if (fields.size() == 3 && fields[0] == "s")
				{
					result.shader_sources.try_emplace(fields[2], fields[1]);
				}
			}

			return result;
		}

		void write_catalog_cache(const std::string& signature, const catalog& result)
		{
			auto data = signature;
			for (const auto& [name, source] : result.sources)
			{
				data += std::format("t\t{}\t{}\t{}\n", source.zone.string(), source.global ? 1 : 0, name);
			}

			for (const auto& image : result.global_images)
			{
				data += std::format("i\t{}\n", image);
			}

			for (const auto& [key, zone] : result.shader_sources)
			{
				data += std::format("s\t{}\t{}\n", zone.string(), key);
			}

			utils::io::write_file(catalog_cache_path.string(), data);
		}

		catalog scan_zones(const std::vector<std::filesystem::path>& zones)
		{
			ZONETOOL_INFO("Reading H1 techsets from \"%s\"...", h1_folder.string().data());

			catalog result;
			for (const auto& zone : zones)
			{
				if (!std::filesystem::exists(zone))
				{
					ZONETOOL_WARNING("H1 zone \"%s\" not found", zone.string().data());
					continue;
				}

				add_zone_to_catalog(result, zone);
			}

			return result;
		}

		catalog build_catalog()
		{
			if (h1_folder.empty())
			{
				throw std::runtime_error("H1 game folder is not set");
			}

			const auto zones = list_techset_zones();
			const auto signature = zone_signature(zones);

			auto result = read_catalog_cache(signature);
			if (!result)
			{
				result = scan_zones(zones);
				write_catalog_cache(signature, *result);
			}

			for (const auto& [name, source] : result->sources)
			{
				result->names.emplace_back(parse_name(name, false));
			}

			std::ranges::sort(result->names, {}, &techset_name::name);

			ZONETOOL_INFO("Found %zu H1 techsets", result->sources.size());
			return std::move(*result);
		}

		template <typename asset_class, typename shader_struct>
		void dump_shader(shader_struct* shader, const zonetool::h1::XAssetType type)
		{
			if (!shader)
			{
				return;
			}

			const auto key = get_shader_key(type, shader->name);
			if (shader->prog.loadDef.program)
			{
				if (dumped_shader_keys.insert(key).second)
				{
					asset_class::dump(shader);
				}

				return;
			}

			if (missing_shader_keys.insert(key).second)
			{
				missing_shaders.emplace_back(type, shader->name);
			}
		}

		void dump_technique_shaders(const zonetool::h1::MaterialTechnique* technique)
		{
			for (auto i = 0; i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				dump_shader<zonetool::h1::vertex_shader>(pass.vertexShader, zonetool::h1::ASSET_TYPE_VERTEXSHADER);
				dump_shader<zonetool::h1::hull_shader>(pass.hullShader, zonetool::h1::ASSET_TYPE_HULLSHADER);
				dump_shader<zonetool::h1::domain_shader>(pass.domainShader, zonetool::h1::ASSET_TYPE_DOMAINSHADER);
				dump_shader<zonetool::h1::pixel_shader>(pass.pixelShader, zonetool::h1::ASSET_TYPE_PIXELSHADER);
			}
		}

		void dump_zone_shaders(const std::filesystem::path& path, const std::unordered_set<std::string>& keys)
		{
			const auto db = db_create();
			db_scope _(*db);

			const auto zone = zonetool::h1::load_zone(path);
			for (const auto& asset : zone->assets)
			{
				if (asset.referenced || !is_shader_type(asset.type)
					|| !keys.contains(get_shader_key(asset.type, zonetool::h1::get_asset_name(asset.type, asset.header))))
				{
					continue;
				}

				switch (asset.type)
				{
				case zonetool::h1::ASSET_TYPE_VERTEXSHADER:
					zonetool::h1::vertex_shader::dump(static_cast<zonetool::h1::MaterialVertexShader*>(asset.header));
					break;
				case zonetool::h1::ASSET_TYPE_HULLSHADER:
					zonetool::h1::hull_shader::dump(static_cast<zonetool::h1::MaterialHullShader*>(asset.header));
					break;
				case zonetool::h1::ASSET_TYPE_DOMAINSHADER:
					zonetool::h1::domain_shader::dump(static_cast<zonetool::h1::MaterialDomainShader*>(asset.header));
					break;
				case zonetool::h1::ASSET_TYPE_PIXELSHADER:
					zonetool::h1::pixel_shader::dump(static_cast<zonetool::h1::MaterialPixelShader*>(asset.header));
					break;
				default:
					break;
				}
			}
		}

		const catalog& get_catalog()
		{
			static const auto result = build_catalog();
			return result;
		}
	}

	h1_zone::h1_zone(const std::filesystem::path& path)
		: database_(db_create())
	{
		db_scope _(*this->database_);
		this->zone_ = zonetool::h1::load_zone(path);

		for (const auto& asset : this->zone_->assets)
		{
			if (asset.referenced)
			{
				continue;
			}

			if (asset.type == zonetool::h1::ASSET_TYPE_TECHNIQUE_SET)
			{
				const auto techset = static_cast<zonetool::h1::MaterialTechniqueSet*>(asset.header);
				this->techsets_.try_emplace(techset->name, techset);
			}
			else if (asset.type == zonetool::h1::ASSET_TYPE_MATERIAL)
			{
				const auto material = static_cast<zonetool::h1::Material*>(asset.header);
				if (material->techniqueSet && material->techniqueSet->name)
				{
					this->donors_.try_emplace(material->techniqueSet->name, material);
				}
			}
		}
	}

	zonetool::h1::Material* h1_zone::find_donor(const std::string& techset) const
	{
		const auto donor = this->donors_.find(techset);
		return donor != this->donors_.end() ? donor->second : nullptr;
	}

	zonetool::h1::MaterialTechniqueSet* h1_zone::find_techset(const std::string& techset) const
	{
		const auto entry = this->techsets_.find(techset);
		return entry != this->techsets_.end() ? entry->second : nullptr;
	}

	database& h1_zone::db() const
	{
		return *this->database_;
	}

	void set_h1_folder(const std::filesystem::path& folder)
	{
		h1_folder = folder;
	}

	std::string select(const std::string& s2_techset)
	{
		if (find_source(s2_techset))
		{
			return s2_techset;
		}

		const auto source = parse_name(s2_techset, true);

		for (auto prefix = std::optional<std::string>(source.prefix); prefix;)
		{
			if (auto result = select_with_prefix(source, *prefix))
			{
				return *result;
			}

			const auto fallback = fallback_prefixes.find(*prefix);
			prefix = fallback != fallback_prefixes.end() ? std::optional<std::string>(fallback->second) : std::nullopt;
		}

		if (!source.prefix.empty())
		{
			if (auto result = select_with_prefix(source, ""))
			{
				return *result;
			}
		}

		return source.prefix.starts_with('m') ? "m_l_sm_lmpb_r0c0n0sd0_nfwpf" : "l_sm_lmpb_r0c0n0sd0_nfwpf";
	}

	const h1_techset_source* find_source(const std::string& h1_techset)
	{
		const auto& sources = get_catalog().sources;
		const auto entry = sources.find(h1_techset);
		return entry != sources.end() ? &entry->second : nullptr;
	}

	bool is_global_image(const std::string& name)
	{
		return get_catalog().global_images.contains(name);
	}

	void dump_missing_shaders()
	{
		std::map<std::filesystem::path, std::unordered_set<std::string>> keys_by_zone;
		for (const auto& [type, name] : missing_shaders)
		{
			const auto key = get_shader_key(type, name);
			if (dumped_shader_keys.contains(key))
			{
				continue;
			}

			const auto& sources = get_catalog().shader_sources;
			const auto source = sources.find(key);
			if (source == sources.end())
			{
				ZONETOOL_WARNING("Shader \"%s\" is not in the H1 techset zones", name.data());
				continue;
			}

			if (!is_global_zone(source->second))
			{
				keys_by_zone[source->second].insert(key);
			}
		}

		for (const auto& [zone, keys] : keys_by_zone)
		{
			ZONETOOL_INFO("Dumping %zu shaders from \"%s\"...", keys.size(), zone.stem().string().data());
			dump_zone_shaders(zone, keys);
		}

		for (const auto& [output, stamp] : pending_techset_outputs)
		{
			update_dump_cache(output, stamp);
		}

		missing_shaders.clear();
		missing_shader_keys.clear();
		dumped_shader_keys.clear();
		pending_techset_outputs.clear();
	}

	void dump(zonetool::h1::MaterialTechniqueSet* asset, const std::filesystem::path& zone_path)
	{
		const auto output = filesystem::get_dump_path() + "techsets\\" + asset->name + ".techset";
		const auto stamp = fingerprint().add_value(cache_revision::s2_h1_techset).add_file_stamp(zone_path).add(asset->name).value();
		if (is_dump_current(output, stamp))
		{
			return;
		}

		zonetool::h1::techset::dump(asset);

		for (const auto technique : asset->techniques)
		{
			if (technique)
			{
				dump_technique_shaders(technique);
			}
		}

		pending_techset_outputs.emplace_back(output, stamp);
	}
}
