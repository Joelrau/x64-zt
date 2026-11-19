#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"

#include <iw8/zone/loader.hpp>

#include <io/game_files.hpp>
#include <utils/string.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::iw7::converter::iw8::material
{
	namespace
	{
		namespace target = zonetool::iw8;

		const char* const techset_name = "mo/lit_3_lit_rpl_ta1_800800_184a000010001010_0_3_1_0_0_13810015b_0_0_1_0_0_0_0_7818015f_0_1_2_0_0_0_0_b818015f_0_1_2_0_0"; // TODO: stock akilo47_fore techset, convert natively
		const char* const effect_techset_name = "unlit_6_effect_bad_ta0_8a03_4_0_1_0_0_0_100000023_0_0_2_0_0"; // TODO: stock gfx_tracer_new_erad_blue techset, convert natively

		GfxImage* find_texture(const Material* asset, const TextureSemantic semantic)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].semantic == semantic && asset->textureTable[i].image)
				{
					return asset->textureTable[i].image;
				}
			}

			return nullptr;
		}

		target::GfxImage* find_image(const Material* asset, const TextureSemantic semantic)
		{
			const auto image = find_texture(asset, semantic);
			if (!image)
			{
				return nullptr;
			}

			return semantic == TS_COLOR_SPECULAR_MAP ? gfximage::dump_color_specular(image) : gfximage::dump_normal_gloss(image);
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

		target::GfxImage* make_image_reference(const char* name, utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = name;
			return image;
		}

		target::MaterialTextureDef* convert_textures(const Material* asset, unsigned char& count, utils::memory::allocator& allocator)
		{
			const auto black = make_image_reference("$black", allocator);
			auto* color_specular = find_image(asset, TS_COLOR_SPECULAR_MAP);
			auto* normal_gloss = find_image(asset, TS_NORMAL_OCCLUSSION_GLOSS_MAP);
			color_specular = color_specular ? color_specular : black;
			normal_gloss = normal_gloss ? normal_gloss : black;

			const std::array<std::pair<unsigned char, target::GfxImage*>, 15> slots
			{{
				{0, color_specular}, {1, color_specular}, {2, color_specular},
				{9, normal_gloss}, {10, normal_gloss}, {11, normal_gloss},
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

		target::MaterialConstantDef* convert_constants(unsigned char& count, utils::memory::allocator& allocator)
		{
			const std::array<std::pair<unsigned char, std::array<std::uint32_t, 4>>, 11> constants
			{{
				{0, {0x00000007, 0x80003C00, 0x3C003C00, 0}},
				{1, {0x0000022F, 0x80003C00, 0x3C003C00, 0}},
				{2, {0x0000022F, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{10, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{11, {0xFFFFFFFF, 0x00FF2FBB, 0x0000BC00, 0}},
				{54, {0x000000FF, 0x00FF00FF, 0x4F800000, 0}},
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

			result->textureTable = convert_textures(asset, result->textureCount, allocator);
			result->constantTable = convert_constants(result->constantCount, allocator);
			return result;
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

		target::Material* convert_effect(const Material* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = asset->name;
			result->contents = 1;
			result->surfaceFlags = 0x10000;
			result->materialType = target::MTL_TYPE_DEFAULT;
			result->cameraRegion = target::CAMERA_REGION_EMISSIVE;
			result->sortKey = asset->info.sortKey;
			result->flags = 0x10;
			result->textureAtlasRowCount = asset->info.textureAtlasRowCount;
			result->textureAtlasColumnCount = asset->info.textureAtlasColumnCount;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = effect_techset_name;

			if (const auto image = find_texture(asset, TS_COLOR_MAP))
			{
				result->textureCount = 1;
				result->textureTable = allocator.allocate<target::MaterialTextureDef>();
				result->textureTable->index = 18;
				result->textureTable->image = gfximage::dump_color(image);
			}

			result->constantTable = allocate_constants({convert_color_tint(asset)}, result->constantCount, allocator);
			return result;
		}

		target::GfxImage* find_color_image(const Material* asset)
		{
			if (const auto image = find_texture(asset, TS_COLOR_SPECULAR_MAP))
			{
				return gfximage::dump_color_specular(image);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP))
			{
				return gfximage::dump_color(image);
			}

			return nullptr;
		}

		GfxImage* find_alpha_texture(const Material* asset)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image;
			}

			return find_texture(asset, TS_COLOR_MAP);
		}

		target::GfxImage* find_alpha_image(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image->name[0] == '$' ? make_image_reference(image->name, allocator) : gfximage::dump_alpha(image);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP))
			{
				return gfximage::dump_color(image);
			}

			return nullptr;
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

		target::CameraRegion get_blend_camera_region(const Material* asset)
		{
			switch (asset->cameraRegion)
			{
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
			variant.lit = !has_techset_token(asset, "unlit");
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

			if (asset->cameraRegion == CAMERA_REGION_LIT_DECAL || asset->cameraRegion == CAMERA_REGION_LIT_TRANS
				|| asset->cameraRegion == CAMERA_REGION_EMISSIVE)
			{
				variant.blend = has_techset_token(asset, "add") ? techset::GENERATED_BLEND_ADD
					: has_techset_token(asset, "multiply") ? techset::GENERATED_BLEND_MULTIPLY
					: techset::GENERATED_BLEND_ALPHA;
				variant.casts_shadow = false;
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

			const auto color = find_color_image(asset);
			const auto uses_alpha = variant.alpha_test || variant.blend != techset::GENERATED_BLEND_OPAQUE;
			result->textureCount = uses_alpha ? 2 : 1;
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(result->textureCount);
			result->textureTable[0].index = 0;
			result->textureTable[0].image = color ? color : make_image_reference("$black", allocator);

			if (uses_alpha)
			{
				const auto alpha = find_alpha_image(asset, allocator);
				result->textureTable[1].index = 27;
				result->textureTable[1].image = alpha ? alpha : make_image_reference("$white", allocator);
			}

			std::vector<target::MaterialConstantDef> constants;
			if (!variant.lit && variant.visible)
			{
				constants.push_back(convert_color_tint(asset));
			}

			if (variant.uv_anim)
			{
				target::MaterialConstantDef uv_anim{};
				uv_anim.index = target::MATERIAL_CONSTANT_UV_ANIM;
				std::memcpy(uv_anim.literal, find_constant(asset, "uvAnimParms")->literal, sizeof(uv_anim.literal));
				constants.push_back(uv_anim);
			}

			result->constantTable = allocate_constants(constants, result->constantCount, allocator);

			result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
			std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
			return result;
		}

		target::Material* convert_sky(const std::string& name, const GfxImage* sky_image, utils::memory::allocator& allocator)
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

			const auto image = sky_image ? gfximage::dump_sky(sky_image) : nullptr;
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
			if (variant.techset != techset::GENERATED_TECHSET_WORLD || !has_techset_token(asset, "distortion") || !find_texture(asset, TS_COLOR_MAP))
			{
				return nullptr;
			}

			return "twc/dist_10_distortion_rpl_ta0_0_3_0_0_0";
		}

		std::unordered_set<std::string> donor_techsets;

		GfxImage* find_base_normal_texture(const Material* asset)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				const auto& texture = asset->textureTable[i];
				if (texture.semantic == TS_NORMAL_MAP && texture.nameStart == 'n' && texture.image)
				{
					return texture.image;
				}
			}

			return nullptr;
		}

		target::GfxImage* find_normal_image(const Material* asset)
		{
			if (const auto image = find_texture(asset, TS_NORMAL_OCCLUSSION_GLOSS_MAP))
			{
				return gfximage::dump_normal_gloss(image);
			}

			return gfximage::dump_normal_gloss(find_base_normal_texture(asset), find_texture(asset, TS_SPECULAR_MAP),
				find_texture(asset, TS_SPECULAR_OCCLUSION_MAP));
		}

		target::GfxImage* find_opacity_image(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto image = find_texture(asset, TS_ALPHA_REVEAL_THICKNESS_MAP))
			{
				return image->name[0] == '$' ? make_image_reference(image->name, allocator) : gfximage::dump_alpha(image);
			}

			if (const auto image = find_texture(asset, TS_COLOR_MAP))
			{
				return gfximage::dump_opacity(image);
			}

			return nullptr;
		}

		target::Material* convert_donor(const Material* asset, const std::string& name, const techset::generated_variant& variant, const char* donor,
			const bool flag_wave, utils::memory::allocator& allocator)
		{
			const auto world = variant.techset == techset::GENERATED_TECHSET_WORLD;
			const auto blend = variant.blend != techset::GENERATED_BLEND_OPAQUE;
			const auto opacity = flag_wave || blend || variant.alpha_test || (world && variant.two_sided);

			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 1;
			result->surfaceFlags = 0xB80000;
			result->materialType = flag_wave ? target::MTL_TYPE_STATIC_MODEL_VERTCOL_SELFVIS : techset::get_material_type(variant.techset);
			result->cameraRegion = blend ? get_blend_camera_region(asset) : target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = blend ? asset->info.sortKey : world ? 33 : 9;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = donor;

			const auto color = find_color_image(asset);
			const auto normal = find_normal_image(asset);
			std::vector<target::MaterialTextureDef> textures
			{
				{0, color ? color : make_image_reference("$black", allocator)},
				{9, normal ? normal : make_image_reference("$identitynormalmap", allocator)},
			};

			if (opacity)
			{
				const auto image = blend || variant.alpha_test ? find_opacity_image(asset, allocator) : nullptr;
				textures.push_back({27, image ? image : make_image_reference("$white", allocator)});
			}

			textures.push_back({59, make_image_reference("$black", allocator)});
			result->textureCount = static_cast<unsigned char>(textures.size());
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(textures.size());
			std::ranges::copy(textures, result->textureTable);

			std::vector<std::pair<unsigned char, std::array<std::uint32_t, 4>>> literals
			{
				{0, {opacity ? 0xFu : 0x7u, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FAD, 0x0000BC00, 0}},
				{54, {0x9, 0x00FF00FF, 0x4F800000, 0}},
				{72, {0x3F800000, 0x1, 0x3FD9364E, 0x3FD9364E}},
			};

			if (blend)
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
			result->textureTable->image = gfximage::dump_color(find_texture(asset, TS_COLOR_MAP));

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
	}

	void dump(Material* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
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
		zonetool::iw8::material::dump(convert_effect(asset, allocator));
		add_csv_row("require", "techsets_common_core_mp");
		add_csv_row("techset", ","s + effect_techset_name);
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
		const auto flag_donor = get_flag_donor_techset(asset, variant);
		if (const auto distortion_donor = get_distortion_donor_techset(asset, variant))
		{
			donor_techsets.emplace(distortion_donor);
			converted = convert_distortion(asset, name, distortion_donor, allocator);
		}
		else if (const auto donor = flag_donor ? flag_donor : get_donor_techset(variant))
		{
			donor_techsets.emplace(donor);
			converted = convert_donor(asset, name, variant, donor, flag_donor != nullptr, allocator);
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

	std::string dump_sky(Material* asset, const GfxImage* sky_image)
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
		const auto converted = convert_sky(name, sky_image, allocator);
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

	void dump_donor_techsets()
	{
		if (donor_techsets.empty())
		{
			return;
		}

		xfile::oodle::load(iw8_folder());
		auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
		const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

		const auto path = game_files::find_zone("techsets_mp_rust");
		if (!path)
		{
			ZONETOOL_WARNING("IW8 zone \"techsets_mp_rust\" is missing, the donor techsets are not dumped");
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

		for (const auto& name : donor_techsets)
		{
			ZONETOOL_WARNING("IW8 donor techset \"%s\" is missing in techsets_mp_rust", name.data());
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
