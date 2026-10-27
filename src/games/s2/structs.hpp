#pragma once
#include <cstddef>
#include <cstdint>
#include <d3d11.h>
#include <formats/ddl.hpp>
#include <struct_asserts.hpp>
#include <xfile/structs.hpp>

namespace zonetool::s2
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	typedef std::uint32_t scr_string_t;

	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET,
		ASSET_TYPE_SNDPHYSPRESET,
		ASSET_TYPE_SNDMUSICSET,
		ASSET_TYPE_PHYSCOLLMAP,
		ASSET_TYPE_PHYSWATERPRESET,
		ASSET_TYPE_PHYSWORLDMAP,
		ASSET_TYPE_PHYSCONSTRAINT,
		ASSET_TYPE_XANIM,
		ASSET_TYPE_XSURFACE_SHARED,
		ASSET_TYPE_XMODEL_SURFS,
		ASSET_TYPE_XMODEL,
		ASSET_TYPE_XMODEL_BASE,
		ASSET_TYPE_MAYHEM,
		ASSET_TYPE_MATERIAL,
		ASSET_TYPE_COMPUTESHADER,
		ASSET_TYPE_VERTEXSHADER,
		ASSET_TYPE_HULLSHADER,
		ASSET_TYPE_DOMAINSHADER,
		ASSET_TYPE_PIXELSHADER,
		ASSET_TYPE_VERTEXDECL,
		ASSET_TYPE_TECHNIQUE_SET,
		ASSET_TYPE_IMAGE,
		ASSET_TYPE_SOUND,
		ASSET_TYPE_SOUNDSUBMIX,
		ASSET_TYPE_SOUND_CURVE,
		ASSET_TYPE_DIST_CURVE,
		ASSET_TYPE_REVERB_CURVE,
		ASSET_TYPE_SOUND_CONTEXT,
		ASSET_TYPE_ALIAS_PARAMETER_MODIFIER,
		ASSET_TYPE_ALIAS_COMBAT_CONE,
		ASSET_TYPE_LOADED_SOUND,
		ASSET_TYPE_CLIPMAP,
		ASSET_TYPE_COMWORLD,
		ASSET_TYPE_GLASSWORLD,
		ASSET_TYPE_PATHDATA,
		ASSET_TYPE_NAVMESH,
		ASSET_TYPE_VEHICLE_TRACK,
		ASSET_TYPE_MAP_ENTS,
		ASSET_TYPE_FXWORLD,
		ASSET_TYPE_GFXWORLD,
		ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE,
		ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE,
		ASSET_TYPE_IESPROFILE,
		ASSET_TYPE_LIGHT_DEF,
		ASSET_TYPE_UI_MAP,
		ASSET_TYPE_ANIMCLASS,
		ASSET_TYPE_LOCALIZE_ENTRY,
		ASSET_TYPE_ATTACHMENT,
		ASSET_TYPE_WEAPON,
		ASSET_TYPE_SNDDRIVER_GLOBALS,
		ASSET_TYPE_FX,
		ASSET_TYPE_IMPACT_FX,
		ASSET_TYPE_SURFACE_FX,
		ASSET_TYPE_AITYPE,
		ASSET_TYPE_MPTYPE,
		ASSET_TYPE_CHARACTER,
		ASSET_TYPE_XMODELALIAS,
		ASSET_TYPE_RAWFILE,
		ASSET_TYPE_SCRIPTFILE,
		ASSET_TYPE_STRINGTABLE,
		ASSET_TYPE_LEADERBOARDDEF,
		ASSET_TYPE_VIRTUAL_LEADERBOARD,
		ASSET_TYPE_STRUCTURED_DATA_DEF,
		ASSET_TYPE_DDL,
		ASSET_TYPE_TRACER,
		ASSET_TYPE_VEHICLE,
		ASSET_TYPE_ADDON_MAP_ENTS,
		ASSET_TYPE_NET_CONST_STRINGS,
		ASSET_TYPE_REVERB_PRESET,
		ASSET_TYPE_LUA_FILE,
		ASSET_TYPE_SCRIPTABLE,
		ASSET_TYPE_EQUIPMENT_SND_TABLE,
		ASSET_TYPE_VECTORFIELD,
		ASSET_TYPE_PARTICLE_SIM_ANIMATION,
		ASSET_TYPE_LASER,
		ASSET_TYPE_BEAM,
		ASSET_TYPE_SKELETON_SCRIPT,
		ASSET_TYPE_CLUT,
		ASSET_TYPE_TTF,
		ASSET_TYPE_SPLINES,
		ASSET_TYPE_PHYS_CLOTH_TUNING,
		ASSET_TYPE_DLOG_SCHEMA,
		ASSET_TYPE_DLOG_ROUTES,
		ASSET_TYPE_COUNT
	};

	enum XFileBlock
	{
		XFILE_BLOCK_VIRTUAL = 0x0,
		XFILE_BLOCK_PHYSICAL = 0x1,
		XFILE_BLOCK_RUNTIME = 0x2,
		XFILE_BLOCK_RUNTIME_UNREAD = 0x3,
		XFILE_BLOCK_SCRIPT = 0x4,
		XFILE_BLOCK_CALLBACK = 0x5,
		XFILE_BLOCK_TEMP = 0x6,
		MAX_XFILE_COUNT = 0x7,
	};

	struct RawFile
	{
		const char* name;
		int compressedLen;
		int len;
		const char* buffer;
	};
	static_assert(sizeof(RawFile) == 0x18);

	struct ScriptFile
	{
		const char* name;
		int compressedLen;
		int len;
		int bytecodeLen;
		const char* buffer;
		char* bytecode;
	};
	static_assert(sizeof(ScriptFile) == 0x28);

	struct StringTableCell
	{
		const char* string;
		int hash;
	};

	struct StringTable
	{
		const char* name;
		int columnCount;
		int rowCount;
		StringTableCell* values;
	};
	static_assert(sizeof(StringTable) == 0x18);

	struct LocalizeEntry
	{
		const char* value;
		const char* name;
	};

	struct LuaFile
	{
		const char* name;
		int len;
		char strippingType;
		const char* buffer;
	};
	static_assert(sizeof(LuaFile) == 0x18);

	struct TTFDef
	{
		const char* name;
		int fileLen;
		const char* file;
		void* ftFace;
	};
	static_assert(sizeof(TTFDef) == 0x20);

	struct GfxComputeShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
		char __pad[4];
	};

	struct ComputeShaderProgram
	{
		ID3D11ComputeShader* cs;
		GfxComputeShaderLoadDef loadDef;
	};

	struct ComputeShader
	{
		const char* name;
		ComputeShaderProgram prog;
	};
	static_assert(sizeof(ComputeShader) == 0x20);

	struct GfxVertexShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
		unsigned int microCodeCrc;
	};

	struct MaterialVertexShaderProgram
	{
		ID3D11VertexShader* vs;
		GfxVertexShaderLoadDef loadDef;
	};

	struct MaterialVertexShader
	{
		const char* name;
		MaterialVertexShaderProgram prog;
	};
	static_assert(sizeof(MaterialVertexShader) == 0x20);

	struct GfxPixelShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
		unsigned int microCodeCrc;
	};

	struct MaterialPixelShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxPixelShaderLoadDef loadDef;
	};

	struct MaterialPixelShader
	{
		const char* name;
		MaterialPixelShaderProgram prog;
	};
	static_assert(sizeof(MaterialPixelShader) == 0x20);

	struct GfxHullShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
		char __pad[4];
	};

	struct MaterialHullShaderProgram
	{
		ID3D11HullShader* hs;
		GfxHullShaderLoadDef loadDef;
	};

	struct MaterialHullShader
	{
		const char* name;
		MaterialHullShaderProgram prog;
	};
	static_assert(sizeof(MaterialHullShader) == 0x20);

	struct GfxDomainShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
		char __pad[4];
	};

	struct MaterialDomainShaderProgram
	{
		ID3D11DomainShader* ds;
		GfxDomainShaderLoadDef loadDef;
	};

	struct MaterialDomainShader
	{
		const char* name;
		MaterialDomainShaderProgram prog;
	};
	static_assert(sizeof(MaterialDomainShader) == 0x20);

	enum MaterialStreamRoutingSource : std::uint8_t
	{
		STREAM_SRC_POSITION = 0,
		STREAM_SRC_COLOR = 1,
		STREAM_SRC_TEXCOORD_0 = 2,
		STREAM_SRC_NORMAL = 3,
		STREAM_SRC_TANGENT = 4,
		STREAM_SRC_WIND = 5, // "ec1", the per vertex foliage sway weights
		STREAM_SRC_LIGHTMAP = 6,
		STREAM_SRC_TEXEL_DENSITY = 7, // "ed" in world vertex decls
		STREAM_SRC_VERTEX_LIGHTING_VISIBILITY = 13,
		STREAM_SRC_VERTEX_LIGHTING_AMBIENT = 14,
		STREAM_SRC_VERTEX_LIGHTING_HIGHLIGHT = 15,
		STREAM_SRC_COUNT = 32,
	};

	enum MaterialStreamRoutingDestination : std::uint8_t
	{
		STREAM_DST_POSITION = 0,
		STREAM_DST_NORMAL = 1,
		STREAM_DST_COLOR_0 = 2,
		STREAM_DST_COLOR_1 = 3,
		STREAM_DST_TEXCOORD_0 = 6,
		STREAM_DST_TEXCOORD_15 = 21,
		STREAM_DST_TEXCOORD_16 = 22,
	};

	struct MaterialStreamRouting
	{
		unsigned char source;
		unsigned char dest;
		unsigned char mask;
		unsigned char slot; // stock stores the routing index, no reader found
	};

	struct MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[32];
		ID3D11InputLayout* decl[276];
	};

	struct MaterialVertexDeclaration
	{
		const char* name;
		unsigned char streamCount;
		bool hasOptionalSource;
		char __pad0[6];
		MaterialVertexStreamRouting routing;
	};
	static_assert(sizeof(MaterialVertexDeclaration) == 0x930);
	static_assert(offsetof(MaterialVertexDeclaration, routing) == 0x10);

	enum MaterialShaderArgumentType : std::uint8_t
	{
		MTL_ARG_CODE_CONST = 0x0,
		MTL_ARG_CODE_TEXTURE = 0x1,
		MTL_ARG_CODE_SAMPLER = 0x2,
		MTL_ARG_CODE_UAV = 0x3,
		MTL_ARG_CODE_BUFFER_UAV = 0x4,
		MTL_ARG_MATERIAL_CONST = 0x5,
		MTL_ARG_LITERAL_CONST = 0x6,
		MTL_ARG_MATERIAL_TEXTURE = 0x7,
		MTL_ARG_MATERIAL_SAMPLER = 0x8,
		MTL_ARG_COUNT = 0x9,
	};

	enum MaterialConstSource : std::int32_t
	{
		CONST_SRC_CODE_GAMETIME = 0,
		CONST_SRC_CODE_GENERIC_MATERIAL_DATA = 2,
		CONST_SRC_CODE_EYEOFFSET = 3,
		CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET = 6,
		CONST_SRC_CODE_SUN_DIRECTION = 11,
		CONST_SRC_CODE_SUN_COLOR = 12,
		CONST_SRC_CODE_SCRIPT_PARMS = 26,
		CONST_SRC_CODE_EFFECT_MODEL_COLOR = 29,
		CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE = 30,
		CONST_SRC_CODE_RENDER_TARGET_SIZE = 47,
		CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS = 55,
		CONST_SRC_CODE_NEARPLANE_ORG = 56,
		CONST_SRC_CODE_NEARPLANE_DX = 57,
		CONST_SRC_CODE_NEARPLANE_DY = 58,
		CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_STARTS = 91,
		CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_SCALES = 92,
		CONST_SRC_CODE_LIGHT_CLUSTER_CELLS = 93,
		CONST_SRC_CODE_LIGHT_TILE_PARMS = 94,
		CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0 = 105,
		CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1 = 106,
		CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2 = 107,
		CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3 = 108,
		CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM = 109,
		CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS = 110,
		CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF = 113,
		CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS = 133,
		CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET = 138,
		CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0 = 140,
		CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1 = 141,
		CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS = 142,
		CONST_SRC_CODE_SHADOWMAP_SCALE = 143,
		CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET = 144,
		CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK = 145,
		CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND = 146,
		CONST_SRC_CODE_ZNEAR = 147,
		CONST_SRC_CODE_INV_SCENE_PROJECTION = 148,
		CONST_SRC_CODE_MATERIAL_COLOR = 161,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_0 = 162,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_1 = 163,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_2 = 164,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_3 = 165,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_4 = 166,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_5 = 167,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_6 = 168,
		CONST_SRC_CODE_ATMOS_FOG_PARMS_7 = 169,
		CONST_SRC_CODE_HDR_STAGE_PARMS = 218,
		CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0 = 241,
		CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1 = 242,
		CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2 = 243,
		CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP = 244,
		CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE = 245,
		CONST_SRC_CODE_WIND_DIRECTION = 278,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 317,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 318,
		CONST_SRC_CODE_MODEL_LIGHTING_INDEX = 319,
		CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS = 320,
		CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD = 321,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 = 322,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 = 323,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 = 324,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 = 325,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 = 326,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 = 327,
		CONST_SRC_CODE_DEPTH_FROM_CLIP = 331,
		CONST_SRC_CODE_CODE_MESH_ARG_0 = 332,
		CONST_SRC_CODE_CODE_MESH_ARG_1 = 333,
		CONST_SRC_CODE_CODE_MESH_ARG_3 = 334,
		CONST_SRC_CODE_CODE_MESH_ARG_4 = 335,

		CONST_SRC_CODE_COUNT_FLOAT4 = 338,
		CONST_SRC_FIRST_CODE_MATRIX = 338,
		CONST_SRC_CODE_VIEW_MATRIX = 338,
		CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 339,
		CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 340,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 341,
		CONST_SRC_CODE_PROJECTION_MATRIX = 342,
		CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 343,
		CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 344,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 345,
		CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 346,
		CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 347,
		CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 348,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 349,
		CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 350,
		CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 351,
		CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 352,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 353,
		CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 354,
		CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 355,
		CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 356,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 357,
		CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX = 358,
		CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX = 359,
		CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX = 360,
		CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX = 361,
		CONST_SRC_CODE_WORLD_MATRIX0 = 362,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 = 363,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 = 364,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 = 365,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX0 = 366,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 = 367,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 368,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 369,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 = 370,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 = 371,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 372,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 373,
		CONST_SRC_CODE_WORLD_MATRIX1 = 374,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 = 375,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 = 376,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 = 377,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX1 = 378,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 = 379,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 380,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 381,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 = 382,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 = 383,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 384,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 385,
		CONST_SRC_CODE_WORLD_MATRIX2 = 386,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 = 387,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 = 388,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 = 389,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX2 = 390,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 = 391,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 392,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 393,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 = 394,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 = 395,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 396,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 397,
		CONST_SRC_TOTAL_COUNT = 398,
	};

	enum MaterialTextureSource : std::int32_t
	{
		TEXTURE_SRC_CODE_BLACK = 0,
		TEXTURE_SRC_CODE_WHITE = 1,
		TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 2,
		TEXTURE_SRC_CODE_IES_LOOKUP = 3,
		TEXTURE_SRC_CODE_SHADOWMAP_SUN = 6,
		TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN = 7,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 8,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1 = 9,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2 = 10,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3 = 11,
		TEXTURE_SRC_CODE_DEFERRED_SUN_SHADOW_VISIBILITY = 12,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT_CACHE = 13,
		TEXTURE_SRC_CODE_FEEDBACK = 14,
		TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 15,
		TEXTURE_SRC_CODE_SSR_BUFFER = 16,
		TEXTURE_SRC_CODE_HQ_SSR_DEPTH = 17,
		TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER = 18,
		TEXTURE_SRC_CODE_RESOLVED_SCENE = 19,
		TEXTURE_SRC_CODE_POST_EFFECT_0 = 20,
		TEXTURE_SRC_CODE_POST_EFFECT_1 = 21,
		TEXTURE_SRC_CODE_OUTDOOR = 22,
		TEXTURE_SRC_CODE_FLOATZ = 23,
		TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 24,
		TEXTURE_SRC_CODE_FLOATZ_STENCIL = 26,
		TEXTURE_SRC_CODE_CINEMATIC_Y = 31,
		TEXTURE_SRC_CODE_CINEMATIC_CR = 32,
		TEXTURE_SRC_CODE_CINEMATIC_CB = 33,
		TEXTURE_SRC_CODE_CINEMATIC_A = 34,
		TEXTURE_SRC_CODE_CINEMATIC_CBCR = 35,
		TEXTURE_SRC_CODE_DALTONIZE_CLUT = 37,
		TEXTURE_SRC_CODE_CLUT = 38,
		TEXTURE_SRC_CODE_HDR_CLUT = 39,
		TEXTURE_SRC_CODE_LINEAR_WRAP_MIPLINEAR = 40,
		TEXTURE_SRC_CODE_LINEAR_CLAMP = 41,
		TEXTURE_SRC_CODE_POINT_CLAMP = 42,
		TEXTURE_SRC_CODE_RANDOM_ROTATIONS = 43,
		TEXTURE_SRC_CODE_SSAO = 44,
		TEXTURE_SRC_CODE_SSAO_PREV_FRAME = 45,
		TEXTURE_SRC_CODE_SSAO_PREV_PREV_FRAME = 46,
		TEXTURE_SRC_CODE_DEPTH_CUR_FRAME = 47,
		TEXTURE_SRC_CODE_DEPTH_PREV_FRAME = 48,
		TEXTURE_SRC_CODE_DEPTH_PREV_PREV_FRAME = 49,
		TEXTURE_SRC_CODE_VELOCITY_CUR_FRAME = 50,
		TEXTURE_SRC_CODE_VELOCITY_PREV_FRAME = 51,
		TEXTURE_SRC_CODE_MSAA_SCENE = 52,
		TEXTURE_SRC_CODE_DEPTH_BLUR_0 = 53,
		TEXTURE_SRC_CODE_DEPTH_BLUR_1 = 54,
		TEXTURE_SRC_CODE_DEFAULT_DEPTH = 55,
		TEXTURE_SRC_CODE_MBLUR_VELOCITY = 56,
		TEXTURE_SRC_CODE_MBLUR_VELOCITY_TILE0 = 57,
		TEXTURE_SRC_CODE_MBLUR_VELOCITY_TILE1 = 58,
		TEXTURE_SRC_CODE_DOF_HALF_PREPASS = 59,
		TEXTURE_SRC_CODE_DOF_HALF_COLOR = 60,
		TEXTURE_SRC_CODE_DOF_HALF_ALPHA = 61,
		TEXTURE_SRC_CODE_DOF_TILE0 = 62,
		TEXTURE_SRC_CODE_DOF_TILE1 = 63,
		TEXTURE_SRC_CODE_SMAA_LUMA = 64,
		TEXTURE_SRC_CODE_SMAA_EDGES = 65,
		TEXTURE_SRC_CODE_SMAA_BLEND = 66,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_COLOR_0 = 67,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_COLOR_1 = 68,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_LUMA_1 = 69,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_LUMA_2 = 70,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_LUMA_3 = 71,
		TEXTURE_SRC_CODE_SMAA_FILMIC_HISTORY = 72,
		TEXTURE_SRC_CODE_SMAA_VELOCITY = 73,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_FILTER_MASK_0 = 74,
		TEXTURE_SRC_CODE_SMAA_TEMPORAL_FILTER_MASK_1 = 75,
		TEXTURE_SRC_CODE_SCENE_SPECULAR = 76,
		TEXTURE_SRC_CODE_SCENE_ALBEDO = 77,
		TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA = 78,
		TEXTURE_SRC_CODE_UI3D = 79,
		TEXTURE_SRC_CODE_FONT_CACHE = 88,
		TEXTURE_SRC_CODE_FOG_SPLINE = 89,
		TEXTURE_SRC_CODE_FOG_SPLINE_BLEND_FROM = 90,
		TEXTURE_SRC_CODE_AIR_LIGHT_F_TABLE = 91,
		TEXTURE_SRC_CODE_AIR_LIGHT_LOW_RES = 92,
		TEXTURE_SRC_CODE_AIR_LIGHT_LOW_RES_DEPTH = 93,
		TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_2D = 94,
		TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_3D = 95,
		TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_2D = 96,
		TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_3D = 97,
		TEXTURE_SRC_CODE_EFFECT_LIGHTMAP = 98,
		TEXTURE_SRC_CODE_PROCEDURAL_EMULATOR = 99,
		TEXTURE_SRC_CODE_DOF_HALF_PREVIOUS_PREPASS = 100,
		TEXTURE_SRC_CODE_DOF_HALF_PREVIOUS_COLOR = 101,
		TEXTURE_SRC_CODE_COUNT = 102,
	};

	struct MaterialArgumentCodeConst
	{
		unsigned short index;
		unsigned char firstRow;
		unsigned char rowCount;
	};

	union MaterialArgumentDef
	{
		float* literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	struct MaterialShaderArgument
	{
		unsigned char type;
		unsigned char shader;
		unsigned short dest;
		MaterialArgumentDef u;
	};
	static_assert(sizeof(MaterialShaderArgument) == 0x10);

	enum CustomBufferFlags : std::uint64_t
	{
		CUSTOM_BUFFER_PER_PRIM = 0x1,
		CUSTOM_BUFFER_PER_OBJECT = 0x2,
		CUSTOM_BUFFER_PER_STABLE = 0x8,
		CUSTOM_BUFFER_MATERIAL = 0x10,
		CUSTOM_BUFFER_INSTANCE_MATRICES = 0x8000, // 8 cb7 rows per instance
		CUSTOM_BUFFER_INSTANCE_MATRICES_LIGHTING = 0x10000, // 9 cb7 rows per instance, row 8 .x is the model lighting index
		CUSTOM_BUFFER_SLF = 0x40000, // binds cb5
		CUSTOM_BUFFER_FORWARD_LIGHTS = 0xC0, // pixel shader t16 light records and t17 spot shadow records
		CUSTOM_BUFFER_LIGHT_TILE_ZBINS = 0x100000, // pixel shader t25 light z bins
		CUSTOM_BUFFER_LIGHT_CLUSTER_BITS = 0x200000, // pixel shader t24 froxel light bits
		CUSTOM_BUFFER_LIGHT_TILE_BITS = 0x400000, // pixel shader t23 light bits of each 8x8 pixel tile

		CUSTOM_BUFFER_FORWARD_LIGHTING = 0x5000C0, // FORWARD_LIGHTS | LIGHT_TILE_BITS | LIGHT_TILE_ZBINS
		CUSTOM_BUFFER_LIGHT_GRID_VOLUME = 0x3800000, // vertex shader t5 index table, t6 transforms, t7 volumes
		CUSTOM_BUFFER_LIGHT_GRID_PROBES = 0xC000000, // probe buffer t8 of the "lgv0d" passes
		CUSTOM_BUFFER_SPRITES = 0x80000000, // raw t12 with the 64 byte fx sprite records
	};
	static_assert(CUSTOM_BUFFER_FORWARD_LIGHTING == (CUSTOM_BUFFER_FORWARD_LIGHTS | CUSTOM_BUFFER_LIGHT_TILE_BITS | CUSTOM_BUFFER_LIGHT_TILE_ZBINS));

	enum CustomSamplerFlags : std::uint8_t
	{
		CUSTOM_SAMPLER_FLAG_USE_REFLECTION_PROBE = 0x1, // pixel shader t1
		CUSTOM_SAMPLER_FLAG_USE_LIGHTMAP_PRIMARY = 0x2, // pixel shader t2
		CUSTOM_SAMPLER_FLAG_USE_LIGHTMAP_SECONDARY = 0x4, // pixel shader t3
	};

	enum MaterialPassModelVertexType : unsigned char
	{
		MODEL_VERTEX_TYPE_SURFACE = 0,
		MODEL_VERTEX_TYPE_INSTANCE_DATA = 1,
		MODEL_VERTEX_TYPE_VERTEX_LIGHTING = 2,
		MODEL_VERTEX_TYPE_LIGHTMAP = 3,
		MODEL_VERTEX_TYPE_SELF_VISIBILITY = 4, // also uploads the light grid per prim constants
		MODEL_VERTEX_TYPE_PREV_FRAME = 5, // also uploads the previous frame world matrix
	};

	struct MaterialPass
	{
		MaterialVertexShader* vertexShader;
		MaterialVertexDeclaration* vertexDecl;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		MaterialPixelShader* pixelShader;
		unsigned char pixelOutputMask;
		unsigned char perPrimArgCount;
		unsigned char perObjArgCount;
		unsigned char volatileArgCount;
		unsigned char stableArgCount;
		char __pad0;
		unsigned short perPrimArgSize;
		unsigned short perObjArgSize;
		unsigned short volatileArgSize;
		unsigned short stableArgSize;
		unsigned short zone;
		unsigned char perPrimConstantBuffer;
		unsigned char perObjConstantBuffer;
		unsigned char volatileConstantBuffer;
		unsigned char stableConstantBuffer;
		char __pad1[4];
		std::uint64_t customBufferFlags;
		unsigned char customSamplerFlags;
		MaterialPassModelVertexType modelVertexType;
		char __pad2[6];
		MaterialShaderArgument* args;
	};
	static_assert(sizeof(MaterialPass) == 0x58);
	static_assert(offsetof(MaterialPass, modelVertexType) == 0x49);
	static_assert(offsetof(MaterialPass, zone) == 0x36);
	static_assert(offsetof(MaterialPass, perPrimConstantBuffer) == 0x38);
	static_assert(offsetof(MaterialPass, customBufferFlags) == 0x40);

	struct MaterialTechniqueHeader
	{
		const char* name;
		unsigned short flags;
		unsigned short passCount;
		char __pad0[4];
	};

	struct MaterialTechnique
	{
		MaterialTechniqueHeader hdr;
		MaterialPass passArray[1];
	};

	enum MaterialTechniqueType : std::int32_t
	{
		TECHNIQUE_ZPREPASS = 0,
		TECHNIQUE_ZPREPASS_VELOCITY_RIGID = 1,
		TECHNIQUE_ZPREPASS_VELOCITY_SKINNED = 2,
		TECHNIQUE_ZPREPASS_HIDIR = 3,
		TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID = 4,
		TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED = 5,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 6,
		TECHNIQUE_BUILD_SHADOWMAP_COLOR = 7,
		TECHNIQUE_UNLIT = 8,
		TECHNIQUE_UNLIT_SRGB_GAMMA = 9,
		TECHNIQUE_UNLIT_BT2020_PQ = 10,
		TECHNIQUE_EMISSIVE = 11,
		TECHNIQUE_EMISSIVE_DFOG = 12,
		TECHNIQUE_EMISSIVE_SHADOW = 13,
		TECHNIQUE_EMISSIVE_SHADOW_DFOG = 14,
		// the light grid source adds 0 (probe buffer), 4 (2d array) or 8 (3d volume), odd slots are the alfog variants
		TECHNIQUE_LIT = 15,
		TECHNIQUE_LIT_ALFOG = 16,
		TECHNIQUE_LIT_UNK_17 = 17,
		TECHNIQUE_LIT_UNK_18 = 18,
		TECHNIQUE_LIT_GRID_2D = 19,
		TECHNIQUE_LIT_GRID_2D_ALFOG = 20,
		TECHNIQUE_LIT_GRID_2D_UNK_21 = 21,
		TECHNIQUE_LIT_GRID_2D_UNK_22 = 22,
		TECHNIQUE_LIT_GRID_3D = 23,
		TECHNIQUE_LIT_GRID_3D_ALFOG = 24,
		TECHNIQUE_LIT_GRID_3D_UNK_25 = 25,
		TECHNIQUE_LIT_GRID_3D_UNK_26 = 26,
		TECHNIQUE_LIT_BEGIN = TECHNIQUE_LIT,
		TECHNIQUE_LIT_END = 27,
		TECHNIQUE_WIREFRAME_SOLID = 32,
		TECHNIQUE_VELOCITY_RIGID = 36,
		TECHNIQUE_VELOCITY_SKINNED = 37,

		TECHNIQUE_GROUP_SIZE = 38,
		TECHNIQUE_STANDARD_BEGIN = 0,
		TECHNIQUE_INSTANCED_BEGIN = 38,
		TECHNIQUE_NO_TESS_BEGIN = 76,
	};

	constexpr auto technique_count = 114;

	// same formats as h1
	enum MaterialWorldVertexFormat : std::int32_t
	{
		MTL_WORLDVERT_T1 = 0x0,
		MTL_WORLDVERT_T2N1D0 = 0x1,
		MTL_WORLDVERT_T2N1D1 = 0x2,
		MTL_WORLDVERT_T2N1D2 = 0x3,
		MTL_WORLDVERT_T2N2D0 = 0x4,
		MTL_WORLDVERT_T2N2D1 = 0x5,
		MTL_WORLDVERT_T2N2D2 = 0x6,
		MTL_WORLDVERT_T3N1D0 = 0x7,
		MTL_WORLDVERT_T3N1D1 = 0x8,
		MTL_WORLDVERT_T3N1D2 = 0x9,
		MTL_WORLDVERT_T3N1D3 = 0xA,
		MTL_WORLDVERT_T3N2D0 = 0xB,
		MTL_WORLDVERT_T3N2D1 = 0xC,
		MTL_WORLDVERT_T3N2D2 = 0xD,
		MTL_WORLDVERT_T3N2D3 = 0xE,
		MTL_WORLDVERT_T3N3D0 = 0xF,
		MTL_WORLDVERT_T3N3D1 = 0x10,
		MTL_WORLDVERT_T3N3D2 = 0x11,
		MTL_WORLDVERT_T3N3D3 = 0x12,
		MTL_WORLDVERT_T4N1D0 = 0x13,
		MTL_WORLDVERT_T4N1D1 = 0x14,
		MTL_WORLDVERT_T4N1D2 = 0x15,
		MTL_WORLDVERT_T4N1D3 = 0x16,
		MTL_WORLDVERT_T4N1D4 = 0x17,
		MTL_WORLDVERT_T4N2D0 = 0x18,
		MTL_WORLDVERT_T4N2D1 = 0x19,
		MTL_WORLDVERT_T4N2D2 = 0x1A,
		MTL_WORLDVERT_T4N2D3 = 0x1B,
		MTL_WORLDVERT_T4N2D4 = 0x1C,
		MTL_WORLDVERT_T4N3D0 = 0x1D,
		MTL_WORLDVERT_T4N3D1 = 0x1E,
		MTL_WORLDVERT_T4N3D2 = 0x1F,
		MTL_WORLDVERT_T4N3D3 = 0x20,
		MTL_WORLDVERT_T4N3D4 = 0x21,
		MTL_WORLDVERT_T4N4D0 = 0x22,
		MTL_WORLDVERT_T4N4D1 = 0x23,
		MTL_WORLDVERT_T4N4D2 = 0x24,
		MTL_WORLDVERT_T4N4D3 = 0x25,
		MTL_WORLDVERT_T4N4D4 = 0x26,
		MTL_WORLDVERT_T5N1D0 = 0x27,
		MTL_WORLDVERT_T5N1D1 = 0x28,
		MTL_WORLDVERT_T5N1D2 = 0x29,
		MTL_WORLDVERT_T5N1D3 = 0x2A,
		MTL_WORLDVERT_T5N1D4 = 0x2B,
		MTL_WORLDVERT_T5N1D5 = 0x2C,
		MTL_WORLDVERT_T5N2D0 = 0x2D,
		MTL_WORLDVERT_T5N2D1 = 0x2E,
		MTL_WORLDVERT_T5N2D2 = 0x2F,
		MTL_WORLDVERT_T5N2D3 = 0x30,
		MTL_WORLDVERT_T5N2D4 = 0x31,
		MTL_WORLDVERT_T5N2D5 = 0x32,
		MTL_WORLDVERT_T5N3D0 = 0x33,
		MTL_WORLDVERT_T5N3D1 = 0x34,
		MTL_WORLDVERT_T5N3D2 = 0x35,
		MTL_WORLDVERT_T5N3D3 = 0x36,
		MTL_WORLDVERT_T5N3D4 = 0x37,
		MTL_WORLDVERT_T5N3D5 = 0x38,
		MTL_WORLDVERT_T5N4D0 = 0x39,
		MTL_WORLDVERT_T5N4D1 = 0x3A,
		MTL_WORLDVERT_T5N4D2 = 0x3B,
		MTL_WORLDVERT_T5N4D3 = 0x3C,
		MTL_WORLDVERT_T5N4D4 = 0x3D,
		MTL_WORLDVERT_T5N4D5 = 0x3E,
		MTL_WORLDVERT_T5N5D0 = 0x3F,
		MTL_WORLDVERT_T5N5D1 = 0x40,
		MTL_WORLDVERT_T5N5D2 = 0x41,
		MTL_WORLDVERT_T5N5D3 = 0x42,
		MTL_WORLDVERT_T5N5D4 = 0x43,
		MTL_WORLDVERT_T5N5D5 = 0x44,
		MTL_WORLDVERT_T6N1D0 = 0x45,
		MTL_WORLDVERT_T6N1D1 = 0x46,
		MTL_WORLDVERT_T6N1D2 = 0x47,
		MTL_WORLDVERT_T6N1D3 = 0x48,
		MTL_WORLDVERT_T6N1D4 = 0x49,
		MTL_WORLDVERT_T6N1D5 = 0x4A,
		MTL_WORLDVERT_T6N1D6 = 0x4B,
		MTL_WORLDVERT_T6N2D0 = 0x4C,
		MTL_WORLDVERT_T6N2D1 = 0x4D,
		MTL_WORLDVERT_T6N2D2 = 0x4E,
		MTL_WORLDVERT_T6N2D3 = 0x4F,
		MTL_WORLDVERT_T6N2D4 = 0x50,
		MTL_WORLDVERT_T6N2D5 = 0x51,
		MTL_WORLDVERT_T6N2D6 = 0x52,
		MTL_WORLDVERT_T6N3D0 = 0x53,
		MTL_WORLDVERT_T6N3D1 = 0x54,
		MTL_WORLDVERT_T6N3D2 = 0x55,
		MTL_WORLDVERT_T6N3D3 = 0x56,
		MTL_WORLDVERT_T6N3D4 = 0x57,
		MTL_WORLDVERT_T6N3D5 = 0x58,
		MTL_WORLDVERT_T6N3D6 = 0x59,
		MTL_WORLDVERT_T6N4D0 = 0x5A,
		MTL_WORLDVERT_T6N4D1 = 0x5B,
		MTL_WORLDVERT_T6N4D2 = 0x5C,
		MTL_WORLDVERT_T6N4D3 = 0x5D,
		MTL_WORLDVERT_T6N4D4 = 0x5E,
		MTL_WORLDVERT_T6N4D5 = 0x5F,
		MTL_WORLDVERT_T6N4D6 = 0x60,
		MTL_WORLDVERT_T6N5D0 = 0x61,
		MTL_WORLDVERT_T6N5D1 = 0x62,
		MTL_WORLDVERT_T6N5D2 = 0x63,
		MTL_WORLDVERT_T6N5D3 = 0x64,
		MTL_WORLDVERT_T6N5D4 = 0x65,
		MTL_WORLDVERT_T6N5D5 = 0x66,
		MTL_WORLDVERT_T6N5D6 = 0x67,
		MTL_WORLDVERT_T7N1D0 = 0x68,
		MTL_WORLDVERT_T7N1D1 = 0x69,
		MTL_WORLDVERT_T7N1D2 = 0x6A,
		MTL_WORLDVERT_T7N1D3 = 0x6B,
		MTL_WORLDVERT_T7N1D4 = 0x6C,
		MTL_WORLDVERT_T7N1D5 = 0x6D,
		MTL_WORLDVERT_T7N1D6 = 0x6E,
		MTL_WORLDVERT_T7N1D7 = 0x6F,
		MTL_WORLDVERT_T7N2D0 = 0x70,
		MTL_WORLDVERT_T7N2D1 = 0x71,
		MTL_WORLDVERT_T7N2D2 = 0x72,
		MTL_WORLDVERT_T7N2D3 = 0x73,
		MTL_WORLDVERT_T7N2D4 = 0x74,
		MTL_WORLDVERT_T7N2D5 = 0x75,
		MTL_WORLDVERT_T7N2D6 = 0x76,
		MTL_WORLDVERT_T7N2D7 = 0x77,
		MTL_WORLDVERT_T7N3D0 = 0x78,
		MTL_WORLDVERT_T7N3D1 = 0x79,
		MTL_WORLDVERT_T7N3D2 = 0x7A,
		MTL_WORLDVERT_T7N3D3 = 0x7B,
		MTL_WORLDVERT_T7N3D4 = 0x7C,
		MTL_WORLDVERT_T7N3D5 = 0x7D,
		MTL_WORLDVERT_T7N3D6 = 0x7E,
		MTL_WORLDVERT_T7N3D7 = 0x7F,
		MTL_WORLDVERT_T7N4D0 = 0x80,
		MTL_WORLDVERT_T7N4D1 = 0x81,
		MTL_WORLDVERT_T7N4D2 = 0x82,
		MTL_WORLDVERT_T7N4D3 = 0x83,
		MTL_WORLDVERT_T7N4D4 = 0x84,
		MTL_WORLDVERT_T7N4D5 = 0x85,
		MTL_WORLDVERT_T7N4D6 = 0x86,
		MTL_WORLDVERT_T7N4D7 = 0x87,
		MTL_WORLDVERT_T7N5D0 = 0x88,
		MTL_WORLDVERT_T7N5D1 = 0x89,
		MTL_WORLDVERT_T7N5D2 = 0x8A,
		MTL_WORLDVERT_T7N5D3 = 0x8B,
		MTL_WORLDVERT_T7N5D4 = 0x8C,
		MTL_WORLDVERT_T7N5D5 = 0x8D,
		MTL_WORLDVERT_T7N5D6 = 0x8E,
		MTL_WORLDVERT_T7N5D7 = 0x8F,
		MTL_WORLDVERT_T8N1D0 = 0x90,
		MTL_WORLDVERT_T8N1D1 = 0x91,
		MTL_WORLDVERT_T8N1D2 = 0x92,
		MTL_WORLDVERT_T8N1D3 = 0x93,
		MTL_WORLDVERT_T8N1D4 = 0x94,
		MTL_WORLDVERT_T8N1D5 = 0x95,
		MTL_WORLDVERT_T8N1D6 = 0x96,
		MTL_WORLDVERT_T8N1D7 = 0x97,
		MTL_WORLDVERT_T8N1D8 = 0x98,
		MTL_WORLDVERT_T8N2D0 = 0x99,
		MTL_WORLDVERT_T8N2D1 = 0x9A,
		MTL_WORLDVERT_T8N2D2 = 0x9B,
		MTL_WORLDVERT_T8N2D3 = 0x9C,
		MTL_WORLDVERT_T8N2D4 = 0x9D,
		MTL_WORLDVERT_T8N2D5 = 0x9E,
		MTL_WORLDVERT_T8N2D6 = 0x9F,
		MTL_WORLDVERT_T8N2D7 = 0xA0,
		MTL_WORLDVERT_T8N2D8 = 0xA1,
		MTL_WORLDVERT_T8N3D0 = 0xA2,
		MTL_WORLDVERT_T8N3D1 = 0xA3,
		MTL_WORLDVERT_T8N3D2 = 0xA4,
		MTL_WORLDVERT_T8N3D3 = 0xA5,
		MTL_WORLDVERT_T8N3D4 = 0xA6,
		MTL_WORLDVERT_T8N3D5 = 0xA7,
		MTL_WORLDVERT_T8N3D6 = 0xA8,
		MTL_WORLDVERT_T8N3D7 = 0xA9,
		MTL_WORLDVERT_T8N3D8 = 0xAA,
		MTL_WORLDVERT_T8N4D0 = 0xAB,
		MTL_WORLDVERT_T8N4D1 = 0xAC,
		MTL_WORLDVERT_T8N4D2 = 0xAD,
		MTL_WORLDVERT_T8N4D3 = 0xAE,
		MTL_WORLDVERT_T8N4D4 = 0xAF,
		MTL_WORLDVERT_T8N4D5 = 0xB0,
		MTL_WORLDVERT_T8N4D6 = 0xB1,
		MTL_WORLDVERT_T8N4D7 = 0xB2,
		MTL_WORLDVERT_T8N4D8 = 0xB3,
		MTL_WORLDVERT_T8N5D0 = 0xB4,
		MTL_WORLDVERT_T8N5D1 = 0xB5,
		MTL_WORLDVERT_T8N5D2 = 0xB6,
		MTL_WORLDVERT_T8N5D3 = 0xB7,
		MTL_WORLDVERT_T8N5D4 = 0xB8,
		MTL_WORLDVERT_T8N5D5 = 0xB9,
		MTL_WORLDVERT_T8N5D6 = 0xBA,
		MTL_WORLDVERT_T8N5D7 = 0xBB,
		MTL_WORLDVERT_T8N5D8 = 0xBC,
		MTL_WORLDVERT_T9N1D0 = 0xBD,
		MTL_WORLDVERT_T9N1D1 = 0xBE,
		MTL_WORLDVERT_T9N1D2 = 0xBF,
		MTL_WORLDVERT_T9N1D3 = 0xC0,
		MTL_WORLDVERT_T9N1D4 = 0xC1,
		MTL_WORLDVERT_T9N1D5 = 0xC2,
		MTL_WORLDVERT_T9N1D6 = 0xC3,
		MTL_WORLDVERT_T9N1D7 = 0xC4,
		MTL_WORLDVERT_T9N1D8 = 0xC5,
		MTL_WORLDVERT_T9N2D0 = 0xC6,
		MTL_WORLDVERT_T9N2D1 = 0xC7,
		MTL_WORLDVERT_T9N2D2 = 0xC8,
		MTL_WORLDVERT_T9N2D3 = 0xC9,
		MTL_WORLDVERT_T9N2D4 = 0xCA,
		MTL_WORLDVERT_T9N2D5 = 0xCB,
		MTL_WORLDVERT_T9N2D6 = 0xCC,
		MTL_WORLDVERT_T9N2D7 = 0xCD,
		MTL_WORLDVERT_T9N2D8 = 0xCE,
		MTL_WORLDVERT_T9N3D0 = 0xCF,
		MTL_WORLDVERT_T9N3D1 = 0xD0,
		MTL_WORLDVERT_T9N3D2 = 0xD1,
		MTL_WORLDVERT_T9N3D3 = 0xD2,
		MTL_WORLDVERT_T9N3D4 = 0xD3,
		MTL_WORLDVERT_T9N3D5 = 0xD4,
		MTL_WORLDVERT_T9N3D6 = 0xD5,
		MTL_WORLDVERT_T9N3D7 = 0xD6,
		MTL_WORLDVERT_T9N3D8 = 0xD7,
		MTL_WORLDVERT_T9N4D0 = 0xD8,
		MTL_WORLDVERT_T9N4D1 = 0xD9,
		MTL_WORLDVERT_T9N4D2 = 0xDA,
		MTL_WORLDVERT_T9N4D3 = 0xDB,
		MTL_WORLDVERT_T9N4D4 = 0xDC,
		MTL_WORLDVERT_T9N4D5 = 0xDD,
		MTL_WORLDVERT_T9N4D6 = 0xDE,
		MTL_WORLDVERT_T9N4D7 = 0xDF,
		MTL_WORLDVERT_T9N4D8 = 0xE0,
		MTL_WORLDVERT_T9N5D0 = 0xE1,
		MTL_WORLDVERT_T9N5D1 = 0xE2,
		MTL_WORLDVERT_T9N5D2 = 0xE3,
		MTL_WORLDVERT_T9N5D3 = 0xE4,
		MTL_WORLDVERT_T9N5D4 = 0xE5,
		MTL_WORLDVERT_T9N5D5 = 0xE6,
		MTL_WORLDVERT_T9N5D6 = 0xE7,
		MTL_WORLDVERT_T9N5D7 = 0xE8,
		MTL_WORLDVERT_T9N5D8 = 0xE9,
		MTL_WORLDVERT_COUNT = 0xEA,
	};

	struct MaterialTechniqueSet
	{
		const char* name;
		unsigned short flags;
		unsigned char worldVertFormat;
		unsigned char preDisplacementOnlyCount;
		char __pad0[4];
		MaterialTechnique* techniques[technique_count];
	};
	static_assert(sizeof(MaterialTechniqueSet) == 0x3A0);

	struct SndPhysPreset
	{
		const char* name;
		char __pad0[0x30];
	};
	static_assert(sizeof(SndPhysPreset) == 0x38);

	struct PhysConstraint
	{
		const char* name;
		char __pad0[0x18];
	};
	static_assert(sizeof(PhysConstraint) == 0x20);

	struct SndCurve
	{
		bool isDefault;
		union
		{
			const char* filename;
			const char* name;
		};
		unsigned short knotCount;
		float knots[16][2];
	};
	static_assert(sizeof(SndCurve) == 0x98);

	struct DistCurve
	{
		const char* name;
		char __pad0[0x108];
	};
	static_assert(sizeof(DistCurve) == 0x110);

	struct SndContext
	{
		const char* name;
		char __pad0[0x8];
	};
	static_assert(sizeof(SndContext) == 0x10);

	struct SndAliasParameterModifier
	{
		const char* name;
		char __pad0[0x10];
	};
	static_assert(sizeof(SndAliasParameterModifier) == 0x18);

	struct SndAliasCombatCone
	{
		const char* name;
		char __pad0[0x18];
	};
	static_assert(sizeof(SndAliasCombatCone) == 0x20);

	struct SndDriverGlobals
	{
		const char* name;
		char __pad0[0x10];
	};
	static_assert(sizeof(SndDriverGlobals) == 0x18);

	struct ReverbPreset
	{
		const char* name;
		char __pad0[0x60];
	};
	static_assert(sizeof(ReverbPreset) == 0x68);

	enum XSurfaceSharedFlags : std::uint16_t
	{
		XSURFACE_SHARED_STREAMED = 0x1,
	};

	struct XSurfaceShared
	{
		std::uint16_t flags;
		char hash[32];
		char __pad1[2];
		unsigned int indexCount;
		unsigned short vertCount;
		unsigned short subdivCount;
		unsigned int dataSize;
		char* data;
		char* subdivData;
		char __pad2[8];
	};
	static_assert(sizeof(XSurfaceShared) == 0x48);
	static_assert(offsetof(XSurfaceShared, indexCount) == 0x24);

	enum NetConstStringType
	{
		NETCONSTSTRINGTYPE_XMODEL = 0, // mdl
		NETCONSTSTRINGTYPE_MATERIAL = 1, // mat
		NETCONSTSTRINGTYPE_RUMBLE = 2, // rmb
		NETCONSTSTRINGTYPE_VEHICLES = 3, // veh
		NETCONSTSTRINGTYPE_FX = 4, // vfx
		NETCONSTSTRINGTYPE_LOCSTRING = 5, // loc
		NETCONSTSTRINGTYPE_SOUNDALIAS = 6, // snd
		NETCONSTSTRINGTYPE_SUBMIX = 7, // sbx
		NETCONSTSTRINGTYPE_SHOCK = 8, // shk
		NETCONSTSTRINGTYPE_SCRIPTMENU = 9, // mnu
		NETCONSTSTRINGTYPE_CLIENT_TAGS = 10, // tag
		NETCONSTSTRINGTYPE_POR = 11, // por, read by an agent script function
		NETCONSTSTRINGTYPE_HEADICON = 12, // hic
		NETCONSTSTRINGTYPE_NAMEPLATE = 13, // nps
		NETCONSTSTRINGTYPE_MINIMAPICON = 14, // mic
		NETCONSTSTRINGTYPE_LOCSELMAT = 15, // sel
		NETCONSTSTRINGTYPE_WEAPON = 16, // wep
		NETCONSTSTRINGTYPE_ATTACHMENT = 17, // att
		NETCONSTSTRINGTYPE_HINTSTRING = 18, // hnt
		NETCONSTSTRINGTYPE_ANIM = 19, // anm
		NETCONSTSTRINGTYPE_ANIMCLASS = 20, // acl
		NETCONSTSTRINGTYPE_LUI = 21, // lui
		NETCONSTSTRINGTYPE_LASER = 22, // lsr
		NETCONSTSTRINGTYPE_CLIENT_SCRIPT_STRING = 23, // css, notify names passed to lua
		NETCONSTSTRINGTYPE_ASSET_COUNT = 24,
	};

	struct NetConstStrings
	{
		const char* name;
		NetConstStringType stringType;
		int sourceType;
		unsigned int entryCount;
		const char** stringList;
	};
	static_assert(sizeof(NetConstStrings) == 0x20);

	struct GfxImageLoadDef
	{
		char levelCount;
		char numElements;
		char pad[2];
		int flags;
		int format;
		int resourceSize;
		char data[1];
	};

	struct GfxTexture
	{
		union
		{
			ID3D11Texture1D* linemap;
			ID3D11Texture2D* map;
			ID3D11Texture3D* volmap;
			ID3D11Texture2D* cubemap;
			GfxImageLoadDef* loadDef;
		};
		ID3D11ShaderResourceView* shaderView;
		ID3D11ShaderResourceView* shaderViewAlternate;
	};

	enum MapType : std::uint8_t
	{
		MAPTYPE_NONE = 0x0,
		MAPTYPE_INVALID1 = 0x1,
		MAPTYPE_1D = 0x2,
		MAPTYPE_2D = 0x3,
		MAPTYPE_3D = 0x4,
		MAPTYPE_CUBE = 0x5,
		MAPTYPE_ARRAY = 0x6,
		MAPTYPE_COUNT = 0x7,
	};

	struct GfxImageStreamLevelCountAndSize
	{
		int pixelSize : 26;
		int levelCount : 6;
	};

	struct GfxImageStreamData
	{
		unsigned short width;
		unsigned short height;
		GfxImageStreamLevelCountAndSize levelCountAndSize;
	};

	enum TextureSemantic : std::uint8_t
	{
		TS_COLOR_MAP = 0x2,
		TS_NORMAL_MAP = 0x5,
		TS_SPECULAR_MAP = 0x8,
		TS_RESIDENT = 0xC,
		TS_ENV_BRDF_LUT = 0xF,
		TS_EMISSIVE_MAP = 0x11,
	};

	struct GfxImage
	{
		const char* name;
		GfxTexture texture;
		unsigned char* pixelData;
		GfxImageStreamData streams[4];
		DXGI_FORMAT imageFormat;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		unsigned int dataLen1;
		MapType mapType;
		unsigned char semantic;
		unsigned char category;
		unsigned char flags;
		unsigned char levelCount;
		unsigned char picmip;
		unsigned char streamed; // the engine reads 4 stream keys for each image that has this set
		char __pad0[1];
	};
	static_assert(sizeof(GfxImage) == 0x60);
	static_assert(offsetof(GfxImage, streamed) == 94);
	static_assert(offsetof(GfxImage, pixelData) == 32);
	static_assert(offsetof(GfxImage, dataLen1) == 84);
	static_assert(offsetof(GfxImage, semantic) == 89);

	struct PhysPresetSound
	{
		const char* name;
		char __pad0[8];
	};

	struct PhysPreset
	{
		const char* name;
		float density;
		float bounce;
		float friction;
		float unk_0x14; // 1.0 in stock
		float bulletForceScale;
		float explosiveForceScale;
		float unk_0x20[2];
		unsigned short collisionMask;
		unsigned char unk_0x2A;
		unsigned char unk_0x2B;
		char __pad0[4];
		const char* sndAliasPrefix;
		char __pad1[8];
		PhysPresetSound sounds[3];
		float unk_0x70[4];
		SndPhysPreset* sndPhysPreset;
	};
	static_assert(sizeof(PhysPreset) == 0x88);
	static_assert(offsetof(PhysPreset, sounds) == 64);
	static_assert(offsetof(PhysPreset, explosiveForceScale) == 0x1C);
	static_assert(offsetof(PhysPreset, sndAliasPrefix) == 0x30);

	struct LoadedSoundStreamInfo
	{
		unsigned long long unknown0;
		const char* name;
		const char* fileName;
		XStreamFileKey streamKey;
	};
	static_assert(sizeof(LoadedSoundStreamInfo) == 0x28);

	enum SndAudioFormatType : std::uint8_t
	{
		SND_FORMAT_PCM = 0x1,
		SND_FORMAT_FLAC = 0x6,
	};

	struct SndAudioFormat
	{
		unsigned int dataLength;
		unsigned int sampleCount;
		unsigned short sampleRate;
		unsigned char channelCount;
		unsigned char bitsPerSample;
		unsigned short blockSize;
		SndAudioFormatType format;
		unsigned char unknown15;
	};
	static_assert(sizeof(SndAudioFormat) == 0x10);

#pragma pack(push, 1)
	struct SoundFileTocHeader
	{
		char magic[8];
		unsigned int version;
		unsigned int entryCount;
	};
	static_assert(sizeof(SoundFileTocHeader) == 0x10);

	struct SoundFileTocEntry
	{
		unsigned long long hash[2]; // the game uses only hash[0]
		unsigned long long offset;
		unsigned int size;
		unsigned short fileIndex;
	};
	static_assert(sizeof(SoundFileTocEntry) == 0x1E);

	struct XPakTocHeader
	{
		char magic[8];
		unsigned int version;
		unsigned int entryCount;
	};
	static_assert(sizeof(XPakTocHeader) == 0x10);

	struct XPakTocEntry
	{
		unsigned char hash[16];
		unsigned long long offset;
		unsigned int size;
		unsigned short fileIndex;
	};
	static_assert(sizeof(XPakTocEntry) == 0x1E);
#pragma pack(pop)

	struct LoadedSound
	{
		const char* name;
		LoadedSoundStreamInfo info;
		SndAudioFormat audioFormat;
		char* data;
		int dataSize;
		unsigned short seekTableCount;
		char __pad1[2];
		unsigned int* seekTable;
	};
	static_assert(sizeof(LoadedSound) == 0x58);

	enum SoundFileType : std::uint8_t
	{
		SAT_LOADED = 0x1,
		SAT_STREAMED = 0x2,
		SAT_PRIMED = 0x3,
	};

	struct StreamedSound
	{
		LoadedSoundStreamInfo info;
		SndAudioFormat audioFormat;
	};
	static_assert(sizeof(StreamedSound) == 0x38);

	struct PrimedSound
	{
		LoadedSound* loadedPart;
		LoadedSoundStreamInfo streamedPart;
		unsigned long long unknown48;
	};
	static_assert(sizeof(PrimedSound) == 0x38);

	union SoundFileRef
	{
		LoadedSound* loadSnd;
		StreamedSound streamSnd;
		PrimedSound primedSnd;
	};

	struct SoundFile
	{
		SoundFileType type;
		bool exists;
		SoundFileRef u;
	};
	static_assert(sizeof(SoundFile) == 0x40);

	struct SpeakerLevels
	{
		float levels[2];
		unsigned char speaker;
		unsigned char numLevels;
	};
	static_assert(sizeof(SpeakerLevels) == 0xC);

	struct SpeakerMap
	{
		const char* name;
		SpeakerLevels channelMaps[2][8];
	};
	static_assert(sizeof(SpeakerMap) == 0xC8);

	enum SndAliasFlags : std::uint32_t
	{
		SND_ALIAS_FLAG_LOOPING = 0x1,
		SND_ALIAS_FLAG_LFE_PERCENTAGE = 0x4,
		SND_ALIAS_FLAG_FULL_DRY_LEVEL = 0x8,
		SND_ALIAS_FLAG_WET_MIX_OVERRIDE = 0x10,
		SND_ALIAS_FLAG_RANDOM_LOOPING = 0x20,
		SND_ALIAS_FLAG_2D = 0x80,
		SND_ALIAS_FLAG_PAUSABLE = 0x100,
		SND_ALIAS_FLAG_USE_CONTEXT_LIST = 0x800,
		SND_ALIAS_FLAG_DMONO = 0x1000,
		SND_ALIAS_FLAG_TYPE_MASK = 0x70000, // SoundFileType << 16
	};

	enum SndVariationType : std::uint8_t
	{
		SND_VARIATION_STICKY = 0x0,
		SND_VARIATION_PICKY = 0x1,
		SND_VARIATION_ORDERED = 0x2,
		SND_VARIATION_SHUFFLE = 0x3,
		SND_VARIATION_WEIGHTED = 0x4,
	};

	enum SndParameterModifierOp : std::uint8_t
	{
		SND_MODIFIER_OP_ADD = 0x0,
		SND_MODIFIER_OP_REVERSE_SUBTRACT = 0x1,
		SND_MODIFIER_OP_SUBTRACT = 0x2,
		SND_MODIFIER_OP_MULTIPLY = 0x3,
		SND_MODIFIER_OP_REVERSE_DIVIDE = 0x4,
		SND_MODIFIER_OP_DIVIDE = 0x5,
		SND_MODIFIER_OP_NONE = 0x6,
	};

	struct snd_alias_t
	{
		const char* aliasName;
		const char* subtitle;
		const char* mixerGroup;
		const char* secondaryAliasName;
		const char* chainAliasName;
		SoundFile* soundFile;
		const char* squelchName;
		int unknown56;
		short polyCount;
		unsigned char dspBusIndex;
		unsigned char priority;
		int playCount;
		int polyClass;
		float volMin;
		float volMax;
		short volModIndex;
		float pitchMin;
		float pitchMax;
		float distMin;
		float distMax;
		float velocityMin;
		int flags; // SndAliasFlags
		unsigned char masterPriority;
		float masterPercentage;
		float slavePercentage;
		float playbackPercentage;
		float probability;
		SndVariationType variationType;
		SndContext* sndContext;
		unsigned char sequence;
		float lfePercentage;
		float centerPercentage;
		int startDelay;
		SndCurve* volumeFalloffCurve;
		DistCurve* lpfCurve;
		SndCurve* reverbSendCurve;
		SpeakerMap* speakerMap;
		float wetMixOverride;
		float focusPercentage;
		float smartpanDistance2d;
		float smartpanDistance3d;
		float smartpanAttenuation3d;
		float minSmartpan2dContribution;
		short stereo3DAngle;
		float stereo3DStart;
		float stereo3DEnd;
		float threshold;
		float unknown232;
		int unknown236;
		int lockedLoopTime;
		int unknown244;
		SndAliasParameterModifier* parameterModifiers[8]; // 0-5 vol, pitch and dist min and max, 6 position offset, 7 playbackPercentage
		SndParameterModifierOp parameterModifierOps[8];
		int length;
		int unknown324;
		SndAliasCombatCone* combatCone;
	};
	static_assert(sizeof(snd_alias_t) == 0x150);
	static_assert(offsetof(snd_alias_t, unknown56) == 56);
	static_assert(offsetof(snd_alias_t, volModIndex) == 80);
	static_assert(offsetof(snd_alias_t, pitchMin) == 84);
	static_assert(offsetof(snd_alias_t, flags) == 104);
	static_assert(offsetof(snd_alias_t, masterPercentage) == 112);
	static_assert(offsetof(snd_alias_t, variationType) == 128);
	static_assert(offsetof(snd_alias_t, sndContext) == 136);
	static_assert(offsetof(snd_alias_t, lfePercentage) == 148);
	static_assert(offsetof(snd_alias_t, speakerMap) == 184);
	static_assert(offsetof(snd_alias_t, stereo3DAngle) == 216);
	static_assert(offsetof(snd_alias_t, stereo3DStart) == 220);
	static_assert(offsetof(snd_alias_t, lockedLoopTime) == 240);
	static_assert(offsetof(snd_alias_t, parameterModifiers) == 248);
	static_assert(offsetof(snd_alias_t, parameterModifierOps) == 312);
	static_assert(offsetof(snd_alias_t, length) == 320);
	static_assert(offsetof(snd_alias_t, combatCone) == 328);

	struct snd_alias_context_list
	{
		unsigned char aliasOffset;
		unsigned char count;
	};
	static_assert(sizeof(snd_alias_context_list) == 2);

	struct snd_alias_list_t
	{
		const char* name;
		snd_alias_t* head;
		snd_alias_context_list* contextList;
		unsigned char count;
		unsigned char contextListCount;
		char __pad0[6];
	};
	static_assert(sizeof(snd_alias_list_t) == 0x20);

	struct pathlink_s
	{
		float fDist;
		unsigned short nodeNum;
		unsigned char disconnectCount;
		unsigned char negotiationLink;
		unsigned char flags;
		unsigned char ubBadPlaceCount[3];
	};
	static_assert(sizeof(pathlink_s) == 12);

	enum nodeType : std::uint16_t
	{
		NODE_ERROR = 0,
		NODE_PATHNODE = 1,
		NODE_COVER_STAND = 2,
		NODE_COVER_CROUCH = 3,
		NODE_COVER_CROUCH_WINDOW = 4,
		NODE_COVER_PRONE = 5,
		NODE_COVER_RIGHT = 6,
		NODE_COVER_LEFT = 7,
		NODE_COVER_MULTI = 8,
		NODE_AMBUSH = 9,
		NODE_EXPOSED = 10,
		NODE_CONCEALMENT_STAND = 11,
		NODE_CONCEALMENT_CROUCH = 12,
		NODE_CONCEALMENT_PRONE = 13,
		NODE_DOOR = 14,
		NODE_DOOR_INTERIOR = 15,
		NODE_SCRIPTED = 16,
		NODE_NEGOTIATION_BEGIN = 17,
		NODE_NEGOTIATION_END = 18,
		NODE_TURRET = 19,
		NODE_GUARD = 20,
		NODE_PATHNODE_3D = 21,
		NODE_COVER_UP_3D = 22,
		NODE_COVER_RIGHT_3D = 23,
		NODE_COVER_LEFT_3D = 24,
		NODE_EXPOSED_3D = 25,
		NODE_SCRIPTED_3D = 26,
		NODE_NEGOTIATION_BEGIN_3D = 27,
		NODE_NEGOTIATION_END_3D = 28,
		NODE_NUMTYPES = 29,
	};

	struct pathnode_yaworient_t
	{
		float fLocalAngle;
		float localForward[2];
	};

	union PathNodeOrientationUnion
	{
		pathnode_yaworient_t yaw_orient;
		float angles[3];
	};

	union PathNodeParentUnion
	{
		scr_string_t name; // until parentIndexResolved
		unsigned short index;
	};

	union PathNodeUseUnion
	{
		float minUseDistSq;
		int error;
	};

	struct pathnode_constant_t
	{
		nodeType type;
		unsigned int spawnflags;
		scr_string_t targetname;
		scr_string_t script_linkName;
		scr_string_t script_noteworthy;
		scr_string_t target;
		scr_string_t animscript;
		int animscriptfunc;
		float vLocalOrigin[3];
		PathNodeOrientationUnion orientation;
		PathNodeParentUnion parent;
		PathNodeUseUnion use;
		short wOverlapNode[2];
		unsigned short totalLinkCount;
		pathlink_s* Links;
	};
	static_assert(sizeof(pathnode_constant_t) == 0x50);
	static_assert(offsetof(pathnode_constant_t, vLocalOrigin) == 32);
	static_assert(offsetof(pathnode_constant_t, parent) == 56);
	static_assert(offsetof(pathnode_constant_t, wOverlapNode) == 64);
	static_assert(offsetof(pathnode_constant_t, totalLinkCount) == 68);
	static_assert(offsetof(pathnode_constant_t, Links) == 72);

	struct SentientHandle
	{
		unsigned short number;
		unsigned short infoIndex;
	};

	// runtime only, zero in zones
	struct pathnode_dynamic_t
	{
		SentientHandle pOwner[3]; // per team
		char __pad0[4];
		int iFreeTime;
		char __pad1[2];
		unsigned char userCount[3];
		char __pad2[87];
	};
	static_assert(sizeof(pathnode_dynamic_t) == 112);

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
	};
	static_assert(sizeof(pathnode_t) == 192);
	static_assert(offsetof(pathnode_t, dynamic.iFreeTime) == 0x60);
	static_assert(offsetof(pathnode_t, dynamic.userCount) == 0x66);

	struct pathnode_tree_t;

	struct pathnode_tree_nodes_t
	{
		int nodeCount;
		unsigned short* nodes;
	};

	union pathnode_tree_info_t
	{
		pathnode_tree_t* child[2];
		pathnode_tree_nodes_t s;
	};

	struct pathnode_tree_t
	{
		int axis;
		float dist;
		pathnode_tree_info_t u;
	};
	static_assert(sizeof(pathnode_tree_t) == 24);

	struct PathDynamicNodeGroup
	{
		unsigned short parentIndex;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
	};
	static_assert(sizeof(PathDynamicNodeGroup) == 16);

	struct PathData
	{
		const char* name;
		unsigned int nodeCount;
		pathnode_t* nodes;
		bool parentIndexResolved;
		unsigned short version;
		unsigned int visBytes;
		unsigned char* pathVis;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
		int dynamicNodeGroupCount;
		PathDynamicNodeGroup* dynamicNodeGroups;
		int exposureBytes;
		unsigned char* pathExposure;
		int noPeekVisBytes;
		unsigned char* pathNoPeekVis;
		int zoneCount;
		char __pad0[4]; // h1 zonesBytes, 0 in s2
		unsigned short* pathZones; // nodeCount entries when zoneCount
		int dynStatesBytes;
		unsigned char* pathDynStates;
	};
	static_assert(sizeof(PathData) == 0x88);
	static_assert(offsetof(PathData, version) == 26);
	static_assert(offsetof(PathData, visBytes) == 28);
	static_assert(offsetof(PathData, zoneCount) == 104);
	static_assert(offsetof(PathData, pathZones) == 112);

	enum MaterialStateFlags : std::uint8_t
	{
		STATE_FLAG_CULL_BACK = 0x1,
		STATE_FLAG_DECAL = 0x2,
		STATE_FLAG_WRITES_DEPTH = 0x4,
		STATE_FLAG_USES_DEPTH_BUFFER = 0x8,
		STATE_FLAG_USES_STENCIL_BUFFER = 0x10,
	};

	enum GfxCameraRegionType : std::uint8_t
	{
		CAMERA_REGION_LIT_OPAQUE = 0x0,
		CAMERA_REGION_LIT_OPAQUE_SSS = 0x1,
		CAMERA_REGION_LIT_DECAL = 0x2,
		CAMERA_REGION_LIT_TRANS = 0x4,
		CAMERA_REGION_EMISSIVE = 0x5,
		CAMERA_REGION_POST_TAA = 0x8, // weapon reticles
		CAMERA_REGION_NONE = 0x9,
	};

	struct GfxMaterialDrawSurfFields
	{
		unsigned __int64 objectBits;
		unsigned __int64 materialBits : 8;
		unsigned __int64 primarySortKey : 6; // always MaterialInfo::sortKey
		unsigned __int64 unused : 18;
		unsigned __int64 techsetHash : 32;
	};

	union GfxMaterialDrawSurf
	{
		GfxMaterialDrawSurfFields fields;
		std::uint64_t packed[2];
	};

	enum MaterialPrepassClass : unsigned char
	{
		PREPASS_CLASS_OPAQUE = 0,
		PREPASS_CLASS_ALPHA_TESTED = 1,
		PREPASS_CLASS_BLENDED = 2,
	};

	struct MaterialInfo
	{
		const char* name;
		unsigned char gameFlags;
		MaterialPrepassClass prepassClass;
		unsigned char sortKey;
		unsigned char unused; // 0 in stock
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		unsigned char textureAtlasFrameBlend;
		unsigned char textureAtlasAsArray;
		GfxMaterialDrawSurf drawSurf;
		unsigned int unkHash;
		unsigned short hashIndex; // runtime: the material index of the draw surfs
		unsigned char unk2; // runtime: 3 bits of the draw surf
		unsigned char __pad0;
		std::uint64_t surfaceTypeBits; // 1 << (surface type - 1), like h1
	};
	static_assert(sizeof(MaterialInfo) == 0x30);
	static_assert(offsetof(MaterialInfo, sortKey) == 10);
	static_assert(offsetof(MaterialInfo, textureAtlasRowCount) == 12);
	static_assert(offsetof(MaterialInfo, unkHash) == 0x20);
	static_assert(offsetof(MaterialInfo, surfaceTypeBits) == 0x28);

	union MaterialTextureDefInfo
	{
		GfxImage* image;
		void* water;
	};

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		unsigned char samplerState;
		unsigned char semantic;
		MaterialTextureDefInfo u;
	};
	static_assert(sizeof(MaterialTextureDef) == 0x10);

	struct MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		float literal[4];
	};
	static_assert(sizeof(MaterialConstantDef) == 0x20);

	struct GfxStateBits
	{
		unsigned int loadBits[6];
		unsigned short zone;
		unsigned char depthStencilState[12];
		unsigned char blendState;
		unsigned char rasterizerState;
	};
	static_assert(sizeof(GfxStateBits) == 0x28);
	static_assert(offsetof(GfxStateBits, blendState) == 0x26);

	struct MaterialConstantBufferDef
	{
		unsigned int vsDataSize;
		unsigned int hsDataSize;
		unsigned int dsDataSize;
		unsigned int psDataSize;
		unsigned int vsOffsetDataSize;
		unsigned int hsOffsetDataSize;
		unsigned int dsOffsetDataSize;
		unsigned int psOffsetDataSize;
		unsigned char* vsData;
		unsigned char* hsData;
		unsigned char* dsData;
		unsigned char* psData;
		unsigned short* vsOffsetData;
		unsigned short* hsOffsetData;
		unsigned short* dsOffsetData;
		unsigned short* psOffsetData;
		char __pad0[32];
	};
	static_assert(sizeof(MaterialConstantBufferDef) == 0x80);

	enum MaterialTypeBits : std::uint32_t
	{
		MTL_TYPE_BIT_MODEL = 0x1, // m
		MTL_TYPE_BIT_WORLD = 0x2, // w
		MTL_TYPE_BIT_X = 0x4, // x: fx particles and sprites
		MTL_TYPE_BIT_V = 0x8, // v
		MTL_TYPE_BIT_LIGHTMAP = 0x10, // l: lightmapped models and xl
		MTL_TYPE_BIT_EFFECT = 0x40, // e
		MTL_TYPE_BIT_VERTCOL = 0x100, // c
		MTL_TYPE_BIT_O = 0x400, // o
		MTL_TYPE_BIT_U = 0x8000, // u
		MTL_TYPE_BIT_K = 0x10000, // k
	};

	enum MaterialFlags : std::uint8_t
	{
		MTL_FLAG_VERTEX_DISPLACEMENT = 0x2, // every "vdh" techset
		MTL_FLAG_PAINTSHOP = 0x8, // "ptshp" techsets, camo-able weapon materials
		MTL_FLAG_UNK_20 = 0x20, // only with 0x2
	};

	struct Material
	{
		union
		{
			const char* name;
			MaterialInfo info;
		};
		unsigned char stateBitsEntry[114];
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char stateBitsCount;
		unsigned char stateFlags;
		unsigned char cameraRegion;
		unsigned char unused1; // 0 in stock
		unsigned int materialType; // MaterialTypeBits
		unsigned char flags; // MaterialFlags
		unsigned char constantBufferCount;
		unsigned char layerCount;
		unsigned char unused2; // 0 in stock
		std::uint64_t unused3; // 0 in stock
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		MaterialConstantDef* constantTable;
		GfxStateBits* stateBitsTable;
		Material* parentMaterial;
		unsigned char constantBufferIndex[114];
		unsigned char __pad2[6];
		MaterialConstantBufferDef* constantBufferTable;
		const char** subMaterials;
	};
	static_assert(sizeof(Material) == 0x168);
	static_assert(offsetof(Material, textureCount) == 162);
	static_assert(offsetof(Material, constantBufferCount) == 173);
	static_assert(offsetof(Material, techniqueSet) == 184);
	static_assert(offsetof(Material, materialType) == 0xA8);
	static_assert(offsetof(Material, flags) == 0xAC);
	static_assert(offsetof(Material, unused3) == 0xB0);
	static_assert(offsetof(Material, parentMaterial) == 216);
	static_assert(offsetof(Material, constantBufferTable) == 344);

	struct dmFloat4
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct dmPlane
	{
		float normal[3];
		float offset;
	};

	struct dmSubEdge
	{
		char twinOffset;
		unsigned char tail;
		unsigned char left;
		unsigned char next;
	};
	static_assert(sizeof(dmSubEdge) == 4);

	struct dmPolytopeData
	{
		dmFloat4* vertices;
		dmPlane* planes;
		dmSubEdge* subEdges;
		unsigned char* faceSubEdges;
		float centroid[3];
		int vertexCount;
		int faceCount;
		int subEdgeCount;
		float volume;
		float area;
		float inertiaMoments[3];
		float inertiaProducts[3];
		unsigned int collisionFlags; // same masks as dmMeshTriangle::collisionFlags
		unsigned int surfaceType; // one per polytope
	};
	static_assert(sizeof(dmPolytopeData) == 0x60);
	static_assert(offsetof(dmPolytopeData, vertexCount) == 44);
	static_assert(offsetof(dmPolytopeData, collisionFlags) == 88);

	struct PhysGeomBox
	{
		float data[4];
	};

	struct PhysGeomCapsule
	{
		float data[7];
	};

	struct PhysGeomInfo
	{
		union
		{
			void* data;
			dmPolytopeData* polytope;
			PhysGeomBox* box;
			PhysGeomCapsule* capsule;
		};
		int type;
		short partIndex; // into PhysCollmap::parts, -1 for none
		char __pad0[2];
	};
	static_assert(sizeof(PhysGeomInfo) == 0x10);

	struct PhysCollmapPart
	{
		scr_string_t name;
		char __pad0[4];
	};
	static_assert(sizeof(PhysCollmapPart) == 8);

	struct PhysCollmap
	{
		const char* name;
		PhysGeomInfo* geoms;
		PhysCollmapPart* parts;
		short geomCount;
		short partCount;
		char __pad0[12];
		float boundsMidPoint[3];
		float boundsHalfSize[3];
	};
	static_assert(sizeof(PhysCollmap) == 0x40);
	static_assert(offsetof(PhysCollmap, boundsMidPoint) == 40);

	struct XSurfaceCollisionNode
	{
		unsigned short mins[3];
		unsigned short maxs[3];
		unsigned short childBeginIndex;
		unsigned short childCount; // 0x8000 marks leaf children
	};
	static_assert(sizeof(XSurfaceCollisionNode) == 16);

	struct XSurfaceCollisionLeaf
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		unsigned int leafCount;
		XSurfaceCollisionNode* nodes;
		unsigned short* leafs;
	};
	static_assert(sizeof(XSurfaceCollisionLeaf) == 0x30);

	struct XRigidVertList
	{
		unsigned short boneOffset;
		unsigned short vertCount;
		unsigned short triOffset;
		unsigned short triCount;
		XSurfaceCollisionLeaf* collisionTree;
	};
	static_assert(sizeof(XRigidVertList) == 0x10);

	enum XSurfaceFlags : unsigned short
	{
		XSURFACE_FLAG_VERTCOL_NONE = 0x2,
		XSURFACE_FLAG_SKINNED = 0x4,
		XSURFACE_FLAG_LIGHTMAP_UV = 0x8,
		XSURFACE_FLAG_UNK_0x20 = 0x20,
		XSURFACE_FLAG_EXTRA_STREAM = 0x40,
	};

	struct XSurfaceSkinVertex
	{
		unsigned short bones[4];
		unsigned short weights[3];
		unsigned short count;
	};
	static_assert(sizeof(XSurfaceSkinVertex) == 16);

	struct XSurface
	{
		unsigned short flags;
		unsigned short vertCount;
		unsigned short triCount;
		unsigned char rigidVertListCount;
		char __pad0[1];
		unsigned char uvSetCount;
		char __pad1[7];
		XSurfaceShared* shared;
		unsigned short* triIndices;
		XRigidVertList* rigidVertLists;
		vec4_t* verts;
		unsigned int partBits[13];
		unsigned int baseVertIndex;
	};
	static_assert(sizeof(XSurface) == 0x68);
	static_assert(offsetof(XSurface, uvSetCount) == 8);
	static_assert(offsetof(XSurface, shared) == 16);

	struct XModelSurfs
	{
		const char* name;
		XSurface* surfs;
		unsigned short numsurfs;
		char __pad0[2];
		unsigned int partBits[13];
		float surfaceArea;
		char __pad2[4];
	};
	static_assert(sizeof(XModelSurfs) == 0x50);
	static_assert(offsetof(XModelSurfs, surfaceArea) == 0x48);

	struct SkeletonScriptCode
	{
		unsigned char byteCount;
		unsigned char intCount;
		unsigned char floatCount;
		unsigned char scriptStringCount;
		char __pad0[4];
		unsigned char* bytes;
		int* ints;
		float* floats;
		scr_string_t* scriptStrings;
	};
	static_assert(sizeof(SkeletonScriptCode) == 0x28);

	struct SkeletonScript
	{
		const char* name;
		SkeletonScriptCode code;
		int dataCount;
		char __pad0[4];
		int* data;
	};
	static_assert(sizeof(SkeletonScript) == 0x40);

	struct PhysClothTuningData
	{
		char __pad0[0x70];
	};

	struct PhysClothTuning
	{
		const char* name;
		PhysClothTuningData* data;
		char __pad0[8];
	};
	static_assert(sizeof(PhysClothTuning) == 0x18);

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};
	static_assert(sizeof(DObjAnimMat) == 32);

	struct XModelAngle
	{
		short x;
		short y;
		short z;
		short base;
	};
	static_assert(sizeof(XModelAngle) == 8);

	struct XModelTagPos
	{
		float x;
		float y;
		float z;
	};
	static_assert(sizeof(XModelTagPos) == 12);

	struct XBoneInfo
	{
		float bounds[2][3]; // midPoint, halfSize
		float radiusSquared;
	};
	static_assert(sizeof(XBoneInfo) == 28);

	struct XModelLodInfo
	{
		float dist;
		unsigned short numsurfs;
		unsigned short surfIndex;
		XModelSurfs* modelSurfs;
		char __pad1[64];
	};
	static_assert(sizeof(XModelLodInfo) == 0x50);

	struct XModelPhysBody
	{
		PhysPreset* physPreset;
		scr_string_t name;
		char __pad0[12];
	};
	static_assert(sizeof(XModelPhysBody) == 0x18);

	struct XModelPhysConstraint
	{
		PhysConstraint* physConstraint;
		char __pad0[28];
		scr_string_t bone0;
		scr_string_t bone1;
		char __pad1[4];
	};
	static_assert(sizeof(XModelPhysConstraint) == 0x30);

	struct XModelPhysRig
	{
		PhysCollmap* collmap;
		XModelPhysBody* bodies;
		XModelPhysConstraint* constraints;
		unsigned short* indices;
		short bodyCount;
		short indexCount;
		unsigned short constraintCount;
		char __pad0[10];
	};
	static_assert(sizeof(XModelPhysRig) == 0x30);

	struct XModelClothSim
	{
		char* particles;
		char* constraints0;
		char* constraints1;
		char* constraints2;
		unsigned short* indices0;
		char* constraints3;
		char* constraints4;
		unsigned short* indices1;
		unsigned short* indices2;
		char __pad0[32];
		int particleCount;
		int constraint0Count;
		char __pad1[4];
		int constraint1Count;
		int constraint2Count;
		char __pad2[8];
		int indices2Count;
		char __pad3[4];
		int constraint3Count;
		int constraint4Count;
		char __pad4[4];
	};
	static_assert(sizeof(XModelClothSim) == 0x98);
	static_assert(offsetof(XModelClothSim, particleCount) == 104);
	static_assert(offsetof(XModelClothSim, indices2Count) == 132);
	static_assert(offsetof(XModelClothSim, constraint4Count) == 144);

	struct XModelCloth
	{
		scr_string_t name;
		char __pad0[4];
		scr_string_t* boneNames;
		scr_string_t* attachNames;
		char* attachData;
		unsigned char attachCount;
		char __pad1[7];
		XModelClothSim sim;
	};
	static_assert(sizeof(XModelCloth) == 0xC0);
	static_assert(offsetof(XModelCloth, sim) == 40);

	struct XModelRigBoneEntry
	{
		char* data;
		scr_string_t name;
		unsigned char dataCount;
		char __pad0[3];
	};
	static_assert(sizeof(XModelRigBoneEntry) == 0x10);
	static_assert(offsetof(XModelRigBoneEntry, dataCount) == 12);

	struct XModelRigTail
	{
		char __pad0[12];
		scr_string_t names[4];
	};
	static_assert(sizeof(XModelRigTail) == 0x1C);

	struct XModelRig
	{
		scr_string_t name;
		unsigned char boneCount;
		unsigned char entryCount;
		char __pad0[2];
		scr_string_t* boneNames;
		char* boneData;
		XModelRigBoneEntry* entries;
		XModelRigTail* tail;
	};
	static_assert(sizeof(XModelRig) == 0x28);

	struct XModelImageRef
	{
		char __pad0[32];
		GfxImage* image;
	};
	static_assert(sizeof(XModelImageRef) == 0x28);

	struct XModelBase
	{
		const char* name;
		unsigned short numBones;
		unsigned short numRootBones;
		unsigned char lodRampType;
		char __pad0[3];
		unsigned short numClientBones;
		char __pad1[2];
		float scale;
		float unkScale[2]; // 1.0 in stock
		char __pad11[56];
		scr_string_t* boneNames;
		unsigned short* parentList;
		short* tagAngles;
		float* tagPositions;
		unsigned char* partClassification;
		char* baseMat;
		XModelLodInfo lodInfo[6];
		unsigned char numLods;
		char collLod;
		unsigned char forcedLod; // 0xFE is automatic
		unsigned char lodForMaxViewDist;
		unsigned short unk_0x26C; // a surface count, the game sizes the callback block with it
		unsigned short collSurfCount;
		char* collSurfs;
		int contents;
		char __pad3[4];
		char* boneInfo;
		float radius;
		float boundsMidPoint[3];
		float boundsHalfSize[3];
		float cullSizeScale; // 15 in stock
		float lodDistScale;
		float lodDistSq[6];
		float boundsRadius; // length of boundsHalfSize
		unsigned char useLodInfoDist; // reads lodInfo[].dist instead
		char __pad10[3];
		unsigned int memUsage;
		char __pad12[8];
		unsigned char unk_0x2D8; // 4 on vehicles
		char __pad13[1];
		unsigned short extraBoneNameCount;
		unsigned short extraDataCount;
		char __pad5[2];
		scr_string_t* extraBoneNames;
		char* extraData;
		PhysPreset* physPreset;
		PhysCollmap* physCollmap;
		unsigned char unk_0x300; // 1 with physics rigs
		signed char unk_0x301; // -1 without physics rigs
		unsigned char physRigCount;
		char __pad7[5];
		scr_string_t* physRigNames;
		XModelPhysRig** physRigs;
		PhysClothTuning* physClothTuning;
		XModelCloth* cloth;
		XModelRig* rig;
		unsigned short imageRefCount;
		char __pad8[6];
		XModelImageRef* imageRefs;
		SkeletonScript* skeletonScripts[4];
		float swayParams[4]; // wires and trees only
		char __pad9[2];
		short unk_0x372; // -1 in stock
		float unk_0x374; // 96 in stock
	};
	static_assert(sizeof(XModelBase) == 0x378);
	static_assert(offsetof(XModelBase, scale) == 0x14);
	static_assert(offsetof(XModelBase, memUsage) == 0x2CC);
	static_assert(offsetof(XModelBase, unk_0x300) == 0x300);
	static_assert(offsetof(XModelBase, swayParams) == 0x360);
	static_assert(offsetof(XModelBase, unk_0x374) == 0x374);
	static_assert(offsetof(XModelBase, numClientBones) == 16);
	static_assert(offsetof(XModelBase, boneNames) == 88);
	static_assert(offsetof(XModelBase, lodInfo) == 136);
	static_assert(offsetof(XModelBase, numLods) == 616);
	static_assert(offsetof(XModelBase, collSurfCount) == 622);
	static_assert(offsetof(XModelBase, radius) == 648);
	static_assert(offsetof(XModelBase, lodDistSq) == 684);
	static_assert(offsetof(XModelBase, boneInfo) == 640);
	static_assert(offsetof(XModelBase, lodRampType) == 12);
	static_assert(offsetof(XModelBase, forcedLod) == 618);
	static_assert(offsetof(XModelBase, cullSizeScale) == 676);
	static_assert(offsetof(XModelBase, boundsRadius) == 708);
	static_assert(offsetof(XModelBase, extraBoneNameCount) == 730);
	static_assert(offsetof(XModelBase, extraBoneNames) == 736);
	static_assert(offsetof(XModelBase, physPreset) == 752);
	static_assert(offsetof(XModelBase, physRigCount) == 770);
	static_assert(offsetof(XModelBase, physRigNames) == 776);
	static_assert(offsetof(XModelBase, imageRefCount) == 816);
	static_assert(offsetof(XModelBase, skeletonScripts) == 832);

	struct XModel;

	struct XModelVariant
	{
		XModel* model;
		char __pad0[28];
		scr_string_t name;
		char __pad1[8];
	};
	static_assert(sizeof(XModelVariant) == 0x30);

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};
	assert_sizeof(Bounds, 24);

	struct XModelCollSurf
	{
		Bounds bounds;
		int boneIdx;
		char __pad0[4];
		unsigned long long contents;
		unsigned int surfFlags;
		char __pad1[4];
	};
	static_assert(sizeof(XModelCollSurf) == 48);

	enum XModelFlags : unsigned int
	{
		XMODEL_FLAG_VARIANTS = 0x200,
		XMODEL_FLAG_UNK_0x800 = 0x800,
		XMODEL_FLAG_SWAY = 0x1000,
		XMODEL_FLAG_NO_LAST_LOD = 0x8000,
	};

	struct XModel
	{
		const char* name;
		union
		{
			XModelBase* base;
			XModelVariant* variants;
		};
		Material** materialHandles;
		unsigned short* materialIndices;
		unsigned int flags;
		unsigned short runtimeSize;
		char __pad0[4];
		unsigned char variantCount;
		unsigned char numMaterials;
		char __pad1[4];
	};
	static_assert(sizeof(XModel) == 0x30);
	static_assert(offsetof(XModel, variantCount) == 42);

	struct LightDef;
	struct FxParticleSimAnimation;
	struct FxEffectDef;

	union FxEffectDefRef
	{
		FxEffectDef* handle;
		const char* name;
	};

	union FxElemVisuals
	{
		const void* anonymous;
		Material* material;
		XModel* model;
		FxEffectDefRef effectDef;
		const char* soundName;
		const char* vectorFieldName;
		LightDef* lightDef;
		FxParticleSimAnimation* particleSimAnimation;
	};

	struct FxElemMarkVisuals
	{
		Material* materials[3];
	};

	union FxElemDefVisuals
	{
		FxElemMarkVisuals* markArray;
		FxElemVisuals* array;
		FxElemVisuals instance;
	};

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	struct FxTrailVertex
	{
		float pos[2];
		float normal[2];
		float texCoord[2];
		float radialNormal[2];
	};
	static_assert(sizeof(FxTrailVertex) == 32);

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		float invSplitDist;
		float invSplitArcDist;
		float invSplitTime;
		float headFadingFactor;
		float tailFadingFactor;
		int vertCount;
		FxTrailVertex* verts;
		int indCount;
		unsigned short* inds;
	};
	static_assert(sizeof(FxTrailDef) == 0x38);

	struct FxSparkFountainDef
	{
		float gravity;
		float bounceFrac;
		float bounceRand;
		float sparkSpacing;
		float sparkLength;
		int sparkCount;
		float loopTime;
		float velMin;
		float velMax;
		float velConeFrac;
		float restSpeed;
		float boostTime;
		float boostFactor;
	};
	static_assert(sizeof(FxSparkFountainDef) == 0x34);

	struct FxSpotLightDef
	{
		char __pad0[48];
		const char* name;
	};
	static_assert(sizeof(FxSpotLightDef) == 0x38);

	struct FxOmniLightDef
	{
		float bulbRadius;
		float bulbLength;
		float fadeOffsetRt[2];
		char __pad0[8];
		const char* name;
	};
	static_assert(sizeof(FxOmniLightDef) == 0x20);

	struct FxFlareDef
	{
		float position;
		int angularRotCount;
		int flags;
		FxFloatRange depthScaleRange;
		FxFloatRange depthScaleValue;
		FxFloatRange radialRot;
		FxFloatRange radialScaleX;
		FxFloatRange radialScaleY;
		float dir[3];
		int intensityXIntervalCount;
		int intensityYIntervalCount;
		int srcCosIntensityIntervalCount;
		int srcCosScaleIntervalCount;
		float* intensityX;
		float* intensityY;
		float* srcCosIntensity;
		float* srcCosScale;
	};
	static_assert(sizeof(FxFlareDef) == 0x70);

	union FxElemExtendedDefPtr
	{
		void* unknownDef;
		FxTrailDef* trailDef;
		FxSparkFountainDef* sparkFountainDef;
		FxSpotLightDef* spotLightDef;
		FxOmniLightDef* omniLightDef;
		FxFlareDef* flareDef;
		char* unknownBytes;
	};

	enum FxEffectDefFlags : std::uint32_t
	{
		FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_AT_SPAWN = 0x1,
		FX_EFFECT_NEEDS_CAST_SHADOW = 0x2,
		FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_PER_FRAME = 0x4,
		FX_EFFECT_NEEDS_DYNAMIC_LIGHTING_PER_FRAME = 0x8,
		FX_EFFECT_NEEDS_EMISSIVE_DRAW = 0x10,
		FX_EFFECT_USE_VECTORFIELDS = 0x20,
		FX_EFFECT_LINK_TO_SUN = 0x40,
		FX_EFFECT_HAS_CAMERA_RELATIVE_ELEM = 0x80,
		FX_EFFECT_NEEDS_POST_RESOLVE_DRAW = 0x100,
		FX_EFFECT_UNK_9 = 0x200,
		FX_EFFECT_HAS_ROTATED_SPRITE_ELEM = 0x400,
		FX_EFFECT_UNK_11 = 0x800, // any elem with FX_ELEM2_UNK_8
		FX_EFFECT_UNK_12 = 0x1000, // any elem with FX_ELEM2_UNK_19
		FX_EFFECT_UNK_13 = 0x2000, // any elem with FX_ELEM2_UNK_20
		FX_EFFECT_UNK_14 = 0x4000, // FX_PlayEffect hands its FX_ELEM_TYPE_UNK_20 elems to cgame
	};

	enum FxElemType : std::uint8_t
	{
		FX_ELEM_TYPE_SPRITE_BILLBOARD = 0,
		FX_ELEM_TYPE_SPRITE_ORIENTED = 1,
		FX_ELEM_TYPE_SPRITE_ROTATED = 2,
		FX_ELEM_TYPE_TAIL = 3,
		FX_ELEM_TYPE_LINE = 4,
		FX_ELEM_TYPE_TRAIL = 5,
		FX_ELEM_TYPE_FLARE = 6,
		FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 7,
		FX_ELEM_TYPE_CLOUD = 8,
		FX_ELEM_TYPE_SPARK_CLOUD = 9,
		FX_ELEM_TYPE_SPARK_FOUNTAIN = 10,
		FX_ELEM_TYPE_MODEL = 11,
		FX_ELEM_TYPE_OMNI_LIGHT = 12,
		FX_ELEM_TYPE_SPOT_LIGHT = 13,
		FX_ELEM_TYPE_SOUND = 14,
		FX_ELEM_TYPE_STRING_15 = 15,
		FX_ELEM_TYPE_DECAL = 16,
		FX_ELEM_TYPE_RUNNER = 17,
		FX_ELEM_TYPE_VECTORFIELD = 18,
		FX_ELEM_TYPE_STRING_19 = 19, // visual is always null
		FX_ELEM_TYPE_UNK_20 = 20, // no visual, cgame reads its spawn origin and offset radius
	};

	enum FxElemDefFlags : std::uint32_t
	{
		FX_ELEM_SPAWN_RELATIVE_TO_EFFECT = 0x2,
		FX_ELEM_SPAWN_FRUSTUM_CULL = 0x4,
		FX_ELEM_RUNNER_USES_RAND_ROT = 0x8,
		FX_ELEM_SPAWN_OFFSET_NONE = 0x0,
		FX_ELEM_SPAWN_OFFSET_SPHERE = 0x10,
		FX_ELEM_SPAWN_OFFSET_CYLINDER = 0x20,
		FX_ELEM_SPAWN_OFFSET_MASK = 0x30,
		FX_ELEM_RUN_RELATIVE_TO_WORLD = 0x0,
		FX_ELEM_RUN_RELATIVE_TO_SPAWN = 0x40,
		FX_ELEM_RUN_RELATIVE_TO_EFFECT = 0x80,
		FX_ELEM_RUN_RELATIVE_TO_OFFSET = 0xC0,
		FX_ELEM_RUN_RELATIVE_TO_CAMERA = 0x100,
		FX_ELEM_RUN_MASK = 0x1C0,
		FX_ELEM_DIE_ON_TOUCH = 0x200,
		FX_ELEM_DRAW_PAST_FOG = 0x400,
		FX_ELEM_DRAW_WITH_VIEWMODEL = 0x800,
		FX_ELEM_BLOCK_SIGHT = 0x1000,
		FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY = 0x2000,
		FX_ELEM_TRAIL_ORIENT_BY_VELOCITY = 0x4000,
		FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
		FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
		FX_ELEM_USE_CAST_SHADOW = 0x20000,
		FX_ELEM_NODRAW_IN_THERMAL_VIEW = 0x40000,
		FX_ELEM_THERMAL_MASK = 0x42000,
		FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME = 0x80000,
		FX_ELEM_RECEIVE_DYNAMIC_LIGHT = 0x100000,
		FX_ELEM_VOLUMETRIC_TRAIL = 0x200000,
		FX_ELEM_USE_COLLISION = 0x400000,
		FX_ELEM_USE_VECTORFIELDS = 0x800000,
		FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL = 0x1000000,
		FX_ELEM_HAS_VELOCITY_GRAPH_WORLD = 0x2000000,
		FX_ELEM_HAS_GRAVITY = 0x4000000,
		FX_ELEM_USE_MODEL_PHYSICS = 0x8000000,
		FX_ELEM_NONUNIFORM_SCALE = 0x10000000,
		FX_ELEM_CLOUD_SHAPE_CUBE = 0x0,
		FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE = 0x20000000,
		FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM = 0x40000000,
		FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL = 0x60000000,
		FX_ELEM_CLOUD_SHAPE_MASK = 0x60000000,
		FX_ELEM_EMIT_BOLT = 0x80000000,
		FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
	};

	enum FxElemDefExtraFlags : std::uint32_t
	{
		FX_ELEM2_BILLBOARD_FACING_CAMERA_PERPENDICULAR = 0x1,
		FX_ELEM2_BILLBOARD_FACING_PLAYER = 0x2,
		FX_ELEM2_BILLBOARD_FACING_MASK = 0x3,
		FX_ELEM2_EMIT_TRAILS = 0x4,
		FX_ELEM2_USE_EMISSIVE_DRAW = 0x8,
		FX_ELEM2_USE_EFFECT_MODEL_COLOR = 0x20,
		FX_ELEM2_REQUIRES_POST_RESOLVE_DRAW = 0x40, // H2 name
		FX_ELEM2_DECAL_EXPONENTIAL_FADE_OUT = 0x80,
		FX_ELEM2_UNK_8 = 0x100, // packs an entity index of the bolted effect into the elem draw bits
		FX_ELEM2_INHERIT_PARENT_VELOCITY = 0x200,
		FX_ELEM2_H1_MASK = 0xFF, // bits with the same meaning in H1
		FX_ELEM2_UNK_14 = 0x4000,
		FX_ELEM2_UNK_15 = 0x8000, // sprite batches split on it
		FX_ELEM2_UNK_16 = 0x10000,
		FX_ELEM2_UNK_17 = 0x20000,
		FX_ELEM2_UNK_18 = 0x40000,
		FX_ELEM2_UNK_19 = 0x80000,
		FX_ELEM2_UNK_20 = 0x100000, // runner elems only
		FX_ELEM2_UNK_21 = 0x200000, // scales an elem value by FxElemDef::unknown344
		FX_ELEM2_UNK_22 = 0x400000, // decal only
		FX_ELEM2_UNK_23 = 0x800000, // decal only
		FX_ELEM2_UNK_24 = 0x1000000, // decal only
		FX_ELEM2_UNK_25 = 0x2000000, // decal only
		FX_ELEM2_UNK_26 = 0x4000000, // decal only
		FX_ELEM2_OCCLUSION_QUERY_NO_SCALE = 0x8000000, // skips the occlusion query size scale
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
	};

	struct FxSpawnDefOneShot
	{
		FxIntRange count;
	};

	union FxSpawnDef
	{
		FxSpawnDefLooping looping;
		FxSpawnDefOneShot oneShot;
	};

	enum FxElemAtlasBehavior : std::uint8_t
	{
		FX_ATLAS_PLAY_OVER_LIFE = 0x4,
	};

	struct FxElemAtlas
	{
		unsigned char behavior;
		unsigned char index;
		unsigned char fps;
		unsigned char loopCount;
		unsigned char colIndexBits;
		unsigned char rowIndexBits;
		short entryCount;
	};

	struct FxElemVec3Range
	{
		float base[3];
		float amplitude[3];
	};

	struct FxElemVelStateInFrame
	{
		FxElemVec3Range velocity;
		FxElemVec3Range totalDelta;
	};

	struct FxElemVelStateSample
	{
		float vectorFieldScale; // 1 in stock
		FxElemVelStateInFrame local;
		FxElemVelStateInFrame world;
		FxElemVelStateInFrame unk;
	};
	static_assert(sizeof(FxElemVelStateSample) == 148);

	struct FxElemVisualState
	{
		float color[4];
		float emissiveScale[3];
		float rotationDelta;
		float rotationTotal;
		float size[2];
		float scale;
		float pivot[2];
		float atlasFrame;
	};

	struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
	};
	static_assert(sizeof(FxElemVisStateSample) == 120);

	struct FxElemDef
	{
		unsigned int flags;
		unsigned int flags2;
		FxSpawnDef spawn;
		FxFloatRange spawnRange;
		FxFloatRange fadeInRange;
		FxFloatRange fadeOutRange;
		float spawnFrustumCullRadius;
		FxIntRange spawnDelayMsec;
		FxIntRange lifeSpanMsec;
		FxFloatRange spawnOrigin[3];
		FxFloatRange spawnOffsetRadius;
		FxFloatRange spawnOffsetHeight;
		FxFloatRange spawnAngles[3];
		FxFloatRange angularVelocity[3];
		FxFloatRange initialRotation;
		FxFloatRange gravity;
		FxFloatRange reflectionFactor;
		FxFloatRange unknownRange; // stock {0.5, 0}
		FxElemAtlas atlas;
		FxElemType elemType;
		unsigned char elemLitType;
		unsigned char visualCount;
		unsigned char velIntervalCount;
		unsigned char visStateIntervalCount;
		FxElemVelStateSample* velSamples;
		FxElemVisStateSample* visSamples;
		FxElemDefVisuals visuals;
		Bounds collBounds;
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		FxElemExtendedDefPtr extended;
		unsigned char sortOrder;
		unsigned char lightingFrac;
		unsigned char useItemClip;
		unsigned char fadeInfo;
		unsigned char fadeOutInfo;
		int randomSeed;
		float emissiveScaleScale;
		float hdrLightingFrac;
		float shadowDensityScale;
		float scatterRatio;
		float volumetricTrailFadeStart;
		float unknown328; // stock 0.5
		char __pad0[12];
		float unknown344; // stock 1
		char __pad1[4];
		const char* name;
	};
	static_assert(sizeof(FxElemDef) == 0x168);
	static_assert(offsetof(FxElemDef, atlas) == 180);
	static_assert(offsetof(FxElemDef, elemType) == 188);
	static_assert(offsetof(FxElemDef, velSamples) == 200);
	static_assert(offsetof(FxElemDef, visuals) == 216);
	static_assert(offsetof(FxElemDef, effectOnImpact) == 248);
	static_assert(offsetof(FxElemDef, extended) == 288);
	static_assert(offsetof(FxElemDef, sortOrder) == 296);
	static_assert(offsetof(FxElemDef, randomSeed) == 304);
	static_assert(offsetof(FxElemDef, unknown328) == 328);
	static_assert(offsetof(FxElemDef, name) == 352);

	struct FxEffectDef
	{
		const char* name;
		unsigned int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		float elemMaxRadius;
		char __pad0[4];
		float occlusionQueryDepthBias;
		int occlusionQueryFadeIn;
		int occlusionQueryFadeOut;
		FxFloatRange occlusionQueryScaleRange;
		char __pad1[4];
		FxElemDef* elemDefs;
	};
	static_assert(sizeof(FxEffectDef) == 0x48);
	static_assert(offsetof(FxEffectDef, occlusionQueryDepthBias) == 40);

	struct LightDef
	{
		const char* name;
		const char* attenuationName;
	};
	static_assert(sizeof(LightDef) == 0x10);

	struct GfxColorFloat
	{
		float array[4];
	};

	// same as h1
	struct FxParticleSimAnimationHeader
	{
		float playbackRate;
		float duration;
		unsigned int frameCount;
		float minX;
		float minY;
		float minZ;
		float boundsXDelta;
		float boundsYDelta;
		float boundsZDelta;
		float maxWidth;
		float maxHeight;
		unsigned int colorTableSize;
		unsigned int particleDataCount;
		bool evalVisStatePerParticle;
		bool sortParticlesAtRuntime; // the draw clears this on the asset when a frame has more than 8832 particles
	};
	static_assert(sizeof(FxParticleSimAnimationHeader) == 0x38);

	// u16 / 65535 of the header bounds (pos), max sizes (size), duration (lifetime) and 2 pi (orientation), orientationDelta is s16 / 32767 of pi
	struct FxParticleSimAnimationParticleData
	{
		unsigned short xNormalizedPos;
		unsigned short yNormalizedPos;
		unsigned short zNormalizedPos;
		unsigned short xNormalizedWidth;
		unsigned short yNormalizedHeight;
		unsigned short orientation;
		unsigned short lifetime;
		unsigned short particleID;
		unsigned short xNormalizedPosNextFrame;
		unsigned short yNormalizedPosNextFrame;
		unsigned short zNormalizedPosNextFrame;
		unsigned short xNormalizedWidthNextFrame;
		unsigned short yNormalizedHeightNextFrame;
		short orientationDelta;
		unsigned short colorTableIndex;
		unsigned short nextColorTableIndex;
	};
	static_assert(sizeof(FxParticleSimAnimationParticleData) == 0x20);

	struct FxParticleSimAnimationFrame
	{
		unsigned int particleDataOffset;
		unsigned int numActiveParticles;
	};
	static_assert(sizeof(FxParticleSimAnimationFrame) == 0x8);

	struct FxParticleSimAnimation
	{
		const char* name;
		Material* material;
		FxParticleSimAnimationHeader header;
		FxParticleSimAnimationParticleData* particleData;
		FxParticleSimAnimationFrame* frames;
		GfxColorFloat* colorTable;
	};
	static_assert(sizeof(FxParticleSimAnimation) == 0x60);
	static_assert(offsetof(FxParticleSimAnimation, header) == 0x10);
	static_assert(offsetof(FxParticleSimAnimation, particleData) == 0x48);
	static_assert(offsetof(FxParticleSimAnimation, colorTable) == 0x58);

	union XAnimDynamicFrames
	{
		unsigned char(*_1)[3];
		unsigned short(*_2)[3];
	};

	union XAnimDynamicIndices
	{
		unsigned char _1[1];
		unsigned short _2[1];
	};

	struct XAnimPartTransFrames
	{
		float mins[3];
		float size[3];
		XAnimDynamicFrames frames;
		XAnimDynamicIndices indices;
	};

	union XAnimPartTransData
	{
		XAnimPartTransFrames frames;
		float frame0[3];
	};

	struct XAnimPartTrans
	{
		unsigned short size;
		unsigned char smallTrans;
		XAnimPartTransData u;
	};
	static_assert(offsetof(XAnimPartTrans, u) == 8);

	struct XAnimDeltaPartQuatDataFrames2
	{
		short(*frames)[2];
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData2
	{
		XAnimDeltaPartQuatDataFrames2 frames;
		short frame0[2];
	};

	struct XAnimDeltaPartQuat2
	{
		unsigned short size;
		XAnimDeltaPartQuatData2 u;
	};
	static_assert(offsetof(XAnimDeltaPartQuat2, u) == 8);

	struct XAnimDeltaPartQuatDataFrames
	{
		short(*frames)[4];
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		short frame0[4];
	};

	struct XAnimDeltaPartQuat
	{
		unsigned short size;
		XAnimDeltaPartQuatData u;
	};
	static_assert(offsetof(XAnimDeltaPartQuat, u) == 8);

	struct XAnimDeltaPart
	{
		XAnimPartTrans* trans;
		XAnimDeltaPartQuat2* quat2;
		XAnimDeltaPartQuat* quat;
	};

	union XAnimIndices
	{
		unsigned char* _1;
		unsigned short* _2;
		void* data;
	};

	enum XAnimPartType : std::size_t
	{
		PART_TYPE_NO_QUAT = 0,
		PART_TYPE_HALF_QUAT = 1,
		PART_TYPE_FULL_QUAT = 2,
		PART_TYPE_HALF_QUAT_NO_SIZE = 3,
		PART_TYPE_FULL_QUAT_NO_SIZE = 4,
		PART_TYPE_SMALL_TRANS = 5,
		PART_TYPE_TRANS = 6,
		PART_TYPE_TRANS_NO_SIZE = 7,
		PART_TYPE_NO_TRANS = 8,
		PART_TYPE_ALL = 9,
	};

	enum XAnimPartsFlags : unsigned short
	{
		ANIM_LOOP = 0x1,
		ANIM_DELTA = 0x2,
	};

	enum XAnimAssetType : unsigned char
	{
		XANIM_ASSET_TYPE_ABSOLUTE = 1,
		XANIM_ASSET_TYPE_DELTA = 2,
		XANIM_ASSET_TYPE_ADDITIVE = 6,
	};

	struct XAnimNotifyInfo
	{
		scr_string_t name;
		float time;
	};

	struct XAnimParts
	{
		const char* name;
		unsigned short dataByteCount;
		unsigned short dataShortCount;
		unsigned short dataIntCount;
		unsigned short numframes;
		unsigned short flags;
		unsigned short boneCount[10];
		unsigned char notifyCount;
		unsigned char assetType;
		unsigned char ikType;
		char __pad1[3];
		unsigned int randomDataByteCount;
		unsigned int randomDataShortCount;
		unsigned int randomDataIntCount;
		unsigned int indexCount;
		float framerate;
		float frequency;
		char __pad2[4];
		scr_string_t* names;
		char* dataByte;
		short* dataShort;
		int* dataInt;
		short* randomDataShort;
		unsigned char* randomDataByte;
		int* randomDataInt;
		XAnimIndices indices;
		XAnimNotifyInfo* notify;
		XAnimDeltaPart* deltaPart;
		const char* secondaryName;
		char __pad3[2];
		unsigned short blendShapeCount;
		char __pad4[4];
		scr_string_t* blendShapeWeightNames;
		float* blendShapeWeights;
		char __pad5[8];
	};
	static_assert(sizeof(XAnimParts) == 0xC0);
	static_assert(offsetof(XAnimParts, boneCount) == 18);
	static_assert(offsetof(XAnimParts, notifyCount) == 38);
	static_assert(offsetof(XAnimParts, framerate) == 60);
	static_assert(offsetof(XAnimParts, randomDataByteCount) == 44);
	static_assert(offsetof(XAnimParts, names) == 72);
	static_assert(offsetof(XAnimParts, secondaryName) == 152);
	static_assert(offsetof(XAnimParts, blendShapeCount) == 162);

	using DDLHashTable = ddl_structs::DDLHashTable;
	struct DDLMember
	{
		const char* name;
		int index;
		unsigned char* parent;
		int bitSize;
		int limitSize;
		int offset;
		int type;
		int externalIndex;
		unsigned int rangeLimit;
		unsigned int serverDelta;
		unsigned int clientDelta;
		int arraySize;
		int enumIndex;
	};

	struct DDLStruct
	{
		const char* name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableUpper;
		DDLHashTable hashTableLower;
	};
	using DDLEnum = ddl_structs::DDLEnum;
	static_assert(sizeof(DDLMember) == 0x40);
	static_assert(sizeof(DDLStruct) == 0x38);
	static_assert(sizeof(DDLEnum) == 0x28);

	struct DDLDef
	{
		char* name;
		unsigned short version;
		unsigned int checksum;
		unsigned char flags;
		int bitSize;
		int byteSize;
		DDLStruct* structList;
		int structCount;
		DDLEnum* enumList;
		int enumCount;
		DDLDef* next;
		int headerBitSize;
		int headerByteSize;
		int reserveSize;
		int userFlagsSize;
	};
	static_assert(sizeof(DDLDef) == 0x58);

	struct DDLRoot
	{
		const char* name;
		DDLDef* ddlDef;
	};

	struct LeaderboardColumnDef
	{
		const char* name;
		char __pad0[16];
		const char* statName;
		char __pad1[24];
	};
	static_assert(sizeof(LeaderboardColumnDef) == 0x38);

	struct LeaderboardDef
	{
		const char* name;
		char __pad0[12];
		int columnCount;
		char __pad1[8];
		LeaderboardColumnDef* columns;
		char __pad2[16];
	};
	static_assert(sizeof(LeaderboardDef) == 0x38);

	struct TracerDef
	{
		const char* name;
		Material* material;
		FxEffectDef* effect;
		char __pad0[104];
	};
	static_assert(sizeof(TracerDef) == 0x80);

	// h1 GfxWorldVertex with one stream per field
	struct GfxMapTrZoneData
	{
		vec3_t* positions;
		float* texelDensities; // uv units per world unit, h1 stores it signed in GfxWorldVertex::binormalSign
		unsigned int* colors;
		vec2_t* texCoords;
		vec2_t* lmapCoords;
		unsigned int* normals; // 10:10:10:2 unorm
		unsigned int* tangents; // 10:10:10:2 unorm, w 3 = binormal is -cross(normal, tangent), w 0 = +cross
	};
	static_assert(sizeof(GfxMapTrZoneData) == 0x38);

	struct GfxMapTrZoneBuffers
	{
		void* buffers[7];
	};

	struct GfxMapTrZoneVertexBuffer
	{
		GfxMapTrZoneData data;
		GfxMapTrZoneBuffers buffers;
	};
	static_assert(sizeof(GfxMapTrZoneVertexBuffer) == 0x70);

	struct GfxMapTrZoneVertexLayerData
	{
		unsigned char* data;
		void* buffer;
	};

	struct GfxMapTrZone
	{
		const char* name;
		char __pad0[4];
		unsigned int vertexCount;
		GfxMapTrZoneVertexBuffer vertices;
		unsigned int vertexLayerDataSize; // GfxSurface::vertexLayerData is a byte offset into vld
		char __pad1[4];
		GfxMapTrZoneVertexLayerData vld;
	};
	static_assert(sizeof(GfxMapTrZone) == 0x98);
	static_assert(offsetof(GfxMapTrZone, vertexLayerDataSize) == 128);
	static_assert(offsetof(GfxMapTrZone, vld) == 136);

	struct MayhemModel
	{
		XModel* model;
	};

	struct MayhemTransBounds
	{
		vec3_t mins;
		vec3_t size;
	};
	static_assert(sizeof(MayhemTransBounds) == 0x18);

	struct GfxWrappedBaseBuffer
	{
		ID3D11Resource* buffer; // created at load
	};

	struct MayhemAnimFramesSplineCompressed
	{
		char* diskQuat;
		char* diskPos;
		unsigned short* diskQuatFrames;
		unsigned short* diskPosFrames;
		unsigned short* numDiskQuatFrames;
		unsigned short* numDiskPosFrames;
		MayhemTransBounds* transBounds;
		GfxWrappedBaseBuffer quat;
		GfxWrappedBaseBuffer pos;
		unsigned int quatStride;
		unsigned int posStride;
		unsigned int totalCompressedQuatFrames;
		unsigned int totalCompressedPosFrames;
	};
	static_assert(sizeof(MayhemAnimFramesSplineCompressed) == 0x58);

	struct MayhemAnimFramesUncompressed
	{
		char* diskQuat;
		char* diskPos;
		MayhemTransBounds* transBounds;
		GfxWrappedBaseBuffer quat;
		GfxWrappedBaseBuffer pos;
		unsigned int quatStride;
		unsigned int posStride;
	};
	static_assert(sizeof(MayhemAnimFramesUncompressed) == 0x30);

	union MayhemFramesUnion
	{
		void* data;
		MayhemAnimFramesSplineCompressed* splineCompressedFrames;
		MayhemAnimFramesUncompressed* uncompressedFrames;
	};

	struct MayhemDataKeysSplineCompressed
	{
		char* keys;
		unsigned short* numKeys;
		unsigned short* keyFrames;
		unsigned int totalCompressedKeyFrames;
		unsigned int numStreams;
		unsigned int keyStride;
	};
	static_assert(sizeof(MayhemDataKeysSplineCompressed) == 0x28);

	struct MayhemDataKeysUncompressed
	{
		char* keys;
		unsigned int numStreams;
		unsigned int keyStride;
	};
	static_assert(sizeof(MayhemDataKeysUncompressed) == 0x10);

	union MayhemDataChannelsUnion
	{
		void* data;
		MayhemDataKeysSplineCompressed* splineCompressedKeys;
		MayhemDataKeysUncompressed* uncompressedKeys;
	};

	struct MayhemAnim
	{
		MayhemFramesUnion frames;
		unsigned int numBones;
		unsigned int numFrames;
		float framerate;
		float frequency;
		unsigned int notifyCount;
		unsigned int dataChannelCount;
		bool isLooping;
		bool isSplineCompressed;
		bool quantizeQuats;
		bool quantizeTrans;
		bool quantizeChannels;
		XAnimNotifyInfo* notify;
		MayhemDataChannelsUnion dataChannels;
	};
	static_assert(sizeof(MayhemAnim) == 0x38);
	static_assert(offsetof(MayhemAnim, isSplineCompressed) == 0x21);
	static_assert(offsetof(MayhemAnim, notify) == 0x28);

	struct MayhemObject
	{
		unsigned int modelIndex;
		unsigned int animIndex;
	};

	struct Mayhem
	{
		const char* name;
		XModel* preModel;
		XModel* postModel;
		unsigned int numModels;
		MayhemModel* models;
		unsigned int numAnims;
		MayhemAnim* anims;
		unsigned int numObjects;
		MayhemObject* objects;
		float length;
	};
	static_assert(sizeof(Mayhem) == 0x50);

	struct Spline;

	enum ScriptableEventType : std::int32_t
	{
		SCRIPTABLE_EVENT_STATE_MODEL = 0,
		SCRIPTABLE_EVENT_MODEL = 1,
		SCRIPTABLE_EVENT_FX = 2,
		SCRIPTABLE_EVENT_STOP_FX = 3,
		SCRIPTABLE_EVENT_SOUND = 4,
		SCRIPTABLE_EVENT_ANIMATION = 5,
		SCRIPTABLE_EVENT_EXPLODE = 6,
		SCRIPTABLE_EVENT_HEALTHDRAIN = 7,
		SCRIPTABLE_EVENT_PHYSICSLAUNCH = 8,
		SCRIPTABLE_EVENT_LIGHTSETTINGS = 9,
		SCRIPTABLE_EVENT_LATE_SPLINE = 10,
		SCRIPTABLE_EVENT_SHAKE = 11,
		SCRIPTABLE_EVENT_LATE_STRING_12 = 12,
		SCRIPTABLE_EVENT_LATE_STRING_13 = 13,
		SCRIPTABLE_EVENT_MAYHEM = 14,
		SCRIPTABLE_EVENT_STRING_15 = 15,
		SCRIPTABLE_EVENT_SPLINE = 16,
		SCRIPTABLE_EVENT_STATECHANGE = 17,
	};

	enum ScriptableNotetrackType : std::int32_t
	{
		SCRIPTABLE_NOTETRACK_FX = 0,
		SCRIPTABLE_NOTETRACK_SOUND = 1,
	};

	struct ScriptableEventModelDef
	{
		XModel* model;
		const char* name;
		unsigned char modelIndex;
		unsigned char brushIndex;
		bool noModel;
		bool noBrush;
		bool setInstanceFlag;
	};

	struct ScriptableEventPartModelDef
	{
		XModel* model;
		bool show;
		unsigned char modelTagIndex;
	};

	struct ScriptableEventFxDef
	{
		FxEffectDef* effect;
		scr_string_t tag;
		unsigned short loopTime;
		unsigned char loopTimeStreamIndex;
		bool tagUseAngles;
	};

	struct ScriptableEventSoundDef
	{
		snd_alias_list_t* alias;
		bool looping;
	};

	struct ScriptableEventAnimationDef
	{
		const char* animName;
		unsigned short timeOffsetMin;
		unsigned short timeOffsetMax;
		unsigned short playbackRateMin;
		unsigned short playbackRateMax;
		unsigned short blendTime;
		bool override;
		unsigned char animEntryIndex;
		unsigned char animPlaybackStreamIndex;
	};
	static_assert(offsetof(ScriptableEventAnimationDef, animEntryIndex) == 19);

	struct ScriptableEventExplodeDef
	{
		unsigned short forceMin;
		unsigned short forceMax;
		unsigned short forceRadius;
		unsigned short radius;
		unsigned short damageMin;
		unsigned short damageMax;
		bool aiAvoid;
	};

	struct ScriptableEventHealthDef
	{
		unsigned short amount;
		unsigned short interval;
		unsigned short badPlaceHealth;
		unsigned short badPlaceHeight;
		unsigned short badPlaceRadius;
		unsigned char streamIndex;
	};

	struct ScriptableEventPhysicsDef
	{
		XModel* model;
		unsigned char launchDirX;
		unsigned char launchDirY;
		unsigned char launchDirZ;
		unsigned short explForceScale;
		unsigned short bulletForceScale;
	};

	struct ScriptableEventShakeDef
	{
		const char* rumbleName;
		unsigned short duration;
		unsigned short durationFadeUp;
		unsigned short durationFadeDown;
		unsigned short radius;
		unsigned short exponent;
		unsigned short scaleEarthquake;
		unsigned char scalePitch;
		unsigned char scaleYaw;
		unsigned char scaleRoll;
		unsigned char frequencyPitch;
		unsigned char frequencyYaw;
		unsigned char frequencyRoll;
		unsigned char flags;
	};

	struct ScriptableEventStateChangeDef
	{
		unsigned char targetIndex;
		unsigned char delayStreamIndex;
		unsigned short delayMin;
		unsigned short delayMax;
	};

	struct ScriptableEventStringDef
	{
		const char* name;
	};

	struct ScriptableEventSplineDef
	{
		char __pad0[24];
		Spline* spline;
	};

	// h1 light settings with a float color, the light index is in the instance flags buffer
	struct ScriptableEventLightSettingsDef
	{
		float color[4];
		unsigned char lightIndexConstIndex;
		unsigned char transStateStreamIndex;
		unsigned char flags;
		unsigned char splineStreamIndex;
		float intensityScaleMin;
		float intensityScaleMax;
		unsigned short radiusScaleMin;
		unsigned short radiusScaleMax;
		unsigned short transitionTimeMin;
		unsigned short transitionTimeMax;
		char __pad0[4];
		const char* noteworthy;
		Spline* spline;
	};
	static_assert(offsetof(ScriptableEventLightSettingsDef, intensityScaleMin) == 0x14);
	static_assert(offsetof(ScriptableEventLightSettingsDef, noteworthy) == 0x28);

	struct ScriptableEventLateSplineDef
	{
		char __pad0[40];
		Spline* spline;
	};

	struct ScriptableEventLateStringDef
	{
		char __pad0[24];
		const char* name;
	};

	struct ScriptableEventMayhemDef
	{
		Mayhem* mayhem;
	};

	union ScriptableEventDataUnion
	{
		ScriptableEventModelDef stateModel;
		XModel* model;
		ScriptableEventPartModelDef partModel;
		ScriptableEventFxDef fx;
		ScriptableEventSoundDef sound;
		ScriptableEventAnimationDef anim;
		ScriptableEventExplodeDef explode;
		ScriptableEventHealthDef healthDrain;
		ScriptableEventPhysicsDef physicsLaunch;
		ScriptableEventShakeDef shake;
		ScriptableEventStateChangeDef stateChange;
		ScriptableEventStringDef string;
		ScriptableEventSplineDef spline;
		ScriptableEventLightSettingsDef lightSettings;
		ScriptableEventLateSplineDef lateSpline;
		ScriptableEventLateStringDef lateString;
		ScriptableEventMayhemDef mayhem;
		char data[56];
	};

	struct ScriptableEventDef
	{
		int type;
		char __pad0[4];
		ScriptableEventDataUnion data;
	};
	static_assert(sizeof(ScriptableEventDef) == 0x40);

	struct ScriptableStateDef
	{
		scr_string_t name;
		scr_string_t tagName;
		ScriptableEventDef* events;
		unsigned char eventCount;
		unsigned char damageParentTransferRate;
		unsigned short damageFlags;
		unsigned char damageParentReceiveRate;
		char __pad0;
		unsigned short maxHealth;
	};
	static_assert(sizeof(ScriptableStateDef) == 0x18);
	static_assert(offsetof(ScriptableStateDef, damageFlags) == 18);
	static_assert(offsetof(ScriptableStateDef, damageParentReceiveRate) == 20);

	enum ScriptablePartFlags : unsigned char
	{
		SCRIPTABLE_PART_FLAG_SERVER_INSTANCE = 0x1,
		SCRIPTABLE_PART_FLAG_SERVER_CONTROLLED = 0x2,
	};

	struct ScriptablePartDef
	{
		ScriptableStateDef* states;
		scr_string_t name;
		unsigned char stateCount;
		unsigned char partFlags;
		unsigned char eventStreamTimeRemainIndex;
		unsigned char eventStreamNextChangeTimeIndex;
		unsigned char flagCount;
		char __pad1[7];
		unsigned char* flags;
	};
	static_assert(sizeof(ScriptablePartDef) == 0x20);
	static_assert(offsetof(ScriptablePartDef, partFlags) == 13);
	static_assert(offsetof(ScriptablePartDef, flagCount) == 16);

	union ScriptableNotetrackDataUnion
	{
		ScriptableEventFxDef fx;
		ScriptableEventSoundDef sound;
	};

	struct ScriptableNotetrackDef
	{
		scr_string_t name;
		int type;
		ScriptableNotetrackDataUnion data;
	};
	static_assert(sizeof(ScriptableNotetrackDef) == 0x18);

	struct ScriptableModelTag
	{
		XModel* model;
		scr_string_t tag;
	};
	static_assert(sizeof(ScriptableModelTag) == 0x10);

	struct ScriptableDef
	{
		const char* name;
		XModel* baseModel;
		const char* baseCollision;
		ScriptablePartDef* parts;
		ScriptableNotetrackDef* notetracks;
		char __pad0[6];
		unsigned short partCount;
		unsigned short serverInstancePartCount;
		unsigned short serverControlledPartCount;
		unsigned char notetrackCount;
		unsigned char eventStreamSize;
		unsigned char flagsSize;
		unsigned char modelCount;
		unsigned char partStreamCount;
		unsigned char modelTagCount;
		char __pad2[6];
		XModel** models;
		ScriptableModelTag* modelTags;
	};
	static_assert(sizeof(ScriptableDef) == 0x50);
	static_assert(offsetof(ScriptableDef, partCount) == 46);
	static_assert(offsetof(ScriptableDef, serverInstancePartCount) == 48);
	static_assert(offsetof(ScriptableDef, notetrackCount) == 52);
	static_assert(offsetof(ScriptableDef, models) == 64);

	struct SplinePoint
	{
		float position[2];
	};

	struct SplineDef
	{
		SplinePoint* points;
		unsigned char* data;
		unsigned int dataSize;
		scr_string_t name;
		char __pad0[4];
		unsigned char pointCount;
		char __pad1[3];
	};
	static_assert(sizeof(SplineDef) == 0x20);

	struct Spline
	{
		const char* name;
		SplineDef* splines;
		unsigned char splineCount;
		char __pad0[7];
	};
	static_assert(sizeof(Spline) == 0x18);

	struct PhysWaterPreset
	{
		const char* name;
		float unknownParams[8]; // H1 has 7 here
		float sharedParams[8]; // same values and order as H1
		unsigned int sharedFlags;
		char __pad0[4];
		FxEffectDef* fx[7];
	};
	static_assert(sizeof(PhysWaterPreset) == 0x88);
	static_assert(offsetof(PhysWaterPreset, fx) == 0x50);

	struct DLogHash
	{
		unsigned int hash;
		int index;
	};
	static_assert(sizeof(DLogHash) == 0x8);

	struct DLogHashTable
	{
		DLogHash* list;
		int count;
		int max;
	};

	struct DLogEnumValue
	{
		const char* name;
		char __pad0[16];
	};

	struct DLogField
	{
		const char* name;
		const char* typeName;
		DLogEnumValue* values;
		int valueCount;
		char __pad0[4];
		DLogHashTable hashTable;
	};
	static_assert(sizeof(DLogField) == 0x30);

	struct DLogEvent
	{
		const char* name;
		DLogField* fields;
		int fieldCount;
		char __pad0[4];
		DLogHashTable hashTable;
	};
	static_assert(sizeof(DLogEvent) == 0x28);

	struct DLogSchema
	{
		const char* name;
		DLogEvent* events;
		int eventCount;
		char __pad0[4];
		DLogHashTable hashTable;
	};
	static_assert(sizeof(DLogSchema) == 0x28);

	struct cbrushside_t
	{
		unsigned int planeIndex;
		unsigned short materialNum;
		unsigned char firstAdjacentSideOffset;
		unsigned char edgeCount;
	};
	static_assert(sizeof(cbrushside_t) == 8);

	struct cbrush_t
	{
		cbrushside_t* sides;
		unsigned int numsides;
		short axialMaterialNum[2][3];
	};
	static_assert(sizeof(cbrush_t) == 24);

	struct BrushesCollisionData
	{
		cbrushside_t* brushSides;
		unsigned int numBrushSides;
		unsigned int numBrushes;
		cbrush_t* brushes;
		Bounds* brushBounds;
		unsigned long long* brushContents;
	};
	static_assert(sizeof(BrushesCollisionData) == 0x28);

	struct CollisionBorder
	{
		float distEq[3];
		float zBase;
		float zSlope;
		float start;
		float length;
	};
	static_assert(sizeof(CollisionBorder) == 28);

	struct CollisionPartition
	{
		unsigned char triCount;
		unsigned char borderCount;
		unsigned char firstVertSegment;
		char __pad0[1];
		int firstTri;
		int firstBorder;
		unsigned int materialIndex;
	};
	static_assert(sizeof(CollisionPartition) == 16);

	struct PatchesCollisionData
	{
		unsigned int vertCount;
		char __pad0[4];
		vec3_t* verts;
		unsigned int triCount;
		char __pad1[4];
		unsigned short* triIndices;
		unsigned char* triEdgeIsWalkable;
		unsigned int borderCount;
		char __pad2[4];
		CollisionBorder* borders;
		unsigned int partitionCount;
		char __pad3[4];
		CollisionPartition* partitions;
	};
	static_assert(sizeof(PatchesCollisionData) == 0x48);

	struct ClipMapTrZoneStaticModel
	{
		XModel* model;
		char __pad0[80];
	};
	static_assert(sizeof(ClipMapTrZoneStaticModel) == 0x58);

	struct ClipMapTrZoneStaticModels
	{
		unsigned int count;
		char __pad0[4];
		ClipMapTrZoneStaticModel* models;
	};

	struct ClipMapTrZone
	{
		const char* name;
		unsigned int index;
		char __pad0[4];
		BrushesCollisionData bCollisionData;
		PatchesCollisionData pCollisionData;
		ClipMapTrZoneStaticModels staticModels;
	};
	static_assert(sizeof(ClipMapTrZone) == 0x90);
	static_assert(offsetof(ClipMapTrZone, staticModels) == 128);

	struct TriggerModel
	{
		unsigned long long contents;
		unsigned short hullCount;
		unsigned short firstHull;
		char __pad0[4];
	};
	static_assert(sizeof(TriggerModel) == 16);

	struct TriggerHull
	{
		Bounds bounds;
		unsigned long long contents;
		unsigned short slabCount;
		unsigned short firstSlab;
		char __pad0[4];
	};
	static_assert(sizeof(TriggerHull) == 40);

	struct TriggerSlab
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};
	static_assert(sizeof(TriggerSlab) == 20);

	struct MapTriggers
	{
		unsigned int count;
		TriggerModel* models;
		unsigned int hullCount;
		TriggerHull* hulls;
		unsigned int slabCount;
		TriggerSlab* slabs;
	};
	static_assert(sizeof(MapTriggers) == 0x30);

	struct ClientTriggerAabbNode
	{
		char __pad0[28];
	};
	static_assert(sizeof(ClientTriggerAabbNode) == 28);

	struct ClientTriggerLeafList
	{
		unsigned int* leafs;
		unsigned int leafCount;
		char __pad0[4];
	};

	struct ClientTriggers
	{
		MapTriggers trigger;
		unsigned short numClientTriggerNodes;
		ClientTriggerAabbNode* clientTriggerAabbTree;
		unsigned int triggerStringLength;
		char* triggerString;
		int* visionSetTriggers;
		int* lightSetTriggers;
		int* clutTriggers;
		short* triggerType;
		vec3_t* origins;
		float* scriptDelay;
		short* audioTriggers;
		short* blendLookup;
		short* npcTriggers;
		short* contextTriggers;
		short* waterTriggers;
		ClientTriggerLeafList* leafLists;
	};
	static_assert(sizeof(ClientTriggers) == 0xB0);

	struct ClientTriggerBlendNode
	{
		float pointA[3];
		float pointB[3];
		unsigned short triggerA;
		unsigned short triggerB;
	};
	static_assert(sizeof(ClientTriggerBlendNode) == 28);

	struct ClientTriggerBlend
	{
		unsigned short numClientTriggerBlendNodes;
		ClientTriggerBlendNode* blendNodes;
	};

	struct SpawnPointEntityRecord
	{
		unsigned short index;
		scr_string_t name;
		scr_string_t target;
		scr_string_t script_noteworthy;
		scr_string_t targetname;
		float origin[3];
		float angles[3];
	};
	static_assert(sizeof(SpawnPointEntityRecord) == 44);

	struct SpawnPointRecordList
	{
		unsigned short spawnsCount;
		SpawnPointEntityRecord* spawns;
	};

	struct SplinePointEntityRecord
	{
		int splineId;
		int splineNodeId;
		char* splineNodeLabel;
		float splineNodeTension;
		float origin[3];
		float corridorDims[2];
		float tangent[3];
		float distToNextNode;
		vec3_t* positionCubic;
		vec3_t* tangentQuadratic;
	};
	static_assert(sizeof(SplinePointEntityRecord) == 72);

	struct SplinePointRecordList
	{
		unsigned short splinePointCount;
		float splineLength;
		SplinePointEntityRecord* splinePoints;
	};
	static_assert(sizeof(SplinePointRecordList) == 16);

	struct SplineRecordList
	{
		unsigned short splineCount;
		SplinePointRecordList* splines;
	};

	// h1 ScriptableInstanceTargetData with a 4 float color
	struct ScriptableInstancePartState
	{
		unsigned int lightIndex;
		float lightAxis[4][3];
		float lightColorStorage[4];
		float lightRadiusStorage;
		scr_string_t name;
	};
	static_assert(sizeof(ScriptableInstancePartState) == 76);

	struct ScriptableInstance
	{
		ScriptableDef* def;
		unsigned char* flags;
		ScriptableInstancePartState* partStates;
		float origin[3];
		float angles[3];
		float startOrigin[3];
		float startAngles[3];
		scr_string_t name;
		char __pad0[6];
		unsigned char partStateCount;
		char __pad1[5];
		char (*partRuntime)[4];
		unsigned char* eventStreamBuffer;
		char __pad2[56];
		unsigned short updateNextInstance;
		char __pad3[4];
		short currentModelIndex;
		char __pad4[3];
		unsigned char linkedObjectCount;
		char __pad5[4];
		unsigned char* modelTagRuntime;
		unsigned short* partStreams;
		unsigned short* linkedObjects;
	};
	static_assert(sizeof(ScriptableInstance) == 200);
	static_assert(offsetof(ScriptableInstance, origin) == 24);
	static_assert(offsetof(ScriptableInstance, name) == 72);
	static_assert(offsetof(ScriptableInstance, updateNextInstance) == 160);
	static_assert(offsetof(ScriptableInstance, currentModelIndex) == 166);
	static_assert(offsetof(ScriptableInstance, partRuntime) == 88);
	static_assert(offsetof(ScriptableInstance, partStateCount) == 82);
	static_assert(offsetof(ScriptableInstance, linkedObjectCount) == 171);

	struct ScriptableAnimationEntry
	{
		const char* animName;
		void* runtimeBuf;
	};

	struct ScriptableMapEnts
	{
		unsigned int instanceStateSize;
		unsigned int instanceCount;
		unsigned int reservedInstanceCount;
		ScriptableInstance* instances;
		unsigned int animEntryCount;
		ScriptableAnimationEntry* animEntries;
		unsigned int replicatedInstanceCount;
	};
	static_assert(sizeof(ScriptableMapEnts) == 0x30);
	static_assert(offsetof(ScriptableMapEnts, instances) == 16);
	static_assert(offsetof(ScriptableMapEnts, animEntries) == 32);

	struct MapEntsUnk344Entry
	{
		scr_string_t names[4];
		char __pad0[64];
	};
	static_assert(sizeof(MapEntsUnk344Entry) == 80);

	struct MapEntsUnk344
	{
		unsigned short count;
		MapEntsUnk344Entry* entries;
	};

	struct MapEntsUnk360
	{
		unsigned int count;
		char (*entries)[16];
	};

	struct MapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars;
		MapTriggers trigger;
		ClientTriggers clientTrigger;
		ClientTriggerBlend clientTriggerBlend;
		SpawnPointRecordList spawnList;
		SplineRecordList splineList;
		ScriptableMapEnts scriptableMapEnts;
		MapEntsUnk344 unk344;
		MapEntsUnk360 unk360;
	};
	static_assert(sizeof(MapEnts) == 0x178);
	static_assert(offsetof(MapEnts, clientTrigger) == 72);
	static_assert(offsetof(MapEnts, scriptableMapEnts) == 296);
	static_assert(offsetof(MapEnts, unk360) == 360);

	union PhysMeshNodeIndex
	{
		unsigned int firstTriangle;
		unsigned int rightChildOffset;
	};

	struct PhysMeshNode
	{
		float mins[3];
		float maxs[3];
		PhysMeshNodeIndex u;
		unsigned int flags : 16;
		unsigned int axis : 8;
		unsigned int triangleCount : 8;
	};
	static_assert(sizeof(PhysMeshNode) == 32);

	struct dmMeshTriangle
	{
		int i1;
		int i2;
		int i3;
		int w1;
		int w2;
		int w3;
		int materialIndex;
		int collisionFlags;
	};
	static_assert(sizeof(dmMeshTriangle) == 32);

	struct PhysMeshData
	{
		PhysMeshNode* nodes;
		dmFloat4* vertices;
		dmMeshTriangle* triangles;
		int nodeCount;
		int vertexCount;
		int triangleCount;
		int treeHeight;
		float surfaceArea;
		int unused;
	};
	static_assert(sizeof(PhysMeshData) == 0x30);

	struct PhysBrushModel
	{
		int polytopeStart;
		unsigned short polytopeCount;
		unsigned short meshIndex;
	};
	static_assert(sizeof(PhysBrushModel) == 8);

	struct PhysWaterVolumeDef
	{
		PhysWaterPreset* preset;
		float origin[3]; // the water polytopes are centered on it
		scr_string_t name;
		short brushModelIndex;
		char __pad0[6];
	};
	static_assert(sizeof(PhysWaterVolumeDef) == 0x20);
	static_assert(offsetof(PhysWaterVolumeDef, brushModelIndex) == 24);

	struct PhysWorld
	{
		const char* name;
		PhysBrushModel* models;
		dmPolytopeData* polytopes;
		PhysMeshData* meshes;
		PhysWaterVolumeDef* waters;
		unsigned int modelCount;
		unsigned int polytopeCount;
		unsigned int meshCount;
		unsigned int waterCount;
	};
	static_assert(sizeof(PhysWorld) == 0x38);

	struct DLogDestination
	{
		const char* name;
		const char* type;
		const char* address;
		char __pad0[16];
	};
	static_assert(sizeof(DLogDestination) == 0x28);

	struct DLogRouteEntry
	{
		DLogDestination* destination;
		char __pad0[8];
		const char* name;
		char __pad1[16];
	};
	static_assert(sizeof(DLogRouteEntry) == 0x28);

	struct DLogRoute
	{
		const char* name;
		DLogRouteEntry* entries;
		int entryCount;
		char __pad0[4];
	};
	static_assert(sizeof(DLogRoute) == 0x18);

	struct DLogRoutes
	{
		const char* name;
		DLogDestination* destinations;
		int destinationCount;
		char __pad0[4];
		DLogRoute* routes;
		int routeCount;
		char __pad1[4];
	};
	static_assert(sizeof(DLogRoutes) == 0x28);

	enum WAFieldType : std::uint8_t
	{
		WAFIELD_TYPE_STRING = 0,
		WAFIELD_TYPE_INT = 4,
		WAFIELD_TYPE_BOOL = 6,
		WAFIELD_TYPE_FLOAT = 7,
		WAFIELD_TYPE_MILLISECONDS = 9,
		WAFIELD_TYPE_FX = 10,
		WAFIELD_TYPE_MODEL = 11,
		WAFIELD_TYPE_ANIM = 12,
		WAFIELD_TYPE_MATERIAL = 13,
		WAFIELD_TYPE_SOUND = 15,
		WAFIELD_TYPE_TRACER = 16,
	};

	enum WAFieldCode : std::uint8_t
	{
		WAFIELD_CODE_NONE = 0,
		WAFIELD_CODE_SET = 3,
		WAFIELD_CODE_MULTIPLY = 4,
	};

	union AttachmentFieldValue
	{
		const char* string;
		int intValue;
		float floatValue;
	};

	struct AttachmentField
	{
		unsigned char index;
		unsigned char type;
		unsigned char code;
		char __pad0[5];
		AttachmentFieldValue value;
	};
	static_assert(sizeof(AttachmentField) == 0x10);

	struct WeaponAttachment
	{
		const char* szInternalName;
		const char* szDisplayName;
		int type;
		int weaponType;
		int weapClass;
		int greebleType;
		int unknown32;
		int unknown36;
		XModel** worldModels;
		XModel** viewModels;
		XModel** worldModelsAdditional;
		XModel** viewModelsAdditional;
		XModel** reticleViewModels;
		char (*unkStruct80)[72];
		unsigned short* fieldOffsets;
		AttachmentField* fields;
		int fieldCount;
		int loadIndex;
		int adsSettingsMode;
		float adsSceneBlurStrength;
		scr_string_t knifeAttachTagOverride;
		bool unknown124[9];
		bool extendedMag;
		bool unknown134[2];
		bool treatAsAltWeapon;
		bool tacticalKnife;
		bool unknown138[6];
	};
	static_assert(sizeof(WeaponAttachment) == 0x90);
	static_assert(offsetof(WeaponAttachment, fieldCount) == 104);
	static_assert(offsetof(WeaponAttachment, knifeAttachTagOverride) == 120);
	static_assert(offsetof(WeaponAttachment, extendedMag) == 133);

	struct LaserDef
	{
		const char* name;
		Material* laserMaterial;
		Material* laserLightMaterial;
		FxEffectDef* effect;
		LaserDef* altLaser;
		scr_string_t ownerTag;
		char __pad0[0x44];
	};
	static_assert(sizeof(LaserDef) == 0x70);

	struct BeamDef
	{
		const char* name;
		Material* beamMaterial;
		Material* beamMaterial2;
		char __pad0[0x1C0];
		FxEffectDef* effects[11];
		XModel* models[2];
		char __pad1[0x58];
		const char* string664;
		const char* string672;
		snd_alias_list_t* sounds[4];
		char __pad2[0x8];
		snd_alias_list_t* sounds2[2];
		char __pad3[0x8];
	};
	static_assert(sizeof(BeamDef) == 0x2E8);

	struct AnimOverrideEntry
	{
		unsigned char animHand;
		unsigned char attachment1;
		unsigned char attachment2;
		unsigned char animTreeType;
		XAnimParts* overrideAnim;
		XAnimParts* altmodeAnim;
		int animTime;
		int altTime;
	};
	static_assert(sizeof(AnimOverrideEntry) == 32);

	struct SoundOverrideEntry
	{
		unsigned char attachment1;
		unsigned char attachment2;
		unsigned char soundType;
		snd_alias_list_t* overrideSound;
		snd_alias_list_t* altmodeSound;
	};
	static_assert(sizeof(SoundOverrideEntry) == 24);

	struct FXOverrideEntry
	{
		unsigned char attachment1;
		unsigned char attachment2;
		unsigned char fxType;
		FxEffectDef* overrideFX;
		FxEffectDef* altmodeFX;
	};
	static_assert(sizeof(FXOverrideEntry) == 24);

	struct ReloadStateTimerEntry
	{
		int attachment;
		int reloadAddTime;
		int reloadEmptyAddTime;
		int reloadStartAddTime;
	};
	static_assert(sizeof(ReloadStateTimerEntry) == 16);

	struct NoteTrackToSoundEntry
	{
		int attachment;
		scr_string_t* notetrackSoundMapKeys;
		scr_string_t* notetrackSoundMapValues;
	};
	static_assert(sizeof(NoteTrackToSoundEntry) == 24);

	struct TurretHydraulicSettings
	{
		int enabled;
		int useAxisSounds;
		float minVelocity;
		float maxVelocity;
		snd_alias_list_t* verticalSound;
		snd_alias_list_t* verticalStopSound;
		snd_alias_list_t* horizontalSound;
		snd_alias_list_t* horizontalStopSound;
		snd_alias_list_t* movementSounds[2];
		float soundFadeInTime;
		float soundFadeOutTime;
	};
	static_assert(sizeof(TurretHydraulicSettings) == 0x48);

	struct ADSOverlay
	{
		Material* shader;
		Material* shaderLowRes;
		Material* shaderEMP;
		Material* shaderEMPLowRes;
		int reticle;
		float width;
		float height;
		float widthSplitscreen;
		float heightSplitscreen;
	};
	static_assert(sizeof(ADSOverlay) == 0x38);

	struct StateTimers
	{
		int fireDelay;
		int meleeDelay;
		int meleeChargeDelay;
		int unknown12;
		int detonateDelay;
		int fireTime;
		int rechamberTimeAlt;
		int rechamberTime;
		int rechamberTimeOneHanded;
		int rechamberBoltTime;
		int unknown40;
		int holdFireTime;
		int grenadePrimeReadyToThrowTime;
		int detonateTime;
		int meleeTime;
		int meleeChargeTime;
		int unknown64;
		int reloadTime;
		int reloadJamTime;
		int unknown76;
		int activeReloadSuccessWindowEnd;
		int activeReloadSuccessWindowStart;
		int activeReloadJamWindowEnd;
		int reloadShowRocketTime;
		int reloadEmptyTime;
		int reloadAddTime;
		int reloadEmptyAddTime;
		int reloadStartTime;
		int reloadStartAddTime;
		int reloadEndTime;
		int reloadTimeDualWield;
		int reloadAddTimeDualWield;
		int reloadEmptyDualMag;
		int reloadEmptyAddTimeDualMag;
		int speedReloadTime;
		int speedReloadAddTime;
		int dropTime;
		int raiseTime;
		int inspectTime;
		int unknown156;
		int altDropTime;
		int unknown164;
		int altRaiseTime;
		int quickDropTime;
		int quickRaiseTime;
		int firstRaiseTime;
		int breachRaiseTime;
		int emptyRaiseTime;
		int emptyDropTime;
		int sprintInTime;
		int sprintLoopTime;
		int sprintOutTime;
		int unknown208[2];
		int stunnedTimeBegin;
		int stunnedTimeLoop;
		int stunnedTimeEnd;
		int nightVisionWearTime;
		int nightVisionWearTimeFadeOutEnd;
		int nightVisionWearTimePowerUp;
		int nightVisionRemoveTime;
		int nightVisionRemoveTimePowerDown;
		int nightVisionRemoveTimeFadeInStart;
		int aiFuseTime;
		int fuseTime;
		int missileTime;
		int primeTime;
		bool bHoldFullPrime;
		int blastFrontTime;
		int blastRightTime;
		int blastBackTime;
		int blastLeftTime;
		int slideInTime;
		int slideLoopTime;
		int slideOutTime;
		int crawlInTime;
		int crawlLoopTime;
		int unknown308;
		int unknown312;
		int offhandSwitchTime;
	};
	static_assert(sizeof(StateTimers) == 320);
	static_assert(offsetof(StateTimers, bHoldFullPrime) == 268);
	static_assert(offsetof(StateTimers, slideInTime) == 288);
	static_assert(offsetof(StateTimers, offhandSwitchTime) == 316);

	struct WeaponCompleteDef
	{
		const char* szInternalName;
		const char* szDisplayName;
		XModel** gunModel;
		XModel** gunModelConditions;
		XModel* handModel;
		const char* lobWorldModelName;
		XAnimParts** szXAnimsRightHanded;
		XAnimParts** szXAnimsLeftHanded;
		scr_string_t* hideTags;
		WeaponAttachment** attachments;
		unsigned char unknown80[8]; // 1-based attachment indexes: second sight, muzzle, alt mode
		XAnimParts** szXAnims;
		AnimOverrideEntry* animOverrides;
		SoundOverrideEntry* soundOverrides;
		FXOverrideEntry* fxOverrides;
		ReloadStateTimerEntry* reloadOverrides;
		NoteTrackToSoundEntry* notetrackOverrides;
		scr_string_t* notetrackSoundMapKeys;
		scr_string_t* notetrackSoundMapValues;
		scr_string_t* notetrackRumbleMapKeys;
		scr_string_t* notetrackRumbleMapValues;
		scr_string_t* notetrackFXMapKeys;
		FxEffectDef** notetrackFXMapValues;
		scr_string_t* notetrackFXMapTagValues;
		const char* szAdsrBaseSetting;
		FxEffectDef* viewFlashEffect;
		FxEffectDef* viewBodyFlashEffect;
		FxEffectDef* worldFlashEffect;
		FxEffectDef* viewFlashADSEffect;
		FxEffectDef* viewBodyFlashADSEffect;
		FxEffectDef* signatureViewFlashEffect;
		FxEffectDef* signatureViewBodyFlashEffect;
		FxEffectDef* signatureWorldFlashEffect;
		FxEffectDef* signatureViewFlashADSEffect;
		FxEffectDef* signatureViewBodyFlashADSEffect;
		FxEffectDef* unknownEffect280;
		FxEffectDef* meleeHitEffect;
		FxEffectDef* meleeMissEffect;
		snd_alias_list_t* pickupSound;
		snd_alias_list_t* pickupSoundPlayer;
		snd_alias_list_t* ammoPickupSound;
		snd_alias_list_t* ammoPickupSoundPlayer;
		snd_alias_list_t* projectileSound;
		snd_alias_list_t* pullbackSound;
		snd_alias_list_t* pullbackSoundPlayer;
		snd_alias_list_t* throwbackSound;
		snd_alias_list_t* throwbackSoundPlayer;
		snd_alias_list_t* fireSound;
		snd_alias_list_t* fireSoundPlayer;
		snd_alias_list_t* fireSoundPlayerAkimbo;
		snd_alias_list_t* fireLoopSound;
		snd_alias_list_t* fireLoopSoundPlayer;
		snd_alias_list_t* fireLoopEndPointSound;
		snd_alias_list_t* fireLoopEndPointSoundPlayer;
		snd_alias_list_t* fireStopSound;
		snd_alias_list_t* fireStopSoundPlayer;
		snd_alias_list_t* fireLastSound;
		snd_alias_list_t* fireLastSoundPlayer;
		snd_alias_list_t* fireFirstSound;
		snd_alias_list_t* fireFirstSoundPlayer;
		snd_alias_list_t* fireCustomSound;
		snd_alias_list_t* fireCustomSoundPlayer;
		snd_alias_list_t* emptyFireSound;
		snd_alias_list_t* emptyFireSoundPlayer;
		snd_alias_list_t* adsRequiredFireSoundPlayer;
		snd_alias_list_t* meleeSwipeSound;
		snd_alias_list_t* meleeSwipeSoundPlayer;
		snd_alias_list_t* meleeHitSound;
		snd_alias_list_t* meleeMissSound;
		snd_alias_list_t* meleeHitSoundPlayer;
		snd_alias_list_t* meleeMissSoundPlayer;
		snd_alias_list_t* rechamberSound;
		snd_alias_list_t* rechamberSoundPlayer;
		snd_alias_list_t* reloadSound;
		snd_alias_list_t* reloadSoundPlayer;
		snd_alias_list_t* reloadEmptySound;
		snd_alias_list_t* reloadEmptySoundPlayer;
		snd_alias_list_t* reloadStartSound;
		snd_alias_list_t* reloadStartSoundPlayer;
		snd_alias_list_t* reloadEndSound;
		snd_alias_list_t* reloadEndSoundPlayer;
		snd_alias_list_t* detonateSound;
		snd_alias_list_t* detonateSoundPlayer;
		snd_alias_list_t* nightVisionWearSound;
		snd_alias_list_t* nightVisionWearSoundPlayer;
		snd_alias_list_t* nightVisionRemoveSound;
		snd_alias_list_t* nightVisionRemoveSoundPlayer;
		snd_alias_list_t* altSwitchSound;
		snd_alias_list_t* altSwitchSoundPlayer;
		snd_alias_list_t* raiseSound;
		snd_alias_list_t* raiseSoundPlayer;
		snd_alias_list_t* firstRaiseSound;
		snd_alias_list_t* firstRaiseSoundPlayer;
		snd_alias_list_t* putawaySound;
		snd_alias_list_t* putawaySoundPlayer;
		snd_alias_list_t* scanSound;
		snd_alias_list_t* changeVariableZoomSound;
		snd_alias_list_t* adsUpSound;
		snd_alias_list_t* adsDownSound;
		snd_alias_list_t* adsCrosshairEnemySound;
		snd_alias_list_t* turretMountSound;
		snd_alias_list_t* turretDismountSound;
		snd_alias_list_t* grenadeStickSound;
		snd_alias_list_t** bounceSound;
		snd_alias_list_t** rollingSound;
		FxEffectDef* viewShellEjectEffect;
		FxEffectDef* worldShellEjectEffect;
		FxEffectDef* viewLastShotEjectEffect;
		FxEffectDef* worldLastShotEjectEffect;
		FxEffectDef* viewMagEjectEffect;
		Material* reticleCenter;
		Material* reticleCenterEnemy;
		Material* reticleSide;
		XModel** worldModel;
		XModel** worldModelConditions;
		XModel* worldClipModel;
		XModel* rocketModel;
		Material* hudIcon;
		Material* pickupIcon;
		Material* grenadeIndicatorIcon;
		Material* ammoCounterIcon;
		const char* szAmmoName;
		const char* szClipName;
		const char* szSharedAmmoCapName;
		PhysCollmap* physCollmap;
		PhysPreset* physPreset;
		const char* szUseHintString;
		const char* dropHintString;
		float* locationDamageMultipliers;
		const char* fireRumble;
		const char* unknownRumble1040;
		const char* meleeImpactRumble;
		TracerDef* tracerType;
		TracerDef* signatureTracerType;
		TracerDef* unknownTracer1072;
		LaserDef* laserType;
		BeamDef* beamType[2];
		snd_alias_list_t* turretOverheatSound;
		FxEffectDef* turretOverheatEffect;
		const char* turretBarrelSpinRumble;
		snd_alias_list_t* turretBarrelSpinMaxSnd;
		snd_alias_list_t* turretBarrelSpinUpSnd[4];
		snd_alias_list_t* turretBarrelSpinDownSnd[4];
		snd_alias_list_t* missileConeSoundAlias;
		snd_alias_list_t* missileConeSoundAliasAtBase;
		XModel* stowOffsetModel;
		TurretHydraulicSettings* turretHydraulicSettings;
		unsigned char numAttachments;
		unsigned char numAnimOverrides;
		unsigned char numSoundOverrides;
		unsigned char numFXOverrides;
		unsigned char numReloadStateTimerOverrides;
		unsigned char numNotetrackOverrides;
		unsigned char playerAnimType;
		unsigned char unknown1239;
		int weapType;
		int weapClass;
		int penetrateType;
		float penetrateDepth;
		int impactType;
		int unknown1260;
		int inventoryType;
		int fireType;
		int fireBarrels;
		int adsFireMode;
		float burstFireCooldown;
		int greebleType;
		int autoReloadType;
		int autoHolsterType;
		int offhandClass;
		int stance;
		int reticleCenterSize;
		int reticleCenterEnemySize;
		int reticleSideSize;
		int reticleMinOfs;
		int activeReticleType;
		float standMove[3];
		float standRot[3];
		float strafeMove[3];
		float strafeRot[3];
		float duckedOfs[3];
		float duckedMove[3];
		float duckedRot[3];
		float proneOfs[3];
		float proneMove[3];
		float proneRot[3];
		float posMoveRate;
		float posProneMoveRate;
		float standMoveMinSpeed;
		float duckedMoveMinSpeed;
		float proneMoveMinSpeed;
		float posRotRate;
		float posProneRotRate;
		int hudIconRatio;
		int pickupIconRatio;
		int grenadeIndicatorIconRatio;
		int ammoCounterIconRatio;
		int ammoCounterClip;
		int unknown1492;
		int unknown1496[7];
		int maxAmmo;
		int maxAmmoClips;
		int minAmmoReq;
		int startAmmo;
		int startAmmoClips;
		int clipSize;
		int shotCount;
		int sharedAmmoCapIndex;
		int sharedAmmoCap;
		int unknown1560; // 95 in stock
		int unknown1564;
		int meleeDamage;
		int damageType;
		StateTimers stateTimers;
		StateTimers akimboStateTimers;
		bool unknown2216[8];
		float autoAimRange;
		float aimAssistRange;
		float aimAssistRangeAds;
		float aimPadding;
		int unknown2240;
		float unknown2244;
		int unknown2248;
		float maxBreathHoldTime;
		float unknown2256;
		float enemyCrosshairRange;
		float moveSpeedScale;
		float adsMoveSpeedScale;
		float sprintDurationScale;
		float unknown2276[11];
		float adsZoomFov;
		float adsZoomInFrac;
		float adsZoomOutFrac;
		float adsSceneBlur;
		ADSOverlay overlay;
		int overlayInterface;
		float adsBobFactor;
		float adsViewBobMult;
		float hipSpreadDecayRate;
		float hipSpreadStandMin;
		float hipSpreadStandMax;
		float hipSpreadStandDecay;
		float hipSpreadDuckedMin;
		float hipSpreadDuckedMax;
		float hipSpreadDuckedDecay;
		float hipSpreadProneMin;
		float hipSpreadProneMax;
		float hipSpreadProneDecay;
		float hipSpreadSprintMin;
		float hipSpreadSprintMax;
		float hipSpreadSprintDecay;
		float hipSpreadSlideMin;
		float hipSpreadSlideMax;
		float hipSpreadSlideDecay;
		float hipSpreadMoveAdd;
		float hipSpreadTurnAdd;
		float hipSpreadFireAdd;
		float hipReticleSidePos;
		float adsIdleAmount;
		float hipIdleAmount;
		float adsIdleSpeed;
		float hipIdleSpeed;
		int unknown2500;
		float idleCrouchFactor;
		float idleProneFactor;
		float gunMaxPitch;
		float gunMaxYaw;
		int adsTransInTime;
		int adsTransInFromSprintTime;
		int adsTransOutTime;
		float unknown2532[2];
		int unknown2540;
		float swayMaxAngle;
		float swayMaxAngleSteadyAim;
		float swayLerpSpeed;
		float swayPitchScale;
		float swayYawScale;
		float swayHorizScale;
		float swayVertScale;
		float swayShellShockScale;
		float adsSwayMaxAngle;
		float adsSwayLerpSpeed;
		float adsSwayPitchScale;
		float adsSwayYawScale;
		float adsSwayHorizScale;
		float adsSwayVertScale;
		float adsViewErrorMin;
		float adsViewErrorMax;
		float adsFireAnimFrac;
		float scopeDriftDelay;
		float scopeDriftLerpInTime;
		float scopeDriftSteadyTime;
		float scopeDriftLerpOutTime;
		float scopeDriftSteadyFactor;
		float scopeDriftUnsteadyFactor;
		float bobVerticalFactor;
		float bobHorizontalFactor;
		float bobViewVerticalFactor;
		float bobViewHorizontalFactor;
		float stationaryZoomFov;
		float stationaryZoomDelay;
		float stationaryZoomLerpInTime;
		float stationaryZoomLerpOutTime;
		float adsDofStart;
		float adsDofEnd;
		float unknown2676;
		Material* killIcon;
		Material* dpadIcon;
		Material* hudProximityWarningIcon;
		int killIconRatio;
		int dpadIconRatio;
		int fireAnimLength;
		int fireAnimLengthAkimbo;
		int reloadAmmoAdd;
		int reloadStartAdd;
		int ammoDropStockMin;
		int ammoDropStockMax;
		int ammoDropClipPercentMin;
		int ammoDropClipPercentMax;
		int explosionRadius;
		int explosionRadiusMin;
		int explosionInnerDamage;
		int explosionOuterDamage;
		float tankMetalSolidDamageMult;
		float tankMetalThinDamageMult;
		float damageConeAngle;
		int bulletExplosionRadius;
		int bulletExplosionInnerDamage;
		int bulletExplosionOuterDamage;
		int projectileSpeed;
		int projectileSpeedUp;
		int projectileSpeedForward;
		int projectileSpeedHorizontal;
		int projectileActivateDist;
		float projLifetime;
		float timeToAccelerate;
		float projectileCurvature;
		float explosionShakeRadius;
		float explosionShakeScale;
		float explosionShakeDuration;
		float projectileSpinRate;
		float unknown2832[3];
		float unknown2844;
		const char* projectileName;
		XModel* projectileModel;
		FxEffectDef* projExplosionEffect;
		FxEffectDef* projDudEffect;
		FxEffectDef* unknownEffect2880;
		snd_alias_list_t* projExplosionSound;
		snd_alias_list_t* projDudSound;
		snd_alias_list_t* unknownSound2904;
		int projExplosion;
		int stickiness;
		int unknown2920;
		float lowAmmoWarningThreshold;
		float ricochetChance;
		int riotShieldHealth;
		float riotShieldDamageMult;
		int unknown2940;
		float* parallelBounce;
		float* perpendicularBounce;
		FxEffectDef* projTrailEffect;
		FxEffectDef* projBeaconEffect;
		float projectileColor[3];
		int guidedMissileType;
		float maxSteeringAccel;
		float unknown2996;
		float guidedPitchMin;
		float guidedPitchMax;
		float guidedYawLimit;
		float guidedRollLimit;
		float guidedPitchRate;
		float guidedYawRate;
		int projIgnitionDelay;
		float unknown3028;
		FxEffectDef* projIgnitionEffect;
		snd_alias_list_t* projIgnitionSound;
		float adsAimPitch;
		float adsCrosshairInFrac;
		float adsCrosshairOutFrac;
		float adsGunKickPitchMin;
		float adsGunKickPitchMax;
		float adsGunKickYawMin;
		float adsGunKickYawMax;
		float adsGunKickMagMin;
		float adsGunKickAccel;
		float adsGunKickSpeedMax;
		float adsGunKickSpeedDecay;
		float adsGunKickStaticDecay;
		int adsGunKickReducedKickBullets;
		float adsGunKickReducedKickPercent;
		float adsViewKickPitchMin;
		float adsViewKickPitchMax;
		float adsViewKickYawMin;
		float adsViewKickYawMax;
		float adsViewKickMagMin;
		float adsViewKickCenterSpeed;
		float adsViewScatterMin;
		float adsViewScatterMax;
		int adsViewKickAltBullets;
		float adsViewKickAltPitchMin;
		float adsViewKickAltPitchMax;
		float adsViewKickAltYawMin;
		float adsViewKickAltYawMax;
		float adsViewKickAltMagMin;
		float adsViewKickAltCenterSpeed;
		float adsViewScatterAltMin;
		float adsViewScatterAltMax;
		float unknown3172[3];
		float adsSpread;
		float adsKickDuckedScale;
		float adsKickProneScale;
		float adsKickCenterSpeedDuckedScale;
		float adsKickCenterSpeedProneScale;
		bool unknown3204[4];
		float unknown3208[5];
		float hipGunKickPitchMin;
		float hipGunKickPitchMax;
		float hipGunKickYawMin;
		float hipGunKickYawMax;
		float hipGunKickMagMin;
		float hipGunKickAccel;
		float hipGunKickSpeedMax;
		float hipGunKickSpeedDecay;
		float hipGunKickStaticDecay;
		int hipGunKickReducedKickBullets;
		float hipGunKickReducedKickPercent;
		float hipViewKickPitchMin;
		float hipViewKickPitchMax;
		float hipViewKickYawMin;
		float hipViewKickYawMax;
		float hipViewKickMagMin;
		float hipViewKickCenterSpeed;
		float hipViewScatterMin;
		float hipViewScatterMax;
		int hipViewKickAltBullets;
		float hipViewKickAltPitchMin;
		float hipViewKickAltPitchMax;
		float hipViewKickAltYawMin;
		float hipViewKickAltYawMax;
		float hipViewKickAltMagMin;
		float hipViewKickAltCenterSpeed;
		float hipViewScatterAltMin;
		float hipViewScatterAltMax;
		float unknown3340[2];
		float hipKickDuckedScale;
		float hipKickProneScale;
		float hipKickCenterSpeedDuckedScale;
		float hipKickCenterSpeedProneScale;
		float unknown3364[5];
		int positionReloadTransTime;
		int unknown3388;
		float unknown3392[2];
		const char* accuracyGraphName[2];
		vec2_t* accuracyGraphKnots[2];
		vec2_t* originalAccuracyGraphKnots[2];
		unsigned short accuracyGraphKnotCount[2];
		unsigned short originalAccuracyGraphKnotCount[2];
		float maxRange;
		float turretFireRange;
		float leftArc;
		float rightArc;
		float topArc;
		float bottomArc;
		float accuracy;
		float aiSpread;
		float playerSpread;
		float minTurnSpeed[2];
		float maxTurnSpeed[2];
		float pitchConvergenceTime;
		float yawConvergenceTime;
		float suppressTime;
		float animHorRotateInc;
		float playerPositionDist;
		float unknown3528;
		unsigned int useHintStringIndex;
		unsigned int dropHintStringIndex;
		float horizViewJitter;
		float vertViewJitter;
		float scanSpeed;
		float scanAccel;
		int scanPauseTime;
		const char* szScript;
		bool damageFalloffLerpNear;
		bool damageFalloffLerpFar;
		unsigned char unknown3570[2];
		int damage;
		float unknown3576;
		int burnDamage;
		float burnDuration;
		int burnType;
		float unknown3592;
		int unknown3596;
		float unknown3600;
		int unknown3604;
		int playerDamage;
		float maxDamageRange;
		int mid1Damage;
		int mid1PlayerDamage;
		float mid1DamageRange;
		int mid2Damage;
		int mid2PlayerDamage;
		int minDamage;
		int minPlayerDamage;
		float minDamageRange;
		float unknown3648;
		int unknown3652[9]; // even slots 0..6 add to the damage tiers
		int signatureDamage;
		int signatureMid1Damage;
		int signatureMinDamage;
		float signatureMaxDamageRange;
		float signatureMinDamageRange;
		float destabilizationRateTime;
		float destabilizationCurvatureMax;
		int destabilizeDistance;
		float turretADSTime;
		float turretFov;
		float turretFovADS;
		float turretScopeZoomRate;
		float turretScopeZoomMin;
		float turretScopeZoomMax;
		float overheatUpRate;
		float overheatDownRate;
		float overheatCooldownRate;
		float overheatPenalty;
		float turretBarrelSpinSpeed;
		float turretBarrelSpinUpTime;
		float missileConeSoundRadiusAtTop;
		float missileConeSoundRadiusAtBase;
		float missileConeSoundHeight;
		float missileConeSoundOriginOffset;
		float missileConeSoundVolumescaleAtCore;
		float missileConeSoundVolumescaleAtEdge;
		float missileConeSoundVolumescaleCoreSize;
		float missileConeSoundPitchAtTop;
		float missileConeSoundPitchAtBottom;
		float missileConeSoundPitchTopSize;
		float missileConeSoundPitchBottomSize;
		float missileConeSoundCrossfadeTopSize;
		float missileConeSoundCrossfadeBottomSize;
		scr_string_t knifeAttachTagOverride;
		float aim_automelee_lerp;
		float aim_automelee_range;
		float unknown3832;
		float aim_automelee_region_width;
		float aim_automelee_region_height;
		float player_meleeHeight;
		float player_meleeRange;
		float player_meleeWidth;
		float unknown3856[5];
		float changedFireTime;
		int changedFireTimeNumBullets;
		int fireTimeInterpolationType;
		int generateAmmo;
		int ammoPerShot;
		int explodeCount;
		int unknown3900[7];
		float unknown3928[4];
		scr_string_t stowTag;
		scr_string_t unknownTag3948;
		unsigned char rattleSoundType;
		bool adsShouldShowCrosshair;
		bool unknown3954;
		bool adsCrosshairShouldScale;
		bool turretADSEnabled;
		bool unknown3957;
		bool meleeOverrideValues;
		bool unknown3959;
		bool unknown3960;
		bool unknown3961;
		bool lockonSupported;
		bool requireLockonToFire;
		bool isAirburstWeapon;
		bool bigExplosion;
		bool noAdsWhenMagEmpty;
		bool unknown3967;
		bool inheritsPerks;
		bool crosshairColorChange;
		bool rifleBullet;
		bool armorPiercing;
		bool boltAction;
		bool aimDownSight;
		bool unknown3974;
		bool canHoldBreath;
		bool unknown3976;
		bool unknown3977;
		bool bulletExplosiveDamage;
		bool cookOffHold;
		bool unknown3980;
		bool reticleSpin45;
		bool unknown3982;
		bool clipOnly;
		bool noAmmoPickup;
		bool unknown3985;
		bool unknown3986;
		bool unknown3987;
		bool unknown3988;
		bool motionTracker;
		bool markableViewmodel;
		bool unknown3991;
		bool flipKillIcon;
		bool dpadIconShowsAmmo;
		bool noPartialReload;
		bool segmentedReload;
		bool multipleReload;
		bool blocksProne;
		bool silenced;
		bool isRollingGrenade;
		bool projExplosionEffectForceNormalUp;
		bool projExplosionEffectInheritParentDirection;
		bool unknown4002;
		bool projImpactExplode;
		bool unknown4004;
		bool unknown4005;
		bool stickToPlayers;
		bool stickToVehicles;
		bool stickToTurrets;
		bool thrownSideways;
		bool detonatesOnEmptyThrow;
		bool detonatesOnDoubleTap;
		bool disableFiring;
		bool timedDetonation;
		bool unknown4014;
		bool fuseLitAfterImpact;
		bool rotate;
		bool holdButtonToThrow;
		bool freezeMovementWhenFiring;
		bool unknown4019;
		bool unknown4020;
		bool turretBarrelSpinEnabled;
		bool missileConeSoundEnabled;
		bool missileConeSoundPitchshiftEnabled;
		bool missileConeSoundCrossfadeEnabled;
		bool offhandHoldIsCancelable;
		bool doNotAllowAttachmentsToOverrideSpread;
		bool unknown4027;
		bool useDualMagReloadAnims;
		bool unknown4029;
		bool unknown4030;
		bool alwaysShatterGlassOnImpact;
		bool unknown4032;
		bool unknown4033;
		bool unknown4034;
		bool unknown4035;
		bool counterSilencer;
		bool projSuppressedByEMP;
		bool projDisabledByEMP;
		bool unknown4039;
		bool unknown4040;
		bool unknown4041;
		bool projPlayTrailEffectForOwnerOnly;
		bool projPlayBeaconEffectForOwnerOnly;
		bool projKillTrailEffectOnDeath;
		bool projKillBeaconEffectOnDeath;
		bool unknown4046;
		bool cloaked;
		bool unknown4048;
		bool hasTransientModels;
		bool unknown4050;
		bool useScriptCallbackForHit;
		bool unknown4052;
		bool unknown4053;
		bool unknown4054;
		bool unknown4055;
		float adsDofPhysicalFstop;
		float adsDofPhysicalFocusDistance;
		float unknown4064[2];
		int unknown4072;
		float unknown4076[4];
		int unknown4092;
		float unknown4096;
		int unknown4100;
		float unknown4104[24];
		int unknown4200[2];
		float unknown4208[4];
		int unknown4224;
		unsigned char unknown4228[4];
		int unknown4232[2];
	};
	static_assert(sizeof(WeaponCompleteDef) == 0x1090);
	static_assert(offsetof(WeaponCompleteDef, numAttachments) == 1232);
	static_assert(offsetof(WeaponCompleteDef, weapType) == 1240);
	static_assert(offsetof(WeaponCompleteDef, stateTimers) == 1576);
	static_assert(offsetof(WeaponCompleteDef, akimboStateTimers) == 1896);
	static_assert(offsetof(WeaponCompleteDef, overlay) == 2336);
	static_assert(offsetof(WeaponCompleteDef, killIcon) == 2680);
	static_assert(offsetof(WeaponCompleteDef, projectileName) == 2848);
	static_assert(offsetof(WeaponCompleteDef, accuracyGraphName) == 3400);
	static_assert(offsetof(WeaponCompleteDef, accuracyGraphKnotCount) == 3448);
	static_assert(offsetof(WeaponCompleteDef, szScript) == 3560);
	static_assert(offsetof(WeaponCompleteDef, knifeAttachTagOverride) == 3820);
	static_assert(offsetof(WeaponCompleteDef, stowTag) == 3944);
	static_assert(offsetof(WeaponCompleteDef, ammoCounterClip) == 1488);
	static_assert(offsetof(WeaponCompleteDef, maxAmmo) == 1524);
	static_assert(offsetof(WeaponCompleteDef, scanPauseTime) == 3556);
	static_assert(offsetof(WeaponCompleteDef, mid2Damage) == 3628);
	static_assert(offsetof(WeaponCompleteDef, minDamage) == 3636);
	static_assert(offsetof(WeaponCompleteDef, rattleSoundType) == 3952);
	static_assert(offsetof(WeaponCompleteDef, useScriptCallbackForHit) == 4051);

	struct SoundSubmixEntry
	{
		const char* name;
		char __pad0[0x10];
	};

	struct SoundSubmix
	{
		const char* name;
		SoundSubmixEntry* entries;
		int entryCount;
	};
	static_assert(sizeof(SoundSubmix) == 0x18);

	struct SndMusicSubState
	{
		const char* name;
		char __pad0[0x50];
	};
	static_assert(sizeof(SndMusicSubState) == 0x58);

	struct SndMusicState
	{
		const char* name;
		char __pad0[0x8];
		SndMusicSubState intro;
		SndMusicSubState outro;
		unsigned int loopCount;
		SndMusicSubState* loops;
		char __pad1[0x18];
	};
	static_assert(sizeof(SndMusicState) == 0xE8);

	struct SndMusicSet
	{
		const char* name;
		char __pad0[0x4];
		unsigned int stateCount;
		SndMusicState* states;
	};
	static_assert(sizeof(SndMusicSet) == 0x18);

	struct AnimationAimSet
	{
		scr_string_t name;
		scr_string_t rootName;
		scr_string_t animName[8];
		char __pad0[0x48];
	};
	static_assert(sizeof(AnimationAimSet) == 0x70);

	struct AnimationEntry
	{
		scr_string_t animName;
		scr_string_t alias;
	};

	struct AnimationState
	{
		scr_string_t name;
		scr_string_t notify;
		char __pad0[0x9];
		unsigned char entryCount;
		char __pad1[0x6];
		AnimationAimSet* aimSet;
		AnimationEntry* animEntries;
		void** runtime;
	};
	static_assert(sizeof(AnimationState) == 0x30);

	struct AnimationStateMachine
	{
		scr_string_t name;
		unsigned short stateCount;
		unsigned short aimSetCount;
		AnimationState* states;
		AnimationAimSet* aimSets;
	};
	static_assert(sizeof(AnimationStateMachine) == 0x18);

	struct AnimationClass
	{
		const char* name;
		AnimationStateMachine* stateMachine;
		char __pad0[0x4];
		scr_string_t animTree;
		ScriptableDef* scriptable;
		unsigned short soundCount;
		unsigned short effectCount;
		char __pad1[0x4];
		scr_string_t* soundNotes;
		scr_string_t* soundNames;
		scr_string_t* soundOptions;
		scr_string_t* effectNotes;
		FxEffectDef** effects;
		scr_string_t* effectTags;
	};
	static_assert(sizeof(AnimationClass) == 0x58);

	struct cplane_s
	{
		float normal[3];
		float dist;
		unsigned char type;
		unsigned char signbits;
		unsigned char pad[2];
	};
	static_assert(sizeof(cplane_s) == 0x14);

	struct ClipMaterial
	{
		const char* name;
		unsigned int surfaceFlags;
		char __pad0[4];
		unsigned long long contents;
	};
	static_assert(sizeof(ClipMaterial) == 0x18);

	struct cLeafBrushNodeLeaf_t
	{
		unsigned int* brushes;
	};

	struct cLeafBrushNodeChildren_t
	{
		float dist;
		float range;
		unsigned short childOffset[2];
	};

	union cLeafBrushNodeData_t
	{
		cLeafBrushNodeLeaf_t leaf;
		cLeafBrushNodeChildren_t children;
	};

	struct cLeafBrushNode_s
	{
		unsigned char axis;
		unsigned char transientZone;
		char __pad0[2];
		int leafBrushCount;
		unsigned long long contents;
		cLeafBrushNodeData_t data;
	};
	static_assert(sizeof(cLeafBrushNode_s) == 0x20);

	struct BrushesCollisionTree
	{
		unsigned int leafbrushNodesCount;
		cLeafBrushNode_s* leafbrushNodes;
		unsigned int numLeafBrushes;
		unsigned int* leafbrushes;
	};
	static_assert(sizeof(BrushesCollisionTree) == 0x20);

	union CollisionAabbTreeIndex
	{
		unsigned int firstChildIndex;
		unsigned int partitionIndex;
	};

	struct CollisionAabbTree
	{
		float midPoint[3];
		unsigned char childCount;
		unsigned char transientZone;
		char __pad0[2];
		float halfSize[3];
		CollisionAabbTreeIndex u;
	};
	static_assert(sizeof(CollisionAabbTree) == 32);

	struct PatchesCollisionTree
	{
		int aabbTreeCount;
		char __pad0[4];
		CollisionAabbTree* aabbTrees;
	};
	static_assert(sizeof(PatchesCollisionTree) == 0x10);

	struct SModelAabbNode
	{
		Bounds bounds;
		unsigned int firstChild;
		unsigned char childCount;
		unsigned char transientZone;
		char __pad0[2];
	};
	static_assert(sizeof(SModelAabbNode) == 32);

	struct SModelsCollisionTree
	{
		unsigned short numStaticModels;
		unsigned short smodelNodeCount;
		char __pad0[4];
		SModelAabbNode* smodelNodes;
	};
	static_assert(sizeof(SModelsCollisionTree) == 0x10);

	struct ClipTransientZoneInfo
	{
		unsigned int brushStart;
		unsigned int brushEnd;
		char __pad0[16];
		unsigned int partitionCount;
	};
	static_assert(sizeof(ClipTransientZoneInfo) == 0x1C);

	struct ClipTransientZoneTable
	{
		unsigned int count;
		ClipTransientZoneInfo zones[16];
	};

	struct ClipInfo
	{
		int planeCount;
		cplane_s* planes;
		unsigned int numMaterials;
		ClipMaterial* materials;
		BrushesCollisionTree bCollisionTree;
		PatchesCollisionTree pCollisionTree;
		SModelsCollisionTree sCollisionTree;
		ClipTransientZoneTable transientZones;
		char __pad0[4];
		ClipMapTrZone* trZones[16];
	};
	static_assert(sizeof(ClipInfo) == 0x2A8);
	static_assert(offsetof(ClipInfo, transientZones) == 0x60);
	static_assert(offsetof(ClipInfo, trZones) == 0x228);

	struct cNode_t
	{
		cplane_s* plane;
		short children[2];
		unsigned char transientZone;
		char __pad0[3];
	};
	static_assert(sizeof(cNode_t) == 0x10);

	struct cLeaf_t
	{
		unsigned int firstCollAabbIndex;
		unsigned int collAabbCount;
		unsigned long long brushContents;
		unsigned long long terrainContents;
		Bounds bounds;
		int leafBrushNode;
		unsigned char brushTransientZone;
		unsigned char aabbTransientZone;
		char __pad0[2];
	};
	static_assert(sizeof(cLeaf_t) == 56);

	struct cmodel_t
	{
		Bounds bounds;
		float radius;
		char __pad0[4];
		ClipInfo* info;
		cLeaf_t leaf;
		unsigned short unk_0x60;
		char __pad1[6];
	};
	static_assert(sizeof(cmodel_t) == 0x68);

	struct Stage
	{
		const char* name;
		float origin[3];
		unsigned short triggerIndex;
		unsigned char sunPrimaryLightIndex;
		char __pad0[1];
		float radiometricScale;
		char __pad1[4];
	};
	static_assert(sizeof(Stage) == 0x20);

	enum DynEntityType : std::int32_t
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_DESTRUCT = 0x2,
		DYNENT_TYPE_HINGE = 0x3,
		DYNENT_TYPE_SCRIPTABLEINST = 0x4,
		DYNENT_TYPE_SCRIPTABLEPHYSICS = 0x5,
		DYNENT_TYPE_LINKED = 0x6,
		DYNENT_TYPE_LINKED_NOSHADOW = 0x7,
		DYNENT_TYPE_COUNT = 0x8,
	};

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	struct DynEntityHingeDef
	{
		float axisOrigin[3];
		float axisDir[3];
		bool isLimited;
		float angleMin;
		float angleMax;
		float momentOfInertia;
		float friction;
	};
	static_assert(sizeof(DynEntityHingeDef) == 44);

	struct DynEntityLinkToDef
	{
		int anchorIndex;
		float originOffset[3];
		float angleOffset[3];
	};
	static_assert(sizeof(DynEntityLinkToDef) == 28);

	struct DynEntityDef
	{
		DynEntityType type;
		GfxPlacement pose;
		XModel* xModel;
		unsigned short brushModel;
		unsigned short physicsBrushModel;
		unsigned short scriptableIndex;
		unsigned short health;
		FxEffectDef* destroyFx;
		snd_alias_list_t* sound;
		PhysPreset* physPreset;
		DynEntityHingeDef* hinge;
		DynEntityLinkToDef* linkTo;
		char __pad0[16];
		unsigned long long contents;
		float unk_0x70[2];
	};
	static_assert(sizeof(DynEntityDef) == 0x78);
	static_assert(offsetof(DynEntityDef, xModel) == 0x20);
	static_assert(offsetof(DynEntityDef, contents) == 0x68);

	struct ClipMapScrEntry
	{
		scr_string_t name;
		char __pad0[0x4];
	};

	struct ClipMayhemInstance
	{
		char __pad0[0x20];
		Mayhem* mayhem;
		char (*transform)[28];
		scr_string_t name;
		char __pad1[0x14];
	};
	static_assert(sizeof(ClipMayhemInstance) == 0x48);

	struct clipMap_t
	{
		const char* name;
		char __pad0[0x8];
		ClipInfo info;
		ClipInfo* pInfo;
		unsigned int numNodes;
		cNode_t* nodes;
		unsigned int numLeafs;
		cLeaf_t* leafs;
		unsigned int numSubModels;
		cmodel_t* cmodels;
		MapEnts* mapEnts;
		Stage* stages;
		unsigned char stageCount;
		char __pad1[0x7];
		MapTriggers stageTrigger;
		unsigned short dynEntCount[2];
		char __pad2[0x4];
		DynEntityDef* dynEntDefList[2];
		char (*dynEntPoseList[2])[60];
		char (*dynEntClientList[2])[48];
		char (*dynEntCollList[2])[20];
		unsigned int scrStringCount;
		scr_string_t* scrStrings;
		unsigned int scrEntryCount;
		ClipMapScrEntry* scrEntries;
		unsigned int mayhemCount;
		ClipMayhemInstance* mayhems;
		unsigned int checksum;
		char __pad3[0x4C];
	};
	static_assert(sizeof(clipMap_t) == 0x400);
	static_assert(offsetof(clipMap_t, checksum) == 0x3B0);

	struct FxImpactEntry
	{
		FxEffectDef* nonflesh[62];
		FxEffectDef* flesh[7];
	};
	static_assert(sizeof(FxImpactEntry) == 0x228);

	struct FxImpactSurfaceEntry
	{
		const char* name;
		char __pad0[0x8];
		FxEffectDef* effect;
		char __pad1[0x8];
	};
	static_assert(sizeof(FxImpactSurfaceEntry) == 0x20);

	struct FxImpactTable
	{
		const char* name;
		FxImpactEntry* table;
		FxImpactSurfaceEntry* surfaces;
	};
	static_assert(sizeof(FxImpactTable) == 0x18);

	struct ComPrimaryLight
	{
		unsigned char type;
		unsigned char canUseShadowMap;
		unsigned char exponent;
		char unk_0x3[9];
		float color[3];
		float intensity;
		float dir[3];
		float up[3];
		float origin[3];
		float fadeOffset[2];
		float bulbRadius;
		float bulbLength[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float cosHalfFovExpanded;
		float rotationLimit;
		float translationLimit;
		char unk_0x70[44];
		int unk_0x9C;
		const char* defName;
	};
	static_assert(sizeof(ComPrimaryLight) == 0xA8);

	struct ComPrimaryLightEnv
	{
		unsigned short primaryLightIndices[4];
		unsigned char numIndices;
		char __pad0[1];
	};
	static_assert(sizeof(ComPrimaryLightEnv) == 10);

	struct ComWorld
	{
		const char* name;
		unsigned int isInUse;
		unsigned int unk_0xC;
		unsigned int primaryLightCount;
		ComPrimaryLight* primaryLights;
		unsigned int primaryLightEnvCount;
		ComPrimaryLightEnv* primaryLightEnvs;
	};
	static_assert(sizeof(ComWorld) == 0x30);

	struct G_GlassPiece
	{
		unsigned short damageTaken;
		unsigned short collapseTime;
		int lastStateChangeTime;
		unsigned char impactDir;
		unsigned char impactPos[2];
	};
	static_assert(sizeof(G_GlassPiece) == 12);

	// same layout as h1, name is a script string
	struct G_GlassName
	{
		const char* nameStr;
		scr_string_t name;
		unsigned short pieceCount;
		unsigned short* pieceIndices;
	};
	static_assert(sizeof(G_GlassName) == 0x18);

	struct G_GlassData
	{
		G_GlassPiece* glassPieces;
		unsigned int pieceCount;
		unsigned short damageToWeaken;
		unsigned short damageToDestroy;
		unsigned int glassNameCount;
		G_GlassName* glassNames;
		char __pad0[0x70];
	};
	static_assert(sizeof(G_GlassData) == 0x90);
	static_assert(offsetof(G_GlassData, glassNames) == 0x18);

	struct GlassWorld
	{
		const char* name;
		G_GlassData* g_glassData;
	};
	static_assert(sizeof(GlassWorld) == 0x10);

	struct NavMeshRegion
	{
		scr_string_t name;
		char __pad0[0x14];
		int dataSize;
		char __pad1[0x38];
		bool hasExtraData;
		char __pad2[0x3];
		unsigned char* data;
		unsigned char* extraData;
	};
	static_assert(sizeof(NavMeshRegion) == 0x68);

	struct NavMeshEdge
	{
		char (*points)[16];
		int pointCount;
		char __pad0[0x1C];
	};
	static_assert(sizeof(NavMeshEdge) == 0x28);

	struct NavMeshEdgeList
	{
		NavMeshEdge* edges;
		int edgeCount;
		char __pad0[0x4];
	};
	static_assert(sizeof(NavMeshEdgeList) == 0x10);

	struct NavMeshPolygon
	{
		char (*points)[16];
		int pointCount;
		char __pad0[0x14];
	};
	static_assert(sizeof(NavMeshPolygon) == 0x20);

	struct NavMeshVolume
	{
		char (*points)[16];
		int pointCount;
		char __pad0[0xC];
		scr_string_t name;
		char __pad1[0x14];
	};
	static_assert(sizeof(NavMeshVolume) == 0x30);

	struct NavMeshLink
	{
		char __pad0[0x30];
		scr_string_t names[3];
		char __pad1[0xC];
	};
	static_assert(sizeof(NavMeshLink) == 0x48);

	struct NavMeshCluster
	{
		char (*data)[36];
		int count;
		char __pad0[0x4];
	};
	static_assert(sizeof(NavMeshCluster) == 0x10);

	struct NavMeshData
	{
		const char* name;
		char __pad0[0x8];
		int regionCount;
		NavMeshRegion* regions;
		int edgeListCount;
		NavMeshEdgeList* edgeLists;
		int polygonCount;
		NavMeshPolygon* polygons;
		int volumeCount;
		NavMeshVolume* volumes;
		int linkCount;
		NavMeshLink* links;
		int nodeCount;
		char (*nodes)[12];
		int faceCount;
		char (*faces)[36];
		int clusterCount;
		NavMeshCluster* clusters;
	};
	static_assert(sizeof(NavMeshData) == 0x90);

	struct SurfaceFxEntry
	{
		FxEffectDef* surfaceEffect[62];
	};
	static_assert(sizeof(SurfaceFxEntry) == 0x1F0);

	struct SurfaceFxTable
	{
		const char* name;
		SurfaceFxEntry* table;
	};
	static_assert(sizeof(SurfaceFxTable) == 0x10);

	struct VectorSubField
	{
		char (*vectorData)[16];
		char __pad0[0x24];
		unsigned int numBlocks[3];
		char __pad1[0x8];
	};
	static_assert(sizeof(VectorSubField) == 0x40);

	struct VectorField
	{
		const char* name;
		VectorSubField* subFields;
		char __pad0[0x18];
		unsigned int numSubFields;
		char __pad1[0x4];
	};
	static_assert(sizeof(VectorField) == 0x30);

	struct FxGlassDef
	{
		float halfThickness;
		vec2_t texVecs[2];
		vec4_t color; // rgba, packed * 255 into the glass vertex color
		char __pad0[0x4];
		Material* material;
		Material* materialShattered;
		PhysPreset* physPreset;
		FxEffectDef* pieceBreakEffect;
		FxEffectDef* shatterEffect;
		FxEffectDef* shatterSmallEffect;
		FxEffectDef* crackDecalEffect;
		snd_alias_list_t* damagedSound;
		snd_alias_list_t* destroyedSound;
		snd_alias_list_t* destroyedQuietSound;
		float invHighMipRadius;
		float shatteredInvHighMipRadius;
		int numCrackRings;
		bool isOpaque;
	};
	static_assert(sizeof(FxGlassDef) == 0x88);
	assert_offsetof(FxGlassDef, material, 0x28);
	assert_offsetof(FxGlassDef, damagedSound, 0x60);
	assert_offsetof(FxGlassDef, invHighMipRadius, 0x78);
	assert_offsetof(FxGlassDef, numCrackRings, 0x80);
	assert_offsetof(FxGlassDef, isOpaque, 0x84);

	struct FxSpatialFrame
	{
		vec4_t quat;
		vec3_t origin;
	};

	struct FxGlassPiecePlacement
	{
		FxSpatialFrame frame;
		float radius;
	};

	union FxGlassPiecePlace
	{
		FxGlassPiecePlacement placement;
		unsigned int nextFree;
	};
	static_assert(sizeof(FxGlassPiecePlace) == 0x20);

	struct FxGlassPieceState
	{
		vec2_t texCoordOrigin;
		unsigned int supportMask;
		unsigned short initIndex;
		unsigned short geoDataStart;
		unsigned short lightingIndex;
		unsigned char defIndex;
		unsigned char pad[3];
		unsigned char vertCount;
		unsigned char holeDataCount;
		unsigned char crackDataCount;
		unsigned char fanDataCount;
		unsigned short flags;
		float areaX2;
	};
	static_assert(sizeof(FxGlassPieceState) == 0x20);
	assert_offsetof(FxGlassPieceState, defIndex, 0x12);
	assert_offsetof(FxGlassPieceState, holeDataCount, 0x17);

	struct FxGlassPieceDynamics
	{
		int fallTime;
		std::uint64_t physObjId;
		std::uint64_t physJointId;
		vec3_t vel;
		vec3_t avel;
	};
	static_assert(sizeof(FxGlassPieceDynamics) == 0x30);
	assert_offsetof(FxGlassPieceDynamics, vel, 0x18);

	struct FxGlassVertex
	{
		short x;
		short y;
	};

	struct FxGlassHoleHeader
	{
		unsigned short uniqueVertCount;
		unsigned char touchVert;
		unsigned char pad[1];
	};

	struct FxGlassCrackHeader
	{
		unsigned short uniqueVertCount;
		unsigned char beginVertIndex;
		unsigned char endVertIndex;
	};

	union FxGlassGeometryData
	{
		FxGlassVertex vert;
		FxGlassHoleHeader hole;
		FxGlassCrackHeader crack;
		unsigned char asBytes[4];
		short anonymous[2];
	};
	static_assert(sizeof(FxGlassGeometryData) == 4);

	struct FxGlassInitPieceState
	{
		FxSpatialFrame frame;
		float radius;
		vec2_t texCoordOrigin;
		unsigned int supportMask;
		float areaX2;
		unsigned short lightingIndex;
		unsigned char defIndex;
		unsigned char vertCount;
		unsigned char fanDataCount;
		unsigned char pad[1];
	};
	static_assert(sizeof(FxGlassInitPieceState) == 0x38);
	assert_offsetof(FxGlassInitPieceState, areaX2, 0x2C);
	assert_offsetof(FxGlassInitPieceState, defIndex, 0x32);

	struct FxGlassSystem
	{
		int time;
		int prevTime;
		unsigned int defCount;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int cellCount;
		unsigned int activePieceCount;
		unsigned int firstFreePiece;
		unsigned int geoDataLimit;
		unsigned int geoDataCount;
		unsigned int initGeoDataCount;
		FxGlassDef* defs;
		FxGlassPiecePlace* piecePlaces;
		FxGlassPieceState* pieceStates;
		FxGlassPieceDynamics* pieceDynamics;
		FxGlassGeometryData* geoData;
		unsigned int* isInUse;
		unsigned int* cellBits;
		unsigned char* visData;
		vec3_t* linkOrg;
		float* halfThickness;
		unsigned short* lightingHandles;
		FxGlassGeometryData* initGeoData;
		bool needToCompactData;
		unsigned char initCount;
		float effectChanceAccum;
		int lastPieceDeletionTime;
		unsigned int initPieceCount;
		FxGlassInitPieceState* initPieceStates;
	};
	static_assert(sizeof(FxGlassSystem) == 0xA8);
	assert_offsetof(FxGlassSystem, defs, 0x30);
	assert_offsetof(FxGlassSystem, halfThickness, 0x78);
	assert_offsetof(FxGlassSystem, initCount, 0x91);
	assert_offsetof(FxGlassSystem, initPieceCount, 0x9C);
	assert_offsetof(FxGlassSystem, initPieceStates, 0xA0);

	struct FxWorld
	{
		const char* name;
		FxGlassSystem glassSys;
	};
	static_assert(sizeof(FxWorld) == 0xB0);

	struct ExtentBounds
	{
		vec3_t mins;
		vec3_t maxs;
	};
	assert_sizeof(ExtentBounds, 24);

	union GfxColorHdr
	{
		unsigned short array[4];
		unsigned long long packed;
	};
	assert_sizeof(GfxColorHdr, 8);

	struct GfxSky
	{
		int skySurfCount;
		int* skyStartSurfs;
		GfxImage* skyImage;
		unsigned char skySamplerState;
		Bounds bounds;
	};
	assert_sizeof(GfxSky, 56);
	assert_offsetof(GfxSky, skyStartSurfs, 8);
	assert_offsetof(GfxSky, skyImage, 16);

	struct GfxWorldDpvsPlanes
	{
		unsigned int cellCount;
		cplane_s* planes;
		unsigned int* nodes;
		unsigned int* sceneEntCellBits;
		unsigned int* unk_0x20; // runtime, 256 bits per cell
	};
	assert_sizeof(GfxWorldDpvsPlanes, 40);
	assert_offsetof(GfxWorldDpvsPlanes, planes, 8);
	assert_offsetof(GfxWorldDpvsPlanes, nodes, 16);
	assert_offsetof(GfxWorldDpvsPlanes, sceneEntCellBits, 24);
	assert_offsetof(GfxWorldDpvsPlanes, unk_0x20, 32);

	struct GfxCellTreeCount
	{
		int aabbTreeCount;
	};

	struct GfxAabbTree
	{
		Bounds bounds;
		int childrenOffset;
		unsigned short childCount;
		unsigned short smodelIndexCount;
		unsigned short* smodelIndexes;
		int startSurfIndex;
		unsigned short surfaceCount;
		unsigned short pad;
	};
	assert_sizeof(GfxAabbTree, 48);
	assert_offsetof(GfxAabbTree, smodelIndexCount, 30);
	assert_offsetof(GfxAabbTree, smodelIndexes, 32);

	struct GfxCellTree
	{
		GfxAabbTree* aabbTree;
	};
	assert_sizeof(GfxCellTree, 8);

	struct GfxCellTransientInfo
	{
		unsigned short indexCount;
		unsigned short* indices;
		char unk_0x10[8]; // zero in stock
	};
	assert_sizeof(GfxCellTransientInfo, 24);
	assert_offsetof(GfxCellTransientInfo, indices, 8);

	struct GfxPortal;

	struct GfxPortalWritable
	{
		bool isQueued;
		bool isAncestor;
		unsigned char recursionDepth;
		unsigned char hullPointCount;
		float (*hullPoints)[2];
		GfxPortal* queuedParent;
	};
	assert_sizeof(GfxPortalWritable, 24);

	struct DpvsPlane
	{
		float coeffs[4];
	};

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		float (*vertices)[3];
		unsigned short cellIndex;
		unsigned short closeDistance;
		unsigned char vertexCount;
		float hullAxis[2][3];
	};
	assert_sizeof(GfxPortal, 80);
	assert_offsetof(GfxPortal, vertices, 40);
	assert_offsetof(GfxPortal, vertexCount, 52);

	struct GfxCell
	{
		Bounds bounds;
		unsigned short portalCount;
		unsigned char reflectionProbeCount;
		unsigned char reflectionProbeReferenceCount;
		GfxPortal* portals;
		unsigned char* reflectionProbes;
		unsigned char* reflectionProbeReferences;
	};
	assert_sizeof(GfxCell, 56);
	assert_offsetof(GfxCell, portalCount, 24);
	assert_offsetof(GfxCell, portals, 32);
	assert_offsetof(GfxCell, reflectionProbes, 40);
	assert_offsetof(GfxCell, reflectionProbeReferences, 48);

	struct GfxPortalGroupInfo
	{
		unsigned short cellIndex;
		unsigned short portalIndex;
	};
	assert_sizeof(GfxPortalGroupInfo, 4);

	struct GfxPortalGroup
	{
		const char* targetName;
		GfxPortalGroupInfo* gfxPortalArray;
		scr_string_t targetNameIndex;
		unsigned short numPortals;
	};
	assert_sizeof(GfxPortalGroup, 24);
	assert_offsetof(GfxPortalGroup, gfxPortalArray, 8);
	assert_offsetof(GfxPortalGroup, numPortals, 20);

	struct GfxReflectionProbeVolume
	{
		unsigned short* probeVolumePlanes;
		unsigned int probeVolumePlaneCount;
	};
	assert_sizeof(GfxReflectionProbeVolume, 16);

	struct GfxReflectionProbe
	{
		float origin[3];
		GfxReflectionProbeVolume* probeVolumes;
		unsigned int probeVolumeCount;
		unsigned int lightingState;
	};
	assert_sizeof(GfxReflectionProbe, 32);
	assert_offsetof(GfxReflectionProbe, probeVolumes, 16);
	assert_offsetof(GfxReflectionProbe, probeVolumeCount, 24);

	struct GfxReflectionProbeReferenceOrigin
	{
		float origin[3];
	};
	assert_sizeof(GfxReflectionProbeReferenceOrigin, 12);

	struct GfxReflectionProbeReference
	{
		unsigned char index;
	};
	assert_sizeof(GfxReflectionProbeReference, 1);

	struct GfxLightmapArray
	{
		GfxImage* primary;
		GfxImage* secondary;
	};
	assert_sizeof(GfxLightmapArray, 16);

	struct GfxLightmapParameters
	{
		int lightmapWidthPrimary;
		int lightmapHeightPrimary;
		int lightmapWidthSecondary;
		int lightmapHeightSecondary;
		int lightmapModelUnitsPerTexel;
	};
	assert_sizeof(GfxLightmapParameters, 20);

	assert_sizeof(GfxTexture, 24);

	struct GfxTransientZoneMaterials
	{
		unsigned int materialCount;
		Material** materials;
	};
	assert_sizeof(GfxTransientZoneMaterials, 16);

	struct GfxWorldDraw
	{
		unsigned int reflectionProbeCount;
		GfxImage** reflectionProbes;
		GfxReflectionProbe* reflectionProbeOrigins;
		GfxTexture* reflectionProbeTextures;
		unsigned int reflectionProbeReferenceCount;
		GfxReflectionProbeReferenceOrigin* reflectionProbeReferenceOrigins;
		GfxReflectionProbeReference* reflectionProbeReferences;
		unsigned int fogSplineCount;
		GfxImage** fogSplineImages;
		char (*fogSplineInfo)[116];
		GfxImage* iesLookup;
		unsigned int iesProfileCount;
		unsigned int iesProfiles[16]; // dynamic lights look up their profile id here, empty in stock
		unsigned int lightmapOverrideCount;
		GfxLightmapArray* lightmapOverrides;
		GfxTexture* lightmapPrimaryTextures;
		GfxTexture* lightmapSecondaryTextures;
		GfxImage* lightmapOverridePrimary;
		GfxImage* lightmapOverrideSecondary;
		unsigned int imageCount;
		GfxImage** images;
		GfxLightmapParameters lightmapParameters;
		unsigned int lightmapTiles[31][3]; // x, y, layer of each used tile
		unsigned int lightmapTileCount;
		unsigned int unk_0x264[2]; // 1 or 2 in stock
		unsigned int lightmapTileSize[2]; // same as lightmapParameters in stock
		unsigned int unk_0x274; // 8 in stock
		unsigned int transientZoneCount;
		GfxMapTrZone* transientZones[16];
		GfxTransientZoneMaterials* transientZoneMaterials;
		unsigned int indexCount;
		unsigned short* indices;
		ID3D11Buffer* indexBuffer;
	};
	assert_sizeof(GfxWorldDraw, 800);
	assert_offsetof(GfxWorldDraw, reflectionProbes, 8);
	assert_offsetof(GfxWorldDraw, reflectionProbeOrigins, 16);
	assert_offsetof(GfxWorldDraw, reflectionProbeTextures, 24);
	assert_offsetof(GfxWorldDraw, reflectionProbeReferenceCount, 32);
	assert_offsetof(GfxWorldDraw, reflectionProbeReferenceOrigins, 40);
	assert_offsetof(GfxWorldDraw, reflectionProbeReferences, 48);
	assert_offsetof(GfxWorldDraw, fogSplineCount, 56);
	assert_offsetof(GfxWorldDraw, fogSplineImages, 64);
	assert_offsetof(GfxWorldDraw, fogSplineInfo, 72);
	assert_offsetof(GfxWorldDraw, iesLookup, 80);
	assert_offsetof(GfxWorldDraw, lightmapOverrideCount, 156);
	assert_offsetof(GfxWorldDraw, lightmapOverrides, 160);
	assert_offsetof(GfxWorldDraw, lightmapPrimaryTextures, 168);
	assert_offsetof(GfxWorldDraw, lightmapSecondaryTextures, 176);
	assert_offsetof(GfxWorldDraw, lightmapOverridePrimary, 184);
	assert_offsetof(GfxWorldDraw, lightmapOverrideSecondary, 192);
	assert_offsetof(GfxWorldDraw, imageCount, 200);
	assert_offsetof(GfxWorldDraw, images, 208);
	assert_offsetof(GfxWorldDraw, iesProfileCount, 88);
	assert_offsetof(GfxWorldDraw, lightmapParameters, 216);
	assert_offsetof(GfxWorldDraw, lightmapTiles, 236);
	assert_offsetof(GfxWorldDraw, lightmapTileCount, 608);
	assert_offsetof(GfxWorldDraw, lightmapTileSize, 620);
	assert_offsetof(GfxWorldDraw, transientZoneCount, 632);
	assert_offsetof(GfxWorldDraw, transientZones, 640);
	assert_offsetof(GfxWorldDraw, transientZoneMaterials, 768);
	assert_offsetof(GfxWorldDraw, indexCount, 776);
	assert_offsetof(GfxWorldDraw, indices, 784);
	assert_offsetof(GfxWorldDraw, indexBuffer, 792);

	struct GfxLightGridEntry
	{
		unsigned int colorsIndex;
		unsigned short primaryLightEnvIndex;
		unsigned char unused;
		unsigned char needsTrace;
	};
	assert_sizeof(GfxLightGridEntry, 8);

	struct GfxLightGridColors
	{
		unsigned short rgb[56][3];
	};
	assert_sizeof(GfxLightGridColors, 336);

	struct GfxLightGridTree
	{
		unsigned char lightingState;
		unsigned char maxDepth;
		unsigned char pad[2];
		int nodeCount;
		int leafCount;
		int coordMinGridSpace[3];
		int coordMaxGridSpace[3];
		int coordHalfSizeGridSpace[3];
		int defaultColorIndexBitCount;
		int defaultLightIndexBitCount;
		unsigned int* p_nodeTable;
		int leafTableSize;
		unsigned char* p_leafTable;
	};
	assert_sizeof(GfxLightGridTree, 80);
	assert_offsetof(GfxLightGridTree, nodeCount, 4);
	assert_offsetof(GfxLightGridTree, p_nodeTable, 56);
	assert_offsetof(GfxLightGridTree, leafTableSize, 64);
	assert_offsetof(GfxLightGridTree, p_leafTable, 72);

	struct GfxLightGrid
	{
		bool hasLightRegions;
		bool useSkyForLowZ;
		unsigned int lastSunPrimaryLightIndex;
		unsigned short mins[3];
		unsigned short maxs[3];
		unsigned int rowAxis;
		unsigned int colAxis;
		unsigned short* rowDataStart;
		unsigned int rawRowDataSize;
		unsigned char* rawRowData;
		unsigned int entryCount;
		GfxLightGridEntry* entries;
		unsigned int colorCount;
		GfxLightGridColors* colors;
		GfxLightGridEntry cachedSkyLightGridEntry;
		float cachedSkyLightEntryAverageColor[3];
		unsigned int missingGridColorIndex;
		int tableVersion;
		int paletteVersion;
		char rangeExponent8BitsEncoding;
		char rangeExponent12BitsEncoding;
		char rangeExponent16BitsEncoding;
		unsigned char stageCount;
		float* stageLightingContrastGain;
		unsigned int paletteEntryCount;
		int* paletteEntryAddress;
		unsigned int paletteBitstreamSize;
		unsigned char* paletteBitstream;
		GfxLightGridColors skyLightGridColors;
		GfxLightGridColors defaultLightGridColors;
		GfxLightGridTree tree[3];
	};
	assert_sizeof(GfxLightGrid, 1080);
	assert_offsetof(GfxLightGrid, mins, 8);
	assert_offsetof(GfxLightGrid, maxs, 14);
	assert_offsetof(GfxLightGrid, rowAxis, 20);
	assert_offsetof(GfxLightGrid, rowDataStart, 32);
	assert_offsetof(GfxLightGrid, rawRowDataSize, 40);
	assert_offsetof(GfxLightGrid, rawRowData, 48);
	assert_offsetof(GfxLightGrid, entryCount, 56);
	assert_offsetof(GfxLightGrid, entries, 64);
	assert_offsetof(GfxLightGrid, colorCount, 72);
	assert_offsetof(GfxLightGrid, colors, 80);
	assert_offsetof(GfxLightGrid, stageCount, 123);
	assert_offsetof(GfxLightGrid, stageLightingContrastGain, 128);
	assert_offsetof(GfxLightGrid, paletteEntryCount, 136);
	assert_offsetof(GfxLightGrid, paletteEntryAddress, 144);
	assert_offsetof(GfxLightGrid, paletteBitstreamSize, 152);
	assert_offsetof(GfxLightGrid, paletteBitstream, 160);
	assert_offsetof(GfxLightGrid, tree, 840);

	struct GfxSHProbeData
	{
		unsigned short coeffs[29];
		unsigned short pad[3];
	};
	assert_sizeof(GfxSHProbeData, 64);

	struct GfxGpuLightGridZone
	{
		unsigned int numProbes;
		unsigned int firstProbe;
		unsigned int numTetrahedrons;
		unsigned int firstTetrahedron;
		unsigned int firstVoxelTetrahedronIndex;
		unsigned int voxelTetrahedronInternalNodeShift;
		unsigned int numVoxelTetrahedronIndices;
		GfxSHProbeData fallbackProbeData;
	};
	assert_sizeof(GfxGpuLightGridZone, 92);

	struct GfxGpuLightGridTetrahedron
	{
		unsigned int indexFlags[4];
	};
	assert_sizeof(GfxGpuLightGridTetrahedron, 16);

	struct GfxGpuLightGridTetrahedronNeighbors
	{
		unsigned int neighbors[4];
	};
	assert_sizeof(GfxGpuLightGridTetrahedronNeighbors, 16);

	struct GfxGpuLightGridTetrahedronVisibility
	{
		unsigned int visibility[16];
	};
	assert_sizeof(GfxGpuLightGridTetrahedronVisibility, 64);

	struct GfxGpuLightGridVoxelStartTetrahedron
	{
		unsigned int index;
	};
	assert_sizeof(GfxGpuLightGridVoxelStartTetrahedron, 4);

	struct GfxGpuLightGridProbePosition
	{
		float origin[3];
	};
	assert_sizeof(GfxGpuLightGridProbePosition, 12);

	struct GfxStaticModelProbeGrid
	{
		unsigned int firstProbe;
		unsigned short probeCountX;
		unsigned short probeCountY;
		unsigned int probeCountZ;
		unsigned int averageProbe; // firstProbe for one probe, else the slot after the grid
		float worldToGrid[3][4]; // world to unit cube over the model bounds
	};
	assert_sizeof(GfxStaticModelProbeGrid, 64);

	struct GfxLightGridProbeData
	{
		unsigned int probeCount;
		GfxSHProbeData* probes;
		ID3D11Buffer* probesBuffer;
		ID3D11ShaderResourceView* probesView;
		ID3D11UnorderedAccessView* probesRWView;
		GfxGpuLightGridProbePosition* probePositions;
		ID3D11Buffer* probePositionsBuffer;
		ID3D11ShaderResourceView* probePositionsView;
		unsigned int zoneCount;
		GfxGpuLightGridZone* zones;
		unsigned int tetrahedronCount;
		unsigned int tetrahedronCountVisible;
		GfxGpuLightGridTetrahedron* tetrahedrons;
		ID3D11Buffer* tetrahedronBuffer;
		ID3D11ShaderResourceView* tetrahedronView;
		GfxGpuLightGridTetrahedronNeighbors* tetrahedronNeighbors;
		ID3D11Buffer* tetrahedronNeighborsBuffer;
		ID3D11ShaderResourceView* tetrahedronNeighborsView;
		GfxGpuLightGridTetrahedronVisibility* tetrahedronVisibility;
		ID3D11Buffer* tetrahedronVisibilityBuffer;
		ID3D11ShaderResourceView* tetrahedronVisibilityView;
		unsigned int voxelStartTetrahedronCount;
		GfxGpuLightGridVoxelStartTetrahedron* voxelStartTetrahedron;
		ID3D11Buffer* voxelStartTetrahedronBuffer;
		ID3D11ShaderResourceView* voxelStartTetrahedronView;
		unsigned int smodelProbeGridCount;
		GfxStaticModelProbeGrid* smodelProbeGrids;
		unsigned int smodelProbeCount;
		GfxSHProbeData* smodelProbes;
		ID3D11Buffer* smodelProbesBuffer;
		ID3D11ShaderResourceView* smodelProbesView;
		ID3D11UnorderedAccessView* smodelProbesRWView;
	};
	assert_sizeof(GfxLightGridProbeData, 248);
	assert_offsetof(GfxLightGridProbeData, probes, 8);
	assert_offsetof(GfxLightGridProbeData, probePositions, 40);
	assert_offsetof(GfxLightGridProbeData, zoneCount, 64);
	assert_offsetof(GfxLightGridProbeData, zones, 72);
	assert_offsetof(GfxLightGridProbeData, tetrahedronCount, 80);
	assert_offsetof(GfxLightGridProbeData, tetrahedronCountVisible, 84);
	assert_offsetof(GfxLightGridProbeData, tetrahedrons, 88);
	assert_offsetof(GfxLightGridProbeData, tetrahedronNeighbors, 112);
	assert_offsetof(GfxLightGridProbeData, tetrahedronVisibility, 136);
	assert_offsetof(GfxLightGridProbeData, voxelStartTetrahedronCount, 160);
	assert_offsetof(GfxLightGridProbeData, voxelStartTetrahedron, 168);
	assert_offsetof(GfxLightGridProbeData, smodelProbeGridCount, 192);
	assert_offsetof(GfxLightGridProbeData, smodelProbeGrids, 200);
	assert_offsetof(GfxLightGridProbeData, smodelProbeCount, 208);
	assert_offsetof(GfxLightGridProbeData, smodelProbes, 216);
	assert_offsetof(GfxLightGridProbeData, smodelProbesRWView, 240);

	template <std::size_t Stride>
	struct GfxWorldUnknownArray
	{
		unsigned int count;
		char (*data)[Stride];
	};
	assert_sizeof(GfxWorldUnknownArray<32>, 16);

	struct GfxWorldUnknownBufferGroup
	{
		unsigned int unk_0x0;
		unsigned int count;
		char (*data)[64];
		ID3D11Buffer* buffer;
		ID3D11ShaderResourceView* view;
		unsigned int countA;
		unsigned int countB;
		char (*dataA)[64];
		ID3D11Buffer* bufferA;
		ID3D11ShaderResourceView* viewA;
		unsigned int* dataB;
		ID3D11Buffer* bufferB;
		ID3D11ShaderResourceView* viewB;
	};
	assert_sizeof(GfxWorldUnknownBufferGroup, 88);
	assert_offsetof(GfxWorldUnknownBufferGroup, count, 4);
	assert_offsetof(GfxWorldUnknownBufferGroup, data, 8);
	assert_offsetof(GfxWorldUnknownBufferGroup, countA, 32);
	assert_offsetof(GfxWorldUnknownBufferGroup, countB, 36);
	assert_offsetof(GfxWorldUnknownBufferGroup, dataA, 40);
	assert_offsetof(GfxWorldUnknownBufferGroup, dataB, 64);

	struct GfxWorldUnknownData
	{
		GfxWorldUnknownArray<32> array0;
		GfxWorldUnknownArray<28> array1;
		GfxWorldUnknownArray<16> array2;
		GfxWorldUnknownArray<20> array3;
		GfxWorldUnknownArray<16> array4;
		GfxWorldUnknownArray<16> array5;
		GfxWorldUnknownArray<16> array6;
		GfxWorldUnknownArray<16> array7;
		GfxWorldUnknownArray<16> array8;
		GfxWorldUnknownArray<16> array9;
		char unk_0xA0[8];
		GfxImage* image0;
		unsigned int image0DataCount;
		unsigned int* image0Data;
		char unk_0xC0[8];
		GfxImage* image1;
		unsigned int image1DataCount;
		unsigned short* image1Data;
		char unk_0xE0[8];
		GfxWorldUnknownBufferGroup groups[3];
	};
	assert_sizeof(GfxWorldUnknownData, 496);
	assert_offsetof(GfxWorldUnknownData, image0, 168);
	assert_offsetof(GfxWorldUnknownData, image0DataCount, 176);
	assert_offsetof(GfxWorldUnknownData, image0Data, 184);
	assert_offsetof(GfxWorldUnknownData, image1, 200);
	assert_offsetof(GfxWorldUnknownData, image1DataCount, 208);
	assert_offsetof(GfxWorldUnknownData, image1Data, 216);
	assert_offsetof(GfxWorldUnknownData, groups, 232);

	struct GfxFrustumLightVertex
	{
		vec3_t xyz;
		char __pad0[16];
		unsigned int normal;
	};
	assert_sizeof(GfxFrustumLightVertex, 32);

	struct GfxFrustumLights
	{
		Bounds bounds;
		unsigned int vertexCount;
		unsigned int indexCount;
		unsigned short* indices;
		char (*vertices)[32];
		ID3D11Buffer* indexBuffer;
		ID3D11Buffer* vertexBuffer;
	};
	assert_sizeof(GfxFrustumLights, 64);
	assert_offsetof(GfxFrustumLights, vertexCount, 24);
	assert_offsetof(GfxFrustumLights, indexCount, 28);
	assert_offsetof(GfxFrustumLights, indices, 32);
	assert_offsetof(GfxFrustumLights, vertices, 40);

	struct GfxVoxelTreeHeader
	{
		int rootNodeDimension[4];
		int nodeCoordBitShift[4];
		vec4_t boundMin;
		vec4_t boundMax;
	};
	assert_sizeof(GfxVoxelTreeHeader, 64);

	struct GfxVoxelTopDownViewNode
	{
		int firstNodeIndex;
		int zMin;
		int zMax;
	};
	assert_sizeof(GfxVoxelTopDownViewNode, 12);

	struct GfxVoxelInternalNode
	{
		int firstNodeIndex[2]; // one base per mask half, bit 31 set when the children are leaves
		unsigned int childNodeMask[2];
	};
	assert_sizeof(GfxVoxelInternalNode, 16);

	struct GfxVoxelLeafNode
	{
		unsigned short lightListAddress;
	};
	assert_sizeof(GfxVoxelLeafNode, 2);

	struct GfxVoxelTree
	{
		Bounds zoneBound;
		unsigned int voxelTopDownViewNodeCount;
		unsigned int voxelInternalNodeCount;
		unsigned int voxelLeafNodeCount;
		unsigned int lightListArraySize;
		GfxVoxelTreeHeader* voxelTreeHeader;
		GfxVoxelTopDownViewNode* voxelTopDownViewNodeArray;
		GfxVoxelInternalNode* voxelInternalNodeArray;
		GfxVoxelLeafNode* voxelLeafNodeArray;
		unsigned short* lightListArray;
		unsigned int (*voxelInternalNodeDynamicLightList)[2];
		char unk_0x58[24];
	};
	assert_sizeof(GfxVoxelTree, 112);
	assert_offsetof(GfxVoxelTree, voxelTopDownViewNodeCount, 24);
	assert_offsetof(GfxVoxelTree, voxelInternalNodeCount, 28);
	assert_offsetof(GfxVoxelTree, voxelLeafNodeCount, 32);
	assert_offsetof(GfxVoxelTree, lightListArraySize, 36);
	assert_offsetof(GfxVoxelTree, voxelTreeHeader, 40);
	assert_offsetof(GfxVoxelTree, voxelTopDownViewNodeArray, 48);
	assert_offsetof(GfxVoxelTree, voxelInternalNodeArray, 56);
	assert_offsetof(GfxVoxelTree, voxelLeafNodeArray, 64);
	assert_offsetof(GfxVoxelTree, lightListArray, 72);
	assert_offsetof(GfxVoxelTree, voxelInternalNodeDynamicLightList, 80);

	struct GfxWorldLightLists
	{
		unsigned int surfaceListOffsetCount;
		unsigned int* surfaceListOffsets;
		unsigned int smodelListOffsetCount;
		unsigned int* smodelListOffsets;
		unsigned int listsSize;
		unsigned short* lists;
	};
	assert_sizeof(GfxWorldLightLists, 48);
	assert_offsetof(GfxWorldLightLists, surfaceListOffsets, 8);
	assert_offsetof(GfxWorldLightLists, smodelListOffsets, 24);
	assert_offsetof(GfxWorldLightLists, listsSize, 32);
	assert_offsetof(GfxWorldLightLists, lists, 40);

	struct GfxBrushModelWritable
	{
		Bounds bounds;
		vec3_t origin;
		vec4_t quat;
		int mdaoVolumeProcessed;
	};

	struct GfxBrushModel
	{
		GfxBrushModelWritable writable;
		Bounds bounds;
		float radius;
		unsigned int startSurfIndex;
		unsigned short surfaceCount;
		int mdaoVolumeIndex;
	};
	assert_sizeof(GfxBrushModel, 96);

	struct MaterialMemory
	{
		Material* material;
		int memory;
	};
	assert_sizeof(MaterialMemory, 16);

	struct sunflare_t
	{
		bool hasValidData;
		Material* spriteMaterial;
		Material* flareMaterial;
		float spriteSize;
		float flareMinSize;
		float flareMinDot;
		float flareMaxSize;
		float flareMaxDot;
		float flareMaxAlpha;
		int flareFadeInTime;
		int flareFadeOutTime;
		float blindMinDot;
		float blindMaxDot;
		float blindMaxDarken;
		int blindFadeInTime;
		int blindFadeOutTime;
		float glareMinDot;
		float glareMaxDot;
		float glareMaxLighten;
		int glareFadeInTime;
		int glareFadeOutTime;
		float sunFxPosition[3];
	};
	assert_sizeof(sunflare_t, 112);
	assert_offsetof(sunflare_t, spriteMaterial, 8);
	assert_offsetof(sunflare_t, flareMaterial, 16);

	struct GfxShadowGeometry
	{
		unsigned short surfaceCount;
		unsigned short smodelCount;
		unsigned int* sortedSurfIndex;
		unsigned short* smodelIndex;
	};
	assert_sizeof(GfxShadowGeometry, 24);
	assert_offsetof(GfxShadowGeometry, sortedSurfIndex, 8);
	assert_offsetof(GfxShadowGeometry, smodelIndex, 16);

	struct GfxLightRegionAxis
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};
	assert_sizeof(GfxLightRegionAxis, 20);

	struct GfxLightRegionHull
	{
		float kdopMidPoint[9];
		float kdopHalfSize[9];
		unsigned int axisCount;
		GfxLightRegionAxis* axis;
	};
	assert_sizeof(GfxLightRegionHull, 88);
	assert_offsetof(GfxLightRegionHull, axisCount, 72);
	assert_offsetof(GfxLightRegionHull, axis, 80);

	struct GfxLightRegion
	{
		unsigned int hullCount;
		GfxLightRegionHull* hulls;
	};
	assert_sizeof(GfxLightRegion, 16);

	struct GfxStaticModelInst
	{
		Bounds bounds;
		float lightingOrigin[3];
	};
	assert_sizeof(GfxStaticModelInst, 36);

	enum StaticModelFlag : std::uint16_t
	{
		STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING = 0x40,
		STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING = 0x80,
		STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING = 0x100,
	};

	struct GfxPackedPlacement
	{
		float origin[3];
		float axis[3][3];
		float scale;
	};
	assert_sizeof(GfxPackedPlacement, 52);

	struct GfxStaticModelDrawInst
	{
		GfxPackedPlacement placement; // placement.scale is the x axis scale
		float axisScale[2]; // y and z, mirrored instances have a negative y
		float maxScale;
		unsigned char mirrored;
		char __pad0[7];
		XModel* model;
		unsigned short unk_0x50; // zero in stock
		unsigned short cullDist; // 0 is no distance cull
		unsigned short flags;
		unsigned short unk_0x56; // zero in stock
		unsigned short staticModelId;
		unsigned short unk_0x5A; // zero in stock
		unsigned int unk_0x5C; // never read by the game
		int lightGridSlot; // runtime, set at load from smodelLightingInsts for flag 0x40, else -1
		unsigned int unk_0x64; // runtime, cleared at load
		unsigned short unk_0x68; // zero in stock
		unsigned short lightListOffset;
		float radius;
		unsigned char reflectionProbeIndex;
		unsigned char sortMaterialIndex;
		unsigned char sunShadowFlags; // bit per sunShadowOptCount partition
		unsigned char minLod;
		float lodScale; // divides the lod and cull distance
		char __pad1[8]; // zero in stock
	};
	assert_sizeof(GfxStaticModelDrawInst, 128);
	assert_offsetof(GfxStaticModelDrawInst, axisScale, 52);
	assert_offsetof(GfxStaticModelDrawInst, maxScale, 60);
	assert_offsetof(GfxStaticModelDrawInst, mirrored, 64);
	assert_offsetof(GfxStaticModelDrawInst, model, 72);
	assert_offsetof(GfxStaticModelDrawInst, cullDist, 82);
	assert_offsetof(GfxStaticModelDrawInst, flags, 84);
	assert_offsetof(GfxStaticModelDrawInst, staticModelId, 88);
	assert_offsetof(GfxStaticModelDrawInst, unk_0x5C, 92);
	assert_offsetof(GfxStaticModelDrawInst, lightGridSlot, 96);
	assert_offsetof(GfxStaticModelDrawInst, lightListOffset, 106);
	assert_offsetof(GfxStaticModelDrawInst, radius, 108);
	assert_offsetof(GfxStaticModelDrawInst, reflectionProbeIndex, 112);
	assert_offsetof(GfxStaticModelDrawInst, sortMaterialIndex, 113);
	assert_offsetof(GfxStaticModelDrawInst, sunShadowFlags, 114);
	assert_offsetof(GfxStaticModelDrawInst, minLod, 115);
	assert_offsetof(GfxStaticModelDrawInst, lodScale, 116);

	struct GfxStaticModelVertexLighting
	{
		unsigned char visibility[4];
		unsigned short ambientColorFloat16[4];
		unsigned short highlightColorFloat16[4];
	};
	assert_sizeof(GfxStaticModelVertexLighting, 20);

	struct GfxStaticModelVertexLightingInfo
	{
		GfxStaticModelVertexLighting* lightingValues;
		ID3D11Buffer* lightingValuesVb;
		unsigned int numLightingValues;
	};
	assert_offsetof(GfxStaticModelVertexLightingInfo, numLightingValues, 16);

	struct GfxStaticModelLightmapInfo
	{
		float offset[2];
		float scale[2];
		unsigned int lightmapIndex;
	};

	struct GfxStaticModelAmbientLightingInfo
	{
		GfxColorHdr groundLighting;
		unsigned int colorIndex;
		float primaryLightWeight;
	};

	struct GfxStaticModelLightGridInfo
	{
		unsigned int slot;
		char unk_0x4[20];
	};

	union GfxStaticModelLighting
	{
		GfxStaticModelLightGridInfo lightGridInfo;
		GfxStaticModelAmbientLightingInfo ambientLightingInfo;
		GfxStaticModelVertexLightingInfo vertexLightingInfo;
		GfxStaticModelLightmapInfo modelLightmapInfo;
	};
	assert_sizeof(GfxStaticModelLighting, 24);

	struct GfxSurfaceBuffer
	{
		unsigned short vertexCount; // streams 7 and 8 are 8 bytes per vertex in every stock buffer
		unsigned short indexCount;
		unsigned short unk_0x4; // count of 16 byte records in stream 2
		unsigned short triCount;
		unsigned int unk_0x8; // size of stream 1 on most stock buffers
		unsigned int flags; // 1 in stock, bit 0x4 is tested at load
		unsigned int dataSize;
		unsigned short unk_0x14; // the game subtracts 0x4EC0 from it at load
		char __pad0[2];
		char hash[32]; // lowercase hex
		char __pad1[8];
		char* data;
		unsigned long long streamOffsets[11]; // into data, stream 0 = 16 bit indices at offset 0
		char __pad2[72];
		ExtentBounds bounds;
	};
	assert_sizeof(GfxSurfaceBuffer, 0x100);
	assert_offsetof(GfxSurfaceBuffer, dataSize, 16);
	assert_offsetof(GfxSurfaceBuffer, hash, 24);
	assert_offsetof(GfxSurfaceBuffer, data, 64);
	assert_offsetof(GfxSurfaceBuffer, streamOffsets, 72);
	assert_offsetof(GfxSurfaceBuffer, bounds, 232);

	enum GfxSurfaceFlag : std::uint16_t
	{
		GFX_SURFACE_FLAG_CASTS_SHADOW = 0x1, // with a shadow material it sets surfaceCastsSunShadow at load
		GFX_SURFACE_FLAG_SUN_SHADOW_PARTITION_0 = 0x10, // bits 4-8, one per sunShadowOptCount partition (surfaceCastsSunShadowOpt)
		GFX_SURFACE_FLAG_SUN_SHADOW_PARTITION_MASK = 0x1F0,
	};

	struct GfxSurface
	{
		unsigned int vertexLayerData;
		unsigned int firstVertex;
		unsigned short vertexCount;
		unsigned short triCount;
		char __pad0[4];
		GfxSurfaceBuffer* buffer;
		unsigned int baseIndex;
		char __pad1[4];
		Material* material;
		unsigned char lightmapIndex;
		unsigned char reflectionProbeIndex;
		unsigned short flags; // read as a word at load
		unsigned short lightListOffset;
		char __pad2[2];
		unsigned int transientZone;
		char __pad3[4];
	};
	assert_sizeof(GfxSurface, 56);
	assert_offsetof(GfxSurface, vertexCount, 8);
	assert_offsetof(GfxSurface, buffer, 16);
	assert_offsetof(GfxSurface, baseIndex, 24);
	assert_offsetof(GfxSurface, material, 32);
	assert_offsetof(GfxSurface, lightmapIndex, 40);
	assert_offsetof(GfxSurface, flags, 42);
	assert_offsetof(GfxSurface, lightListOffset, 44);
	assert_offsetof(GfxSurface, transientZone, 48);

	struct GfxSurfaceBounds
	{
		Bounds bounds;
		float coneApex[3]; // backface normal cone, sin-like cutoff around a unit axis
		float coneCutoff; // -FLT_MAX with a zero apex and axis = no cone
		float coneAxis[3];
		unsigned short mipRadius; // same values as H1 GfxSurfaceBounds
		unsigned char invHighMipRadius[5]; // one per material image, read by texture streaming
		char __pad0[5];
	};
	assert_sizeof(GfxSurfaceBounds, 64);
	assert_offsetof(GfxSurfaceBounds, coneApex, 24);
	assert_offsetof(GfxSurfaceBounds, coneCutoff, 36);
	assert_offsetof(GfxSurfaceBounds, coneAxis, 40);
	assert_offsetof(GfxSurfaceBounds, mipRadius, 52);
	assert_offsetof(GfxSurfaceBounds, invHighMipRadius, 54);

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount;
		unsigned int staticSurfaceCount;
		unsigned int surfaceRangeBegins[8];
		unsigned int surfaceRangeEnds[8];
		unsigned int smodelVisDataCount;
		unsigned int surfaceVisDataCount;
		unsigned int primaryLightVisDataCount;
		char __pad0[4];
		unsigned int* smodelVisData[4]; // the smodel and surface vis arrays are bitsets cleared each frame
		unsigned int* smodelVisDataUnused[8];
		unsigned int* smodelUnknownVisData[27];
		unsigned int* surfaceVisData[4];
		unsigned int* surfaceVisDataUnused[8];
		unsigned int* surfaceUnknownVisData[27];
		unsigned int* unk_0x2C8; // runtime, 2 bits per surface, cleared each frame
		unsigned int* primaryLightVisData;
		unsigned int* unk_0x2D8; // runtime, 1 bit per static model, cleared each frame
		unsigned int* unk_0x2E0; // runtime, 1 bit per surface, set to all ones each frame
		unsigned int* sortedSurfIndex;
		GfxSurface* surfaces;
		GfxSurfaceBounds* surfacesBounds;
		unsigned long long* surfaceSortKeys; // runtime, packed from the material at load, low word = texture streaming index
		GfxStaticModelInst* smodelInsts;
		GfxStaticModelDrawInst* smodelDrawInsts;
		unsigned int* unk_0x318; // zero in stock, like H1 unknownSModelVisData2
		GfxStaticModelLighting* smodelLightingInsts;
		unsigned int* surfaceCastsSunShadow; // runtime, built at load from GFX_SURFACE_FLAG_CASTS_SHADOW
		unsigned short* sortedSmodelIndices;
		unsigned int sunShadowOptCount;
		unsigned int sunSurfVisDataCount;
		unsigned int* surfaceCastsSunShadowOpt;
		unsigned long long* surfaceMaterials;
		ID3D11Buffer** smodelConstantBuffers; // runtime, per model constant buffer of the cached draw path
		char __pad1[8];
	};
	assert_sizeof(GfxWorldDpvsStatic, 864);
	assert_offsetof(GfxWorldDpvsStatic, surfaceRangeBegins, 8);
	assert_offsetof(GfxWorldDpvsStatic, surfaceRangeEnds, 40);
	assert_offsetof(GfxWorldDpvsStatic, smodelVisDataCount, 72);
	assert_offsetof(GfxWorldDpvsStatic, surfaceVisDataCount, 76);
	assert_offsetof(GfxWorldDpvsStatic, primaryLightVisDataCount, 80);
	assert_offsetof(GfxWorldDpvsStatic, smodelVisData, 88);
	assert_offsetof(GfxWorldDpvsStatic, smodelVisDataUnused, 120);
	assert_offsetof(GfxWorldDpvsStatic, smodelUnknownVisData, 184);
	assert_offsetof(GfxWorldDpvsStatic, surfaceVisData, 400);
	assert_offsetof(GfxWorldDpvsStatic, surfaceVisDataUnused, 432);
	assert_offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData, 496);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x2C8, 712);
	assert_offsetof(GfxWorldDpvsStatic, primaryLightVisData, 720);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x2D8, 728);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x2E0, 736);
	assert_offsetof(GfxWorldDpvsStatic, sortedSurfIndex, 744);
	assert_offsetof(GfxWorldDpvsStatic, surfaces, 752);
	assert_offsetof(GfxWorldDpvsStatic, surfacesBounds, 760);
	assert_offsetof(GfxWorldDpvsStatic, surfaceSortKeys, 768);
	assert_offsetof(GfxWorldDpvsStatic, smodelInsts, 776);
	assert_offsetof(GfxWorldDpvsStatic, smodelDrawInsts, 784);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x318, 792);
	assert_offsetof(GfxWorldDpvsStatic, smodelLightingInsts, 800);
	assert_offsetof(GfxWorldDpvsStatic, surfaceCastsSunShadow, 808);
	assert_offsetof(GfxWorldDpvsStatic, sortedSmodelIndices, 816);
	assert_offsetof(GfxWorldDpvsStatic, sunShadowOptCount, 824);
	assert_offsetof(GfxWorldDpvsStatic, sunSurfVisDataCount, 828);
	assert_offsetof(GfxWorldDpvsStatic, surfaceCastsSunShadowOpt, 832);
	assert_offsetof(GfxWorldDpvsStatic, surfaceMaterials, 840);
	assert_offsetof(GfxWorldDpvsStatic, smodelConstantBuffers, 848);

	struct GfxWorldDpvsDynamic
	{
		unsigned int dynEntClientWordCount[2];
		unsigned int dynEntClientCount[2];
		unsigned int* dynEntCellBits[2];
		unsigned char* dynEntVisData[2][4];
	};
	assert_sizeof(GfxWorldDpvsDynamic, 96);
	assert_offsetof(GfxWorldDpvsDynamic, dynEntCellBits, 16);
	assert_offsetof(GfxWorldDpvsDynamic, dynEntVisData, 32);

	struct GfxHeroOnlyLight
	{
		unsigned char type;
		unsigned char needsDynamicShadows;
		unsigned char unused[2];
		float color[3];
		float dir[3];
		float up[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		int exponent;
		int unk_0x44;
	};
	assert_sizeof(GfxHeroOnlyLight, 72);

	struct MdaoVolume
	{
		ExtentBounds bounds;
		unsigned short cellCount[3];
		unsigned short parentBoneIndex;
		GfxImage* volumeData;
	};
	assert_sizeof(MdaoVolume, 40);
	assert_offsetof(MdaoVolume, volumeData, 32);

	struct GfxBuildInfo
	{
		const char* bspCommandline;
		const char* lightCommandline;
		const char* bspTimestamp;
		const char* lightTimestamp;
	};
	assert_sizeof(GfxBuildInfo, 32);

	// the same in every stock map, named after iw7
	struct GfxWorldSortKeys
	{
		unsigned char unk_0x0;
		unsigned char unk_0x1;
		unsigned char unk_0x2;
		unsigned char twoD;
		unsigned char unk_0x4;
		unsigned char effectDecal; // a key outside the emissive range that the game also accepts
		unsigned char unk_0x6;
		unsigned char effectAuto;
		unsigned char distortion;
		unsigned char transBegin;
		unsigned char transEnd;
		unsigned char emissiveBegin;
		unsigned char emissiveEnd;
		unsigned char postResolveLit; // the game labels it "post resolve lit"
		char __pad0[6];
	};
	static_assert(sizeof(GfxWorldSortKeys) == 20);
	static_assert(offsetof(GfxWorldSortKeys, distortion) == 8);

	struct GfxWorld
	{
		const char* name;
		const char* baseName;
		unsigned int bspVersion;
		int planeCount;
		int nodeCount;
		unsigned int surfaceCount;
		int skyCount;
		GfxSky* skies;
		unsigned int portalGroupCount;
		unsigned int lastSunPrimaryLightIndex;
		unsigned int primaryLightCount;
		unsigned int movingScriptablePrimaryLightCount; // iw7 name, the last lights of the list
		unsigned int unk_0x40; // equals primaryLightCount in stock
		GfxWorldSortKeys sortKeys;
		GfxWorldDpvsPlanes dpvsPlanes;
		GfxCellTreeCount* aabbTreeCounts;
		GfxCellTree* aabbTrees;
		GfxCellTransientInfo* cellTransientInfos;
		GfxCell* cells;
		GfxPortalGroup* portalGroup;
		unsigned int portalDistanceAnchorCount;
		vec4_t* portalDistanceAnchorsAndCloseDistSquared;
		bool cullPrimaryLights; // 1 in stock, 0 marks every primary light visible each frame
		char __pad0[7];
		GfxWorldDraw draw;
		GfxLightGrid lightGrid;
		GfxLightGridProbeData probeData;
		GfxWorldUnknownData unknownData;
		GfxFrustumLights* frustumLights;
		int voxelTreeCount;
		GfxVoxelTree* voxelTree;
		GfxWorldLightLists lightLists;
		int modelCount;
		GfxBrushModel* models;
		Bounds bounds;
		Bounds shadowBounds;
		unsigned int checksum;
		int materialMemoryCount;
		MaterialMemory* materialMemory;
		sunflare_t sun;
		float outdoorLookupMatrix[4][4];
		GfxImage* outdoorImage;
		unsigned int* cellCasterBits;
		unsigned int* cellHasSunLitSurfsBits;
		char (*sceneDynModel)[12];
		char (*sceneDynBrush)[8];
		unsigned int* primaryLightEntityShadowVis;
		unsigned int* primaryLightDynEntShadowVis[2];
		unsigned short* nonSunPrimaryLightForModelDynEnt;
		GfxShadowGeometry* shadowGeom;
		GfxShadowGeometry* shadowGeomOptimized;
		GfxLightRegion* lightRegion;
		GfxWorldDpvsStatic dpvs;
		GfxWorldDpvsDynamic dpvsDyn;
		unsigned int mapVtxChecksum;
		unsigned int heroOnlyLightCount;
		GfxHeroOnlyLight* heroOnlyLights;
		unsigned char fogTypesAllowed;
		unsigned int mdaoVolumesCount;
		MdaoVolume* mdaoVolumes;
		bool unk_0x1088;
		bool unk_0x1089; // 0 or 1 in stock, h1 has the same byte before its lookup flags
		bool useLightGridDefaultModelLightingLookup; // 0 in stock
		bool useLightGridDefaultFXLightingLookup; // 1 in stock, with a point in the playable area
		float lightGridDefaultModelLightingLookup[3];
		float lightGridDefaultFXLightingLookup[3];
		GfxBuildInfo buildInfo;
	};
	assert_sizeof(GfxWorld, 0x10C8);
	assert_offsetof(GfxWorld, planeCount, 20);
	assert_offsetof(GfxWorld, nodeCount, 24);
	assert_offsetof(GfxWorld, surfaceCount, 28);
	assert_offsetof(GfxWorld, skyCount, 32);
	assert_offsetof(GfxWorld, skies, 40);
	assert_offsetof(GfxWorld, portalGroupCount, 48);
	assert_offsetof(GfxWorld, lastSunPrimaryLightIndex, 52);
	assert_offsetof(GfxWorld, primaryLightCount, 56);
	assert_offsetof(GfxWorld, movingScriptablePrimaryLightCount, 60);
	assert_offsetof(GfxWorld, unk_0x40, 64);
	assert_offsetof(GfxWorld, sortKeys, 68);
	assert_offsetof(GfxWorld, dpvsPlanes, 88);
	assert_offsetof(GfxWorld, aabbTreeCounts, 128);
	assert_offsetof(GfxWorld, aabbTrees, 136);
	assert_offsetof(GfxWorld, cellTransientInfos, 144);
	assert_offsetof(GfxWorld, cells, 152);
	assert_offsetof(GfxWorld, portalGroup, 160);
	assert_offsetof(GfxWorld, portalDistanceAnchorCount, 168);
	assert_offsetof(GfxWorld, portalDistanceAnchorsAndCloseDistSquared, 176);
	assert_offsetof(GfxWorld, draw, 192);
	assert_offsetof(GfxWorld, lightGrid, 992);
	assert_offsetof(GfxWorld, probeData, 2072);
	assert_offsetof(GfxWorld, unknownData, 2320);
	assert_offsetof(GfxWorld, frustumLights, 2816);
	assert_offsetof(GfxWorld, voxelTreeCount, 2824);
	assert_offsetof(GfxWorld, voxelTree, 2832);
	assert_offsetof(GfxWorld, lightLists, 2840);
	assert_offsetof(GfxWorld, modelCount, 2888);
	assert_offsetof(GfxWorld, models, 2896);
	assert_offsetof(GfxWorld, bounds, 2904);
	assert_offsetof(GfxWorld, shadowBounds, 2928);
	assert_offsetof(GfxWorld, checksum, 2952);
	assert_offsetof(GfxWorld, materialMemoryCount, 2956);
	assert_offsetof(GfxWorld, materialMemory, 2960);
	assert_offsetof(GfxWorld, sun, 2968);
	assert_offsetof(GfxWorld, outdoorLookupMatrix, 3080);
	assert_offsetof(GfxWorld, outdoorImage, 3144);
	assert_offsetof(GfxWorld, cellCasterBits, 3152);
	assert_offsetof(GfxWorld, cellHasSunLitSurfsBits, 3160);
	assert_offsetof(GfxWorld, sceneDynModel, 3168);
	assert_offsetof(GfxWorld, sceneDynBrush, 3176);
	assert_offsetof(GfxWorld, primaryLightEntityShadowVis, 3184);
	assert_offsetof(GfxWorld, primaryLightDynEntShadowVis, 3192);
	assert_offsetof(GfxWorld, nonSunPrimaryLightForModelDynEnt, 3208);
	assert_offsetof(GfxWorld, shadowGeom, 3216);
	assert_offsetof(GfxWorld, shadowGeomOptimized, 3224);
	assert_offsetof(GfxWorld, lightRegion, 3232);
	assert_offsetof(GfxWorld, dpvs, 3240);
	assert_offsetof(GfxWorld, dpvsDyn, 4104);
	assert_offsetof(GfxWorld, mapVtxChecksum, 4200);
	assert_offsetof(GfxWorld, heroOnlyLightCount, 4204);
	assert_offsetof(GfxWorld, heroOnlyLights, 4208);
	assert_offsetof(GfxWorld, fogTypesAllowed, 4216);
	assert_offsetof(GfxWorld, mdaoVolumesCount, 4220);
	assert_offsetof(GfxWorld, mdaoVolumes, 4224);
	assert_offsetof(GfxWorld, useLightGridDefaultModelLightingLookup, 4234);
	assert_offsetof(GfxWorld, useLightGridDefaultFXLightingLookup, 4235);
	assert_offsetof(GfxWorld, lightGridDefaultModelLightingLookup, 4236);
	assert_offsetof(GfxWorld, lightGridDefaultFXLightingLookup, 4248);
	assert_offsetof(GfxWorld, buildInfo, 4264);

	struct AddonMapEntsCollision
	{
		char (*vertices)[16];
		char (*normals)[16];
		int* indices;
		char* data;
		char __pad0[0xC];
		int vertexCount;
		int normalCount;
		int indexCount;
		char __pad1[0x28];
	};
	static_assert(sizeof(AddonMapEntsCollision) == 0x60);

	struct AddonMapEntsShape
	{
		char (*planes)[32];
		char (*points)[16];
		char (*edges)[32];
		int planeCount;
		int pointCount;
		int edgeCount;
		char __pad0[0xC];
	};
	static_assert(sizeof(AddonMapEntsShape) == 0x30);

	struct AddonMapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars;
		MapTriggers trigger;
		ClipInfo* info;
		unsigned int numSubModels;
		cmodel_t* cmodels;
		char (*models)[96];
		char (*modelBounds)[8];
		AddonMapEntsCollision* collisions;
		AddonMapEntsShape* shapes;
		unsigned int collisionCount;
		unsigned int shapeCount;
	};
	static_assert(sizeof(AddonMapEnts) == 0x88);

	struct VirtualLeaderboardColumnDef
	{
		const char* name;
		char __pad0[0x18];
	};
	static_assert(sizeof(VirtualLeaderboardColumnDef) == 0x20);

	struct VirtualLeaderboardDef
	{
		const char* name;
		const char* sourceName;
		char __pad0[0x8];
		VirtualLeaderboardColumnDef* columns;
		int columnCount;
		char __pad1[0xC];
	};
	static_assert(sizeof(VirtualLeaderboardDef) == 0x30);

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
	};
	static_assert(sizeof(PhysMass) == 0x24);

	struct VehiclePhysDef
	{
		int physicsEnabled;
		PhysPreset* physPreset;
		const char* unk_0x10;
		PhysMass* physMass;
		const char* unk_0x20;
		char __pad0[0x140];
	};
	static_assert(sizeof(VehiclePhysDef) == 0x168);

	struct VehicleSoundEntry
	{
		snd_alias_list_t* sound;
		char __pad0[0x8];
	};
	static_assert(sizeof(VehicleSoundEntry) == 0x10);

	struct VehicleTrackObstacle
	{
		float origin[2];
		float radius;
	};
	static_assert(sizeof(VehicleTrackObstacle) == 0xC);

	struct VehicleTrackSector
	{
		float startEdgeDir[2];
		float startEdgeDist;
		float leftEdgeDir[2];
		float leftEdgeDist;
		float rightEdgeDir[2];
		float rightEdgeDist;
		float sectorLength;
		float sectorWidth;
		float totalPriorLength;
		float totalFollowingLength;
		VehicleTrackObstacle* obstacles;
		unsigned int obstacleCount;
	};
	static_assert(sizeof(VehicleTrackSector) == 0x48);

	struct VehicleTrackSegment
	{
		const char* targetName;
		VehicleTrackSector* sectors;
		unsigned int sectorCount;
		VehicleTrackSegment** nextBranches;
		unsigned int nextBranchesCount;
		VehicleTrackSegment** prevBranches;
		unsigned int prevBranchesCount;
		float endEdgeDir[2];
		float endEdgeDist;
		float totalLength;
	};
	static_assert(sizeof(VehicleTrackSegment) == 0x48);

	// same layout as H1
	struct VehicleTrack
	{
		const char* name;
		VehicleTrackSegment* segments;
		unsigned int segmentCount;
	};
	static_assert(sizeof(VehicleTrack) == 0x18);

	struct VehicleDef
	{
		const char* name;
		char __pad0[0x8];
		const char* unk_0x10;
		char __pad1[0x8];
		const char* unk_0x20;
		char __pad2[0x12C];
		scr_string_t unk_0x154[18];
		char __pad3[0x4];
		VehiclePhysDef vehPhysDef;
		char __pad4[0x40];
		FxEffectDef* unk_0x348[5];
		char __pad5[0xA8];
		FxEffectDef* unk_0x418[2];
		char __pad6[0x138];
		const char* unk_0x560;
		char __pad7[0x288];
		const char* unk_0x7F0;
		WeaponCompleteDef* unk_0x7F8;
		char __pad8[0x68];
		snd_alias_list_t* unk_0x868[2];
		char __pad9[0x14];
		scr_string_t unk_0x88C[4];
		char __pad10[0x4];
		FxEffectDef* unk_0x8A0[2];
		Material* unk_0x8B0[2];
		char __pad11[0x8];
		XAnimParts* unk_0x8C8[2];
		char __pad12[0x8];
		snd_alias_list_t* unk_0x8E0[5];
		char __pad13[0x4];
		scr_string_t unk_0x90C;
		snd_alias_list_t* unk_0x910[4];
		char __pad14[0x4];
		scr_string_t unk_0x934;
		snd_alias_list_t* unk_0x938[3];
		char __pad15[0x8];
		snd_alias_list_t* unk_0x958[4];
		char __pad16[0x8];
		VehicleSoundEntry unk_0x980[5];
		const char* unk_0x9D0;
		snd_alias_list_t* unk_0x9D8[62];
		char __pad17[0x10];
		const char* unk_0xBD8;
		char __pad18[0x8];
	};
	static_assert(sizeof(VehicleDef) == 0xBE8);

	struct Clut
	{
		int width;
		int height;
		int depth;
		char __pad0[0x4];
		char* data;
		char __pad1[0x10];
		GfxImage* image;
		const char* name;
	};
	static_assert(sizeof(Clut) == 0x38);

	struct EquipmentSoundSet
	{
		snd_alias_list_t* unk_0x0;
		snd_alias_list_t* unk_0x8;
	};
	static_assert(sizeof(EquipmentSoundSet) == 0x10);

	struct EquipmentSndEquipment
	{
		const char* name;
		char __pad0[0x8];
	};
	static_assert(sizeof(EquipmentSndEquipment) == 0x10);

	struct EquipmentSndSurfaceSounds
	{
		EquipmentSoundSet sets[14];
	};
	static_assert(sizeof(EquipmentSndSurfaceSounds) == 0xE0);

	struct EquipmentSndTable
	{
		const char* name;
		unsigned int surfaceCount;
		unsigned int equipmentCount;
		unsigned int soundCount;
		char __pad0[0x4];
		const char** surfaceNames;
		EquipmentSndEquipment* equipment;
		vec3_t*** unk_0x28;
		vec3_t*** unk_0x30;
		EquipmentSoundSet** surfaceSounds;
		EquipmentSoundSet** equipmentSounds;
		EquipmentSoundSet* sounds;
		EquipmentSndSurfaceSounds* unk_0x50;
	};
	static_assert(sizeof(EquipmentSndTable) == 0x58);

	union XAssetHeader
	{
		void* data;
		RawFile* rawfile;
		ScriptFile* scriptfile;
		StringTable* stringTable;
		LocalizeEntry* localize;
		LuaFile* luaFile;
		TTFDef* ttfDef;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetEntry
	{
		XAsset asset;
		char zoneIndex;
		volatile char inuseMask;
		unsigned int nextHash;
		unsigned int nextOverride;
		unsigned int nextPoolEntry;
	};

	struct ScriptStringList
	{
		int count;
		const char** strings;
	};

	union GfxZoneTableEntry
	{
		char* dataPtr;
		void* voidPtr;
	};

	typedef std::uint32_t GfxBlendStateBits[3];

	struct XGfxGlobals
	{
		unsigned int depthStencilStateCount;
		unsigned int blendStateCount;
		std::uint64_t* depthStencilStateBits;
		GfxBlendStateBits* blendStateBits;
		GfxZoneTableEntry* depthStencilStates;
		GfxZoneTableEntry* blendStates;
		unsigned int perPrimConstantBufferCount;
		unsigned int perObjConstantBufferCount;
		unsigned int volatileConstantBufferCount;
		unsigned int stableConstantBufferCount;
		unsigned int* perPrimConstantBufferSizes;
		unsigned int* perObjConstantBufferSizes;
		unsigned int* volatileConstantBufferSizes;
		unsigned int* stableConstantBufferSizes;
		GfxZoneTableEntry* perPrimConstantBuffers;
		GfxZoneTableEntry* perObjConstantBuffers;
		GfxZoneTableEntry* volatileConstantBuffers;
		GfxZoneTableEntry* stableConstantBuffers;
	};
	static_assert(sizeof(XGfxGlobals) == 0x78);

	struct XGlobals
	{
		XGfxGlobals* gfxGlobals;
	};

	struct XAssetList
	{
		ScriptStringList stringList;
		int assetCount;
		XAsset* assets;
		XGlobals* globals;
	};
	static_assert(sizeof(XAssetList) == 0x28);
}
