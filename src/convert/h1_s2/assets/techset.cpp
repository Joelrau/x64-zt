#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		const std::unordered_map<std::string_view, std::string_view> s2_prefixes
		{
			{"m", "mo"},
			{"mc", "mco"},
			{"wc", "wc"},
			{"w", "w"},
			{"me", "me"},
			{"mec", "mco"},
			{"mce", "mco"},
		};

		const std::vector<std::pair<std::string_view, std::string_view>> s2_token_renames
		{
			{"sld0", "sl0"},
			{"sd0", "s0"},
		};

		const std::unordered_map<std::string_view, float> token_weights
		{
			{"l", 20.0f},
			{"sm", 5.0f},
			{"lmpb", 4.0f},
			{"ndw", 8.0f},
			{"add", 10.0f},
			{"blend", 10.0f},
			{"replace", 10.0f},
			{"atest", 10.0f},
			{"sky", 30.0f},
			{"b0", 10.0f},
			{"r0", 10.0f},
			{"t0", 10.0f},
			{"ct0", 6.0f},
			{"cltrans", 6.0f},
		};

		const std::unordered_map<std::string_view, std::string_view> default_techsets
		{
			{"mco", "mco_l_sm_r0c0n0s0_nfwpf"},
			{"mo", "mo_l_sm_lmpb_t0c0n0s0_nfwpf"},
			{"wc", "wc_l_sm_lmpb_r0c0n0s0_nfwpf"},
		};

		struct techset_name
		{
			std::string name;
			std::string prefix;
			std::vector<std::string> tokens;
		};

		std::unordered_set<std::string> dumped_techsets;
		std::unordered_set<std::string> dumped_shaders;

		enum class draw_kind
		{
			hidden,
			sky,
			translucent,
			opaque,
		};

		const std::unordered_set<std::string_view> hidden_tokens
		{
			"tools",
			"shadowcaster",
			"distortion",
		};

		// "ndw" is no depth write, "b0" and "ba0" are blended layers
		const std::unordered_set<std::string_view> translucent_tokens
		{
			"ndw",
			"add",
			"blend",
			"multiply",
			"b0",
			"ba0",
		};

		// layer groups mix one and two letter layers, for example "t0c0n0om0" or "b0hb0c0"
		void add_layer_tokens(const std::string& part, std::vector<std::string>& tokens)
		{
			static const std::regex layer_group(R"(^([a-z]+[0-9])+$)");
			static const std::regex layer(R"([a-z]+[0-9])");
			if (!std::regex_match(part, layer_group))
			{
				tokens.emplace_back(part);
				return;
			}

			for (std::sregex_iterator i(part.begin(), part.end(), layer), end; i != end; ++i)
			{
				tokens.emplace_back(i->str());
			}
		}

		techset_name parse_name(std::string name, const bool from_h1)
		{
			if (from_h1)
			{
				for (const auto& [from, to] : s2_token_renames)
				{
					for (auto pos = name.find(from); pos != std::string::npos; pos = name.find(from, pos + to.size()))
					{
						name.replace(pos, from.size(), to);
					}
				}
			}

			techset_name result{name};
			auto parts = utils::string::split(name, '_');
			if (!parts.empty())
			{
				result.prefix = parts.front();
				parts.erase(parts.begin());
			}

			if (from_h1)
			{
				const auto prefix = s2_prefixes.find(result.prefix);
				result.prefix = prefix != s2_prefixes.end() ? std::string(prefix->second) : result.prefix;
			}

			for (const auto& part : parts)
			{
				add_layer_tokens(part, result.tokens);
			}

			return result;
		}

		bool has_any_token(const techset_name& name, const std::unordered_set<std::string_view>& tokens)
		{
			return std::ranges::any_of(name.tokens, [&](const std::string& token)
			{
				return tokens.contains(token);
			});
		}

		draw_kind get_draw_kind(const techset_name& name)
		{
			if (hidden_tokens.contains(name.prefix) || has_any_token(name, hidden_tokens))
			{
				return draw_kind::hidden;
			}

			if (has_any_token(name, {"sky"}))
			{
				return draw_kind::sky;
			}

			return has_any_token(name, translucent_tokens) ? draw_kind::translucent : draw_kind::opaque;
		}

		bool is_model_prefix(const std::string& prefix)
		{
			return prefix.starts_with('m');
		}

		bool lightmap_allowed(const techset_name& source, const techset_name& candidate)
		{
			return !has_any_token(source, {"unlit"}) || !has_any_token(candidate, {"lmpb"});
		}

		float token_weight(const std::string& token)
		{
			const auto weight = token_weights.find(token);
			return weight != token_weights.end() ? weight->second : 2.0f;
		}

		float score(const techset_name& source, const techset_name& candidate)
		{
			auto result = 0.0f;
			for (const auto& token : source.tokens)
			{
				const auto found = std::find(candidate.tokens.begin(), candidate.tokens.end(), token) != candidate.tokens.end();
				result += found ? token_weight(token) * 2.0f : -token_weight(token);
			}

			for (const auto& token : candidate.tokens)
			{
				if (std::find(source.tokens.begin(), source.tokens.end(), token) == source.tokens.end())
				{
					result -= token_weight(token);
				}
			}

			return result;
		}

		std::optional<std::string> find_best(const techset_name& source, const std::function<bool(const techset_name&)>& accept)
		{
			std::optional<std::string> best;
			auto best_score = -std::numeric_limits<float>::infinity();
			for (const auto& name : donor_zone().donor_techsets())
			{
				const auto candidate = parse_name(name, false);
				if (!accept(candidate))
				{
					continue;
				}

				const auto value = score(source, candidate);
				if (value > best_score)
				{
					best_score = value;
					best = name;
				}
			}

			return best;
		}

		template <typename T>
		void dump_shader(T* shader)
		{
			if (!shader || !shader->name || !dumped_shaders.insert(shader->name).second)
			{
				return;
			}

			if constexpr (std::is_same_v<T, zonetool::s2::MaterialVertexShader>)
			{
				zonetool::s2::vertex_shader::dump(shader);
				add_csv_row("vertexshader", shader->name);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialPixelShader>)
			{
				zonetool::s2::pixel_shader::dump(shader);
				add_csv_row("pixelshader", shader->name);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialHullShader>)
			{
				zonetool::s2::hull_shader::dump(shader);
				add_csv_row("hullshader", shader->name);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialDomainShader>)
			{
				zonetool::s2::domain_shader::dump(shader);
				add_csv_row("domainshader", shader->name);
			}
			else
			{
				zonetool::s2::vertex_decl::dump(shader);
				add_csv_row("vertexdecl", shader->name);
			}
		}
	}

	s2_zone::s2_zone(const std::filesystem::path& path)
		: database_(db_create())
	{
		db_scope _(*this->database_);
		this->zone_ = zonetool::s2::load_zone(path);

		for (const auto& asset : this->zone_->assets)
		{
			if (!asset.referenced)
			{
				this->assets_[asset.type].emplace_back(asset.header);
			}

			if (asset.type == zonetool::s2::ASSET_TYPE_TECHNIQUE_SET)
			{
				const auto techset = static_cast<zonetool::s2::MaterialTechniqueSet*>(asset.header);
				this->techsets_.try_emplace(techset->name, techset);
				if (asset.referenced)
				{
					this->global_techsets_.insert(techset->name);
				}
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_IMAGE && asset.referenced)
			{
				this->global_images_.insert(static_cast<zonetool::s2::GfxImage*>(asset.header)->name);
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_XMODEL_BASE && !asset.referenced && !this->model_base_donor_)
			{
				const auto base = static_cast<zonetool::s2::XModelBase*>(asset.header);
				if (base->numBones == 1 && !base->physRigCount && !base->cloth && !base->rig && !base->imageRefCount)
				{
					this->model_base_donor_ = base;
				}
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_MATERIAL && !asset.referenced)
			{
				const auto material = static_cast<zonetool::s2::Material*>(asset.header);
				if (!material->techniqueSet || !material->techniqueSet->name || !material->textureTable)
				{
					continue;
				}

				auto& donor = this->donors_[material->techniqueSet->name];
				if (!donor || (donor->parentMaterial && !material->parentMaterial))
				{
					donor = material;
				}
			}
		}
	}

	void* s2_zone::find_asset(const zonetool::s2::XAssetType type) const
	{
		const auto& assets = this->find_assets(type);
		return assets.empty() ? nullptr : assets.front();
	}

	const std::vector<void*>& s2_zone::find_assets(const zonetool::s2::XAssetType type) const
	{
		static const std::vector<void*> none;
		const auto entry = this->assets_.find(type);
		return entry != this->assets_.end() ? entry->second : none;
	}

	zonetool::s2::Material* s2_zone::find_donor(const std::string& techset) const
	{
		const auto donor = this->donors_.find(techset);
		return donor != this->donors_.end() ? donor->second : nullptr;
	}

	zonetool::s2::MaterialTechniqueSet* s2_zone::find_techset(const std::string& techset) const
	{
		const auto entry = this->techsets_.find(techset);
		return entry != this->techsets_.end() ? entry->second : nullptr;
	}

	bool s2_zone::is_global_techset(const std::string& techset) const
	{
		return this->global_techsets_.contains(techset);
	}

	bool s2_zone::is_global_image(const std::string& image) const
	{
		return this->global_images_.contains(image);
	}

	std::vector<std::string> s2_zone::donor_techsets() const
	{
		std::vector<std::string> result;
		for (const auto& [name, _] : this->donors_)
		{
			result.emplace_back(name);
		}

		std::sort(result.begin(), result.end());
		return result;
	}

	zonetool::s2::XModelBase* s2_zone::model_base_donor() const
	{
		return this->model_base_donor_;
	}

	database& s2_zone::db() const
	{
		return *this->database_;
	}

	const s2_zone& donor_zone()
	{
		static const s2_zone zone(s2_folder() / "zone" / (std::string(donor_zone_name) + ".ff"));
		return zone;
	}

	std::string select(const std::string& h1_techset)
	{
		const auto source = parse_name(h1_techset, true);
		const auto source_kind = get_draw_kind(source);
		const auto same_kind_and_prefix = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == source_kind && candidate.prefix == source.prefix && lightmap_allowed(source, candidate);
		};

		const auto hidden_same_geometry = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == draw_kind::hidden && is_model_prefix(candidate.prefix) == is_model_prefix(source.prefix);
		};
		const auto hidden = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == draw_kind::hidden;
		};

		const auto drawn_same_prefix = [&](const techset_name& candidate)
		{
			const auto kind = get_draw_kind(candidate);
			return kind != draw_kind::hidden && kind != draw_kind::sky && candidate.prefix == source.prefix && lightmap_allowed(source, candidate);
		};

		const auto filters = source_kind == draw_kind::hidden
			? std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, hidden_same_geometry, hidden}
			: std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, drawn_same_prefix};

		for (const auto& accept : filters)
		{
			if (const auto best = find_best(source, accept))
			{
				return *best;
			}
		}

		const auto fallback = default_techsets.find(source.prefix);
		return fallback != default_techsets.end() ? std::string(fallback->second) : std::string(default_techsets.at("mco"));
	}

	void dump(zonetool::s2::MaterialTechniqueSet* asset)
	{
		if (!dumped_techsets.insert(asset->name).second)
		{
			return;
		}

		zonetool::s2::techset::dump(asset);
		for (const auto* technique : asset->techniques)
		{
			for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				dump_shader(pass.vertexShader);
				dump_shader(pass.vertexDecl);
				dump_shader(pass.hullShader);
				dump_shader(pass.domainShader);
				dump_shader(pass.pixelShader);
			}
		}
	}
}
