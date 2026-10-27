#include <std_include.hpp>

#include "../include.hpp"
#include "netconststrings.hpp"

namespace zonetool::h1::converter::s2::netconststrings
{
	namespace
	{
		constexpr auto s2_type_none = zonetool::s2::NETCONSTSTRINGTYPE_ASSET_COUNT;

		constexpr std::array<std::string_view, zonetool::s2::NETCONSTSTRINGTYPE_ASSET_COUNT> s2_tags
		{
			"mdl", "mat", "rmb", "veh", "vfx", "loc", "snd", "sbx", "shk", "mnu", "tag", "por",
			"hic", "nps", "mic", "sel", "wep", "att", "hnt", "anm", "acl", "lui", "lsr", "css",
		};

		constexpr std::array<std::string_view, NETCONSTSTRINGTYPE_ASSET_COUNT> h1_tags
		{
			"mdl", "mat", "rmb", "veh", "vfx", "loc", "snd", "sbx", "snl", "shk", "mnu", "tag",
			"hic", "nps", "mic", "sel", "wep", "att", "hnt", "anm", "fxt", "acl", "lui", "lsr",
		};

		struct net_const_strings
		{
			zonetool::s2::NetConstStringType type;
			int source;
			std::vector<std::string> strings;
		};

		std::map<std::string, net_const_strings> converted;
		bool dumping_map{};

		zonetool::s2::NetConstStringType convert_type(const NetConstStringType type)
		{
			switch (type)
			{
			case NETCONSTSTRINGTYPE_XMODEL: return zonetool::s2::NETCONSTSTRINGTYPE_XMODEL;
			case NETCONSTSTRINGTYPE_MATERIAL: return zonetool::s2::NETCONSTSTRINGTYPE_MATERIAL;
			case NETCONSTSTRINGTYPE_RUMBLE: return zonetool::s2::NETCONSTSTRINGTYPE_RUMBLE;
			case NETCONSTSTRINGTYPE_VEHICLES: return zonetool::s2::NETCONSTSTRINGTYPE_VEHICLES;
			case NETCONSTSTRINGTYPE_FX: return zonetool::s2::NETCONSTSTRINGTYPE_FX;
			case NETCONSTSTRINGTYPE_LOCSTRING: return zonetool::s2::NETCONSTSTRINGTYPE_LOCSTRING;
			case NETCONSTSTRINGTYPE_SOUNDALIAS: return zonetool::s2::NETCONSTSTRINGTYPE_SOUNDALIAS;
			case NETCONSTSTRINGTYPE_SUBMIX: return zonetool::s2::NETCONSTSTRINGTYPE_SUBMIX;
			case NETCONSTSTRINGTYPE_SOUNDALIAS_LOOPING: return zonetool::s2::NETCONSTSTRINGTYPE_SOUNDALIAS; // s2 lists looping aliases with the others
			case NETCONSTSTRINGTYPE_SHOCK: return zonetool::s2::NETCONSTSTRINGTYPE_SHOCK;
			case NETCONSTSTRINGTYPE_SCRIPTMENU: return zonetool::s2::NETCONSTSTRINGTYPE_SCRIPTMENU;
			case NETCONSTSTRINGTYPE_CLIENT_TAGS: return zonetool::s2::NETCONSTSTRINGTYPE_CLIENT_TAGS;
			case NETCONSTSTRINGTYPE_HEADICON: return zonetool::s2::NETCONSTSTRINGTYPE_HEADICON;
			case NETCONSTSTRINGTYPE_NAMEPLATE: return zonetool::s2::NETCONSTSTRINGTYPE_NAMEPLATE;
			case NETCONSTSTRINGTYPE_MINIMAPICON: return zonetool::s2::NETCONSTSTRINGTYPE_MINIMAPICON;
			case NETCONSTSTRINGTYPE_LOCSELMAT: return zonetool::s2::NETCONSTSTRINGTYPE_LOCSELMAT;
			case NETCONSTSTRINGTYPE_WEAPON: return zonetool::s2::NETCONSTSTRINGTYPE_WEAPON;
			case NETCONSTSTRINGTYPE_ATTACHMENT: return zonetool::s2::NETCONSTSTRINGTYPE_ATTACHMENT;
			case NETCONSTSTRINGTYPE_HINTSTRING: return zonetool::s2::NETCONSTSTRINGTYPE_HINTSTRING;
			case NETCONSTSTRINGTYPE_ANIM: return zonetool::s2::NETCONSTSTRINGTYPE_ANIM;
			case NETCONSTSTRINGTYPE_ANIMCLASS: return zonetool::s2::NETCONSTSTRINGTYPE_ANIMCLASS;
			case NETCONSTSTRINGTYPE_LUI: return zonetool::s2::NETCONSTSTRINGTYPE_LUI;
			case NETCONSTSTRINGTYPE_LASER: return zonetool::s2::NETCONSTSTRINGTYPE_LASER;
			default: return s2_type_none;
			}
		}

		std::string convert_name(const std::string& name, const NetConstStringType type, const zonetool::s2::NetConstStringType new_type)
		{
			const auto prefix = std::format("ncs_{}_", h1_tags[type]);
			if (!name.starts_with(prefix))
			{
				return name;
			}

			return std::format("ncs_{}_{}", s2_tags[new_type], name.substr(prefix.size()));
		}

		void dump_converted(const std::string& name, net_const_strings& asset)
		{
			std::vector<const char*> strings;
			for (const auto& string : asset.strings)
			{
				strings.emplace_back(string.data());
			}

			zonetool::s2::NetConstStrings new_asset{};
			new_asset.name = name.data();
			new_asset.stringType = asset.type;
			new_asset.sourceType = asset.source;
			new_asset.entryCount = static_cast<unsigned int>(strings.size());
			new_asset.stringList = strings.empty() ? nullptr : strings.data();
			zonetool::s2::net_const_strings::dump(&new_asset);
			add_csv_row("netconststrings", name);
		}
	}

	void dump(NetConstStrings* asset)
	{
		if (asset->stringType >= NETCONSTSTRINGTYPE_ASSET_COUNT)
		{
			ZONETOOL_WARNING("netconststrings \"%s\": dvar string types are not converted, dropping it", asset->name);
			return;
		}

		const auto type = convert_type(asset->stringType);
		if (type == s2_type_none)
		{
			ZONETOOL_WARNING("netconststrings \"%s\": S2 has no \"%s\" type, dropping it", asset->name, h1_tags[asset->stringType].data());
			return;
		}

		auto& entry = converted[convert_name(asset->name, asset->stringType, type)];
		entry.type = type;
		entry.source = asset->sourceType;
		for (auto i = 0u; asset->stringList && i < asset->entryCount; i++)
		{
			if (std::find(entry.strings.begin(), entry.strings.end(), asset->stringList[i]) == entry.strings.end())
			{
				entry.strings.emplace_back(asset->stringList[i]);
			}
		}
	}

	void add_level_strings()
	{
		dumping_map = true;
	}

	void finish()
	{
		for (auto it = converted.begin(); dumping_map && it != converted.end();)
		{
			const auto tag = s2_tags[it->second.type];
			if (!it->first.starts_with(std::format("ncs_{}_patch_", tag)))
			{
				++it;
				continue;
			}

			auto& level = converted[std::format("ncs_{}_level", tag)];
			level.type = it->second.type;
			level.source = NETCONSTSTRINGSOURCE_MAP;
			for (auto& string : it->second.strings)
			{
				if (std::ranges::find(level.strings, string) == level.strings.end())
				{
					level.strings.emplace_back(std::move(string));
				}
			}

			it = converted.erase(it);
		}

		// s2 maps need an ncs_<tag>_level asset of every type
		for (auto type = 0; dumping_map && type < zonetool::s2::NETCONSTSTRINGTYPE_ASSET_COUNT; type++)
		{
			const auto name = std::format("ncs_{}_level", s2_tags[type]);
			if (!converted.contains(name))
			{
				converted[name] = {static_cast<zonetool::s2::NetConstStringType>(type), NETCONSTSTRINGSOURCE_MAP};
			}
		}

		for (auto& [name, asset] : converted)
		{
			dump_converted(name, asset);
		}

		converted.clear();
		dumping_map = false;
	}
}
