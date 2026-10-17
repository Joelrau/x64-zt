#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "techset.hpp"

namespace zonetool::s2::converter::h1::material
{
	namespace
	{

		struct converted_material
		{
			zonetool::h1::Material material;
			std::vector<zonetool::h1::MaterialTextureDef> textures;
			std::vector<zonetool::h1::MaterialConstantDef> constants;
			std::vector<zonetool::h1::GfxImage> images;
			std::vector<std::string> image_names;
		};

		struct conversion_stats
		{
			std::size_t source_images;
			std::size_t global_images;
			std::size_t fallback_images;
			std::size_t lost_images;
			std::unordered_set<std::string> missing_images;
		};

		std::vector<Material*> pending_materials;
		std::unordered_map<std::string, std::uint8_t> camera_regions;

		const Material* find_table_owner(const Material* asset)
		{
			while (!asset->textureTable && !asset->constantTable && asset->parentMaterial)
			{
				asset = asset->parentMaterial;
			}

			return asset;
		}

		const MaterialTextureDef* find_texture(const Material* asset, const std::uint32_t hash)
		{
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->textureCount && owner->textureTable; i++)
			{
				if (owner->textureTable[i].nameHash == hash)
				{
					return &owner->textureTable[i];
				}
			}

			return nullptr;
		}

		const MaterialConstantDef* find_constant(const Material* asset, const std::uint32_t hash)
		{
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->constantCount && owner->constantTable; i++)
			{
				if (owner->constantTable[i].nameHash == hash)
				{
					return &owner->constantTable[i];
				}
			}

			return nullptr;
		}

		std::string fallback_image(const std::uint8_t semantic)
		{
			switch (semantic)
			{
			case zonetool::h1::TS_NORMAL_MAP:
				return "$identitynormalmap";
			case zonetool::h1::TS_SPECULAR_MAP:
				return "$black";
			default:
				return "$white";
			}
		}

		std::string select_image(const Material* asset, const zonetool::h1::MaterialTextureDef& donor_texture, conversion_stats& stats)
		{
			const auto source_texture = find_texture(asset, donor_texture.nameHash);
			const auto source_image = source_texture && source_texture->u.image ? source_texture->u.image->name : nullptr;
			if (source_image && (gfximage::is_dumped(source_image) || techset::is_global_image(source_image)))
			{
				stats.source_images++;
				return source_image;
			}

			if (source_image)
			{
				stats.missing_images.insert(source_image);
			}

			const auto donor_image = donor_texture.u.image && donor_texture.u.image->name ? donor_texture.u.image->name : "";
			if (donor_texture.semantic != zonetool::h1::TS_COLOR_MAP && techset::is_global_image(donor_image))
			{
				stats.global_images++;
				return donor_image;
			}

			stats.fallback_images++;
			return fallback_image(donor_texture.semantic);
		}

		bool is_sky(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name && std::strstr(asset->techniqueSet->name, "_sky");
		}

		bool is_ui3d_screen(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name && std::strstr(asset->techniqueSet->name, "_ui3d");
		}

		std::optional<std::string> convert_sky_image(const Material* asset, const zonetool::h1::MaterialTextureDef& donor_texture)
		{
			if (!is_sky(asset) || donor_texture.semantic != zonetool::h1::TS_COLOR_MAP)
			{
				return {};
			}

			const auto source_texture = find_texture(asset, donor_texture.nameHash);
			if (!source_texture || !source_texture->u.image)
			{
				return {};
			}

			const auto rotation = find_constant(asset, 0xBC8EB53C);
			const auto rotation_cos = rotation ? rotation->literal[0] : 1.0f;
			const auto rotation_sin = rotation ? rotation->literal[1] : 0.0f;
			const auto rotation_degrees = rotation ? static_cast<int>(std::lround(rotation->literal[2])) : 0;
			return gfximage::dump_sky_cube(source_texture->u.image, rotation_cos, rotation_sin, rotation_degrees);
		}

		void count_lost_images(const Material* asset, const zonetool::h1::Material* donor, conversion_stats& stats)
		{
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->textureCount && owner->textureTable; i++)
			{
				const auto hash = owner->textureTable[i].nameHash;
				const auto donor_has_slot = std::any_of(donor->textureTable, donor->textureTable + donor->textureCount,
					[&](const zonetool::h1::MaterialTextureDef& texture)
				{
					return texture.nameHash == hash;
				});

				if (!donor_has_slot)
				{
					stats.lost_images++;
				}
			}
		}

		std::unique_ptr<converted_material> convert(const Material* asset, const zonetool::h1::Material* donor, conversion_stats& stats)
		{
			auto result = std::make_unique<converted_material>();
			auto& material = result->material;
			material = *donor;

			material.info.name = asset->name;
			material.info.surfaceTypeBits = zonetool::h1::SURFTYPE_BITS_DEFAULT;

			result->images.resize(donor->textureCount);
			result->image_names.resize(donor->textureCount);
			result->textures.assign(donor->textureTable, donor->textureTable + donor->textureCount);
			for (auto i = 0u; i < result->textures.size(); i++)
			{
				auto& texture = result->textures[i];
				if (texture.semantic == zonetool::h1::TS_WATER_MAP)
				{
					continue;
				}

				result->image_names[i] = select_image(asset, texture, stats);
				if (const auto sky_cube = convert_sky_image(asset, texture))
				{
					result->image_names[i] = *sky_cube;
				}
				else if (is_ui3d_screen(asset) && texture.semantic == zonetool::h1::TS_COLOR_MAP)
				{
					result->image_names[i] = "$black";
				}

				result->images[i].name = result->image_names[i].data();
				texture.u.image = &result->images[i];
			}

			result->constants.assign(donor->constantTable, donor->constantTable + donor->constantCount);
			for (auto& constant : result->constants)
			{
				if (const auto source_constant = find_constant(asset, constant.nameHash))
				{
					std::memcpy(constant.literal, source_constant->literal, sizeof(constant.literal));
				}
			}

			material.textureTable = result->textures.data();
			material.constantTable = result->constants.data();

			material.layerCount = asset->layerCount;
			material.subMaterials = asset->subMaterials;

			count_lost_images(asset, donor, stats);
			return result;
		}

		std::string get_techset_name(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "";
		}

		std::unordered_map<std::string, std::string> select_techsets()
		{
			std::unordered_map<std::string, std::string> result;
			for (const auto asset : pending_materials)
			{
				const auto s2_techset = get_techset_name(asset);
				if (!s2_techset.empty() && !result.contains(s2_techset))
				{
					result[s2_techset] = techset::select(s2_techset);
					ZONETOOL_INFO("techset %s -> %s", s2_techset.data(), result[s2_techset].data());
				}
			}

			return result;
		}

		void dump_with_zone_globals(const techset::h1_zone& zone, zonetool::h1::Material* material)
		{
			const db_scope _(zone.db());
			zonetool::h1::material::dump(material);
		}

		void convert_zone_materials(const std::filesystem::path& zone_path, const std::vector<std::pair<Material*, std::string>>& materials,
			conversion_stats& stats)
		{
			ZONETOOL_INFO("Converting %zu materials with \"%s\"...", materials.size(), zone_path.stem().string().data());

			const techset::h1_zone zone(zone_path);
			std::unordered_set<std::string> dumped_techsets;

			for (const auto& [asset, h1_techset] : materials)
			{
				const auto donor = zone.find_donor(h1_techset);
				if (!donor)
				{
					ZONETOOL_WARNING("No H1 material uses techset \"%s\" in \"%s\"", h1_techset.data(), zone_path.stem().string().data());
					continue;
				}

				const auto source = techset::find_source(h1_techset);
				if (!source->global && dumped_techsets.insert(h1_techset).second)
				{
					if (const auto techniques = zone.find_techset(h1_techset))
					{
						techset::dump(techniques, zone_path);
					}
				}

				add_csv_row("techset", h1_techset, source->global);

				const auto converted = convert(asset, donor, stats);
				dump_with_zone_globals(zone, &converted->material);
				camera_regions[asset->name] = converted->material.cameraRegion;
				add_csv_row("material", asset->name);

				for (const auto& image : converted->images)
				{
					if (image.name && !gfximage::is_dumped(image.name))
					{
						add_csv_row("image", image.name, true);
					}
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
		if (pending_materials.empty())
		{
			return;
		}

		const auto selected = select_techsets();

		std::map<std::filesystem::path, std::vector<std::pair<Material*, std::string>>> materials_by_zone;
		for (const auto asset : pending_materials)
		{
			const auto s2_techset = get_techset_name(asset);
			if (s2_techset.empty())
			{
				ZONETOOL_WARNING("Material \"%s\" has no techset", asset->name);
				continue;
			}

			const auto& h1_techset = selected.at(s2_techset);
			const auto source = techset::find_source(h1_techset);
			if (!source)
			{
				ZONETOOL_WARNING("H1 techset \"%s\" for material \"%s\" is not found", h1_techset.data(), asset->name);
				continue;
			}

			materials_by_zone[source->zone].emplace_back(asset, h1_techset);
		}

		conversion_stats stats{};
		for (const auto& [zone, materials] : materials_by_zone)
		{
			convert_zone_materials(zone, materials, stats);
		}

		techset::dump_missing_shaders();

		ZONETOOL_INFO("Material images: %zu from source, %zu global, %zu fallback, %zu S2 slots without H1 slot",
			stats.source_images, stats.global_images, stats.fallback_images, stats.lost_images);

		for (const auto& image : stats.missing_images)
		{
			ZONETOOL_WARNING("Image \"%s\" is not in the source zone", image.data());
		}

		pending_materials.clear();
	}

	std::optional<std::uint8_t> find_camera_region(const std::string& name)
	{
		const auto region = camera_regions.find(name);
		if (region == camera_regions.end())
		{
			return {};
		}

		return region->second;
	}
}
