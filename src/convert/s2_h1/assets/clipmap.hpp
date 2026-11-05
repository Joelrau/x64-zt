#pragma once

namespace zonetool::s2::converter::h1::clipmap
{
	void dump(zonetool::s2::clipMap_t* asset);
	void add_transient_zone(zonetool::s2::ClipMapTrZone* zone);
	void finish();
}
