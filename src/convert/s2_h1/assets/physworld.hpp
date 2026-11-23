#pragma once

namespace zonetool::s2::converter::h1::physworld
{
	zonetool::h1::dmPolytopeData convert_polytope(const zonetool::s2::dmPolytopeData& polytope, utils::memory::allocator& allocator);
	void dump(zonetool::s2::PhysWorld* asset);
}
