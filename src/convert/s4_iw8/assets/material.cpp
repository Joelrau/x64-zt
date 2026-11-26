#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"

#include "gfximage.hpp"
#include "physics.hpp"

#include <iw8/zone/loader.hpp>

#include <io/game_files.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::s4::converter::iw8::material
{
	namespace
	{
		namespace target = zonetool::iw8;

		// TODO: replace the stock IW8 techsets with a DXIL to DXBC shader port
		const char* const stock_zones[] = {"techsets_mp_rust", "techsets_mp_aniyah_tac", "techsets_mp_donetsk"};
		const char* const common_zones[] = {"techsets_common", "techsets_common_mp", "techsets_common_core_mp", "techsets_common_base_mp", "techsets_global_core_mp", "techsets_global_mp", "techsets_global_stream_mp"};

		std::unordered_set<std::string> donor_techsets;
		std::unordered_map<std::string, const stock_material*> dumped_materials;

		const char* get_name(const Material* asset, utils::memory::allocator& allocator)
		{
			const auto name = std::string_view(asset->name);
			const auto star = name.find('*'); // '*' is not a valid file name
			if (star == std::string_view::npos)
			{
				return asset->name;
			}

			return allocator.duplicate_string(std::format("{}generated/{}", name.substr(0, star), name.substr(star + 1)));
		}

		std::size_t find_alpha_token(const std::string_view name)
		{
			for (const auto* alpha : {"_ta0_", "_ta1_", "_tca_", "_tom_"})
			{
				if (const auto pos = name.find(alpha); pos != std::string_view::npos)
				{
					return pos;
				}
			}

			return std::string_view::npos;
		}

		std::string get_techset_key(const std::string_view name)
		{
			const auto pos = find_alpha_token(name);
			return std::string(pos == std::string_view::npos ? name : name.substr(0, pos + 4));
		}

		std::string_view get_techset_blend(const std::string_view name)
		{
			const auto pos = find_alpha_token(name);
			return pos == std::string_view::npos || pos < 4 ? std::string_view() : name.substr(pos - 4, 4);
		}

		std::uint64_t get_techset_features(const std::string_view name)
		{
			const auto pos = find_alpha_token(name);
			auto features = 0ull;
			if (pos != std::string_view::npos)
			{
				std::from_chars(name.data() + pos + 5, name.data() + name.size(), features, 16);
			}

			return features;
		}

		int get_techset_layer_count(const std::string_view name)
		{
			auto pos = find_alpha_token(name);
			for (auto token = 0; pos != std::string_view::npos && token < 4; token++)
			{
				pos = name.find('_', pos + 1);
			}

			auto count = 0;
			if (pos != std::string_view::npos)
			{
				std::from_chars(name.data() + pos + 1, name.data() + name.size(), count);
			}

			return count;
		}

		bool is_vertex_layer_mask_techset(const std::string_view name)
		{
			const auto prefix = name.substr(0, name.find('/'));
			return (prefix == "mco" || prefix == "tmco" || prefix == "tm2co") && get_techset_layer_count(name) >= 2
				&& !(get_techset_features(name) & target::TECHSET_FEATURE_WIND);
		}

		bool has_wind(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name && get_techset_features(asset->techniqueSet->name) & TECHSET_FEATURE_WIND;
		}

		bool reads_transmission_image(const Material* asset)
		{
			if (!asset->techniqueSet || !asset->techniqueSet->name)
			{
				return true;
			}

			const std::string_view name = asset->techniqueSet->name;
			const auto gated = name.find("_lit_alt_") != std::string_view::npos || name.find("_lit_rpl_") != std::string_view::npos;
			return !gated || get_techset_features(name) & TECHSET_FEATURE_TRANSMISSION;
		}

		std::string get_techset_suffix(const std::string_view name)
		{
			const auto slash = name.find('/');
			return std::string(slash == std::string_view::npos ? name : name.substr(slash + 1));
		}

		void count_suffix_uses(std::unordered_map<std::string, std::vector<stock_material>>& materials)
		{
			std::unordered_map<std::string, int> uses;
			for (const auto& [key, list] : materials)
			{
				for (const auto& stock : list)
				{
					uses[get_techset_suffix(stock.techset)]++;
				}
			}

			for (auto& [key, list] : materials)
			{
				for (auto& stock : list)
				{
					stock.suffix_uses = uses[get_techset_suffix(stock.techset)];
				}
			}
		}

		template <typename TechniqueSet>
		render_state get_render_state(const TechniqueSet* techset)
		{
			render_state state{};
			for (std::size_t i = 0; techset->maskedTechniques && i < get_technique_count(techset); i++)
			{
				if (const auto* technique = techset->maskedTechniques[i])
				{
					state.depth_stencil_rasterizer.emplace(static_cast<unsigned short>(technique->stateBits.depthStencilRasterizerBits));
					state.blend.emplace(static_cast<unsigned int>(technique->stateBits.blendBits));
				}
			}

			return state;
		}

		void add_inline_techsets(const target::loaded_zone& zone, std::unordered_map<std::string, render_state>& techsets)
		{
			for (const auto& asset : zone.assets)
			{
				if (!asset.referenced && asset.type == target::ASSET_TYPE_TECHNIQUE_SET)
				{
					const auto* techset = static_cast<const target::MaterialTechniqueSet*>(asset.header);
					techsets.emplace(techset->name, get_render_state(techset));
				}
			}
		}

		std::unordered_map<std::string, std::vector<stock_material>> load_stock_materials()
		{
			std::unordered_map<std::string, std::vector<stock_material>> materials;
			std::unordered_map<std::string, render_state> available_techsets;

			xfile::oodle::load(iw8_folder());
			auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
			const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

			for (const auto* zone_name : stock_zones)
			{
				const auto path = game_files::find_zone(zone_name);
				if (!path)
				{
					ZONETOOL_WARNING("IW8 zone \"%s\" is missing, its techsets are not available", zone_name);
					continue;
				}

				const auto database = db_create();
				db_scope scope(*database);
				const auto zone = target::load_zone(*path);
				add_inline_techsets(*zone, available_techsets);
				for (const auto& asset : zone->assets)
				{
					const auto* material = static_cast<const target::Material*>(asset.header);
					if (asset.referenced || asset.type != target::ASSET_TYPE_MATERIAL || !material->techniqueSet || !material->techniqueSet->name)
					{
						continue;
					}

					stock_material stock{};
					stock.techset = material->techniqueSet->name;
					stock.wind = get_techset_features(stock.techset) & target::TECHSET_FEATURE_WIND;
					stock.layer_count = material->layerCount;
					stock.material_type = material->materialType;
					stock.camera_region = material->cameraRegion;
					stock.sort_key = material->sortKey;
					stock.flags = material->flags;
					for (auto i = 0; material->textureTable && i < material->textureCount; i++)
					{
						const auto& texture = material->textureTable[i];
						stock.textures.push_back({texture.index, texture.image && texture.image->name ? texture.image->name : ""});
					}

					for (auto i = 0; material->constantTable && i < material->constantCount; i++)
					{
						stock.constants.push_back(material->constantTable[i]);
					}

					materials[get_techset_key(stock.techset)].emplace_back(std::move(stock));
				}
			}

			for (const auto* zone_name : common_zones)
			{
				if (const auto path = game_files::find_zone(zone_name))
				{
					const auto database = db_create();
					db_scope scope(*database);
					add_inline_techsets(*target::load_zone(*path), available_techsets);
				}
			}

			for (auto& [key, list] : materials)
			{
				for (auto& stock : list)
				{
					if (const auto found = available_techsets.find(stock.techset); found != available_techsets.end())
					{
						stock.techset_available = true;
						stock.state = found->second;
					}
				}
			}

			count_suffix_uses(materials);
			return materials;
		}

		const std::unordered_map<std::string, std::vector<stock_material>>& stock_materials()
		{
			static const auto materials = load_stock_materials();
			return materials;
		}

		int convert_texture_index(const unsigned char index)
		{
			if (index < 24)
			{
				return index / 8 * 9 + index % 8;
			}

			if (index < 40)
			{
				return 27 + index % 8;
			}

			if (index == 48)
			{
				return 45;
			}

			if (index >= 79 && index < 87)
			{
				return 59 + index - 79;
			}

			if (index >= 185 && index < 191)
			{
				return 159 + index - 185;
			}

			switch (index)
			{
			case 87:
				return 68;
			case 95:
				return 77;
			case 111:
				return 68;
			case 159:
				return 126;
			case 177:
				return 151;
			case 184:
				return 158;
			default:
				return -1;
			}
		}

		int convert_constant_index(const unsigned char index)
		{
			if (index >= 24 && index < 32)
			{
				return -1; // format differs from IW8 27..35
			}

			if (index < 56)
			{
				return index / 8 * 9 + index % 8;
			}

			switch (index)
			{
			case 100:
				return 87;
			case 101:
				return 88;
			case 102:
				return 89;
			case 107:
				return 94;
			case 108:
				return 95;
			case 110:
				return 100;
			case 134:
				return 127;
			case 135:
				return 128;
			case 136:
				return 130;
			case 137:
				return 131;
			default:
				return -1;
			}
		}

		bool is_layer_constant(const int index, const int first)
		{
			return index >= first && index < first + 9;
		}

		void convert_layer_packing(vec4_t& literal, const int index)
		{
			const auto word0 = std::bit_cast<std::uint32_t>(literal[0]);
			if (is_layer_constant(index, target::MATERIAL_CONSTANT_LAYER_FLAGS))
			{
				literal[0] = std::bit_cast<float>(word0 >> 2);
			}
			else if (is_layer_constant(index, target::MATERIAL_CONSTANT_LAYER_TINT))
			{
				const auto alpha = std::clamp(half_float::half_to_float(static_cast<std::uint16_t>(std::bit_cast<std::uint32_t>(literal[3]) & 0xFFFF)), 0.0f, 1.0f);
				literal[0] = std::bit_cast<float>((word0 & 0x00FFFFFF) | static_cast<std::uint32_t>(std::lround(alpha * 255.0f)) << 24);
				literal[1] = std::bit_cast<float>(std::bit_cast<std::uint32_t>(literal[1]) | 0x00FF0000);
				literal[3] = 0.0f;
			}
		}

		void set_sky_texel_inset(vec4_t& literal, const std::map<int, const GfxImage*>& textures)
		{
			const auto sky = textures.find(151);
			if (sky == textures.end() || !sky->second || !sky->second->width || !sky->second->height)
			{
				return;
			}

			literal[2] = 8.0f / sky->second->width;
			literal[3] = 4.0f / sky->second->height;
		}

		bool is_default_image(const GfxImage* image)
		{
			return !image || !image->name || image->name[0] == '$';
		}

		std::map<int, const GfxImage*> get_textures(const Material* asset)
		{
			std::map<int, const GfxImage*> textures;
			const auto transmission = reads_transmission_image(asset);
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				const auto& texture = asset->textureTable[i];
				const auto index = convert_texture_index(texture.index);
				if (index < 0 || texture.index == 87 && !transmission)
				{
					continue;
				}

				const auto found = textures.find(index);
				if (found == textures.end() || is_default_image(found->second))
				{
					textures[index] = texture.image;
				}
			}

			return textures;
		}

		std::map<int, const GfxImage*> get_layer_textures(const Material* asset, const unsigned char first_index)
		{
			std::map<int, const GfxImage*> textures;
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				const auto& texture = asset->textureTable[i];
				if (texture.index >= first_index && texture.index < first_index + 8 && !is_default_image(texture.image))
				{
					textures[27 + texture.index - first_index] = texture.image;
				}
			}

			return textures;
		}

		const GfxImage* find_loaded_image(const std::map<int, const GfxImage*>& textures, const int index)
		{
			const auto found = textures.find(index);
			return found == textures.end() ? nullptr : static_cast<GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, found->second->name));
		}

		target::GfxImage* dump_layer_mask(const Material* asset, const int index, utils::memory::allocator& allocator)
		{
			const auto reveal = find_loaded_image(get_layer_textures(asset, 32), index);
			if (!reveal)
			{
				return nullptr;
			}

			return gfximage::dump_alpha_reveal(find_loaded_image(get_layer_textures(asset, 24), index), reveal, allocator);
		}

		std::array<const char*, 4> get_blend_order(const std::string_view blend)
		{
			if (blend == "_alt")
			{
				return {"_alt", "_bad", "_bar", "_rpl"};
			}

			if (blend == "_bad")
			{
				return {"_bad", "_bar", "_alt", "_rpl"};
			}

			if (blend == "_bar")
			{
				return {"_bar", "_bad", "_alt", "_rpl"};
			}

			return {"_rpl", "_alt", "_bad", "_bar"};
		}

		std::vector<std::string> get_candidate_keys(const Material* asset)
		{
			const std::string_view name = asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "null";
			auto key = get_techset_key(name);
			const auto splined = key.starts_with('~');
			if (splined)
			{
				key.erase(0, 1);
			}

			const auto slash = key.find('/');
			auto prefix = slash == std::string::npos ? std::string() : key.substr(0, slash);
			auto rest = slash == std::string::npos ? key : key.substr(slash + 1);

			static const std::unordered_map<std::string_view, std::string_view> prefixes =
			{
				{"tmrco", "tmco"}, {"twck", "twc"}, {"twk", "tw"}, {"elcqp", "elcq"}, {"g", "mo"},
			};

			if (const auto found = prefixes.find(prefix); found != prefixes.end())
			{
				prefix = found->second;
			}

			if (asset->layerCount > 1 && (prefix == "twc" || prefix == "tw"))
			{
				prefix += "4";
			}

			if (rest.starts_with("effect_6_effect"))
			{
				rest.replace(0, 15, "unlit_6_effect");
			}

			std::vector<std::string> keys;
			const auto add = [&](const std::string& candidate_prefix, const std::string& candidate_rest)
			{
				keys.emplace_back(std::string(splined ? "~" : "") + (candidate_prefix.empty() ? candidate_rest : candidate_prefix + "/" + candidate_rest));
			};

			static const std::unordered_map<std::string_view, std::string_view> without_vertex_color =
			{
				{"twc", "tw"}, {"twc4", "tw4"}, {"tmco", "tmo"}, {"tm2co", "tm2o"}, {"mco", "mo"},
			};

			add(prefix, rest);
			if (const auto found = without_vertex_color.find(prefix); found != without_vertex_color.end())
			{
				add(std::string(found->second), rest);
			}

			if (rest.size() > 8)
			{
				const auto alpha = rest.substr(rest.size() - 4);
				const auto base = rest.substr(0, rest.size() - 8);
				const auto blends = get_blend_order(rest.substr(rest.size() - 8, 4));
				for (const auto* blend : blends)
				{
					add(prefix, base + blend + alpha);
				}

				for (const auto* blend : blends)
				{
					for (const auto* other_alpha : {"_ta1", "_tca", "_ta0"})
					{
						add(prefix, base + blend + other_alpha);
					}
				}
			}

			if (prefix.ends_with('4'))
			{
				prefix.pop_back();
				add(prefix, rest);
			}

			return keys;
		}

		render_state get_source_state(const Material* asset)
		{
			if (!asset->techniqueSet || !asset->techniqueSet->name)
			{
				return {};
			}

			const auto* techset = static_cast<const MaterialTechniqueSet*>(db_find_asset(ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet->name));
			return techset ? get_render_state(techset) : render_state{};
		}

		unsigned char get_base_rasterizer_bits(const render_state& state)
		{
			unsigned char bits = 0xFF;
			for (const auto value : state.depth_stencil_rasterizer)
			{
				bits = std::min(bits, static_cast<unsigned char>(value));
			}

			return bits;
		}

		bool has_same_state(const render_state& state, const stock_material& candidate)
		{
			return candidate.state == state;
		}

		bool has_same_rasterizer(const render_state& state, const stock_material& candidate)
		{
			return !candidate.state.depth_stencil_rasterizer.empty() && get_base_rasterizer_bits(candidate.state) == get_base_rasterizer_bits(state);
		}

		bool has_any_state(const render_state&, const stock_material&)
		{
			return true;
		}

		using state_match = bool(*)(const render_state&, const stock_material&);

		unsigned int count_shared_textures(const stock_material& candidate, const std::map<int, const GfxImage*>& textures)
		{
			auto shared = 0u;
			for (const auto& texture : candidate.textures)
			{
				shared += textures.contains(texture.index) ? 1 : 0;
			}

			return shared;
		}

		unsigned int count_missing_textures(const stock_material& candidate, const std::map<int, const GfxImage*>& textures)
		{
			auto missing = 0u;
			for (const auto& texture : candidate.textures)
			{
				missing += !textures.contains(texture.index) && !texture.image.starts_with('$') ? 1 : 0;
			}

			return missing;
		}

		const stock_material* find_best_material(const std::vector<stock_material>& candidates, const Material* asset,
			const std::map<int, const GfxImage*>& textures, const render_state& state, const state_match matches, const unsigned int min_shared,
			const std::string_view blend)
		{
			const stock_material* best{};
			auto best_score = -1.0f;
			for (const auto& candidate : candidates)
			{
				const auto shared = count_shared_textures(candidate, textures);
				if (!matches(state, candidate) || shared < min_shared || (!blend.empty() && get_techset_blend(candidate.techset) != blend))
				{
					continue;
				}

				const auto total = candidate.textures.size() + textures.size() - shared + count_missing_textures(candidate, textures);
				auto score = total ? static_cast<float>(shared) / static_cast<float>(total) : 1.0f;
				score += candidate.layer_count == asset->layerCount ? 2.0f : 0.0f;
				score += candidate.wind == has_wind(asset) ? 1.0f : 0.0f;
				if (score > best_score || (score == best_score
					&& std::tie(candidate.techset_available, candidate.suffix_uses) > std::tie(best->techset_available, best->suffix_uses)))
				{
					best_score = score;
					best = &candidate;
				}
			}

			return best;
		}

		const stock_material* find_stock_material(const std::vector<std::string>& keys, const Material* asset,
			const std::map<int, const GfxImage*>& textures, const render_state& state, const state_match matches, const unsigned int min_shared,
			const std::string_view blend)
		{
			const auto& stock = stock_materials();
			for (const auto& key : keys)
			{
				const auto found = stock.find(key);
				if (found == stock.end())
				{
					continue;
				}

				if (const auto* best = find_best_material(found->second, asset, textures, state, matches, min_shared, blend))
				{
					return best;
				}
			}

			return nullptr;
		}

		const stock_material* find_stock_material(const Material* asset, const std::map<int, const GfxImage*>& textures)
		{
			const auto keys = get_candidate_keys(asset);
			const auto state = get_source_state(asset);
			const auto* any_state = find_stock_material(keys, asset, textures, state, has_any_state, 0, {});
			if (!any_state || state.depth_stencil_rasterizer.empty() || has_same_state(state, *any_state))
			{
				return any_state;
			}

			const auto min_shared = count_shared_textures(*any_state, textures);
			const auto blend = get_techset_blend(any_state->techset);
			if (const auto* same_state = find_stock_material(keys, asset, textures, state, has_same_state, min_shared, blend))
			{
				return same_state;
			}

			const auto* same_rasterizer = find_stock_material(keys, asset, textures, state, has_same_rasterizer, min_shared, blend);
			const auto* best = same_rasterizer ? same_rasterizer : any_state;
			ZONETOOL_WARNING("material \"%s\": stock IW8 techset \"%s\" has a different render state than \"%s\"", asset->name,
				best->techset.data(), asset->techniqueSet->name);
			return best;
		}

		target::GfxImage* make_builtin_image_reference(const std::string_view name, utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = allocator.duplicate_string(std::string(name));
			add_csv_row(target::type_to_string(target::ASSET_TYPE_IMAGE), ","s + image->name);
			return image;
		}

		std::string_view get_default_image(const stock_texture& texture)
		{
			if (texture.image.starts_with('$'))
			{
				return texture.image;
			}

			if (texture.index >= 9 && texture.index < 18)
			{
				return "$identitynormalmap";
			}

			const auto black = texture.index >= 18 && texture.index < 27 || texture.index >= 59 && texture.index < 67 || texture.index == 68;
			return black ? "$black" : "$white";
		}

		target::Material* convert(const Material* asset, const stock_material& stock, const std::map<int, const GfxImage*>& textures, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = get_name(asset, allocator);
			result->contents = physics::convert_contents(asset->contents);
			result->surfaceFlags = asset->surfaceFlags;
			result->maxDisplacement = asset->maxDisplacement;
			result->materialType = stock.material_type;
			result->cameraRegion = stock.camera_region;
			result->sortKey = stock.sort_key;
			result->flags = stock.flags;
			result->layerCount = stock.layer_count;
			result->packedAtlasDataSize = asset->packedAtlasDataSize;
			result->textureAtlasRowCount = asset->textureAtlasRowCount;
			result->textureAtlasColumnCount = asset->textureAtlasColumnCount;
			result->packedAtlasData = asset->packedAtlasData;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = allocator.duplicate_string(stock.techset);

			if (asset->decalVolumeMaterial && asset->decalVolumeMaterial->name)
			{
				result->decalVolumeMaterial = allocator.allocate<target::GfxDecalVolumeMaterial>();
				result->decalVolumeMaterial->name = allocator.duplicate_string(asset->decalVolumeMaterial->name);
			}

			result->textureCount = static_cast<unsigned char>(stock.textures.size());
			result->textureTable = stock.textures.empty() ? nullptr : allocator.allocate_array<target::MaterialTextureDef>(stock.textures.size());
			for (auto i = 0u; i < stock.textures.size(); i++)
			{
				const auto& texture = stock.textures[i];
				result->textureTable[i].index = texture.index;

				if (const auto mask = dump_layer_mask(asset, texture.index, allocator))
				{
					result->textureTable[i].image = mask;
					continue;
				}

				const auto found = textures.find(texture.index);
				result->textureTable[i].image = found != textures.end() && !is_default_image(found->second) && gfximage::dump_used(found->second)
					? gfximage::image_reference(found->second, allocator)
					: make_builtin_image_reference(get_default_image(texture), allocator);
			}

			result->constantCount = static_cast<unsigned char>(stock.constants.size());
			result->constantTable = stock.constants.empty() ? nullptr : allocator.allocate_array<target::MaterialConstantDef>(stock.constants.size());
			for (auto i = 0u; i < stock.constants.size(); i++)
			{
				result->constantTable[i] = stock.constants[i];
				for (auto j = 0; asset->constantTable && j < asset->constantCount; j++)
				{
					const auto& constant = asset->constantTable[j];
					if (convert_constant_index(constant.index) != stock.constants[i].index)
					{
						continue;
					}

					std::memcpy(result->constantTable[i].literal, constant.literal, sizeof(vec4_t));
					convert_layer_packing(result->constantTable[i].literal, stock.constants[i].index);
					if (constant.index == 135)
					{
						set_sky_texel_inset(result->constantTable[i].literal, textures);
					}
				}
			}

			return result;
		}

		void dump_zone_donor_techsets(const char* zone_name)
		{
			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				return;
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);

			std::unordered_set<std::string> shaders;
			for (const auto& asset : zone->assets)
			{
				auto* techset = static_cast<target::MaterialTechniqueSet*>(asset.header);
				if (asset.referenced || asset.type != target::ASSET_TYPE_TECHNIQUE_SET || !donor_techsets.erase(techset->name))
				{
					continue;
				}

				target::technique_set::dump(techset);
				add_csv_row(target::type_to_string(target::ASSET_TYPE_TECHNIQUE_SET), techset->name);
				for (auto i = 0u; techset->maskedTechniques && i < target::get_technique_count(techset); i++)
				{
					const auto* technique = techset->maskedTechniques[i];
					if (!technique)
					{
						continue;
					}

					for (const auto* shader : {technique->vertexShader ? technique->vertexShader->name : nullptr,
						technique->hullShader ? technique->hullShader->name : nullptr,
						technique->domainShader ? technique->domainShader->name : nullptr,
						technique->pixelShader ? technique->pixelShader->name : nullptr})
					{
						if (shader)
						{
							shaders.emplace(shader);
						}
					}
				}
			}

			for (const auto& asset : zone->assets)
			{
				const auto* shader = *static_cast<const char* const*>(asset.header);
				if (!shader || !shaders.contains(shader))
				{
					continue;
				}

				if (asset.referenced)
				{
					add_csv_row(target::type_to_string(asset.type), ","s + shader);
					continue;
				}

				switch (asset.type)
				{
				case target::ASSET_TYPE_VERTEXSHADER:
					target::vertex_shader::dump(static_cast<target::MaterialVertexShader*>(asset.header));
					break;
				case target::ASSET_TYPE_HULLSHADER:
					target::hull_shader::dump(static_cast<target::MaterialHullShader*>(asset.header));
					break;
				case target::ASSET_TYPE_DOMAINSHADER:
					target::domain_shader::dump(static_cast<target::MaterialDomainShader*>(asset.header));
					break;
				case target::ASSET_TYPE_PIXELSHADER:
					target::pixel_shader::dump(static_cast<target::MaterialPixelShader*>(asset.header));
					break;
				default:
					continue;
				}

				add_csv_row(target::type_to_string(asset.type), shader);
			}
		}

		void reference_common_techsets(const char* zone_name)
		{
			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				return;
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);
			for (const auto& asset : zone->assets)
			{
				const auto* techset = static_cast<const target::MaterialTechniqueSet*>(asset.header);
				if (asset.referenced || asset.type != target::ASSET_TYPE_TECHNIQUE_SET || !donor_techsets.erase(techset->name))
				{
					continue;
				}

				add_csv_row("require", zone_name);
				add_csv_row(target::type_to_string(target::ASSET_TYPE_TECHNIQUE_SET), ","s + techset->name);
			}
		}
	}

	target::Material* reference(const Material* asset, utils::memory::allocator& allocator)
	{
		if (!asset)
		{
			return nullptr;
		}

		const auto result = allocator.allocate<target::Material>();
		result->name = get_name(asset, allocator);

		const auto loaded = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, asset->name));
		if (!loaded || !loaded->techniqueSet || !dump(loaded))
		{
			add_csv_row(target::type_to_string(target::ASSET_TYPE_MATERIAL), ","s + result->name);
		}

		return result;
	}

	int layer_count(const Material* asset)
	{
		const auto loaded = asset ? static_cast<const Material*>(db_find_asset(ASSET_TYPE_MATERIAL, asset->name)) : nullptr;
		for (auto i = 0; loaded && loaded->constantTable && i < loaded->constantCount; i++)
		{
			if (loaded->constantTable[i].index == MATERIAL_CONSTANT_LAYER_COUNT)
			{
				return std::max(1, static_cast<int>(loaded->constantTable[i].literal[0]));
			}
		}

		return 1;
	}

	bool uses_vertex_layer_mask(const Material* asset)
	{
		const auto found = asset ? dumped_materials.find(asset->name) : dumped_materials.end();
		return found != dumped_materials.end() && found->second && is_vertex_layer_mask_techset(found->second->techset);
	}

	bool dump(Material* asset)
	{
		if (const auto found = dumped_materials.find(asset->name); found != dumped_materials.end())
		{
			return found->second != nullptr;
		}

		const auto textures = get_textures(asset);
		const auto* stock = find_stock_material(asset, textures);
		dumped_materials[asset->name] = stock;
		if (!stock)
		{
			ZONETOOL_WARNING("material \"%s\": no stock IW8 techset for \"%s\"", asset->name,
				asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "");
			return false;
		}

		if (is_vertex_layer_mask_techset(stock->techset) && get_techset_layer_count(stock->techset) > 2)
		{
			ZONETOOL_WARNING("material \"%s\": IW8 techset \"%s\" uses one vertex mask for %d layers", asset->name,
				stock->techset.data(), get_techset_layer_count(stock->techset));
		}

		utils::memory::allocator allocator;
		const auto converted = convert(asset, *stock, textures, allocator);
		donor_techsets.emplace(stock->techset);

		target::material::dump(converted);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_MATERIAL), converted->name);
		return true;
	}

	void dump_donor_techsets()
	{
		if (donor_techsets.empty())
		{
			return;
		}

		xfile::oodle::load(iw8_folder());
		auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
		const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

		for (const auto* zone : stock_zones)
		{
			if (!donor_techsets.empty())
			{
				dump_zone_donor_techsets(zone);
			}
		}

		for (const auto* zone : common_zones)
		{
			if (!donor_techsets.empty())
			{
				reference_common_techsets(zone);
			}
		}

		for (const auto& name : donor_techsets)
		{
			ZONETOOL_WARNING("IW8 donor techset \"%s\" is not in the stock zones, the build needs a zone that holds it", name.data());
			add_csv_row(target::type_to_string(target::ASSET_TYPE_TECHNIQUE_SET), ","s + name);
		}

		donor_techsets.clear();
	}
}
