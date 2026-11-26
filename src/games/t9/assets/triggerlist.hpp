#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	std::string trigger_entry_value(const TriggerEntry& entry);

	class trigger_list
	{
	public:
		static void read(auto& reader, TriggerList* asset);
		static void dump(TriggerList* asset);
	};

	class entity_list
	{
	public:
		static void read(auto& reader, EntityList* asset);
		static void dump(EntityList* asset);
	};
}
