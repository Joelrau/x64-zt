#include <std_include.hpp>

#include "shader.hpp"

#include "components/reader.hpp"
#include "components/writer.hpp"
#include "components/disassembler.hpp"

#include "instructions/generic.hpp"
#include "instructions/dcl_resource.hpp"
#include "instructions/dcl_globalflags.hpp"
#include "instructions/customdata.hpp"

#include <zlib.h>
#include "../dxbc_checksum.hpp"
#include <utils/io.hpp>

namespace shader
{
	namespace
	{
		unsigned char* parse_shader_chunk(unsigned char* program, unsigned int program_size, unsigned int* chunk_size)
		{
			const auto header = reinterpret_cast<dx11_shader_header*>(program);

			auto shader_chunk_offset = 0;
			for (auto i = 0u; i < header->chunk_count; i++)
			{
				const auto offset = *reinterpret_cast<int*>(
					reinterpret_cast<size_t>(program) + sizeof(dx11_shader_header) + sizeof(int) * i);

				const auto type = *reinterpret_cast<int*>(reinterpret_cast<size_t>(program) + offset);
				if (type == 'RDHS' || type == 'XEHS')
				{
					shader_chunk_offset = offset;
				}

				if (i >= header->chunk_count - 1)
				{
					*chunk_size = header->program_size - shader_chunk_offset;
				}
				else
				{
					const auto next_offset = *reinterpret_cast<int*>(
						reinterpret_cast<size_t>(program) + sizeof(dx11_shader_header) + sizeof(int) * (i + 1));
					*chunk_size = next_offset - shader_chunk_offset;
				}
			}

			return reinterpret_cast<unsigned char*>(reinterpret_cast<size_t>(program) + shader_chunk_offset);
		}

		unsigned long get_crc32(void* data_address, std::uint32_t data_len)
		{
			unsigned long crc = crc32(0L, Z_NULL, 0);
			crc = crc32(crc, reinterpret_cast<const unsigned char*>(data_address), data_len);
			return crc;
		}

		std::string get_crc32_str(void* data_address, std::uint32_t data_len)
		{
			return std::to_string(get_crc32(data_address, data_len));
		}
	}

	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size)
	{
		shader_checksum checksum{};
		dxbc::CalculateDXBCChecksum(program, program_size, reinterpret_cast<DWORD*>(&checksum));
		return checksum;
	}

	unsigned int calc_crc32(unsigned char* program, unsigned int program_size)
	{
		if (program == nullptr)
		{
			return 0;
		}

		return get_crc32(program, program_size);
	}

	namespace asm_
	{
		namespace
		{
			std::array<std::unique_ptr<base_instruction>, D3D10_SB_NUM_OPCODES> instruction_handlers;

			template <typename T>
			class initializer final
			{
			public:
				initializer(T fn)
				{
					fn();
				}
			};

			template <typename T, typename... Args>
			void register_instruction_handler(const std::uint32_t type, Args&&... args)
			{
				instruction_handlers[type] = std::make_unique<T>(std::forward<Args>(args)...);
			}

			void initialize()
			{
				/* dx10.0 opcodes */

				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ADD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_AND);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_BREAK);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_BREAKC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CALL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CALLC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CASE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CONTINUE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CONTINUEC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_CUT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DEFAULT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DERIV_RTX);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DERIV_RTY);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DISCARD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DIV);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DP2);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DP3);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_DP4);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ELSE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_EMIT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_EMITTHENCUT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ENDIF);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ENDLOOP);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ENDSWITCH);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_EQ);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_EXP);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_FRC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_FTOI);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_FTOU);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_GE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IADD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IF);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IEQ);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IGE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ILT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IMAD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IMAX);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IMIN);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_IMUL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_INE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_INEG);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ISHL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ISHR);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ITOF);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LABEL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LD_MS);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LOG);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LOOP);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_LT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MAD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MIN);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MAX);
				register_instruction_handler<customdata>(D3D10_SB_OPCODE_CUSTOMDATA);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MOV);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MOVC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_MUL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_NE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_NOP);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_NOT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_OR);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_RESINFO);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_RET);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_RETC);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ROUND_NE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ROUND_NI);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ROUND_PI);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ROUND_Z);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_RSQ);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE_C);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE_C_LZ);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE_L);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE_D);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SAMPLE_B);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SQRT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SWITCH);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_SINCOS);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UDIV);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_ULT);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UGE);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UMUL);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UMAD);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UMAX);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UMIN);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_USHR);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_UTOF);
				register_instruction_handler<general_instruction>(D3D10_SB_OPCODE_XOR);

				/* dx10 declarations */

				register_instruction_handler<dcl_resource>(D3D10_SB_OPCODE_DCL_RESOURCE);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D10_SB_OPCODE_DCL_SAMPLER);
				register_instruction_handler<declaration_instruction<1, 1>>(D3D10_SB_OPCODE_DCL_INDEX_RANGE);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D10_SB_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D10_SB_OPCODE_DCL_GS_INPUT_PRIMITIVE);
				register_instruction_handler<declaration_instruction<0, 1>>(D3D10_SB_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D10_SB_OPCODE_DCL_INPUT);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_INPUT_SGV);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_INPUT_SIV);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D10_SB_OPCODE_DCL_INPUT_PS);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_INPUT_PS_SGV);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_INPUT_PS_SIV);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D10_SB_OPCODE_DCL_OUTPUT);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_OUTPUT_SGV);
				register_instruction_handler<declaration_instruction_nametoken>(D3D10_SB_OPCODE_DCL_OUTPUT_SIV);
				register_instruction_handler<declaration_instruction<0, 1>>(D3D10_SB_OPCODE_DCL_TEMPS);
				register_instruction_handler<declaration_instruction<0, 3>>(D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP);
				register_instruction_handler<dcl_globalflags>(D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS);

				/* dx10.1 opcodes */

				register_instruction_handler<general_instruction>(D3D10_1_SB_OPCODE_LOD);
				register_instruction_handler<general_instruction>(D3D10_1_SB_OPCODE_GATHER4);
				register_instruction_handler<general_instruction>(D3D10_1_SB_OPCODE_SAMPLE_POS);
				register_instruction_handler<general_instruction>(D3D10_1_SB_OPCODE_SAMPLE_INFO);

				/* dx11.0 opcodes */

				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_HS_DECLS);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_HS_CONTROL_POINT_PHASE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_HS_FORK_PHASE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_HS_JOIN_PHASE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_EMIT_STREAM);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_CUT_STREAM);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_EMITTHENCUT_STREAM);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_INTERFACE_CALL);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_BUFINFO);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DERIV_RTX_COARSE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DERIV_RTX_FINE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DERIV_RTY_COARSE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DERIV_RTY_FINE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_GATHER4_C);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_GATHER4_PO);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_GATHER4_PO_C);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_RCP);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_F32TOF16);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_F16TOF32);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_UADDC);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_USUBB);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_COUNTBITS);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_FIRSTBIT_HI);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_FIRSTBIT_LO);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_FIRSTBIT_SHI);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_UBFE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IBFE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_BFI);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_BFREV);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_SWAPC);

				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_LD_UAV_TYPED);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_STORE_UAV_TYPED);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_LD_RAW);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_STORE_RAW);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_LD_STRUCTURED);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_STORE_STRUCTURED);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_AND);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_OR);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_XOR);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_CMP_STORE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_IADD);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_IMAX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_IMIN);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_UMAX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ATOMIC_UMIN);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_ALLOC);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_CONSUME);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_IADD);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_AND);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_OR);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_XOR);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_EXCH);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_CMP_EXCH);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_IMAX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_IMIN);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_UMAX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_IMM_ATOMIC_UMIN);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_SYNC);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DADD);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DMAX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DMIN);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DMUL);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DEQ);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DGE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DLT);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DNE);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DMOV);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DMOVC);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DTOF);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_FTOD);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_EVAL_SNAPPED);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_EVAL_SAMPLE_INDEX);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_EVAL_CENTROID);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_ABORT);
				register_instruction_handler<general_instruction>(D3D11_SB_OPCODE_DEBUG_BREAK);

				/* dx11 declarations */

				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_STREAM);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_FUNCTION_BODY);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_FUNCTION_TABLE);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_INTERFACE);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_TESS_DOMAIN);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_TESS_PARTITIONING);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE);
				register_instruction_handler<declaration_instruction<0, 1>>(D3D11_SB_OPCODE_DCL_HS_MAX_TESSFACTOR);
				register_instruction_handler<declaration_instruction<0, 1>>(D3D11_SB_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT);
				register_instruction_handler<declaration_instruction<0, 0>>(D3D11_SB_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT);
				register_instruction_handler<declaration_instruction<0, 3>>(D3D11_SB_OPCODE_DCL_THREAD_GROUP);
				register_instruction_handler<dcl_resource>(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW);
				register_instruction_handler<declaration_instruction<1, 1>>(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED);
				register_instruction_handler<declaration_instruction<1, 1>>(D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW);
				register_instruction_handler<declaration_instruction<1, 2>>(D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED);
				register_instruction_handler<declaration_instruction<1, 0>>(D3D11_SB_OPCODE_DCL_RESOURCE_RAW);
				register_instruction_handler<declaration_instruction<1, 1>>(D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED);

				/* dx11.1 opcodes */

				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_DDIV);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_DFMA);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_DRCP);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_MSAD);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_DTOI);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_DTOU);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_ITOD);
				register_instruction_handler<general_instruction>(D3D11_1_SB_OPCODE_UTOD);

				/* wddm 1.3 opcodes */

				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_GATHER4_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_GATHER4_C_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_GATHER4_PO_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_GATHER4_PO_C_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_LD_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_LD_MS_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_LD_UAV_TYPED_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_LD_RAW_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_LD_STRUCTURED_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_L_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_C_LZ_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_CLAMP_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_B_CLAMP_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_D_CLAMP_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_SAMPLE_C_CLAMP_FEEDBACK);
				//register_instruction_handler<general_instruction>(D3DWDDM1_3_SB_OPCODE_CHECK_ACCESS_FULLY_MAPPED);
			}

			initializer _(initialize);

			void process_instruction(utils::bit_buffer_le& input_buffer, utils::bit_buffer_le& output_buffer, const instruction_cb& callback)
			{
				const auto instruction = read_instruction(input_buffer);
				const auto skip = callback(output_buffer, instruction);

				if (!skip)
				{
					write_instruction(output_buffer, instruction);
				}
			}
		}

		instruction_t read_instruction(utils::bit_buffer_le& input_buffer)
		{
			const auto beg = input_buffer.total();
			const auto opcode_type = input_buffer.read_bits(11);
			input_buffer.set_bit(beg);

			if (const auto& handler = instruction_handlers[opcode_type]; handler.get() != nullptr)
			{
				return handler->read(input_buffer);
			}

			throw std::runtime_error("unsupported instruction");
		}

		void write_instruction(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
		{
			if (const auto& handler = instruction_handlers[instruction.opcode.type]; handler.get() != nullptr)
			{
				return handler->write(output_buffer, instruction);
			}

			throw std::runtime_error("unsupported instruction");
		}

		void print_instruction(const instruction_t& instruction)
		{
			if (const auto& handler = instruction_handlers[instruction.opcode.type]; handler.get() != nullptr)
			{
				return handler->print(instruction);
			}

			throw std::runtime_error("unsupported instruction");
		}
	}

	std::string patch_shader(unsigned char* program, unsigned int program_size, const instruction_cb& callback)
	{
		std::string output_program;

		if (program_size == 0)
		{
			return output_program;
		}

		unsigned int chunk_size{};
		auto chunk = parse_shader_chunk(program, program_size, &chunk_size);
		const auto offset = chunk - program;

		output_program.append(reinterpret_cast<char*>(program), offset);

		utils::bit_buffer_le input_buffer({reinterpret_cast<char*>(chunk), chunk_size});
		utils::bit_buffer_le output_buffer;

		const auto type = input_buffer.read_bytes(4); // "SHEX"
		const auto len = input_buffer.read_bytes(4);
		const auto minor_version = input_buffer.read_bits(4);
		const auto major_version = input_buffer.read_bits(4);
		const auto program_type = input_buffer.read_bytes(1);
		const auto unk = input_buffer.read_bytes(2);
		const auto num_dwords = input_buffer.read_bytes(4);

		output_buffer.write_bytes(4, type);
		output_buffer.write_bytes(4, len);
		output_buffer.write_bits(4, minor_version);
		output_buffer.write_bits(4, major_version);
		output_buffer.write_bytes(1, program_type);
		output_buffer.write_bytes(2, unk);
		output_buffer.write_bytes(4, num_dwords);

		while (input_buffer.total() < chunk_size * 8)
		{
			asm_::process_instruction(input_buffer, output_buffer, callback);
		}

		const auto total_len = output_buffer.total() / 8u;
		const auto new_len = total_len - 8u;
		const auto new_num_dwords = (new_len / 4u);

		output_buffer.set_byte(4);
		output_buffer.write_bytes(4, new_len);
		output_buffer.set_byte(12);
		output_buffer.write_bytes(4, new_num_dwords);

		const auto buffer = output_buffer.get_buffer().data();
		output_program.append(buffer, total_len);

		const auto header = reinterpret_cast<dx11_shader_header*>(output_program.data());

		header->program_size = static_cast<std::uint32_t>(output_program.size());

		dxbc::CalculateDXBCChecksum(reinterpret_cast<unsigned char*>(output_program.data()),
			static_cast<std::uint32_t>(output_program.size()), reinterpret_cast<DWORD*>(&header->checksum));

		return output_program;
	}
}
