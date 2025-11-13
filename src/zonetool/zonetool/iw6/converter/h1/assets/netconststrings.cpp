#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "scriptfile.hpp"

#include "zonetool/h1/assets/netconststrings.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace net_const_strings
		{
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
			}
		}
	}
}