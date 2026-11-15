#include <std_include.hpp>

#include "../include.hpp"
#include "../shader_compiler.hpp"
#include "techset.hpp"
#include "shaders.hpp"

#include <iw7/assets/techset.hpp>
#include <iw7/assets/vertexshader.hpp>
#include <iw7/assets/pixelshader.hpp>

namespace zonetool::iw8::converter::iw7::techset
{
	namespace
	{
		namespace target = zonetool::iw7;

		struct stream_route
		{
			target::MaterialStreamStreamSource source;
			target::MaterialStreamDestination dest;
			std::uint8_t mask;
		};

		struct technique_definition
		{
			const char* name;
			const char* vertex_shader;
			std::vector<std::string_view> vertex_source;
			const char* pixel_shader;
			std::vector<std::string_view> pixel_source;
			const char* vertex_decl;
			std::vector<stream_route> routes;
			unsigned int custom_buffer_flags;
			unsigned int depth_stencil_state_mask;
			bool instanced;
			unsigned short technique_flags;
			target::MaterialPassVertexFormat vertex_format = target::PASS_VERTEX_FORMAT_WORLD;
			std::uint64_t resource_flags;
		};

		struct technique_slot
		{
			generated_technique type;
			bool instanced;
		};

		target::MaterialShaderArgument code_const(const std::uint8_t stage, const std::uint16_t dest, const target::ShaderCodeConstants constant,
			const std::uint8_t rows)
		{
			target::MaterialShaderArgument arg{};
			arg.type = target::MTL_ARG_CODE_CONST;
			arg.shader = stage;
			arg.dest = dest;
			arg.u.codeConst.index = static_cast<unsigned short>(constant);
			arg.u.codeConst.rowCount = rows;
			return arg;
		}

		target::MaterialShaderArgument named_arg(const target::MaterialShaderArgumentType type, const std::uint16_t dest, const char* name)
		{
			target::MaterialShaderArgument arg{};
			arg.type = type;
			arg.shader = target::MTL_SHADER_STAGE_PS;
			arg.dest = dest;
			arg.u.nameHash = hash_material_name(name);
			return arg;
		}

		void append_named_args(std::vector<target::MaterialShaderArgument>& args, const target::MaterialShaderArgumentType type, const std::span<const char* const> names)
		{
			const auto first = args.size();
			for (auto i = 0u; i < names.size(); i++)
			{
				args.emplace_back(named_arg(type, static_cast<std::uint16_t>(i), names[i]));
			}

			std::ranges::sort(args.begin() + first, args.end(), {}, [](const target::MaterialShaderArgument& arg) { return arg.u.nameHash; });
		}

		std::vector<target::MaterialShaderArgument> transform_args(const bool instanced)
		{
			std::vector<target::MaterialShaderArgument> args;
			if (!instanced)
			{
				args.emplace_back(code_const(target::MTL_SHADER_STAGE_VS, 0, target::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, 4));
			}

			args.emplace_back(code_const(target::MTL_SHADER_STAGE_VS, 0, target::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, 4));
			args.emplace_back(code_const(target::MTL_SHADER_STAGE_VS, 0, target::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, 1));
			return args;
		}

		std::vector<target::MaterialShaderArgument> lit_args(const bool instanced, const std::span<const char* const> constant_names,
			const std::span<const char* const> texture_names)
		{
			auto args = transform_args(instanced);
			args.emplace_back(code_const(target::MTL_SHADER_STAGE_PS, 1, target::CONST_SRC_CODE_SUN_LIGHT_DIR, 1));
			args.emplace_back(code_const(target::MTL_SHADER_STAGE_PS, 2, target::CONST_SRC_CODE_SUN_LIGHT_COLOR_AND_UV, 1));

			append_named_args(args, target::MTL_ARG_MATERIAL_CONST, constant_names);
			append_named_args(args, target::MTL_ARG_MATERIAL_TEXTURE, texture_names);
			append_named_args(args, target::MTL_ARG_MATERIAL_SAMPLER, texture_names);
			return args;
		}

		std::vector<target::MaterialShaderArgument> unlit_args(const bool instanced)
		{
			const std::array<const char*, 1> color_map{material_texture_names[0]};

			auto args = transform_args(instanced);
			append_named_args(args, target::MTL_ARG_MATERIAL_CONST, material_constant_names);
			append_named_args(args, target::MTL_ARG_MATERIAL_TEXTURE, color_map);
			append_named_args(args, target::MTL_ARG_MATERIAL_SAMPLER, color_map);
			return args;
		}

		std::vector<target::MaterialShaderArgument> sky_args()
		{
			const std::array<const char*, 1> color_map{material_texture_names[0]};

			auto args = transform_args(false);
			append_named_args(args, target::MTL_ARG_MATERIAL_CONST, sky_constant_names);
			append_named_args(args, target::MTL_ARG_MATERIAL_TEXTURE, color_map);
			append_named_args(args, target::MTL_ARG_MATERIAL_SAMPLER, color_map);
			return args;
		}

		std::vector<target::MaterialShaderArgument> make_args(const generated_technique type, const bool instanced)
		{
			switch (type)
			{
			case GENERATED_LIT:
				return lit_args(instanced, material_constant_names, material_texture_names);
			case GENERATED_DECAL:
				return lit_args(false, decal_constant_names, decal_texture_names);
			case GENERATED_UNLIT:
				return unlit_args(instanced);
			case GENERATED_SKY:
				return sky_args();
			default:
				return transform_args(instanced);
			}
		}

		unsigned short get_stable_size(const std::vector<target::MaterialShaderArgument>& args, const unsigned int first_stable)
		{
			unsigned short rows{};
			for (auto i = first_stable; i < args.size(); i++)
			{
				if (args[i].type == target::MTL_ARG_CODE_CONST)
				{
					rows = std::max<unsigned short>(rows, args[i].dest + args[i].u.codeConst.rowCount);
				}
			}

			return rows * 16;
		}

		template <typename T>
		T* make_shader(const char* name, const std::span<const std::string_view> sources, const char* profile, utils::memory::allocator& allocator)
		{
			auto full_source = std::string(shaders::common);
			for (const auto source : sources)
			{
				full_source += source;
			}

			const auto bytecode = shader_compiler::compile(full_source, name, "main", profile);

			auto* shader = allocator.allocate<T>();
			shader->name = allocator.duplicate_string(name);
			shader->debugName = shader->name;
			shader->prog.loadDef.program = allocator.allocate_array<unsigned char>(bytecode.size());
			std::memcpy(shader->prog.loadDef.program, bytecode.data(), bytecode.size());
			shader->prog.loadDef.programSize = static_cast<unsigned int>(bytecode.size());
			return shader;
		}

		target::MaterialVertexDeclaration* make_vertex_decl(const technique_definition& definition, utils::memory::allocator& allocator)
		{
			auto* decl = allocator.allocate<target::MaterialVertexDeclaration>();
			decl->name = definition.vertex_decl;
			decl->streamCount = static_cast<unsigned char>(definition.routes.size());
			for (auto i = 0u; i < definition.routes.size(); i++)
			{
				decl->routing.data[i].source = definition.routes[i].source;
				decl->routing.data[i].dest = definition.routes[i].dest;
				decl->routing.data[i].mask = definition.routes[i].mask;
			}

			return decl;
		}

		target::MaterialTechnique* make_technique(const technique_definition& definition, const std::vector<target::MaterialShaderArgument>& args,
			utils::memory::allocator& allocator)
		{
			auto* technique = reinterpret_cast<target::MaterialTechnique*>(allocator.allocate_array<char>(sizeof(target::MaterialTechnique)));
			technique->hdr.name = definition.name;
			technique->hdr.flags = definition.technique_flags;
			technique->hdr.passCount = 1;

			auto& pass = technique->passArray[0];
			pass.vertexShader = make_shader<target::MaterialVertexShader>(definition.vertex_shader, definition.vertex_source, "vs_5_0", allocator);
			pass.vertexDecl = make_vertex_decl(definition, allocator);
			if (definition.pixel_shader)
			{
				pass.pixelShader = make_shader<target::MaterialPixelShader>(definition.pixel_shader, definition.pixel_source, "ps_5_0", allocator);
			}

			const auto per_prim_count = definition.instanced ? 0u : 1u;
			pass.pixelOutputMask = definition.pixel_source.back() == shaders::null_ps ? 0 : 1;
			pass.perPrimArgCount = static_cast<unsigned char>(per_prim_count);
			pass.perObjArgCount = 1;
			pass.stableArgCount = static_cast<unsigned char>(args.size() - per_prim_count - 1);
			pass.perPrimArgSize = static_cast<unsigned short>(per_prim_count * 64);
			pass.perObjArgSize = 64;
			pass.stableArgSize = get_stable_size(args, per_prim_count + 1);
			pass.customBufferFlags = definition.custom_buffer_flags;
			pass.depthStencilStateMask = definition.depth_stencil_state_mask;
			pass.resourceFlags = definition.resource_flags;
			pass.vertexFormat = definition.vertex_format;
			pass.customSamplerFlags = 1; // TODO: meaning unknown, stock world passes use 1

			pass.args = allocator.allocate_array<target::MaterialShaderArgument>(args.size());
			std::memcpy(pass.args, args.data(), args.size() * sizeof(target::MaterialShaderArgument));
			return technique;
		}

		void dump_shaders(const target::MaterialTechnique* technique)
		{
			const auto& pass = technique->passArray[0];
			target::vertex_shader::dump(pass.vertexShader);
			if (pass.pixelShader)
			{
				target::pixel_shader::dump(pass.pixelShader);
			}
		}

		bool is_lit(const std::uint32_t technique, const std::uint32_t first)
		{
			return technique >= first && technique < first + 14;
		}

		std::optional<technique_slot> get_world_slot(const std::uint32_t technique)
		{
			switch (technique)
			{
			case target::TECHNIQUE_DEPTH_PREPASS:
			case target::TECHNIQUE_DEPTH_PREPASS_VELOCITY_RIGID:
			case target::TECHNIQUE_DEPTH_PREPASS_VELOCITY_SKINNED:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_VELOCITY_RIGID:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_VELOCITY_SKINNED:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID:
			case target::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED:
				return technique_slot{GENERATED_DEPTH_PREPASS, false}; // TODO: velocity and object id outputs
			case target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
			case target::TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_DEPTH:
				return technique_slot{GENERATED_SHADOWMAP, false};
			case target::TECHNIQUE_UNLIT:
			case target::TECHNIQUE_NO_DISPLACEMENT_UNLIT:
				return technique_slot{GENERATED_UNLIT, false};
			default:
				break;
			}

			if (is_lit(technique, target::TECHNIQUE_LIT) || is_lit(technique, target::TECHNIQUE_NO_DISPLACEMENT_LIT))
			{
				return technique_slot{GENERATED_LIT, false};
			}

			return {};
		}

		std::optional<technique_slot> get_model_slot(const std::uint32_t technique)
		{
			switch (technique)
			{
			case target::TECHNIQUE_DEPTH_PREPASS:
			case target::TECHNIQUE_DEPTH_PREPASS_VELOCITY_RIGID:
			case target::TECHNIQUE_DEPTH_PREPASS_VELOCITY_SKINNED:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_RIGID:
			case target::TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELOCITY_SKINNED:
				return technique_slot{GENERATED_DEPTH_PREPASS, false}; // TODO: velocity and object id outputs
			case target::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
				return technique_slot{GENERATED_SHADOWMAP, false};
			case target::TECHNIQUE_UNLIT:
				return technique_slot{GENERATED_UNLIT, false};
			case target::TECHNIQUE_INSTANCED_DEPTH_PREPASS:
			case target::TECHNIQUE_INSTANCED_DEPTH_PREPASS_VELOCITY_RIGID:
			case target::TECHNIQUE_INSTANCED_DEPTH_PREPASS_OBJECTID:
				return technique_slot{GENERATED_DEPTH_PREPASS, true};
			case target::TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_DEPTH:
				return technique_slot{GENERATED_SHADOWMAP, true};
			case target::TECHNIQUE_INSTANCED_UNLIT:
				return technique_slot{GENERATED_UNLIT, true};
			default:
				break;
			}

			if (is_lit(technique, target::TECHNIQUE_LIT))
			{
				return technique_slot{GENERATED_LIT, false};
			}

			if (is_lit(technique, target::TECHNIQUE_INSTANCED_LIT))
			{
				return technique_slot{GENERATED_LIT, true};
			}

			return {};
		}

		std::optional<technique_slot> get_sky_slot(const std::uint32_t technique)
		{
			if (technique == target::TECHNIQUE_UNLIT || is_lit(technique, target::TECHNIQUE_LIT))
			{
				return technique_slot{GENERATED_SKY, false};
			}

			return {};
		}

		std::optional<technique_slot> get_decal_slot(const std::uint32_t technique)
		{
			if (technique == target::TECHNIQUE_UNLIT || technique == target::TECHNIQUE_NO_DISPLACEMENT_UNLIT
				|| is_lit(technique, target::TECHNIQUE_LIT) || is_lit(technique, target::TECHNIQUE_NO_DISPLACEMENT_LIT))
			{
				return technique_slot{GENERATED_DECAL, false};
			}

			return {};
		}

		std::optional<technique_slot> get_slot(const generated_techset techset, const std::uint32_t technique)
		{
			switch (techset)
			{
			case GENERATED_TECHSET_WORLD:
				return get_world_slot(technique);
			case GENERATED_TECHSET_MODEL:
				return get_model_slot(technique);
			case GENERATED_TECHSET_DECAL:
				return get_decal_slot(technique);
			default:
				return get_sky_slot(technique);
			}
		}

		technique_definition get_base_definition(const generated_technique type, const bool instanced)
		{
			const std::vector<stream_route> position_routes{{target::STREAM_SRC_POSITION, target::STREAM_DST_POSITION, 7}};
			const std::vector<stream_route> unlit_routes
			{
				{target::STREAM_SRC_POSITION, target::STREAM_DST_POSITION, 15},
				{target::STREAM_SRC_TEXCOORD_0, target::STREAM_DST_TEXCOORD_0, 15},
			};
			const std::vector<stream_route> lit_routes
			{
				{target::STREAM_SRC_POSITION, target::STREAM_DST_POSITION, 15},
				{target::STREAM_SRC_TEXCOORD_0, target::STREAM_DST_TEXCOORD_0, 15},
				{target::STREAM_SRC_NORMAL, target::STREAM_DST_NORMAL, 15},
				{target::STREAM_SRC_TANGENT, target::STREAM_DST_TEXCOORD_2, 15},
			};

			switch (type)
			{
			case GENERATED_DEPTH_PREPASS:
				return instanced
					? technique_definition{"iw8_depth_prepass_instanced", "iw8_position_only_instanced_vs", {shaders::position_only_instanced_vs},
						"iw8_null_ps", {shaders::null_ps}, "pp.7", position_routes, 1, 0x17, true}
					: technique_definition{"iw8_depth_prepass", "iw8_position_only_vs", {shaders::position_only_vs},
						"iw8_null_ps", {shaders::null_ps}, "pp.7", position_routes, 1, 0x17, false, target::TECHNIQUE_FLAG_POSITION_ONLY_PREPASS};
			case GENERATED_SHADOWMAP:
				return instanced
					? technique_definition{"iw8_shadowmap_instanced", "iw8_position_only_instanced_vs", {shaders::position_only_instanced_vs},
						"iw8_null_ps", {shaders::null_ps}, "pp.7", position_routes, 3, 1, true}
					: technique_definition{"iw8_shadowmap", "iw8_position_only_vs", {shaders::position_only_vs},
						"iw8_null_ps", {shaders::null_ps}, "pp.7", position_routes, 3, 1, false};
			case GENERATED_UNLIT:
				return instanced
					? technique_definition{"iw8_unlit_instanced", "iw8_unlit_instanced_vs", {shaders::unlit_vs_body, shaders::unlit_instanced_vs},
						"iw8_unlit_ps", {shaders::surface_material, shaders::unlit_ps}, "pp.ft0t0.f", unlit_routes, 1, 1, true}
					: technique_definition{"iw8_unlit", "iw8_unlit_vs", {shaders::unlit_vs_body, shaders::unlit_vs},
						"iw8_unlit_ps", {shaders::surface_material, shaders::unlit_ps}, "pp.ft0t0.f", unlit_routes, 1, 1, false};
			case GENERATED_SKY:
				return technique_definition{"iw8_sky", "iw8_sky_vs", {shaders::sky_vs}, "iw8_sky_ps", {shaders::sky_ps}, "pp.7", position_routes, 1, 1, false};
			case GENERATED_DECAL:
				return technique_definition{"iw8_world_decal", "iw8_world_lit_vs", {shaders::lit_vs_body, shaders::lit_vs},
					"iw8_world_decal_ps", {shaders::surface_material, shaders::lit_ps_body, shaders::decal_ps}, "pp.ft0t0.fnn.ftt2.f", lit_routes, 1, 1, false};
			default:
				return instanced
					? technique_definition{"iw8_model_lit_instanced", "iw8_lit_instanced_vs", {shaders::lit_vs_body, shaders::lit_instanced_vs},
						"iw8_world_lit_ps", {shaders::surface_material, shaders::lit_ps_body, shaders::lit_ps}, "pp.ft0t0.fnn.ftt2.f", lit_routes, 1, 1, true}
					: technique_definition{"iw8_world_lit", "iw8_world_lit_vs", {shaders::lit_vs_body, shaders::lit_vs},
						"iw8_world_lit_ps", {shaders::surface_material, shaders::lit_ps_body, shaders::lit_ps}, "pp.ft0t0.fnn.ftt2.f", lit_routes, 1, 1, false};
			}
		}

		const char* get_model_technique_name(const generated_technique type, const bool instanced)
		{
			switch (type)
			{
			case GENERATED_DEPTH_PREPASS:
				return instanced ? "iw8_model_depth_prepass_instanced" : "iw8_model_depth_prepass";
			case GENERATED_SHADOWMAP:
				return instanced ? "iw8_model_shadowmap_instanced" : "iw8_model_shadowmap";
			case GENERATED_UNLIT:
				return instanced ? "iw8_model_unlit_instanced" : "iw8_model_unlit";
			default:
				return instanced ? "iw8_model_lit_instanced" : "iw8_model_lit";
			}
		}

		std::uint64_t get_model_resource_flags(const generated_technique type, const bool instanced)
		{
			std::uint64_t flags = target::PASS_RESOURCE_PER_OBJECT_BUFFER | target::PASS_RESOURCE_STABLE_BUFFER;
			flags |= instanced ? target::PASS_RESOURCE_INSTANCED : target::PASS_RESOURCE_PER_PRIM_BUFFER;
			if (type == GENERATED_LIT || type == GENERATED_UNLIT)
			{
				flags |= target::PASS_RESOURCE_MATERIAL_BUFFER;
			}

			return flags;
		}

		technique_definition get_definition(const generated_techset techset, const generated_technique type, const bool instanced)
		{
			auto definition = get_base_definition(type, instanced);
			if (techset != GENERATED_TECHSET_MODEL)
			{
				return definition;
			}

			definition.name = get_model_technique_name(type, instanced);
			definition.resource_flags = get_model_resource_flags(type, instanced);
			definition.vertex_format = type == GENERATED_LIT ? target::PASS_VERTEX_FORMAT_MODEL_LIT : target::PASS_VERTEX_FORMAT_MODEL;
			if (type == GENERATED_LIT)
			{
				definition.technique_flags |= target::TECHNIQUE_FLAG_MODEL_LIT;
			}

			return definition;
		}
	}

	std::span<const char* const> get_constant_names(const generated_techset techset)
	{
		switch (techset)
		{
		case GENERATED_TECHSET_SKY:
			return sky_constant_names;
		case GENERATED_TECHSET_DECAL:
			return decal_constant_names;
		default:
			return material_constant_names;
		}
	}

	std::uint32_t hash_material_name(const std::string_view name)
	{
		std::uint32_t hash{};
		for (const auto c : name)
		{
			hash = hash * 33 ^ static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(c)));
		}

		return hash;
	}

	const char* get_techset_name(const generated_techset techset)
	{
		switch (techset)
		{
		case GENERATED_TECHSET_WORLD:
			return "iw8_world_lit";
		case GENERATED_TECHSET_MODEL:
			return "iw8_model_lit";
		case GENERATED_TECHSET_DECAL:
			return "iw8_world_decal";
		default:
			return "iw8_sky";
		}
	}

	std::optional<generated_technique> get_generated_technique(const generated_techset techset, const std::uint32_t technique)
	{
		if (const auto slot = get_slot(techset, technique))
		{
			return slot->type;
		}

		return {};
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
		std::map<std::pair<generated_technique, bool>, target::MaterialTechnique*> techniques;

		auto* asset = allocator.allocate<target::MaterialTechniqueSet>();
		asset->name = get_techset_name(techset);

		std::vector<target::MaterialTechnique*> masked;
		for (auto i = 0u; i < target::TECHNIQUE_COUNT; i++)
		{
			const auto slot = get_slot(techset, i);
			if (!slot)
			{
				continue;
			}

			auto& technique = techniques[{slot->type, slot->instanced}];
			if (!technique)
			{
				technique = make_technique(get_definition(techset, slot->type, slot->instanced), make_args(slot->type, slot->instanced), allocator);
			}

			asset->techniqueMask[i >> 6] |= 1ull << (i & target::TECHNIQUE_MASK);
			masked.emplace_back(technique);
		}

		for (auto i = 1u; i < std::size(asset->techniqueMaskStartIndex); i++)
		{
			asset->techniqueMaskStartIndex[i] = static_cast<short>(asset->techniqueMaskStartIndex[i - 1] + std::popcount(asset->techniqueMask[i - 1]));
		}

		asset->techniqueCount = static_cast<unsigned short>(masked.size());
		asset->techniques = masked.data();

		target::techset::dump(asset);
		for (const auto& [key, technique] : techniques)
		{
			dump_shaders(technique);
		}
	}
}
