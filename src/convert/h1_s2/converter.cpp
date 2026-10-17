#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::h1::converter::s2
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
		std::filesystem::path target_folder;

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

		constexpr std::array<std::string_view, SURF_TYPE_COUNT> h1_surface_types
		{
			"default", "bark", "brick", "carpet", "cloth", "concrete", "dirt", "flesh", "foliage_debris", "glass",
			"grass", "gravel", "ice", "metal_solid", "metal_grate", "mud", "paper", "plaster", "rock", "sand", "snow",
			"water_waist", "wood_solid", "asphalt", "ceramic", "plastic_solid", "rubber", "fruit", "paintedmetal",
			"riotshield", "slush", "asphalt_wet", "asphalt_debris", "concrete_wet", "concrete_debris",
			"foliage_vegetation", "foliage_leaves", "grass_tall", "metal_hollow", "metal_vehicle", "metal_thin",
			"metal_wet", "metal_debris", "plastic_hollow", "plastic_tarp", "rock_wet", "rock_debris", "water_ankle",
			"water_knee", "wood_hollow", "wood_wet", "wood_debris", "cushion",
		};

		const std::unordered_map<std::string_view, std::string_view> h1_surface_renames
		{
			{"brick", "brick_red"},
			{"mud", "mud_dry"},
			{"sand", "sand_dry"},
		};

		std::array<std::uint32_t, 64> build_surface_type_map()
		{
			std::array<std::uint32_t, 64> map{};
			for (auto i = 0u; i < h1_surface_types.size(); i++)
			{
				auto name = h1_surface_types[i];
				if (const auto rename = h1_surface_renames.find(name); rename != h1_surface_renames.end())
				{
					name = rename->second;
				}

				const auto s2_type = std::find(s2_surface_types.begin(), s2_surface_types.end(), name);
				map[i] = s2_type == s2_surface_types.end() ? 0 : static_cast<std::uint32_t>(s2_type - s2_surface_types.begin());
			}

			return map;
		}
	}

	std::uint32_t convert_surface_type(const std::uint32_t h1_type)
	{
		static const auto map = build_surface_type_map();
		return map[h1_type & 0x3F];
	}

	std::uint32_t convert_surface_flags(const std::uint32_t h1_flags)
	{
		return (h1_flags & ~(0x3Fu << 20)) | (convert_surface_type(h1_flags >> 20) << 20);
	}

	std::uint64_t convert_surface_type_bits(const std::uint64_t h1_bits)
	{
		std::uint64_t result{};
		for (auto i = 0u; i < SURF_TYPE_COUNT; i++)
		{
			if (h1_bits & (1ull << i))
			{
				result |= 1ull << convert_surface_type(i);
			}
		}

		return result;
	}

	void add_csv_row(const std::string_view type, const std::string_view name, const bool referenced)
	{
		if (csv_keys.insert(std::format("{},{}", type, name)).second)
		{
			csv_rows.emplace_back(std::string(type), std::string(name), referenced);
		}
	}

	const std::filesystem::path& s2_folder()
	{
		if (target_folder.empty())
		{
			throw std::runtime_error("S2 game folder is not set");
		}

		return target_folder;
	}

	void set_s2_folder(const std::filesystem::path& folder)
	{
		target_folder = folder;
		zonetool::s2::set_zone_key_folder(folder);
	}

	void finish(const std::string& zone_name)
	{
		material::finish();
		comworld::finish();
		gfxworld::finish();
		clipmap::finish();
		comworld::clear();
		write_csv(zone_name);
	}
}

namespace zonetool::convert::h1_s2
{
	namespace
	{
		namespace source = zonetool::h1;
		namespace target = zonetool::h1::converter::s2;

		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			switch (type)
			{
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clipmap::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				target::gfxworld::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				target::glassworld::dump(header_as<source::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_PHYSWORLDMAP:
				target::physworld::dump(header_as<source::PhysWorld>(header));
				break;
			case source::ASSET_TYPE_SOUND:
				target::sound::dump(header_as<source::snd_alias_list_t>(header));
				break;
			case source::ASSET_TYPE_SOUND_CURVE:
				target::sound::dump_curve(header_as<source::SndCurve>(header));
				break;
			case source::ASSET_TYPE_REVERB_CURVE:
				target::sound::dump_reverb_curve(header_as<source::SndCurve>(header));
				break;
			case source::ASSET_TYPE_LOADED_SOUND:
				target::loaded_sound::dump(header_as<source::LoadedSound>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			target::set_s2_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			target::finish(zone_name);
		}
	}

	const converter& definition()
	{
		static const converter h1_s2
		{
			.source = "h1",
			.target = "s2",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
		};

		return h1_s2;
	}
}
