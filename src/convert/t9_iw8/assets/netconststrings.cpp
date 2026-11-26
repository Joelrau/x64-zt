#include <std_include.hpp>

#include "../include.hpp"
#include "netconststrings.hpp"

#include <iw8/assets/netconststrings.hpp>

namespace zonetool::t9::converter::iw8::netconststrings
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

		std::vector<std::string> level_models;

		void dump_table(const target::NetConstStringType type, const unsigned int count, const char** strings)
		{
			const auto name = std::format("ncs_{}_level", level_tags[type]);

			target::NetConstStrings asset{};
			asset.name = name.data();
			asset.stringType = type;
			asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
			asset.entryCount = count;
			asset.stringList = strings;
			target::net_const_strings::dump(&asset);

			add_csv_row("netconststrings", name);
		}
	}

	void add_level_model(const std::string& name)
	{
		if (std::ranges::find(level_models, name) == level_models.end())
		{
			level_models.emplace_back(name);
		}
	}

	void dump_level_strings()
	{
		std::vector<const char*> models;
		for (const auto& name : level_models)
		{
			models.emplace_back(name.data());
		}

		dump_table(target::NETCONSTSTRINGTYPE_XMODEL, static_cast<unsigned int>(models.size()), models.empty() ? nullptr : models.data());
		level_models.clear();

		for (auto type = 1; type < target::NETCONSTSTRINGTYPE_COUNT; type++)
		{
			dump_table(static_cast<target::NetConstStringType>(type), 0, nullptr);
		}
	}
}
