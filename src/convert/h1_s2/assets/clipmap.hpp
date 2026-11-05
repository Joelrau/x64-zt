#pragma once

namespace zonetool::h1::converter::s2::clipmap
{
	struct vertex_range
	{
		std::uint32_t lowest;
		std::uint32_t highest;
	};

	void dump(clipMap_t* asset);
	void finish();
}
