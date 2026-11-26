#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "havok.hpp"
#include "scriptable.hpp"

#include <iw8/assets/mapents.hpp>

#include <xsk/gsc/engine/iw8.hpp>
#include <xsk/gsc/engine/s4.hpp>

namespace zonetool::s4::converter::iw8::mapents
{
	namespace
	{
		namespace target = zonetool::iw8;

		xsk::gsc::s4::context& source_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::s4::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		xsk::gsc::iw8::context& target_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw8::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
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

		const std::unordered_map<std::uint32_t, std::string_view> unnamed_source_tokens = // gsc-tool has no S4 name
		{
			{93, "animscript"},
			{371, "dmg"},
			{571, "intensity"},
			{758, "nophysics"},
			{761, "northyaw"},
			{857, "radius"},
			{919, "script_exploder"},
			{920, "script_linkname"},
			{921, "script_linkto"},
			{923, "script_noteworthy"},
			{1002, "spawnflags"},
			{1045, "suncolor"},
			{1046, "sundirection"},
			{1047, "sunlight"},
			{1153, "targetname"},
			{2841, "__smuid"},
			{2978, "_color"},
			{3232, "_primarylightcount"},
			{24120, "footprint_mask_group"},
			{45130, "physasset"},
			{52421, "script_bombmode_original"},
			{52585, "script_gameobjectname"},
			{52633, "script_label"},
			{56795, "skyrotation"},
		};

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

			if (const auto token = unnamed_source_tokens.find(id); token != unnamed_source_tokens.end())
			{
				return std::string(token->second);
			}

			return source_context().token_name(id);
		}

		std::string convert_entity_string(const MapEnts* asset)
		{
			std::string converted;
			converted.reserve(asset->numEntityChars);

			const std::string_view source(asset->entityString, std::strlen(asset->entityString));
			auto dropped_keys = 0u;
			std::size_t pos{};
			for (auto token = next_token(source, pos); !token.empty(); token = next_token(source, pos))
			{
				if (token != "{")
				{
					continue;
				}

				converted.append("{");
				while (true)
				{
					auto peek = pos;
					const auto next = next_token(source, peek);
					if (next.empty() || next == "}")
					{
						pos = peek;
						break;
					}

					const auto name = read_key(source, pos);
					const auto value = unquote(next_token(source, pos));
					const auto key = name.starts_with("_id_") ? 0 : target_context().token_id(name);
					if (!key)
					{
						dropped_keys++;
						continue;
					}

					converted.append(std::format(" {} \"{}\"", key, value));
				}

				converted.append(" }\n");
			}

			if (dropped_keys)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u keys have no IW8 id", asset->name, dropped_keys);
			}

			return converted;
		}

		template <typename Target, typename Source>
		void copy_identical(Target& target_field, const Source& source_field)
		{
			static_assert(sizeof(Target) == sizeof(Source));
			std::memcpy(&target_field, &source_field, sizeof(Target));
		}

		void convert_client_triggers(const ClientTriggers& source, target::ClientTriggers& converted, utils::memory::allocator& allocator)
		{
			static_assert(offsetof(ClientTriggers, unknown0) == sizeof(target::ClientTriggers));
			std::memcpy(&converted, &source, sizeof(converted));
			converted.spatialTree = convert_spatial_tree(source.spatialTree, allocator);
		}

		template <typename Target, typename Source>
		Target* copy_without_sound(const Source* source, const unsigned int count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(Target) == sizeof(Source));
			if (!source || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<Target>(count);
			std::memcpy(result, source, sizeof(Target) * count);
			for (auto i = 0u; i < count; i++)
			{
				result[i].effectSound.name = nullptr;
				result[i].aliasList = nullptr;
			}

			return result;
		}

		void convert_client_side_effects(const MapEnts* asset, target::ClientSideEffects& converted, utils::memory::allocator& allocator)
		{
			const auto& source = asset->clientSideEffects;
			converted.oneshotEffectCount = source.oneshotEffectCount;
			converted.oneshotEffects = copy_without_sound<target::ClientOneshotEffectDef>(source.oneshotEffects, source.oneshotEffectCount, allocator);
			converted.oneshotEffectsTree = convert_spatial_tree(source.oneshotEffectsTree, allocator);
			converted.exploderCount = source.exploderCount;
			converted.exploders = copy_without_sound<target::ClientExploderDef>(source.exploders, source.exploderCount, allocator);
			converted.reactiveEntCount = source.reactiveEntCount;
			converted.reactiveEnts = copy_without_sound<target::ClientReactiveEntDef>(source.reactiveEnts, source.reactiveEntCount, allocator);
			converted.reactiveSpatialTree = convert_spatial_tree(source.reactiveSpatialTree, allocator);

			if (source.oneshotSoundCount || source.intervalSoundCount || source.loopSoundCount)
			{
				ZONETOOL_WARNING("mapents \"%s\" loses %u oneshot, %u interval and %u loop sounds, S4 has only the alias hash", asset->name,
					source.oneshotSoundCount, source.intervalSoundCount, source.loopSoundCount);
			}

			if (source.unknown0Count || source.unknown1Count || source.unknown2Count || source.unknown3Count)
			{
				ZONETOOL_WARNING("mapents \"%s\": client side effect lists %u, %u, %u and %u have no IW8 type", asset->name,
					source.unknown0Count, source.unknown1Count, source.unknown2Count, source.unknown3Count);
			}
		}

		const target::ScriptableDef* find_converted_def(const ScriptableDef* def, std::unordered_map<std::string, const target::ScriptableDef*>& defs,
			utils::memory::allocator& allocator)
		{
			const auto [entry, inserted] = defs.try_emplace(def->name, nullptr);
			if (inserted)
			{
				const auto loaded = static_cast<const ScriptableDef*>(db_find_asset(ASSET_TYPE_SCRIPTABLE, def->name));
				entry->second = loaded ? scriptable::convert(loaded, allocator) : nullptr;
			}

			return entry->second;
		}

		void convert_scriptable_instance(const ScriptableMapEnts& source, const ScriptableInstance& instance, target::ScriptableInstance& converted,
			utils::memory::allocator& allocator)
		{
			const auto def = allocator.allocate<target::ScriptableDef>();
			def->name = instance.initialDef->name;
			converted.initialDef = def;

			std::memcpy(converted.initialOrigin, instance.initialOrigin, sizeof(converted.initialOrigin));
			std::memcpy(converted.initialAngles, instance.initialAngles, sizeof(converted.initialAngles));
			converted.classname = static_cast<target::scr_string_t>(instance.classname);
			converted.target = static_cast<target::scr_string_t>(instance.target);
			converted.targetname = static_cast<target::scr_string_t>(instance.targetname);
			converted.script_linkname = static_cast<target::scr_string_t>(instance.script_linkname);
			converted.script_linkto = static_cast<target::scr_string_t>(instance.script_linkto);
			converted.script_noteworthy = static_cast<target::scr_string_t>(instance.script_noteworthy);

			if (source.unknown2 && instance.lightIndex < source.unknown2Count)
			{
				converted.lightIndices[0] = source.unknown2[instance.lightIndex].primaryLightIndex;
			}
		}

		void convert_scriptable_map_ents(const MapEnts* asset, target::ScriptableMapEnts& converted, utils::memory::allocator& allocator)
		{
			const auto& source = asset->scriptableMapEnts;
			converted.mapInstanceCount = source.mapInstances ? source.mapInstanceCount : 0;
			converted.reservedInstanceCount = source.reservedInstanceCount;
			converted.lootInstanceCount = source.lootInstanceCount;
			converted.mapInstances = allocator.allocate_array<target::ScriptableInstance>(converted.mapInstanceCount);

			std::unordered_map<std::string, const target::ScriptableDef*> defs;
			auto physics_instances = 0u;
			auto missing_defs = 0u;
			for (auto i = 0u; i < converted.mapInstanceCount; i++)
			{
				const auto& instance = source.mapInstances[i];
				if (!instance.initialDef)
				{
					continue;
				}

				convert_scriptable_instance(source, instance, converted.mapInstances[i], allocator);

				const auto def = find_converted_def(instance.initialDef, defs, allocator);
				if (!def)
				{
					missing_defs++;
					continue;
				}

				converted.serverInstanceCount += (def->flags & 0x2) != 0;
				converted.serverFixedPartCount += def->serverInstancedPartCount;
				converted.serverUsablePartCount += def->usablePartCount;
				physics_instances += (def->flags & 0x8000) != 0;
			}

			converted.physicsLimits.clientStandaloneMainRigidBodyCount = physics_instances;
			converted.physicsLimits.clientStandaloneDetailedRigidBodyCount = physics_instances;
			converted.physicsLimits.clientStandaloneMotionCount = physics_instances;
			converted.physicsLimits.serverStandaloneMainRigidBodyCount = physics_instances;
			converted.physicsLimits.serverStandaloneDetailedRigidBodyCount = physics_instances;
			converted.physicsLimits.serverStandaloneMotionCount = physics_instances;

			copy_identical(converted.sunClientDatas, source.unknown1);
			copy_identical(converted.reservedDynents, source.reservedDynents);
			copy_identical(converted.spatialPopulationTree, source.spatialPopulationTree);

			converted.ffMemCost = sizeof(target::ScriptableMapEnts);
			for (const auto& dynents : converted.reservedDynents)
			{
				converted.ffMemCost += dynents.numReservedDynents * sizeof(target::ScriptableReservedDynent);
			}

			if (missing_defs)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u scriptable instances have a def that no loaded zone holds, their server counts are lost", asset->name, missing_defs);
			}

			if (source.lootInstanceCount)
			{
				ZONETOOL_WARNING("mapents \"%s\": the server counts do not include %u loot instances", asset->name, source.lootInstanceCount);
			}
		}

		void angles_to_quat(const float* angles, float* quat)
		{
			const auto half_radians = [&](const int axis)
			{
				return angles[axis] * std::numbers::pi_v<float> / 360.0f;
			};

			const auto sp = std::sin(half_radians(0));
			const auto cp = std::cos(half_radians(0));
			const auto sy = std::sin(half_radians(1));
			const auto cy = std::cos(half_radians(1));
			const auto sr = std::sin(half_radians(2));
			const auto cr = std::cos(half_radians(2));

			quat[0] = sr * cp * cy - cr * sp * sy;
			quat[1] = cr * sp * cy + sr * cp * sy;
			quat[2] = cr * cp * sy - sr * sp * cy;
			quat[3] = cr * cp * cy + sr * sp * sy;
		}

		template <typename Tree, typename Visit>
		unsigned int descend_population_tree(const Tree& tree, const float* origin, const Visit& visit)
		{
			auto node = 0u;
			while (node < tree.partitionCount)
			{
				visit(node);
				const auto& partition = tree.partitions[node];
				if (partition.axis == SPATIAL_PARTITION_POPULATION_LEAF)
				{
					break;
				}

				node = origin[partition.axis] < partition.dist ? node + 1 : partition.rightChild;
			}

			return node;
		}

		std::unordered_set<unsigned int> find_instances_with_dynent()
		{
			std::unordered_set<unsigned int> instances;
			db_enum_assets(ASSET_TYPE_DYN_ENTITY_LIST, [&](void* header)
			{
				const auto list = static_cast<const DynEntityList*>(header);
				for (auto i = 0; i < 2; i++)
				{
					for (auto index = 0u; list->dynEntDefList[i] && index < list->dynEntCount[i]; index++)
					{
						const auto& dynent = list->dynEntDefList[i][index];
						if (dynent.type == DYNENT_TYPE_SCRIPTABLEINST || dynent.type == DYNENT_TYPE_SCRIPTABLEPHYSICS)
						{
							instances.insert(dynent.scriptableMapIndex);
						}
					}
				}
			});

			return instances;
		}

		void add_scriptable_dynents(const MapEnts* asset, target::MapEnts& ents, utils::memory::allocator& allocator)
		{
			const auto dynents = make_scriptable_dynents(asset);
			if (dynents.empty())
			{
				return;
			}

			const auto count = static_cast<unsigned int>(dynents.size());
			ents.dynEntCount[0] += count;
			ents.dynEntCountTotal += count;
			for (const auto& dynent : dynents)
			{
				ents.dynEntMaxPosePartCount = std::max(ents.dynEntMaxPosePartCount, dynent.scriptableMaxPartCount);
				ents.dynEntsWithExtraPosePartsCount += dynent.scriptableMaxPartCount > 1;
			}

			const auto& source_tree = *asset->dynEntSpatialPopulation[1]->tree;
			const auto tree = allocator.allocate<target::SpatialPartition_Population_Tree>();
			copy_identical(*tree, source_tree);
			tree->extents = allocator.allocate_array<target::ExtentBounds>(tree->partitionCount);
			static_assert(sizeof(target::ExtentBounds) == sizeof(ExtentBounds));
			std::memcpy(tree->extents, source_tree.extents, sizeof(target::ExtentBounds) * tree->partitionCount);

			const auto population = allocator.allocate<target::SpatialPartition_Population>();
			copy_identical(*population, *asset->dynEntSpatialPopulation[1]);
			population->tree = tree;
			ents.dynEntSpatialPopulation[1] = population;

			for (const auto& dynent : dynents)
			{
				const auto origin = dynent.initialPose.origin;
				descend_population_tree(*tree, origin, [&](const unsigned int node)
				{
					auto& extents = tree->extents[node];
					for (auto axis = 0; axis < 3; axis++)
					{
						extents.mins[axis] = std::min(extents.mins[axis], origin[axis]);
						extents.maxs[axis] = std::max(extents.maxs[axis], origin[axis]);
					}
				});
			}
		}
	}

	std::vector<target::DynEntityDef> make_scriptable_dynents(const MapEnts* asset)
	{
		std::vector<target::DynEntityDef> dynents;
		const auto& source = asset->scriptableMapEnts;
		if (!source.mapInstances)
		{
			return dynents;
		}

		const auto instances_with_dynent = find_instances_with_dynent();
		std::unordered_map<std::string, int> def_flags;
		for (auto i = 0u; i < source.mapInstanceCount; i++)
		{
			const auto& instance = source.mapInstances[i];
			if (!instance.initialDef || instances_with_dynent.contains(i))
			{
				continue;
			}

			const auto def = static_cast<const ScriptableDef*>(db_find_asset(ASSET_TYPE_SCRIPTABLE, instance.initialDef->name));
			if (!def)
			{
				continue;
			}

			const auto [flags, inserted] = def_flags.try_emplace(def->name, 0);
			if (inserted)
			{
				flags->second = scriptable::converted_flags(def);
			}

			if (!(flags->second & 0x2) || !(flags->second & target::SCRIPTABLE_DEF_FLAG_NO_ENTITY))
			{
				continue;
			}

			auto& dynent = dynents.emplace_back();
			angles_to_quat(instance.initialAngles, dynent.initialPose.quat);
			std::memcpy(dynent.initialPose.origin, instance.initialOrigin, sizeof(dynent.initialPose.origin));
			dynent.type = target::DYNENT_TYPE_SCRIPTABLEINST;
			dynent.priority = 3;
			dynent.scriptableMapIndex = i;
			dynent.scriptableMaxPartCount = static_cast<char>(def->maxNumDynEntPartsBase);
			dynent.collisionFlags = 1;
		}

		const auto population = asset->dynEntSpatialPopulation[1];
		if (!dynents.empty() && (!population || !population->tree))
		{
			ZONETOOL_WARNING("mapents \"%s\" has no dynent population tree, %u scriptable instances get no dynent", asset->name,
				static_cast<unsigned int>(dynents.size()));
			return {};
		}

		for (auto& dynent : dynents)
		{
			dynent.spatialPartitionIndex = descend_population_tree(*population->tree, dynent.initialPose.origin, [](unsigned int) {});
		}

		return dynents;
	}

	void dump(MapEnts* asset)
	{
		static_assert(offsetof(target::MapEnts, scriptableMapEnts) - offsetof(target::MapEnts, spawnList) == offsetof(MapEnts, scriptableMapEnts) - offsetof(MapEnts, spawnList));
		static_assert(offsetof(target::MapEnts, audioPASpeakerCount) - offsetof(target::MapEnts, createFxAssetData) == offsetof(MapEnts, audioPASpeakerCount) - offsetof(MapEnts, createFxAssetData));
		static_assert(offsetof(target::MapEnts, numCollmaps) - offsetof(target::MapEnts, numAudioPropNodes) == offsetof(MapEnts, numCollmaps) - offsetof(MapEnts, numAudioPropNodes));

		utils::memory::allocator allocator;
		const auto ents = allocator.allocate<target::MapEnts>();
		ents->name = bsp_name(asset->name, allocator);

		const auto entity_string = convert_entity_string(asset);
		ents->entityString = allocator.duplicate_string(entity_string);
		ents->numEntityChars = static_cast<int>(entity_string.size() + 1);

		copy_identical(ents->trigger, asset->trigger);
		convert_client_triggers(asset->clientTrigger, ents->clientTrigger, allocator);
		copy_identical(ents->clientTriggerBlend, asset->clientTriggerBlend);

		std::memcpy(&ents->spawnList, &asset->spawnList, offsetof(MapEnts, scriptableMapEnts) - offsetof(MapEnts, spawnList));
		ents->edgeListSpatialTree = convert_spatial_tree(asset->edgeListSpatialTree, allocator);
		add_scriptable_dynents(asset, *ents, allocator);

		convert_scriptable_map_ents(asset, ents->scriptableMapEnts, allocator);

		copy_identical(ents->spawnGroupLoot, asset->spawnGroupLoot);
		convert_client_side_effects(asset, ents->clientSideEffects, allocator);
		std::memcpy(&ents->createFxAssetData, &asset->createFxAssetData, offsetof(MapEnts, audioPASpeakerCount) - offsetof(MapEnts, createFxAssetData));

		if (asset->audioPASpeakerCount)
		{
			ZONETOOL_WARNING("mapents \"%s\" loses its %u PA speakers", asset->name, asset->audioPASpeakerCount);
		}

		std::memcpy(&ents->numAudioPropNodes, &asset->numAudioPropNodes, offsetof(MapEnts, numCollmaps) - offsetof(MapEnts, numAudioPropNodes));
		ents->havokEntsShapeDataSize = 0;
		ents->havokEntsShapeData = nullptr;
		if (asset->havokEntsShapeData && asset->havokEntsShapeDataSize)
		{
			const auto shape_data = havok::convert_shape_list(std::string_view(asset->havokEntsShapeData, asset->havokEntsShapeDataSize), asset->name);
			if (shape_data)
			{
				ents->havokEntsShapeDataSize = static_cast<unsigned int>(shape_data->size());
				ents->havokEntsShapeData = allocator.allocate_array<char>(shape_data->size());
				std::memcpy(ents->havokEntsShapeData, shape_data->data(), shape_data->size());
			}
		}

		ents->numCollmaps = asset->numCollmaps;
		copy_identical(ents->collmapLookups, asset->collmapLookups);

		if (asset->waterDynamicsCount || asset->waterTileCount)
		{
			ZONETOOL_WARNING("mapents \"%s\" loses its water dynamics", asset->name);
		}

		target::map_ents::dump(ents);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_MAP_ENTS), ents->name);
	}
}
