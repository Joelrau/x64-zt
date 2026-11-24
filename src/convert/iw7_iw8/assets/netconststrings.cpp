#include <std_include.hpp>

#include "../include.hpp"
#include "netconststrings.hpp"
#include "xmodel.hpp"

#include <iw8/assets/netconststrings.hpp>

namespace zonetool::iw7::converter::iw8::netconststrings
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

		std::unordered_set<int> dumped_types;
		std::vector<std::string> level_models;

		std::optional<target::NetConstStringType> convert_type(const NetConstStringType type)
		{
			switch (type)
			{
			case NETCONSTSTRINGTYPE_XMODEL: return target::NETCONSTSTRINGTYPE_XMODEL;
			case NETCONSTSTRINGTYPE_MATERIAL: return target::NETCONSTSTRINGTYPE_MATERIAL;
			case NETCONSTSTRINGTYPE_RUMBLE: return target::NETCONSTSTRINGTYPE_RUMBLE;
			case NETCONSTSTRINGTYPE_RUMBLE_GRAPH: return target::NETCONSTSTRINGTYPE_RUMBLEGRAPH;
			case NETCONSTSTRINGTYPE_VEHICLES: return target::NETCONSTSTRINGTYPE_VEHICLES;
			case NETCONSTSTRINGTYPE_VFX: return target::NETCONSTSTRINGTYPE_VFX;
			case NETCONSTSTRINGTYPE_LOCSTRING: return target::NETCONSTSTRINGTYPE_LOCSTRING;
			case NETCONSTSTRINGTYPE_SHOCK: return target::NETCONSTSTRINGTYPE_SHOCK;
			case NETCONSTSTRINGTYPE_CLIENT_TAGS: return target::NETCONSTSTRINGTYPE_CLIENT_TAGS;
			case NETCONSTSTRINGTYPE_HEADICON: return target::NETCONSTSTRINGTYPE_HEADICON;
			case NETCONSTSTRINGTYPE_STATUSICON: return target::NETCONSTSTRINGTYPE_STATUSICON;
			case NETCONSTSTRINGTYPE_MINIMAPICON: return target::NETCONSTSTRINGTYPE_MINIMAPICON;
			case NETCONSTSTRINGTYPE_WEAPON: return target::NETCONSTSTRINGTYPE_WEAPON;
			case NETCONSTSTRINGTYPE_HINTSTRING: return target::NETCONSTSTRINGTYPE_HINTSTRING;
			case NETCONSTSTRINGTYPE_ANIM: return target::NETCONSTSTRINGTYPE_ANIM;
			case NETCONSTSTRINGTYPE_ANIMCLASS: return target::NETCONSTSTRINGTYPE_ANIMCLASS;
			case NETCONSTSTRINGTYPE_LUI: return target::NETCONSTSTRINGTYPE_LUI;
			case NETCONSTSTRINGTYPE_SUIT: return target::NETCONSTSTRINGTYPE_SUIT;
			case NETCONSTSTRINGTYPE_GESTURE: return target::NETCONSTSTRINGTYPE_GESTURE;
			case NETCONSTSTRINGTYPE_TGT: return target::NETCONSTSTRINGTYPE_SPACESHIPTARGET;
			case NETCONSTSTRINGTYPE_VISION: return target::NETCONSTSTRINGTYPE_VISIONSET;
			default: return {};
			}
		}

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

			dumped_types.emplace(type);
			add_csv_row("netconststrings", name);
		}

		void dump_models(const unsigned int count, const char** names)
		{
			for (auto i = 0u; i < count; i++)
			{
				if (!names[i])
				{
					continue;
				}

				if (const auto model = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, names[i])); model && model->numsurfs)
				{
					xmodel::dump(model);
				}
			}
		}
	}

	void dump(NetConstStrings* asset)
	{
		const auto type = convert_type(asset->stringType);
		if (!type)
		{
			if (asset->entryCount)
			{
				ZONETOOL_WARNING("netconststrings \"%s\": %u strings have no IW8 type", asset->name, asset->entryCount);
			}

			return;
		}

		if (!std::string_view(asset->name).ends_with("_level") || dumped_types.contains(*type))
		{
			return;
		}

		if (*type == target::NETCONSTSTRINGTYPE_XMODEL)
		{
			dump_models(asset->entryCount, asset->stringList);
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				if (asset->stringList[i])
				{
					add_level_model(asset->stringList[i]);
				}
			}

			return;
		}

		dump_table(*type, asset->entryCount, asset->stringList);
	}

	void add_level_model(const std::string& name)
	{
		if (std::ranges::find(level_models, name) == level_models.end())
		{
			level_models.emplace_back(name);
		}
	}

	void dump_missing_level_strings()
	{
		std::vector<const char*> models;
		for (const auto& name : level_models)
		{
			models.emplace_back(name.data());
		}

		dump_table(target::NETCONSTSTRINGTYPE_XMODEL, static_cast<unsigned int>(models.size()), models.empty() ? nullptr : models.data());
		level_models.clear();

		for (auto type = 0; type < target::NETCONSTSTRINGTYPE_COUNT; type++)
		{
			if (!dumped_types.contains(type))
			{
				dump_table(static_cast<target::NetConstStringType>(type), 0, nullptr);
			}
		}

		dumped_types.clear();
	}
}
