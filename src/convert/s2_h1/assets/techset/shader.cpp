#include <std_include.hpp>

#include "../../include.hpp"
#include "instancing.hpp"
#include "shader.hpp"

#include <formats/dxbc.hpp>

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		std::uint32_t convert_constant_buffer(const std::uint32_t slot)
		{
			switch (slot)
			{
			case 0:
			case 1:
				return slot;
			case 3:
			case 4:
			case 5:
				return slot - 1;
			case s2_instance_matrix_buffer:
				return 6u;
			default:
				throw std::runtime_error(utils::string::va("constant buffer cb%u has no h1 slot", slot));
			}
		}

		void remap_constant_buffers(alys::shader::detail::operand_t& operand)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					remap_constant_buffers(*index.extra_operand);
				}
			}

			if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
			{
				auto& slot = operand.indices[0].value.uint32;
				slot = convert_constant_buffer(slot);
			}
		}

		struct register_redirect
		{
			std::uint32_t from;
			std::uint32_t to;
			std::uint32_t component;
			bool rename;
			std::uint8_t source_mask;
			bool absolute;
		};

		void make_absolute(alys::shader::detail::operand_t& operand)
		{
			for (auto& extension : operand.extensions)
			{
				if (extension.type == D3D10_SB_EXTENDED_OPERAND_MODIFIER)
				{
					const auto negated = extension.modifier == D3D10_SB_OPERAND_MODIFIER_NEG || extension.modifier == D3D10_SB_OPERAND_MODIFIER_ABSNEG;
					extension.modifier = static_cast<std::uint8_t>(negated ? D3D10_SB_OPERAND_MODIFIER_ABSNEG : D3D10_SB_OPERAND_MODIFIER_ABS);
					return;
				}
			}

			alys::shader::detail::operand_extended_t modifier{};
			modifier.type = D3D10_SB_EXTENDED_OPERAND_MODIFIER;
			modifier.modifier = D3D10_SB_OPERAND_MODIFIER_ABS;
			operand.extensions.emplace_back(modifier);
		}

		void redirect_input(alys::shader::detail::operand_t& operand, const register_redirect& redirect)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					redirect_input(*index.extra_operand, redirect);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_INPUT)
			{
				return;
			}

			if (operand.indices[0].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				throw std::runtime_error("vertex shader indexes its inputs");
			}

			if (operand.indices[0].value.uint32 != redirect.from)
			{
				return;
			}

			operand.indices[0].value.uint32 = redirect.to;
			if (redirect.absolute)
			{
				make_absolute(operand);
			}

			auto& components = operand.components;
			switch (components.selection_mode)
			{
			case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
				components.mask = static_cast<std::uint8_t>(components.mask << redirect.component);
				break;
			case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
				for (auto& name : components.names)
				{
					name = static_cast<std::uint8_t>(name + redirect.component);
				}
				break;
			default:
				components.names[0] = static_cast<std::uint8_t>(components.names[0] + redirect.component);
				break;
			}
		}

		std::vector<register_redirect> redirect_signature(alys::shader::shader_object& shader, const std::vector<input_redirect>& redirects)
		{
			std::vector<register_redirect> result;
			if (redirects.empty())
			{
				return result;
			}

			auto& inputs = shader.get_signatures()[alys::shader::chunk_isgn];
			for (const auto& redirect : redirects)
			{
				const auto source = std::ranges::find_if(inputs, [&](const dxbc::signature_element& element)
				{
					return element.name == "TEXCOORD" && element.semantic_index == redirect.texcoord;
				});

				if (source == inputs.end())
				{
					continue;
				}

				const auto target = std::ranges::find_if(inputs, [&](const dxbc::signature_element& element)
				{
					return element.name == redirect.target && element.semantic_index == redirect.target_index;
				});

				if ((source->mask << redirect.component) > 0xF)
				{
					throw std::runtime_error(utils::string::va("TEXCOORD%u does not fit in %s%u", redirect.texcoord, redirect.target.data(), redirect.target_index));
				}

				if (target == inputs.end())
				{
					if (redirect.target != "TEXCOORD")
					{
						throw std::runtime_error(utils::string::va("vertex shader reads TEXCOORD%u without %s%u", redirect.texcoord, redirect.target.data(), redirect.target_index));
					}

					source->semantic_index = redirect.target_index;
					source->mask <<= redirect.component;
					source->rw_mask <<= redirect.component;
					result.emplace_back(source->register_, source->register_, redirect.component, true, std::uint8_t{}, redirect.absolute);
					continue;
				}

				if (target->rw_mask & (source->mask << redirect.component))
				{
					throw std::runtime_error(utils::string::va("vertex shader reads the %s%u components of TEXCOORD%u", redirect.target.data(), redirect.target_index, redirect.texcoord));
				}

				target->mask |= source->mask << redirect.component;
				target->rw_mask |= source->rw_mask << redirect.component;
				result.emplace_back(source->register_, target->register_, redirect.component, false, std::uint8_t{}, redirect.absolute);
				inputs.erase(source);
			}

			return result;
		}

		bool is_input_of(const alys::shader::detail::instruction_t& instruction, const std::uint32_t input)
		{
			return dxbc::is_input_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_INPUT
				&& instruction.operands[0].indices[0].value.uint32 == input;
		}
	}

	void decode_srgb_samples(instruction_list& instructions, const std::set<std::uint32_t>& textures)
	{
		auto temps = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS;
		});

		if (temps == instructions.end())
		{
			throw std::runtime_error("srgb decode in a shader without temps");
		}

		const auto high = temps->operands[0].custom.u.value;
		const auto low = high + 1;
		const auto select = high + 2;
		temps->operands[0].custom.u.value = high + 3;

		const auto make = [](const std::uint32_t opcode, std::vector<alys::shader::detail::operand_t> operands)
		{
			alys::shader::detail::instruction_t instruction{};
			instruction.opcode = alys::shader::detail::create_opcode(opcode);
			instruction.operands = std::move(operands);
			return instruction;
		};

		instruction_list result;
		for (auto& instruction : instructions)
		{
			result.emplace_back(instruction);
			if (dxbc::is_declaration(instruction) || instruction.operands.size() < 3 || instruction.operands[0].type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				continue;
			}

			const auto& resource = instruction.operands[2];
			const auto sampled = std::ranges::any_of(textures, [&](const std::uint32_t texture)
			{
				return dxbc::is_sample(instruction, texture) || (instruction.opcode.type == D3D10_SB_OPCODE_SAMPLE_L && dxbc::is_resource(resource, texture));
			});

			if (!sampled)
			{
				continue;
			}

			const auto& dest = instruction.operands[0];
			std::uint8_t mask{};
			for (auto component = 0u; component < 4; component++)
			{
				if ((dest.components.mask & (1u << component)) && resource.components.names[component] < 3)
				{
					mask |= static_cast<std::uint8_t>(1u << component);
				}
			}

			if (!mask)
			{
				continue;
			}

			auto value = dest;
			value.components.mask = mask;
			value.extensions.clear();
			auto value_read = value;
			value_read.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE;
			for (auto component = 0u; component < 4; component++)
			{
				value_read.components.names[component] = static_cast<std::uint8_t>(component);
			}

			const auto temp = [&](const std::uint32_t index, const bool write)
			{
				auto operand = write ? value : value_read;
				operand.indices[0].value.uint32 = index;
				return operand;
			};

			result.emplace_back(make(D3D10_SB_OPCODE_MAD, {temp(high, true), value_read, asm_::l(0.947867f, 0.947867f, 0.947867f, 0.947867f), asm_::l(0.0521327f, 0.0521327f, 0.0521327f, 0.0521327f)}));
			result.emplace_back(make(D3D10_SB_OPCODE_LOG, {temp(high, true), temp(high, false)}));
			result.emplace_back(make(D3D10_SB_OPCODE_MUL, {temp(high, true), temp(high, false), asm_::l(2.4f, 2.4f, 2.4f, 2.4f)}));
			result.emplace_back(make(D3D10_SB_OPCODE_EXP, {temp(high, true), temp(high, false)}));
			result.emplace_back(make(D3D10_SB_OPCODE_MUL, {temp(low, true), value_read, asm_::l(0.0773994f, 0.0773994f, 0.0773994f, 0.0773994f)}));
			result.emplace_back(make(D3D10_SB_OPCODE_LT, {temp(select, true), asm_::l(0.04045f, 0.04045f, 0.04045f, 0.04045f), value_read}));
			result.emplace_back(make(D3D10_SB_OPCODE_MOVC, {value, temp(select, false), temp(high, false), temp(low, false)}));
		}

		instructions = std::move(result);
	}

	void set_sun_mode(instruction_list& instructions, const sun_direction& sun)
	{
		auto temps = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS;
		});

		if (temps == instructions.end())
		{
			throw std::runtime_error("sun mode in a shader without temps");
		}

		const auto temp = temps->operands[0].custom.u.value;
		temps->operands[0].custom.u.value = temp + 1;

		const std::function<void(alys::shader::detail::operand_t&)> redirect = [&](alys::shader::detail::operand_t& operand)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					redirect(*index.extra_operand);
				}
			}

			if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == sun.buffer
				&& operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32 && operand.indices[1].value.uint32 == sun.row)
			{
				operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
				operand.dimension = D3D10_SB_OPERAND_INDEX_1D;
				operand.indices[0].value.uint32 = temp;
				operand.indices[1] = {};
			}
		};

		auto first = instructions.end();
		for (auto it = instructions.begin(); it != instructions.end(); ++it)
		{
			if (dxbc::is_declaration(*it))
			{
				continue;
			}

			if (first == instructions.end())
			{
				first = it;
			}

			for (auto& operand : it->operands)
			{
				redirect(operand);
			}
		}

		if (first == instructions.end())
		{
			return;
		}

		const auto make = [](std::vector<alys::shader::detail::operand_t> operands)
		{
			alys::shader::detail::instruction_t instruction{};
			instruction.opcode = alys::shader::detail::create_opcode(D3D10_SB_OPCODE_MOV);
			instruction.operands = std::move(operands);
			return instruction;
		};

		const auto position = first - instructions.begin();
		instructions.insert(instructions.begin() + position, {
			make({asm_::r(temp).select("xyz").mask(), asm_::cb(sun.buffer, sun.row).select("xyzx").swz()}),
			make({asm_::r(temp).w().mask(), asm_::l(sun.w)}),
		});
	}

	alys::shader::detail::operand_t swizzled_literal(const alys::shader::detail::operand_t& operand, const std::array<float, 4>& values)
	{
		const auto& names = operand.components.names;
		auto result = operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE
			? asm_::l(values[names[0]])
			: asm_::l(values[names[0]], values[names[1]], values[names[2]], values[names[3]]);
		result.extensions = operand.extensions;
		return result;
	}

	void patch_atmos_fog(instruction_list& instructions, const atmos_fog_rows& fog)
	{
		constexpr auto log2_e = 1.44269502f;
		const auto& rows = fog.rows;
		if ((rows[0] || rows[1] || rows[4]) && !rows[3])
		{
			throw std::runtime_error("atmos fog rows without ATMOS_FOG_PARMS_3");
		}

		auto temps = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS;
		});

		if (temps == instructions.end())
		{
			throw std::runtime_error("atmos fog in a shader without temps");
		}

		const auto first_temp = temps->operands[0].custom.u.value;
		temps->operands[0].custom.u.value = first_temp + 5;

		const std::function<void(alys::shader::detail::operand_t&)> redirect = [&](alys::shader::detail::operand_t& operand)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					redirect(*index.extra_operand);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != fog.buffer
				|| operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				return;
			}

			const auto row = std::ranges::find(rows, std::optional(operand.indices[1].value.uint32));
			const auto parm = static_cast<std::uint32_t>(row - rows.begin());
			if (parm < 5)
			{
				operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
				operand.dimension = D3D10_SB_OPERAND_INDEX_1D;
				operand.indices[0].value.uint32 = first_temp + parm;
				operand.indices[1] = {};
			}
			else if (parm == 7)
			{
				operand = swizzled_literal(operand, {0.0f, 1.0f, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()});
			}
		};

		auto first = instructions.end();
		for (auto it = instructions.begin(); it != instructions.end(); ++it)
		{
			if (dxbc::is_declaration(*it))
			{
				continue;
			}

			if (first == instructions.end())
			{
				first = it;
			}

			if (fog.lut_texture && dxbc::is_sample(*it, *fog.lut_texture))
			{
				alys::shader::detail::instruction_t zero{};
				zero.opcode = alys::shader::detail::create_opcode(D3D10_SB_OPCODE_MOV);
				zero.operands = {it->operands[0], dxbc::uint_literals(0, 0, 0, 0)};
				*it = std::move(zero);
				continue;
			}

			for (auto& operand : it->operands)
			{
				redirect(operand);
			}
		}

		if (first == instructions.end())
		{
			return;
		}

		alys::shader::shader_object scratch;
		auto a = scratch.get_assembler();
		const auto row = [&](const std::uint32_t parm) { return asm_::cb(fog.buffer, *rows[parm]); };
		const auto temp = [&](const std::uint32_t parm) { return asm_::r(first_temp + parm); };
		for (const auto parm : {0u, 1u})
		{
			if (rows[parm])
			{
				a.mul(temp(parm).select("xyz"), row(parm).select("xyzx").swz(), row(3).select("xyzx").swz());
				a.mov(temp(parm).w(), row(parm).select("wwww").swz());
			}
		}

		if (rows[2])
		{
			a.mul(temp(2).select("xyzw"), row(2).select("xyzw").swz(), asm_::l(log2_e, log2_e, log2_e, 1.0f));
		}

		if (rows[3])
		{
			a.mov(temp(3).select("xyz"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
			a.mov(temp(3).w(), row(3).select("wwww").swz());
		}

		if (rows[4])
		{
			a.mov(temp(4).select("xyz"), row(4).select("xyzx").swz());
			a.add(temp(4).w(), row(4).select("wwww").swz(), row(3).select("wwww").swz());
		}

		auto& code = scratch.get_instructions();
		instructions.insert(first, code.begin(), code.end());
	}

	std::map<std::uint32_t, std::uint32_t> plan_sampler_moves(const instruction_list& instructions, const std::set<std::uint32_t>& moved, const std::set<std::uint32_t>& reserved)
	{
		std::set<std::uint32_t> taken = reserved;
		std::set<std::uint32_t> declared;
		for (const auto& instruction : instructions)
		{
			if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_SAMPLER)
			{
				declared.insert(instruction.operands[0].indices[0].value.uint32);
			}
		}

		taken.insert(declared.begin(), declared.end());
		std::map<std::uint32_t, std::uint32_t> result;
		for (const auto slot : moved)
		{
			if (!declared.contains(slot))
			{
				continue;
			}

			auto target = 0u;
			while (taken.contains(target))
			{
				target++;
			}

			if (target >= 16u)
			{
				throw std::runtime_error(utils::string::va("no free sampler slot for s%u", slot));
			}

			taken.insert(target);
			result.emplace(slot, target);
		}

		return result;
	}

	converted_program convert_program(const unsigned char* program, const unsigned int size, const std::vector<input_redirect>& redirects,
		const std::vector<constant_move>& moves, const std::optional<sun_shadow_request>& sun_shadow, const std::set<std::uint32_t>& srgb_textures,
		const std::optional<sun_direction>& sun, const std::optional<light_array_request>& lights,
		const std::set<std::uint32_t>& moved_samplers, const std::set<std::uint32_t>& reserved_samplers, const std::optional<atmos_fog_rows>& fog)
	{
		auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});
		auto registers = redirect_signature(shader, redirects);

		converted_program result{};
		auto& instructions = shader.get_instructions();
		convert_instancing(instructions);
		result.sampler_moves = plan_sampler_moves(instructions, moved_samplers, reserved_samplers);
		for (auto& redirect : registers)
		{
			const auto source = std::ranges::find_if(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return is_input_of(instruction, redirect.from);
			});

			redirect.source_mask = source != instructions.end() ? source->operands[0].components.mask : std::uint8_t{};
		}

		std::vector<alys::shader::detail::instruction_t> converted;
		for (auto& instruction : instructions)
		{
			const auto removed = std::ranges::any_of(registers, [&](const register_redirect& redirect)
			{
				return !redirect.rename && is_input_of(instruction, redirect.from);
			});

			if (removed)
			{
				continue;
			}

			const auto is_buffer_declaration = instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER;
			for (auto& operand : instruction.operands)
			{
				remap_constant_buffers(operand);
				if (operand.type == D3D10_SB_OPERAND_TYPE_SAMPLER)
				{
					if (const auto move = result.sampler_moves.find(operand.indices[0].value.uint32); move != result.sampler_moves.end())
					{
						operand.indices[0].value.uint32 = move->second;
					}
				}

				for (const auto& redirect : registers)
				{
					if (redirect.rename || !is_input_of(instruction, redirect.to))
					{
						redirect_input(operand, redirect);
					}
				}

				for (const auto& move : moves)
				{
					if (!is_buffer_declaration)
					{
						dxbc::relocate_constant_row(operand, move.buffer, move.from, move.to, 2u);
					}
				}
			}

			for (const auto& redirect : registers)
			{
				if (!redirect.rename && is_input_of(instruction, redirect.to))
				{
					instruction.operands[0].components.mask |= static_cast<std::uint8_t>(redirect.source_mask << redirect.component);
				}
			}

			if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER)
			{
				result.constant_buffers.insert(instruction.operands[0].indices[0].value.uint32);
			}

			converted.emplace_back(std::move(instruction));
		}

		if (!moves.empty())
		{
			const auto rows = std::ranges::max(moves, {}, &constant_move::to).to + 1;
			const auto stable = std::ranges::find_if(converted, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == 2u;
			});

			if (stable != converted.end())
			{
				auto& stable_rows = stable->operands[0].indices[1].value.uint32;
				stable_rows = std::max(stable_rows, rows);
			}
			else
			{
				const auto first = std::ranges::find_if(converted, [](const alys::shader::detail::instruction_t& instruction)
				{
					return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER;
				});

				if (first == converted.end())
				{
					throw std::runtime_error("shader moves constant rows without a constant buffer");
				}

				auto stable_declaration = *first;
				stable_declaration.operands[0].indices[0].value.uint32 = 2u;
				stable_declaration.operands[0].indices[1].value.uint32 = rows;
				converted.insert(first + 1, stable_declaration);
				result.constant_buffers.insert(2u);
			}
		}

		if (sun_shadow)
		{
			result.sun_shadow = patch_sun_shadow(shader, converted, *sun_shadow);
		}

		if (!srgb_textures.empty())
		{
			decode_srgb_samples(converted, srgb_textures);
		}

		if (sun)
		{
			set_sun_mode(converted, *sun);
		}

		if (fog)
		{
			patch_atmos_fog(converted, *fog);
		}

		if (lights)
		{
			const auto fed = feed_light_array(shader, converted, *lights);
			result.light_array = fed.fed;
			result.light_array_shadows = fed.shadows;
		}

		if (shader.get_info().program_type == D3D10_SB_PIXEL_SHADER)
		{
			result.light_loops = remove_light_loops(converted);
			if (result.light_loops)
			{
				remove_dead_code(converted);
			}
		}

		result.usage = collect_usage(converted);
		instructions = std::move(converted);
		result.data = shader.serialize();
		return result;
	}
}
