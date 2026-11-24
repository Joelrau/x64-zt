#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "gfximage.hpp"
#include "havok.hpp"
#include "material.hpp"
#include "rawfile.hpp"
#include "scriptable.hpp"
#include "vfx.hpp"

#include <iw8/assets/edgelist.hpp>
#include <iw8/assets/mapents.hpp>
#include <iw8/assets/scriptfile.hpp>

#include <utils/compression.hpp>
#include <utils/string.hpp>

#include <xsk/gsc/engine/iw7.hpp>
#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::iw7::converter::iw8::mapents
{
	namespace
	{
		namespace target = zonetool::iw8;

		xsk::gsc::iw7::context& source_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		script_context& target_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<script_context>();
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		std::string decompile(const ScriptFile* asset)
		{
			auto& context = source_context();
			const auto stack = utils::compression::zlib::decompress({asset->buffer, static_cast<std::size_t>(asset->compressedLen)});
			const auto assembly = context.disassembler().disassemble(
				std::vector<std::uint8_t>{asset->bytecode, asset->bytecode + asset->bytecodeLen},
				std::vector<std::uint8_t>{stack.begin(), stack.end()});

			const auto tree = context.decompiler().decompile(*assembly);
			const auto source = context.printer().print(*tree);
			return {source.begin(), source.end()};
		}

		std::string make_default_map_script(const std::string& map)
		{
			return std::format(R"(main()
{{
    scripts\mp\load::main();
    level.outofboundstriggers = getentarray( "OutOfBounds", "targetname" );
    scripts\mp\compass::setupminimap( "compass_map_{}" );
    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";
}}
)", map);
		}

		const std::regex& load_fx_pattern()
		{
			static const std::regex pattern(R"re(level\._effect\[\s*"([^"]+)"\s*\]\s*=\s*loadfx\s*\(\s*"([^"]+?)(?:\.vfx)?"\s*\))re", std::regex::icase);
			return pattern;
		}

		const std::regex& script_oneshot_pattern()
		{
			static const std::regex pattern(R"re(\s*(\w+)\s*=\s*scripts\\engine\\utility::createoneshoteffect\(\s*"([^"]+)"\s*\);\s*)re", std::regex::icase);
			return pattern;
		}

		const std::regex& script_createfx_field_pattern()
		{
			static const std::regex pattern(R"re(\s*(\w+)\.v\[\s*"(\w+)"\s*\]\s*=\s*(.+);\s*)re");
			return pattern;
		}

		std::vector<std::string> split_lines(const std::string& source)
		{
			auto lines = utils::string::split(source, '\n');
			for (auto& line : lines)
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}
			}

			return lines;
		}

		bool is_script_effect_line(const std::string& line, std::unordered_set<std::string>& createfx_variables)
		{
			std::smatch match;
			if (std::regex_search(line, load_fx_pattern()))
			{
				return true;
			}

			if (std::regex_match(line, match, script_oneshot_pattern()))
			{
				createfx_variables.emplace(match[1].str());
				return true;
			}

			return std::regex_match(line, match, script_createfx_field_pattern()) && createfx_variables.contains(match[1].str());
		}

		std::string make_map_script(const std::string& map, const std::string& source, const bool remove_effects) // TODO: convert the precache and art scripts and the render dvars
		{
			const std::regex map_script_call(std::format(R"re(\s*scripts\\mp\\maps\\{}\\.*::main\(\s*\);\s*)re", map));
			static const std::regex dvar_line(R"re(\s*setdvar\(.*\);\s*)re");

			std::string script;
			std::unordered_set<std::string> createfx_variables;
			for (const auto& line : split_lines(source))
			{
				if (!std::regex_match(line, map_script_call) && !std::regex_match(line, dvar_line)
					&& !(remove_effects && is_script_effect_line(line, createfx_variables)))
				{
					script += line + "\n";
				}
			}

			return script;
		}

		std::string decompile_map_script(const std::string& map)
		{
			const auto* asset = static_cast<const ScriptFile*>(db_find_asset(ASSET_TYPE_SCRIPTFILE, std::format("scripts/mp/maps/{}/{}", map, map).data()));
			if (!asset || !asset->buffer || !asset->bytecode)
			{
				return {};
			}

			try
			{
				return decompile(asset);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("scriptfile \"%s\" does not decompile: %s", asset->name, e.what());
				return {};
			}
		}

		void dump_map_script(const std::string& map, const std::string& map_script, const bool remove_effects)
		{
			const auto name = std::format("{}/{}.gsc", rawfile::get_map_script_folder(map), map);
			const auto source = map_script.empty() ? make_default_map_script(map) : make_map_script(map, map_script, remove_effects);
			std::vector<std::uint8_t> data(source.begin(), source.end());

			auto& context = target_context();
			const auto assembly = context.compiler().compile(name, data);
			const auto [bytecode, stack, devmap] = context.assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			target::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			target::scriptfile::dump(&script);
			add_csv_row("scriptfile", name);

			material::dump_2d(static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, std::format("compass_map_{}", map).data())));

			if (const auto* loadscreen = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, std::format("loadscreen_{}", map).data())))
			{
				gfximage::dump_preview(loadscreen);
			}
		}

		std::string get_rawfile_text(const std::string& name)
		{
			const auto* asset = static_cast<const RawFile*>(db_find_asset(ASSET_TYPE_RAWFILE, name.data()));
			return asset && asset->len ? rawfile::read_text(asset) : std::string{};
		}

		std::unordered_map<std::string, std::string> read_effect_paths(const std::string& source)
		{
			std::unordered_map<std::string, std::string> paths;
			for (auto it = std::sregex_iterator(source.begin(), source.end(), load_fx_pattern()); it != std::sregex_iterator(); ++it)
			{
				paths[(*it)[1].str()] = (*it)[2].str();
			}

			return paths;
		}

		std::string get_stock_createfx_script(const std::string& map)
		{
			return get_rawfile_text(std::format("{}/gen/{}_fx.gsc", rawfile::get_map_script_folder(map), map));
		}

		std::vector<createfx_oneshot> read_stock_oneshots(const std::string& source)
		{
			static const std::regex oneshot(R"re(createOneshotEffect\(\s*"([^"]+)"\s*\);\s*ent set_origin_and_angles\(\s*\(([^)]*)\)\s*,\s*\(([^)]*)\)\s*\);(?:\s*ent\.v\[\s*"fxid"\s*\]\s*=\s*"[^"]*"\s*;)?(?:\s*ent\.v\[\s*"delay"\s*\]\s*=\s*([-\d.]+)\s*;)?)re");

			std::vector<createfx_oneshot> oneshots;
			for (auto it = std::sregex_iterator(source.begin(), source.end(), oneshot); it != std::sregex_iterator(); ++it)
			{
				const auto& match = *it;
				oneshots.push_back({match[1].str(), match[2].str(), match[3].str(), match[4].matched ? match[4].str() : std::string{}});
			}

			return oneshots;
		}

		std::vector<createfx_oneshot> read_script_oneshots(const std::string& source)
		{
			static const std::regex vector_value(R"re(\(([^)]*)\))re");

			std::vector<createfx_oneshot> oneshots;
			std::string variable;
			std::smatch match;
			for (const auto& line : split_lines(source))
			{
				if (std::regex_match(line, match, script_oneshot_pattern()))
				{
					variable = match[1].str();
					oneshots.push_back({match[2].str()});
					continue;
				}

				if (oneshots.empty() || !std::regex_match(line, match, script_createfx_field_pattern()) || match[1].str() != variable)
				{
					continue;
				}

				const auto field = match[2].str();
				const auto value = match[3].str();
				std::smatch vector;
				auto& oneshot = oneshots.back();
				if (field == "origin" && std::regex_search(value, vector, vector_value))
				{
					oneshot.origin = vector[1].str();
				}
				else if (field == "angles" && std::regex_search(value, vector, vector_value))
				{
					oneshot.angles = vector[1].str();
				}
				else if (field == "delay")
				{
					oneshot.delay = value;
				}
			}

			return oneshots;
		}

		void read_vector(const std::string& text, target::vec3_t& vector)
		{
			std::sscanf(text.data(), "%f , %f , %f", &vector[0], &vector[1], &vector[2]);
		}

		bool convert_oneshot_effects(const std::string& map, const std::string& map_script, target::ClientSideEffects& effects, utils::memory::allocator& allocator)
		{
			const auto stock_script = get_stock_createfx_script(map);
			const auto paths = read_effect_paths(stock_script.empty() ? map_script
				: get_rawfile_text(std::format("{}/{}_fx.gsc", rawfile::get_map_script_folder(map), map)));
			const auto oneshots = stock_script.empty() ? read_script_oneshots(map_script) : read_stock_oneshots(stock_script);

			std::vector<target::ClientOneshotEffectDef> converted;
			for (const auto& oneshot : oneshots)
			{
				const auto path = paths.find(oneshot.fx);
				auto* vfx = path != paths.end() ? static_cast<ParticleSystemDef*>(db_find_asset(ASSET_TYPE_VFX, path->second.data())) : nullptr;
				if (!vfx)
				{
					ZONETOOL_WARNING("createfx oneshot \"%s\" has no vfx", oneshot.fx.data());
					continue;
				}

				vfx::dump(vfx);

				auto& effect = converted.emplace_back();
				read_vector(oneshot.origin, effect.origin);
				read_vector(oneshot.angles, effect.angles);
				effect.radius = 0.0f; // 0 spawns at load, no spatial tree (sub_16896F0)
				effect.delayMsec = oneshot.delay.empty() ? 0 : static_cast<int>(std::stof(oneshot.delay) * 1000.0f);
				effect.effect.particleSystemDef = allocator.allocate<target::ParticleSystemDef>();
				const_cast<target::ParticleSystemDef*>(effect.effect.particleSystemDef)->name = allocator.duplicate_string(vfx->name);
			}

			effects.oneshotEffectCount = static_cast<unsigned int>(converted.size());
			effects.oneshotEffects = allocator.allocate_array<target::ClientOneshotEffectDef>(converted.size());
			std::copy(converted.begin(), converted.end(), effects.oneshotEffects);
			return stock_script.empty() && !oneshots.empty();
		}

		std::string_view next_token(const std::string_view source, std::size_t& pos)
		{
			while (pos < source.size() && std::isspace(static_cast<unsigned char>(source[pos])))
			{
				pos++;
			}

			if (pos >= source.size())
			{
				return {};
			}

			const auto begin = pos;
			if (source[pos] == '"')
			{
				const auto end = source.find('"', pos + 1);
				pos = end == std::string_view::npos ? source.size() : end + 1;
				return source.substr(begin, pos - begin);
			}

			if (source[pos] == '{' || source[pos] == '}')
			{
				return source.substr(pos++, 1);
			}

			while (pos < source.size() && !std::isspace(static_cast<unsigned char>(source[pos])) && source[pos] != '"')
			{
				pos++;
			}

			return source.substr(begin, pos - begin);
		}

		std::string_view unquote(const std::string_view token)
		{
			return token.size() >= 2 && token.front() == '"' ? token.substr(1, token.size() - 2) : token;
		}

		std::string read_key(const std::string_view source, std::size_t& pos)
		{
			const auto key = next_token(source, pos);
			std::uint32_t id{};
			if (key.front() == '"' || std::from_chars(key.data(), key.data() + key.size(), id).ptr != key.data() + key.size())
			{
				return std::string(unquote(key));
			}

			if (id == 0)
			{
				return std::string(unquote(next_token(source, pos)));
			}

			return source_context().token_name(id);
		}

		std::uint32_t convert_key(const std::string_view source, std::size_t& pos)
		{
			const auto name = read_key(source, pos);
			return name.starts_with("_id_") ? 0 : target_context().token_id(name);
		}

		std::vector<std::unordered_map<std::string, std::string>> read_entities(const MapEnts* asset)
		{
			std::vector<std::unordered_map<std::string, std::string>> entities;
			const std::string_view source(asset->entityString, std::strlen(asset->entityString));
			std::size_t pos{};
			for (auto token = next_token(source, pos); !token.empty(); token = next_token(source, pos))
			{
				if (token != "{")
				{
					continue;
				}

				auto& entity = entities.emplace_back();
				while (true)
				{
					auto peek = pos;
					const auto next = next_token(source, peek);
					if (next.empty() || next == "}")
					{
						pos = peek;
						break;
					}

					const auto key = read_key(source, pos);
					entity[key] = unquote(next_token(source, pos));
				}
			}

			return entities;
		}

		void make_spawn_points(const MapEnts* asset, target::SpawnPointRecordList& converted, utils::memory::allocator& allocator)
		{
			std::vector<target::SpawnPointEntityRecord> spawns;
			const auto add_spawn = [&](const std::unordered_map<std::string, std::string>& entity, const char* classname)
			{
				auto& spawn = spawns.emplace_back();
				spawn.index = static_cast<unsigned short>(spawns.size() - 1); // TODO: stock index rule unknown
				spawn.name = static_cast<target::scr_string_t>(SL_GetString(classname));
				spawn.target = static_cast<target::scr_string_t>(SL_GetString(""));
				spawn.script_noteworthy = spawn.target;
				if (const auto origin = entity.find("origin"); origin != entity.end())
				{
					std::sscanf(origin->second.data(), "%f %f %f", &spawn.origin[0], &spawn.origin[1], &spawn.origin[2]);
				}
				if (const auto angles = entity.find("angles"); angles != entity.end())
				{
					std::sscanf(angles->second.data(), "%f %f %f", &spawn.angles[0], &spawn.angles[1], &spawn.angles[2]);
				}
			};

			for (const auto& entity : read_entities(asset))
			{
				const auto targetname = entity.find("targetname");
				if (targetname == entity.end())
				{
					continue;
				}

				if (targetname->second == "default_player_start")
				{
					for (const auto* classname : {"mp_tdm_spawn_allies_start", "mp_tdm_spawn_axis_start", "mp_dm_spawn_start"})
					{
						add_spawn(entity, classname);
					}
				}
				else if (targetname->second == "player_respawn_loc")
				{
					for (const auto* classname : {"mp_tdm_spawn", "mp_dm_spawn"})
					{
						add_spawn(entity, classname);
					}
				}
			}

			ZONETOOL_INFO("mapents \"%s\": made %zu spawn points from zombies player structs", asset->name, spawns.size());
			converted.spawnsCount = static_cast<unsigned short>(spawns.size());
			converted.spawns = allocator.allocate_array<target::SpawnPointEntityRecord>(spawns.size());
			std::copy(spawns.begin(), spawns.end(), converted.spawns);
		}

		std::string convert_entity_string(const MapEnts* asset)
		{
			std::string converted;
			converted.reserve(asset->numEntityChars);

			const std::string_view source(asset->entityString, std::strlen(asset->entityString));
			const auto classname_key = target_context().token_id("classname");
			auto dropped_keys = 0u;
			std::size_t pos{};
			while (true)
			{
				const auto token = next_token(source, pos);
				if (token.empty())
				{
					break;
				}

				if (token == "{")
				{
					converted.append("{");
					std::string_view classname;
					auto has_20186 = false;
					while (true)
					{
						auto peek = pos;
						const auto next = next_token(source, peek);
						if (next.empty() || next == "}")
						{
							pos = peek;
							break;
						}

						const auto key = convert_key(source, pos);
						const auto value = unquote(next_token(source, pos));
						if (!key)
						{
							dropped_keys++;
							continue;
						}

						classname = key == classname_key ? value : classname;
						has_20186 |= key == 20186;
						converted.append(std::format(" {} \"{}\"", key, value));
					}

					if (!has_20186 && (classname == "script_struct" || classname == "script_brushmodel" || classname == "script_model"))
					{
						converted.append(" 20186 \"default\""); // TODO: stock mp_rust only, requirement unknown
					}
					converted.append(" }\n");
				}
			}

			if (dropped_keys)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u keys have no IW8 id", asset->name, dropped_keys);
			}

			return converted;
		}

		template <typename T, typename S>
		T* convert_array(const S* source, const unsigned int count, utils::memory::allocator& allocator)
		{
			auto* converted = allocator.allocate_array<T>(count);
			for (auto i = 0u; i < count; i++)
			{
				converted[i] = std::bit_cast<T>(source[i]);
			}

			return converted;
		}

		void convert_splines(const SplineRecordList& source, target::SplineRecordList& converted, utils::memory::allocator& allocator)
		{
			converted.splineCount = source.splineCount;
			converted.splines = allocator.allocate_array<target::SplinePointRecordList>(source.splineCount);
			for (auto i = 0u; i < source.splineCount; i++)
			{
				const auto& spline = source.splines[i];
				auto& result = converted.splines[i];
				result.splinePointCount = spline.splinePointCount;
				result.splineLength = spline.splineLength;
				result.splinePoints = convert_array<target::SplinePointEntityRecord>(spline.splinePoints, spline.splinePointCount, allocator);
			}
		}

		target::SpatialPartition_Population_Tree* make_leaf_tree(utils::memory::allocator& allocator)
		{
			auto* tree = allocator.allocate<target::SpatialPartition_Population_Tree>();
			tree->partitionCount = 1;
			tree->partitions = allocator.allocate<target::SpatialPartition_Population_Partition>();
			tree->extents = allocator.allocate<target::ExtentBounds>();
			return tree;
		}

		target::SpatialPartition_Population* make_empty_population(utils::memory::allocator& allocator)
		{
			auto* population = allocator.allocate<target::SpatialPartition_Population>();
			population->tree = make_leaf_tree(allocator);
			population->partitionCount = 1;
			population->buckets = allocator.allocate<target::SpatialPartition_Population_Node>();
			population->buckets->spatialNodeNext = 0xFFFFFFFF;
			return population;
		}

		void add_edge(target::MapEdgeList* list, const std::array<float, 3>& start, const std::array<float, 3>& end,
			const float adjacent_normal, const std::uint16_t open_angle)
		{
			auto& edge = list->edges[list->numEdges];
			auto& metadata = list->edgeMetadata[list->numEdges];
			for (auto axis = 0; axis < 3; axis++)
			{
				edge[0].v[axis] = start[axis];
				edge[1].v[axis] = end[axis];
				list->mins[axis] = std::min({list->mins[axis], start[axis] - 1.0f, end[axis] - 1.0f});
				list->maxs[axis] = std::max({list->maxs[axis], start[axis] + 1.0f, end[axis] + 1.0f});
			}

			metadata.packedAdjacentNormal = adjacent_normal;
			metadata.packedOpenAngle = open_angle;
			metadata.flags = 1 << target::EDGE_QUERY_LADDER;
			list->numEdges++;
		}

		target::PMROctreeMetadata* make_root_octree(const target::MapEdgeList* list, const unsigned char bucket, utils::memory::allocator& allocator)
		{
			auto* octree = allocator.allocate<target::PMROctreeMetadata>();
			float center[3]{};
			auto half_width = 0.0f;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto min = list->mins[axis] + 1.0f;
				const auto max = list->maxs[axis] - 1.0f;
				center[axis] = (min + max) * 0.5f;
				half_width = std::max(half_width, (max - min) * 0.5f);
			}
			half_width += 1.0f;

			for (auto depth = 0; depth < 16; depth++)
			{
				const auto width = half_width / static_cast<float>(1 << depth);
				octree->nodeHalfWidth[depth] = {{width, width, width, 0.0f}};
				octree->nodeBoundingRadiusSq[depth] = 3.0f * width * width;
			}

			octree->minExtent = {{center[0] - half_width, center[1] - half_width, center[2] - half_width, -half_width}};
			octree->rootNode.flags = 6; // leaf with an edge table
			octree->rootNode.qEdgeTableIndex = 0;
			octree->rootNode.qEdgeTableCount = list->numEdges;
			octree->edgeBucket = bucket;
			octree->flags = 3;
			return octree;
		}

		target::MapEdgeList* make_ladder_edge_list(const char* name, const std::vector<havok::ladder>& ladders, utils::memory::allocator& allocator)
		{
			auto* list = allocator.allocate<target::MapEdgeList>();
			list->name = allocator.duplicate_string(name);
			for (auto axis = 0; axis < 3; axis++)
			{
				list->mins[axis] = std::numeric_limits<float>::max();
				list->maxs[axis] = std::numeric_limits<float>::lowest();
			}

			const auto edge_count = static_cast<unsigned int>(ladders.size() * 2);
			list->edges = allocator.allocate_array<target::float4[2]>(edge_count);
			list->edgeMetadata = allocator.allocate_array<target::FlaggedEdgeMetadata>(edge_count);
			const auto open_angle = half_float::half_to_float(14740);
			for (const auto& ladder : ladders)
			{
				const auto& bottom = ladder.bottom;
				const std::array<float, 3> top{bottom[0], bottom[1], bottom[2] + ladder.height};
				const auto yaw = std::atan2(ladder.facing[1], ladder.facing[0]);
				auto adjacent_normal = yaw - open_angle * 0.5f;
				if (adjacent_normal <= -std::numbers::pi_v<float>)
				{
					adjacent_normal += 2.0f * std::numbers::pi_v<float>;
				}
				add_edge(list, bottom, top, adjacent_normal, 14740);

				const auto half_width = ladder.width * 0.5f;
				const std::array<float, 2> across{-ladder.facing[1] * half_width, ladder.facing[0] * half_width};
				add_edge(list, {top[0] - across[0], top[1] - across[1], top[2]}, {top[0] + across[0], top[1] + across[1], top[2]}, 0.0f, 15944);
			}

			list->valid = true;
			list->staticQueryTypes = 1 << target::EDGE_QUERY_LADDER;
			list->numStaticEdges = list->numEdges;

			list->numEdgeOctrees = 1;
			list->edgeOctrees = make_root_octree(list, target::EDGE_QUERY_LADDER, allocator);
			list->numEdgeIndices = list->numEdges;
			list->edgeIndices = allocator.allocate_array<unsigned int>(list->numEdges);
			for (auto i = 0u; i < list->numEdges; i++)
			{
				list->edgeIndices[i] = i;
			}

			list->numEdgeAdjacencyMetadata = list->numEdges;
			list->edgeAdjacencyMetadata = allocator.allocate_array<target::EdgeAdjacencyMetadata>(list->numEdges);
			return list;
		}

		void convert_ladders(const MapEnts* asset, target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			const auto* clip = static_cast<const clipMap_t*>(db_find_asset(ASSET_TYPE_CLIPMAP, asset->name));
			if (!clip)
			{
				return;
			}

			const auto ladders = havok::find_ladders(std::string_view(clip->havokWorldShapeData, clip->havokWorldShapeDataSize));
			if (ladders.empty())
			{
				return;
			}

			auto* list = make_ladder_edge_list(std::format("{}_edgelist", filesystem::get_fastfile()).data(), ladders, allocator);
			converted->edgeListUsedQueryTypes = list->staticQueryTypes;
			converted->numEdgeLists = 1;
			converted->edgeLists = allocator.allocate_array<target::MapEdgeList*>(1);
			converted->edgeLists[0] = list;

			converted->edgeListSpatialTree = allocator.allocate<target::SpatialPartition_Tree>();
			converted->edgeListSpatialTree->alwaysListLength = 1;
			converted->edgeListSpatialTree->alwaysList = allocator.allocate_array<unsigned int>(1);

			target::map_edge_list::dump(list);
			ZONETOOL_INFO("Converted %zu ladders of \"%s\" to edges", ladders.size(), asset->name);
		}

		void convert_submodels(const MapEnts* asset, target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			converted->numSubModels = asset->numSubModels;
			converted->cmodels = allocator.allocate_array<target::cmodel_t>(asset->numSubModels);
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				const auto& model = asset->cmodels[i];
				auto& cmodel = converted->cmodels[i];
				cmodel.bounds = std::bit_cast<target::Bounds>(model.bounds);
				cmodel.radius = model.radius;
				cmodel.physicsShapeOverrideIdx = 0xFFFF; // map_ents shapes are not converted
				cmodel.navObstacleIdx = model.navObstacleIdx;
				cmodel.edgeFirstIndex = model.edgeFirstIndex;
			}

			const auto shape_data = havok::convert_ents_shape_list(std::string_view(asset->havokEntsShapeData, asset->havokEntsShapeDataSize));
			converted->havokEntsShapeDataSize = static_cast<unsigned int>(shape_data.size());
			converted->havokEntsShapeData = allocator.allocate_array<char>(shape_data.size());
			std::memcpy(converted->havokEntsShapeData, shape_data.data(), shape_data.size());
		}
	}

	void convert_triggers(const MapTriggers& source, target::MapTriggers& converted, utils::memory::allocator& allocator)
	{
		converted.count = source.count;
		converted.models = allocator.allocate_array<target::TriggerModel>(source.count);
		for (auto i = 0u; i < source.count; i++)
		{
			const auto& model = source.models[i];
			auto& result = converted.models[i];
			result.contents = model.contents;
			result.hullCount = model.hullCount;
			result.firstHull = model.firstHull;
			result.windingCount = model.windingCount;
			result.firstWinding = model.firstWinding;
			result.flags = model.flags;
			result.physicsShapeOverrideIdx = 0xFFFF; // map_ents shapes are not converted
		}

		converted.hullCount = source.hullCount;
		converted.hulls = convert_array<target::TriggerHull>(source.hulls, source.hullCount, allocator);
		converted.slabCount = source.slabCount;
		converted.slabs = convert_array<target::TriggerSlab>(source.slabs, source.slabCount, allocator);
		converted.windingCount = source.windingCount;
		converted.windings = convert_array<target::TriggerWinding>(source.windings, source.windingCount, allocator);
		converted.windingPointCount = source.windingPointCount;
		converted.windingPoints = convert_array<target::TriggerWindingPoint>(source.windingPoints, source.windingPointCount, allocator);
	}

	void convert_scriptables(const ScriptableMapEnts& source, target::ScriptableMapEnts& converted, utils::memory::allocator& allocator)
	{
		std::vector<std::pair<const ScriptableInstance*, const ScriptableDef*>> server_instances;
		std::vector<std::pair<const ScriptableInstance*, const ScriptableDef*>> client_instances;
		for (auto i = 0u; i < source.totalInstanceCount; i++)
		{
			const auto& instance = source.instances[i];
			if (const auto def = scriptable::dump_used(instance.contextHeader.context.def))
			{
				auto& instances = (def->flags & SCRIPTABLE_DEFFLAG_SERVER_INSTANCE) ? server_instances : client_instances;
				instances.emplace_back(&instance, def);
			}
		}

		converted.serverInstanceCount = static_cast<unsigned int>(server_instances.size());
		converted.mapInstanceCount = static_cast<unsigned int>(server_instances.size() + client_instances.size());
		converted.mapInstances = allocator.allocate_array<target::ScriptableInstance>(converted.mapInstanceCount);

		auto index = 0u;
		for (const auto* instances : {&server_instances, &client_instances})
		{
			for (const auto& [instance, def] : *instances)
			{
				const auto& context = instance->contextHeader.context;
				auto& result = converted.mapInstances[index++];
				const auto initial_def = allocator.allocate<target::ScriptableDef>();
				initial_def->name = def->name;
				result.initialDef = initial_def;
				std::memcpy(result.initialOrigin, context.initialOrigin, sizeof(result.initialOrigin));
				std::memcpy(result.initialAngles, context.initialAngles, sizeof(result.initialAngles));
				result.classname = static_cast<target::scr_string_t>(SL_GetString(std::format("scriptable_{}", def->name).data()));
				result.targetname = static_cast<target::scr_string_t>(instance->targetname);
			}
		}

		for (const auto& [instance, def] : server_instances)
		{
			converted.serverFixedPartCount += def->partCount; // TODO: inferred from stock count
		}
	}

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;

		auto* converted = allocator.allocate<target::MapEnts>();
		converted->name = asset->name;
		auto entity_string = convert_entity_string(asset);
		converted->entityString = entity_string.data();
		converted->numEntityChars = static_cast<int>(entity_string.size() + 1);

		convert_triggers(asset->trigger, converted->trigger, allocator); // TODO: client triggers are not converted

		converted->clientTriggerBlend.numClientTriggerBlendNodes = asset->clientTriggerBlend.numClientTriggerBlendNodes;
		converted->clientTriggerBlend.blendNodes = convert_array<target::ClientTriggerBlendNode>(asset->clientTriggerBlend.blendNodes,
			asset->clientTriggerBlend.numClientTriggerBlendNodes, allocator);

		if (asset->spawnList.spawnsCount)
		{
			converted->spawnList.spawnsCount = asset->spawnList.spawnsCount;
			converted->spawnList.spawns = convert_array<target::SpawnPointEntityRecord>(asset->spawnList.spawns, asset->spawnList.spawnsCount, allocator);
		}
		else
		{
			make_spawn_points(asset, converted->spawnList, allocator);
		}
		convert_splines(asset->splineList, converted->splineList, allocator);

		convert_submodels(asset, converted, allocator); // TODO: havok ents shapes and dynents are not converted
		convert_ladders(asset, converted, allocator);

		for (auto& population : converted->dynEntSpatialPopulation)
		{
			population = make_empty_population(allocator);
		}

		convert_scriptables(asset->scriptableMapEnts, converted->scriptableMapEnts, allocator);
		converted->scriptableMapEnts.spatialPopulationTree = make_leaf_tree(allocator); // TODO: scriptable spatial population

		const auto map = filesystem::get_fastfile();
		const auto map_script = decompile_map_script(map);
		const auto script_effects = convert_oneshot_effects(map, map_script, converted->clientSideEffects, allocator); // TODO: sounds and reactive ents are not baked
		converted->createFxEffectTotal = converted->clientSideEffects.oneshotEffectCount;

		target::map_ents::dump(converted);
		add_csv_row("map_ents", asset->name);

		dump_map_script(map, map_script, script_effects);
	}
}
