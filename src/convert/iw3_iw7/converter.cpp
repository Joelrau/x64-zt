#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>
#include <iw3/iwd.hpp>

#include <iw3_iw4/assets/xanim_parts.hpp>
#include <iw4_iw5/assets/xanim_parts.hpp>
#include <iw5_iw7/assets/xanim_parts.hpp>
#include <iw3_iw4/assets/xmodel.hpp>
#include <iw4_iw5/assets/xmodel.hpp>
#include <iw5_iw7/assets/xmodel.hpp>
#include <iw3_iw4/assets/gfx_image.hpp>
#include <iw4_iw5/assets/gfx_image.hpp>
#include <iw5_iw7/assets/gfx_image.hpp>
#include <iw3_iw4/assets/comworld.hpp>
#include <iw4_iw5/assets/comworld.hpp>
#include <iw5_iw7/assets/comworld.hpp>
#include <iw3_iw4/assets/mapents.hpp>
#include <iw4_iw5/assets/mapents.hpp>
#include <iw5_iw7/assets/mapents.hpp>
#include <iw3_iw4/assets/fx.hpp>
#include <iw4_iw5/assets/fx.hpp>
#include <iw5_iw7/assets/fx.hpp>
#include <iw3_iw4/assets/material.hpp>
#include <iw4_iw5/assets/material.hpp>
#include <iw5_iw7/assets/material.hpp>
#include <iw3_iw4/assets/clipmap.hpp>
#include <iw4_iw5/assets/clipmap.hpp>
#include <iw5_iw7/assets/clipmap.hpp>
#include <iw3_iw4/assets/gfxworld.hpp>
#include <iw4_iw5/assets/gfxworld.hpp>
#include <iw5_iw7/assets/gfxworld.hpp>
#include <iw7/assets/netconststrings.hpp>
#include <iw7/assets/aipaths.hpp>
#include <iw7/assets/glassworld.hpp>
#include <iw7/assets/scriptfile.hpp>

#include <utils/compression.hpp>

#include <xsk/gsc/engine/iw7.hpp>

namespace zonetool::convert::iw3_iw7
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_level_net_const_strings()
		{
			namespace target = zonetool::iw7;

			static const char* level_tags[]
			{
				"mdl", "mat", "rmb", "rmg", "veh", "efx", "vfx", "loc", "shk", "tag", "hic", "sic",
				"nps", "mic", "wep", "hnt", "anm", "acl", "lui", "sut", "ges", "tgt", "vsn",
			};

			for (auto type = 0; type <= target::NETCONSTSTRINGTYPE_VISION; type++)
			{
				const auto name = std::format("ncs_{}_level", level_tags[type]);

				target::NetConstStrings asset{};
				asset.name = name.data();
				asset.stringType = static_cast<target::NetConstStringType>(type);
				asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
				target::net_const_strings::dump(&asset); // empty lists: IW3 has no net const strings
			}
		}

		void dump_empty_path_data(const char* name)
		{
			namespace target = zonetool::iw7;

			target::pathnode_tree_t tree{};
			tree.axis = -1;

			static target::pathnode_t dynamic_nodes[64]{};

			target::PathData asset{};
			asset.name = name;
			asset.parentIndexResolved = true;
			asset.version = 22;
			asset.nodeTreeCount = 1;
			asset.nodeTree = &tree;
			asset.nodeCount = 64;
			asset.nodes = dynamic_nodes; // IW7 clears the dynamic node slots after the fixed nodes
			asset.maxDynamicSpawnedNodeCount = 64;
			target::path_data::dump(&asset); // IW3 multiplayer maps have no path nodes, IW7 game init needs the asset
		}

		void dump_empty_glass_world(const char* name)
		{
			namespace target = zonetool::iw7;

			target::G_GlassData data{};
			target::GlassWorld asset{};
			asset.name = name;
			asset.g_glassData = &data;
			target::glass_world::dump(&asset); // IW3 has no breakable glass, IW7 game init reads the glass data of the map
		}

		void dump_level_script(const char* world_name)
		{
			namespace target = zonetool::iw7;

			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();

			const auto map = std::filesystem::path(world_name).stem().string();
			const auto name = std::format("scripts/mp/maps/{}/{}", map, map);
			const auto source = std::format(R"(main()
{{
    scripts\mp\load::main();
    scripts\mp\compass::setupminimap( "compass_map_{}" );
    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";
}}
)", map);
			std::vector<std::uint8_t> data(source.begin(), source.end());

			const auto assembly = context->compiler().compile(name + ".gsc", data);
			const auto [bytecode, stack, devmap] = context->assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			target::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			target::scriptfile::dump(&script); // IW3 level scripts are source gsc, IW7 game init needs a compiled level script
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw3;

			x86::allocator allocator;
			zonetool::iw5::converter::iw7::gfx_image::read_file = zonetool::iw3::read_iwd_file;

			switch (type)
			{
			case source::ASSET_TYPE_XANIMPARTS:
			{
				const auto iw4_asset = source::converter::iw4::xanim_parts::convert(header_as<source::XAnimParts>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::xanim_parts::convert(iw4_asset);
				zonetool::iw5::converter::iw7::xanim_parts::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_XMODEL:
			{
				const auto iw4_asset = source::converter::iw4::xmodel::convert(header_as<source::XModel>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::xmodel::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::xmodel::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_IMAGE:
			{
				const auto iw4_asset = source::converter::iw4::gfx_image::convert(header_as<source::GfxImage>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::gfx_image::convert(iw4_asset);
				zonetool::iw5::converter::iw7::gfx_image::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_COMWORLD:
			{
				const auto iw4_asset = source::converter::iw4::comworld::convert(header_as<source::ComWorld>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::comworld::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::comworld::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_CLIPMAP:
			case source::ASSET_TYPE_CLIPMAP_PVS: // multiplayer maps ship the same struct under this type
			{
				const auto iw4_asset = source::converter::iw4::clipmap::convert(header_as<source::clipMap_t>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::clipmap::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::clipmap::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_GFXWORLD:
			{
				const auto iw4_asset = source::converter::iw4::gfxworld::convert(header_as<source::GfxWorld>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::gfxworld::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::gfxworld::dump(iw5_asset);
				dump_level_net_const_strings();
				dump_empty_path_data(iw5_asset->name);
				dump_empty_glass_world(iw5_asset->name);
				dump_level_script(iw5_asset->name);
				break;
			}
			case source::ASSET_TYPE_MAP_ENTS:
				break; // the clipmap conversion carries the full mapents
			case source::ASSET_TYPE_FX:
			{
				const auto iw4_asset = source::converter::iw4::fx::convert(header_as<source::FxEffectDef>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::fx::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::fx::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_MATERIAL:
			{
				const auto iw4_asset = source::converter::iw4::material::convert(header_as<source::Material>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::material::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::material::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_RAWFILE:
				source::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				source::string_table::dump(header_as<source::StringTable>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				source::localize::dump(header_as<source::LocalizeEntry>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter iw3_iw7
		{
			.source = "iw3",
			.target = "iw7",
			.dump_asset = dump_asset,
		};

		return iw3_iw7;
	}
}
