#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::material
{
	namespace
	{
		constexpr std::uint8_t s2_semantic_color_map = 2;
		constexpr std::uint8_t s2_semantic_normal_map = 5;
		constexpr std::uint8_t s2_semantic_specular_map = 8;

		// S2 dielectric reflectance 49, full occlusion, medium gloss
		constexpr std::array<std::uint8_t, 4> default_specular{49, 255, 128, 255};
		constexpr std::array<std::uint8_t, 4> white{255, 255, 255, 255};
		// the sky shader scales the flow vector by blue, zero keeps the converted sky still
		constexpr std::array<std::uint8_t, 4> still_flow{128, 128, 0, 255};

		struct converted_material
		{
			zonetool::s2::Material material;
			std::vector<zonetool::s2::MaterialTextureDef> textures;
			std::vector<zonetool::s2::GfxImage> images;
			std::vector<std::string> image_names;
		};

		std::vector<Material*> pending_materials;
		std::unordered_set<std::string> dumped_donor_images;

		const MaterialTextureDef* find_texture(const Material* asset, const std::uint32_t hash)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].nameHash == hash && asset->textureTable[i].semantic != TS_WATER_MAP)
				{
					return &asset->textureTable[i];
				}
			}

			return nullptr;
		}

		std::string fallback_image(const std::uint8_t semantic)
		{
			switch (semantic)
			{
			case s2_semantic_color_map:
				return gfximage::dump_solid("h1_s2_white", s2_semantic_color_map, white);
			case s2_semantic_normal_map:
				return "$identitynormalmap";
			case s2_semantic_specular_map:
				return gfximage::dump_solid("h1_s2_specular", s2_semantic_specular_map, default_specular);
			default:
				return "$black";
			}
		}

		std::string select_image(const Material* asset, const zonetool::s2::MaterialTextureDef& donor_texture)
		{
			const auto& zone = techset::donor_zone();
			const auto source_texture = find_texture(asset, donor_texture.nameHash);
			const auto source_image = source_texture && source_texture->u.image ? source_texture->u.image->name : nullptr;

			// H1 skies are cubemaps, S2 sky shaders read a 2D color map
			if (source_image && donor_texture.semantic == s2_semantic_color_map && source_texture->u.image->mapType == MAPTYPE_CUBE)
			{
				if (const auto sky = gfximage::dump_sky(source_texture->u.image))
				{
					return *sky;
				}
			}

			if (source_image && gfximage::is_dumped(source_image))
			{
				return source_image;
			}

			if (source_image && source_image[0] != '$')
			{
				ZONETOOL_WARNING("Image \"%s\" of material \"%s\" is not in the zone", source_image, asset->name);
			}

			const auto donor_image = donor_texture.u.image;
			if (donor_image && donor_image->name)
			{
				const auto is_core_semantic = donor_texture.semantic == s2_semantic_color_map
					|| donor_texture.semantic == s2_semantic_normal_map
					|| donor_texture.semantic == s2_semantic_specular_map;
				if (zone.is_global_image(donor_image->name))
				{
					return donor_image->name;
				}

				if (!is_core_semantic)
				{
					if (dumped_donor_images.insert(donor_image->name).second)
					{
						db_scope _(zone.db());
						zonetool::s2::gfx_image::dump(donor_image);
						add_csv_row("image", donor_image->name);
					}

					return donor_image->name;
				}
			}

			return fallback_image(donor_texture.semantic);
		}

		std::unique_ptr<converted_material> convert(const Material* asset, const zonetool::s2::Material* donor)
		{
			auto result = std::make_unique<converted_material>();
			auto& material = result->material;
			material = *donor;

			material.info.name = asset->name;
			material.info.surfaceTypeBits = convert_surface_type_bits(static_cast<std::uint64_t>(asset->info.surfaceTypeBits));
			material.parentMaterial = nullptr;

			result->images.resize(donor->textureCount);
			result->image_names.resize(donor->textureCount);
			result->textures.assign(donor->textureTable, donor->textureTable + donor->textureCount);
			for (auto i = 0u; i < result->textures.size(); i++)
			{
				auto& texture = result->textures[i];
				result->image_names[i] = select_image(asset, texture);
				result->images[i].name = result->image_names[i].data();
				texture.u.image = &result->images[i];
			}

			// H1 skies are static, replace the donor flow map so the converted sky does not move
			const auto is_sky = std::ranges::any_of(result->image_names, [](const std::string& name)
			{
				return name.ends_with("_s2sky");
			});
			for (auto i = 0u; is_sky && i < result->textures.size(); i++)
			{
				const auto donor_image = donor->textureTable[i].u.image;
				if (donor_image && donor_image->name && std::string_view{donor_image->name}.ends_with("_flow"))
				{
					result->image_names[i] = gfximage::dump_solid("h1_s2_sky_still_flow", result->textures[i].semantic, still_flow);
					result->images[i].name = result->image_names[i].data();
				}
			}

			material.textureTable = result->textures.data();
			return result;
		}

		std::string get_techset_name(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "";
		}

		void convert_material(Material* asset, std::unordered_map<std::string, std::string>& selected)
		{
			const auto h1_techset = get_techset_name(asset);
			if (h1_techset.empty())
			{
				ZONETOOL_WARNING("Material \"%s\" has no techset", asset->name);
				return;
			}

			auto [entry, inserted] = selected.try_emplace(h1_techset);
			if (inserted)
			{
				entry->second = techset::select(h1_techset);
				ZONETOOL_INFO("techset %s -> %s", h1_techset.data(), entry->second.data());
			}

			const auto& s2_techset = entry->second;
			const auto& zone = techset::donor_zone();
			const auto donor = zone.find_donor(s2_techset);
			if (!donor)
			{
				ZONETOOL_WARNING("No S2 material uses techset \"%s\"", s2_techset.data());
				return;
			}

			const auto global = zone.is_global_techset(s2_techset);
			if (!global)
			{
				if (const auto techniques = zone.find_techset(s2_techset))
				{
					db_scope _(zone.db());
					techset::dump(techniques);
				}
			}
			add_csv_row("techset", s2_techset, global);

			const auto converted = convert(asset, donor);
			{
				db_scope _(zone.db());
				zonetool::s2::material::dump(&converted->material);
			}
			add_csv_row("material", asset->name);

			for (const auto& image : converted->images)
			{
				if (image.name && !gfximage::is_dumped(image.name) && !dumped_donor_images.contains(image.name))
				{
					add_csv_row("image", image.name, true);
				}
			}
		}
	}

	void dump(Material* asset)
	{
		pending_materials.emplace_back(asset);
	}

	void finish()
	{
		std::unordered_map<std::string, std::string> selected;
		for (const auto asset : pending_materials)
		{
			try
			{
				convert_material(asset, selected);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to convert material \"%s\": %s", asset->name, e.what());
			}
		}

		pending_materials.clear();
	}
}
