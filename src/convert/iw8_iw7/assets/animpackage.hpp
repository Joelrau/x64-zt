#pragma once
#include "../include.hpp"

namespace zonetool::iw8::converter::iw7::animpackage
{
	std::vector<XAnimParts*> converted_anims(const WeaponAnimPackage* asset);
	void dump(WeaponAnimPackage* asset);
}
