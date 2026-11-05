#include <std_include.hpp>

#include "../../include.hpp"
#include "../techset.hpp"
#include "args.hpp"
#include "slots.hpp"
#include "velocity.hpp"
#include "writer.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		std::optional<std::int32_t> velocity_source_type(const std::int32_t type)
		{
			switch (type)
			{
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
				return TECHNIQUE_ZPREPASS_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED:
				return TECHNIQUE_ZPREPASS_VELOCITY_SKINNED;
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
				return TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED:
				return TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED;
			case zonetool::s2::TECHNIQUE_VELOCITY_RIGID:
				return TECHNIQUE_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_VELOCITY_SKINNED:
				return TECHNIQUE_VELOCITY_SKINNED;
			default:
				return {};
			}
		}

		template <typename Technique>
		bool uses_material_arguments(const Technique* technique)
		{
			for (auto i = 0; i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				if (pass.hullShader)
				{
					return true;
				}

				auto arg_count = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
				auto material_types = std::array<std::uint8_t, 3>{MTL_ARG_MATERIAL_CONST, MTL_ARG_MATERIAL_TEXTURE, MTL_ARG_MATERIAL_SAMPLER};
				if constexpr (std::is_same_v<Technique, zonetool::s2::MaterialTechnique>)
				{
					arg_count += pass.volatileArgCount;
					material_types = {argument_types[MTL_ARG_MATERIAL_CONST], argument_types[MTL_ARG_MATERIAL_TEXTURE], argument_types[MTL_ARG_MATERIAL_SAMPLER]};
				}

				const auto args = std::span{pass.args, pass.args ? static_cast<std::size_t>(arg_count) : 0u};
				const auto is_material = std::ranges::any_of(args, [&](const auto& arg)
				{
					return std::ranges::find(material_types, arg.type) != material_types.end();
				});

				if (is_material)
				{
					return true;
				}
			}

			return false;
		}

		constexpr std::array<velocity_arg, 1> velocity_per_obj_args{{{0, 0x15C, 4}}};
		constexpr std::array<velocity_arg, 7> velocity_stable_args{{{6, 0x86, 1}, {7, 0x87, 1}, {8, 0x88, 1}, {9, 0x89, 1}, {10, 0x8A, 1}, {34, 0x6, 1}, {42, 0x58, 1}}};

		const std::vector<velocity_technique> velocity_techniques
		{
			{zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID, "zprepass_velocity_rigid",
				"vertcol_simple_zp_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, false, {{0, 0x16C, 4}, {6, 0x168, 4}}},
			{zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED, "zprepass_velocity_skinned",
				"vertcol_simple_zp_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, true, {{0, 0x16C, 4}, {6, 0x168, 4}, {10, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID, "zprepass_hidir_velocity_rigid",
				"vertcol_simple_zp_hidir_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, false, {{0, 0x16C, 4}, {4, 0x149, 1}, {6, 0x168, 4}}},
			{zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED, "zprepass_hidir_velocity_skinned",
				"vertcol_simple_zp_hidir_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, true, {{0, 0x16C, 4}, {4, 0x149, 1}, {6, 0x168, 4}, {10, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_VELOCITY_RIGID, "velocity_rigid_co_sm",
				"velocity_sm_co_vrigid.hlsl", "velocity_sm_co_vrigid.hlsl", false, false, {{0, 0x16C, 4}, {6, 0x168, 4}}},
			{zonetool::s2::TECHNIQUE_VELOCITY_SKINNED, "velocity_skinned_co_sm",
				"velocity_sm_co_vskinned.hlsl", "velocity_sm_co_vrigid.hlsl", false, true, {{0, 0x16C, 4}, {6, 0x168, 4}, {10, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_INSTANCED_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID, "instanced_zprepass_velocity_rigid",
				"vertcol_simple_zp_i_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, false, {}},
			{zonetool::s2::TECHNIQUE_INSTANCED_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED, "instanced_zprepass_velocity_skinned",
				"vertcol_simple_zp_i_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, true, {{0, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_INSTANCED_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID, "instanced_zprepass_hidir_velocity_rigid",
				"vertcol_simple_zp_i_hidir_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, false, {}},
			{zonetool::s2::TECHNIQUE_INSTANCED_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED, "instanced_zprepass_hidir_velocity_skinned",
				"vertcol_simple_zp_i_hidir_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, true, {{0, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_NO_TESS_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID, "no_tess_zprepass_velocity_rigid",
				"vertcol_simple_zp_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, false, {{0, 0x16C, 4}, {6, 0x168, 4}}},
			{zonetool::s2::TECHNIQUE_NO_TESS_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED, "no_tess_zprepass_velocity_skinned",
				"vertcol_simple_zp_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_vel_nc_noatest_dso.hlsl", false, true, {{0, 0x16C, 4}, {6, 0x168, 4}, {10, 0x14, 1}}},
			{zonetool::s2::TECHNIQUE_NO_TESS_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID, "no_tess_zprepass_hidir_velocity_rigid",
				"vertcol_simple_zp_hidir_rvel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, false, {{0, 0x16C, 4}, {4, 0x149, 1}, {6, 0x168, 4}}},
			{zonetool::s2::TECHNIQUE_NO_TESS_BEGIN + zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED, "no_tess_zprepass_hidir_velocity_skinned",
				"vertcol_simple_zp_hidir_svel_nc_noatest_dso.hlsl", "vertcol_simple_zp_hidir_vel_nc_noatest_dso.hlsl", true, true, {{0, 0x16C, 4}, {4, 0x149, 1}, {6, 0x168, 4}, {10, 0x14, 1}}},
		};

		template <typename T>
		T* global_shader(const char* name, utils::memory::allocator& allocator)
		{
			static std::unordered_map<std::string_view, T*> shaders;
			auto& shader = shaders[name];
			if (!shader)
			{
				shader = make_stub<T>(name, allocator);
				add_global_shader(shader);
			}

			return shader;
		}

		void add_velocity_args(const std::span<const velocity_arg> group, std::vector<zonetool::s2::MaterialShaderArgument>& args,
			unsigned char& count, unsigned short& size)
		{
			auto rows = 0u;
			for (const auto& arg : group)
			{
				auto& result = args.emplace_back();
				result.type = zonetool::s2::MTL_ARG_CODE_CONST;
				result.shader = static_cast<unsigned char>(MTL_SHADER_STAGE_VERTEX);
				result.dest = arg.dest;
				result.u.codeConst.index = arg.code_const;
				result.u.codeConst.rowCount = arg.row_count;
				rows = std::max(rows, static_cast<std::uint32_t>(arg.dest + arg.row_count));
			}

			count = static_cast<unsigned char>(group.size());
			size = arg_size(0, rows);
		}

		zonetool::s2::MaterialTechnique* make_velocity_technique(const velocity_technique& velocity)
		{
			static utils::memory::allocator allocator;
			const auto instanced = velocity.slot >= zonetool::s2::TECHNIQUE_INSTANCED_BEGIN && velocity.slot < zonetool::s2::TECHNIQUE_NO_TESS_BEGIN;

			auto* technique = allocator.allocate<zonetool::s2::MaterialTechnique>();
			technique->hdr.name = velocity.name;
			technique->hdr.passCount = 1;

			auto& pass = technique->passArray[0];
			pass.vertexShader = global_shader<zonetool::s2::MaterialVertexShader>(velocity.vertex_shader, allocator);
			pass.vertexDecl = global_shader<zonetool::s2::MaterialVertexDeclaration>(velocity.hidir ? "pp.f.0nn.f.1" : "pp.f.0", allocator);
			pass.pixelShader = global_shader<zonetool::s2::MaterialPixelShader>(velocity.pixel_shader, allocator);
			pass.pixelOutputMask = static_cast<unsigned char>(velocity.hidir ? 3 : 1);

			std::vector<zonetool::s2::MaterialShaderArgument> args;
			add_velocity_args(velocity.per_prim_args, args, pass.perPrimArgCount, pass.perPrimArgSize);
			add_velocity_args(velocity_per_obj_args, args, pass.perObjArgCount, pass.perObjArgSize);
			add_velocity_args(velocity_stable_args, args, pass.stableArgCount, pass.stableArgSize);
			pass.args = copy_array(args.data(), args.size(), allocator);

			pass.customBufferFlags = zonetool::s2::CUSTOM_BUFFER_PER_OBJECT | zonetool::s2::CUSTOM_BUFFER_PER_STABLE;
			if (!velocity.per_prim_args.empty())
			{
				pass.customBufferFlags |= zonetool::s2::CUSTOM_BUFFER_PER_PRIM;
			}
			if (instanced)
			{
				pass.customBufferFlags |= zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES;
			}
			if (velocity.skinned)
			{
				pass.customBufferFlags |= zonetool::s2::CUSTOM_BUFFER_SKINNED;
			}

			pass.modelVertexType = instanced ? zonetool::s2::MODEL_VERTEX_TYPE_INSTANCE_DATA
				: velocity.skinned ? zonetool::s2::MODEL_VERTEX_TYPE_SURFACE : zonetool::s2::MODEL_VERTEX_TYPE_PREV_FRAME;
			return technique;
		}

		zonetool::s2::MaterialTechnique* find_velocity_technique(const std::int32_t slot)
		{
			static std::unordered_map<std::int32_t, zonetool::s2::MaterialTechnique*> techniques;
			if (const auto entry = techniques.find(slot); entry != techniques.end())
			{
				return entry->second;
			}

			auto& result = techniques[slot];
			const auto velocity = std::ranges::find(velocity_techniques, slot, &velocity_technique::slot);
			if (velocity != velocity_techniques.end())
			{
				result = make_velocity_technique(*velocity);
			}

			return result;
		}
	}

	bool use_stock_velocity_technique(const MaterialTechniqueSet* techset, const std::int32_t slot, native_techset& result)
	{
		const auto type = velocity_source_type(slot % zonetool::s2::TECHNIQUE_GROUP_SIZE);
		if (!type)
		{
			return false;
		}

		const auto source = find_group_technique(techset, slot - slot % zonetool::s2::TECHNIQUE_GROUP_SIZE, *type);
		if (source < 0 || uses_material_arguments(techset->techniques[source]))
		{
			return false;
		}

		auto* technique = find_velocity_technique(slot);
		if (!technique)
		{
			return false;
		}

		result.techset->techniques[slot] = technique;
		result.sources[slot] = source;
		return true;
	}
}
