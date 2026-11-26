#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class post_fx_bundle
	{
	public:
		static void read(auto& reader, PostFxBundle* asset);
	};

	class render_override_bundle
	{
	public:
		static void read(auto& reader, RenderOverrideBundle* asset);
	};
}
