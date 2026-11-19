#pragma once

namespace zonetool::h1::converter::s2::physworld
{
	zonetool::s2::dmPolytopeData convert_polytope(const dmPolytopeData& polytope);

	void dump(PhysWorld* asset);
	void dump_water_preset(PhysWaterPreset* asset);
}
