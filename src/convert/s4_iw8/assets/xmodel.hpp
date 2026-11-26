#pragma once

namespace zonetool::s4::converter::iw8::xmodel
{
	zonetool::iw8::XModel* convert(const XModel* asset, utils::memory::allocator& allocator);
	void dump(XModel* asset);
}
