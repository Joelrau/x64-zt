#include <std_include.hpp>

#include "../../include.hpp"
#include "../techset.hpp"
#include "decls.hpp"
#include "writer.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		std::unordered_set<std::string> dumped_techsets;
		std::vector<zonetool::s2::MaterialTechniqueSet*> pending_techsets; // written in finish, once every pass of a decl is known
		template <typename T>
		std::unordered_set<std::string> dumped_shaders;
		std::unordered_set<const void*> global_shaders; // velocity technique shaders and decls, s2 has them in its common zones

		template <typename T>
		void dump_shader_file(T* shader)
		{
			if constexpr (std::is_same_v<T, zonetool::s2::MaterialVertexShader>)
			{
				zonetool::s2::vertex_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialPixelShader>)
			{
				zonetool::s2::pixel_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialHullShader>)
			{
				zonetool::s2::hull_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialDomainShader>)
			{
				zonetool::s2::domain_shader::dump(shader);
			}
			else
			{
				zonetool::s2::vertex_decl::dump(shader);
			}
		}

		template <typename T>
		constexpr std::string_view shader_csv_type()
		{
			if constexpr (std::is_same_v<T, zonetool::s2::MaterialVertexShader>)
			{
				return "vertexshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialPixelShader>)
			{
				return "pixelshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialHullShader>)
			{
				return "hullshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialDomainShader>)
			{
				return "domainshader";
			}
			else
			{
				return "vertexdecl";
			}
		}

		template <typename T>
		void dump_shader(T* shader)
		{
			if (!shader || !shader->name || !dumped_shaders<T>.insert(shader->name).second)
			{
				return;
			}

			const auto global = global_shaders.contains(shader);
			if (!global)
			{
				dump_shader_file(shader);
			}

			add_csv_row(shader_csv_type<T>(), shader->name, global);
		}
	}

	void add_global_shader(const void* shader)
	{
		global_shaders.insert(shader);
	}

	void dump(zonetool::s2::MaterialTechniqueSet* asset)
	{
		if (dumped_techsets.insert(asset->name).second)
		{
			pending_techsets.emplace_back(asset);
		}
	}

	void finish()
	{
		name_decls(pending_techsets, global_shaders);
		for (auto* asset : pending_techsets)
		{
			zonetool::s2::techset::dump(asset);
			for (const auto* technique : asset->techniques)
			{
				for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
				{
					const auto& pass = technique->passArray[i];
					dump_shader(pass.vertexShader);
					dump_shader(pass.vertexDecl);
					dump_shader(pass.hullShader);
					dump_shader(pass.domainShader);
					dump_shader(pass.pixelShader);
				}
			}
		}

		pending_techsets.clear();
	}

	void clear()
	{
		dumped_techsets.clear();
		pending_techsets.clear();
		dumped_shaders<zonetool::s2::MaterialVertexShader>.clear();
		dumped_shaders<zonetool::s2::MaterialVertexDeclaration>.clear();
		dumped_shaders<zonetool::s2::MaterialHullShader>.clear();
		dumped_shaders<zonetool::s2::MaterialDomainShader>.clear();
		dumped_shaders<zonetool::s2::MaterialPixelShader>.clear();
	}
}
