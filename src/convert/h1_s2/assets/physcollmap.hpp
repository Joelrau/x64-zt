#pragma once

namespace zonetool::h1::converter::s2::physcollmap
{
	zonetool::s2::PhysCollmap* reference(const PhysCollmap* collmap, utils::memory::allocator& allocator);
	void clear();
}
