#pragma once

namespace zonetool::h1::converter::s2::xmodel
{
	struct default_part
	{
		const XModel* model;
		std::array<float, 3> offset;
	};

	const char* converted_name(zonetool::s2::XAssetType type, const char* name);
	void dump(zonetool::h1::XModel* asset);
	zonetool::s2::XModel* reference(const zonetool::h1::XModel* asset, utils::memory::allocator& allocator);
	void dump_used(const char* name);
	std::optional<std::string> find_built_in_scope(const std::string& weapon, const zonetool::h1::XModel* gun_model);
	void clear();
}
