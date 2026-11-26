#include <std_include.hpp>

#include "../include.hpp"
#include "netconststrings.hpp"

#include <iw8/assets/netconststrings.hpp>

namespace zonetool::s4::converter::iw8::netconststrings
{
	namespace
	{
		namespace target = zonetool::iw8;

		const char* level_tags[target::NETCONSTSTRINGTYPE_COUNT]
		{
			"mdl", "mat", "img", "rmb", "rmg", "veh", "vfx", "loc", "shk", "tag", "sic", "hic", "obj", "mic",
			"wep", "hnt", "anm", "acl", "nms", "lui", "sut", "cam", "hol", "ges", "tgt", "vsn", "atr", "ccr",
			"xac", "cin", "acc", "bs2", "cmo", "pas", "ait", "sbd", "xcm", "exx", "cob", "stk", "vcm",
		};

		std::map<int, std::vector<std::string>> level_strings;

		std::optional<target::NetConstStringType> find_type(const std::string_view name)
		{
			for (auto type = 0; type < target::NETCONSTSTRINGTYPE_COUNT; type++)
			{
				if (name == std::format("ncs_{}_level", level_tags[type]))
				{
					return static_cast<target::NetConstStringType>(type);
				}
			}

			return {};
		}

		void dump_table(const int type, const std::vector<std::string>& strings)
		{
			const auto name = std::format("ncs_{}_level", level_tags[type]);

			std::vector<const char*> string_list;
			for (const auto& string : strings)
			{
				string_list.emplace_back(string.data());
			}

			target::NetConstStrings asset{};
			asset.name = name.data();
			asset.stringType = static_cast<target::NetConstStringType>(type);
			asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
			asset.entryCount = static_cast<unsigned int>(string_list.size());
			asset.stringList = string_list.empty() ? nullptr : string_list.data();
			target::net_const_strings::dump(&asset);

			add_csv_row(target::type_to_string(target::ASSET_TYPE_NET_CONST_STRINGS), name);
		}
	}

	void dump(NetConstStrings* asset)
	{
		const std::string_view name = asset->name;
		if (!name.ends_with("_level"))
		{
			return;
		}

		const auto type = find_type(name);
		if (!type)
		{
			if (asset->entryCount)
			{
				ZONETOOL_WARNING("netconststrings \"%s\": %u strings have no IW8 type", asset->name, asset->entryCount);
			}

			return;
		}

		if (level_strings.contains(*type))
		{
			return;
		}

		auto& strings = level_strings[*type];
		for (auto i = 0u; i < asset->entryCount; i++)
		{
			strings.emplace_back(asset->stringList[i]);
		}
	}

	void add_level_string(const target::NetConstStringType type, const std::string& string)
	{
		auto& strings = level_strings[type];
		if (std::find(strings.begin(), strings.end(), string) == strings.end())
		{
			strings.emplace_back(string);
		}
	}

	void dump_level_strings()
	{
		for (auto type = 0; type < target::NETCONSTSTRINGTYPE_COUNT; type++)
		{
			dump_table(type, level_strings[type]);
		}

		level_strings.clear();
	}
}
