#pragma once

namespace zonetool::s2::converter::h1::gfxworld
{
	void dump(zonetool::s2::GfxWorld* asset);
	void add_transient_zone(zonetool::s2::GfxMapTrZone* zone);
	void finish();
}
