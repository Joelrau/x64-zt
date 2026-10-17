#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		template <typename V>
		void visit_triggers(const V& trigger)
		{
			const auto count = [&](const std::size_t offset)
			{
				return trigger.template at<unsigned int>(offset);
			};

			trigger.array(S2_ARRAY_FIELD(MapTriggers, models), 7, count(offsetof(MapTriggers, count)));
			trigger.array(S2_ARRAY_FIELD(MapTriggers, hulls), 7, count(offsetof(MapTriggers, hullCount)));
			trigger.array(S2_ARRAY_FIELD(MapTriggers, slabs), 3, count(offsetof(MapTriggers, slabCount)));
		}

		template <typename V>
		void visit_client_triggers(const V& trigger)
		{
			visit_triggers(trigger.sub(offsetof(ClientTriggers, trigger)));

			const auto count = trigger.template at<unsigned int>(offsetof(ClientTriggers, trigger) + offsetof(MapTriggers, count));
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, clientTriggerAabbTree), 3, trigger.template at<unsigned short>(offsetof(ClientTriggers, numClientTriggerNodes)));
			trigger.bytes(offsetof(ClientTriggers, triggerString), 0, trigger.template at<unsigned int>(offsetof(ClientTriggers, triggerStringLength)));
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, visionSetTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, lightSetTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, clutTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, triggerType), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, origins), 3, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, scriptDelay), 3, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, audioTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, blendLookup), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, npcTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, contextTriggers), 1, count);
			trigger.array(S2_ARRAY_FIELD(ClientTriggers, waterTriggers), 1, count);
			trigger.entries(offsetof(ClientTriggers, leafLists), sizeof(ClientTriggerLeafList), count, [](const auto& list, std::size_t)
			{
				list.array(S2_ARRAY_FIELD(ClientTriggerLeafList, leafs), 3, list.template at<unsigned int>(offsetof(ClientTriggerLeafList, leafCount)));
			});
		}

		template <typename V>
		void visit_spawn_list(const V& list)
		{
			list.aligned_entries(offsetof(SpawnPointRecordList, spawns), 3, sizeof(SpawnPointEntityRecord),
				list.template at<unsigned short>(offsetof(SpawnPointRecordList, spawnsCount)), [](const auto& spawn, std::size_t)
			{
				spawn.script_string(offsetof(SpawnPointEntityRecord, name));
				spawn.script_string(offsetof(SpawnPointEntityRecord, target));
				spawn.script_string(offsetof(SpawnPointEntityRecord, script_noteworthy));
				spawn.script_string(offsetof(SpawnPointEntityRecord, targetname));
			});
		}

		template <typename V>
		void visit_spline_list(const V& list)
		{
			list.entries(offsetof(SplineRecordList, splines), sizeof(SplinePointRecordList),
				list.template at<unsigned short>(offsetof(SplineRecordList, splineCount)), [](const auto& spline, std::size_t)
			{
				spline.entries(offsetof(SplinePointRecordList, splinePoints), sizeof(SplinePointEntityRecord),
					spline.template at<unsigned short>(offsetof(SplinePointRecordList, splinePointCount)), [](const auto& point, std::size_t)
				{
					point.bytes(offsetof(SplinePointEntityRecord, splineNodeLabel), 0, 64);
					point.array(S2_ARRAY_FIELD(SplinePointEntityRecord, positionCubic), 3, 4);
					point.array(S2_ARRAY_FIELD(SplinePointEntityRecord, tangentQuadratic), 3, 3);
				});
			});
		}

		template <typename V>
		void visit_scriptable_instance(const V& instance)
		{
			instance.asset(ASSET_TYPE_SCRIPTABLE, offsetof(ScriptableInstance, def));

			// The def sizes come from the real def asset, because a parsed def is a name stub.
			const auto* def = instance.template header<ScriptableDef>(ASSET_TYPE_SCRIPTABLE, offsetof(ScriptableInstance, def));
			const auto part_count = def ? def->partCount : 0;
			const auto event_stream_size = def ? def->eventStreamSize : 0;
			const auto flags_size = def ? def->flagsSize : 0;
			const auto part_stream_count = def ? def->partStreamCount : 0;
			const auto model_tag_count = def ? def->modelTagCount : 0;

			instance.bytes(offsetof(ScriptableInstance, flags), 0, flags_size);
			instance.aligned_entries(offsetof(ScriptableInstance, partStates), 3, sizeof(ScriptableInstancePartState),
				instance.template at<unsigned char>(offsetof(ScriptableInstance, partStateCount)), [](const auto& state, std::size_t)
			{
				state.script_string(offsetof(ScriptableInstancePartState, name));
			});
			instance.script_string(offsetof(ScriptableInstance, name));
			instance.runtime_bytes(offsetof(ScriptableInstance, partRuntime), 1, 4 * part_count);
			instance.runtime_bytes(offsetof(ScriptableInstance, eventStreamBuffer), 0, event_stream_size);
			instance.runtime_bytes(offsetof(ScriptableInstance, modelTagRuntime), 0, model_tag_count);
			instance.array(S2_ARRAY_FIELD(ScriptableInstance, partStreams), 1, part_stream_count);
			instance.array(S2_ARRAY_FIELD(ScriptableInstance, linkedObjects), 1, instance.template at<unsigned char>(offsetof(ScriptableInstance, linkedObjectCount)));
		}

		template <typename V>
		void visit_scriptable_map_ents(const V& ents)
		{
			ents.entries(offsetof(ScriptableMapEnts, instances), sizeof(ScriptableInstance),
				ents.template at<unsigned int>(offsetof(ScriptableMapEnts, instanceCount)), [](const auto& instance, std::size_t)
			{
				visit_scriptable_instance(instance);
			});
			ents.entries(offsetof(ScriptableMapEnts, reservedDefs), sizeof(ScriptableReservedDef),
				ents.template at<unsigned int>(offsetof(ScriptableMapEnts, reservedDefCount)), [](const auto& reserved, std::size_t)
			{
				reserved.string(offsetof(ScriptableReservedDef, name));
			});
		}

		template <typename V>
		void visit_clip_info(const V& w, const std::size_t offset)
		{
			w.entries(offset, sizeof(ClipInfo), 1, [](const auto& info, std::size_t)
			{
				clip_info_layout::visit(info);
			});
		}
	}

	template <typename V>
	void map_ents_layout::visit(const V& w)
	{
		w.string(offsetof(MapEnts, name));
		w.bytes(offsetof(MapEnts, entityString), 0, w.template at<int>(offsetof(MapEnts, numEntityChars)));
		visit_triggers(w.sub(offsetof(MapEnts, trigger)));
		visit_client_triggers(w.sub(offsetof(MapEnts, clientTrigger)));

		const auto blend = w.sub(offsetof(MapEnts, clientTriggerBlend));
		blend.array(S2_ARRAY_FIELD(ClientTriggerBlend, blendNodes), 3, blend.template at<unsigned short>(offsetof(ClientTriggerBlend, numClientTriggerBlendNodes)));

		visit_spawn_list(w.sub(offsetof(MapEnts, spawnList)));
		visit_spline_list(w.sub(offsetof(MapEnts, splineList)));
		visit_scriptable_map_ents(w.sub(offsetof(MapEnts, scriptableMapEnts)));

		const auto unk344 = w.sub(offsetof(MapEnts, unk344));
		unk344.aligned_entries(offsetof(MapEntsUnk344, entries), 3, sizeof(MapEntsUnk344Entry),
			unk344.template at<unsigned short>(offsetof(MapEntsUnk344, count)), [](const auto& entry, std::size_t)
		{
			for (auto i = 0u; i < std::size(MapEntsUnk344Entry{}.names); i++)
			{
				entry.script_string(offsetof(MapEntsUnk344Entry, names) + i * sizeof(scr_string_t));
			}
		});

		const auto unk360 = w.sub(offsetof(MapEnts, unk360));
		unk360.array(S2_ARRAY_FIELD(MapEntsUnk360, entries), 3, unk360.template at<unsigned int>(offsetof(MapEntsUnk360, count)));
	}

	template <typename V>
	void addon_map_ents_layout::visit(const V& w)
	{
		const auto count = [&](const std::size_t offset)
		{
			return w.template at<unsigned int>(offset);
		};

		w.string(offsetof(AddonMapEnts, name));
		w.bytes(offsetof(AddonMapEnts, entityString), 0, w.template at<int>(offsetof(AddonMapEnts, numEntityChars)));
		visit_triggers(w.sub(offsetof(AddonMapEnts, trigger)));
		visit_clip_info(w, offsetof(AddonMapEnts, info));

		const auto sub_model_count = count(offsetof(AddonMapEnts, numSubModels));
		w.entries(offsetof(AddonMapEnts, cmodels), sizeof(cmodel_t), sub_model_count, [](const auto& model, std::size_t)
		{
			visit_clip_info(model, offsetof(cmodel_t, info));
		});
		w.array(S2_ARRAY_FIELD(AddonMapEnts, models), 3, sub_model_count);
		w.array(S2_ARRAY_FIELD(AddonMapEnts, modelBounds), 3, sub_model_count);

		w.entries(offsetof(AddonMapEnts, collisions), sizeof(AddonMapEntsCollision), count(offsetof(AddonMapEnts, collisionCount)), [](const auto& collision, std::size_t)
		{
			const auto normal_count = collision.template at<int>(offsetof(AddonMapEntsCollision, normalCount));
			collision.array(S2_ARRAY_FIELD(AddonMapEntsCollision, vertices), 15, collision.template at<int>(offsetof(AddonMapEntsCollision, vertexCount)));
			collision.array(S2_ARRAY_FIELD(AddonMapEntsCollision, normals), 15, normal_count);
			collision.array(S2_ARRAY_FIELD(AddonMapEntsCollision, indices), 0, collision.template at<int>(offsetof(AddonMapEntsCollision, indexCount)));
			collision.array(S2_ARRAY_FIELD(AddonMapEntsCollision, data), 0, normal_count);
		});
		w.entries(offsetof(AddonMapEnts, shapes), sizeof(AddonMapEntsShape), count(offsetof(AddonMapEnts, shapeCount)), [](const auto& shape, std::size_t)
		{
			shape.array(S2_ARRAY_FIELD(AddonMapEntsShape, planes), 15, shape.template at<int>(offsetof(AddonMapEntsShape, planeCount)));
			shape.array(S2_ARRAY_FIELD(AddonMapEntsShape, points), 15, shape.template at<int>(offsetof(AddonMapEntsShape, pointCount)));
			shape.array(S2_ARRAY_FIELD(AddonMapEntsShape, edges), 3, shape.template at<int>(offsetof(AddonMapEntsShape, edgeCount)));
		});
	}

	template void map_ents_layout::visit(const field_reader&);
	template void map_ents_layout::visit(const field_writer&);
	template void map_ents_layout::visit(const field_dumper&);
	template void map_ents_layout::visit(const field_parser&);
	template void map_ents_layout::visit(const field_dependencies&);
	template void map_ents_layout::visit(const field_preparer&);

	template void addon_map_ents_layout::visit(const field_reader&);
	template void addon_map_ents_layout::visit(const field_writer&);
	template void addon_map_ents_layout::visit(const field_dumper&);
	template void addon_map_ents_layout::visit(const field_parser&);
	template void addon_map_ents_layout::visit(const field_dependencies&);
	template void addon_map_ents_layout::visit(const field_preparer&);
}
