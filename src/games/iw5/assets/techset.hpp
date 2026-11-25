#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class vertex_decl
	{
	public:
		static void read(zone_reader32& reader, MaterialVertexDeclaration* asset);
	};

	class vertex_shader
	{
	public:
		static void read(zone_reader32& reader, MaterialVertexShader* asset);
	};

	class pixel_shader
	{
	public:
		static void read(zone_reader32& reader, MaterialPixelShader* asset);
	};

	class tech_set
	{
	public:
		static void read(zone_reader32& reader, MaterialTechniqueSet* asset);
	};
}
