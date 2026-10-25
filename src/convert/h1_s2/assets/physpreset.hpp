#pragma once

namespace zonetool::h1::converter::s2::physpreset
{
	zonetool::s2::PhysPreset* reference(const PhysPreset* preset, const XModel* model, utils::memory::allocator& allocator);
	void clear();
}
