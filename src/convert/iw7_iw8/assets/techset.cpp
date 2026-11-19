#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"
#include "shaders.hpp"

#include <utils/cryptography.hpp>
#include <utils/shader_compiler.hpp>

#include <iw8/assets/material.hpp>

namespace zonetool::iw7::converter::iw8::techset
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		using shader_cache = std::unordered_map<std::string_view, T*>;

		std::vector<target::MaterialShaderArgument> transform_args(const generated_techset techset)
		{
			std::vector<target::MaterialShaderArgument> args =
			{
				{target::MTL_ARG_CODE_CONST, target::MTL_SHADER_STAGE_VS, 0, target::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX},
				{target::MTL_ARG_CODE_CONST, target::MTL_SHADER_STAGE_VS, 5, target::CONST_SRC_CODE_CAMERA_OFFSET},
			};

			if (techset == GENERATED_TECHSET_WORLD)
			{
				args.push_back({target::MTL_ARG_CODE_BUFFER, target::MTL_SHADER_STAGE_VS, 13, target::BUFFER_SRC_CODE_WORLD_SURFACE_DATA});
			}

			return args;
		}

		std::vector<target::MaterialShaderArgument> lit_args(const generated_techset techset)
		{
			auto args = transform_args(techset);
			const unsigned short color_map = techset == GENERATED_TECHSET_WORLD ? 1 : 0;
			args.push_back({target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, color_map, 0});
			args.push_back({target::MTL_ARG_CODE_CBUFFER, target::MTL_SHADER_STAGE_PS, 7, 0});
			return args;
		}

		unsigned short get_code_constant_rows(const unsigned short index)
		{
			return index == target::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX ? 4 : 1;
		}

		unsigned short get_stable_size(const std::vector<target::MaterialShaderArgument>& args)
		{
			unsigned short rows = 0;
			for (const auto& arg : args)
			{
				if (arg.type == target::MTL_ARG_CODE_CONST)
				{
					rows = std::max<unsigned short>(rows, arg.dest + get_code_constant_rows(arg.index));
				}
			}

			return rows * 16;
		}

		std::vector<technique_definition> get_definitions(const generated_techset techset)
		{
			const auto world = techset == GENERATED_TECHSET_WORLD;
			const auto prepass_vs = world ? "world_prepass_vs" : "smodel_prepass_vs";
			const auto shadow_vs = world ? "world_shadow_vs" : "smodel_shadow_vs";
			const auto lit_vs = world ? "world_lit_vs" : "smodel_lit_vs";
			const auto lit_ps = world ? "world_lit_ps" : "smodel_lit_ps";
			const std::vector<unsigned char> transform_decls = world ? std::vector<unsigned char>{0} : std::vector<unsigned char>{2, 5, 6};
			const std::vector<unsigned char> lit_decls = world ? std::vector<unsigned char>{0} : std::vector<unsigned char>{5, 6};
			const std::vector<target::MaterialRenderTargetFormat> shadow_formats = {{0x14, {0, 0, 0, 0}}};

			return
			{
				{
					target::TECHNIQUE_DEPTH_PREPASS, "TECHNIQUE_DEPTH_PREPASS", prepass_vs, "prepass_ps",
					{{0x16, {0x06, 0x11, 0, 0}}, {0x16, {0x10, 0x11, 0, 0}}}, 2,
					world ? 0x18100u : 0u, static_cast<unsigned char>(world ? 34 : 1),
					{0x0E01, 0xFFFFFF0F00000000}, transform_args(techset), transform_decls,
				},
				{
					target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH, "TECHNIQUE_BUILD_SHADOWMAP_DEPTH", shadow_vs, nullptr,
					shadow_formats, 0,
					world ? 0x8100u : 0u, static_cast<unsigned char>(world ? 32 : 1),
					{0x0E21, 0xFFFFFFFF00000000}, transform_args(techset), transform_decls,
				},
				{
					target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH_BACK_FACE, "TECHNIQUE_BUILD_SHADOWMAP_DEPTH_BACK_FACE", shadow_vs, nullptr,
					shadow_formats, 0,
					world ? 0x8100u : 0u, static_cast<unsigned char>(world ? 32 : 1),
					{0x0E22, 0xFFFFFFFF00000000}, transform_args(techset), transform_decls,
				},
				{
					target::TECHNIQUE_LIT_FORWARDPLUS_BITMASK, "TECHNIQUE_LIT_FORWARDPLUS_BITMASK", lit_vs, lit_ps,
					{{0x16, {0x20, 0x01, 0, 0}}}, 2,
					world ? 0x18100u : 0u, static_cast<unsigned char>(world ? 35 : 2),
					{0x0801, 0}, lit_args(techset), lit_decls,
				},
			};
		}

		template <typename T>
		T* get_shader(shader_cache<T>& cache, const char* entry, const char* profile, utils::memory::allocator& allocator)
		{
			if (const auto itr = cache.find(entry); itr != cache.end())
			{
				return itr->second;
			}

			const auto bytecode = utils::shader_compiler::compile(shaders::source, entry, entry, profile);
			const auto name = utils::cryptography::sha1::compute(bytecode, true).substr(0, 32);

			auto* shader = allocator.allocate<T>();
			shader->name = allocator.duplicate_string(name);
			shader->debugName = shader->name;
			shader->prog.loadDef.program = allocator.allocate_array<char>(bytecode.size());
			std::memcpy(shader->prog.loadDef.program, bytecode.data(), bytecode.size());
			shader->prog.loadDef.programSize = static_cast<unsigned int>(bytecode.size());

			cache.emplace(entry, shader);
			return shader;
		}

		target::MaterialPixelShader* make_empty_pixel_shader(utils::memory::allocator& allocator)
		{
			auto* shader = allocator.allocate<target::MaterialPixelShader>();
			shader->name = "iw7_null_ps";
			shader->debugName = shader->name;
			return shader;
		}

		std::uint64_t get_pipeline_state_hash(const std::string& technique_name, const std::size_t format_index)
		{
			const auto hash = utils::cryptography::sha1::compute(technique_name + "_" + std::to_string(format_index));
			std::uint64_t value{};
			std::memcpy(&value, hash.data(), sizeof(value));
			return value;
		}

		target::MaterialTechnique* make_technique(const technique_definition& definition, const std::string& suffix,
			shader_cache<target::MaterialVertexShader>& vertex_shaders, shader_cache<target::MaterialPixelShader>& pixel_shaders,
			target::MaterialPixelShader* empty_pixel_shader, utils::memory::allocator& allocator)
		{
			const auto name = definition.name + "_"s + suffix;

			auto* technique = allocator.allocate<target::MaterialTechnique>();
			technique->name = allocator.duplicate_string(name);
			technique->type = definition.type;
			technique->pipelineStateInputLayoutCount = 1;

			std::memset(technique->vertexDeclInputLayout, 0xFF, sizeof(technique->vertexDeclInputLayout));
			for (const auto decl : definition.vertex_decls)
			{
				technique->vertexDeclInputLayout[decl] = 0;
			}

			const auto format_count = definition.render_target_formats.size();
			technique->pipelineStates = allocator.allocate_array<target::Packed128>(format_count);
			for (auto i = 0u; i < format_count; i++)
			{
				technique->pipelineStates[i].p0 = get_pipeline_state_hash(name, i);
			}

			technique->inputLayout = allocator.allocate<target::MaterialInputLayout>();
			std::memset(technique->inputLayout->elements, 0xFF, sizeof(technique->inputLayout->elements));

			technique->vertexShader = get_shader(vertex_shaders, definition.vertex_entry, "vs_5_0", allocator);
			technique->pixelShader = definition.pixel_entry
				? get_shader(pixel_shaders, definition.pixel_entry, "ps_5_0", allocator)
				: empty_pixel_shader;

			technique->stableArgCount = static_cast<unsigned char>(definition.args.size());
			technique->stableArgSize = get_stable_size(definition.args);
			technique->colorRtCount = definition.color_rt_count;
			technique->renderTargetFormatCount = static_cast<unsigned char>(format_count);
			technique->renderTargetFormats = allocator.allocate_array<target::MaterialRenderTargetFormat>(format_count);
			std::memcpy(technique->renderTargetFormats, definition.render_target_formats.data(), format_count * sizeof(target::MaterialRenderTargetFormat));

			technique->customBufferFlags = definition.custom_buffer_flags;
			technique->precompiledIndex = definition.precompiled_index;
			technique->stateBits = definition.state_bits;

			technique->args = allocator.allocate_array<target::MaterialShaderArgument>(definition.args.size());
			std::memcpy(technique->args, definition.args.data(), definition.args.size() * sizeof(target::MaterialShaderArgument));
			return technique;
		}
	}

	const char* get_techset_name(const generated_techset techset)
	{
		return techset == GENERATED_TECHSET_WORLD ? "tw/iw7_lit_opaque" : "tmo/iw7_lit_opaque";
	}

	void dump_techset(const generated_techset techset)
	{
		static std::array<bool, GENERATED_TECHSET_COUNT> dumped{};
		if (dumped[techset])
		{
			return;
		}
		dumped[techset] = true;

		utils::memory::allocator allocator;
		shader_cache<target::MaterialVertexShader> vertex_shaders;
		shader_cache<target::MaterialPixelShader> pixel_shaders;
		auto* empty_pixel_shader = make_empty_pixel_shader(allocator);

		const auto definitions = get_definitions(techset);
		const auto suffix = utils::cryptography::sha1::compute(get_techset_name(techset), true).substr(0, 16);

		auto* asset = allocator.allocate<target::MaterialTechniqueSet>();
		asset->name = get_techset_name(techset);
		asset->flags = techset == GENERATED_TECHSET_WORLD ? 0x404001020 : 0x4001020;
		asset->materialType = techset == GENERATED_TECHSET_WORLD ? target::MTL_TYPE_WORLD : target::MTL_TYPE_STATIC_MODEL_SELFVIS;
		asset->maskedTechniques = allocator.allocate_array<target::MaterialTechnique*>(definitions.size());

		for (auto i = 0u; i < definitions.size(); i++)
		{
			const auto type = definitions[i].type;
			asset->techniqueMask.mask[type / 64] |= 1ull << (type % 64);
			asset->maskedTechniques[i] = make_technique(definitions[i], suffix, vertex_shaders, pixel_shaders, empty_pixel_shader, allocator);
		}

		target::technique_set::dump(asset);

		for (const auto& [entry, shader] : vertex_shaders)
		{
			target::vertex_shader::dump(shader);
		}

		for (const auto& [entry, shader] : pixel_shaders)
		{
			target::pixel_shader::dump(shader);
		}

		target::pixel_shader::dump(empty_pixel_shader);
	}
}
