#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"

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

			result->constantCount = 1;
			result->constantTable = allocator.allocate<target::MaterialConstantDef>();
			result->constantTable->index = 36;
			const auto tint = find_constant(asset, "colorTint");
			const std::array<float, 4> white{1.0f, 1.0f, 1.0f, 1.0f};
			std::memcpy(result->constantTable->literal, tint ? tint->literal : white.data(), sizeof(result->constantTable->literal));
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

		std::string get_lit_name(const char* name, const techset::generated_techset techset)
		{
			const auto prefix = techset == techset::GENERATED_TECHSET_WORLD ? "tw" : "tmo";
			if (name[0] == '*')
			{
				return std::format("{}/maps/mp/generated/{}", prefix, name + 1); // '*' is not a valid file name
			}

			return std::format("{}/{}", prefix, name);
		}

		target::Material* convert_lit(const Material* asset, const std::string& name, const techset::generated_techset techset,
			utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = 1;
			result->surfaceFlags = 0xB80000;
			result->materialType = techset == techset::GENERATED_TECHSET_WORLD ? target::MTL_TYPE_WORLD : target::MTL_TYPE_STATIC_MODEL_SELFVIS;
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 2;
			result->flags = 0x10;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;

			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = techset::get_techset_name(techset);

			const auto color = find_color_image(asset);
			result->textureCount = 1;
			result->textureTable = allocator.allocate<target::MaterialTextureDef>();
			result->textureTable->index = 0;
			result->textureTable->image = color ? color : make_image_reference("$black", allocator);

			result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
			std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
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
		techset::dump_techset(techset);

		utils::memory::allocator allocator;
		const auto converted = convert_lit(asset, name, techset, allocator);
		zonetool::iw8::material::dump(converted);
		if (converted->textureTable->image->name == "$black"sv)
		{
			add_csv_row("image", ",$black");
		}
		add_csv_row("material", name);
		return name;
	}
}
