#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_occlusion_buffer(auto& reader, CpuOcclusionBuffer& buffer);

	class cpu_occlusion_data
	{
	public:
		static void read(auto& reader, CpuOcclusionData* asset);
	};
}
