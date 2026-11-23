#pragma once

namespace zonetool::s2::converter::h1::xmodel
{
	void dump(zonetool::s2::XModel* asset);
	void reference(zonetool::s2::XModel* asset);
	bool is_dumped(const std::string& name);
	bool is_wind_sway(const zonetool::s2::XModel* asset);
	const char* reference_phys_preset(const zonetool::s2::XModel* asset, const zonetool::s2::PhysPreset* preset);
}
