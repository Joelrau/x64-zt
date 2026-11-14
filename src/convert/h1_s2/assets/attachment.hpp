#pragma once

namespace zonetool::h1::converter::s2::attachment
{
	void dump(WeaponAttachment* asset);
	zonetool::s2::WeaponAttachment* reference(const WeaponAttachment* asset, utils::memory::allocator& allocator);
	zonetool::s2::WeaponAttachment* built_in_scope(const WeaponDef* weapon, utils::memory::allocator& allocator);
	void clear();
}
