#pragma once

namespace zonetool::h1::converter::s2::techset
{
	zonetool::s2::MaterialVertexDeclaration* convert_decl(const MaterialVertexDeclaration* decl, const bool wind, const bool lightmap);
	bool has_lightmap_stream(const zonetool::s2::MaterialVertexDeclaration* decl);
	bool has_vertex_lighting_stream(const zonetool::s2::MaterialVertexDeclaration* decl);
	void name_decls(const std::vector<zonetool::s2::MaterialTechniqueSet*>& pending_techsets, std::unordered_set<const void*>& global_shaders);
}
