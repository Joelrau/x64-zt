#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::t9::converter::iw8
{
	namespace
	{
		std::vector<std::string> csv_rows;
		std::filesystem::path target_folder;
	}

	const std::filesystem::path& iw8_folder()
	{
		if (target_folder.empty())
		{
			throw std::runtime_error("IW8 game folder is not set");
		}

		return target_folder;
	}

	void set_iw8_folder(const std::filesystem::path& folder)
	{
		target_folder = folder;
	}

	void add_csv_row(const std::string_view type, const std::string_view name)
	{
		auto row = std::format("{},{}", type, name);
		if (std::ranges::find(csv_rows, row) == csv_rows.end())
		{
			csv_rows.emplace_back(std::move(row));
		}
	}

	bool mark_dumped(const zonetool::iw8::XAssetType type, const std::string_view name)
	{
		static std::unordered_set<std::string> dumped;
		return dumped.emplace(std::format("{},{}", static_cast<int>(type), name)).second;
	}

	void write_csv(const std::string& zone_name)
	{
		if (csv_rows.empty())
		{
			return;
		}

		std::ranges::stable_sort(csv_rows, {}, [](const std::string& row)
		{
			return row.starts_with("require,") ? 0 : row.contains(",,") ? 1 : 2;
		});

		std::string text;
		for (const auto& row : csv_rows)
		{
			text += row + "\n";
		}

		filesystem::file csv(zone_name + ".csv");
		csv.open("wb");
		csv.write(text);
		csv.close();
		csv_rows.clear();
	}
}

namespace zonetool::convert::t9_iw8
{
	namespace
	{
		const zonetool::t9::Districts* zone_districts = nullptr;
		const zonetool::t9::EntityList* zone_entities = nullptr;
		const zonetool::t9::ClipMap* zone_clip_map = nullptr;
		const zonetool::t9::TriggerList* zone_triggers = nullptr;

		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::t9;
			namespace target = zonetool::t9::converter::iw8;

			switch (type)
			{
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_DISTRICTS:
				zone_districts = header_as<source::Districts>(header);
				break;
			case source::ASSET_TYPE_ENTITYLIST:
				zone_entities = header_as<source::EntityList>(header);
				break;
			case source::ASSET_TYPE_CLIP_MAP:
				zone_clip_map = header_as<source::ClipMap>(header);
				break;
			case source::ASSET_TYPE_TRIGGERLIST:
				zone_triggers = header_as<source::TriggerList>(header);
				break;
			default:
				break;
			}
		}

		void dump_map(const std::string& zone_name)
		{
			namespace target = zonetool::t9::converter::iw8;

			std::vector<zonetool::t9::StaticModelPlacement> placements;
			const auto models = target::gfxworld::get_placed_models(zone_districts, placements);
			const auto world_bounds = target::gfxworld::get_world_bounds(zone_districts, models);
			const auto dyn_entity_count = target::dynentitylist::dump(zone_clip_map);
			const auto sky_material = target::sky::dump(zone_entities, zone_name);
			target::gfxworld::dump(zone_districts, models, placements, sky_material, zone_name);
			target::comworld::dump(zone_districts, zone_name);
			target::fxworld::dump(zone_name);
			target::glassworld::dump(zone_name);

			const auto triggers = target::mapents::convert_triggers(zone_triggers, zone_name);
			auto collision = target::havok::build_world_collision(zone_clip_map, models, target::gfxworld::get_collision_triangles(zone_districts),
				triggers, zone_name);
			target::clipmap::dump(zone_name, world_bounds, collision.world_shape);
			target::mapents::dump(target::mapents::read_entities(zone_entities), triggers, world_bounds, collision.ents_shape, dyn_entity_count,
				zone_name);
			target::netconststrings::dump_level_strings();
			target::vision::dump(zone_name);

			zone_districts = nullptr;
			zone_entities = nullptr;
			zone_clip_map = nullptr;
			zone_triggers = nullptr;
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			zonetool::t9::converter::iw8::set_iw8_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			if (zone_districts)
			{
				dump_map(zone_name);
			}

			zonetool::t9::converter::iw8::material::dump_donor_techsets();
			zonetool::t9::converter::iw8::write_csv(zone_name);
		}

		std::vector<std::string> required_zones(const std::string& zone_name)
		{
			return {"core_bootstrap", "core_common", "1080_core_common", "mp_common", "ww_core_common", "ww_mp_common", "ww_" + zone_name, "techset_core_bootstrap", "techset_core_common", "techset_" + zone_name};
		}
	}

	const converter& definition()
	{
		static const converter t9_iw8
		{
			.source = "t9",
			.target = "iw8",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
			.required_zones = required_zones,
		};

		return t9_iw8;
	}
}
