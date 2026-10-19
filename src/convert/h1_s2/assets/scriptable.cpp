#include <std_include.hpp>

#include "../include.hpp"
#include "scriptable.hpp"

namespace zonetool::h1::converter::s2::scriptable
{
	namespace
	{
		constexpr unsigned short final_state_damage_flags = 0x07BF;

		zonetool::s2::ScriptablePartDef* make_static_part(const bool has_model, utils::memory::allocator& allocator)
		{
			const auto state = allocator.allocate<zonetool::s2::ScriptableStateDef>();
			state->name = SL_GetString("healthy");
			state->damageFlags = final_state_damage_flags;

			const auto part = allocator.allocate<zonetool::s2::ScriptablePartDef>();
			part->states = state;
			part->stateCount = 1;
			part->partFlags = has_model ? zonetool::s2::SCRIPTABLE_PART_FLAG_SERVER_INSTANCE : 0;
			return part;
		}

		unsigned int instance_state_size(const zonetool::s2::ScriptableDef* def, const zonetool::s2::ScriptableInstance& instance)
		{
			if (!def)
			{
				return 0;
			}

			// sub_140602750
			const auto size = def->partCount * sizeof(*instance.partRuntime) + def->eventStreamSize + def->flagsSize
				+ instance.partStateCount * sizeof(zonetool::s2::ScriptableInstancePartState) + def->modelTagCount;
			return static_cast<unsigned int>(size);
		}
	}

	zonetool::s2::ScriptableDef* convert(ScriptableDef* asset, utils::memory::allocator& allocator)
	{
		// TODO: convert the H1 parts, states and events
		const auto new_asset = allocator.allocate<zonetool::s2::ScriptableDef>();
		new_asset->name = asset->name;
		new_asset->baseModel = reinterpret_cast<zonetool::s2::XModel*>(asset->baseModel);
		new_asset->baseCollision = "";

		const auto has_model = new_asset->baseModel != nullptr;
		new_asset->parts = make_static_part(has_model, allocator);
		new_asset->partCount = 1;
		new_asset->serverInstancePartCount = has_model ? 1 : 0;

		if (has_model)
		{
			new_asset->models = allocator.allocate_array<zonetool::s2::XModel*>(1);
			new_asset->models[0] = new_asset->baseModel;
			new_asset->modelCount = 1;
		}

		return new_asset;
	}

	zonetool::s2::ScriptableMapEnts convert_map_ents(const ScriptableMapEnts& map_ents, utils::memory::allocator& allocator)
	{
		zonetool::s2::ScriptableMapEnts new_map_ents{};
		if (!map_ents.instances || !map_ents.instanceCount)
		{
			return new_map_ents;
		}

		std::unordered_map<ScriptableDef*, zonetool::s2::ScriptableDef*> defs;
		new_map_ents.instanceCount = map_ents.instanceCount;
		new_map_ents.instances = allocator.allocate_array<zonetool::s2::ScriptableInstance>(map_ents.instanceCount);
		for (auto i = 0u; i < map_ents.instanceCount; i++)
		{
			const auto& instance = map_ents.instances[i];
			auto& new_instance = new_map_ents.instances[i];

			if (instance.def)
			{
				auto& def = defs[instance.def];
				if (!def)
				{
					def = convert(instance.def, allocator);
				}

				new_instance.def = def;
				new_instance.partRuntime = allocator.allocate_array<char[4]>(def->partCount);
			}

			std::memcpy(new_instance.origin, instance.origin, sizeof(new_instance.origin));
			std::memcpy(new_instance.angles, instance.angles, sizeof(new_instance.angles));
			std::memcpy(new_instance.startOrigin, instance.startOrigin, sizeof(new_instance.startOrigin));
			std::memcpy(new_instance.startAngles, instance.startAngles, sizeof(new_instance.startAngles));
			new_instance.name = static_cast<zonetool::s2::scr_string_t>(instance.targetname);
			new_instance.updateNextInstance = instance.updateNextInstance;
			new_instance.currentModelIndex = -1;

			new_map_ents.instanceStateSize += instance_state_size(new_instance.def, new_instance);
			if (new_instance.def && new_instance.def->serverInstancePartCount)
			{
				new_map_ents.replicatedInstanceCount++;
			}
		}

		return new_map_ents;
	}

	void dump(ScriptableDef* asset)
	{
		utils::memory::allocator allocator;
		zonetool::s2::scriptable_def::dump(convert(asset, allocator));
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_SCRIPTABLE), asset->name);
	}
}
