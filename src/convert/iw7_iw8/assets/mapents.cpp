#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "havok.hpp"
#include "material.hpp"
#include "rawfile.hpp"
#include "scriptable.hpp"
#include "vfx.hpp"

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

		std::string make_map_script(const std::string& map, const std::string& source) // TODO: convert the precache, art and fx scripts and the render dvars
		{
			const std::regex map_script_call(std::format(R"re(\s*scripts\\mp\\maps\\{}\\.*::main\(\s*\);\s*)re", map));
			static const std::regex dvar_line(R"re(\s*setdvar\(.*\);\s*)re");

			std::string script;
			for (auto line : utils::string::split(source, '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				if (!std::regex_match(line, map_script_call) && !std::regex_match(line, dvar_line))
				{
					script += line + "\n";
				}
			}

			return script;
		}

		std::string get_map_script(const std::string& map)
		{
			const auto* asset = static_cast<const ScriptFile*>(db_find_asset(ASSET_TYPE_SCRIPTFILE, std::format("scripts/mp/maps/{}/{}", map, map).data()));
			if (!asset || !asset->buffer || !asset->bytecode)
			{
				return make_default_map_script(map);
			}

			try
			{
				return make_map_script(map, decompile(asset));
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("scriptfile \"%s\" does not decompile: %s", asset->name, e.what());
				return make_default_map_script(map);
			}
		}

		void dump_map_script(const std::string& map)
		{
			const auto name = std::format("scripts/mp/maps/{}/{}.gsc", map, map);
			const auto source = get_map_script(map);
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
		}

		std::string get_rawfile_text(const std::string& name)
		{
			const auto* asset = static_cast<const RawFile*>(db_find_asset(ASSET_TYPE_RAWFILE, name.data()));
			return asset && asset->len ? rawfile::read_text(asset) : std::string{};
		}

		std::unordered_map<std::string, std::string> read_effect_paths(const std::string& map)
		{
			static const std::regex load_fx(R"re(level\._effect\[\s*"([^"]+)"\s*\]\s*=\s*loadfx\(\s*"([^"]+?)(?:\.vfx)?"\s*\))re");

			std::unordered_map<std::string, std::string> paths;
			const auto source = get_rawfile_text(std::format("scripts/mp/maps/{}/{}_fx.gsc", map, map));
			for (auto it = std::sregex_iterator(source.begin(), source.end(), load_fx); it != std::sregex_iterator(); ++it)
			{
				paths[(*it)[1].str()] = (*it)[2].str();
			}

			return paths;
		}

		void read_vector(const std::string& text, target::vec3_t& vector)
		{
			std::sscanf(text.data(), "%f , %f , %f", &vector[0], &vector[1], &vector[2]);
		}

		void convert_oneshot_effects(const std::string& map, target::ClientSideEffects& effects, utils::memory::allocator& allocator)
		{
			static const std::regex oneshot(R"re(createOneshotEffect\(\s*"([^"]+)"\s*\);\s*ent set_origin_and_angles\(\s*\(([^)]*)\)\s*,\s*\(([^)]*)\)\s*\);(?:\s*ent\.v\[\s*"fxid"\s*\]\s*=\s*"[^"]*"\s*;)?(?:\s*ent\.v\[\s*"delay"\s*\]\s*=\s*([-\d.]+)\s*;)?)re");

			const auto paths = read_effect_paths(map);
			const auto source = get_rawfile_text(std::format("scripts/mp/maps/{}/gen/{}_fx.gsc", map, map));

			std::vector<target::ClientOneshotEffectDef> converted;
			for (auto it = std::sregex_iterator(source.begin(), source.end(), oneshot); it != std::sregex_iterator(); ++it)
			{
				const auto& match = *it;
				const auto path = paths.find(match[1].str());
				auto* vfx = path != paths.end() ? static_cast<ParticleSystemDef*>(db_find_asset(ASSET_TYPE_VFX, path->second.data())) : nullptr;
				if (!vfx)
				{
					ZONETOOL_WARNING("createfx oneshot \"%s\" has no vfx", match[1].str().data());
					continue;
				}

				vfx::dump(vfx);

				auto& effect = converted.emplace_back();
				read_vector(match[2].str(), effect.origin);
				read_vector(match[3].str(), effect.angles);
				effect.radius = 0.0f; // TODO: stock radius rule unknown (mp_rust uses 0 to 900000 per effect)
				effect.delayMsec = match[4].matched ? static_cast<int>(std::stof(match[4].str()) * 1000.0f) : 0;
				effect.effect.particleSystemDef = allocator.allocate<target::ParticleSystemDef>();
				const_cast<target::ParticleSystemDef*>(effect.effect.particleSystemDef)->name = allocator.duplicate_string(vfx->name);
			}

			effects.oneshotEffectCount = static_cast<unsigned int>(converted.size());
			effects.oneshotEffects = allocator.allocate_array<target::ClientOneshotEffectDef>(converted.size());
			std::copy(converted.begin(), converted.end(), effects.oneshotEffects);
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

		std::uint32_t convert_key(const std::string_view source, std::size_t& pos)
		{
			const auto key = next_token(source, pos);
			std::uint32_t id{};
			if (key.front() == '"' || std::from_chars(key.data(), key.data() + key.size(), id).ptr != key.data() + key.size())
			{
				return target_context().token_id(std::string(unquote(key)));
			}

			if (id == 0)
			{
				return target_context().token_id(std::string(unquote(next_token(source, pos))));
			}

			const auto name = source_context().token_name(id);
			return name.starts_with("_id_") ? 0 : target_context().token_id(name);
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

		void convert_submodels(const MapEnts* asset, target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			converted->numSubModels = 0; // map_ents shapes are not converted
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

		converted->spawnList.spawnsCount = asset->spawnList.spawnsCount;
		converted->spawnList.spawns = convert_array<target::SpawnPointEntityRecord>(asset->spawnList.spawns, asset->spawnList.spawnsCount, allocator);
		convert_splines(asset->splineList, converted->splineList, allocator);

		convert_submodels(asset, converted, allocator); // TODO: havok ents shapes and dynents are not converted

		for (auto& population : converted->dynEntSpatialPopulation)
		{
			population = make_empty_population(allocator);
		}

		convert_scriptables(asset->scriptableMapEnts, converted->scriptableMapEnts, allocator);
		converted->scriptableMapEnts.spatialPopulationTree = make_leaf_tree(allocator); // TODO: scriptable spatial population

		convert_oneshot_effects(filesystem::get_fastfile(), converted->clientSideEffects, allocator); // TODO: sounds and reactive ents are not baked
		converted->createFxEffectTotal = converted->clientSideEffects.oneshotEffectCount;

		target::map_ents::dump(converted);
		add_csv_row("map_ents", asset->name);

		dump_map_script(filesystem::get_fastfile());
	}
}
