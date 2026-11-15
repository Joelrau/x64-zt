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

		target::GfxImage* find_image(const Material* asset, const TextureSemantic semantic)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].semantic == semantic && asset->textureTable[i].image)
				{
					return semantic == TS_COLOR_SPECULAR_MAP
						? gfximage::dump_color_specular(asset->textureTable[i].image)
						: gfximage::dump_normal_gloss(asset->textureTable[i].image);
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
	}

	void dump(Material* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return;
		}

		if (!asset->textureTable)
		{
			if (const auto loaded = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, asset->name)))
			{
				asset = loaded;
			}
		}

		utils::memory::allocator allocator;
		zonetool::iw8::material::dump(convert(asset, allocator));
		add_csv_row("require", "techsets_global_stream_mp");
		add_csv_row("techset", ","s + techset_name);
		add_csv_row("image", ",$black");
		add_csv_row("material", asset->name);
	}
}
