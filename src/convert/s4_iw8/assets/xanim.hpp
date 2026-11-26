#pragma once

namespace zonetool::s4::converter::iw8::xanim
{
	zonetool::iw8::XAnimParts* convert(const XAnimParts* asset, utils::memory::allocator& allocator);
	void dump(XAnimParts* asset);
}
