#include <std_include.hpp>

#include "../include.hpp"
#include "netconststrings.hpp"

#include <iw7/assets/netconststrings.hpp>

namespace zonetool::iw8::converter::iw7::netconststrings
{
	namespace
	{
		namespace target = zonetool::iw7;

		const char* level_tags[target::NETCONSTSTRINGTYPE_VISION + 1]
		{
			"mdl", "mat", "rmb", "rmg", "veh", "efx", "vfx", "loc", "shk", "tag", "hic", "sic",
			"nps", "mic", "wep", "hnt", "anm", "acl", "lui", "sut", "ges", "tgt", "vsn",
		};
	}

	void dump_level_strings()
	{
		for (auto type = 0; type <= target::NETCONSTSTRINGTYPE_VISION; type++)
		{
			const auto name = std::format("ncs_{}_level", level_tags[type]);

			target::NetConstStrings asset{};
			asset.name = name.data();
			asset.stringType = static_cast<target::NetConstStringType>(type);
			asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
			target::net_const_strings::dump(&asset); // TODO: the IW8 level lists name assets that are not converted

			add_csv_row("netconststrings", name);
		}
	}
}
