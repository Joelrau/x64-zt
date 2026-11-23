#pragma once

namespace zonetool::s2::converter::h1::techset
{
	zonetool::h1::MaterialTechniqueSet* convert_native(const MaterialTechniqueSet* asset, const std::vector<std::uint32_t>& srgb_textures);
	void dump_native(zonetool::h1::MaterialTechniqueSet* asset);
	void finish_native();
}
