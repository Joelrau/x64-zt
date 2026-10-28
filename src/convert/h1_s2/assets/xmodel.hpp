#pragma once

namespace zonetool::h1::converter::s2::xmodel
{
	const char* converted_name(zonetool::s2::XAssetType type, const char* name);
	void dump(zonetool::h1::XModel* asset);
	zonetool::s2::XModel* reference(const zonetool::h1::XModel* asset, utils::memory::allocator& allocator);
	void dump_used(const char* name);
	void clear();
}
