#include <std_include.hpp>

#include "../include.hpp"
#include "sky.hpp"
#include "gfximage.hpp"
#include "mapents.hpp"

#include <iw8/zone/loader.hpp>

#include <formats/dxil.hpp>
#include <io/game_files.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::t9::converter::iw8::sky
{
	namespace
	{
		namespace target = zonetool::iw8;

		std::string find_worldspawn_value(const std::vector<mapents::entity>& entities, const std::string_view key)
		{
			for (const auto& entity : entities)
			{
				const auto is_worldspawn = std::ranges::any_of(entity, [](const auto& pair)
				{
					return pair.first == "classname" && pair.second == "worldspawn";
				});

				if (!is_worldspawn)
				{
					continue;
				}

				for (const auto& [entity_key, value] : entity)
				{
					if (entity_key == key)
					{
						return value;
					}
				}

				return {};
			}

			return {};
		}

		const Material* find_sky_material(const std::string& model_name)
		{
			const auto* model = static_cast<const XModel*>(db_find_asset(ASSET_TYPE_XMODEL, xhash_name(xhash(model_name))));
			if (!model || !model->numLods || !model->lodMaterials || !model->lodMaterials[0].count || !model->lodMaterials[0].materials)
			{
				return nullptr;
			}

			const auto* material = model->lodMaterials[0].materials[0];
			if (!material || material->techniqueSet || material->textureCount)
			{
				return material;
			}

			const auto* loaded = static_cast<const Material*>(db_find_asset(ASSET_TYPE_MATERIAL, xhash_name(material->name)));
			return loaded ? loaded : material;
		}

		const GfxImage* find_color_map(const Material* asset)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].slot == TEXTURE_SLOT_COLOR_MAP)
				{
					return asset->textureTable[i].image;
				}
			}

			return nullptr;
		}

		std::optional<float> read_sky_scale(const Material* asset)
		{
			const auto* loaded = asset->techniqueSet
				? static_cast<const MaterialTechniqueSet*>(db_find_asset(ASSET_TYPE_TECHSET, xhash_name(asset->techniqueSet->name)))
				: nullptr;
			const auto* techset = loaded ? loaded : asset->techniqueSet;
			const auto* technique = techset ? techset->techniques[5] : nullptr;
			const auto* shader_set = technique ? technique->passes[0].shaderSet : nullptr;
			const auto* program = shader_set ? shader_set->programs[1] : nullptr;
			if (!program || !program->bytecode)
			{
				return {};
			}

			const auto cbuffers = dxil::read_cbuffers(program->bytecode, program->bytecodeSize);
			const auto* globals = dxil::find_cbuffer(cbuffers, "$Globals");
			const auto* field = globals ? globals->find("skyScale") : nullptr;
			const auto& physical = asset->physicalData;
			const auto* entry = physical.entrySets[5] ? physical.entrySets[5]->entries[2] : nullptr;
			if (!field || field->size < sizeof(float) || !entry || !physical.data || entry->offset + entry->size > physical.dataSize
				|| field->offset + sizeof(float) > entry->size)
			{
				return {};
			}

			float value{};
			std::memcpy(&value, physical.data + entry->offset + field->offset, sizeof(value));
			return value;
		}

		bool dump_donor_techset(const char* zone_name, const std::string_view techset_name) // TODO: stock IW8 techset, convert natively
		{
			xfile::oodle::load(iw8_folder());
			auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
			const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				return false;
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);

			std::unordered_set<std::string> shaders;
			for (const auto& asset : zone->assets)
			{
				auto* techset = static_cast<target::MaterialTechniqueSet*>(asset.header);
				if (asset.referenced || asset.type != target::ASSET_TYPE_TECHNIQUE_SET || techset->name != techset_name)
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

			if (shaders.empty())
			{
				return false;
			}

			for (const auto& asset : zone->assets)
			{
				if (asset.type != target::ASSET_TYPE_VERTEXSHADER && asset.type != target::ASSET_TYPE_HULLSHADER
					&& asset.type != target::ASSET_TYPE_DOMAINSHADER && asset.type != target::ASSET_TYPE_PIXELSHADER)
				{
					continue;
				}

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

			return true;
		}

		void dump_material(const std::string& name, const char* techset, target::GfxImage* image, const float sky_scale)
		{
			utils::memory::allocator allocator;
			const auto result = allocator.allocate<target::Material>();
			result->name = name.data();
			result->contents = 0x800;
			result->surfaceFlags = 0x34;
			result->materialType = target::MTL_TYPE_WORLD;
			result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
			result->sortKey = 3;
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;
			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = techset;

			auto* black = allocator.allocate<target::GfxImage>();
			black->name = "$black";

			result->textureCount = 2;
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(2);
			result->textureTable[0] = {151, image};
			result->textureTable[1] = {152, black}; // thermal sky

			const std::array<std::uint32_t, 4> sky_parms{std::bit_cast<std::uint32_t>(sky_scale / 1024.0f), 3, 0x3B800000, 0x3B000000}; // TODO: Q28 sky unit
			result->constantCount = 2;
			result->constantTable = allocator.allocate_array<target::MaterialConstantDef>(2);
			result->constantTable[0].index = 128;
			std::memcpy(result->constantTable[0].literal, sky_parms.data(), sizeof(result->constantTable[0].literal));
			result->constantTable[1].index = 129;

			zonetool::iw8::material::dump(result);
			add_csv_row("image", ",$black");
			add_csv_row("material", name);
		}
	}

	std::optional<std::string> dump(const EntityList* entities, const std::string& map_name)
	{
		const auto model_name = entities ? find_worldspawn_value(mapents::read_entities(entities), "skyboxmodel") : std::string{};
		const auto* material = model_name.empty() ? nullptr : find_sky_material(model_name);
		const auto* color_map = material ? find_color_map(material) : nullptr;
		if (!color_map)
		{
			ZONETOOL_WARNING("map \"%s\": sky model \"%s\" has no color map, the map has no sky", map_name.data(), model_name.data());
			return {};
		}

		const auto sky_scale = read_sky_scale(material);
		if (!sky_scale)
		{
			ZONETOOL_WARNING("map \"%s\": sky material \"%s\" has no skyScale, the map has no sky", map_name.data(), xhash_name(material->name).data());
			return {};
		}

		auto* image = gfximage::dump_sky(color_map);
		if (!image)
		{
			return {};
		}

		const auto* techset = "tw/sky_3_sky_rpl_ta1_0_16_0_0_0";
		if (!dump_donor_techset("techsets_mp_rust", techset))
		{
			ZONETOOL_WARNING("map \"%s\": IW8 donor techset \"%s\" is missing, the map has no sky", map_name.data(), techset);
			return {};
		}

		const auto name = "tw/sky_" + map_name;
		if (mark_dumped(target::ASSET_TYPE_MATERIAL, name))
		{
			dump_material(name, techset, image, *sky_scale);
		}

		ZONETOOL_INFO("map \"%s\": sky \"%s\" from \"%s\", image \"%s\", skyScale %g", map_name.data(), name.data(), model_name.data(), image->name,
			*sky_scale);
		return name;
	}
}
