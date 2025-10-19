#pragma once

#include <utils/bit_buffer.hpp>

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

	typedef enum D3D10_SB_OPCODE_TYPE
	{
		D3D10_SB_OPCODE_ADD,
		D3D10_SB_OPCODE_AND,
		D3D10_SB_OPCODE_BREAK,
		D3D10_SB_OPCODE_BREAKC,
		D3D10_SB_OPCODE_CALL,
		D3D10_SB_OPCODE_CALLC,
		D3D10_SB_OPCODE_CASE,
		D3D10_SB_OPCODE_CONTINUE,
		D3D10_SB_OPCODE_CONTINUEC,
		D3D10_SB_OPCODE_CUT,
		D3D10_SB_OPCODE_DEFAULT,
		D3D10_SB_OPCODE_DERIV_RTX,
		D3D10_SB_OPCODE_DERIV_RTY,
		D3D10_SB_OPCODE_DISCARD,
		D3D10_SB_OPCODE_DIV,
		D3D10_SB_OPCODE_DP2,
		D3D10_SB_OPCODE_DP3,
		D3D10_SB_OPCODE_DP4,
		D3D10_SB_OPCODE_ELSE,
		D3D10_SB_OPCODE_EMIT,
		D3D10_SB_OPCODE_EMITTHENCUT,
		D3D10_SB_OPCODE_ENDIF,
		D3D10_SB_OPCODE_ENDLOOP,
		D3D10_SB_OPCODE_ENDSWITCH,
		D3D10_SB_OPCODE_EQ,
		D3D10_SB_OPCODE_EXP,
		D3D10_SB_OPCODE_FRC,
		D3D10_SB_OPCODE_FTOI,
		D3D10_SB_OPCODE_FTOU,
		D3D10_SB_OPCODE_GE,
		D3D10_SB_OPCODE_IADD,
		D3D10_SB_OPCODE_IF,
		D3D10_SB_OPCODE_IEQ,
		D3D10_SB_OPCODE_IGE,
		D3D10_SB_OPCODE_ILT,
		D3D10_SB_OPCODE_IMAD,
		D3D10_SB_OPCODE_IMAX,
		D3D10_SB_OPCODE_IMIN,
		D3D10_SB_OPCODE_IMUL,
		D3D10_SB_OPCODE_INE,
		D3D10_SB_OPCODE_INEG,
		D3D10_SB_OPCODE_ISHL,
		D3D10_SB_OPCODE_ISHR,
		D3D10_SB_OPCODE_ITOF,
		D3D10_SB_OPCODE_LABEL,
		D3D10_SB_OPCODE_LD,
		D3D10_SB_OPCODE_LD_MS,
		D3D10_SB_OPCODE_LOG,
		D3D10_SB_OPCODE_LOOP,
		D3D10_SB_OPCODE_LT,
		D3D10_SB_OPCODE_MAD,
		D3D10_SB_OPCODE_MIN,
		D3D10_SB_OPCODE_MAX,
		D3D10_SB_OPCODE_CUSTOMDATA,
		D3D10_SB_OPCODE_MOV,
		D3D10_SB_OPCODE_MOVC,
		D3D10_SB_OPCODE_MUL,
		D3D10_SB_OPCODE_NE,
		D3D10_SB_OPCODE_NOP,
		D3D10_SB_OPCODE_NOT,
		D3D10_SB_OPCODE_OR,
		D3D10_SB_OPCODE_RESINFO,
		D3D10_SB_OPCODE_RET,
		D3D10_SB_OPCODE_RETC,
		D3D10_SB_OPCODE_ROUND_NE,
		D3D10_SB_OPCODE_ROUND_NI,
		D3D10_SB_OPCODE_ROUND_PI,
		D3D10_SB_OPCODE_ROUND_Z,
		D3D10_SB_OPCODE_RSQ,
		D3D10_SB_OPCODE_SAMPLE,
		D3D10_SB_OPCODE_SAMPLE_C,
		D3D10_SB_OPCODE_SAMPLE_C_LZ,
		D3D10_SB_OPCODE_SAMPLE_L,
		D3D10_SB_OPCODE_SAMPLE_D,
		D3D10_SB_OPCODE_SAMPLE_B,
		D3D10_SB_OPCODE_SQRT,
		D3D10_SB_OPCODE_SWITCH,
		D3D10_SB_OPCODE_SINCOS,
		D3D10_SB_OPCODE_UDIV,
		D3D10_SB_OPCODE_ULT,
		D3D10_SB_OPCODE_UGE,
		D3D10_SB_OPCODE_UMUL,
		D3D10_SB_OPCODE_UMAD,
		D3D10_SB_OPCODE_UMAX,
		D3D10_SB_OPCODE_UMIN,
		D3D10_SB_OPCODE_USHR,
		D3D10_SB_OPCODE_UTOF,
		D3D10_SB_OPCODE_XOR,
		D3D10_SB_OPCODE_DCL_RESOURCE, // DCL* opcodes have
		D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER, // custom operand formats.
		D3D10_SB_OPCODE_DCL_SAMPLER,
		D3D10_SB_OPCODE_DCL_INDEX_RANGE,
		D3D10_SB_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY,
		D3D10_SB_OPCODE_DCL_GS_INPUT_PRIMITIVE,
		D3D10_SB_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT,
		D3D10_SB_OPCODE_DCL_INPUT,
		D3D10_SB_OPCODE_DCL_INPUT_SGV,
		D3D10_SB_OPCODE_DCL_INPUT_SIV,
		D3D10_SB_OPCODE_DCL_INPUT_PS,
		D3D10_SB_OPCODE_DCL_INPUT_PS_SGV,
		D3D10_SB_OPCODE_DCL_INPUT_PS_SIV,
		D3D10_SB_OPCODE_DCL_OUTPUT,
		D3D10_SB_OPCODE_DCL_OUTPUT_SGV,
		D3D10_SB_OPCODE_DCL_OUTPUT_SIV,
		D3D10_SB_OPCODE_DCL_TEMPS,
		D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP,
		D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS,

		// -----------------------------------------------

		D3D10_SB_OPCODE_RESERVED0,

		// ---------- DX 10.1 op codes---------------------

		D3D10_1_SB_OPCODE_LOD,
		D3D10_1_SB_OPCODE_GATHER4,
		D3D10_1_SB_OPCODE_SAMPLE_POS,
		D3D10_1_SB_OPCODE_SAMPLE_INFO,

		// -----------------------------------------------

			// This should be 10.1's version of D3D10_SB_NUM_OPCODES
		D3D10_1_SB_OPCODE_RESERVED1,

		// ---------- DX 11 op codes---------------------
		D3D11_SB_OPCODE_HS_DECLS, // token marks beginning of HS sub-shader
		D3D11_SB_OPCODE_HS_CONTROL_POINT_PHASE, // token marks beginning of HS sub-shader
		D3D11_SB_OPCODE_HS_FORK_PHASE, // token marks beginning of HS sub-shader
		D3D11_SB_OPCODE_HS_JOIN_PHASE, // token marks beginning of HS sub-shader

		D3D11_SB_OPCODE_EMIT_STREAM,
		D3D11_SB_OPCODE_CUT_STREAM,
		D3D11_SB_OPCODE_EMITTHENCUT_STREAM,
		D3D11_SB_OPCODE_INTERFACE_CALL,

		D3D11_SB_OPCODE_BUFINFO,
		D3D11_SB_OPCODE_DERIV_RTX_COARSE,
		D3D11_SB_OPCODE_DERIV_RTX_FINE,
		D3D11_SB_OPCODE_DERIV_RTY_COARSE,
		D3D11_SB_OPCODE_DERIV_RTY_FINE,
		D3D11_SB_OPCODE_GATHER4_C,
		D3D11_SB_OPCODE_GATHER4_PO,
		D3D11_SB_OPCODE_GATHER4_PO_C,
		D3D11_SB_OPCODE_RCP,
		D3D11_SB_OPCODE_F32TOF16,
		D3D11_SB_OPCODE_F16TOF32,
		D3D11_SB_OPCODE_UADDC,
		D3D11_SB_OPCODE_USUBB,
		D3D11_SB_OPCODE_COUNTBITS,
		D3D11_SB_OPCODE_FIRSTBIT_HI,
		D3D11_SB_OPCODE_FIRSTBIT_LO,
		D3D11_SB_OPCODE_FIRSTBIT_SHI,
		D3D11_SB_OPCODE_UBFE,
		D3D11_SB_OPCODE_IBFE,
		D3D11_SB_OPCODE_BFI,
		D3D11_SB_OPCODE_BFREV,
		D3D11_SB_OPCODE_SWAPC,

		D3D11_SB_OPCODE_DCL_STREAM,
		D3D11_SB_OPCODE_DCL_FUNCTION_BODY,
		D3D11_SB_OPCODE_DCL_FUNCTION_TABLE,
		D3D11_SB_OPCODE_DCL_INTERFACE,

		D3D11_SB_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT,
		D3D11_SB_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT,
		D3D11_SB_OPCODE_DCL_TESS_DOMAIN,
		D3D11_SB_OPCODE_DCL_TESS_PARTITIONING,
		D3D11_SB_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE,
		D3D11_SB_OPCODE_DCL_HS_MAX_TESSFACTOR,
		D3D11_SB_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT,
		D3D11_SB_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT,

		D3D11_SB_OPCODE_DCL_THREAD_GROUP,
		D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED,
		D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW,
		D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED,
		D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW,
		D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED,
		D3D11_SB_OPCODE_DCL_RESOURCE_RAW,
		D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED,
		D3D11_SB_OPCODE_LD_UAV_TYPED,
		D3D11_SB_OPCODE_STORE_UAV_TYPED,
		D3D11_SB_OPCODE_LD_RAW,
		D3D11_SB_OPCODE_STORE_RAW,
		D3D11_SB_OPCODE_LD_STRUCTURED,
		D3D11_SB_OPCODE_STORE_STRUCTURED,
		D3D11_SB_OPCODE_ATOMIC_AND,
		D3D11_SB_OPCODE_ATOMIC_OR,
		D3D11_SB_OPCODE_ATOMIC_XOR,
		D3D11_SB_OPCODE_ATOMIC_CMP_STORE,
		D3D11_SB_OPCODE_ATOMIC_IADD,
		D3D11_SB_OPCODE_ATOMIC_IMAX,
		D3D11_SB_OPCODE_ATOMIC_IMIN,
		D3D11_SB_OPCODE_ATOMIC_UMAX,
		D3D11_SB_OPCODE_ATOMIC_UMIN,
		D3D11_SB_OPCODE_IMM_ATOMIC_ALLOC,
		D3D11_SB_OPCODE_IMM_ATOMIC_CONSUME,
		D3D11_SB_OPCODE_IMM_ATOMIC_IADD,
		D3D11_SB_OPCODE_IMM_ATOMIC_AND,
		D3D11_SB_OPCODE_IMM_ATOMIC_OR,
		D3D11_SB_OPCODE_IMM_ATOMIC_XOR,
		D3D11_SB_OPCODE_IMM_ATOMIC_EXCH,
		D3D11_SB_OPCODE_IMM_ATOMIC_CMP_EXCH,
		D3D11_SB_OPCODE_IMM_ATOMIC_IMAX,
		D3D11_SB_OPCODE_IMM_ATOMIC_IMIN,
		D3D11_SB_OPCODE_IMM_ATOMIC_UMAX,
		D3D11_SB_OPCODE_IMM_ATOMIC_UMIN,
		D3D11_SB_OPCODE_SYNC,

		D3D11_SB_OPCODE_DADD,
		D3D11_SB_OPCODE_DMAX,
		D3D11_SB_OPCODE_DMIN,
		D3D11_SB_OPCODE_DMUL,
		D3D11_SB_OPCODE_DEQ,
		D3D11_SB_OPCODE_DGE,
		D3D11_SB_OPCODE_DLT,
		D3D11_SB_OPCODE_DNE,
		D3D11_SB_OPCODE_DMOV,
		D3D11_SB_OPCODE_DMOVC,
		D3D11_SB_OPCODE_DTOF,
		D3D11_SB_OPCODE_FTOD,

		D3D11_SB_OPCODE_EVAL_SNAPPED,
		D3D11_SB_OPCODE_EVAL_SAMPLE_INDEX,
		D3D11_SB_OPCODE_EVAL_CENTROID,

		D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT,

		D3D10_SB_NUM_OPCODES                                     // Should be the last entry
	} D3D10_SB_OPCODE_TYPE;

	typedef enum D3D10_SB_OPERAND_TYPE
	{
		D3D10_SB_OPERAND_TYPE_TEMP = 0,  // Temporary Register File
		D3D10_SB_OPERAND_TYPE_INPUT = 1,  // General Input Register File
		D3D10_SB_OPERAND_TYPE_OUTPUT = 2,  // General Output Register File
		D3D10_SB_OPERAND_TYPE_INDEXABLE_TEMP = 3,  // Temporary Register File (indexable)
		D3D10_SB_OPERAND_TYPE_IMMEDIATE32 = 4,  // 32bit/component immediate value(s)
		// If for example, operand token bits
		// [01:00]==D3D10_SB_OPERAND_4_COMPONENT,
		// this means that the operand type:
		// D3D10_SB_OPERAND_TYPE_IMMEDIATE32
		// results in 4 additional 32bit
		// DWORDS present for the operand.
		D3D10_SB_OPERAND_TYPE_IMMEDIATE64 = 5,  // 64bit/comp.imm.val(s)HI:LO
		D3D10_SB_OPERAND_TYPE_SAMPLER = 6,  // Reference to sampler state
		D3D10_SB_OPERAND_TYPE_RESOURCE = 7,  // Reference to memory resource (e.g. texture)
		D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER = 8,  // Reference to constant buffer
		D3D10_SB_OPERAND_TYPE_IMMEDIATE_CONSTANT_BUFFER = 9,  // Reference to immediate constant buffer
		D3D10_SB_OPERAND_TYPE_LABEL = 10, // Label
		D3D10_SB_OPERAND_TYPE_INPUT_PRIMITIVEID = 11, // Input primitive ID
		D3D10_SB_OPERAND_TYPE_OUTPUT_DEPTH = 12, // Output Depth
		D3D10_SB_OPERAND_TYPE_NULL = 13, // Null register, used to discard results of operations
		// Below Are operands new in DX 10.1
		D3D10_SB_OPERAND_TYPE_RASTERIZER = 14, // DX10.1 Rasterizer register, used to denote the depth/stencil and render target resources
		D3D10_SB_OPERAND_TYPE_OUTPUT_COVERAGE_MASK = 15, // DX10.1 PS output MSAA coverage mask (scalar)
		// Below Are operands new in DX 11
		D3D11_SB_OPERAND_TYPE_STREAM = 16, // Reference to GS stream output resource
		D3D11_SB_OPERAND_TYPE_FUNCTION_BODY = 17, // Reference to a function definition
		D3D11_SB_OPERAND_TYPE_FUNCTION_TABLE = 18, // Reference to a set of functions used by a class
		D3D11_SB_OPERAND_TYPE_INTERFACE = 19, // Reference to an interface
		D3D11_SB_OPERAND_TYPE_FUNCTION_INPUT = 20, // Reference to an input parameter to a function
		D3D11_SB_OPERAND_TYPE_FUNCTION_OUTPUT = 21, // Reference to an output parameter to a function
		D3D11_SB_OPERAND_TYPE_OUTPUT_CONTROL_POINT_ID = 22, // HS Control Point phase input saying which output control point ID this is
		D3D11_SB_OPERAND_TYPE_INPUT_FORK_INSTANCE_ID = 23, // HS Fork Phase input instance ID
		D3D11_SB_OPERAND_TYPE_INPUT_JOIN_INSTANCE_ID = 24, // HS Join Phase input instance ID
		D3D11_SB_OPERAND_TYPE_INPUT_CONTROL_POINT = 25, // HS Fork+Join, DS phase input control points (array of them)
		D3D11_SB_OPERAND_TYPE_OUTPUT_CONTROL_POINT = 26, // HS Fork+Join phase output control points (array of them)
		D3D11_SB_OPERAND_TYPE_INPUT_PATCH_CONSTANT = 27, // DS+HSJoin Input Patch Constants (array of them)
		D3D11_SB_OPERAND_TYPE_INPUT_DOMAIN_POINT = 28, // DS Input Domain point
		D3D11_SB_OPERAND_TYPE_THIS_POINTER = 29, // Reference to an interface this pointer
		D3D11_SB_OPERAND_TYPE_UNORDERED_ACCESS_VIEW = 30, // Reference to UAV u#
		D3D11_SB_OPERAND_TYPE_THREAD_GROUP_SHARED_MEMORY = 31, // Reference to Thread Group Shared Memory g#
		D3D11_SB_OPERAND_TYPE_INPUT_THREAD_ID = 32, // Compute Shader Thread ID
		D3D11_SB_OPERAND_TYPE_INPUT_THREAD_GROUP_ID = 33, // Compute Shader Thread Group ID
		D3D11_SB_OPERAND_TYPE_INPUT_THREAD_ID_IN_GROUP = 34, // Compute Shader Thread ID In Thread Group
		D3D11_SB_OPERAND_TYPE_INPUT_COVERAGE_MASK = 35, // Pixel shader coverage mask input
		D3D11_SB_OPERAND_TYPE_INPUT_THREAD_ID_IN_GROUP_FLATTENED = 36, // Compute Shader Thread ID In Group Flattened to a 1D value.
		D3D11_SB_OPERAND_TYPE_INPUT_GS_INSTANCE_ID = 37, // Input GS instance ID
		D3D11_SB_OPERAND_TYPE_OUTPUT_DEPTH_GREATER_EQUAL = 38, // Output Depth, forced to be greater than or equal than current depth
		D3D11_SB_OPERAND_TYPE_OUTPUT_DEPTH_LESS_EQUAL = 39, // Output Depth, forced to be less than or equal to current depth
		D3D11_SB_OPERAND_TYPE_CYCLE_COUNTER = 40, // Cycle counter
	} D3D10_SB_OPERAND_TYPE;

	typedef enum D3D10_SB_OPERAND_INDEX_REPRESENTATION
	{
		D3D10_SB_OPERAND_INDEX_IMMEDIATE32 = 0, // Extra DWORD
		D3D10_SB_OPERAND_INDEX_IMMEDIATE64 = 1, // 2 Extra DWORDs
		//   (HI32:LO32)
		D3D10_SB_OPERAND_INDEX_RELATIVE = 2, // Extra operand
		D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE = 3, // Extra DWORD followed by
		//   extra operand
		D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE = 4, // 2 Extra DWORDS
		//   (HI32:LO32) followed
		//   by extra operand
	} D3D10_SB_OPERAND_INDEX_REPRESENTATION;

	typedef enum D3D10_SB_OPERAND_INDEX_DIMENSION
	{
		D3D10_SB_OPERAND_INDEX_0D = 0, // e.g. Position
		D3D10_SB_OPERAND_INDEX_1D = 1, // Most common.  e.g. Temp registers.
		D3D10_SB_OPERAND_INDEX_2D = 2, // e.g. Geometry Program Input registers.
		D3D10_SB_OPERAND_INDEX_3D = 3, // 3D rarely if ever used.
	} D3D10_SB_OPERAND_INDEX_DIMENS;

	typedef enum D3D10_SB_OPERAND_NUM_COMPONENTS
	{
		D3D10_SB_OPERAND_0_COMPONENT = 0,
		D3D10_SB_OPERAND_1_COMPONENT = 1,
		D3D10_SB_OPERAND_4_COMPONENT = 2,
		D3D10_SB_OPERAND_N_COMPONENT = 3 // unused for now
	} D3D10_SB_OPERAND_NUM_COMPONENTS;

	typedef enum D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE
	{
		D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE = 0,  // mask 4 components
		D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE = 1,  // swizzle 4 components
		D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE = 2, // select 1 of 4 components
	} D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE;

	typedef enum D3D10_SB_4_COMPONENT_NAME
	{
		D3D10_SB_4_COMPONENT_X = 0,
		D3D10_SB_4_COMPONENT_Y = 1,
		D3D10_SB_4_COMPONENT_Z = 2,
		D3D10_SB_4_COMPONENT_W = 3,
		D3D10_SB_4_COMPONENT_R = 0,
		D3D10_SB_4_COMPONENT_G = 1,
		D3D10_SB_4_COMPONENT_B = 2,
		D3D10_SB_4_COMPONENT_A = 3
	} D3D10_SB_4_COMPONENT_NAME;

#define D3D10_SB_OPERAND_4_COMPONENT_MASK_MASK   0x000000f0
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_SHIFT  4
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_X      0x00000010
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_Y      0x00000020
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_Z      0x00000040
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_W      0x00000080
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_R      D3D10_SB_OPERAND_4_COMPONENT_MASK_X
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_G      D3D10_SB_OPERAND_4_COMPONENT_MASK_Y
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_B      D3D10_SB_OPERAND_4_COMPONENT_MASK_Z
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_A      D3D10_SB_OPERAND_4_COMPONENT_MASK_W
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_ALL    D3D10_SB_OPERAND_4_COMPONENT_MASK_MASK

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

	struct operand_indices_t
	{
		std::uint32_t representation;
		std::uint32_t data[4];
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
		std::uint32_t value : 31;
		std::uint32_t extended : 1;
	};

	struct operand_t;

	struct operand_t
	{
		std::uint32_t type;
		std::uint32_t dimension;
		std::uint32_t extended;
		std::vector<operand_extended_t> extensions;
		operand_indices_t indices[3];
		operand_components_t components;
		operand_t* extra_operand;
	};

	struct opcode_extended_t
	{
		std::uint32_t value : 31;
		std::uint32_t extended : 1;
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

	using instruction_cb = std::function<bool(utils::bit_buffer_le&, instruction_t&)>;

	void print_instruction(const instruction_t& instruction);

	void write_opcode(utils::bit_buffer_le& output_buffer, const opcode_t& opcode);
	std::uint32_t get_operand_length(const operand_t& operand);
	void write_operand(utils::bit_buffer_le& output_buffer, const operand_t& operand);

	opcode_t create_opcode(const std::uint32_t type, const std::uint32_t controls);
	operand_t create_literal_operand(const float x, const float y, const float z, const float w);

	std::string patch_shader(unsigned char* program, unsigned int program_size, const instruction_cb& callback);

	std::vector<size_t> get_dest_reference_offsets(unsigned char* program, unsigned int program_size);
	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size);

	unsigned int calc_crc32(unsigned char* program, unsigned int program_size);
}
