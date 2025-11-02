#include <std_include.hpp>

#include "tokens.hpp"
#include "writer.hpp"

namespace shader::asm_::tokens
{
	opcode_t create_opcode(const std::uint32_t type, const std::uint32_t controls)
	{
		opcode_t opcode{};

		opcode.type = type;
		opcode.controls = controls;
		opcode.length = 1;

		return opcode;
	}

	instruction_t create_instruction(const std::uint32_t type, const std::uint32_t controls)
	{
		instruction_t inst{};

		inst.opcode = create_opcode(type, controls);

		return inst;
	}

	void add_operand(instruction_t& instruction, const operand_t& operand)
	{
		instruction.operands.emplace_back(operand);
		instruction.opcode.length += writer::get_operand_length(operand);
	}

	operand_t create_literal_operand(const float value)
	{
		operand_t operand{};

		operand.type = D3D10_SB_OPERAND_TYPE_IMMEDIATE32;

		operand.dimension = D3D10_SB_OPERAND_INDEX_0D;
		operand.extended = 0;

		operand.components.type = D3D10_SB_OPERAND_1_COMPONENT;

		operand.indices[0].values[0].f32 = value;

		return operand;
	}

	operand_t create_literal_operand(const float x, const float y, const float z, const float w)
	{
		operand_t operand{};

		operand.type = D3D10_SB_OPERAND_TYPE_IMMEDIATE32;

		operand.dimension = D3D10_SB_OPERAND_INDEX_0D;
		operand.extended = 0;

		operand.components.type = D3D10_SB_OPERAND_4_COMPONENT;
		operand.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
		operand.components.mask = 0;

		operand.indices[0].values[0].f32 = x;
		operand.indices[0].values[1].f32 = y;
		operand.indices[0].values[2].f32 = z;
		operand.indices[0].values[3].f32 = w;

		return operand;
	}

	std::uint32_t get_swizzle_component(const char c)
	{
		static std::unordered_map<char, std::uint32_t> component_name_map =
		{
			{'x', D3D10_SB_4_COMPONENT_X},
			{'y', D3D10_SB_4_COMPONENT_Y},
			{'z', D3D10_SB_4_COMPONENT_Z},
			{'w', D3D10_SB_4_COMPONENT_W},
			{'r', D3D10_SB_4_COMPONENT_R},
			{'g', D3D10_SB_4_COMPONENT_G},
			{'b', D3D10_SB_4_COMPONENT_B},
			{'a', D3D10_SB_4_COMPONENT_A},
		};

		const auto iter = component_name_map.find(c);
		if (iter == component_name_map.end())
		{
			throw std::runtime_error("invalid swizzle component");
		}

		return iter->second;
	}

	operand_t create_operand(const std::uint32_t type, const operand_components_t& operand_components, const std::vector<std::uint32_t>& indices)
	{
		if (indices.size() > 3)
		{
			throw std::runtime_error("create_operand: invalid args");
		}

		operand_t operand{};

		operand.type = type;

		operand.dimension = static_cast<std::uint32_t>(indices.size());
		operand.extended = 0;

		operand.components = operand_components;

		for (auto i = 0u; i < indices.size(); i++)
		{
			operand.indices[i].values[0].u32 = indices[i];
		}

		return operand;
	}

	operand_t create_operand(const std::uint32_t type, const std::vector<std::uint32_t>& components, const std::vector<std::uint32_t>& indices)
	{
		if (components.size() > 4)
		{
			throw std::runtime_error("create_src_operand_swizzle: invalid args");
		}

		operand_components_t operand_components{};
		operand_components.type = D3D10_SB_OPERAND_4_COMPONENT;
		operand_components.mask = 0;

		if (components.size() == 1)
		{
			operand_components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE;
		}
		else
		{
			operand_components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE;
		}

		for (auto i = 0u; i < components.size(); i++)
		{
			operand_components.names[i] = components[i];
		}

		return create_operand(type, operand_components, indices);
	}

	operand_t create_operand(const std::uint32_t type, const std::vector<D3D10_SB_4_COMPONENT_NAME>& components, const std::vector<std::uint32_t>& indices)
	{
		std::vector<std::uint32_t> components_;
		for (auto i = 0u; i < components.size(); i++)
		{
			components_[i] = components[i];
		}

		return create_operand(type, components_, indices);
	}

	operand_t create_operand(const std::uint32_t type, const std::string& component_names, const std::vector<std::uint32_t>& indices)
	{
		if (component_names.size() > 4)
		{
			throw std::runtime_error("create_swizzle_components: invalid args");
		}

		operand_components_t operand_components{};

		operand_components.type = D3D10_SB_OPERAND_4_COMPONENT;
		operand_components.mask = 0;

		if (component_names.size() == 1)
		{
			operand_components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE;
		}
		else
		{
			operand_components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE;
		}

		for (auto i = 0u; i < component_names.size(); i++)
		{
			operand_components.names[i] = get_swizzle_component(component_names[i]);
		}

		return create_operand(type, operand_components, indices);
	}

	operand_t create_operand(const std::uint32_t type, const std::uint32_t component_mask, const std::vector<std::uint32_t>& indices)
	{
		operand_components_t operand_components{};
		operand_components.type = D3D10_SB_OPERAND_4_COMPONENT;
		operand_components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
		operand_components.mask = component_mask; // e.g. X|Y|Z|W

		return create_operand(type, operand_components, indices);
	}

	std::vector<std::uint32_t> find_operands(const instruction_t& instruction, const std::uint32_t beg,
		const std::function<bool(const operand_t&)>& cb)
	{
		std::vector<std::uint32_t> result;

		if (beg >= instruction.operands.size())
		{
			return result;
		}

		for (auto i = beg; i < instruction.operands.size(); i++)
		{
			if (cb(instruction.operands[i]))
			{
				result.emplace_back(i);
			}
		}

		return result;
	}
}
