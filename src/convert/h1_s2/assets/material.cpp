#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::material
{
	namespace
	{
		constexpr std::uint8_t no_entry = 0xFF;
		constexpr std::uint32_t h1_polygon_offset_shift = 4;
		constexpr std::uint32_t h1_polygon_offset_mask = 3u << h1_polygon_offset_shift;
		constexpr std::uint32_t s2_polygon_offset_shift = 27;
		constexpr std::uint32_t blend_class_shift = 26;
		constexpr std::uint32_t blend_word_count = 3;
		constexpr auto depth_stencil_mode_count = std::extent_v<decltype(zonetool::s2::material_state::depth_stencil)>;

		std::vector<Material*> pending_materials;
		std::unordered_set<std::string> pending_names;
		std::unordered_map<const Material*, std::string> pending_sprites;
		std::unordered_set<std::string> minimap_materials;

		struct native_material
		{
			zonetool::s2::Material* material;
			zonetool::s2::material_state* states;
		};

		zonetool::s2::GfxImage* make_image_stub(const std::string& name, utils::memory::allocator& allocator)
		{
			return make_stub<zonetool::s2::GfxImage>(allocator.duplicate_string(name), allocator);
		}

		std::string fallback_image(const std::uint8_t semantic)
		{
			switch (semantic)
			{
			case zonetool::s2::TS_COLOR_MAP:
				return gfximage::dump_solid("h1_s2_white", zonetool::s2::TS_COLOR_MAP, {255, 255, 255, 255});
			case zonetool::s2::TS_NORMAL_MAP:
				return "$identitynormalmap";
			case zonetool::s2::TS_SPECULAR_MAP:
				return gfximage::dump_solid("h1_s2_specular", zonetool::s2::TS_SPECULAR_MAP, {49, 255, 128, 255});
			default:
				return "$black";
			}
		}

		std::uint32_t convert_material_type(const std::string& h1_techset)
		{
			// h1 world techsets have no type prefix ("l_sm_...")
			using namespace zonetool::s2;

			const auto prefix = h1_techset.substr(0, h1_techset.find('_'));
			if (prefix == "l" || prefix == "w" || prefix == "wc")
			{
				return MTL_TYPE_BIT_WORLD;
			}

			if (prefix == "distortion")
			{
				return MTL_TYPE_BIT_X | MTL_TYPE_BIT_V;
			}

			if (!prefix.starts_with('m'))
			{
				return 0;
			}

			if (prefix == "ml" || prefix == "mlc" || prefix == "msl" || prefix == "mslc")
			{
				return MTL_TYPE_BIT_MODEL | MTL_TYPE_BIT_LIGHTMAP;
			}

			if (prefix == "me" || prefix == "mce" || prefix == "mec")
			{
				return MTL_TYPE_BIT_MODEL | MTL_TYPE_BIT_EFFECT;
			}

			return MTL_TYPE_BIT_MODEL;
		}

		std::uint32_t convert_blend_word(const std::uint32_t h1_word)
		{
			return (h1_word & 0x7FF) | ((h1_word & 0x7FF000) >> 1) | ((h1_word & 0xF000000) >> 2);
		}

		std::uint32_t blend_class(const std::int32_t slot)
		{
			const auto type = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			const auto is_lit = type >= zonetool::s2::TECHNIQUE_EMISSIVE && type < zonetool::s2::TECHNIQUE_LIT_END;
			return type == zonetool::s2::TECHNIQUE_UNLIT || is_lit || type == zonetool::s2::TECHNIQUE_WIREFRAME_SOLID ? 1 : 0;
		}

		std::uint8_t rasterizer_index(const std::uint32_t load_bits0, const std::uint32_t load_bits2)
		{
			return static_cast<std::uint8_t>(((load_bits0 >> 3) & 3) | ((load_bits2 >> 25) & 0x1C) | (load_bits0 & 0x20));
		}

		std::string get_techset_name(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "";
		}

		bool is_effect_material(const Material* asset)
		{
			const auto techset = get_techset_name(asset);
			return techset.starts_with("effect") || techset.starts_with("particle_cloud");
		}

		std::uint32_t convert_load_bits0(const std::uint32_t load_bits0, const bool effect)
		{
			// stock s2 effects never alpha test > 0
			if (effect && (load_bits0 & (GFXS0_ATEST_DISABLE | GFXS0_ATEST_MASK)) == GFXS0_ATEST_GT_0)
			{
				return (load_bits0 & ~GFXS0_ATEST_MASK) | GFXS0_ATEST_DISABLE;
			}

			return load_bits0;
		}

		zonetool::s2::GfxStateBits convert_state_bits(const GfxStateBits& source, const std::uint32_t blend_class, const bool effect)
		{
			const auto polygon_offset = (source.loadBits[1] & h1_polygon_offset_mask) >> h1_polygon_offset_shift;

			zonetool::s2::GfxStateBits result{};
			result.loadBits[0] = convert_load_bits0(source.loadBits[0], effect);
			result.loadBits[1] = source.loadBits[1] & ~h1_polygon_offset_mask;
			result.loadBits[2] = source.loadBits[2] | (polygon_offset << s2_polygon_offset_shift);
			for (auto i = 0u; i < blend_word_count; i++)
			{
				result.loadBits[3 + i] = convert_blend_word(source.loadBits[3 + i]);
			}
			result.loadBits[3] |= blend_class << blend_class_shift; // the output color space class, only the first render target has one
			result.rasterizerState = rasterizer_index(result.loadBits[0], result.loadBits[2]);
			return result;
		}

		zonetool::s2::GfxStateBits convert_light_state_bits(const GfxStateBits& lit, const GfxStateBits& light, const std::uint32_t blend_class, const bool effect)
		{
			// the h1 light pass polygon offset fails the s2 depth test, so it takes the lit pass state with the light blend
			auto source = lit;
			source.loadBits[1] = lit.loadBits[1] & ~(h1_polygon_offset_mask | GFXS1_DEPTHWRITE);
			for (auto i = 0u; i < blend_word_count; i++)
			{
				source.loadBits[3 + i] = light.loadBits[3 + i];
			}

			return convert_state_bits(source, blend_class, effect);
		}

		std::array<std::uint64_t, depth_stencil_mode_count> depth_stencil_states(const std::uint32_t load_bits1, const std::uint32_t load_bits2)
		{
			constexpr std::uint32_t depth_write = 1;
			constexpr std::uint32_t front_stencil_func = 0x000E0000;
			constexpr std::uint32_t back_stencil_func = 0xE0000000;
			constexpr std::uint32_t default_stencil_masks = 0xFFFF;
			constexpr std::array<std::uint32_t, depth_stencil_mode_count> default_masks
			{
				0x0909, 0x1010, 0x0404, 0x0808, 0x0902, 0x0912, 0x0303, 0x0707, 0x1313, 0x1717, 0x0909, 0x1010,
			};

			const auto front_func = load_bits1 & GFXS1_STENCIL_FRONT_ENABLE ? load_bits1 & front_stencil_func : front_stencil_func;
			const auto back_func = load_bits1 & GFXS1_STENCIL_BACK_ENABLE
				? load_bits1 & back_stencil_func
				: front_func << (GFXS1_STENCIL_BACK_FUNC_SHIFT - GFXS1_STENCIL_FRONT_FUNC_SHIFT);
			const auto stencil_states = load_bits1 & 0xFFFFFF00 ? load_bits1 | depth_write : load_bits1 | 0xE02E0241;
			const std::array<std::uint32_t, depth_stencil_mode_count> states
			{
				load_bits1,
				(load_bits1 & 0xBF) | 0xE02E0240,
				(load_bits1 & 0xBF) | front_func | back_func | 0x200240,
				load_bits1,
				(load_bits1 & ~depth_write) | 0x400400C0,
				(load_bits1 & ~depth_write) | 0x400400C0,
				stencil_states,
				stencil_states,
				stencil_states,
				stencil_states,
				load_bits1 | depth_write,
				(load_bits1 & 0xBE) | 0xE02E0241,
			};

			const auto keeps_stencil_masks = load_bits2 != default_stencil_masks;
			std::array<std::uint64_t, depth_stencil_mode_count> result{};
			for (auto mode = 0u; mode < depth_stencil_mode_count; mode++)
			{
				const auto uses_stencil_masks = mode == 0 || (mode >= 6 && mode <= 9);
				const auto masks = keeps_stencil_masks && uses_stencil_masks ? load_bits2 & default_stencil_masks : default_masks[mode];
				result[mode] = (static_cast<std::uint64_t>(masks) << 32) | states[mode];
			}

			return result;
		}

		zonetool::s2::material_state convert_state(const zonetool::s2::GfxStateBits& state_bits)
		{
			zonetool::s2::material_state result{};
			const auto depth_stencil = depth_stencil_states(state_bits.loadBits[1], state_bits.loadBits[2]);
			std::ranges::copy(depth_stencil, result.depth_stencil);
			for (auto i = 0u; i < blend_word_count; i++)
			{
				result.blend[i] = state_bits.loadBits[3 + i] & ~(7u << blend_class_shift);
			}

			return result;
		}

		std::string select_native_image(const MaterialTextureDef& texture)
		{
			if (texture.u.image && texture.u.image->name)
			{
				auto* image = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_IMAGE, texture.u.image->name).image;
				if (const auto name = image ? gfximage::dump_native(image) : std::nullopt)
				{
					return *name;
				}
			}

			if (texture.semantic == TS_SPECULAR_MAP)
			{
				return gfximage::dump_solid("h1_s2_native_specular", zonetool::s2::TS_SPECULAR_MAP, {32, 32, 32, 128});
			}

			return fallback_image(texture.semantic);
		}

		zonetool::s2::MaterialConstantBufferDef convert_constant_buffer(const MaterialConstantBufferDef& source)
		{
			zonetool::s2::MaterialConstantBufferDef result{};
			result.vsDataSize = source.vsDataSize;
			result.hsDataSize = source.hsDataSize;
			result.dsDataSize = source.dsDataSize;
			result.psDataSize = source.psDataSize;
			result.vsOffsetDataSize = source.vsOffsetDataSize;
			result.hsOffsetDataSize = source.hsOffsetDataSize;
			result.dsOffsetDataSize = source.dsOffsetDataSize;
			result.psOffsetDataSize = source.psOffsetDataSize;
			result.vsData = source.vsData;
			result.hsData = source.hsData;
			result.dsData = source.dsData;
			result.psData = source.psData;
			result.vsOffsetData = source.vsOffsetData;
			result.hsOffsetData = source.hsOffsetData;
			result.dsOffsetData = source.dsOffsetData;
			result.psOffsetData = source.psOffsetData;
			return result;
		}

		zonetool::s2::material_state* convert_native_states(const Material* asset, const techset::native_techset& native, zonetool::s2::Material* material,
			utils::memory::allocator& allocator)
		{
			std::ranges::fill(material->stateBitsEntry, no_entry);
			std::ranges::fill(material->constantBufferIndex, no_entry);

			// pass p of a technique uses stateBitsEntry[slot] + p
			std::vector<zonetool::s2::GfxStateBits> state_bits;
			std::vector<zonetool::s2::material_state> states;
			std::map<std::tuple<std::uint8_t, std::uint8_t, std::uint32_t, std::uint32_t>, std::uint8_t> entries;
			const auto effect = is_effect_material(asset);
			const auto state_source = [&](const std::int32_t source)
			{
				constexpr auto group_size = TECHNIQUE_INSTANCED_BEGIN - TECHNIQUE_STANDARD_BEGIN;
				const auto instanced = source >= TECHNIQUE_INSTANCED_BEGIN && source < TECHNIQUE_INSTANCED_BEGIN + group_size;
				if (!instanced || asset->stateBitsEntry[source] != no_entry)
				{
					return source;
				}

				return source - group_size;
			};

			for (auto slot = 0; slot < zonetool::s2::technique_count; slot++)
			{
				const auto source = state_source(native.sources[slot]);
				if (source < 0)
				{
					continue;
				}

				material->constantBufferIndex[slot] = asset->constantBufferIndex[source];

				const auto h1_entry = asset->stateBitsEntry[source];
				if (h1_entry == no_entry)
				{
					continue;
				}

				const auto* technique = native.techset->techniques[slot];
				const auto pass_count = technique ? std::max<std::uint32_t>(technique->hdr.passCount, 1u) : 1u;
				const auto light_source = state_source(native.light_sources[slot]);
				const auto light_entry = light_source >= 0 ? asset->stateBitsEntry[light_source] : no_entry;
				const auto base_pass_count = pass_count - (light_source >= 0 ? 1u : 0u);
				if (!asset->stateBitsTable || h1_entry + base_pass_count > asset->stateBitsCount
					|| (light_source >= 0 && (light_entry == no_entry || light_entry >= asset->stateBitsCount)))
				{
					throw std::runtime_error(std::format("technique {} has no state bits", source));
				}

				const auto key = std::make_tuple(h1_entry, light_entry, blend_class(slot), pass_count);
				const auto [entry, inserted] = entries.try_emplace(key, static_cast<std::uint8_t>(state_bits.size()));
				if (inserted)
				{
					for (auto pass = 0u; pass < pass_count; pass++)
					{
						if (pass < base_pass_count)
						{
							state_bits.emplace_back(convert_state_bits(asset->stateBitsTable[h1_entry + pass], std::get<2>(key), effect));
						}
						else
						{
							state_bits.emplace_back(convert_light_state_bits(asset->stateBitsTable[h1_entry], asset->stateBitsTable[light_entry], std::get<2>(key), effect));
						}

						states.emplace_back(convert_state(state_bits.back()));
					}
				}

				material->stateBitsEntry[slot] = entry->second;
			}

			if (state_bits.size() >= no_entry)
			{
				throw std::runtime_error("too many state bits");
			}

			material->stateBitsCount = static_cast<unsigned char>(state_bits.size());
			material->stateBitsTable = copy_array(state_bits.data(), state_bits.size(), allocator);
			return copy_array(states.data(), states.size(), allocator);
		}

		void convert_native_constants(const Material* asset, zonetool::s2::Material* material, utils::memory::allocator& allocator)
		{
			const auto count = asset->constantTable ? asset->constantCount : 0;
			material->constantCount = static_cast<unsigned char>(count);
			material->constantTable = allocator.allocate_array<zonetool::s2::MaterialConstantDef>(count);
			for (auto i = 0; i < count; i++)
			{
				const auto& source = asset->constantTable[i];
				auto& constant = material->constantTable[i];
				constant.nameHash = source.nameHash;
				std::ranges::copy(source.name, constant.name);
				std::ranges::copy(source.literal, constant.literal);
			}

			const auto buffer_count = asset->constantBufferTable ? asset->constantBufferCount : 0;
			material->constantBufferCount = static_cast<unsigned char>(buffer_count);
			material->constantBufferTable = allocator.allocate_array<zonetool::s2::MaterialConstantBufferDef>(buffer_count);
			for (auto i = 0; i < buffer_count; i++)
			{
				material->constantBufferTable[i] = convert_constant_buffer(asset->constantBufferTable[i]);
			}
		}

		void convert_native_textures(const Material* asset, zonetool::s2::Material* material, utils::memory::allocator& allocator)
		{
			const auto count = asset->textureTable ? asset->textureCount : 0;
			for (auto i = 0; i < count; i++)
			{
				if (asset->textureTable[i].semantic == TS_WATER_MAP)
				{
					throw std::runtime_error("water textures are not converted");
				}
			}

			material->textureCount = static_cast<unsigned char>(count);
			material->textureTable = allocator.allocate_array<zonetool::s2::MaterialTextureDef>(count);
			for (auto i = 0; i < count; i++)
			{
				const auto& source = asset->textureTable[i];
				auto& texture = material->textureTable[i];
				texture.nameHash = source.nameHash;
				texture.nameStart = source.nameStart;
				texture.nameEnd = source.nameEnd;
				texture.samplerState = source.samplerState;
				texture.semantic = source.semantic;
				texture.u.image = make_image_stub(select_native_image(source), allocator);
			}
		}

		bool deforms_prepass_vertices(const Material* asset)
		{
			const auto* prepass = asset->techniqueSet ? asset->techniqueSet->techniques[TECHNIQUE_ZPREPASS] : nullptr;
			const auto* shader = prepass && prepass->hdr.passCount ? prepass->passArray[0].vertexShader : nullptr;
			return shader && shader->name && std::string_view(shader->name) != "position_only.hlsl";
		}

		zonetool::s2::MaterialPrepassClass convert_prepass_class(const Material* asset, const zonetool::s2::MaterialTechniqueSet* techset)
		{
			if (!(asset->stateFlags & STATE_FLAG_WRITES_DEPTH))
			{
				return zonetool::s2::PREPASS_CLASS_BLENDED;
			}

			const auto name = std::string_view(asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "");
			const auto alpha_tested = std::ranges::any_of(utils::string::split(std::string(name), '_'), [](const std::string& token)
			{
				return token.starts_with("t0") || token.starts_with("a0") || token == "vdh";
			});

			if (!alpha_tested && !deforms_prepass_vertices(asset)) // opaque materials get the s2 generic position only prepass
			{
				return zonetool::s2::PREPASS_CLASS_OPAQUE;
			}

			return techset && techset->techniques[zonetool::s2::TECHNIQUE_ZPREPASS] ? zonetool::s2::PREPASS_CLASS_ALPHA_TESTED : zonetool::s2::PREPASS_CLASS_BLENDED;
		}

		std::uint8_t convert_state_flags(const std::uint8_t h1_flags)
		{
			constexpr std::array<std::pair<std::uint8_t, std::uint8_t>, 5> flag_map
			{{
				{STATE_FLAG_CULL_BACK, zonetool::s2::STATE_FLAG_CULL_BACK},
				{STATE_FLAG_DECAL, zonetool::s2::STATE_FLAG_DECAL},
				{STATE_FLAG_WRITES_DEPTH, zonetool::s2::STATE_FLAG_WRITES_DEPTH},
				{STATE_FLAG_USES_DEPTH_BUFFER, zonetool::s2::STATE_FLAG_USES_DEPTH_BUFFER},
				{STATE_FLAG_USES_STENCIL_BUFFER, zonetool::s2::STATE_FLAG_USES_STENCIL_BUFFER},
			}};

			std::uint8_t result = 0;
			for (const auto& [h1_flag, s2_flag] : flag_map)
			{
				if (h1_flags & h1_flag)
				{
					result |= s2_flag;
				}
			}

			return result;
		}

		std::uint8_t convert_camera_region(const std::uint8_t camera_region)
		{
			switch (camera_region)
			{
			case CAMERA_REGION_LIT_OPAQUE:
			case CAMERA_REGION_DEPTH_HACK:
			case CAMERA_REGION_PRE_DEPTH_HACK:
			case CAMERA_REGION_PRE_OPAQUE:
				return zonetool::s2::CAMERA_REGION_LIT_OPAQUE;
			case CAMERA_REGION_LIT_OPAQUE_SSS:
			case CAMERA_REGION_DEPTH_HACK_SSS:
			case CAMERA_REGION_PRE_DEPTH_HACK_SSS:
			case CAMERA_REGION_PRE_SSS:
				return zonetool::s2::CAMERA_REGION_LIT_OPAQUE_SSS;
			case CAMERA_REGION_LIT_DECAL:
				return zonetool::s2::CAMERA_REGION_LIT_DECAL;
			case CAMERA_REGION_LIT_TRANS:
			case CAMERA_REGION_DEPTH_HACK_TRANS:
				return zonetool::s2::CAMERA_REGION_LIT_TRANS;
			case CAMERA_REGION_EMISSIVE:
				return zonetool::s2::CAMERA_REGION_EMISSIVE;
			default:
				return zonetool::s2::CAMERA_REGION_NONE;
			}
		}

		native_material convert_native(const Material* asset, const std::string& h1_techset, const techset::native_techset& native,
			utils::memory::allocator& allocator)
		{
			const auto material = allocator.allocate<zonetool::s2::Material>();
			material->info.name = asset->name;
			material->info.surfaceTypeBits = convert_surface_type_bits(static_cast<std::uint64_t>(asset->info.surfaceTypeBits));
			material->materialType = convert_material_type(h1_techset);
			material->info.gameFlags = asset->info.gameFlags;
			material->info.sortKey = convert_sort_key(asset->info.sortKey);
			material->info.drawSurf.fields.primarySortKey = material->info.sortKey;
			material->info.prepassClass = convert_prepass_class(asset, native.techset);
			material->cameraRegion = convert_camera_region(asset->cameraRegion);
			material->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
			material->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			material->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
			material->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
			material->stateFlags = convert_state_flags(asset->stateFlags);
			material->techniqueSet = native.techset;
			material->layerCount = asset->subMaterials ? asset->layerCount : 0;
			material->subMaterials = asset->subMaterials;

			const auto states = convert_native_states(asset, native, material, allocator);
			convert_native_constants(asset, material, allocator);
			convert_native_textures(asset, material, allocator);
			return {material, states};
		}

		void add_image_rows(const zonetool::s2::Material* material)
		{
			for (auto i = 0; i < material->textureCount; i++)
			{
				const auto image = material->textureTable[i].u.image;
				if (image && image->name && !gfximage::is_dumped(image->name))
				{
					add_csv_row("image", image->name, true);
				}
			}
		}

		bool dump_native(Material* asset, const std::string& h1_techset, const bool sprite, utils::memory::allocator& allocator)
		{
			const auto native = techset::convert_native(h1_techset, sprite);
			if (!native)
			{
				return false;
			}

			native_material converted{};
			try
			{
				converted = convert_native(asset, h1_techset, *native, allocator);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("Material \"%s\" is not converted: %s", asset->name, e.what());
				return false;
			}

			techset::dump(native->techset);
			add_csv_row("techset", native->techset->name);
			zonetool::s2::material::dump(converted.material, converted.states);
			add_csv_row("material", asset->name);
			add_image_rows(converted.material);
			return true;
		}

		void convert_material(Material* asset, const bool sprite)
		{
			const auto h1_techset = get_techset_name(asset);
			if (h1_techset.empty())
			{
				ZONETOOL_WARNING("Material \"%s\" has no techset", asset->name);
				return;
			}

			utils::memory::allocator allocator;
			if (sprite && dump_native(asset, h1_techset, true, allocator))
			{
				return;
			}

			if (sprite)
			{
				ZONETOOL_WARNING("Material \"%s\" has no sprite techset, the fx sprites using it are not drawn", asset->name);
			}

			dump_native(asset, h1_techset, false, allocator);
		}

		void convert_sprite(const Material* asset, const std::string& name)
		{
			auto sprite = *asset;
			sprite.name = name.data();
			convert_material(&sprite, true);
		}

		struct minimap_variant
		{
			const char* suffix;
			const char* techset;
			std::uint8_t sort_key;
			std::uint32_t blend;
			const char* alpha_map;
		};

		constexpr std::array<minimap_variant, 3> minimap_variants
		{{
			{"", "2d", 62, 0x03CB1165, nullptr},
			{"_circular", "2d_uia_uiuv", 53, 0x01CB1165, "compass_circular_base_cutout_2"},
			{"_ui3d", "2d", 62, 0x03CB1165, nullptr},
		}};
		constexpr std::array<std::uint32_t, 3> minimap_load_bits{0xA, 0xE00E000C, 0xFFFF};
		constexpr std::uint8_t minimap_color_sampler = 0xE2;
		constexpr std::uint8_t minimap_alpha_sampler = 0xF3;

		std::uint32_t texture_name_hash(const std::string_view name)
		{
			std::uint32_t hash = 0;
			for (const auto c : name)
			{
				hash = (static_cast<std::uint8_t>(c) | 0x20) ^ (hash * 33);
			}

			return hash;
		}

		zonetool::s2::MaterialTextureDef make_texture(const std::string_view name, const std::uint8_t sampler_state, const std::string& image,
			utils::memory::allocator& allocator)
		{
			zonetool::s2::MaterialTextureDef texture{};
			texture.nameHash = texture_name_hash(name);
			texture.nameStart = name.front();
			texture.nameEnd = name.back();
			texture.samplerState = sampler_state;
			texture.u.image = make_image_stub(image, allocator);
			return texture;
		}

		native_material make_minimap_material(const minimap_variant& variant, const std::string& name, const std::string& map_image,
			utils::memory::allocator& allocator)
		{
			const auto material = allocator.allocate<zonetool::s2::Material>();
			material->info.name = allocator.duplicate_string(name);
			material->info.prepassClass = zonetool::s2::PREPASS_CLASS_BLENDED;
			material->info.sortKey = variant.sort_key;
			material->info.textureAtlasRowCount = 1;
			material->info.textureAtlasColumnCount = 1;
			material->cameraRegion = zonetool::s2::CAMERA_REGION_NONE;
			material->techniqueSet = make_stub<zonetool::s2::MaterialTechniqueSet>(variant.techset, allocator);

			std::vector<zonetool::s2::MaterialTextureDef> textures;
			if (variant.alpha_map)
			{
				textures.emplace_back(make_texture("alphaMap", minimap_alpha_sampler, variant.alpha_map, allocator));
			}
			textures.emplace_back(make_texture("colorMap", minimap_color_sampler, map_image, allocator));
			material->textureCount = static_cast<unsigned char>(textures.size());
			material->textureTable = copy_array(textures.data(), textures.size(), allocator);

			constexpr auto unlit_technique_count = 3u; // unlit, unlit srgb gamma and unlit bt2020 pq, one blend class each
			std::ranges::fill(material->stateBitsEntry, no_entry);
			std::ranges::fill(material->constantBufferIndex, no_entry);
			material->stateBitsCount = unlit_technique_count;
			material->stateBitsTable = allocator.allocate_array<zonetool::s2::GfxStateBits>(unlit_technique_count);
			const auto states = allocator.allocate_array<zonetool::s2::material_state>(unlit_technique_count);
			for (auto i = 0u; i < unlit_technique_count; i++)
			{
				auto& state_bits = material->stateBitsTable[i];
				std::ranges::copy(minimap_load_bits, state_bits.loadBits);
				state_bits.loadBits[3] = variant.blend | (1u << (blend_class_shift + i));
				state_bits.rasterizerState = rasterizer_index(state_bits.loadBits[0], state_bits.loadBits[2]);
				states[i] = convert_state(state_bits);
				material->stateBitsEntry[zonetool::s2::TECHNIQUE_UNLIT + i] = static_cast<std::uint8_t>(i);
			}

			return {material, states};
		}
	}

	std::uint8_t convert_sort_key(const std::uint8_t sort_key)
	{
		constexpr auto h1_opaque = 2;
		constexpr auto h1_sky = 3;
		constexpr auto h1_decal_end = 17;
		constexpr auto h1_trans_end = 36;
		constexpr auto h1_2d = 60;
		constexpr auto s2_opaque = 4;
		constexpr auto s2_sky = 6;
		constexpr auto s2_trans_begin = 22;
		constexpr auto max_sort_key = 63;

		switch (sort_key)
		{
		case h1_opaque:
			return s2_opaque;
		case h1_sky:
			return s2_sky;
		default:
			break;
		}

		if (sort_key <= h1_decal_end)
		{
			return static_cast<std::uint8_t>(sort_key + 3);
		}

		if (sort_key <= h1_trans_end)
		{
			return static_cast<std::uint8_t>(std::max(+sort_key, s2_trans_begin));
		}

		if (sort_key >= h1_2d)
		{
			return static_cast<std::uint8_t>(std::min(sort_key + 2, max_sort_key));
		}

		return static_cast<std::uint8_t>(sort_key + 1);
	}

	void dump(Material* asset)
	{
		if (pending_names.insert(asset->name).second)
		{
			pending_materials.emplace_back(asset);
		}
	}

	bool dump_used(const char* name)
	{
		if (techset::is_stock_asset(zonetool::s2::ASSET_TYPE_MATERIAL, name))
		{
			add_csv_row("material", name, true); // the s2 material of the same name is used
			return true;
		}

		auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_MATERIAL, name).material;
		if (!loaded)
		{
			ZONETOOL_WARNING("material \"%s\" not found in the loaded h1 zones", name);
			return false;
		}

		dump(loaded);
		return true;
	}

	const char* dump_sprite(const Material* asset)
	{
		return pending_sprites.try_emplace(asset, std::string(asset->name) + "_sprite").first->second.data();
	}

	void dump_minimap(const std::string& minimap)
	{
		const auto* h1_material = db_find_x_asset_header_safe(ASSET_TYPE_MATERIAL, minimap).material;
		const auto* h1_texture = h1_material && h1_material->textureCount ? &h1_material->textureTable[0] : nullptr;
		const auto map_image = h1_texture ? select_native_image(*h1_texture) : std::string{};
		if (map_image.empty())
		{
			ZONETOOL_WARNING("Minimap material \"%s\" has no image", minimap.data());
			return;
		}

		utils::memory::allocator allocator;
		for (const auto& variant : minimap_variants)
		{
			const auto [material, states] = make_minimap_material(variant, minimap + variant.suffix, map_image, allocator);
			if (variant.alpha_map)
			{
				add_csv_row("image", variant.alpha_map, true);
			}
			add_csv_row("image", map_image);
			add_csv_row("techset", variant.techset, true);
			zonetool::s2::material::dump(material, states);
			add_csv_row("material", material->info.name);
			minimap_materials.insert(material->info.name);
		}
	}

	void finish()
	{
		for (const auto asset : pending_materials)
		{
			if (minimap_materials.contains(asset->name))
			{
				continue;
			}

			try
			{
				convert_material(asset, false);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to convert material \"%s\": %s", asset->name, e.what());
			}
		}

		for (const auto& [asset, name] : pending_sprites)
		{
			try
			{
				convert_sprite(asset, name);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to convert material \"%s\": %s", name.data(), e.what());
			}
		}

		pending_materials.clear();
		pending_names.clear();
		pending_sprites.clear();
		minimap_materials.clear();
	}
}
