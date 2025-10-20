#pragma once

#include <utils/bit_buffer.hpp>
#include <utils/memory.hpp>
#include "d3d11TokenizedProgramFormat.hpp"

namespace shader
{
	namespace fxcd
	{
		bool DxbcToAsmWithFxcd(const void* dxbc, size_t size,
			std::wstring fxcdPath,
			std::wstring disassemblerMode,
			std::wstring extraFlags,
			std::string& asmOut,
			std::string* logOut = nullptr);

		bool AsmToDxbcWithFxcd(const std::string& asmText,
			std::wstring fxcdPath,
			std::wstring extraAssembleFlags,
			const void* originalDxbc,
			std::size_t originalDxbcSize,
			std::vector<uint8_t>& outDxbc,
			std::string* logOut = nullptr);
	}

	struct dx11_shader_header
	{
		unsigned char dxbc[4]; // "DXBC"
		unsigned int checksum[4];
		unsigned int unk_int; // always 1, sanity check?
		unsigned int program_size;
		unsigned int chunk_count;
		// unsigned int chunk_offsets[chunk_count];
		// unsigned char chunks[chunk_count][?];
	};

	struct shader_checksum
	{
		unsigned int cheksum[4];
	};

	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size);
	unsigned int calc_crc32(unsigned char* program, unsigned int program_size);

	namespace asm_
	{
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

		struct operand_dcl_temps_t
		{
			std::uint32_t size;
		};

		struct operand_custom_t
		{
			bool is_custom;
			std::uint32_t type;
			union
			{
				operand_dcl_temps_t dcl_temps;
			} types;
		};

		struct operand_t
		{
			std::uint32_t type;
			std::uint32_t dimension;
			std::uint32_t extended;
			std::vector<operand_extended_t> extensions;
			operand_index_t indices[3];
			operand_components_t components;
			operand_t* extra_operand;
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

		struct instruction_t
		{
			opcode_t opcode;
			std::vector<operand_t> operands;
		};

		using instruction_cb = std::function<bool(utils::bit_buffer_le&, instruction_t)>;

		namespace reader
		{
			operand_extended_t read_extended_operand(utils::bit_buffer_le& bit_buffer);
			operand_t read_operand(utils::memory::allocator& allocator, utils::bit_buffer_le& bit_buffer);
			opcode_extended_t read_opcode_extended(utils::bit_buffer_le& input_buffer);
			opcode_t read_opcode(utils::bit_buffer_le& input_buffer);
			instruction_t read_instruction(utils::memory::allocator& allocator, utils::bit_buffer_le& input_buffer);
		}

		namespace writer
		{
			void write_operand(utils::bit_buffer_le& output_buffer, const operand_t& operand);
			void write_opcode_extended(utils::bit_buffer_le& output_buffer, const opcode_extended_t& opcode);
			void write_opcode(utils::bit_buffer_le& output_buffer, const opcode_t& opcode);
			std::uint32_t get_operand_length(const operand_t& operand);
		}

		namespace disassembler
		{
			void print_operand(const operand_t& op, bool last);
			void print_operands(const std::vector<operand_t>& operands);
			void print_instruction(const instruction_t& instruction);
		}

		namespace tokens
		{
			opcode_t create_opcode(const std::uint32_t type, const std::uint32_t controls);
			operand_t create_literal_operand(const float x, const float y, const float z, const float w);
			operand_t create_register_operand(const std::uint32_t type, const std::string& component_names, const std::vector<std::uint32_t>& indices);

			template <typename ...Args>
			operand_t create_register_operand(const std::uint32_t type, const std::string& component_names, Args&&... args)
			{
				return create_register_operand(type, component_names, {std::forward<Args>(args)...});
			}

			std::vector<std::uint32_t> find_operands(const instruction_t& instruction, const std::uint32_t beg,
				const std::function<bool(const operand_t&)>& cb);
		}
	}

	std::string patch_shader(unsigned char* program, unsigned int program_size, const asm_::instruction_cb& callback);
}
