#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "rawfile.hpp"

#include <iw8/zone/loader.hpp>

#include <io/game_files.hpp>
#include <utils/string.hpp>
#include <xfile/oodle.hpp>

#include <bit>

namespace zonetool::iw7::converter::iw8::material
{
	namespace
	{
		namespace target = zonetool::iw8;

		const char* const techset_name = "mo/lit_3_lit_rpl_ta1_800800_184a000010001010_0_3_1_0_0_13810015b_0_0_1_0_0_0_0_7818015f_0_1_2_0_0_0_0_b818015f_0_1_2_0_0"; // TODO: stock akilo47_fore techset, convert natively
		const char* const effect_techset_name = "unlit_6_effect_bad_ta0_8a03_4_0_1_0_0_0_100000023_0_0_2_0_0"; // TODO: stock gfx_tracer_new_erad_blue techset, convert natively
		const char* const sprite_techset_name = "elcq/unlit_6_effect_bad_ta0_802_4_0_1_0_0_0_100000023_0_0_2_0_0"; // TODO: stock vfx_ui_marker_1 techset, convert natively
		const char* const atlas_sprite_techset_name = "elcq/unlit_6_effect_bad_ta0_802_1004_0_1_0_0_0_100000023_0_0_2_0_0"; // TODO: stock techsets_common atlas techset, convert natively
		const char* const blend_sprite_techset_name = "elcq/unlit_6_effect_bad_ta0_802_4_0_1_0_0_0_10000020b_0_0_2_0_0"; // TODO: stock techsets_common_mp techset, convert natively
		const char* const blend_atlas_sprite_techset_name = "elcq/unlit_6_effect_bad_ta0_802_1004_0_1_0_0_0_10000020b_0_0_2_0_0"; // TODO: stock vfx_particulate_dust_sheet_1 techset, convert natively
		const char* const hud_techset_name = "cust_2_2d_rpl_ta0_0_0_0_0_0"; // TODO: stock compass_map_mp_rust techset, convert natively

		GfxImage* find_texture(const Material* asset, const TextureSemantic semantic, const char name_start = 0)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				const auto& texture = asset->textureTable[i];
				if (texture.semantic == semantic && texture.image && (!name_start || texture.nameStart == name_start))
				{
					return texture.image;
				}
			}

			return nullptr;
		}

		const MaterialConstantDef* find_constant(const Material* asset, const std::string_view name)
		{
			for (auto i = 0; asset->constantTable && i < asset->constantCount; i++)
			{
				if (std::string_view(asset->constantTable[i].name, strnlen(asset->constantTable[i].name, sizeof(asset->constantTable[i].name))) == name)
				{
					return &asset->constantTable[i];
				}
			}

			return nullptr;
		}

		unsigned int get_name_hash(const std::string_view name)
		{
			auto hash = 0u;
			for (const auto c : name)
			{
				hash = (static_cast<unsigned char>(c) | 0x20) ^ (33 * hash);
			}

			return hash;
		}

		unsigned int get_layer_name_hash(const std::string_view name, const unsigned int layer)
		{
			return get_name_hash(layer ? std::format("{}{}", name, layer) : std::string(name));
		}

		const MaterialTextureDef* find_layer_texture(const Material* asset, const std::string_view name, const unsigned int layer)
		{
			const auto hash = get_layer_name_hash(name, layer);
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].nameHash == hash && asset->textureTable[i].image)
				{
					return &asset->textureTable[i];
				}
			}

			return nullptr;
		}

		GfxImage* find_layer_image(const Material* asset, const std::string_view name, const unsigned int layer, const TextureSemantic semantic)
		{
			const auto texture = find_layer_texture(asset, name, layer);
			return texture && texture->semantic == semantic ? texture->image : nullptr;
		}

		const MaterialConstantDef* find_layer_constant(const Material* asset, const std::string_view name, const unsigned int layer)
		{
			const auto hash = get_layer_name_hash(name, layer);
			for (auto i = 0; asset->constantTable && i < asset->constantCount; i++)
			{
				if (asset->constantTable[i].nameHash == hash)
				{
					return &asset->constantTable[i];
				}
			}

			return nullptr;
		}

		unsigned int get_layer_count(const Material* asset)
		{
			auto count = 1u;
			while (count < 8 && find_layer_texture(asset, "colorMap", count))
			{
				count++;
			}

			return count;
		}

		std::array<std::uint32_t, 4> get_scale_bias_literal(const target::GfxImage* color, const target::GfxImage* normal)
		{
			return {color ? color->semanticSpecific.albedoMapScaleBias : 0x9, normal ? normal->semanticSpecific.normalMapScaleBias : 0x00FF00FF, 0x4F800000, 0};
		}

		target::GfxImage* make_image_reference(const char* name, utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = name;
			return image;
		}

		target::GfxImage* image_or_reference(target::GfxImage* image, const char* fallback, utils::memory::allocator& allocator)
		{
			return image ? image : make_image_reference(fallback, allocator);
		}

		target::MaterialConstantDef convert_color_tint(const Material* asset)
		{
			target::MaterialConstantDef result{};
			result.index = target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT;
			const auto tint = find_constant(asset, "colorTint");
			const std::array<float, 4> white{1.0f, 1.0f, 1.0f, 1.0f};
			std::memcpy(result.literal, tint ? tint->literal : white.data(), sizeof(result.literal));
			return result;
		}

		target::MaterialConstantDef* allocate_constants(const std::vector<target::MaterialConstantDef>& constants, unsigned char& count,
			utils::memory::allocator& allocator)
		{
			count = static_cast<unsigned char>(constants.size());
			if (constants.empty())
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<target::MaterialConstantDef>(constants.size());
			std::memcpy(result, constants.data(), constants.size() * sizeof(target::MaterialConstantDef));
			return result;
		}

		bool has_techset_token(const Material* asset, const std::string_view token)
		{
			if (!asset->techniqueSet || !asset->techniqueSet->name)
			{
				return false;
			}

			const auto parts = utils::string::split(asset->techniqueSet->name, '_');
			return std::ranges::find(parts, token) != parts.end();
		}

		bool is_atlas(const Material* asset)
		{
			return asset->info.textureAtlasRowCount * asset->info.textureAtlasColumnCount > 1;
		}

		bool is_blend_sprite(const Material* asset)
		{
			return !has_techset_token(asset, "add");
		}

		const char* get_sprite_techset_name(const Material* asset)
		{
			if (is_blend_sprite(asset))
			{
				return is_atlas(asset) ? blend_atlas_sprite_techset_name : blend_sprite_techset_name;
			}

			return is_atlas(asset) ? atlas_sprite_techset_name : sprite_techset_name;
		}

		std::vector<target::MaterialConstantDef> convert_sprite_constants(const Material* asset)
		{
			std::vector<target::MaterialConstantDef> constants;
			if (is_blend_sprite(asset))
			{
				auto& parms = constants.emplace_back();
				parms.index = target::MATERIAL_CONSTANT_SPRITE_BLEND_PARMS;
				const std::array<std::uint32_t, 4> stock_parms{0xFFFFFFFF, 0x00FF17E7, 0x0000BC00, 0}; // TODO: stock vfx_particulate_dust_sheet_1 value
				std::memcpy(parms.literal, stock_parms.data(), sizeof(parms.literal));

				auto& tint = constants.emplace_back(convert_color_tint(asset));
				tint.index = target::MATERIAL_CONSTANT_SPRITE_BLEND_TINT;
			}
			else
			{
				constants.push_back(convert_color_tint(asset));
			}

			if (!is_atlas(asset))
			{
				return constants;
			}

			const auto rows = asset->info.textureAtlasRowCount;
			const auto columns = asset->info.textureAtlasColumnCount;

			auto& size = constants.emplace_back();
			size.index = target::MATERIAL_CONSTANT_ATLAS_SIZE;
			size.literal[0] = static_cast<float>(columns);
			size.literal[1] = static_cast<float>(rows);
			size.literal[2] = 1.0f / columns;
			size.literal[3] = 1.0f / rows;

			const std::array<std::int32_t, 4> indexing{rows * columns, columns - 1, std::bit_width(columns) - 1, 0};
			auto& indexing_constant = constants.emplace_back();
			indexing_constant.index = target::MATERIAL_CONSTANT_ATLAS_INDEXING;
			std::memcpy(indexing_constant.literal, indexing.data(), sizeof(indexing_constant.literal));
			return constants;
		}

		target::Material* convert_effect(const Material* asset, const bool sprite, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = asset->name;
			result->contents = 1;
			result->surfaceFlags = sprite ? 0 : 0x10000;
			result->materialType = sprite ? target::MTL_TYPE_EFFECT_QUAD : target::MTL_TYPE_DEFAULT;
			result->cameraRegion = target::CAMERA_REGION_EMISSIVE;
			result->sortKey = asset->info.sortKey;
			result->flags = 0x10;
			result->textureAtlasRowCount = asset->info.textureAtlasRowCount;
			result->textureAtlasColumnCount = asset->info.textureAtlasColumnCount;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = sprite ? get_sprite_techset_name(asset) : effect_techset_name;

			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'c'); image && sprite && is_blend_sprite(asset))
			{
				result->textureCount = 2;
				result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(2);
				result->textureTable[0].index = 0;
				result->textureTable[0].image = image_or_reference(gfximage::dump_color_premultiplied(image), "$black", allocator);
				result->textureTable[1].index = 0x44;
				result->textureTable[1].image = image_or_reference(gfximage::dump_transmission(image, true), "$white", allocator);
			}
			else
			{
				result->textureCount = 1;
				result->textureTable = allocator.allocate<target::MaterialTextureDef>();
				result->textureTable->index = 18;
				result->textureTable->image = image ? image_or_reference(gfximage::dump_color(image), "$black", allocator) : make_image_reference("$white", allocator);
			}

			result->constantTable = allocate_constants(sprite ? convert_sprite_constants(asset) : std::vector{convert_color_tint(asset)}, result->constantCount, allocator);
			return result;
		}

		bool is_model_effect(const Material* asset)
		{
			return has_techset_token(asset, "unlit") && has_techset_token(asset, "add");
		}

		bool is_two_sided(const Material* asset)
		{
			return (asset->stateFlags & STATE_FLAG_CULL_MASK) != STATE_FLAG_CULL_BACK;
		}

		const char* get_model_effect_techset_name(const Material* asset) // TODO: stock techsets_common_base_mp techsets, convert natively
		{
			return is_two_sided(asset) ? "mo/unlit_6_effect_bad_ta0_8902_64_0_1_0_0_0_100000023_0_0_2_0_0"
				: "mo/unlit_6_effect_bad_ta0_4102_64_0_1_0_0_0_100000023_0_0_2_0_0";
		}

		target::Material* convert_model_effect(const Material* asset, utils::memory::allocator& allocator)
		{
			const auto result = convert_effect(asset, false, allocator);
			result->materialType = target::MTL_TYPE_MODEL_SELFVIS;
			result->techniqueSet->name = get_model_effect_techset_name(asset);

			target::MaterialConstantDef parms{};
			parms.index = target::MATERIAL_CONSTANT_MODEL_EFFECT_PARMS;
			const auto stock_parms = is_two_sided(asset) ? std::array{0.1f, 10.0f, 10.0f, 0.025f} : std::array{2.0f, 0.5f, 0.5f, 0.3333f}; // TODO: stock vfx_drone_blast_radius_mat and vfx_test_burning_plate_1 values
			std::memcpy(parms.literal, stock_parms.data(), sizeof(parms.literal));
			result->constantTable = allocate_constants({convert_color_tint(asset), parms}, result->constantCount, allocator);
			return result;
		}

		target::Material* convert_2d(const Material* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = asset->name;
			result->contents = 1;
			result->surfaceFlags = 0x10000;
			result->materialType = target::MTL_TYPE_DEFAULT;
			result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
			result->sortKey = 30;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = hud_techset_name;

			const auto source = find_texture(asset, TS_2D);
			const auto image = source ? gfximage::dump_2d(source) : nullptr;
			result->textureCount = 1;
			result->textureTable = allocator.allocate<target::MaterialTextureDef>();
			result->textureTable->index = 158;
			result->textureTable->image = image ? image : make_image_reference("$white", allocator);

			auto tint = convert_color_tint(asset);
			tint.index = target::MATERIAL_CONSTANT_2D_COLOR_TINT;
			result->constantTable = allocate_constants({tint}, result->constantCount, allocator);

			result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
			std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
			result->constantBufferIndex[target::TECHNIQUE_EMISSIVE] = 0;

			result->constantBufferCount = 1;
			result->constantBufferTable = allocator.allocate<target::MaterialConstantBufferDef>();
			result->constantBufferTable->psDataSize = sizeof(tint.literal);
			result->constantBufferTable->psData = allocator.allocate_array<char>(sizeof(tint.literal));
			std::memcpy(result->constantBufferTable->psData, tint.literal, sizeof(tint.literal));
			return result;
		}

		std::array<float, 3> get_color_tint(const Material* asset, const unsigned int layer = 0)
		{
			const auto tint = find_layer_constant(asset, "colorTint", layer);
			return tint ? std::array{tint->literal[0], tint->literal[1], tint->literal[2]} : std::array{1.0f, 1.0f, 1.0f};
		}

		target::GfxImage* find_color_image(const Material* asset, const bool bake_tint, const unsigned int layer = 0, const bool premultiply_alpha = false)
		{
			const auto tint = bake_tint ? get_color_tint(asset, layer) : std::array{1.0f, 1.0f, 1.0f};
			const auto dump_unpacked_color = [&](GfxImage* image, const GfxImage* specular_gloss)
			{
				return specular_gloss ? gfximage::dump_color_specular(image, specular_gloss, tint, premultiply_alpha) : gfximage::dump_color(image, tint); // TODO: glass without a specular map is not premultiplied
			};

			if (const auto texture = find_layer_texture(asset, "colorMap", layer))
			{
				return texture->semantic == TS_COLOR_SPECULAR_MAP ? gfximage::dump_color_specular(texture->image, tint)
					: dump_unpacked_color(texture->image, find_layer_image(asset, "specularMap", layer, TS_SPECULAR_MAP));
			}

			if (layer)
			{
				return nullptr;
			}

			if (const auto image = find_texture(asset, TS_COLOR_SPECULAR_MAP))
			{
				return gfximage::dump_color_specular(image, tint);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				return dump_unpacked_color(image, find_texture(asset, TS_SPECULAR_MAP));
			}

			return nullptr;
		}

		GfxImage* find_alpha_texture(const Material* asset)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image;
			}

			return find_texture(asset, TS_COLOR_MAP, 'c');
		}

		target::GfxImage* find_alpha_image(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image->name[0] == '$' ? make_image_reference(image->name, allocator) : gfximage::dump_alpha(image);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				return gfximage::dump_color(image);
			}

			return nullptr;
		}

		target::GfxImage* find_normal_image(const Material* asset, const unsigned int layer = 0)
		{
			const auto normal = find_layer_texture(asset, "normalMap", layer);
			if (!normal && !layer)
			{
				if (const auto image = find_texture(asset, TS_NORMAL_OCCLUSSION_GLOSS_MAP))
				{
					return gfximage::dump_normal_gloss(image);
				}

				return gfximage::dump_normal_gloss(find_texture(asset, TS_NORMAL_MAP, 'n'), find_texture(asset, TS_SPECULAR_MAP),
					find_texture(asset, TS_SPECULAR_OCCLUSION_MAP));
			}

			if (normal && normal->semantic == TS_NORMAL_OCCLUSSION_GLOSS_MAP)
			{
				return gfximage::dump_normal_gloss(normal->image);
			}

			return gfximage::dump_normal_gloss(normal ? normal->image : nullptr, find_layer_image(asset, "specularMap", layer, TS_SPECULAR_MAP),
				find_layer_image(asset, "specOcclusionMap", layer, TS_SPECULAR_OCCLUSION_MAP));
		}

		target::MaterialConstantDef convert_constant(const target::MaterialConstantIndex index, const MaterialConstantDef* source)
		{
			target::MaterialConstantDef result{};
			result.index = index;
			std::memcpy(result.literal, source->literal, sizeof(result.literal));
			return result;
		}

		target::MaterialConstantDef convert_constant(const target::MaterialConstantIndex index, const MaterialConstantDef* source,
			const std::array<float, 4>& fallback)
		{
			target::MaterialConstantDef result{};
			result.index = index;
			std::memcpy(result.literal, source ? source->literal : fallback.data(), sizeof(result.literal));
			return result;
		}

		bool has_vertex_color(const Material* asset)
		{
			if (!asset->techniqueSet || !asset->techniqueSet->name)
			{
				return false;
			}

			const std::string_view name = asset->techniqueSet->name;
			const auto prefix = name.substr(0, name.find('_'));
			return prefix.size() >= 2 && prefix[1] == 'c';
		}

		std::array<float, 4> get_mean_vertex_color(const Material* asset)
		{
			std::array<double, 4> sum{};
			std::array<float, 4> min{1.0f, 1.0f, 1.0f, 1.0f};
			std::array<float, 4> max{};
			std::size_t count = 0;
			db_enum_assets(ASSET_TYPE_XMODEL, [&](void* header)
			{
				const auto* model = static_cast<const XModel*>(header);
				for (auto lod = 0; model->materialHandles && lod < model->numLods; lod++)
				{
					const auto& info = model->lodInfo[lod];
					auto* surfs = info.modelSurfs;
					if (surfs && !surfs->surfs && surfs->name)
					{
						surfs = static_cast<XModelSurfs*>(db_find_asset(ASSET_TYPE_XMODEL_SURFS, surfs->name));
					}

					for (auto i = 0; surfs && surfs->surfs && i < surfs->numsurfs; i++)
					{
						const auto& surface = surfs->surfs[i];
						const auto* material = model->materialHandles[info.surfIndex + i];
						if (!material || !material->name || std::strcmp(material->name, asset->name) || !surface.verts0.packedVerts0
							|| (surface.flags & SURF_FLAG_REACTIVE_MOTION))
						{
							continue;
						}

						for (auto vertex = 0; vertex < surface.vertCount; vertex++)
						{
							for (auto channel = 0u; channel < 4; channel++)
							{
								const auto value = surface.verts0.packedVerts0[vertex].color.array[channel] / 255.0f;
								sum[channel] += value;
								min[channel] = std::min(min[channel], value);
								max[channel] = std::max(max[channel], value);
							}
						}

						count += surface.vertCount;
					}
				}
			});

			if (!count)
			{
				return {1.0f, 1.0f, 1.0f, 1.0f};
			}

			std::array<float, 4> mean{};
			for (auto channel = 0u; channel < 4; channel++)
			{
				mean[channel] = static_cast<float>(sum[channel] / count);
				if (max[channel] - min[channel] > 0.1f)
				{
					ZONETOOL_WARNING("Material \"%s\" has a vertex color that is not uniform (channel %u: %.2f to %.2f)", asset->name, channel, min[channel], max[channel]);
				}
			}

			return mean;
		}

		std::array<float, 4> get_effect_vertex_color(const Material* asset, const techset::generated_variant& variant)
		{
			if (variant.techset != techset::GENERATED_TECHSET_STATIC_MODEL)
			{
				return {1.0f, 1.0f, 1.0f, 1.0f};
			}

			const auto color = get_mean_vertex_color(asset);
			ZONETOOL_WARNING("Material \"%s\" uses the mean vertex color (%.2f %.2f %.2f %.2f) on static models", asset->name, color[0], color[1], color[2], color[3]);
			return color;
		}

		std::vector<target::MaterialConstantDef> convert_effect_constants(const Material* asset, const techset::generated_variant& variant,
			const std::array<float, 4>& vertex_color)
		{
			const auto falloff = has_techset_token(asset, "falloff");
			const auto find_falloff_constant = [&](const std::string_view name)
			{
				return falloff ? find_layer_constant(asset, name, 0) : nullptr;
			};

			if (has_techset_token(asset, "zfeather"))
			{
				ZONETOOL_WARNING("Material \"%s\" loses its depth feather", asset->name);
			}

			std::vector<target::MaterialConstantDef> constants =
			{
				convert_constant(target::MATERIAL_CONSTANT_FALLOFF_PARMS, find_falloff_constant("falloffParms"), {0.0f, 0.0f, 0.0f, 1.0f}),
				convert_constant(target::MATERIAL_CONSTANT_FALLOFF_BEGIN_COLOR, find_falloff_constant("falloffBeginColor"), {1.0f, 1.0f, 1.0f, 1.0f}),
				convert_constant(target::MATERIAL_CONSTANT_FALLOFF_END_COLOR, find_falloff_constant("falloffEndColor"), {1.0f, 1.0f, 1.0f, 1.0f}),
				convert_constant(target::MATERIAL_CONSTANT_EYE_OFFSET_PARMS,
					has_techset_token(asset, "eyeoffset") ? find_layer_constant(asset, "eyeOffsetParms", 0) : nullptr, {0.0f, 0.0f, 0.0f, 0.0f}),
			};

			for (auto& constant : std::span(constants).subspan(1, 2))
			{
				for (auto channel = 0u; channel < 3; channel++)
				{
					constant.literal[channel] *= vertex_color[channel];
				}
			}

			if (variant.near_fade)
			{
				constants.push_back(convert_constant(target::MATERIAL_CONSTANT_FEATHER_PARMS, find_layer_constant(asset, "featherParms", 0)));
			}

			return constants;
		}

		target::CameraRegion get_blend_camera_region(const Material* asset)
		{
			switch (asset->cameraRegion)
			{
			case CAMERA_REGION_LIT_OPAQUE:
			case CAMERA_REGION_LIT_DECAL:
				return target::CAMERA_REGION_LIT_DECAL;
			case CAMERA_REGION_EMISSIVE:
				return target::CAMERA_REGION_EMISSIVE;
			default:
				return target::CAMERA_REGION_LIT_TRANS;
			}
		}

		techset::generated_variant get_variant(const Material* asset, const techset::generated_techset techset)
		{
			techset::generated_variant variant{.techset = techset};
			const auto effect = has_techset_token(asset, "effect") && !has_techset_token(asset, "lit");
			variant.lit = !has_techset_token(asset, "unlit") && !effect;
			variant.exposure_compensated = !variant.lit && !has_techset_token(asset, "multiply") && has_techset_token(asset, "evc");
			variant.uv_anim = has_techset_token(asset, "ua") && find_constant(asset, "uvAnimParms");
			variant.two_sided = (asset->stateFlags & STATE_FLAG_CULL_MASK) != STATE_FLAG_CULL_BACK;
			variant.casts_shadow = !has_techset_token(asset, "nocast");

			const auto alpha_test = has_techset_token(asset, "atest") && find_alpha_texture(asset);
			if (has_techset_token(asset, "onlycast") || asset->cameraRegion == CAMERA_REGION_NONE_2)
			{
				variant.visible = false;
				variant.alpha_test = alpha_test;
				return variant;
			}

			const auto blend_techset = has_techset_token(asset, "blend") || has_techset_token(asset, "add") || has_techset_token(asset, "multiply");
			if (blend_techset || asset->cameraRegion == CAMERA_REGION_LIT_DECAL || asset->cameraRegion == CAMERA_REGION_LIT_TRANS
				|| asset->cameraRegion == CAMERA_REGION_EMISSIVE)
			{
				variant.blend = has_techset_token(asset, "add") ? techset::GENERATED_BLEND_ADD
					: has_techset_token(asset, "multiply") ? techset::GENERATED_BLEND_MULTIPLY
					: techset::GENERATED_BLEND_ALPHA;
				variant.casts_shadow = false;
				variant.water = variant.lit && variant.uv_anim && variant.blend == techset::GENERATED_BLEND_ALPHA
					&& find_layer_constant(asset, "oceanScrollNormal0", 0) && find_layer_constant(asset, "oceanScrollNormal1", 0);
				variant.effect = effect && variant.blend == techset::GENERATED_BLEND_ADD && !variant.uv_anim && !variant.exposure_compensated;
				variant.near_fade = variant.effect && has_techset_token(asset, "zfeather") && find_layer_constant(asset, "featherParms", 0);
				variant.vertex_color = techset == techset::GENERATED_TECHSET_WORLD && !variant.effect && !variant.water && has_vertex_color(asset)
					&& ((variant.blend == techset::GENERATED_BLEND_ADD && !variant.lit) || variant.blend == techset::GENERATED_BLEND_MULTIPLY);
				return variant;
			}

			variant.alpha_test = alpha_test;
			return variant;
		}

		std::string get_lit_name(const char* name, const techset::generated_techset techset)
		{
			const auto prefix = techset == techset::GENERATED_TECHSET_STATIC_MODEL ? "tmo" : "tw";
			if (name[0] == '*')
			{
				return std::format("{}/maps/mp/generated/{}", prefix, name + 1); // '*' is not a valid file name
			}

			return std::format("{}/{}", prefix, name);
		}

		target::Material* convert_lit(const Material* asset, const std::string& name, const techset::generated_variant& variant,
			utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 1;
			result->surfaceFlags = 0xB80000;
			result->materialType = techset::get_material_type(variant.techset);
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 2;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			if (variant.blend != techset::GENERATED_BLEND_OPAQUE)
			{
				result->cameraRegion = get_blend_camera_region(asset);
				result->sortKey = asset->info.sortKey;
			}

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = allocator.duplicate_string(techset::get_techset_name(variant));

			const auto color = find_color_image(asset, variant.lit && !variant.water);
			const auto uses_alpha = !variant.water && (variant.alpha_test || variant.blend != techset::GENERATED_BLEND_OPAQUE);
			result->textureCount = uses_alpha || variant.water ? 2 : 1;
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(result->textureCount);
			result->textureTable[0].index = 0;
			result->textureTable[0].image = color ? color : make_image_reference("$black", allocator);

			if (uses_alpha)
			{
				const auto alpha = find_alpha_image(asset, allocator);
				result->textureTable[1].index = 27;
				result->textureTable[1].image = alpha ? alpha : make_image_reference("$white", allocator);
			}

			if (variant.water)
			{
				const auto normal = find_normal_image(asset);
				result->textureTable[1].index = 9;
				result->textureTable[1].image = normal ? normal : make_image_reference("$identitynormalmap", allocator);
			}

			std::vector<target::MaterialConstantDef> constants;
			if ((!variant.lit || variant.water) && variant.visible)
			{
				constants.push_back(convert_color_tint(asset));
			}

			if (variant.uv_anim)
			{
				constants.push_back(convert_constant(target::MATERIAL_CONSTANT_UV_ANIM, find_constant(asset, "uvAnimParms")));
			}

			if (variant.water)
			{
				constants.push_back(convert_constant(target::MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_0, find_layer_constant(asset, "oceanScrollNormal0", 0)));
				constants.push_back(convert_constant(target::MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_1, find_layer_constant(asset, "oceanScrollNormal1", 0)));
			}

			if (variant.effect)
			{
				const auto vertex_color = get_effect_vertex_color(asset, variant);
				for (auto& constant : constants)
				{
					if (constant.index == target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT)
					{
						std::ranges::for_each(std::span(constant.literal).first(3), [&](float& value) { value *= vertex_color[3]; });
					}
				}

				std::ranges::copy(convert_effect_constants(asset, variant, vertex_color), std::back_inserter(constants));
			}

			result->constantTable = allocate_constants(constants, result->constantCount, allocator);

			result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
			std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
			return result;
		}

		target::Material* convert_sky(const std::string& name, const GfxImage* sky_image, const std::array<float, 3>& rotation_angles,
			utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 0x800;
			result->surfaceFlags = 0x34;
			result->materialType = techset::get_material_type(techset::GENERATED_TECHSET_SKY);
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 3;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = allocator.duplicate_string(techset::get_techset_name({.techset = techset::GENERATED_TECHSET_SKY}));

			const auto image = sky_image ? gfximage::dump_sky(sky_image, rotation_angles, rawfile::get_sky_color_scale()) : nullptr;
			result->textureCount = 1;
			result->textureTable = allocator.allocate<target::MaterialTextureDef>();
			result->textureTable->index = 0;
			result->textureTable->image = image ? image : make_image_reference("$black", allocator);

			result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
			std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
			return result;
		}

		const char* get_donor_techset(const techset::generated_variant& variant) // TODO: stock mp_rust techsets, convert natively
		{
			if (!variant.visible || !variant.lit || variant.uv_anim)
			{
				return nullptr;
			}

			const auto world = variant.techset == techset::GENERATED_TECHSET_WORLD;
			if (variant.blend == techset::GENERATED_BLEND_ALPHA)
			{
				return world ? "tw/lit_3_lit_bad_tca_815002_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0"
					: "tmo/lit_3_lit_bad_tca_284002_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			}

			if (variant.blend != techset::GENERATED_BLEND_OPAQUE)
			{
				return nullptr;
			}

			if (world)
			{
				return variant.alpha_test || variant.two_sided ? "tw/lit_3_lit_alt_ta1_808800_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0"
					: "tw/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810015b_0_0_1_0_0";
			}

			if (variant.alpha_test)
			{
				return variant.two_sided ? "tmo/lit_3_lit_alt_ta1_808800_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0"
					: "tmo/lit_3_lit_alt_ta1_800800_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0";
			}

			return variant.two_sided ? "tmo/lit_3_lit_rpl_ta1_808800_1002000000000010_0_1_1_0_0_13810015b_0_0_1_0_0"
				: "tmo/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810015b_0_0_1_0_0";
		}

		const char* get_flag_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock mp_rust techset, convert natively
		{
			if (variant.techset != techset::GENERATED_TECHSET_STATIC_MODEL || !variant.visible || !variant.lit || variant.uv_anim
				|| variant.blend != techset::GENERATED_BLEND_OPAQUE || !has_techset_token(asset, "flagext") || !find_constant(asset, "flagParms"))
			{
				return nullptr;
			}

			return "tmco/lit_3_lit_alt_ta1_808810_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0";
		}

		const char* get_distortion_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock mp_rust techset, convert natively
		{
			if (variant.techset != techset::GENERATED_TECHSET_WORLD || !has_techset_token(asset, "distortion") || !find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				return nullptr;
			}

			return "twc/dist_10_distortion_rpl_ta0_0_3_0_0_0";
		}

		const char* get_glass_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock IW8 glass techset, convert natively
		{
			if (!variant.visible || !variant.lit || variant.uv_anim || variant.blend != techset::GENERATED_BLEND_ALPHA
				|| asset->cameraRegion != CAMERA_REGION_LIT_TRANS || (!has_techset_token(asset, "blendadd") && !has_techset_token(asset, "blendaddrgb")))
			{
				return nullptr;
			}

			return variant.techset == techset::GENERATED_TECHSET_WORLD ? "tw/lit_3_lit_bad_ta1_c02_1002000000000011_0_1_1_0_0_13810035b_0_0_1_0_0"
				: "tmo/lit_3_lit_bad_ta1_c02_1002000000000011_0_1_1_0_0_13810035b_0_0_1_0_0";
		}

		const char* get_frosted_glass_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock mp_donetsk techset, convert natively
		{
			if (variant.techset != techset::GENERATED_TECHSET_WORLD || !has_techset_token(asset, "sraf0") || !get_glass_donor_techset(asset, variant))
			{
				return nullptr;
			}

			return "twc/lit_3_lit_bad_ta1_1002_1002000000000015_0_1_1_0_0_13810075b_0_0_1_0_0";
		}

		const char* get_emissive_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock mp_aniyah_tac techsets, convert natively
		{
			if (variant.techset == techset::GENERATED_TECHSET_WORLD || !variant.visible || !variant.lit || variant.uv_anim
				|| variant.blend != techset::GENERATED_BLEND_OPAQUE || variant.alpha_test || !find_texture(asset, TS_COLOR_MAP, 'e'))
			{
				return nullptr;
			}

			return variant.two_sided ? "tmo/lit_3_lit_rpl_ta1_808800_1002000000000010_0_1_1_0_0_13810017b_0_0_1_0_0"
				: "tmo/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810017b_0_0_1_0_0";
		}

		const char* get_layered_donor_techset(const Material* asset, const techset::generated_variant& variant) // TODO: stock mp_rust techsets, convert natively
		{
			if (variant.techset != techset::GENERATED_TECHSET_WORLD || !variant.visible || !variant.lit || variant.uv_anim
				|| variant.blend != techset::GENERATED_BLEND_OPAQUE || variant.alpha_test || !asset->techniqueSet || !asset->techniqueSet->name)
			{
				return nullptr;
			}

			switch (get_layer_count(asset))
			{
			case 2:
				return "twc4/lit_3_lit_rpl_ta1_800800_1002000000000010_0_2_2_0_0_13810015b_0_0_1_0_0_1_0_13810015b_0_0_3_0_0";
			case 3:
				return "twc4/lit_3_lit_rpl_ta1_800800_1002000000000010_0_3_3_0_0_13810015b_0_0_1_0_0_1_0_13810015b_0_0_3_0_0_2_0_13810015b_0_0_3_0_0";
			case 4:
				return "twc4/lit_3_lit_rpl_ta1_800800_1002000000000010_0_4_4_0_0_13810015b_0_0_1_0_0_1_0_13810015b_0_0_3_0_0_2_0_13810015b_0_0_3_0_0"
					"_3_0_13810015b_0_0_3_0_0";
			case 5:
				return "twc4/lit_3_lit_rpl_ta1_800800_1002000000000010_0_5_5_0_0_13810015b_0_0_1_0_0_1_0_13810015b_0_0_3_0_0_2_0_13810015b_0_0_3_0_0"
					"_3_0_13810015b_0_0_3_0_0_4_0_13810015b_0_0_3_0_0";
			default:
				return nullptr;
			}
		}

		std::pair<const char*, donor_kind> get_donor(const Material* asset, const techset::generated_variant& variant)
		{
			if (const auto donor = get_layered_donor_techset(asset, variant))
			{
				return {donor, donor_kind::layered};
			}

			if (const auto donor = get_flag_donor_techset(asset, variant))
			{
				return {donor, donor_kind::flag_wave};
			}

			if (const auto donor = get_frosted_glass_donor_techset(asset, variant))
			{
				return {donor, donor_kind::frosted_glass};
			}

			if (const auto donor = get_glass_donor_techset(asset, variant))
			{
				return {donor, donor_kind::glass};
			}

			if (const auto donor = get_emissive_donor_techset(asset, variant))
			{
				return {donor, donor_kind::emissive};
			}

			return {get_donor_techset(variant), donor_kind::lit};
		}

		std::array<std::uint32_t, 4> get_emissive_tint(const Material* asset)
		{
			const auto source = find_constant(asset, "emissivePara");
			const auto scale = std::bit_cast<std::uint32_t>(source ? source->literal[0] : 1.0f);
			return {scale, scale, scale, 0x3F800000};
		}

		std::array<std::uint32_t, 4> get_transmission_tint(const Material* asset)
		{
			std::array<std::uint32_t, 4> result{0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000};
			if (const auto source = find_constant(asset, "absorptionSc"))
			{
				std::memcpy(result.data(), source->literal, sizeof(result));
			}

			return result;
		}

		std::unordered_set<std::string> donor_techsets;

		target::MaterialTextureDef* convert_textures(target::GfxImage* color, target::GfxImage* normal, unsigned char& count,
			utils::memory::allocator& allocator)
		{
			const auto black = make_image_reference("$black", allocator);
			color = color ? color : black;
			normal = normal ? normal : make_image_reference("$identitynormalmap", allocator);

			const std::array<std::pair<unsigned char, target::GfxImage*>, 15> slots
			{{
				{0, color}, {1, color}, {2, color},
				{9, normal}, {10, normal}, {11, normal},
				{28, black}, {29, black}, {54, black}, {55, black}, {59, black}, {60, black}, {61, black}, {123, black}, {124, black},
			}};

			count = static_cast<unsigned char>(slots.size());
			const auto textures = allocator.allocate_array<target::MaterialTextureDef>(slots.size());
			for (auto i = 0u; i < slots.size(); i++)
			{
				textures[i].index = slots[i].first;
				textures[i].image = slots[i].second;
			}

			return textures;
		}

		target::MaterialConstantDef* convert_constants(const target::GfxImage* color, const target::GfxImage* normal, unsigned char& count,
			utils::memory::allocator& allocator)
		{
			const std::array<std::pair<unsigned char, std::array<std::uint32_t, 4>>, 11> constants
			{{
				{0, {0x00000007, 0x80003C00, 0x3C003C00, 0}},
				{1, {0x0000022F, 0x80003C00, 0x3C003C00, 0}},
				{2, {0x0000022F, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{10, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{11, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{54, get_scale_bias_literal(color, normal)},
				{72, {0x40400000, 0x00000001, 0x3FD9364E, 0x3FD9364E}},
				{98, {0x00000000, 0x3F800000, 0x00000000, 0}},
				{103, {0x3E020C4A, 0x3D656042, 0x3C3E0DED, 0x3D8B0959}},
				{104, {0x3E4CCCCD, 0x3E4CCCCD, 0x3E4CCCCD, 0}},
			}};

			count = static_cast<unsigned char>(constants.size());
			const auto result = allocator.allocate_array<target::MaterialConstantDef>(constants.size());
			for (auto i = 0u; i < constants.size(); i++)
			{
				result[i].index = constants[i].first;
				std::memcpy(result[i].literal, constants[i].second.data(), sizeof(result[i].literal));
			}

			return result;
		}

		target::Material* convert(const Material* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = asset->name;
			result->contents = 1;
			result->surfaceFlags = 0xB00000;
			result->materialType = target::MTL_TYPE_MODEL_SELFVIS;
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 2;
			result->flags = 0x19;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = techset_name;

			const auto color = find_color_image(asset, true);
			const auto normal = find_normal_image(asset);
			result->textureTable = convert_textures(color, normal, result->textureCount, allocator);
			result->constantTable = convert_constants(color, normal, result->constantCount, allocator);
			return result;
		}

		std::vector<std::string> get_layer_tokens(const Material* asset, const unsigned int layer)
		{
			const auto digit = static_cast<char>('0' + layer);
			for (const auto& part : utils::string::split(asset->techniqueSet->name, '_'))
			{
				if (part.size() > 2 && std::string_view(layer ? "bmt" : "ri").find(part[0]) != std::string_view::npos && part[1] == digit)
				{
					return utils::string::split(part, digit);
				}
			}

			return {};
		}

		bool has_layer_token(const Material* asset, const unsigned int layer, const std::initializer_list<std::string_view> names)
		{
			const auto tokens = get_layer_tokens(asset, layer);
			return std::ranges::any_of(names, [&](const std::string_view name) { return std::ranges::find(tokens, name) != tokens.end(); });
		}

		bool has_layer_normal_transform(const Material* asset, const unsigned int layer)
		{
			return has_layer_token(asset, 0, {"n", "nl"}) && has_layer_token(asset, layer, {"nl", "nal", "nbl"});
		}

		GfxImage* find_layer_coverage_image(const Material* asset, const unsigned int layer)
		{
			const auto color = find_layer_image(asset, "colorMap", layer, TS_COLOR_MAP);
			return color && color->name[0] != '$' ? color : nullptr;
		}

		target::GfxImage* find_layer_mask(const Material* asset, const unsigned int layer, utils::memory::allocator& allocator)
		{
			if (const auto mask = find_layer_image(asset, "specOcclusionMap", layer, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image_or_reference(gfximage::dump_layer_mask(mask), "$white", allocator);
			}

			if (const auto coverage = find_layer_coverage_image(asset, layer))
			{
				const auto reveal = find_layer_image(asset, "revealMap", layer, TS_COLOR_MAP);
				return image_or_reference(reveal ? gfximage::dump_layer_mask(coverage, reveal) : gfximage::dump_opacity(coverage), "$white", allocator);
			}

			return make_image_reference("$white", allocator);
		}

		unsigned int get_layer_flags(const Material* asset, const unsigned int layer)
		{
			if (!layer)
			{
				return target::MATERIAL_LAYER_BLEND_NORMAL | target::MATERIAL_LAYER_BLEND_OCCLUSION | target::MATERIAL_LAYER_BLEND_GLOSS;
			}

			const auto tokens = get_layer_tokens(asset, layer);
			const auto has_token = [&](const std::initializer_list<std::string_view> names)
			{
				return std::ranges::any_of(names, [&](const std::string_view name) { return std::ranges::find(tokens, name) != tokens.end(); });
			};

			auto flags = 0u;
			if (has_layer_normal_transform(asset, layer))
			{
				flags |= target::MATERIAL_LAYER_BLEND_NORMAL; // TODO: nal is additive in IW7
			}

			if (has_token({"o", "ol"}))
			{
				flags |= target::MATERIAL_LAYER_BLEND_OCCLUSION;
			}

			if (has_token({"s", "sl"}))
			{
				flags |= target::MATERIAL_LAYER_BLEND_GLOSS;
			}

			if (!tokens.empty() && tokens.front() == "m")
			{
				flags |= target::MATERIAL_LAYER_COLOR_MULTIPLY;
			}

			const auto packed_mask = find_layer_image(asset, "specOcclusionMap", layer, TS_ALPHA_REVEAL_THICKNESS_MAP);
			if ((packed_mask && has_token({"pa"})) || (!packed_mask && find_layer_coverage_image(asset, layer)))
			{
				flags |= target::MATERIAL_LAYER_BLEND_MASK; // TODO: IW7 t layers use alpha test at 0.5
			}

			const auto has_reveal = packed_mask ? has_token({"v"}) : has_token({"vl"}) && find_layer_image(asset, "revealMap", layer, TS_COLOR_MAP);
			if (has_reveal && find_layer_constant(asset, "revealParams", layer))
			{
				flags |= target::MATERIAL_LAYER_BLEND_HEIGHT;
			}

			return flags;
		}

		std::uint32_t get_layer_tint(const Material* asset, const unsigned int layer)
		{
			const auto reveal = layer ? find_layer_constant(asset, "revealParams", layer) : nullptr;
			const auto sharpness = reveal ? std::clamp(std::lround(reveal->literal[0] * 255.0f), 0l, 255l) : 255l;
			return (static_cast<std::uint32_t>(sharpness) << 24) | 0xFFFFFF;
		}

		target::GfxImage* find_opacity_image(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image->name[0] == '$' ? make_image_reference(image->name, allocator) : gfximage::dump_alpha(image);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				return gfximage::dump_opacity(image);
			}

			return nullptr;
		}

		target::GfxImage* find_transmission_image(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'a'))
			{
				return gfximage::dump_transmission(image, false);
			}

			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return gfximage::dump_transmission(image, false);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				return gfximage::dump_transmission(image, true);
			}

			return make_image_reference("$white", allocator);
		}

		target::Material* convert_donor(const Material* asset, const std::string& name, const techset::generated_variant& variant, const char* donor,
			const donor_kind kind, utils::memory::allocator& allocator)
		{
			const auto flag_wave = kind == donor_kind::flag_wave;
			const auto frosted = kind == donor_kind::frosted_glass;
			const auto glass = kind == donor_kind::glass || frosted;
			const auto emissive = kind == donor_kind::emissive;
			const auto world = variant.techset == techset::GENERATED_TECHSET_WORLD;
			const auto blend = variant.blend != techset::GENERATED_BLEND_OPAQUE;
			const auto opacity = !glass && (flag_wave || blend || variant.alpha_test || (world && variant.two_sided));

			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 1;
			result->surfaceFlags = glass ? 0x480000 : 0xB80000;
			result->materialType = flag_wave ? target::MTL_TYPE_STATIC_MODEL_VERTCOL_SELFVIS : techset::get_material_type(variant.techset);
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 2;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			if (frosted)
			{
				result->materialType = target::MTL_TYPE_WORLD_VERTCOL;
			}

			if (glass)
			{
				result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
				result->sortKey = frosted ? 24 : 25;
			}
			else if (blend)
			{
				result->cameraRegion = get_blend_camera_region(asset);
				result->sortKey = asset->info.sortKey;
			}

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = donor;

			const auto color = find_color_image(asset, true, 0, glass); // TODO: IW7 glass emissive 'e' has no input in the glass donors
			const auto normal = find_normal_image(asset);
			std::vector<target::MaterialTextureDef> textures
			{
				{0, color ? color : make_image_reference("$black", allocator)},
				{9, normal ? normal : make_image_reference("$identitynormalmap", allocator)},
			};

			if (emissive)
			{
				textures.push_back({18, image_or_reference(gfximage::dump_color(find_texture(asset, TS_COLOR_MAP, 'e')), "$black", allocator)});
			}

			if (opacity || glass)
			{
				const auto image = opacity && (blend || variant.alpha_test) ? find_opacity_image(asset, allocator) : nullptr;
				textures.push_back({27, image ? image : make_image_reference("$white", allocator)});
			}

			textures.push_back({59, make_image_reference("$black", allocator)});
			if (glass)
			{
				const auto transmission = find_transmission_image(asset, allocator);
				textures.push_back({68, transmission ? transmission : make_image_reference("$white", allocator)});
			}

			if (frosted)
			{
				textures.push_back({77, make_image_reference("$white", allocator)});
			}

			result->textureCount = static_cast<unsigned char>(textures.size());
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(textures.size());
			std::ranges::copy(textures, result->textureTable);

			std::vector<std::pair<unsigned char, std::array<std::uint32_t, 4>>> literals
			{
				{0, {opacity ? 0xFu : 0x7u, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FAD, 0x0000BC00, 0}},
				{54, get_scale_bias_literal(color, normal)},
				{72, {0x3F800000, 0x1, 0x3FD9364E, 0x3FD9364E}},
			};

			if (glass)
			{
				literals.insert(literals.begin() + 2, {target::MATERIAL_CONSTANT_TRANSMISSION_TINT, get_transmission_tint(asset)});
			}

			if (frosted)
			{
				literals.insert(literals.begin() + 3, {target::MATERIAL_CONSTANT_FROST, {0x3FA66666, 0x3F800000, 0, 0}}); // TODO: stock glass_frosted_01 values
			}
			else if (emissive)
			{
				literals.insert(literals.begin() + 2, {target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT, get_emissive_tint(asset)});
			}
			else if (blend)
			{
				literals.push_back({87, {0x49742400, 0x36039605, 0x4BFA459C, 0xB4000000}});
				literals.push_back({89, {0, 0, 0, 0}});
			}

			std::vector<target::MaterialConstantDef> constants(literals.size());
			for (auto i = 0u; i < literals.size(); i++)
			{
				constants[i].index = literals[i].first;
				std::memcpy(constants[i].literal, literals[i].second.data(), sizeof(constants[i].literal));
			}

			if (flag_wave)
			{
				const std::array<std::pair<target::MaterialConstantIndex, std::string_view>, 4> waves
				{{
					{target::MATERIAL_CONSTANT_FLAG_PARMS, "flagParms"},
					{target::MATERIAL_CONSTANT_FLAG_WAVE_0, "flagWave0Par"},
					{target::MATERIAL_CONSTANT_FLAG_WAVE_1, "flagWave1Par"},
					{target::MATERIAL_CONSTANT_FLAG_WAVE_2, "flagWave2Par"},
				}};

				for (const auto& [index, source_name] : waves)
				{
					auto& wave = constants.emplace_back();
					wave.index = index;
					if (const auto source = find_constant(asset, source_name))
					{
						std::memcpy(wave.literal, source->literal, sizeof(wave.literal));
					}
				}
			}

			result->constantTable = allocate_constants(constants, result->constantCount, allocator);
			return result;
		}

		const char* get_glass_piece_donor_techset(const bool shattered) // TODO: stock mp_aniyah_tac techsets, convert natively
		{
			return shattered ? "g/lit_3_lit_bar_ta1_1006_1002000000080403_0_1_1_0_0_113810035b_0_0_1_0_0"
				: "g/lit_3_lit_bad_ta1_c02_1002000000000011_0_1_1_0_0_13810035b_0_0_1_0_0";
		}

		target::Material* convert_glass_piece(const Material* asset, const std::string& name, const bool shattered, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = shattered ? 0x2010 : 0x10; // TODO: stock g/com_glass_clear_shattered, g/glass_dirty_old_01 values
			result->surfaceFlags = shattered ? 0x4C4000 : 0x480000;
			result->materialType = target::MTL_TYPE_GLASS;
			result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
			result->sortKey = shattered ? 26 : 25;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = get_glass_piece_donor_techset(shattered);

			const auto color = find_color_image(asset, true);
			const auto normal = find_normal_image(asset);
			const auto transmission = find_transmission_image(asset, allocator);
			std::vector<target::MaterialTextureDef> textures
			{
				{0, color ? color : make_image_reference("$black", allocator)},
				{9, normal ? normal : make_image_reference("$identitynormalmap", allocator)},
				{27, make_image_reference("$white", allocator)},
				{59, make_image_reference("$black", allocator)},
				{68, transmission ? transmission : make_image_reference("$white", allocator)},
			};

			if (shattered)
			{
				textures.push_back({133, make_image_reference("$white", allocator)});
			}

			result->textureCount = static_cast<unsigned char>(textures.size());
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(textures.size());
			std::ranges::copy(textures, result->textureTable);

			std::vector<std::pair<unsigned char, std::array<std::uint32_t, 4>>> literals
			{
				{0, {0x7, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FAD, 0x0000BC00, 0}},
				{target::MATERIAL_CONSTANT_TRANSMISSION_TINT, get_transmission_tint(asset)},
				{54, get_scale_bias_literal(color, normal)},
				{72, {0x3F800000, 0x1, 0x3FD9364E, 0x3FD9364E}},
			};

			if (shattered)
			{
				literals.push_back({77, {0x3F000000, 0, 0, 0}}); // TODO: stock g/com_glass_clear_shattered value
			}

			std::vector<target::MaterialConstantDef> constants(literals.size());
			for (auto i = 0u; i < literals.size(); i++)
			{
				constants[i].index = literals[i].first;
				std::memcpy(constants[i].literal, literals[i].second.data(), sizeof(constants[i].literal));
			}

			result->constantTable = allocate_constants(constants, result->constantCount, allocator);
			return result;
		}

		target::Material* convert_layered(const Material* asset, const std::string& name, const char* donor, utils::memory::allocator& allocator)
		{
			const auto layer_count = get_layer_count(asset);

			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 1;
			result->surfaceFlags = 0xB80000;
			result->materialType = target::MTL_TYPE_WORLD_LAYERED_VERTCOL;
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 2;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = donor;

			std::vector<target::GfxImage*> colors, normals;
			for (auto layer = 0u; layer < layer_count; layer++)
			{
				colors.push_back(find_color_image(asset, true, layer));
				normals.push_back(find_normal_image(asset, layer));
			}

			std::vector<target::MaterialTextureDef> textures;
			for (auto layer = 0u; layer < layer_count; layer++)
			{
				textures.push_back({static_cast<unsigned char>(layer), colors[layer] ? colors[layer] : make_image_reference("$black", allocator)});
			}

			for (auto layer = 0u; layer < layer_count; layer++)
			{
				textures.push_back({static_cast<unsigned char>(9 + layer), normals[layer] ? normals[layer] : make_image_reference("$identitynormalmap", allocator)});
			}

			for (auto layer = 1u; layer < layer_count; layer++)
			{
				textures.push_back({static_cast<unsigned char>(27 + layer), find_layer_mask(asset, layer, allocator)});
			}

			for (auto layer = 0u; layer < layer_count; layer++)
			{
				textures.push_back({static_cast<unsigned char>(59 + layer), make_image_reference("$black", allocator)});
			}

			result->textureCount = static_cast<unsigned char>(textures.size());
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(textures.size());
			std::ranges::copy(textures, result->textureTable);

			std::vector<target::MaterialConstantDef> constants;
			const auto add_constant = [&](const unsigned int index, const std::array<std::uint32_t, 4>& literal)
			{
				auto& constant = constants.emplace_back();
				constant.index = static_cast<unsigned char>(index);
				std::memcpy(constant.literal, literal.data(), sizeof(constant.literal));
			};

			for (auto layer = 0u; layer < layer_count; layer++)
			{
				add_constant(layer, {get_layer_flags(asset, layer), 0x80003C00, 0x3C003C00, 0});
			}

			for (auto layer = 0u; layer < layer_count; layer++)
			{
				add_constant(9 + layer, {get_layer_tint(asset, layer), 0x00FF2FAD, 0x0000BC00, 0});
			}

			for (auto layer = 0u; layer < layer_count; layer++)
			{
				add_constant(54 + layer, get_scale_bias_literal(colors[layer], normals[layer]));
			}

			add_constant(72, {std::bit_cast<std::uint32_t>(static_cast<float>(layer_count)), 0x1, 0x3FD9364E, 0x3FD9364E});
			result->constantTable = allocate_constants(constants, result->constantCount, allocator);

			result->layerCount = static_cast<unsigned char>(layer_count);
			result->subMaterials = allocator.allocate_array<const char*>(layer_count);
			for (auto layer = 0u; layer < layer_count; layer++)
			{
				const auto source = asset->subMaterials && layer < asset->layerCount ? asset->subMaterials[layer] : nullptr;
				result->subMaterials[layer] = source ? allocator.duplicate_string(get_lit_name(source, techset::GENERATED_TECHSET_WORLD)) : result->name;
			}

			return result;
		}

		target::Material* convert_distortion(const Material* asset, const std::string& name, const char* donor, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 0x20;
			result->surfaceFlags = 0xACA030;
			result->materialType = target::MTL_TYPE_WORLD_VERTCOL;
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 33;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = donor;

			result->textureCount = 1;
			result->textureTable = allocator.allocate<target::MaterialTextureDef>();
			result->textureTable->index = 157;
			result->textureTable->image = image_or_reference(gfximage::dump_color(find_texture(asset, TS_COLOR_MAP, 'c')), "$black", allocator);

			const std::array<std::pair<target::MaterialConstantIndex, std::string_view>, 2> sources
			{{
				{target::MATERIAL_CONSTANT_UV_ANIM, "uvAnimParms"},
				{target::MATERIAL_CONSTANT_DISTORTION_SCALE, "distortionSc"},
			}};

			std::vector<target::MaterialConstantDef> constants;
			for (const auto& [index, source_name] : sources)
			{
				auto& constant = constants.emplace_back();
				constant.index = index;
				if (const auto source = find_constant(asset, source_name))
				{
					std::memcpy(constant.literal, source->literal, sizeof(constant.literal));
				}
			}

			result->constantTable = allocate_constants(constants, result->constantCount, allocator);
			return result;
		}

		Material* find_loaded(Material* asset)
		{
			if (!asset->textureTable)
			{
				if (const auto loaded = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, asset->name)))
				{
					return loaded;
				}
			}

			return asset;
		}

		std::string get_decal_name(const Material* asset)
		{
			const std::string_view name = asset->name;
			return "i/"s + std::string(name.substr(name.find('/') + 1));
		}

		target::GfxDecalVolumeMaterial* convert_decal_volume(const Material* asset, const std::string& name, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::GfxDecalVolumeMaterial>();
			result->name = allocator.duplicate_string(name);
			result->flags = target::DECAL_VOLUME_MATERIAL_FLAG_UNK_40 | target::DECAL_VOLUME_MATERIAL_FLAG_UNK_400;

			if (const auto color = find_texture(asset, TS_COLOR_MAP, 'c'))
			{
				result->channels[target::DECAL_VOLUME_CHANNEL_COLOR_SPECULAR] = gfximage::dump_decal_color(color);
				result->channels[target::DECAL_VOLUME_CHANNEL_OPACITY] = gfximage::dump_opacity(color);
				if (result->channels[target::DECAL_VOLUME_CHANNEL_COLOR_SPECULAR])
				{
					result->flags |= target::DECAL_VOLUME_MATERIAL_FLAG_COLOR;
				}
			}

			if (find_texture(asset, TS_NORMAL_MAP, 'n'))
			{
				result->channels[target::DECAL_VOLUME_CHANNEL_NORMAL_GLOSS] = find_normal_image(asset);
				if (result->channels[target::DECAL_VOLUME_CHANNEL_NORMAL_GLOSS])
				{
					result->flags |= target::DECAL_VOLUME_MATERIAL_FLAG_NORMAL;
				}
			}

			const auto tint = get_color_tint(asset);
			std::memcpy(result->colorTint, tint.data(), sizeof(result->colorTint));
			result->emissiveScale = 1.0f;
			result->packedDisplacementScaleAndBias = 0xBC004000; // TODO: stock decal values
			result->displacementCutoffDistance = 750.0f;
			result->displacementCutoffFalloff = 0.004f;
			result->packedTemperatureBaseAndScale = 0xBC00;
			result->textureAtlasRowCount = asset->info.textureAtlasRowCount;
			result->textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			return result;
		}

		target::Material* convert_decal(const Material* asset, const std::string& name, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->surfaceFlags = 0x8000;
			result->materialType = target::MTL_TYPE_INFO_CARRIER;
			result->cameraRegion = target::CAMERA_REGION_LIT_DECAL;
			result->sortKey = asset->info.sortKey;
			result->textureAtlasRowCount = asset->info.textureAtlasRowCount;
			result->textureAtlasColumnCount = asset->info.textureAtlasColumnCount;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = "null";

			result->decalVolumeMaterial = convert_decal_volume(asset, name, allocator);
			return result;
		}

		void dump_zone_donor_techsets(const char* zone_name)
		{
			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				ZONETOOL_WARNING("IW8 zone \"%s\" is missing, its donor techsets are not dumped", zone_name);
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
				if (!shaders.contains(shader))
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
					break;
				}
			}
		}
	}

	void dump(Material* asset)
	{
		if (!asset || asset->name[0] == '*' || has_techset_token(asset, "sky") || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name)) // dump_lit and dump_sky write the generated variants
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
		if (is_model_effect(asset))
		{
			zonetool::iw8::material::dump(convert_model_effect(asset, allocator));
			add_csv_row("require", "techsets_common_base_mp");
			add_csv_row("techset", ","s + get_model_effect_techset_name(asset));
			add_csv_row("material", asset->name);
			return;
		}

		zonetool::iw8::material::dump(convert(asset, allocator));
		add_csv_row("require", "techsets_global_stream_mp");
		add_csv_row("techset", ","s + techset_name);
		add_csv_row("image", ",$black");
		add_csv_row("material", asset->name);
	}

	void dump_effect(Material* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
		zonetool::iw8::material::dump(convert_effect(asset, false, allocator));
		add_csv_row("require", "techsets_common_core_mp");
		add_csv_row("techset", ","s + effect_techset_name);
		add_csv_row("material", asset->name);
	}

	void dump_sprite(Material* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
		zonetool::iw8::material::dump(convert_effect(asset, true, allocator));
		add_csv_row("require", is_atlas(asset) ? "techsets_common" : "techsets_common_mp");
		add_csv_row("techset", ","s + get_sprite_techset_name(asset));
		add_csv_row("material", asset->name);
	}

	std::string dump_decal(Material* asset)
	{
		asset = find_loaded(asset);
		const auto name = get_decal_name(asset);
		if (!mark_dumped(ASSET_TYPE_MATERIAL, name))
		{
			return name;
		}

		utils::memory::allocator allocator;
		const auto converted = convert_decal(asset, name, allocator);
		zonetool::iw8::decal_volume_material::dump(converted->decalVolumeMaterial);
		zonetool::iw8::material::dump(converted);
		add_csv_row("require", "techsets_common");
		add_csv_row("techset", ",null");
		add_csv_row("decalvolumematerial", name);
		add_csv_row("material", name);
		return name;
	}

	void dump_2d(Material* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
		const auto converted = convert_2d(asset, allocator);
		zonetool::iw8::material::dump(converted);
		if (const auto image = converted->textureTable->image->name; image[0] == '$')
		{
			add_csv_row("image", ","s + image);
		}
		add_csv_row("techset", ","s + hud_techset_name);
		add_csv_row("material", asset->name);
	}

	std::string dump_lit(Material* asset, const techset::generated_techset techset)
	{
		if (!asset || !asset->name)
		{
			return {};
		}

		const auto name = get_lit_name(asset->name, techset);
		if (!mark_dumped(ASSET_TYPE_MATERIAL, name))
		{
			return name;
		}

		asset = find_loaded(asset);
		const auto variant = get_variant(asset, techset);

		utils::memory::allocator allocator;
		target::Material* converted;
		if (const auto distortion_donor = get_distortion_donor_techset(asset, variant))
		{
			donor_techsets.emplace(distortion_donor);
			converted = convert_distortion(asset, name, distortion_donor, allocator);
		}
		else if (const auto [donor, kind] = get_donor(asset, variant); donor)
		{
			donor_techsets.emplace(donor);
			converted = kind == donor_kind::layered ? convert_layered(asset, name, donor, allocator)
				: convert_donor(asset, name, variant, donor, kind, allocator);
		}
		else
		{
			techset::dump_techset(variant);
			converted = convert_lit(asset, name, variant, allocator);
		}

		zonetool::iw8::material::dump(converted);
		for (auto i = 0; i < converted->textureCount; i++)
		{
			if (const auto image = converted->textureTable[i].image->name; image[0] == '$')
			{
				add_csv_row("image", ","s + image);
			}
		}
		add_csv_row("material", name);
		return name;
	}

	std::string dump_glass_piece(Material* asset, const bool shattered)
	{
		const std::string_view source_name = asset->name;
		const auto name = "g/"s + std::string(source_name.starts_with("m/") ? source_name.substr(2) : source_name);
		if (!mark_dumped(ASSET_TYPE_MATERIAL, name))
		{
			return name;
		}

		asset = find_loaded(asset);
		const auto donor = get_glass_piece_donor_techset(shattered);
		donor_techsets.emplace(donor);

		utils::memory::allocator allocator;
		const auto converted = convert_glass_piece(asset, name, shattered, allocator);
		zonetool::iw8::material::dump(converted);
		for (auto i = 0; i < converted->textureCount; i++)
		{
			if (const auto image = converted->textureTable[i].image->name; image[0] == '$')
			{
				add_csv_row("image", ","s + image);
			}
		}
		add_csv_row("material", name);
		return name;
	}

	std::string dump_sky(Material* asset, const GfxImage* sky_image, const std::array<float, 3>& rotation_angles)
	{
		if (!asset || !asset->name)
		{
			return {};
		}

		const auto name = get_lit_name(asset->name, techset::GENERATED_TECHSET_SKY);
		if (!mark_dumped(ASSET_TYPE_MATERIAL, name))
		{
			return name;
		}

		techset::dump_techset({.techset = techset::GENERATED_TECHSET_SKY});

		utils::memory::allocator allocator;
		const auto converted = convert_sky(name, sky_image, rotation_angles, allocator);
		zonetool::iw8::material::dump(converted);
		if (converted->textureTable->image->name == "$black"sv)
		{
			add_csv_row("image", ",$black");
		}
		add_csv_row("material", name);
		return name;
	}

	zonetool::iw8::GfxSModelSurfCullType get_smodel_cull_type(Material* asset)
	{
		if (!asset)
		{
			return zonetool::iw8::SMODEL_SURF_CULL_OPAQUE;
		}

		asset = find_loaded(asset);
		const auto back_face = (asset->stateFlags & STATE_FLAG_CULL_MASK) == STATE_FLAG_CULL_BACK;
		const auto cull_type = (asset->stateFlags & STATE_FLAG_DECAL)
			? (back_face ? zonetool::iw8::SMODEL_SURF_CULL_DECAL : zonetool::iw8::SMODEL_SURF_CULL_DECAL_DOUBLE_SIDED)
			: (back_face ? zonetool::iw8::SMODEL_SURF_CULL_OPAQUE : zonetool::iw8::SMODEL_SURF_CULL_DOUBLE_SIDED);

		const auto variant = get_variant(asset, techset::GENERATED_TECHSET_STATIC_MODEL);
		if (get_flag_donor_techset(asset, variant))
		{
			return static_cast<zonetool::iw8::GfxSModelSurfCullType>(cull_type & ~zonetool::iw8::SMODEL_SURF_CULL_ENABLED);
		}

		return cull_type;
	}

	zonetool::iw8::GfxSModelSurfEmitType get_smodel_emit_type(Material* asset)
	{
		auto emit_type = zonetool::iw8::SMODEL_SURF_EMIT_NONE | zonetool::iw8::SMODEL_SURF_EMIT_MAIN_VIEW | zonetool::iw8::SMODEL_SURF_EMIT_SHADOW;
		if (!asset)
		{
			return static_cast<zonetool::iw8::GfxSModelSurfEmitType>(emit_type);
		}

		const auto variant = get_variant(find_loaded(asset), techset::GENERATED_TECHSET_STATIC_MODEL);
		if (!variant.visible)
		{
			emit_type &= ~zonetool::iw8::SMODEL_SURF_EMIT_MAIN_VIEW;
		}

		if (!variant.casts_shadow)
		{
			emit_type &= ~zonetool::iw8::SMODEL_SURF_EMIT_SHADOW;
		}

		return static_cast<zonetool::iw8::GfxSModelSurfEmitType>(emit_type);
	}

	unsigned int get_world_layer_count(Material* asset)
	{
		if (!asset)
		{
			return 1;
		}

		asset = find_loaded(asset);
		const auto variant = get_variant(asset, techset::GENERATED_TECHSET_WORLD);
		return !get_distortion_donor_techset(asset, variant) && get_layered_donor_techset(asset, variant) ? get_layer_count(asset) : 1;
	}

	bool has_world_layer_normal_transform(Material* asset, const unsigned int layer)
	{
		return has_layer_normal_transform(find_loaded(asset), layer);
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

		for (const auto* zone : {"techsets_mp_rust", "techsets_mp_aniyah_tac", "techsets_mp_donetsk"})
		{
			if (!donor_techsets.empty())
			{
				dump_zone_donor_techsets(zone);
			}
		}

		for (const auto& name : donor_techsets)
		{
			ZONETOOL_WARNING("IW8 donor techset \"%s\" is missing", name.data());
		}
	}
}
