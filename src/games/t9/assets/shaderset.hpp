#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class compute_shader_set
	{
	public:
		static void read(auto& reader, ComputeShaderSet* asset);
	};

	class render_targets
	{
	public:
		static void read(auto& reader, RenderTargets* asset);
	};

	class hw_platform
	{
	public:
		static void read(auto& reader, HwPlatform* asset);
	};

	class raytrace_shader_set
	{
	public:
		static void read(auto& reader, RaytraceShaderSet* asset);
	};
}
