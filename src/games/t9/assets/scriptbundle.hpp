#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_script_bundle_object(auto& reader, ScriptBundleObject& object);

	class script_bundle
	{
	public:
		static void read(auto& reader, ScriptBundle* asset);
	};
}
