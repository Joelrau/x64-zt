#pragma once

namespace zonetool::h1::converter::s2::weapon
{
	zonetool::s2::weapType_t convert_weap_type(weapType_t type);
	std::optional<zonetool::s2::weapClass_t> convert_weap_class(weapClass_t weap_class);

	void dump(WeaponDef* asset);
	void clear();
}
