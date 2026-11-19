#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "havok.hpp"

#include <iw7/assets/mapents.hpp>
#include <iw7/assets/scriptfile.hpp>

#include <utils/compression.hpp>

#include <xsk/gsc/engine/iw7.hpp>
#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::iw8::converter::iw7::mapents
{
	namespace
	{
		namespace target = zonetool::iw7;

		const xsk::gsc::iw8::context& source_tokens()
		{
			static const xsk::gsc::iw8::context context{xsk::gsc::instance::server};
			return context;
		}

		xsk::gsc::iw7::context& target_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		std::string make_map_script(const std::string& map)
		{
			return std::format(R"(main()
{{
    scripts\mp\load::main();
    scripts\mp\compass::setupminimap( "compass_map_{}" );
    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";
}}
)", map);
		}

		void dump_map_script(const std::string& map)
		{
			const auto name = std::format("scripts/mp/maps/{}/{}", map, map);
			const auto source = make_map_script(map);
			std::vector<std::uint8_t> data(source.begin(), source.end());

			auto& context = target_context();
			const auto assembly = context.compiler().compile(name + ".gsc", data);
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
		}

		std::string convert_key(const std::string_view key)
		{
			std::uint32_t id{};
			if (std::from_chars(key.data(), key.data() + key.size(), id).ptr != key.data() + key.size())
			{
				return std::string(key);
			}

			const auto name = source_tokens().token_name(id);
			if (const auto target_id = target_context().token_id(name); target_id && !name.starts_with("_id_"))
			{
				return std::to_string(target_id);
			}

			return std::format("\"{}\"", name);
		}

		std::string convert_entity_string(const MapEnts* asset)
		{
			std::string converted;
			converted.reserve(asset->numEntityChars);

			for (const auto& line : utils::string::split(asset->entityString, '\n'))
			{
				const auto separator = line.find(' ');
				if (separator == std::string::npos)
				{
					converted.append(line);
				}
				else
				{
					converted.append(convert_key(std::string_view(line).substr(0, separator)));
					converted.append(line, separator);
				}

				converted.push_back('\n');
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
				cmodel.physicsShapeOverrideIdx = model.physicsShapeOverrideIdx;
				cmodel.navObstacleIdx = model.navObstacleIdx;
				cmodel.edgeFirstIndex = model.edgeFirstIndex;
			}
		}

		void reserve_scriptable_dynents(target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			converted->dynEntCount[0] = 64; // TODO: dynents are not converted, reserve as v1 h1_iw7 does
			converted->dynEntCountTotal = converted->dynEntCount[0];
			converted->dynEntDefList[0] = allocator.allocate_array<target::DynEntityDef>(converted->dynEntCount[0]);
			converted->dynEntPoseList[0][0] = allocator.allocate_array<target::DynEntityPose>(converted->dynEntCount[0]);
			converted->dynEntPoseList[1][0] = allocator.allocate_array<target::DynEntityPose>(converted->dynEntCount[0]);
			converted->dynEntGlobalIdList[0] = allocator.allocate_array<target::DynEntityGlobalId>(converted->dynEntCountTotal);
			converted->dynEntGlobalIdList[1] = allocator.allocate_array<target::DynEntityGlobalId>(converted->dynEntCountTotal);
			for (auto i = 0u; i < converted->dynEntCount[0]; i++)
			{
				auto& dynent = converted->dynEntDefList[0][i];
				dynent.type = target::DYNENT_TYPE_SCRIPTABLEINST;
				dynent.instanceIndex = 500;
				dynent.spawnEnabled = true;
			}

			auto& scriptables = converted->scriptableMapEnts;
			scriptables.totalInstanceCount = 500; // TODO: scriptables are not converted, reserve as v1 h1_iw7 does
			scriptables.runtimeInstanceCount = scriptables.totalInstanceCount;
			scriptables.reservedInstanceCount = scriptables.totalInstanceCount;
			scriptables.instances = allocator.allocate_array<target::ScriptableInstance>(scriptables.totalInstanceCount);
			for (auto& reserved : scriptables.reservedDynents)
			{
				reserved.numReservedDynents = converted->dynEntCount[0];
				reserved.reservedDynents = allocator.allocate_array<target::ScriptableReservedDynent>(reserved.numReservedDynents);
			}
		}
	}

	void convert_triggers(const MapTriggers& source, target::MapTriggers& converted, utils::memory::allocator& allocator)
	{
		converted.count = source.count;
		converted.models = convert_array<target::TriggerModel>(source.models, source.count, allocator);
		for (auto i = 0u; i < source.count; i++)
		{
			converted.models[i].physicsAsset = nullptr; // TODO: IW8 physics assets are not converted
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

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;

		auto* converted = allocator.allocate<target::MapEnts>();
		converted->name = asset->name;
		auto entity_string = convert_entity_string(asset);
		converted->entityString = entity_string.data();
		converted->numEntityChars = static_cast<int>(entity_string.size() + 1);

		convert_triggers(asset->trigger, converted->trigger, allocator);

		converted->spawnList.spawnsCount = asset->spawnList.spawnsCount;
		converted->spawnList.spawns = convert_array<target::SpawnPointEntityRecord>(asset->spawnList.spawns, asset->spawnList.spawnsCount, allocator);

		convert_submodels(asset, converted, allocator);
		reserve_scriptable_dynents(converted, allocator);

		std::string ents_shapes;
		if (asset->havokEntsShapeData && asset->havokEntsShapeDataSize)
		{
			try
			{
				ents_shapes = havok::convert_ents_shapes(std::string_view(asset->havokEntsShapeData, asset->havokEntsShapeDataSize));
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("%s: %s", asset->name, e.what());
			}
		}

		converted->havokEntsShapeDataSize = static_cast<unsigned int>(ents_shapes.size());
		converted->havokEntsShapeData = ents_shapes.data();

		target::map_ents::dump(converted);
		add_csv_row("map_ents", asset->name);
		dump_map_script(filesystem::get_fastfile());
	}
}
