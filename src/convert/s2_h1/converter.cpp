#include <std_include.hpp>
#include "converter.hpp"
#include <utils/io.hpp>

#include <game.hpp>

namespace zonetool::s2::converter::h1
{
	namespace
	{
		struct csv_row
		{
			std::string type;
			std::string name;
			bool referenced;
		};

		std::vector<csv_row> csv_rows;
		std::unordered_set<std::string> csv_keys;

		void write_csv(const std::string& zone_name)
		{
			filesystem::file csv(zone_name + ".csv");
			csv.open("wb");

			for (const auto& row : csv_rows)
			{
				csv.write(std::format("{},{}{}\n", row.type, row.referenced ? "," : "", row.name));
			}

			csv.close();
			csv_rows.clear();
			csv_keys.clear();
		}

		// S2 table read from the exe at 0x140FB0DE0
		constexpr std::array<std::string_view, 62> s2_surface_types
		{
			"default", "asphalt", "asphalt_debris", "asphalt_wet", "bark", "brick_dark", "brick_light", "brick_red",
			"carpet", "ceramic", "cloth", "concrete", "concrete_debris", "concrete_wet", "cushion", "dirt", "flesh",
			"foliage_debris", "foliage_leaves", "foliage_vegetation", "fruit", "glass", "grass", "grass_tall", "gravel",
			"ice", "metal_debris", "metal_grate", "metal_hollow", "metal_solid", "metal_thin", "metal_vehicle",
			"metal_wet", "mud_dry", "mud_wet", "paintedmetal", "paper", "plaster", "plastic_hollow", "plastic_solid",
			"plastic_tarp", "riotshield", "rock", "rock_debris", "rock_wet", "rubber", "sand_dry", "sand_wet", "slush",
			"snow", "water_ankle", "water_knee", "water_waist", "wood_debris", "wood_hollow", "wood_solid", "wood_wet",
			"hay", "metal_armor", "concrete_debris_small", "rock_debris_small", "snow_hard",
		};

		constexpr std::array<std::string_view, zonetool::h1::SURF_TYPE_COUNT> h1_surface_types
		{
			"default", "bark", "brick", "carpet", "cloth", "concrete", "dirt", "flesh", "foliage_debris", "glass",
			"grass", "gravel", "ice", "metal_solid", "metal_grate", "mud", "paper", "plaster", "rock", "sand", "snow",
			"water_waist", "wood_solid", "asphalt", "ceramic", "plastic_solid", "rubber", "fruit", "paintedmetal",
			"riotshield", "slush", "asphalt_wet", "asphalt_debris", "concrete_wet", "concrete_debris",
			"foliage_vegetation", "foliage_leaves", "grass_tall", "metal_hollow", "metal_vehicle", "metal_thin",
			"metal_wet", "metal_debris", "plastic_hollow", "plastic_tarp", "rock_wet", "rock_debris", "water_ankle",
			"water_knee", "wood_hollow", "wood_wet", "wood_debris", "cushion",
		};

		const std::unordered_map<std::string_view, std::string_view> s2_surface_renames
		{
			{"brick_dark", "brick"},
			{"brick_light", "brick"},
			{"brick_red", "brick"},
			{"mud_dry", "mud"},
			{"mud_wet", "mud"},
			{"sand_dry", "sand"},
			{"sand_wet", "sand"},
			{"hay", "foliage_debris"},
			{"metal_armor", "metal_solid"},
			{"concrete_debris_small", "concrete_debris"},
			{"rock_debris_small", "rock_debris"},
			{"snow_hard", "snow"},
		};

		std::array<std::uint32_t, 64> build_surface_type_map()
		{
			std::array<std::uint32_t, 64> map{};
			for (auto i = 0u; i < s2_surface_types.size(); i++)
			{
				auto name = s2_surface_types[i];
				if (const auto rename = s2_surface_renames.find(name); rename != s2_surface_renames.end())
				{
					name = rename->second;
				}

				const auto h1_type = std::find(h1_surface_types.begin(), h1_surface_types.end(), name);
				map[i] = h1_type == h1_surface_types.end() ? 0 : static_cast<std::uint32_t>(h1_type - h1_surface_types.begin());
			}

			return map;
		}
	}

	std::uint32_t convert_surface_type(const std::uint32_t s2_type)
	{
		static const auto map = build_surface_type_map();
		return map[s2_type & 0x3F];
	}

	std::uint32_t convert_surface_flags(const std::uint32_t s2_flags)
	{
		return (s2_flags & ~(0x3Fu << 20)) | (convert_surface_type(s2_flags >> 20) << 20);
	}

	void add_csv_row(const std::string_view type, const std::string_view name, const bool referenced)
	{
		if (csv_keys.insert(std::format("{},{}", type, name)).second)
		{
			csv_rows.emplace_back(std::string(type), std::string(name), referenced);
		}
	}
}

namespace zonetool::convert::s2_h1
{
	namespace
	{
		namespace source = zonetool::s2;
		namespace target = zonetool::s2::converter::h1;

		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

#include "debug_probe.inl"

		void dump_asset(const std::int32_t type, void* header)
		{
			if (std::getenv("ZT_PROBE_ONLY") && type != source::ASSET_TYPE_CLIPMAP && type != source::ASSET_TYPE_GFXWORLD && type != source::ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE && type != source::ASSET_TYPE_COMWORLD && type != source::ASSET_TYPE_MAP_ENTS && type != source::ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE && type != source::ASSET_TYPE_XMODEL && type != source::ASSET_TYPE_XMODEL_SURFS && type != source::ASSET_TYPE_FXWORLD && type != source::ASSET_TYPE_GLASSWORLD && type != source::ASSET_TYPE_PHYSWORLDMAP) return;
			if (std::getenv("ZT_PROBE_SKY"))
			{
				if (type == source::ASSET_TYPE_MATERIAL) probe_sky_shaders(header_as<source::Material>(header));
				return;
			}
			switch (type)
			{
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				target::glassworld::dump(header_as<source::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_PHYSWORLDMAP:
				target::physworld::dump(header_as<source::PhysWorld>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				target::lightsets::dump(header_as<source::StringTable>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				if (std::getenv("ZT_PROBE_ONLY")) { probe_clipmap(header_as<source::clipMap_t>(header)); probe_clip_trees(header_as<source::clipMap_t>(header)); probe_clip_facts(header_as<source::clipMap_t>(header)); }
				target::clipmap::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE:
				target::clipmap::add_transient_zone(header_as<source::ClipMapTrZone>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				if (std::getenv("ZT_PROBE_ONLY")) { probe_surface_bytes(header_as<source::GfxWorld>(header)); probe_smodel_flags(header_as<source::GfxWorld>(header)); probe_smodel_bounds(header_as<source::GfxWorld>(header)); probe_light_grid(header_as<source::GfxWorld>(header)); probe_smodel_sh(header_as<source::GfxWorld>(header)); probe_surface_env(header_as<source::GfxWorld>(header)); }
				target::gfxworld::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE:
				target::gfxworld::add_transient_zone(header_as<source::GfxMapTrZone>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				if (std::getenv("ZT_PROBE_LODS")) probe_xmodel_lods(header_as<source::XModel>(header));
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_IMAGE:
				if (std::getenv("ZT_SKIP_IMAGES")) break;
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			target::techset::set_h1_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			target::material::finish();
			target::gfxworld::finish();
			target::comworld::finish();
			target::clipmap::finish();
			target::write_csv(zone_name);
		}
	}

	const converter& definition()
	{
		static const converter s2_h1
		{
			.source = "s2",
			.target = "h1",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
		};

		return s2_h1;
	}
}
