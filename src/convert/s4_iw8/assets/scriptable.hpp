#pragma once

namespace zonetool::s4::converter::iw8::scriptable
{
	struct child_flags
	{
		int source;
		int converted;
	};

	struct part_slots
	{
		std::optional<unsigned short> animation;
		std::optional<unsigned short> light;
		std::optional<unsigned short> move_server;
		std::optional<unsigned short> move_client;
	};

	struct event_stream
	{
		unsigned short server;
		unsigned short client;
		part_slots part;
	};

	struct conversion
	{
		const char* name;
		utils::memory::allocator& allocator;
		std::map<int, unsigned int> dropped_events;
		unsigned int dropped_weapons;
		std::vector<zonetool::iw8::ScriptablePartReference*> part_references;
		std::set<std::string> animations;
		unsigned int blocked_events;
		unsigned int rounded_hint_holds;
		bool moves;
		std::vector<zonetool::iw8::ScriptableEventModelDef*> model_events;
	};

	zonetool::iw8::ScriptableDef* convert(const ScriptableDef* asset, utils::memory::allocator& allocator);
	int converted_flags(const ScriptableDef* asset);
	void dump(ScriptableDef* asset);
	void dump_anim_trees();
}
