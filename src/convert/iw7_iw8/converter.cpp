#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::iw7::converter::iw8
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

	bool mark_dumped(const XAssetType type, const std::string_view name)
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

		std::ranges::stable_partition(csv_rows, [](const std::string& row) { return row.starts_with("require,"); });

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

namespace zonetool::convert::iw7_iw8
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw7;
			namespace target = zonetool::iw7::converter::iw8;

			switch (type)
			{
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_TRACER:
				target::tracer::dump(header_as<source::TracerDef>(header));
				break;
			case source::ASSET_TYPE_XANIMPARTS:
				target::xanim::dump(header_as<source::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_ANIM_PACKAGE:
				target::animpackage::dump(header_as<source::WeaponAnimPackage>(header));
				break;
			case source::ASSET_TYPE_WEAPON:
				target::weapon::dump(header_as<source::WeaponCompleteDef>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			zonetool::iw7::converter::iw8::set_iw8_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			zonetool::iw7::converter::iw8::write_csv(zone_name);
		}

		std::vector<std::string> required_zones(const std::string&)
		{
			return {"code_post_gfx", "global_core_mp", "techsets_global_core_mp", "common_core_mp", "common_mp"};
		}
	}

	const converter& definition()
	{
		static const converter iw7_iw8
		{
			.source = "iw7",
			.target = "iw8",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
			.required_zones = required_zones,
		};

		return iw7_iw8;
	}
}
