#include <std_include.hpp>
#include "../include.hpp"
#include "scriptfile.hpp"

#include <h1/assets/netconststrings.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace net_const_strings
		{
			namespace
			{
				constexpr const char* level_net_const_string_tags[zonetool::h1::NETCONSTSTRINGTYPE_ASSET_COUNT]
				{
					"mdl", "mat", "rmb", "veh", "vfx", "loc", "snd", "sbx", "snl", "shk", "mnu", "tag",
					"hic", "nps", "mic", "sel", "wep", "att", "hnt", "anm", "fxt", "acl", "lui", "lsr",
				};

				std::unordered_set<std::string> dumped_names;
			}

			zonetool::h1::NetConstStringType convert_type(NetConstStringType t)
			{
				switch (t)
				{
				case NETCONSTSTRINGTYPE_XMODEL: return zonetool::h1::NETCONSTSTRINGTYPE_XMODEL;
				case NETCONSTSTRINGTYPE_MATERIAL: return zonetool::h1::NETCONSTSTRINGTYPE_MATERIAL;
				case NETCONSTSTRINGTYPE_RUMBLE: return zonetool::h1::NETCONSTSTRINGTYPE_RUMBLE;
				case NETCONSTSTRINGTYPE_VEHICLES: return zonetool::h1::NETCONSTSTRINGTYPE_VEHICLES;
				case NETCONSTSTRINGTYPE_FX: return zonetool::h1::NETCONSTSTRINGTYPE_FX;
				case NETCONSTSTRINGTYPE_LOCSTRING: return zonetool::h1::NETCONSTSTRINGTYPE_LOCSTRING;
				case NETCONSTSTRINGTYPE_SOUNDALIAS: return zonetool::h1::NETCONSTSTRINGTYPE_SOUNDALIAS;
				case NETCONSTSTRINGTYPE_SOUNDALIAS_LOOPING: return zonetool::h1::NETCONSTSTRINGTYPE_SOUNDALIAS_LOOPING;
				case NETCONSTSTRINGTYPE_SHOCK: return zonetool::h1::NETCONSTSTRINGTYPE_SHOCK;
				case NETCONSTSTRINGTYPE_SCRIPTMENU: return zonetool::h1::NETCONSTSTRINGTYPE_SCRIPTMENU;
				case NETCONSTSTRINGTYPE_CLIENT_TAGS: return zonetool::h1::NETCONSTSTRINGTYPE_CLIENT_TAGS;
				case NETCONSTSTRINGTYPE_HEADICON: return zonetool::h1::NETCONSTSTRINGTYPE_HEADICON;
				case NETCONSTSTRINGTYPE_STATUSICON: return zonetool::h1::NETCONSTSTRINGTYPE_HEADICON;
				case NETCONSTSTRINGTYPE_NAMEPLATE: return zonetool::h1::NETCONSTSTRINGTYPE_NAMEPLATE;
				case NETCONSTSTRINGTYPE_MINIMAPICON: return zonetool::h1::NETCONSTSTRINGTYPE_MINIMAPICON;
				case NETCONSTSTRINGTYPE_LOCSELMAT: return zonetool::h1::NETCONSTSTRINGTYPE_LOCSELMAT;
				case NETCONSTSTRINGTYPE_WEAPON: return zonetool::h1::NETCONSTSTRINGTYPE_WEAPON;
				case NETCONSTSTRINGTYPE_HINTSTRING: return zonetool::h1::NETCONSTSTRINGTYPE_HINTSTRING;
				case NETCONSTSTRINGTYPE_ANIM: return zonetool::h1::NETCONSTSTRINGTYPE_ANIM;
				case NETCONSTSTRINGTYPE_TAGS: return zonetool::h1::NETCONSTSTRINGTYPE_TAGS;
				case NETCONSTSTRINGTYPE_ANIMCLASS: return zonetool::h1::NETCONSTSTRINGTYPE_ANIMCLASS;
				case NETCONSTSTRINGTYPE_LUI: return zonetool::h1::NETCONSTSTRINGTYPE_LUI;
				default: return zonetool::h1::NETCONSTSTRINGTYPE_NONE;
				}
			}

			zonetool::h1::NetConstStrings* convert(NetConstStrings* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::NetConstStrings>();
				std::memcpy(new_asset, asset, sizeof(NetConstStrings));
				new_asset->stringType = convert_type(asset->stringType);
				return new_asset;
			}

			void dump(NetConstStrings* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::net_const_strings::dump(converted_asset);
				dumped_names.insert(asset->name);
			}

			void add_missing_level_strings()
			{
				const auto has_level_strings = std::ranges::any_of(dumped_names, [](const std::string& name)
				{
					return name.ends_with("_level");
				});

				if (!has_level_strings)
				{
					dumped_names.clear();
					return;
				}

				// IW6 has no submix, attachment or laser types, but H1 needs one ncs_<tag>_level asset per type
				for (auto type = 0; type < zonetool::h1::NETCONSTSTRINGTYPE_ASSET_COUNT; type++)
				{
					const auto name = std::format("ncs_{}_level", level_net_const_string_tags[type]);
					if (dumped_names.contains(name))
					{
						continue;
					}

					zonetool::h1::NetConstStrings asset{};
					asset.name = name.data();
					asset.stringType = static_cast<zonetool::h1::NetConstStringType>(type);
					asset.sourceType = zonetool::h1::NETCONSTSTRINGSOURCE_MAP;
					zonetool::h1::net_const_strings::dump(&asset);

					add_csv_row("netconststrings", name);
				}

				dumped_names.clear();
			}
		}
	}
}