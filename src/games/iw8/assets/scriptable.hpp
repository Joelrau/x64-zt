#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class scriptable_def
	{
	public:
		static void read(zone_reader& reader, ScriptableDef* asset);
	};

	class composite_model
	{
	public:
		static void read(zone_reader& reader, XCompositeModelDef* asset);
	};
}
