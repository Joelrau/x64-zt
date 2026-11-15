#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class material
	{
	public:
		static void read(zone_reader& reader, Material* asset);
		static void dump(Material* asset);
	};

	class technique_set
	{
	public:
		static void read(zone_reader& reader, MaterialTechniqueSet* asset);
		static void dump(MaterialTechniqueSet* asset);
	};

	std::size_t get_technique_count(const MaterialTechniqueSet* asset);
	std::optional<std::uint32_t> get_technique_type(const MaterialTechniqueSet* asset, std::size_t masked_index);

	template <typename T, shader_type Type>
	class material_shader
	{
	public:
		static void read(zone_reader& reader, T* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->debugName);
			reader.read_array(asset->prog.loadDef.program, 3, asset->prog.loadDef.programSize);
			reader.pop_stream();
		}

		static void dump(T* asset)
		{
			filesystem::file file(get_shader_path(asset->name, Type));
			file.open("wb");
			file.write(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
			file.close();
		}
	};

	using lib_shader = material_shader<MaterialLibShader, libshader>;
	using compute_shader = material_shader<ComputeShader, computeshader>;
	using vertex_shader = material_shader<MaterialVertexShader, vertexshader>;
	using hull_shader = material_shader<MaterialHullShader, hullshader>;
	using domain_shader = material_shader<MaterialDomainShader, domainshader>;
	using pixel_shader = material_shader<MaterialPixelShader, pixelshader>;
}
