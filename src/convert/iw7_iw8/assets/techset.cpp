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

			if (uses_world_vertices(techset))
			{
				args.push_back({target::MTL_ARG_CODE_BUFFER, target::MTL_SHADER_STAGE_VS, 13, target::BUFFER_SRC_CODE_WORLD_SURFACE_DATA});
			}

			return args;
		}

		void add_arg(std::vector<target::MaterialShaderArgument>& args, const target::MaterialShaderArgument& arg)
		{
			const auto position = std::ranges::find_if(args, [&](const target::MaterialShaderArgument& other)
			{
				return std::tie(other.type, other.dest) > std::tie(arg.type, arg.dest);
			});

			args.insert(position, arg);
		}

		std::vector<target::MaterialShaderArgument> lit_args(const generated_techset techset)
		{
			auto args = transform_args(techset);
			add_arg(args, {target::MTL_ARG_CODE_CONST, target::MTL_SHADER_STAGE_PS, 6, target::CONST_SRC_CODE_RENDER_TARGET_SIZE});
			add_arg(args, {target::MTL_ARG_CODE_CONST, target::MTL_SHADER_STAGE_PS, 7, target::CONST_SRC_CODE_RESOLUTION_SCALE});
			if (uses_world_vertices(techset))
			{
				add_arg(args, {target::MTL_ARG_CODE_BUFFER, target::MTL_SHADER_STAGE_PS, 0, target::BUFFER_SRC_CODE_LIGHTMAP_PACKING});
			}

			const unsigned short color_map = uses_world_vertices(techset) ? 1 : 0;
			add_arg(args, {target::MTL_ARG_CODE_BUFFER, target::MTL_SHADER_STAGE_PS, 5, target::BUFFER_SRC_CODE_SCREEN_TILES});
			add_arg(args, {target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, color_map, 0});
			add_arg(args, {target::MTL_ARG_CODE_CBUFFER, target::MTL_SHADER_STAGE_PS, 7, 0});
			return args;
		}

		std::vector<target::MaterialShaderArgument> unlit_args(const generated_techset techset)
		{
			auto args = transform_args(techset);
			const unsigned short color_map = uses_world_vertices(techset) ? 1 : 0;
			add_arg(args, {target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, color_map, 0});
			add_arg(args, {target::MTL_ARG_CODE_CBUFFER, target::MTL_SHADER_STAGE_PS, 7, 0});
			add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_PS, 0, target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT});
			return args;
		}

		std::vector<target::MaterialShaderArgument> add_alpha_map(std::vector<target::MaterialShaderArgument> args)
		{
			add_arg(args, {target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, 2, 27});
			return args;
		}

		std::vector<target::MaterialShaderArgument> get_lit_args(const generated_variant& variant)
		{
			auto args = variant.lit ? lit_args(variant.techset) : unlit_args(variant.techset);
			if (variant.blend != GENERATED_BLEND_OPAQUE && !variant.water)
			{
				args = add_alpha_map(args);
			}

			if (variant.uv_anim)
			{
				const unsigned char time_stage = variant.water ? target::MTL_SHADER_STAGE_VS | target::MTL_SHADER_STAGE_PS : target::MTL_SHADER_STAGE_VS;
				add_arg(args, {target::MTL_ARG_CODE_CONST, time_stage, 4, target::CONST_SRC_CODE_GAME_TIME});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 1, target::MATERIAL_CONSTANT_UV_ANIM});
			}

			if (variant.water)
			{
				add_arg(args, {target::MTL_ARG_CODE_CONST, target::MTL_SHADER_STAGE_PS, 8, target::CONST_SRC_CODE_CAMERA_POSITION});
				add_arg(args, {target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, 3, 9});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_PS, 0, target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_PS, 2, target::MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_0});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_PS, 3, target::MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_1});
			}

			if (variant.effect)
			{
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 4, target::MATERIAL_CONSTANT_FALLOFF_PARMS});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 5, target::MATERIAL_CONSTANT_FALLOFF_BEGIN_COLOR});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 6, target::MATERIAL_CONSTANT_FALLOFF_END_COLOR});
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 8, target::MATERIAL_CONSTANT_EYE_OFFSET_PARMS});
			}

			if (variant.near_fade)
			{
				add_arg(args, {target::MTL_ARG_MATERIAL_CONST, target::MTL_SHADER_STAGE_VS, 7, target::MATERIAL_CONSTANT_FEATHER_PARMS});
			}

			return args;
		}

		const char* get_lit_vertex_entry(const generated_variant& variant)
		{
			if (variant.effect)
			{
				if (uses_world_vertices(variant.techset))
				{
					return variant.near_fade ? "world_effect_fade_vs" : "world_effect_vs";
				}

				return variant.near_fade ? "smodel_effect_fade_vs" : "smodel_effect_vs";
			}

			if (variant.vertex_color)
			{
				return variant.uv_anim ? "world_unlit_color_uv_anim_vs" : "world_unlit_color_vs";
			}

			if (uses_world_vertices(variant.techset))
			{
				return variant.water ? "world_water_vs" : variant.uv_anim ? "world_lit_uv_anim_vs" : "world_lit_vs";
			}

			return variant.water ? "smodel_water_vs" : variant.uv_anim ? "smodel_lit_uv_anim_vs" : "smodel_lit_vs";
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

		std::vector<technique_definition> get_sky_definitions()
		{
			auto args = transform_args(GENERATED_TECHSET_WORLD);
			args.push_back({target::MTL_ARG_MATERIAL_TEXTURE, target::MTL_SHADER_STAGE_PS, 1, 0});
			args.push_back({target::MTL_ARG_CODE_CBUFFER, target::MTL_SHADER_STAGE_PS, 7, 0});

			const auto make_definition = [&](const target::MaterialTechniqueType type, const char* name) -> technique_definition
			{
				return {type, name, "world_sky_vs", "sky_ps", {{0x16, {0x20, 0, 0, 0}}}, 1, 0x8100, 32, {0x0C01, 0}, args, {0}};
			};

			return
			{
				make_definition(target::TECHNIQUE_EMISSIVE, "TECHNIQUE_EMISSIVE"),
				make_definition(target::TECHNIQUE_LIT_FORWARDPLUS_BITMASK, "TECHNIQUE_LIT_FORWARDPLUS_BITMASK"),
			};
		}

		const char* get_unlit_pixel_entry(const generated_variant& variant)
		{
			const auto world = uses_world_vertices(variant.techset);
			const auto evc = variant.exposure_compensated;
			if (variant.effect)
			{
				return world ? "world_effect_add_ps" : "smodel_effect_add_ps";
			}

			if (variant.vertex_color)
			{
				return evc ? "world_unlit_add_color_evc_ps" : "world_unlit_add_color_ps";
			}

			switch (variant.blend)
			{
			case GENERATED_BLEND_ALPHA:
				return world ? (evc ? "world_unlit_blend_evc_ps" : "world_unlit_blend_ps") : (evc ? "smodel_unlit_blend_evc_ps" : "smodel_unlit_blend_ps");
			case GENERATED_BLEND_ADD:
				return world ? (evc ? "world_unlit_add_evc_ps" : "world_unlit_add_ps") : (evc ? "smodel_unlit_add_evc_ps" : "smodel_unlit_add_ps");
			default:
				return world ? (evc ? "world_unlit_evc_ps" : "world_unlit_ps") : (evc ? "smodel_unlit_evc_ps" : "smodel_unlit_ps");
			}
		}

		const char* get_lit_pixel_entry(const generated_variant& variant)
		{
			const auto world = uses_world_vertices(variant.techset);
			if (variant.blend == GENERATED_BLEND_MULTIPLY)
			{
				return variant.vertex_color ? "world_multiply_color_ps" : world ? "world_multiply_ps" : "smodel_multiply_ps";
			}

			if (!variant.lit)
			{
				return get_unlit_pixel_entry(variant);
			}

			if (variant.water)
			{
				return world ? "world_water_ps" : "smodel_water_ps";
			}

			switch (variant.blend)
			{
			case GENERATED_BLEND_ALPHA:
				return world ? "world_blend_ps" : "smodel_blend_ps";
			case GENERATED_BLEND_ADD:
				return world ? "world_add_ps" : "smodel_add_ps";
			default:
				return world ? "world_lit_ps" : "smodel_lit_ps";
			}
		}

		std::uint64_t get_blend_bits(const generated_blend blend)
		{
			switch (blend)
			{
			case GENERATED_BLEND_ALPHA:
			case GENERATED_BLEND_ADD:
				return 0x28050; // ONE, INV_SRC_ALPHA
			case GENERATED_BLEND_MULTIPLY:
				return 0x8808; // DEST_COLOR, ZERO
			default:
				return 0;
			}
		}

		std::vector<technique_definition> get_definitions(const generated_variant& variant)
		{
			if (variant.techset == GENERATED_TECHSET_SKY)
			{
				return get_sky_definitions();
			}

			const auto world = uses_world_vertices(variant.techset);
			const auto brush = variant.techset == GENERATED_TECHSET_BRUSH_MODEL;
			const auto alpha_test = variant.alpha_test && variant.blend == GENERATED_BLEND_OPAQUE;
			const std::uint64_t cull_back = variant.two_sided ? 0 : 1;
			const std::uint64_t cull_front = variant.two_sided ? 0 : 2;
			const std::vector<unsigned char> transform_decls = world ? std::vector<unsigned char>{0} : std::vector<unsigned char>{2, 5, 6};
			const std::vector<unsigned char> lit_decls = world ? std::vector<unsigned char>{0} : std::vector<unsigned char>{5, 6};
			const std::vector<target::MaterialRenderTargetFormat> shadow_formats = {{0x14, {0, 0, 0, 0}}};
			const auto transform = transform_args(variant.techset);
			const auto alpha_transform = add_alpha_map(transform);

			std::vector<technique_definition> definitions;
			if (variant.visible && variant.blend == GENERATED_BLEND_OPAQUE)
			{
				definitions.push_back(
				{
					target::TECHNIQUE_DEPTH_PREPASS, "TECHNIQUE_DEPTH_PREPASS",
					world ? "world_prepass_vs" : "smodel_prepass_vs", alpha_test ? "prepass_alpha_ps" : "prepass_ps",
					{{0x16, {0x06, 0x11, 0, 0}}, {0x16, {0x10, 0x11, 0, 0}}}, 2,
					world ? 0x18100u : 0u, static_cast<unsigned char>(brush ? target::MTL_CONSTANT_LAYOUT_BMODEL_AUX_QUERY : world ? 34 : 1),
					{(alpha_test ? 0x2000E00 : 0x0E00) | cull_back, 0xFFFFFF0F00000000}, alpha_test ? alpha_transform : transform, transform_decls,
				});
			}

			if (variant.casts_shadow && variant.blend == GENERATED_BLEND_OPAQUE)
			{
				const auto shadow_vs = alpha_test
					? (world ? "world_shadow_alpha_vs" : "smodel_shadow_alpha_vs")
					: (world ? "world_shadow_vs" : "smodel_shadow_vs");
				const auto shadow_ps = alpha_test ? "shadow_alpha_ps" : nullptr;
				const auto shadow_buffer_flags = world ? (alpha_test ? 0x18100u : 0x8100u) : 0u;
				const auto brush_shadow_index = alpha_test ? target::MTL_CONSTANT_LAYOUT_BMODEL_AUX : target::MTL_CONSTANT_LAYOUT_BMODEL;
				const auto shadow_index = static_cast<unsigned char>(brush ? brush_shadow_index : world ? (alpha_test ? 33 : 32) : 1);

				definitions.push_back(
				{
					target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH, "TECHNIQUE_BUILD_SHADOWMAP_DEPTH", shadow_vs, shadow_ps,
					shadow_formats, 0, shadow_buffer_flags, shadow_index,
					{0x0E20 | cull_back, 0xFFFFFFFF00000000}, alpha_test ? alpha_transform : transform, transform_decls,
				});
				definitions.push_back(
				{
					target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH_BACK_FACE, "TECHNIQUE_BUILD_SHADOWMAP_DEPTH_BACK_FACE", shadow_vs, shadow_ps,
					shadow_formats, 0, shadow_buffer_flags, shadow_index,
					{0x0E20 | cull_front, 0xFFFFFFFF00000000}, alpha_test ? alpha_transform : transform, transform_decls,
				});
			}

			if (variant.visible)
			{
				const auto blend = variant.blend != GENERATED_BLEND_OPAQUE;
				definitions.push_back(
				{
					target::TECHNIQUE_LIT_FORWARDPLUS_BITMASK, "TECHNIQUE_LIT_FORWARDPLUS_BITMASK",
					get_lit_vertex_entry(variant), get_lit_pixel_entry(variant),
					{{0x16, {0x20, 0x01, 0, 0}}}, 2,
					world ? 0x18100u : 0u, static_cast<unsigned char>(brush ? target::MTL_CONSTANT_LAYOUT_BMODEL_LIT_LMAP : world ? 35 : 2),
					{(blend ? 0x0C10 : 0x0800) | cull_back, get_blend_bits(variant.blend)},
					get_lit_args(variant), lit_decls,
				});
			}

			return definitions;
		}

		std::uint64_t get_techset_flags(const generated_variant& variant)
		{
			const auto world = uses_world_vertices(variant.techset);
			if (variant.techset == GENERATED_TECHSET_SKY)
			{
				return 0x20;
			}

			if (variant.blend != GENERATED_BLEND_OPAQUE)
			{
				return world ? 0x1404010001 : 0x4010000;
			}

			if (variant.alpha_test)
			{
				return world ? 0x14040000A1 : 0x10040000A1;
			}

			return world ? 0x404001020 : 0x4001020;
		}

		std::string get_techset_base_name(const generated_variant& variant)
		{
			if (!variant.visible)
			{
				if (!variant.casts_shadow)
				{
					return "nodraw";
				}

				return variant.alpha_test ? "shadowcaster_alphatest" : "shadowcaster";
			}

			const std::string lighting = variant.lit ? "lit" : "unlit";
			switch (variant.blend)
			{
			case GENERATED_BLEND_ALPHA:
				return lighting + "_blend";
			case GENERATED_BLEND_ADD:
				return lighting + "_add";
			case GENERATED_BLEND_MULTIPLY:
				return lighting + "_multiply";
			default:
				return lighting + (variant.alpha_test ? "_alphatest" : "_opaque");
			}
		}

		template <typename T>
		T* get_shader(shader_cache<T>& cache, const std::string& source, const char* entry, const char* profile, utils::memory::allocator& allocator)
		{
			if (const auto itr = cache.find(entry); itr != cache.end())
			{
				return itr->second;
			}

			const auto bytecode = utils::shader_compiler::compile(source, entry, entry, profile);
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

		target::MaterialTechnique* make_technique(const technique_definition& definition, const std::string& suffix, const std::string& source,
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

			technique->vertexShader = get_shader(vertex_shaders, source, definition.vertex_entry, "vs_5_0", allocator);
			technique->pixelShader = definition.pixel_entry
				? get_shader(pixel_shaders, source, definition.pixel_entry, "ps_5_0", allocator)
				: empty_pixel_shader;

			const auto never_args = std::ranges::count(definition.args, target::MTL_ARG_MATERIAL_CONST, &target::MaterialShaderArgument::type);
			technique->stableArgCount = static_cast<unsigned char>(definition.args.size() - never_args);
			technique->neverArgCount = static_cast<unsigned char>(never_args);
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

	bool uses_world_vertices(const generated_techset techset)
	{
		return techset == GENERATED_TECHSET_WORLD || techset == GENERATED_TECHSET_BRUSH_MODEL;
	}

	std::string get_techset_name(const generated_variant& variant)
	{
		if (variant.techset == GENERATED_TECHSET_SKY)
		{
			return "tw/iw7_sky";
		}

		const auto prefix = variant.techset == GENERATED_TECHSET_STATIC_MODEL ? "tmo" : variant.techset == GENERATED_TECHSET_BRUSH_MODEL ? "tb" : "tw";
		const auto two_sided = variant.two_sided ? "_2s" : "";
		const auto no_cast = variant.visible && !variant.casts_shadow && variant.blend == GENERATED_BLEND_OPAQUE ? "_nocast" : "";
		const auto uv_anim = variant.visible && variant.uv_anim ? "_ua" : "";
		const auto evc = variant.visible && variant.exposure_compensated ? "_evc" : "";
		const auto water = variant.visible && variant.water ? "_water" : "";
		const auto effect = variant.visible && variant.effect ? (variant.near_fade ? "_effect_fade" : "_effect") : "";
		const auto vertex_color = variant.visible && variant.vertex_color ? "_vc" : "";
		return std::format("{}/iw7_{}{}{}{}{}{}{}{}", prefix, get_techset_base_name(variant), water, effect, vertex_color, evc, uv_anim, two_sided, no_cast);
	}

	target::MaterialGeometryType get_material_type(const generated_techset techset)
	{
		return techset == GENERATED_TECHSET_STATIC_MODEL ? target::MTL_TYPE_STATIC_MODEL_SELFVIS : target::MTL_TYPE_WORLD;
	}

	void dump_techset(const generated_variant& variant)
	{
		static std::unordered_set<std::string> dumped;
		const auto name = get_techset_name(variant);
		if (!dumped.emplace(name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		shader_cache<target::MaterialVertexShader> vertex_shaders;
		shader_cache<target::MaterialPixelShader> pixel_shaders;
		auto* empty_pixel_shader = make_empty_pixel_shader(allocator);

		const auto definitions = get_definitions(variant);
		const auto suffix = utils::cryptography::sha1::compute(name, true).substr(0, 16);
		const auto source = (variant.techset == GENERATED_TECHSET_BRUSH_MODEL ? "#define BRUSH_MODEL\n"s : ""s) + std::string(shaders::source);

		auto* asset = allocator.allocate<target::MaterialTechniqueSet>();
		asset->name = allocator.duplicate_string(name);
		asset->flags = get_techset_flags(variant);
		asset->materialType = get_material_type(variant.techset);
		asset->maskedTechniques = allocator.allocate_array<target::MaterialTechnique*>(definitions.size());

		for (auto i = 0u; i < definitions.size(); i++)
		{
			const auto type = definitions[i].type;
			asset->techniqueMask.mask[type / 64] |= 1ull << (type % 64);
			asset->maskedTechniques[i] = make_technique(definitions[i], suffix, source, vertex_shaders, pixel_shaders, empty_pixel_shader, allocator);
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
