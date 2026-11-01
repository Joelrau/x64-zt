#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "instancing.hpp"
#include "layout.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		bool reads_instance_row(const alys::shader::detail::operand_t& operand, const std::unordered_set<std::uint32_t>& instance_rows)
		{
			const auto& row = operand.indices[1];
			const auto is_relative = row.representation == D3D10_SB_OPERAND_INDEX_RELATIVE;
			const auto offset = is_relative ? 0u : row.value.uint32;
			if ((!is_relative && row.representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE) || offset >= (1u << h1_instance_row_shift)
				|| !row.extra_operand || row.extra_operand->type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				return false;
			}

			const auto& temp = *row.extra_operand;
			return instance_rows.contains(temp_component(temp.indices[0].value.uint32, temp.components.names[0]));
		}

		bool is_instance_row(const alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input)
		{
			const auto& operands = instruction.operands;
			return instruction.opcode.type == D3D10_SB_OPCODE_ISHL && operands[0].type == D3D10_SB_OPERAND_TYPE_TEMP
				&& operands[1].type == D3D10_SB_OPERAND_TYPE_INPUT && operands[1].indices[0].value.uint32 == instance_input
				&& operands[2].type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32 && operands[2].immediate_values[0].uint32 == h1_instance_row_shift;
		}
	}

	bool declares_instance_matrices(const std::vector<alys::shader::detail::instruction_t>& instructions)
	{
		return std::ranges::any_of(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_instance_matrix_buffer;
		});
	}

	std::optional<std::uint32_t> find_instance_input(const std::vector<alys::shader::detail::instruction_t>& instructions)
	{
		const auto input = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV && instruction.operands[1].custom.u.value == D3D10_SB_NAME_INSTANCE_ID;
		});

		return input != instructions.end() ? std::optional(input->operands[0].indices[0].value.uint32) : std::nullopt;
	}

	bool reads_instance_matrices(const alys::shader::detail::operand_t& operand)
	{
		return operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == s2_instance_matrix_buffer;
	}

	void patch_instance_rows(alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input, const bool lit_rows,
		std::unordered_set<std::uint32_t>& instance_rows)
	{
		const auto reads_without_row = std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
		{
			return reads_instance_matrices(operand) && !reads_instance_row(operand, instance_rows);
		});

		if (reads_without_row)
		{
			throw std::runtime_error("shader reads the instance matrices without the instance row");
		}

		const auto& operands = instruction.operands;
		const auto first_source = std::ranges::find_if_not(operands, is_destination) - operands.begin();
		for (auto dest = 0; dest < first_source; dest++)
		{
			for (auto component = 0u; operands[dest].type == D3D10_SB_OPERAND_TYPE_TEMP && component < 4; component++)
			{
				if ((operands[dest].components.mask >> component) & 1)
				{
					instance_rows.erase(temp_component(operands[dest].indices[0].value.uint32, component));
				}
			}
		}

		if (!is_instance_row(instruction, instance_input))
		{
			return;
		}

		if (lit_rows)
		{
			instruction.opcode.type = D3D10_SB_OPCODE_IMAD;
			instruction.operands[2].immediate_values[0].uint32 = s2_lit_instance_rows;
			auto zero = instruction.operands[2];
			zero.immediate_values[0].uint32 = 0;
			instruction.operands.emplace_back(zero);
		}
		else
		{
			instruction.operands[2].immediate_values[0].uint32 = s2_instance_row_shift;
		}

		for (auto component = 0u; component < 4; component++)
		{
			if ((operands[0].components.mask >> component) & 1)
			{
				instance_rows.insert(temp_component(operands[0].indices[0].value.uint32, component));
			}
		}
	}

	std::string instance_program(const unsigned char* program, const unsigned int size, const std::optional<std::uint32_t> coords_row)
	{
		constexpr auto h1_instance_matrix_rows = 512u;
		constexpr auto h1_model_lighting_rows = 128u;

		auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});
		alys::shader::shader_object result;
		result.get_info() = shader.get_info();
		result.get_signatures() = shader.get_signatures();
		result.get_unknown_chunks() = shader.get_unknown_chunks();

		auto& signatures = result.get_signatures();
		const auto inputs = signatures.find(alys::shader::chunk_isgn);
		if (inputs == signatures.end() || inputs->second.empty())
		{
			throw std::runtime_error("vertex shader has no inputs to instance");
		}

		signature_element instance{};
		instance.name = "SV_InstanceID";
		instance.system_value_type = alys::shader::INSTID;
		instance.component_type = alys::shader::format_uint;
		instance.register_ = std::ranges::max(inputs->second, {}, &signature_element::register_).register_ + 1;
		instance.mask = 0x1;
		instance.rw_mask = 0x1;
		inputs->second.emplace_back(instance);

		const auto& instructions = shader.get_instructions();
		const auto first_code = std::ranges::find_if_not(instructions, is_declaration) - instructions.begin();
		const auto last_input = std::find_if(instructions.rbegin(), instructions.rend(), is_input_declaration);
		if (last_input == instructions.rend() || first_code == static_cast<std::ptrdiff_t>(instructions.size()))
		{
			throw std::runtime_error("vertex shader has no input declarations or code");
		}

		const auto last_input_index = instructions.rend() - last_input - 1;
		auto a = result.get_assembler();
		std::optional<std::uint32_t> temp;
		auto declares_world = false;
		for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(instructions.size()); i++)
		{
			auto instruction = instructions[i];
			if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS)
			{
				temp = instruction.operands[0].custom.u.value;
				instruction.operands[0].custom.u.value++;
			}

			if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == 0)
			{
				declares_world = true;
				instruction.opcode.controls |= D3D10_SB_CONSTANT_BUFFER_DYNAMIC_INDEXED;
				instruction.operands[0].indices[0].value.uint32 = h1_instance_matrix_buffer;
				instruction.operands[0].indices[1].value.uint32 = h1_instance_matrix_rows;
				a(instruction);
				if (coords_row)
				{
					instruction.operands[0].indices[0].value.uint32 = h1_model_lighting_buffer;
					instruction.operands[0].indices[1].value.uint32 = h1_model_lighting_rows;
					a(instruction);
				}

				continue;
			}

			if (i == first_code)
			{
				if (!declares_world)
				{
					throw std::runtime_error("vertex shader reads no world matrix to instance");
				}

				if (!temp)
				{
					temp = 0;
					a.dcl_temps(1);
				}

				a.ishl(asm_::r(*temp).x(), asm_::v(instance.register_).x(), uint_literal(h1_instance_row_shift));
				a.mov(asm_::r(*temp).y(), asm_::v(instance.register_).x());
			}

			for (auto& operand : instruction.operands)
			{
				if (i < first_code || operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != 0)
				{
					continue;
				}

				if (operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
				{
					throw std::runtime_error("vertex shader indexes the per prim constants");
				}

				const auto row = operand.indices[1].value.uint32;
				const auto is_coords = coords_row && row == *coords_row;
				if (row >= (1u << h1_instance_row_shift) && !is_coords)
				{
					throw std::runtime_error(utils::string::va("per prim row cb0[%u] has no instanced source", row));
				}

				operand.indices[0].value.uint32 = is_coords ? h1_model_lighting_buffer : h1_instance_matrix_buffer;
				operand.indices[1].representation = D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE;
				operand.indices[1].value.uint32 = is_coords ? 0 : row;
				operand.indices[1].extra_operand = std::make_shared<alys::shader::detail::operand_t>(
					is_coords ? asm_::r(*temp).y().scalar() : asm_::r(*temp).x().scalar());
			}

			a(instruction);
			if (i == last_input_index)
			{
				a.dcl_input_sgv(asm_::v(instance.register_).x(), asm_::instance_id);
			}
		}

		return result.serialize();
	}
}
