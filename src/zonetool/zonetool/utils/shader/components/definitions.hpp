#pragma once

#include "../../d3d11TokenizedProgramFormat.hpp"
#include <utils/bit_buffer.hpp>

namespace shader::asm_
{
	constexpr auto component_x = 1 << D3D10_SB_4_COMPONENT_X;
	constexpr auto component_y = 1 << D3D10_SB_4_COMPONENT_Y;
	constexpr auto component_z = 1 << D3D10_SB_4_COMPONENT_Z;
	constexpr auto component_w = 1 << D3D10_SB_4_COMPONENT_W;
	constexpr auto component_all = component_x | component_y | component_z | component_w;

	union operand_index_data_t
	{
		struct u64_fields_t
		{
			std::uint32_t low;
			std::uint32_t high;
		};

		union u64_t
		{
			u64_fields_t fields;
			std::uint64_t value;
		};

		std::uint32_t u32;
		float f32;
		u64_t u64;
	};

	struct operand_index_t
	{
		std::uint32_t representation;
		operand_index_data_t values[4];
	};

	struct operand_components_t
	{
		std::uint32_t type;
		std::uint32_t selection_mode;
		std::uint32_t mask;
		std::uint32_t names[4];
	};

	struct operand_extended_t
	{
		std::uint32_t type;
		std::uint32_t modifier;
		bool extended;
	};

	struct operand_t;

	enum operand_custom_type
	{
		operand_dcl_temps0,
		operand_resource_return_type0,
	};

	struct operand_dcl_temps_t
	{
		std::uint32_t size;
	};

	struct operand_custom_t
	{
		union
		{
			std::uint32_t value;
			std::uint32_t values[4];
		} u;
	};

	struct operand_t
	{
		std::uint32_t type;
		std::uint32_t dimension;
		std::uint32_t extended;
		std::vector<operand_extended_t> extensions;
		operand_index_t indices[3];
		operand_components_t components;
		std::shared_ptr<operand_t> extra_operand;
		operand_custom_t custom;
	};

	struct opcode_extended_t
	{
		std::uint32_t type;
		std::uint32_t values[4];
		bool extended;
	};

	struct opcode_t
	{
		std::uint32_t type;
		std::uint32_t controls;
		std::uint32_t length;
		bool extended;
		std::vector<opcode_extended_t> extensions;
	};

	union instruction_customdata_value_t
	{
		std::uint32_t u32;
		float f32;
	};

	struct instruction_customdata_t
	{
		std::uint32_t data_class;
		std::uint32_t count;
		std::vector<instruction_customdata_value_t> values;
	};

	struct instruction_t
	{
		opcode_t opcode;
		std::vector<operand_t> operands;
		instruction_customdata_t customdata;
	};

	extern std::unordered_map<std::uint32_t, const char*> opcode_names;

	const char* get_resource_dimension_name(const std::uint32_t dimension);
	const char* get_return_type_name(const std::uint32_t type);
	const char* get_name_token(const std::uint32_t type);
}
