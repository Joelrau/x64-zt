#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "layout.hpp"
#include "light_grid.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		constexpr auto light_grid_direction_count = 12u;

		constexpr auto light_grid_volume_slot = 7u;
		constexpr auto light_grid_probe_colors_slot = 8u;
		constexpr auto light_grid_probe_visibility_slot = 9u;

		constexpr std::array<std::array<float, 3>, light_grid_direction_count> light_grid_directions
		{{
			{0.850651f, 0.525731f, 0.0f},
			{-0.850651f, 0.525731f, 0.0f},
			{0.850651f, -0.525731f, 0.0f},
			{-0.850651f, -0.525731f, 0.0f},
			{0.0f, 0.850651f, 0.525731f},
			{0.0f, -0.850651f, 0.525731f},
			{0.0f, 0.850651f, -0.525731f},
			{0.0f, -0.850651f, -0.525731f},
			{0.525731f, 0.0f, 0.850651f},
			{-0.525731f, 0.0f, 0.850651f},
			{0.525731f, 0.0f, -0.850651f},
			{-0.525731f, 0.0f, -0.850651f},
		}};

		bool is_light_grid_slot(const std::uint32_t slot)
		{
			return (slot >= light_grid_volume_slot && slot <= light_grid_probe_visibility_slot) || (slot >= forward_lights_slot && slot <= light_tile_zbins_slot);
		}

		void emit_light_grid_coords(alys::shader::assembler& a, const program_patch& patch, const std::uint32_t input,
			const std::uint32_t volume, const std::uint32_t coords, const std::uint32_t probe, const std::uint32_t weight)
		{
			const auto texture = light_grid_volume_slot;

			a.add(asm_::r(volume).x(), asm_::v(input).w(), asm_::l(0.5f));
			a.ftou(asm_::r(volume).x(), asm_::r(volume).x());
			a.ld_structured(asm_::r(coords).xyz(), asm_::r(volume).x(), uint_literal(0), asm_::t(texture).select("xyzx"));
			a.ld_structured(asm_::r(probe).xyz(), asm_::r(volume).x(), uint_literal(16), asm_::t(texture).select("xyzx"));
			a.mov_sat(asm_::r(weight).xyz(), asm_::v(input).select("xyzx"));
			a.mad(asm_::r(coords).xyz(), asm_::r(weight).select("xyzx"), asm_::r(coords).select("xyzx"), asm_::r(probe).select("xyzx"));
			if (patch.grid == light_grid::probe_buffer)
			{
				a.add(asm_::r(coords).x(), asm_::r(coords).x(), asm_::l(0.5f));
				a.ftoi(asm_::r(coords).x(), asm_::r(coords).x());
			}
		}

		constexpr auto h1_scene_constant_buffer = 1u;
		constexpr std::array h1_sun_color_rows = {9u, 10u};

		bool is_sun_color(const alys::shader::detail::operand_t& operand)
		{
			return sun_color_index(operand).has_value();
		}
	}

	std::map<std::uint32_t, std::uint32_t> relocate_textures(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t lighting_texture)
	{
		constexpr auto texture_slot_count = 16u;

		std::set<std::uint32_t> used;
		for (const auto& instruction : instructions)
		{
			if (is_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_RESOURCE)
			{
				used.insert(instruction.operands[0].indices[0].value.uint32);
			}
		}

		used.erase(lighting_texture);

		std::vector<std::uint32_t> free;
		for (auto slot = 0u; slot < texture_slot_count; slot++)
		{
			if (!is_light_grid_slot(slot) && !used.contains(slot))
			{
				free.emplace_back(slot);
			}
		}

		std::map<std::uint32_t, std::uint32_t> result;
		auto next = free.begin();
		for (const auto slot : used)
		{
			if (!is_light_grid_slot(slot))
			{
				continue;
			}

			if (next == free.end())
			{
				throw std::runtime_error(utils::string::va("pixel shader has no free slot for t%u", slot));
			}

			result.emplace(slot, *next++);
		}

		return result;
	}

	vertex_lighting find_vertex_lighting(alys::shader::shader_object& shader, const model_lighting& lighting)
	{
		const auto& instructions = shader.get_instructions();
		const auto writes_coords = [&](const alys::shader::detail::instruction_t& instruction)
		{
			if (instruction.opcode.type != D3D10_SB_OPCODE_MOV || instruction.operands[0].type != D3D10_SB_OPERAND_TYPE_OUTPUT
				|| instruction.operands[1].type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
			{
				return false;
			}

			const auto& source = instruction.operands[1];
			if (lighting.instanced)
			{
				return source.indices[0].value.uint32 == h1_model_lighting_buffer;
			}

			return source.indices[0].value.uint32 == 0 && source.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32
				&& source.indices[1].value.uint32 == lighting.coords_dest;
		};

		const auto coords_write = std::ranges::find_if(instructions, writes_coords);
		if (coords_write == instructions.end() || std::find_if(coords_write + 1, instructions.end(), writes_coords) != instructions.end())
		{
			throw std::runtime_error("vertex shader does not write the model lighting coords once");
		}

		vertex_lighting result{static_cast<std::size_t>(coords_write - instructions.begin())};
		result.output = coords_write->operands[0].indices[0].value.uint32;
		result.semantic = find_signature_element(shader, alys::shader::chunk_osgn, [&](const signature_element& element)
		{
			return element.register_ == result.output;
		}, "model lighting output").semantic_index;

		const auto position = find_signature_element(shader, alys::shader::chunk_osgn, [](const signature_element& element)
		{
			return element.system_value_type == alys::shader::POS;
		}, "position output").register_;

		std::optional<std::size_t> position_write;
		for (auto i = 0u; i < result.coords_write; i++)
		{
			const auto& operands = instructions[i].operands;
			if (instructions[i].opcode.type == D3D10_SB_OPCODE_DP4 && operands[0].type == D3D10_SB_OPERAND_TYPE_OUTPUT
				&& operands[0].indices[0].value.uint32 == position && operands[1].type == D3D10_SB_OPERAND_TYPE_TEMP)
			{
				position_write = i;
			}
		}

		if (!position_write)
		{
			throw std::runtime_error("vertex shader writes no clip position before the model lighting coords");
		}

		result.position_write = *position_write;
		return result;
	}

	void emit_vertex_lighting(alys::shader::assembler& a, const alys::shader::detail::operand_t& position, const model_lighting& lighting,
		const std::optional<std::uint32_t> instance_input, const std::uint32_t temp)
	{
		const auto world = temp;
		const auto volume = temp + 1;
		const auto coords = temp + 2;

		a.add(asm_::r(world).xyz(), position, asm_::cb(s2_stable_constant_buffer, lighting.eye_offset_dest).select("xyzx"));
		a.mov(asm_::r(world).w(), asm_::l(1.0f));

		if (lighting.instanced)
		{
			a.imad(asm_::r(volume).x(), asm_::v(*instance_input).x(), uint_literal(s2_lit_instance_rows), uint_literal(s2_lit_instance_lighting_row));
			alys::shader::detail::operand_t index = asm_::cb(s2_instance_matrix_buffer, 0).select("xxxx").swz();
			index.indices[1].representation = D3D10_SB_OPERAND_INDEX_RELATIVE;
			index.indices[1].value.uint32 = 0;
			index.indices[1].extra_operand = std::make_shared<alys::shader::detail::operand_t>(asm_::r(volume).x().scalar());
			a.ftou(asm_::r(volume).x(), index);
		}
		else
		{
			a.ftou(asm_::r(volume).x(), asm_::cb(0, s2_model_lighting_index_dest).select("xxxx").swz());
		}

		a.ld_structured(asm_::r(volume).x(), asm_::r(volume).x(), uint_literal(0), asm_::t(light_grid_index_slot).select("xxxx"));
		for (auto row = 0u; row < 3; row++)
		{
			a.ld_structured(asm_::r(coords).select("xyzw"), asm_::r(volume).x(), uint_literal(row * 16), asm_::t(light_grid_transform_slot).select("xyzw"));
			a.dp4(asm_::r(volume).select(alys::shader::component_y + row), asm_::r(coords).select("xyzw"), asm_::r(world).select("xyzw"));
		}

		a.mov(asm_::r(coords).xyz(), asm_::r(volume).select("yzwy"));
		a.utof(asm_::r(coords).w(), asm_::r(volume).x());
	}

	void emit_light_grid_lookup(alys::shader::assembler& a, const program_patch& patch, const alys::shader::detail::operand_t& sampler,
		const std::uint32_t input, const std::uint32_t temp, const bool directional)
	{
		constexpr auto mean_directional_weight = 0.7f / 6.0f + 0.3f / 10.0f;

		const auto dir = temp;
		const auto volume = temp + 1;
		const auto coords = temp + 2;
		const auto probe = temp + 3;
		const auto weight = temp + 4;
		const auto color = temp + 5;

		emit_light_grid_coords(a, patch, input, volume, coords, probe, weight);

		a.mov(asm_::r(color).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
		for (auto i = 0u; i < light_grid_direction_count; i++)
		{
			const auto& direction = light_grid_directions[i];
			if (directional)
			{
				a.dp3_sat(asm_::r(weight).x(), asm_::r(dir).select("xyzx"), asm_::l(direction[0], direction[1], direction[2], 0.0f));
				a.mul(asm_::r(weight).x(), asm_::r(weight).x(), asm_::r(weight).x());
				a.mul(asm_::r(weight).y(), asm_::r(weight).x(), asm_::r(weight).x());
				a.mul(asm_::r(weight).y(), asm_::r(weight).y(), asm_::l(0.3f));
				a.mad(asm_::r(weight).x(), asm_::r(weight).x(), asm_::l(0.7f), asm_::r(weight).y());
			}
			else
			{
				a.mov(asm_::r(weight).x(), asm_::l(mean_directional_weight));
			}

			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.imad(asm_::r(probe).x(), asm_::r(coords).x(), uint_literal(light_grid_direction_count), uint_literal(i));
				a.ld_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xxxx"), asm_::t(light_grid_probe_colors_slot).select("xyzw"));
				break;
			case light_grid::array_2d:
				a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
				a.mov(asm_::r(probe).z(), asm_::l(static_cast<float>(i)));
				a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			case light_grid::volume_3d:
				a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
				a.mad(asm_::r(probe).z(), asm_::r(coords).z(), asm_::l(1.0f / light_grid_direction_count),
					asm_::l(static_cast<float>(i) / light_grid_direction_count));
				a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			}

			a.mad(asm_::r(color).xyz(), asm_::r(probe).select("xyzx"), asm_::r(weight).select("xxxx"), asm_::r(color).select("xyzx"));
		}

		switch (patch.grid)
		{
		case light_grid::probe_buffer:
			a.ld_indexable(asm_::r(probe).x(), asm_::r(coords).select("xxxx"), asm_::t(light_grid_probe_visibility_slot).select("xyzw"));
			break;
		case light_grid::array_2d:
			a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyxx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
			break;
		case light_grid::volume_3d:
			a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyzx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
			break;
		}

		a.mov(asm_::r(color).w(), asm_::r(probe).x());
	}

	void emit_probe_lookup(alys::shader::assembler& a, const alys::shader::detail::instruction_t& sample, const program_patch& patch,
		const std::uint32_t input, const std::uint32_t temp)
	{
		const auto dir = temp;
		const auto color = temp + 5;

		a.mad(asm_::r(dir).xyz(), asm_::v(input).select("xyzx"), asm_::l(-1.0f), sample.operands[1]);
		a.dp3(asm_::r(dir).w(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("xyzx"));
		a.rsq(asm_::r(dir).w(), asm_::r(dir).w());
		a.mul(asm_::r(dir).xyz(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("wwww"));

		emit_light_grid_lookup(a, patch, sample.operands[3], input, temp, true);

		alys::shader::detail::operand_t result = asm_::r(color).select("xyzw").swz();
		std::ranges::copy(sample.operands[2].components.names, result.components.names);
		a.mov(sample.operands[0], result);
	}

	std::optional<std::uint32_t> sun_color_index(const alys::shader::detail::operand_t& operand)
	{
		if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != h1_scene_constant_buffer
			|| operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
		{
			return {};
		}

		const auto row = std::ranges::find(h1_sun_color_rows, operand.indices[1].value.uint32);
		if (row == h1_sun_color_rows.end())
		{
			return {};
		}

		return static_cast<std::uint32_t>(row - h1_sun_color_rows.begin());
	}

	bool reads_sun_color(const std::vector<alys::shader::detail::instruction_t>& instructions)
	{
		return std::ranges::any_of(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return !is_declaration(instruction) && std::ranges::any_of(instruction.operands, is_sun_color);
		});
	}

	void emit_probe_sun_color(alys::shader::assembler& a, const program_patch& patch, const std::uint32_t input, const std::uint32_t temp)
	{
		// h1 draws a model without sun visibility with a sunless light env
		const auto gate = temp;
		const auto coords = temp + 4;

		emit_light_grid_coords(a, patch, input, temp + 1, coords, temp + 2, temp + 3);
		a.ld_indexable(asm_::r(gate).x(), asm_::r(coords).select("xxxx"), asm_::t(light_grid_probe_visibility_slot).select("xyzw"));
		a.lt(asm_::r(gate).x(), asm_::l(0.0f), asm_::r(gate).x());
		a.movc(asm_::r(gate).x(), asm_::r(gate).x(), asm_::l(1.0f), asm_::l(0.0f));
		for (auto i = 0u; i < h1_sun_color_rows.size(); i++)
		{
			const auto sun_color = asm_::cb(h1_scene_constant_buffer, h1_sun_color_rows[i]).select("xyzw");
			a.mul(asm_::r(temp + 1 + i).select("xyzw"), sun_color, asm_::r(gate).select("xxxx"));
		}
	}

	void declare_light_grid(alys::shader::assembler& a, const program_patch& patch)
	{
		a.dcl_resource_structured(asm_::t(light_grid_volume_slot), 32u);
		switch (patch.grid)
		{
		case light_grid::probe_buffer:
			a.dcl_resource(asm_::t(light_grid_probe_colors_slot), asm_::buffer, float4_type);
			a.dcl_resource(asm_::t(light_grid_probe_visibility_slot), asm_::buffer, float4_type);
			break;
		case light_grid::array_2d:
			a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture2darray, float4_type);
			a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture2d, float4_type);
			break;
		case light_grid::volume_3d:
			a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture3d, float4_type);
			a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture3d, float4_type);
			break;
		}
	}
}
