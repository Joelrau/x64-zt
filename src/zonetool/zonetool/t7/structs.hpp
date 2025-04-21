#pragma once
#include <d3d11.h>

namespace zonetool::t7
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	typedef uint32_t ScrString_t;
	typedef ScrString_t ScrString;

	typedef int qboolean;

	typedef uint16_t ushort;
	typedef uint32_t uint;

	typedef uint16_t UnsignedShort;

	typedef float float2[2];
	typedef float float3[3];
	typedef float float4[4];

	typedef uint uint2[2];
	typedef uint uint3[3];
	typedef uint uint4[4];

	typedef byte ByteVec[3];
	typedef uint16_t UShortVec[3];

	typedef byte byte4;
	typedef byte byte16;

	typedef unsigned __int64 __uint64_t;
	typedef __uint64_t __uintptr_t;
	typedef __uintptr_t uintptr_t_0;
	typedef __int64 intptr_t_0;

	typedef intptr_t_0 scr_funcptr_t;

	enum LocalClientNum_t : __int32
	{
		//INVALID_LOCAL_CLIENT = 0xFFFFFFFF,
		LOCAL_CLIENT_0 = 0x0,
		LOCAL_CLIENT_FIRST = 0x0,
		LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
		LOCAL_CLIENT_1 = 0x1,
		LOCAL_CLIENT_2 = 0x2,
		LOCAL_CLIENT_3 = 0x3,
		LOCAL_CLIENT_COUNT = 0x4,
	};
	typedef LocalClientNum_t LocalClientNum_t_0;

	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCONSTRAINTS = 0x1,
		ASSET_TYPE_DESTRUCTIBLEDEF = 0x2,
		ASSET_TYPE_XANIMPARTS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_XMODELMESH = 0x5,
		ASSET_TYPE_MATERIAL = 0x6,
		ASSET_TYPE_COMPUTE_SHADER_SET = 0x7,
		ASSET_TYPE_TECHNIQUE_SET = 0x8,
		ASSET_TYPE_IMAGE = 0x9,
		ASSET_TYPE_SOUND = 0xA,
		ASSET_TYPE_SOUND_PATCH = 0xB,
		ASSET_TYPE_CLIPMAP = 0xC,
		ASSET_TYPE_COMWORLD = 0xD,
		ASSET_TYPE_GAMEWORLD = 0xE,
		ASSET_TYPE_MAP_ENTS = 0xF,
		ASSET_TYPE_GFXWORLD = 0x10,
		ASSET_TYPE_LIGHT_DEF = 0x11,
		ASSET_TYPE_LENSFLARE_DEF = 0x12,
		ASSET_TYPE_UI_MAP = 0x13,
		ASSET_TYPE_FONT = 0x14,
		ASSET_TYPE_FONTICON = 0x15,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
		ASSET_TYPE_WEAPON = 0x17,
		ASSET_TYPE_WEAPONDEF = 0x18,
		ASSET_TYPE_WEAPON_VARIANT = 0x19,
		ASSET_TYPE_WEAPON_FULL = 0x1A,
		ASSET_TYPE_CGMEDIA = 0x1B,
		ASSET_TYPE_PLAYERSOUNDS = 0x1C,
		ASSET_TYPE_PLAYERFX = 0x1D,
		ASSET_TYPE_SHAREDWEAPONSOUNDS = 0x1E,
		ASSET_TYPE_ATTACHMENT = 0x1F,
		ASSET_TYPE_ATTACHMENT_UNIQUE = 0x20,
		ASSET_TYPE_WEAPON_CAMO = 0x21,
		ASSET_TYPE_CUSTOMIZATION_TABLE = 0x22,
		ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES = 0x23,
		ASSET_TYPE_CUSTOMIZATION_TABLE_COLOR = 0x24,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x25,
		ASSET_TYPE_FX = 0x26,
		ASSET_TYPE_TAGFX = 0x27,
		ASSET_TYPE_NEW_LENSFLARE_DEF = 0x28,
		ASSET_TYPE_IMPACT_FX = 0x29,
		ASSET_TYPE_IMPACT_SOUND = 0x2A,
		ASSET_TYPE_PLAYER_CHARACTER = 0x2B,
		ASSET_TYPE_AITYPE = 0x2C,
		ASSET_TYPE_CHARACTER = 0x2D,
		ASSET_TYPE_XMODELALIAS = 0x2E,
		ASSET_TYPE_RAWFILE = 0x2F,
		ASSET_TYPE_STRINGTABLE = 0x30,
		ASSET_TYPE_STRUCTURED_TABLE = 0x31,
		ASSET_TYPE_LEADERBOARD = 0x32,
		ASSET_TYPE_DDL = 0x33,
		ASSET_TYPE_GLASSES = 0x34,
		ASSET_TYPE_TEXTURELIST = 0x35,
		ASSET_TYPE_SCRIPTPARSETREE = 0x36,
		ASSET_TYPE_KEYVALUEPAIRS = 0x37,
		ASSET_TYPE_VEHICLEDEF = 0x38,
		ASSET_TYPE_ADDON_MAP_ENTS = 0x39,
		ASSET_TYPE_TRACER = 0x3A,
		ASSET_TYPE_SLUG = 0x3B,
		ASSET_TYPE_SURFACEFX_TABLE = 0x3C,
		ASSET_TYPE_SURFACESOUNDDEF = 0x3D,
		ASSET_TYPE_FOOTSTEP_TABLE = 0x3E,
		ASSET_TYPE_ENTITYFXIMPACTS = 0x3F,
		ASSET_TYPE_ENTITYSOUNDIMPACTS = 0x40,
		ASSET_TYPE_ZBARRIER = 0x41,
		ASSET_TYPE_VEHICLEFXDEF = 0x42,
		ASSET_TYPE_VEHICLESOUNDDEF = 0x43,
		ASSET_TYPE_TYPEINFO = 0x44,
		ASSET_TYPE_SCRIPTBUNDLE = 0x45,
		ASSET_TYPE_SCRIPTBUNDLELIST = 0x46,
		ASSET_TYPE_RUMBLE = 0x47,
		ASSET_TYPE_BULLETPENETRATION = 0x48,
		ASSET_TYPE_LOCDMGTABLE = 0x49,
		ASSET_TYPE_AIMTABLE = 0x4A,
		ASSET_TYPE_ANIMSELECTORTABLESET = 0x4B,
		ASSET_TYPE_ANIMMAPPINGTABLE = 0x4C,
		ASSET_TYPE_ANIMSTATEMACHINE = 0x4D,
		ASSET_TYPE_BEHAVIORTREE = 0x4E,
		ASSET_TYPE_BEHAVIORSTATEMACHINE = 0x4F,
		ASSET_TYPE_TTF = 0x50,
		ASSET_TYPE_SANIM = 0x51,
		ASSET_TYPE_LIGHT_DESCRIPTION = 0x52,
		ASSET_TYPE_SHELLSHOCK = 0x53,
		ASSET_TYPE_XCAM = 0x54,
		ASSET_TYPE_BG_CACHE = 0x55,
		ASSET_TYPE_TEXTURE_COMBO = 0x56,
		ASSET_TYPE_FLAMETABLE = 0x57,
		ASSET_TYPE_BITFIELD = 0x58,
		ASSET_TYPE_ATTACHMENT_COSMETIC_VARIANT = 0x59,
		ASSET_TYPE_MAPTABLE = 0x5A,
		ASSET_TYPE_MAPTABLE_LOADING_IMAGES = 0x5B,
		ASSET_TYPE_MEDAL = 0x5C,
		ASSET_TYPE_MEDALTABLE = 0x5D,
		ASSET_TYPE_OBJECTIVE = 0x5E,
		ASSET_TYPE_OBJECTIVE_LIST = 0x5F,
		ASSET_TYPE_UMBRA_TOME = 0x60,
		ASSET_TYPE_NAVMESH = 0x61,
		ASSET_TYPE_NAVVOLUME = 0x62,
		ASSET_TYPE_BINARYHTML = 0x63,
		ASSET_TYPE_LASER = 0x64,
		ASSET_TYPE_BEAM = 0x65,
		ASSET_TYPE_STREAMER_HINT = 0x66,
		ASSET_TYPE_COUNT = 0x67,
		ASSET_TYPE_STRING = 0x68,
		ASSET_TYPE_ASSETLIST = 0x69,
		ASSET_TYPE_REPORT = 0x6A,
		ASSET_TYPE_DEPEND = 0x68,
		ASSET_TYPE_FULL_COUNT = 0x6C,
	};

	struct PhysPreset;
	struct PhysConstraints;
	//struct DestructibleDef;
	struct XAnimParts;
	struct XModel;
	struct XModelMesh;
	struct Material;
	struct MaterialComputeShaderSet;
	struct MaterialTechniqueSet;
	struct GfxImage;
	//struct SndBank*;
	//struct SndPatch;
	struct clipMap_t;
	struct ComWorld;
	struct GameWorld;
	struct MapEnts;
	//struct GfxWorld;
	//struct GfxLightDef;
	//struct GfxLensFlareDef;
	//struct Font;
	//struct FontIcon;
	//struct LocalizeEntry;
	//struct WeaponVariantDef;
	//struct WeaponAttachment;
	//struct WeaponAttachmentUnique;
	//struct WeaponCamo;
	//struct CustomizationTable;
	//struct CustomizationColorInfo;
	//struct SndDriverGlobals;
	//struct FxEffectDefHandleRaw;
	//struct TagFxSet;
	//struct FxLensFlareDefPtr;
	//struct FxImpactTable;
	//struct SoundsImpactTable;
	//struct CgMediaTable;
	//struct PlayerSoundsTable;
	//struct PlayerFXTable;
	//struct SharedWeaponSounds;
	//struct RawFile;
	//struct StringTable;
	//struct StructuredTable;
	//struct LeaderboardDef;
	//struct DDLRoot;
	//struct Glasses;
	//struct TextureList;
	//struct ScriptParseTree;
	//struct KeyValuePairs;
	//struct VehicleDef;
	//struct AddonMapEnts;
	//struct TracerDef;
	//struct Qdb;
	//struct Slug;
	//struct SurfaceFXTableDef;
	//struct SurfaceSoundDef;
	//struct FootstepTableDef;
	//struct EntitySoundImpacts;
	//struct EntityFxImpacts;
	//struct ZBarrierDef;
	//struct VehicleFxDef;
	//struct VehicleSoundDef;
	//struct ArchiveTypeInfoArray;
	//struct ScriptBundle;
	//struct ScriptBundleList;
	//struct RumbleInfo;
	//struct BulletPenetrationTable;
	//struct LocDmgTable;
	//struct AimTable;
	//struct XModelAlias;
	//struct Character;
	//struct AIType;
	//struct PlayerCharacter;
	//struct AnimSelectorTableSet;
	//struct AnimMappingTable;
	//struct AnimStateMachine;
	//struct BehaviorTree;
	//struct BehaviorStateMachine;
	//struct TTFDef;
	//struct GfxSiegeAnim;
	//struct GfxLightDescription;
	//struct ShellshockParams;
	//struct XCam;
	//struct BGCacheInfo;
	//struct TextureCombo;
	//struct FlameTable;
	//struct Bitfield;
	//struct AttachmentCosmeticVariant;
	//struct MapTable;
	//struct Medal;
	//struct MedalTable;
	//struct Objective;
	//struct ObjectiveList;
	//struct NavMeshData;
	//struct NavVolumeData;
	//struct BinaryHTML;
	//struct LaserDef;
	//struct BeamDef;
	//struct StreamerHint;

	struct FxEffectDef;
	typedef const FxEffectDef* FxEffectDefHandle;

	typedef int FxUniqueHandle;

	typedef uint32_t SndStringHash;
	typedef SndStringHash SndAliasId;

	struct XPakEntryInfo
	{
		uint64_t key;
		int64_t offset;
		uint64_t size;
		unsigned __int64 xpakIndex : 5;
		unsigned __int64 compressed : 1;
		unsigned __int64 valid : 1;
		unsigned __int64 adjacentLeftType : 3;
		unsigned __int64 adjacentRightType : 3;
		unsigned __int64 adjacentLeft : 18;
		unsigned __int64 adjacentRight : 18;
		unsigned __int64 padding : 15;
	};

	typedef uint32_t contents_t;

	union GfxColor
	{
		uint32_t packed;
		byte array[4];
	};

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};

	struct GfxPlacement
	{
		vec4_t quat;
		vec3_t origin;
	};

	enum MaterialDrawMethodType : uint8_t
	{
		MATERIAL_DRAW_METHOD_SHIP = 0x0,
		MATERIAL_DRAW_METHOD_DEBUG = 0x1,
		MATERIAL_DRAW_METHOD_COUNT = 0x2,
		MATERIAL_DRAW_METHOD_VENDOR_COUNT = 0x2,
		MATERIAL_DRAW_METHOD_VENDOR_VALID_COUNT = 0x2,
		MATERIAL_DRAW_METHOD_DEFAULT = 0x0,
		MATERIAL_DRAW_METHOD_INVALID = 0xFF,
	};

	struct MaterialArgumentCodeConst
	{
		unsigned __int16 index;
		byte firstRow;
		byte rowCount;
	};

	typedef uint16_t GfxSamplerState;

	union MaterialArgumentDef
	{
		const float* literalConst;
		MaterialArgumentCodeConst codeConst;
		uint codeTexture;
		GfxSamplerState samplerState;
		uint nameHash;
	};

	struct MaterialShaderArgument
	{
		unsigned __int16 type;
		unsigned __int16 location;
		unsigned __int16 size;
		unsigned __int16 buffer;
		MaterialArgumentDef u;
	};

	struct GfxComputeShaderLoadDef
	{
		byte* program;
		uint programSize;
	};

	struct MaterialComputeShaderProgram
	{
		ID3D11ComputeShader* cs;
		GfxComputeShaderLoadDef loadDef;
	};

	struct MaterialComputeShaderMethod
	{
		MaterialComputeShaderProgram prog;
		uint groupSize[3];
		uint argCount;
		MaterialShaderArgument* args;
		MaterialDrawMethodType drawMethodType;
	};

	struct MaterialComputeShader
	{
		const char* name;
		const char* extraDef;
		MaterialComputeShaderMethod methods[8];
	};

	struct MaterialComputeShaderSet
	{
		const char* name;
		uint count;
		MaterialComputeShader** shaders;
	};

	enum MaterialShaderPipeline : int32_t
	{
		SHADER_PIPELINE_NONE = 0x0,
		SHADER_PIPELINE_VS_PS = 0x1,
		SHADER_PIPELINE_LS_HS_VS_PS = 0x2,
		SHADER_PIPELINE_LS_HS_ES_GS_VS_PS = 0x3,
		SHADER_PIPELINE_ES_GS_VS_PS = 0x4,
	};

	typedef byte MaterialStreamDest;

	struct VertexDeclaration
	{
		char __pad0[16];
	};

	struct MaterialVertexDeclaration
	{
		char __pad0[24];
		VertexDeclaration data[6];
	}; assert_sizeof(MaterialVertexDeclaration, 0x78);

	enum GfxPrimitiveType : int32_t
	{
		kPrimitiveTypeNone = 0x0,
		kPrimitiveTypePointList = 0x1,
		kPrimitiveTypeLineList = 0x2,
		kPrimitiveTypeLineStrip = 0x3,
		kPrimitiveTypeTriList = 0x4,
		kPrimitiveTypeTriFan = 0x5,
		kPrimitiveTypeTriStrip = 0x6,
		kPrimitiveTypePatch = 0x9,
		kPrimitiveTypeLineListAdjacency = 0xA,
		kPrimitiveTypeLineStripAdjacency = 0xB,
		kPrimitiveTypeTriListAdjacency = 0xC,
		kPrimitiveTypeTriStripAdjacency = 0xD,
		kPrimitiveTypeRectList = 0x11,
		kPrimitiveTypeLineLoop = 0x12,
		kPrimitiveTypeQuadList = 0x13,
		kPrimitiveTypeQuadStrip = 0x14,
		kPrimitiveTypePolygon = 0x15,
	};

	struct GfxVertexShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialVertexShaderProgram
	{
		ID3D11VertexShader* vs;
		GfxVertexShaderLoadDef loadDef;
	};

	struct MaterialVertexShader
	{
		const char* name;
		const char* extraDef;
		MaterialVertexShaderProgram prog;
	};

	struct GfxPixelShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialPixelShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxPixelShaderLoadDef loadDef;
	};

	struct MaterialPixelShader
	{
		const char* name;
		const char* extraDef;
		MaterialPixelShaderProgram prog;
	};

	struct GfxGeometryShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialGeometryShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxGeometryShaderLoadDef loadDef;
	};

	struct MaterialGeometryShader
	{
		const char* name;
		const char* extraDef;
		MaterialGeometryShaderProgram prog;
	};

	struct GfxLocalShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialLocalShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxLocalShaderLoadDef loadDef;
	};

	struct MaterialLocalShader
	{
		const char* name;
		const char* extraDef;
		MaterialLocalShaderProgram prog;
	};

	struct GfxHullShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialHullShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxHullShaderLoadDef loadDef;
	};

	struct MaterialHullShader
	{
		const char* name;
		const char* extraDef;
		MaterialHullShaderProgram prog;
	};

	struct GfxDomainShaderLoadDef
	{
		byte* program;
		uint programSize;
		uint microCodeCrc;
	};

	struct MaterialDomainShaderProgram
	{
		ID3D11PixelShader* ps;
		GfxDomainShaderLoadDef loadDef;
	};

	struct MaterialDomainShader
	{
		const char* name;
		const char* extraDef;
		MaterialDomainShaderProgram prog;
	};

	struct MaterialDrawMethod
	{
		MaterialShaderPipeline pipeline;
		MaterialVertexDeclaration* vertexDecl;
		GfxPrimitiveType primitiveType;
		uint32_t indexCount;
		MaterialVertexShader* vertexShader;
		MaterialPixelShader* pixelShader;
		MaterialGeometryShader* geometryShader;
		MaterialLocalShader* localShader;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		//uint shaderUsageCount;
		//uint* shaderUsageTable;
		byte stableArgCount;
		byte materialType;
		MaterialDrawMethodType drawMethodType;
		MaterialShaderArgument* args;
	}; assert_sizeof(MaterialDrawMethod, 88);

	typedef void* GfxBlendStatePtr;
	typedef void* GfxDepthStencilStatePtr;
	typedef void* GfxRasterizerStatePtr;

	struct GfxStateMap
	{
		struct Blend
		{
			unsigned __int32 valid : 1;
			unsigned __int32 enabled : 1;
			unsigned __int32 rgbSrc : 4;
			unsigned __int32 rgbOp : 3;
			unsigned __int32 rgbDst : 4;
			unsigned __int32 alphaSrc : 4;
			unsigned __int32 alphaOp : 3;
			unsigned __int32 alphaDst : 4;
			unsigned __int32 writeMask : 4;
		};

		struct Depth
		{
			unsigned __int32 write : 1;
			unsigned __int32 test : 3;
		};

		struct Stencil
		{
			unsigned __int32 enable : 1;
			unsigned __int32 passOp : 3;
			unsigned __int32 failOp : 3;
			unsigned __int32 zFailOp : 3;
			unsigned __int32 func : 3;
			unsigned __int32 mask : 8;
		};

		struct Primitive
		{
			unsigned __int32 mrtBlending : 1;
			unsigned __int32 cullMode : 2;
			unsigned __int32 polygonOffset : 2;
			unsigned __int32 wireframe : 1;
			unsigned __int32 depthClipEnable : 1;
		};

		Blend blend[8];
		Depth depth;
		Stencil stencilFront;
		Stencil stencilBack;
		Primitive primitive;
		byte stencilRef;
		GfxBlendStatePtr blendState;
		GfxDepthStencilStatePtr depthStencilState;
		GfxDepthStencilStatePtr depthStencilState_reverseDepthTest;
		GfxDepthStencilStatePtr depthStencilState_replaceStencil;
		GfxRasterizerStatePtr rasterizerState;
		GfxRasterizerStatePtr rasterizerState_reverseCull;
		GfxRasterizerStatePtr rasterizerState_depthClipDisable;
	}; assert_sizeof(GfxStateMap, 112);

	struct MaterialTechnique
	{
		const char* name;
		MaterialDrawMethod drawMethods[8];
		GfxStateMap* stateMap;
	}; assert_sizeof(MaterialTechnique, 720);

	struct MaterialTechniqueSet
	{
		const char* name;
		uint renderFlags;
		MaterialTechnique* techniques[12];
	};

	struct GfxDrawSurfFields
	{
		unsigned __int64 useSiegeTextures : 1;
		unsigned __int64 objectId : 18;
		unsigned __int64 lateDepthSort : 8;
		unsigned __int64 materialSortedIndex : 16;
		unsigned __int64 decalSurfSort : 6;
		unsigned __int64 earlyDepthSort : 15;
	};

	union GfxSortKey
	{
		GfxDrawSurfFields fields;
		uint64_t packed;
	};

	struct MaterialInfo
	{
		const char* name;
		uint32_t gameFlags;
		byte layerSortDecal;
		byte pad2;
		byte textureAtlasRowCount;
		byte textureAtlasColumnCount;
		GfxSortKey drawSurf;
		uint64_t surfaceTypeBits;
		uint32_t layeredSurfaceTypes;
		uint32_t surfaceFlags;
		contents_t contents;
	};

	struct GfxStaticConstantBufferLoadDef
	{
		byte* initData;
		uint initDataSize;
	};

	struct GfxConstantBuffer
	{
		void* buffer1;
		void* buffer2;
		void* buffer3;
		void* buffer4;
		void* buffer5;
	};

	struct MaterialStaticConstantBufferData
	{
		GfxConstantBuffer buffer;
		GfxStaticConstantBufferLoadDef loadDef;
	};

	struct MaterialStaticConstantBuffer
	{
		MaterialStaticConstantBufferData data;
		const char* name;
		uint deviceSlot;
	};

	struct MaterialTechniqueConstantBuffer
	{
		MaterialStaticConstantBuffer* dsStatic[2];
		MaterialStaticConstantBuffer* vsStatic[2];
		MaterialStaticConstantBuffer* psStatic[2];
	};

	struct MaterialConstantBufferInfo
	{
		MaterialTechniqueConstantBuffer constantBuffers[12];
	};

	enum GfxCameraRegion : int8_t
	{
		CAMERA_REGION_GBUFFER_DEPTH_HACK = 0x0,
		CAMERA_REGION_GBUFFER_DEPTH_HACK_DECAL = 0x1,
		CAMERA_REGION_GBUFFER = 0x2,
		CAMERA_REGION_GBUFFER_DECAL = 0x3,
		CAMERA_REGION_LIT_OPAQUE = 0x4,
		CAMERA_REGION_LIT_TRANS = 0x5,
		CAMERA_REGION_LIT_TRANS_POST_RESOLVE = 0x6,
		CAMERA_REGION_LIT_POST_RESOLVE = 0x7,
		CAMERA_REGION_EMISSIVE_FX = 0x8,
		CAMERA_REGION_DEPTH_HACK_SSS = 0x9,
		CAMERA_REGION_FORWARD_SSS = 0xA,
		CAMERA_REGION_DEPTH_HACK = 0xB,
		CAMERA_REGION_DEPTH_HACK_TRANS = 0xC,
		CAMERA_REGION_DEPTH_HACK_POST_BLUR = 0xD,
		CAMERA_REGION_SONAR = 0xE,
		CAMERA_REGION_OIT = 0xF,
		CAMERA_REGION_COUNT = 0x10,
		CAMERA_REGION_NONE = 0x10,
	};

	struct MaterialTextureDef
	{
		GfxImage* image;
		uint nameHash;
		vec2_t uvScale;
		byte texSizeShift[2];
		uint16_t uvOffset[3];
		uint16_t usage;
		byte isMatureContent;
	};

	struct MaterialSamplerDef
	{
		uint nameHash;
		GfxSamplerState samplerState;
	};

	struct MaterialConstantDef
	{
		uint nameHash;
		char name[12];
		vec4_t literal;
	};

	struct Material
	{
		union
		{
			MaterialInfo info;
			const char* name;
		};
		MaterialConstantBufferInfo constantBufferInfo;
		byte textureCount;
		byte samplerCount;
		byte constantCount;
		GfxCameraRegion cameraRegion;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		MaterialSamplerDef* samplerTable;
		MaterialConstantDef* constantTable;
		Material* enemyMaterial;
	}; assert_sizeof(Material, 0x2A0);

	typedef Material* MaterialHandle;

	struct GfxImageStreamLevelCountAndSize
	{
		uint levelCount : 4;
		uint pixelSize : 28;
	};

	struct GfxStreamedPartInfo
	{
		GfxImageStreamLevelCountAndSize levelCountAndSize; //uint levelCountAndSize;
		uint16_t width;
		uint16_t height;
		XPakEntryInfo xpakEntry;
	};

	enum GfxImageSemantic : int8_t
	{
		IMG_SEMANTIC_UNKNOWN = 0x0,
		IMG_SEMANTIC_2D = 0x1,
		IMG_SEMANTIC_DIFFUSE_MAP = 0x2,
		IMG_SEMANTIC_EFFECT_MAP = 0x3,
		IMG_SEMANTIC_NORMAL_MAP = 0x4,
		IMG_SEMANTIC_SPECULAR_MASK = 0x5,
		IMG_SEMANTIC_SPECULAR_MAP = 0x6,
		IMG_SEMANTIC_GLOSS_MAP = 0x7,
		IMG_SEMANTIC_OCCLUSION_MAP = 0x8,
		IMG_SEMANTIC_REVEAL_MAP = 0x9,
		IMG_SEMANTIC_MULTIPLE_MASK = 0xA,
		IMG_SEMANTIC_THICKNESS_MAP = 0xB,
		IMG_SEMANTIC_CAMO_MAP = 0xC,
		IMG_SEMANTIC_ONE_CHANNEL = 0xD,
		IMG_SEMANTIC_TWO_CHANNEL = 0xE,
		IMG_SEMANTIC_EMBLEM = 0xF,
		IMG_SEMANTIC_CUSTOM = 0x10,
		IMG_SEMANTIC_LUT_TPAGE = 0x11,
		IMG_SEMANTIC_LIGHT_COOKIE = 0x12,
		IMG_SEMANTIC_HDR = 0x13,
		IMG_SEMANTIC_EYE_CAUSTIC = 0x14,
		IMG_SEMANTIC_COUNT = 0x15,
	};

	enum MapType : byte
	{
		MAPTYPE_NONE = 0x0,
		MAPTYPE_2D = 0x1,
		MAPTYPE_2D_ARRAY = 0x2,
		MAPTYPE_3D = 0x3,
		MAPTYPE_CUBE = 0x4,
		MAPTYPE_CUBE_ARRAY = 0x5,
		MAPTYPE_COUNT = 0x6,
	};

	enum GfxImageCategory : byte
	{
		IMG_CATEGORY_UNKNOWN = 0x0,
		IMG_CATEGORY_AUTO_GENERATED = 0x1,
		IMG_CATEGORY_LIGHTMAP = 0x2,
		IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
		IMG_CATEGORY_RAW = 0x4,
		IMG_CATEGORY_RENDERTARGET = 0x5,
		IMG_CATEGORY_TEMP = 0x6,
	};

	struct GfxTexture
	{
		uint64_t data;
	};

	enum GfxPixelFormat : int32_t
	{
		GFX_PF_INVALID = 0x0,
		GFX_PF_UNKNOWN = 0x0,
		GFX_PF_D24S8 = 0x1,
		GFX_PF_D24FS8 = 0x2,
		GFX_PF_A8 = 0x3,
		GFX_PF_R8_UN = 0x4,
		GFX_PF_R8_UI = 0x5,
		GFX_PF_D16 = 0x6,
		GFX_PF_R16_UN = 0x7,
		GFX_PF_R16_UI = 0x8,
		GFX_PF_R16F = 0x9,
		GFX_PF_R8G8_UN = 0xA,
		GFX_PF_R8G8_SNORM = 0xB,
		GFX_PF_B5G6R5 = 0xC,
		GFX_PF_B4G4R4A4 = 0xD,
		GFX_PF_D32F = 0xE,
		GFX_PF_R32F = 0xF,
		GFX_PF_R16G16_UN = 0x10,
		GFX_PF_R16G16_SNORM = 0x11,
		GFX_PF_R16G16F = 0x12,
		GFX_PF_R9G9B9E5 = 0x13,
		GFX_PF_R8G8B8A8 = 0x14,
		GFX_PF_R8G8B8A8_SNORM = 0x15,
		GFX_PF_B8G8R8A8 = 0x16,
		GFX_PF_R8G8B8A8_SRGB = 0x17,
		GFX_PF_R10G10B10A2 = 0x18,
		GFX_PF_B10G10R10A2 = 0x19,
		GFX_PF_R11G11B10F = 0x1A,
		GFX_PF_R32G32F = 0x1B,
		GFX_PF_R16G16B16A16_UN = 0x1C,
		GFX_PF_R16G16B16A16F = 0x1D,
		GFX_PF_R32G32B32A32F = 0x1E,
		GFX_PF_BC1 = 0x1F,
		GFX_PF_BC1_SRGB = 0x20,
		GFX_PF_BC2 = 0x21,
		GFX_PF_BC2_SRGB = 0x22,
		GFX_PF_BC3 = 0x23,
		GFX_PF_BC3_SRGB = 0x24,
		GFX_PF_BC4 = 0x25,
		GFX_PF_BC5 = 0x26,
		GFX_PF_BC6_UH = 0x27,
		GFX_PF_BC6_SH = 0x28,
		GFX_PF_BC7 = 0x29,
		GFX_PF_BC7_SRGB = 0x2A,
		GFX_PF_R32G32B32A32_UN = 0x2B,
		GFX_PF_R32_UI = 0x2C,
		GFX_PF_R32G32_UI = 0x2D,
	};

	enum file_image_flags_t : int32_t
	{
		IMG_FLAG_NOPICMIP = 0x1,
		IMG_FLAG_NOMIPMAPS = 0x2,
		IMG_FLAG_CUBEMAP = 0x4,
		IMG_FLAG_VOLMAP = 0x8,
		IMG_FLAG_STREAMING = 0x10,
		IMG_FLAG_CLAMP_U = 0x40,
		IMG_FLAG_CLAMP_V = 0x80,
		IMG_FLAG_FORCE_SYSTEM = 0x100,
		IMG_FLAG_ARRAY = 0x200,
		IMG_FLAG_DYNAMIC = 0x10000,
		IMG_FLAG_RENDER_TARGET = 0x20000,
		IMG_FLAG_UNTILED = 0x40000,
		IMG_FLAG_CPU_READ = 0x80000,
		IMG_FLAG_CPU_WRITE = 0x100000,
		IMG_FLAG_GPU_WRITE = 0x200000,
		IMG_FLAG_TYPE_CAST = 0x400000,
		IMG_FLAG_LIGHTDEF = 0x800000,
		IMG_FLAG_DX_ONLY = 0x1000000,
		IMG_FLAG_INTERFRAME_USE = 0x2000000,
		IMG_FLAG_CLEARMEM = 0x4000000,
	};

	struct GfxImage
	{
		GfxStreamedPartInfo streamedParts[4];
		byte streamedPartCount;
		GfxImageSemantic semantic;
		MapType mapType;
		GfxImageCategory category;
		byte useFallback;
		char __pad0[3];
		GfxTexture texture;
		GfxTexture fallbackTexture;
		unsigned int flags;
		bool delayLoadPixels;
		char __pad1[2];
		byte levelCount1;
		unsigned __int16 width1;
		unsigned __int16 height1;
		unsigned __int16 width;
		unsigned __int16 height;
		unsigned __int16 depth;
		int alignment; // always 16
		byte levelCount;
		byte fallbackLevel;
		byte streaming;
		byte* pixels;
		byte* fallbackPixels;
		uint totalSize;
		uint fallbackSize;
		DXGI_FORMAT format; //GfxPixelFormat format;
		const char* name;
		unsigned int hash;
	}; assert_sizeof(GfxImage, 0x108);
	assert_offsetof(GfxImage, name, 248);

	struct PhysPreset
	{
		const char* name;
		int flags;
		float mass;
		float bounce;
		float friction;
		float damping_linear;
		float damping_angular;
		float bulletForceScale;
		float explosiveForceScale;
		const char* sndAliasPrefix;
		int canFloat;
		float gravityScale;
		vec3_t centerOfMassOffset;
		vec3_t buoyancyBoxMin;
		vec3_t buoyancyBoxMax;
		void* trailFX; //FxEffectDefHandle trailFX;
		void* impactFX; //FxImpactTablePtr impactFX;
		void* impactSounds; //SoundsImpactTablePtr impactSounds;
	}; assert_sizeof(PhysPreset, 0x78);

	enum ConstraintType : int32_t
	{
		CONSTRAINT_NONE = 0x0,
		CONSTRAINT_POINT = 0x1,
		CONSTRAINT_DISTANCE = 0x2,
		CONSTRAINT_HINGE = 0x3,
		CONSTRAINT_ACTUATOR = 0x4,
		CONSTRAINT_FAKE_SHAKE = 0x5,
		CONSTRAINT_LAUNCH = 0x6,
		CONSTRAINT_ANTENNA = 0x7,
		CONSTRAINT_ROPE = 0x8,
		CONSTRAINT_LIGHT = 0x9,
		NUM_CONSTRAINT_TYPES = 0xA,
	};

	enum AttachPointType : int32_t
	{
		ATTACH_POINT_WORLD = 0x0,
		ATTACH_POINT_DYNENT = 0x1,
		ATTACH_POINT_ENT = 0x2,
		ATTACH_POINT_BONE = 0x3,
	};

	struct PhysConstraint
	{
		ScrString_t targetname;
		ConstraintType type;
		AttachPointType attach_point_type1;
		int target_index1;
		ScrString_t target_ent1;
		ScrString_t target_bone1;
		AttachPointType attach_point_type2;
		int target_index2;
		ScrString_t target_ent2;
		ScrString_t target_bone2;
		vec3_t offset;
		vec3_t pos;
		vec3_t pos2;
		vec3_t dir;
		int flags;
		int timeout;
		int min_health;
		int max_health;
		float distance;
		float damp;
		float power;
		vec3_t force_scale;
		float misc_scale;
		float minAngle;
		float maxAngle;
		float minAngleYaw;
		float maxAngleYaw;
		MaterialHandle material;
		int model;
		int slice_count;
		float gravity;
		bool useAntennaXAxis;
		__int64 constraintHandle;
		int rope_index;
		int centity_num[5];
	};

	struct PhysConstraints
	{
		const char* name;
		uint count;
		PhysConstraint data[8];
	}; assert_sizeof(PhysConstraints, 0x690);

	union XAnimIndices
	{
		byte* _1;
		ushort* _2;
		void* data;
	};

	struct XAnimIKPitchLayer
	{
		float angle;
		unsigned __int16 numBones;
		unsigned __int16 bonesStart;
	};

	struct XAnimIKPitchBone
	{
		UnsignedShort index;
		vec3_t rotation;
		vec3_t translation;
	};

	struct XAnimNotifyInfo
	{
		ScrString_t type;
		float time;
		ScrString_t param1;
		ScrString_t param2;
	};

	union XAnimDynamicFrames
	{
		ByteVec* _1;
		UShortVec* _2;
	};

	union XAnimDynamicIndices
	{
		byte _1[1];
		unsigned __int16 _2[1];
	};

	struct XAnimPartTransFrames
	{
		vec3_t mins;
		vec3_t size;
		XAnimDynamicFrames frames;
		XAnimDynamicIndices indices;
	};

	union XAnimPartTransData
	{
		XAnimPartTransFrames frames;
		vec3_t frame0;
	};

	struct XAnimPartTrans
	{
		unsigned __int16 size;
		byte smallTrans;
		XAnimPartTransData u;
	};

	typedef uint16_t XQuat2[2];
	typedef uint16_t XQuat[4];

	struct XAnimDeltaPartQuatDataFrames2
	{
		XQuat2* frames;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData2
	{
		XAnimDeltaPartQuatDataFrames2 frames;
		XQuat2 frame0;
	};

	struct XAnimDeltaPartQuat2
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData2 u;
	};

	struct XAnimDeltaPartQuatDataFrames
	{
		XQuat* frames;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		XQuat frame0;
	};

	struct XAnimDeltaPartQuat
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData u;
	};

	struct XAnimDeltaPart
	{
		XAnimPartTrans* trans;
		XAnimDeltaPartQuat2* quat2;
		XAnimDeltaPartQuat* quat;
	};

	enum BoneCountTypes
	{
		NoneRotatedBoneCount,
		TwoDRotatedBoneCount,
		NormalRotatedBoneCount, 
		TwoDStaticRotatedBoneCount, 
		NormalStaticRotatedBoneCount, 
		NormalTranslatedBoneCount, 
		PreciseTranslatedBoneCount, 
		StaticTranslatedBoneCount, 
		NoneTranslatedBoneCount, 
		TotalBoneCount
	};

	struct XAnimParts
	{
		const char* name;
		uint32_t randomDataByteCount;
		uint32_t dataShortCount;
		uint32_t extraChannelDataCount;
		uint32_t dataByteCount;
		uint32_t dataIntCount;
		uint32_t randomDataIntCount;
		unsigned __int16 numframes;
		unsigned __int16 numBones;
		bool bLoop;
		bool bLoopSync;
		bool bDelta;
		bool bDelta3D;
		bool bLeftHandGripIK;
		bool bHasBoneMask;
		bool bExtraChannel;
		uint streamedFileSize;
		uint16_t boneCount[10];
		byte assetType;
		bool pad;
		uint randomDataShortCount;
		uint indexCount;
		float framerate;
		float frequency;
		float primedLength;
		float loopEntryTime;
		uint numIKPitchLayers;
		uint numIKPitchBones;
		ScrString_t* names;
		byte* dataByte;
		__int16* dataShort;
		int* dataInt;
		__int16* randomDataShort;
		byte* randomDataByte;
		int* randomDataInt;
		byte* extraChannelData;
		XAnimIndices indices;
		XAnimIKPitchLayer* ikPitchLayers;
		XAnimIKPitchBone* ikPitchBones;
		XAnimNotifyInfo* notify;
		byte notifyCount;
		XAnimNotifyInfo* startupNotify;
		byte startupNotifyCount;
		XAnimNotifyInfo* shutdownNotify;
		byte shutdownNotifyCount;
		XAnimDeltaPart* deltaPart;
	};

	typedef int XPartBits[12];

	struct DObjAnimMat
	{
		vec4_t quat;
		vec3_t trans;
		float transWeight;
	};

	enum XModelLod : uint8_t
	{
		XMODEL_LOD_NONE = 0xFF,
		XMODEL_LOD_0 = 0x0,
		XMODEL_LOD_1 = 0x1,
		XMODEL_LOD_2 = 0x2,
		XMODEL_LOD_3 = 0x3,
		XMODEL_LOD_4 = 0x4,
		XMODEL_LOD_5 = 0x5,
		XMODEL_LOD_6 = 0x6,
		XMODEL_LOD_7 = 0x7,
		XMODEL_LOD_COUNT = 0x8,
		XMODEL_LOD_LOW = 0x0,
		XMODEL_LOD_HIGH = 0x7,
	};

	struct XModelMeshMaterials
	{
		ushort numMaterials;
		Material** materials;
		vec_t* himipInvSqRadii;
	};

	enum XModelUsageFlags : int32_t
	{
		XMODEL_USAGE_WEAPON = 0x1,
		XMODEL_USAGE_ATTACHMENT = 0x2,
		XMODEL_USAGE_HERO = 0x4,
		XMODEL_USAGE_VIEW = 0x8,
		XMODEL_USAGE_PREFAB = 0x10,
		XMODEL_USAGE_ZOMBIE_BODY = 0x20,
		XMODEL_USAGE_ZOMBIE_WORLD_PROP = 0x40,
	};
	typedef uint64_t XModelUsage;

	struct XBoneInfo
	{
		vec3_t bounds[2];
		vec3_t offset;
		float radiusSquared;
		byte collmap;
	};

	struct XModelAngle
	{
		short x;
		short y;
		short z;
		short w;
	};

	struct XModelTagPos
	{
		float x;
		float y;
		float z;
	}; // Load_Stream(1, v9, 16 * (varXModel->numBones + varXModel->numCosmeticBones - varXModel->numRootBones));

	union PackedUnitVec
	{
		uint packed;
	};

	union PackedUnitVecAndSign
	{
		uint packed;
	};

	struct GfxStreamVertex
	{
		uint8_t Color[4];

		uint16_t UVUPosition;
		uint16_t UVVPosition;

		int32_t VertexNormal;
		int32_t VertexTangent;
	};

	struct GfxStreamWeight
	{
		uint8_t WeightVal1;
		uint8_t WeightVal2;
		uint8_t WeightVal3;
		uint8_t WeightVal4;

		uint16_t WeightID1;
		uint16_t WeightID2;
		uint16_t WeightID3;
		uint16_t WeightID4;
	};

	struct GfxStreamFace
	{
		uint16_t Index1;
		uint16_t Index2;
		uint16_t Index3;
	};

	struct XSurfaceShared
	{
		uint32_t flags;
		uint32_t vertCount;
		uint32_t skinVertCount;
		uint32_t indexCount;
		byte* data;
		uint32_t dataSize;
		uint32_t posOffset;
		uint32_t vertsOffset;
		uint32_t indicesOffset;
		uint32_t skinWeightsOffset;
		void* vbPos;
		void* vbVerts;
		void* vbSkinWeights;
		void* ibIndices;
	};

	struct XSurfaceCollisionAabb
	{
		uint16_t mins[3];
		uint16_t maxs[3];
	};

	struct XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		uint16_t childBeginIndex;
		uint16_t childCount;
	};

	struct XSurfaceCollisionLeaf
	{
		uint16_t triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		vec3_t trans;
		vec3_t scale;
		uint32_t nodeCount;
		XSurfaceCollisionNode* nodes;
		uint32_t leafCount;
		XSurfaceCollisionLeaf* leafs;
	};

	struct XRigidVertList
	{
		unsigned __int16 boneOffset;
		unsigned __int16 vertCount;
		unsigned __int16 triOffset;
		unsigned __int16 triCount;
		XSurfaceCollisionTree* collisionTree;
	};

	enum XSurfaceFlags : uint16_t
	{
		XSURFACE_FLAG_SKINNED = 0x2,
		//XSURFACE_FLAG_CONSTANT_COLOR = 0x4,
		//XSURFACE_FLAG_SIEGE_SKIN = 0x8,
		XSURFACE_FLAG_STREAMED = 0x20,
		//XSURFACE_FLAG_UMBRA_OCCLUDER = 0x40,
		//XSURFACE_FLAG_DEFORMED = 0x80,
	};

	struct XSurface
	{
		byte tileMode;
		byte vertListCount;
		XSurfaceFlags flags;
		unsigned __int16 vertCount;
		unsigned __int16 triCount;
		uint32_t baseVertOffset;
		uint32_t baseIndexOffset;
		XSurfaceShared* shared;
		XRigidVertList* vertList;
		XPartBits partBits;
		uint64_t unk;
		const char* name;
	};

	struct XModelMesh
	{
		const char* name;
		uint nameHash;
		XPartBits partBits;
		byte numSurfs;
		float avgRenderTriArea;
		float avgCollisionTriArea;
		XPakEntryInfo xpakEntry;
		XSurface* surfs;
		XSurfaceShared* shared;
	}; assert_sizeof(XModelMesh, 0x78);

	struct XModelCollTri_s
	{
		vec4_t plane;
		vec4_t svec;
		vec4_t tvec;
	};

	typedef XModelCollTri_s XModelCollTri;

	typedef int16_t BoneIndex;

	struct XModelCollSurf_s
	{
		XModelCollTri* collTris;
		int numCollTris;
		vec3_t mins;
		vec3_t maxs;
		BoneIndex boneIdx;
		contents_t contents;
		int surfFlags;
	};

	typedef XModelCollSurf_s XModelCollSurf;

	struct cplane_s
	{
		vec3_t normal;
		float dist;
		byte type;
		byte signbits;
		byte pad[2];
	};

	typedef cplane_s cplane_t;

	struct cbrushside_t
	{
		cplane_t* plane;
		contents_t cflags;
		int sflags;
	};

	struct BrushWrapper
	{
		vec3_t mins;
		contents_t contents;
		vec3_t maxs;
		uint numsides;
		cbrushside_t* sides;
		contents_t axial_cflags[2][3];
		int axial_sflags[2][3];
		uint numverts;
		vec3_t* verts;
		cplane_t* planes;
	};

	struct PhysGeomInfo
	{
		BrushWrapper* brush;
		int type;
		vec3_t orientation[3];
		vec3_t offset;
		vec3_t halfLengths;
	};

	struct PhysGeomList
	{
		uint count;
		PhysGeomInfo* geoms;
		contents_t contents;
	};

	struct Collmap
	{
		PhysGeomList* geomList;
		BoneIndex boneIndex;
	};

	struct XSubmodel
	{
		XModel* model;
		ScrString_t boneName;
		vec3_t trans;
		vec3_t rot;
	};

	struct XModel
	{
		const char* name;
		byte numBones;
		byte numRootBones;
		uint16_t numCosmeticBones;
		ScrString_t* boneNames;
		byte* parentList;
		XModelAngle* tagAngles;
		XModelTagPos* tagPositions;
		byte* partClassification;
		DObjAnimMat* baseMat;
		byte numLods;
		XModelLod highestShadowLod;
		XModelLod highestRenderLod;
		float areaScale;
		float cullOutRadius;
		float unk[6];
		float averageTriArea[8];
		XModelMesh* meshes[8];
		XModelMeshMaterials* meshMaterials;
		XModelUsage usage;
		XModelCollSurf* collSurfs;
		int numCollSurfs;
		contents_t contents;
		XBoneInfo* boneInfo;
		float radius;
		vec3_t mins;
		vec3_t maxs;
		XModelMesh** collLod;
		byte numCollmaps;
		byte useMe;
		int flags;
		char __pad1[8];
		PhysPreset* physPreset;
		Collmap* collmaps;
		PhysConstraints* physConstraints;
		int numSubmodels;
		XSubmodel* submodels;
		uint nameHash;
		XPartBits combinedPartBits;
	}; assert_sizeof(XModel, 0x188);

	struct TriggerModel
	{
		contents_t contents;
		ushort hullCount;
		ushort firstHull;
	};

	struct TriggerHull
	{
		Bounds bounds;
		contents_t contents;
		ushort slabCount;
		ushort firstSlab;
	};

	struct TriggerSlab
	{
		vec3_t dir;
		float midPoint;
		float halfSize;
	};

	struct MapTriggers
	{
		uint count;
		TriggerModel* models;
		uint hullCount;
		TriggerHull* hulls;
		uint slabCount;
		TriggerSlab* slabs;
	};

	struct MapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars;
		MapTriggers trigger;
	};

	struct ClipMaterial
	{
		const char* name;
		int surfaceFlags;
		contents_t contentFlags;
	};

	struct cLeafBrushNodeLeaf_t
	{
		unsigned __int16* brushes;
	};

	struct cLeafBrushNodeChildren_t
	{
		float dist;
		float range;
		unsigned __int16 childOffset[2];
	};

	union cLeafBrushNodeData_t
	{
		cLeafBrushNodeLeaf_t leaf;
		cLeafBrushNodeChildren_t children;
	};

	struct cLeafBrushNode_s
	{
		byte axis;
		__int16 leafBrushCount;
		contents_t contents;
		cLeafBrushNodeData_t data;
	};
	typedef cLeafBrushNode_s cLeafBrushNode_t;

	struct cbrush_t
	{
		vec3_t mins;
		contents_t contents;
		vec3_t maxs;
		uint numsides;
		cbrushside_t* sides;
		contents_t axial_cflags[2][3];
		int axial_sflags[2][3];
		uint numverts;
		vec3_t* verts;
	};

	struct ClipInfo
	{
		int planeCount;
		cplane_t* planes;
		uint numMaterials;
		ClipMaterial* materials;
		uint numBrushSides;
		cbrushside_t* brushsides;
		uint leafbrushNodesCount;
		cLeafBrushNode_t* leafbrushNodes;
		uint numLeafBrushes;
		unsigned __int16* leafbrushes;
		uint numBrushVerts;
		vec3_t* brushVerts;
		uint nuinds;
		uint* uinds;
		unsigned __int16 numBrushes;
		cbrush_t* brushes;
		Bounds* brushBounds;
		contents_t* brushContents;
	};

	struct cStaticModelWritable
	{
		unsigned __int16 nextModelInWorldSector;
	};

	struct cStaticModel_s
	{
		cStaticModelWritable writable;
		XModel* xmodel;
		contents_t contents;
		vec3_t origin;
		vec3_t invScaledAxis[3];
		vec3_t absmin;
		vec3_t absmax;
		DObjAnimMat* boneMtxs;
		int numBoneMtxs;
		ScrString_t targetname;
	};
	typedef cStaticModel_s cStaticModel_t;

	struct cNode_t
	{
		cplane_t* plane;
		int children[2];
	};

	struct cLeaf_s
	{
		uint firstCollAabbIndex;
		uint collAabbCount;
		contents_t brushContents;
		contents_t terrainContents;
		vec3_t mins;
		vec3_t maxs;
		intptr_t_0 leafBrushNode;
		int cluster;
	};
	typedef cLeaf_s cLeaf_t;

	struct CollisionPartition
	{
		byte triCount;
		int firstTri;
		int nuinds;
		int fuind;
	};

	union CollisionAabbTreeIndex
	{
		int firstChildIndex;
		int partitionIndex;
	};

	struct CollisionAabbTree
	{
		vec3_t origin;
		unsigned __int16 materialIndex;
		unsigned __int16 childCount;
		vec3_t halfSize;
		CollisionAabbTreeIndex u;
	};

	struct cmodel_t
	{
		vec3_t mins;
		vec3_t maxs;
		float radius;
		ClipInfo* info;
		cLeaf_t leaf;
	};

	typedef unsigned __int16 DynEntityId;

	struct DynEntityDef
	{
		GfxPlacement pose;
		XModel* xModel;
		XModel* destroyedxModel;
		ushort brushModel;
		ushort physicsBrushModel;
		FxEffectDefHandle destroyFx;
		SndAliasId destroySound;
		FxEffectDefHandle rattleFx;
		SndAliasId rattleSound;
		FxEffectDefHandle splashUpFx[3];
		PhysPreset* physPreset;
		__int16 physConstraints[4];
		int health;
		int flags;
		contents_t contents;
		ScrString_t targetname;
		ScrString_t target;
	};

	struct GfxEntityPreFrame
	{
		uint activePartBits[12];
		int modelCount;
		int numBones;
		int numCosmeticBones;
		int boneArrayOffset;
	};

	struct DynEntityPose
	{
		GfxPlacement pose;
		float radius;
		GfxEntityPreFrame prevFrame;
	};

	struct LocalClientFxUniqueHandle
	{
		LocalClientNum_t_0 localClient;
		FxUniqueHandle fxHandle;
	};

	struct DynEntityClient
	{
		intptr_t_0 physObjId;
		ushort flags;
		int health;
		ushort burnTime;
		ushort fadeTime;
		int physicsStartTime;
		LocalClientFxUniqueHandle fxHandles[10];
		int numFxHandles;
	};

	struct DynEntityServer
	{
		ushort flags;
		int health;
	};

	struct DynEntityColl
	{
		ushort sector;
		ushort nextEntInSector;
		vec3_t linkMins;
		vec3_t linkMaxs;
		contents_t contents;
	};

	struct par_t
	{
		vec3_t p;
		vec3_t p0;
		vec3_t p_prev;
		int flags;
	};

	enum rope_constraint_e : __int32
	{
		ROPE_PAIR_CONSTRAINT = 0x0,
		ROPE_WORLD_CONSTRAINT = 0x1,
		ROPE_DENTITY_CONSTRAINT = 0x2,
		ROPE_CENTITY_CONSTRAINT = 0x3,
	};

	struct constraint_t
	{
		vec3_t p;
		rope_constraint_e type;
		int entity_index;
		ScrString_t bone_name_hash;
		byte pi1;
		byte pi2;
	};

	struct rope_frame_verts_t
	{
		int num_verts;
		vec3_t v[60];
	};

	struct rope_client_verts_t
	{
		rope_frame_verts_t frame_verts[2];
		unsigned int frame_index;
	};

	struct rope_t
	{
		par_t m_particles[30];
		constraint_t m_constraints[35];
		int m_entity_anchors[3];
		int m_num_particles;
		int m_num_constraints;
		int m_num_entity_anchors;
		int m_num_draw_verts;
		rope_client_verts_t m_client_verts;
		vec3_t m_min;
		vec3_t m_max;
		vec3_t m_start;
		vec3_t m_end;
		qboolean m_in_use;
		qboolean m_visible;
		qboolean m_dist_constraint;
		int m_flags;
		MaterialHandle m_material;
		float m_seglen;
		float m_length;
		float m_width;
		float m_scale;
		vec3_t m_force_scale;
		int m_health;
		int m_frame;
		int m_stable_count;
		int m_static_rope;
		int m_model;
		int m_slice_count;
	};

	struct clipMap_t
	{
		const char* name;
		qboolean isInUse;
		ClipInfo info;
		ClipInfo* pInfo;
		uint numStaticModels;
		cStaticModel_t* staticModelList;
		uint numNodes;
		cNode_t* nodes;
		uint numLeafs;
		cLeaf_t* leafs;
		uint vertCount;
		vec3_t* verts;
		int triCount;
		uint* triIndices;
		byte* triEdgeIsWalkable;
		int partitionCount;
		CollisionPartition* partitions;
		int aabbTreeCount;
		CollisionAabbTree* aabbTrees;
		uint numSubModels;
		cmodel_t* cmodels;
		int numClusters;
		int clusterBytes;
		byte* visibility;
		qboolean vised;
		MapEnts* mapEnts;
		cbrush_t* box_brush;
		cmodel_t box_model;
		DynEntityId originalDynEntCount;
		DynEntityId dynEntCount[4];
		DynEntityDef* dynEntDefList[2];
		DynEntityPose* dynEntPoseList[2];
		DynEntityClient* dynEntClientList[2];
		DynEntityServer* dynEntServerList[2];
		DynEntityColl* dynEntCollList[4];
		int num_constraints;
		PhysConstraint* constraints;
		int max_ropes;
		rope_t* ropes;
		ScrString_t miscModelTargetnameGroups[32];
		unsigned int checksum;
	};

	enum LightType : __int32
	{
		LIGHT_TYPE_NONE = 0x0,
		LIGHT_TYPE_DIR = 0x1,
		LIGHT_TYPE_SPOT = 0x2,
		LIGHT_TYPE_OMNI = 0x4,
		LIGHT_TYPE_ALL_MASK = 0x7,
	};

	enum ShadowQuality : __int32
	{
		SHADOW_QUALITY_NONE = 0x1,
		SHADOW_QUALITY_SST_ONLY = 0x2,
		SHADOW_QUALITY_HARD = 0x3,
		SHADOW_QUALITY_PCSS = 0x4,
		SHADOW_QUALITY_INVALID = 0x0,
	};

	enum ShadowUpdateType : __int32
	{
		SHADOW_UPDATE_ALWAYS_NOCACHE = 0x0,
		SHADOW_UPDATE_ALWAYS = 0x1,
		SHADOW_UPDATE_NEVER = 0x2,
	};

	enum ShadowResolutionScaleType : __int32
	{
		SHADOW_DS_4_0X = 0x0,
		SHADOW_DS_2_0X = 0x1,
		SHADOW_DS_1_0X = 0x2,
		SHADOW_DS_0_5X = 0x3,
		SHADOW_DS_COUNT = 0x4,
	};

	struct GfxConfig_Light
	{
		vec4_t nonSunShadowTransform[4];
		LightType type;
		uint guid;
		vec3_t origin;
		float dAttenuation;
		vec3_t _color;
		float specScale;
		float cut_on;
		float cut_off;
		float near_edge;
		float far_edge;
		bool mCullingUseCutOffRadius;
		float mCullingFalloff;
		float mCullingCutoff;
		ShadowQuality shadowQuality;
		bool probeOnly;
		bool mExcludeNeverUpdateFromDedicated;
		bool mShadowUseFxAxis;
		float pcss_min;
		float pcss_max;
		float mShadowPenumbraRadius;
		ShadowUpdateType shadowUpdate;
		vec3_t wldDir;
		vec4_t wldClipPlanes[6];
		float spotRotation;
		float cosHalfFov;
		float cosHalfCull;
		float roundness;
		vec4_t superellipse;
		float ortho_effect;
		float orthoDist;
		float finalCosHalfFov;
		float bulbRadius;
		vec4_t bulbDirAndLength;
		uint lightPriority;
		float cullRadius;
		uint shadowPriority;
		ShadowResolutionScaleType mShadowResolutionScale;
		uint lightStateMask;
		bool exploderDisabled;
		float exploderFade;
		bool volumetric;
		bool volumetricCookies;
		uint volumetricSampleCount;
		float volumetricIntensityScale;
		vec4_t cookieTransform[2];
		char lightDefName[64];
		float cookieAngle;
		float cookieRotation;
		vec2_t cookieScale;
		vec2_t cookieScroll;
		vec2_t cookieOffset;
		vec3_t wldCullMin;
		vec3_t wldCullMax;
		int umbraId;
		vec3_t mAxisFromFx[3];
	};


	struct ComPrimaryLight
	{
		GfxConfig_Light config;
		int cookieIndex;
	};

	struct ComLightTriggerData
	{
		int delayOn;
		int delayOff;
		int fadeIn;
		int fadeOut;
		ushort primaryLightIndex;
		bool reversed;
	};

	struct ComLightExploder
	{
		int nameHash;
		int triggerCount;
		ComLightTriggerData* triggerData;
	};

	struct ComProbeTriggerData
	{
		int probeID;
		int volumeID;
		int delayOn;
		int delayOff;
		int fadeIn;
		int fadeOut;
		bool reversed;
	};

	struct ComProbeExploder
	{
		int nameHash;
		int triggerCount;
		ComProbeTriggerData* triggerData;
	};

	struct ComUmbraVolume
	{
		vec3_t min;
		vec3_t max;
	};

	struct ComUmbraGate
	{
		const char* name;
		int umbraId;
		int gateIndex;
	};

	struct ComUmbraTome
	{
		const char* name;
		uint numVolumes;
		ComUmbraVolume* volumes;
		uint indicesSize;
		byte4* indicesSTQuery;
		byte4* indicesMTQuery;
		uint numGates;
		ComUmbraGate* gates;
	};

	struct ComWorld
	{
		const char* name;
		qboolean isInUse;
		uint primaryLightCount;
		ComPrimaryLight* primaryLights;
		uint lightExploderCount;
		ComLightExploder* lightExploders;
		uint sunVolumeCount;
		uint* sunVolumeProbeCount;
		uint probeCount;
		uint* probeGuids;
		uint probeExploderCount;
		ComProbeExploder* probeExploders;
		uint umbraTomeCount;
		ComUmbraTome* umbraTomes;
	};

	struct pathnode_t;

	struct pathnode_tree_t;
	typedef pathnode_tree_t* pathnode_tree_ptr;

	enum nodeType : __int32
	{
		NODE_BADNODE = 0x0,
		NODE_PATHNODE = 0x1,
		NODE_COVER_STAND = 0x2,
		NODE_COVER_CROUCH = 0x3,
		NODE_COVER_CROUCH_WINDOW = 0x4,
		NODE_COVER_PRONE = 0x5,
		NODE_COVER_RIGHT = 0x6,
		NODE_COVER_LEFT = 0x7,
		NODE_COVER_PILLAR = 0x8,
		NODE_AMBUSH = 0x9,
		NODE_EXPOSED = 0xA,
		NODE_CONCEALMENT_STAND = 0xB,
		NODE_CONCEALMENT_CROUCH = 0xC,
		NODE_CONCEALMENT_PRONE = 0xD,
		NODE_REACQUIRE = 0xE,
		NODE_BALCONY = 0xF,
		NODE_SCRIPTED = 0x10,
		NODE_NEGOTIATION_BEGIN = 0x11,
		NODE_NEGOTIATION_END = 0x12,
		NODE_TURRET = 0x13,
		NODE_GUARD = 0x14,
		NODE_NUMTYPES = 0x15,
		NODE_DONTLINK = 0x15,
	};

	struct pathlink_s
	{
		float fDist;
		unsigned __int16 nodeNum;
		byte disconnectCount;
		byte negotiationLink;
		byte flags;
		byte ubBadPlaceCount[5];
	};
	typedef pathlink_s pathlink_t;

	struct pathnode_constant_t
	{
		nodeType type;
		int spawnflags;
		int movementtype_ignore;
		int movementtype_require;
		ScrString_t targetname;
		ScrString_t script_linkName;
		ScrString_t script_noteworthy;
		ScrString_t target;
		ScrString_t animscript;
		scr_funcptr_t animscriptfunc;
		vec3_t vOrigin;
		float fAngle;
		vec2_t forward;
		__int16 wOverlapNode[4];
		ushort totalLinkCount;
		pathlink_t* Links;
		int navFaceIndex;
		int32_t navRegionIndex;
		int negotiationLinkedNodeIndex;
		float negotiationCostModifier;
		float width;
	};

	struct SentientHandle
	{
		ushort number;
		ushort infoIndex;
		uint32_t pad;
	};

	struct pathnode_dynamic_t
	{
		SentientHandle pOwner;
		int iFreeTime;
		int iValidTime[3];
		int dangerousNodeTime[3];
		int inPlayerLOSTime;
		__int16 wLinkCount;
		__int16 wOverlapCount;
		__int16 turretEntNumber;
		__int16 userCount;
		int flags;
	};

	struct pathnode_transient_t
	{
		int iSearchFrame;
		pathnode_t* pNextOpen;
		pathnode_t* pPrevOpen;
		pathnode_t* pParent;
		float fCost;
		float fHeuristic;
		union
		{
			float nodeCost;
			int linkIndex;
		};
	};

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
		pathnode_transient_t transient;
	};

	struct pathbasenode_t
	{
		vec3_t vOrigin;
		uint type;
	};

	struct pathnode_tree_nodes_t
	{
		int nodeCount;
		ushort* nodes;
	};

	union pathnode_tree_info_t
	{
		pathnode_tree_ptr child[2];
		pathnode_tree_nodes_t s;
	};

	struct pathnode_tree_t
	{
		int axis;
		vec_t dist;
		pathnode_tree_info_t u;
	};

	struct PathData
	{
		uint nodeCount;
		uint originalNodeCount;
		pathnode_t* nodes;
		pathbasenode_t* basenodes;
		int visBytes;
		byte* pathVis;
		int smoothBytes;
		byte* smoothCache;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
	};

	struct GameWorld
	{
		const char* name;
		PathData path;
	};

	struct GfxStreamingAabbTree
	{
		vec4_t mins;
		vec4_t maxs;
		float maxStreamingDistance;
		unsigned int firstItem;
		unsigned __int16 firstChild;
		unsigned __int16 childCount;
		unsigned __int16 smodelCount;
		unsigned __int16 surfaceCount;
	}; assert_sizeof(GfxStreamingAabbTree, 48);

	struct GfxWorldStreamInfo
	{
		int aabbTreeCount;
		GfxStreamingAabbTree* aabbTrees;
		int leafRefCount;
		int* leafRefs;
	}; assert_sizeof(GfxWorldStreamInfo, 0x20);

	struct GfxConfig_FogBank
	{
		float3 _fogcolor;
		float fogopacity;
		float fogintensity;
		float basedist;
		float halfdist;
		float baseheight;
		float halfheight;
		float skyhalfheightoffset;
		float probebakeworldfogdensityscaler;
		float3 _sunfogcolor;
		float sunfogopacity;
		float sunfogintensity;
		float sunfoginner;
		float sunfogouter;
		float sunPitchOffset;
		float sunYawOffset;
		float3 _atmospherefogcolor;
		float atmospherefogdensity;
		float3 _atmospherehazecolor;
		float atmospherehazebasedist;
		float atmospherehazefadedist;
		float atmospherehazedensity;
		float atmospherehazespread;
		float atmosphereinscatterstrength;
		float atmosphereextinctionstrength;
		float atmospheresunstrength;
		float atmospherepbramount;
		float worldfogskysize;
	};

	struct WorldSpawnConfig
	{
		//char name[64];
		float fogTransitionTime;
		GfxConfig_FogBank defaultFog;
		uint32_t expEvAuto;
		uint16_t numSpotShadowSlices;
		uint16_t numOmniShadowSlices;
	};

	struct GfxLightCorona
	{
		vec3_t origin;
		float radius;
		vec3_t color;
		float intensity;
	};

	struct GfxGlobalLightSettings
	{
		float pitch;
		float yaw;
		float3 color;
		float intensity;
		float specScale;
		float penumbra_inches;
		char skyboxmodel[64];
		bool shadow;
		char __pad0[24];
		float ev;
		float evcmp;
		float evmin;
		float evmax;
		float sunCookieIntensity;
		char sunCookieLightDefName[64];
		float sunCookieScale;
		float sunCookieAngle;
		float sunCookieRotation;
		float2 sunCookieOffset;
		float2 sunCookieScroll;
		uint sunCookieIndex;
		float sunVolumetricCookieIntensity;
	};

	struct GfxConfig_Sun
	{
		float shadowSplitDistance;
		float shadowBiasScale;
		float transitionTime;
		bool streamLighting;
		uint lightStateMask;
		float3 globalProbeCenter;
		float3 globalProbeFill;
		uint maxGridDimension;
		uint3 gridDimensions;
		GfxGlobalLightSettings settings[4];
	};

	struct GfxConfig_SST
	{
		vec4_t wldToPinTransform[4];
		float2 dimensionInTiles;
		float inchesPerTexel;
		float spanInInches;
		float yaw;
		float pitch;
	};

	struct GfxConfig_SSTMultiRes
	{
		uint mipDrop;
		uint rootOffsets[3];
		GfxConfig_SST uniformMip;
	};

	typedef int32_t StreamWrappedHandle;
	enum StreamWrappedType : __int8
	{
		SWT_PROBE_ARRAY = 0x0,
		SWT_PROBE_VOLUME = 0x1,
		SWT_SST = 0x2,
		SWT_UNCOMP_SIEGE = 0x3,
		SWT_SKY_BOX = 0x4,
	};
	enum StreamWrappedMode : __int8
	{
		SWM_RESIDENT = 0x0,
		SWM_STREAMED = 0x1,
	};

	struct StreamWrappedBuffer
	{
		StreamWrappedHandle handle;
		XPakEntryInfo xpakEntry;
		const char* name;
		StreamWrappedType type;
		StreamWrappedMode mode;
		uint dataSize;
		byte* data;
	};

	struct GfxBuffer
	{
		char pad[8];
	};

	typedef GfxBuffer GfxStructuredBuffer;

	struct GfxShadowTreeBuffer
	{
		byte* elements;
		uint32_t elementStrideBytes;
		uint32_t elementCount;
		GfxStructuredBuffer buffer;
	};

	struct GfxStreamedShadowTreeBuffer
	{
		StreamWrappedBuffer elements;
		uint32_t* mipByteOffsets;
		uint32_t* mipElementCounts;
		uint32_t totalElementCount;
		uint32_t mipCount;
		uint32_t mipDropTarget;
		uint32_t mipDropCurrent;
		GfxStructuredBuffer buffer;
		GfxShadowTreeBuffer* fallback;
	};

	typedef GfxStreamedShadowTreeBuffer* GfxStreamedShadowTreeBufferPtr;

	struct GfxMultiResSST
	{
		GfxConfig_SSTMultiRes config;
		GfxStreamedShadowTreeBufferPtr shadowTree;
	};

	typedef GfxImage* GfxImagePtr;

	struct GfxMinMaxTree
	{
		int mip0Width;
		int mip0Height;
		float2 sstTexScale;
		uint sstMinMaxCount;
		ushort* sstMaxCPU;
		GfxImagePtr sstMaxGPU;
		float treeMax;
	};

	struct GfxConfig_Probe
	{
		float3 origin;
		float3 rotation[3];
		float3 size_min;
		float3 size_max;
		float3 cullOrigin;
		float cullRadius;
		float resolution;
		uint grid_density;
		uint3 gridDim;
		float evcomp;
		float brightnessAdjust;
		float ao_range;
		float ao_strength;
		float ao_strength_double_sided;
		float ao_power;
		bool inpaint_occluded_rays;
		uint adaptQuality;
		uint planeCount;
		vec4_t wldClipPlanes[6];
		uint guid;
		bool isDynamic;
		bool isVolumetric;
		bool isOverride;
		uint debug_render_index_x;
		uint debug_render_index_y;
		uint debug_render_index_z;
		uint debug_render_phase;
		uint debugLineIndex;
	};

	struct GfxReflectionProbe
	{
		float exposure;
		vec3_t avgCubeColor;
		vec3_t volumeCoordMul;
		vec3_t volumeCoordAdd;
		uint16_t firstBlend;
		uint16_t numBlends;
		GfxConfig_Probe config;
		float exploderFade;
		bool exploderDisabled;
		int umbraId;
	};

	struct GfxConfig_ProbeBlend
	{
		float3 origin;
		float3 rotation[3];
		float3 size_min;
		float3 size_max;
		float3 blend_mins;
		float3 blend_maxs;
		uint faceCount;
		vec4_t faces[24];
		float blends[24];
		float3 cullOrigin;
		float cullRadius;
		float evcomp;
		bool negative;
	};

	struct GfxReflectionProbeArray
	{
		StreamWrappedBuffer* pixelData;
		GfxImagePtr probeImages;
		GfxReflectionProbe* configs;
		GfxConfig_ProbeBlend* blends;
		uint localReflectionProbeCount;
		uint probeBlendsCount;
	};

	struct GfxProbeVolumeTextures
	{
		GfxImagePtr X;
		GfxImagePtr Y;
		GfxImagePtr Z;
		StreamWrappedBuffer* pixelData;
	};

	typedef XModel* XModelPtr;

	struct GfxSkyBoxImage
	{
		GfxImagePtr image;
		StreamWrappedBuffer pixelData;
	};

	struct GfxSkyBox
	{
		XModelPtr model;
		float rotation;
		float size;
		GfxSkyBoxImage* image;
	};

	struct GfxExposureProbeBlend
	{
		vec3_t pos;
		vec3_t size;
		vec3_t blendMins;
		vec3_t blendMaxs;
		vec3_t angles;
		uint numFaces;
		vec4_t* faces;
		bool negative;
	};

	struct GfxExposureProbe
	{
		vec3_t pos;
		vec3_t size;
		vec3_t angles;
		float value;
		uint16_t dX;
		uint16_t dY;
		uint16_t dZ;
		uint16_t* pData;
		uint16_t numBlends;
		GfxExposureProbeBlend* blends;
	};

	struct GfxExposureGrid
	{
		uint32_t nProbe;
		float baseValue;
		GfxExposureProbe* pEprobe;
	};

	struct GfxVBLOOMGPU
	{
		vec4_t vRGB1;
		vec4_t vLUM1;
		vec4_t vRGB2;
		vec4_t vLUM2;
		vec4_t vRGB3;
		vec4_t vLUM3;
		vec4_t vRGB4;
		vec4_t vLUM4;
		vec4_t vRGB5;
		vec4_t vLUM5;
		vec4_t vLIB;
		vec4_t vLIW;
		vec4_t vLIG;
		vec4_t vLOB;
		vec4_t vLOW;
		vec4_t vMXR;
		vec4_t vMXG;
		vec4_t vMXB;
		char nLVI[32];
	};

	struct GfxVCOLORGPU
	{
		vec4_t vLumaM;
		vec4_t vRangeM;
		vec4_t vRangeA;
		vec4_t vMtxSR;
		vec4_t vMtxSG;
		vec4_t vMtxSB;
		vec4_t vMtxMR;
		vec4_t vMtxMG;
		vec4_t vMtxMB;
		vec4_t vMtxHR;
		vec4_t vMtxHG;
		vec4_t vMtxHB;
		vec4_t vMixR;
		vec4_t vMixG;
		vec4_t vMixB;
		char nLVI[32];
	};

	struct GfxSunVolume
	{
		int planeStart;
		int planeCount;
		uint lightExportFlags;
		uint lightingStateMask;
		GfxConfig_Sun sun;
		GfxMultiResSST multiResSST[4];
		GfxMinMaxTree sstMinMaxs[4];
		uint lightCount;
		ushort* lights;
		GfxReflectionProbeArray reflectionProbes[4];
		GfxProbeVolumeTextures probeVolumes[4];
		GfxSkyBox skyboxes[4];
		GfxExposureGrid exposureGrid[4];
		uint16_t idxLUT;
		uint16_t idxSKY;
		GfxVBLOOMGPU vbloomSUN[4];
		GfxVCOLORGPU vcolorSUN[4];
	}; assert_sizeof(GfxSunVolume, 4536);
	assert_offsetof(GfxSunVolume, multiResSST, 1056);

	struct GfxOutdoorForState
	{
		GfxConfig_SST sst;
		GfxStreamedShadowTreeBufferPtr outdoorTree;
		float pitch;
		float yaw;
	};

	struct GfxOutdoorVolume
	{
		int planeStart;
		int planeCount;
		GfxOutdoorForState state[4];
	};

	struct GfxExposureVolume
	{
		int priority;
		vec3_t angles;
		int planeStart;
		int planeCount;
		float blends[6];
		uint16_t evmin[4];
		uint16_t evmax[4];
		uint16_t evcmp[4];
		float exposure[4];
		float adaptation[4];
		uint8_t bankMask;
	};

	struct GfxWorldFogVolume
	{
		int planeStart;
		int planeCount;
		vec3_t mins;
		vec3_t maxs;
		float fogTransitionTime;
		uint32_t controlEx;
		uint lightingStateMask;
		char __pad0[4];
		GfxConfig_FogBank bank[4];
	};

	struct GfxWorldFogModifierVolume
	{
		uint32_t control;
		uint16_t minX;
		uint16_t minY;
		uint16_t minZ;
		uint16_t maxX;
		uint16_t maxY;
		uint16_t maxZ;
		uint32_t controlEx;
		float transitionTime;
		float depthScale;
		float heightScale;
		vec4_t colorAdjust;
	};

	struct GfxVolumePlane
	{
		vec4_t plane;
	};

	struct GfxLutVolume
	{
		vec3_t mins;
		uint32_t control;
		vec3_t maxs;
		float blends[6];
		float lutTransitionTime;
		uint32_t lutIndex;
	};

	struct GfxWeatherGrimeVolume
	{
		int planeStart;
		int planeCount;
		bool enabled;
		float rain;
		float windDirection;
		float windSpeed;
		float weatherPitch;
		float weatherYaw;
		float weatherTiling;
		vec3_t weatherTint;
		vec3_t weatherTint2;
		char __pad0[8];
		GfxImage* weatherGlossMap;
		GfxImage* weatherColorMap;
		GfxImage* weatherNormalMap;
		GfxImage* weatherGlossMap2;
		GfxImage* weatherColorMap2;
		GfxImage* weatherNormalMap2;
	}; assert_sizeof(GfxWeatherGrimeVolume, 120);
	assert_offsetof(GfxWeatherGrimeVolume, weatherGlossMap, 72);

	struct GfxGenericVolume
	{
		int type;
		int planeStart;
		int planeCount;
	};

	struct GfxConfig_LightAttenuationVolume
	{
		vec4_t planes[8];
		vec3_t color;
		uint planeCount;
	};

	struct GfxSkyDynamicIntensity
	{
		float angle0;
		float angle1;
		float factor0;
		float factor1;
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount;
		cplane_t* planes;
		uint* nodes;
		uint* sceneEntCellBits;
	};

	typedef ushort StaticModelIndex;

	struct GfxAabbTree
	{
		vec3_t mins;
		vec3_t maxs;
		ushort childCount;
		ushort surfaceCount;
		ushort startSurfIndex;
		ushort smodelIndexCount;
		StaticModelIndex* smodelIndexes;
		intptr_t_0 childrenOffset;
	};

	struct GfxPortal;
	struct GfxCell;

	struct GfxPortalWritable
	{
		bool isQueued;
		bool isAncestor;
		byte recursionDepth;
		byte hullPointCount;
		vec2_t* hullPoints;
		GfxPortal* queuedParent;
	};

	struct DpvsPlane
	{
		vec4_t coeffs;
		byte side[3];
		byte pad;
	};

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		GfxCell* cell;
		vec3_t* vertices;
		byte vertexCount;
		vec3_t hullAxis[2];
		vec3_t bounds[2];
	};

	struct GfxCell
	{
		vec3_t mins;
		vec3_t maxs;
		int aabbTreeCount;
		GfxAabbTree* aabbTree;
		int portalCount;
		GfxPortal* portals;
	};
	
	struct GfxVertexBuffer
	{
		char pad[8];
	};

	struct GfxIndexBuffer
	{
		char pad[8];
	};

	struct GfxWorldVertexData0
	{
		byte* data;
		GfxVertexBuffer vb;
	};

	struct GfxWorldVertexData1
	{
		byte* data;
		GfxVertexBuffer vb;
	};

	typedef uint16_t r_index_t;

	struct GfxWorldDraw
	{
		int cookieCount;
		GfxImagePtr cookieArray;
		uint vertexCount;
		uint vertexDataSize0;
		GfxWorldVertexData0 vd0;
		uint vertexDataSize1;
		GfxWorldVertexData1 vd1;
		int indexCount;
		r_index_t* indices;
		GfxIndexBuffer indexBuffer;
	};

	struct GfxBrushModelWritable
	{
		vec3_t mins;
		vec_t padding1;
		vec3_t maxs;
		int prevFrameBoneArrayOffset;
	};

	struct GfxBrushModel
	{
		GfxBrushModelWritable writable;
		vec3_t bounds[2];
		uint surfaceCount;
		uint startSurfIndex;
	};

	struct MaterialMemory
	{
		Material* material;
		int memory;
	};

	struct GfxBoneArrayInfo
	{
		int boneArrayOffset;
		int prevFrameBoneArrayOffset;
	};

	struct GfxSceneDynModel : GfxBoneArrayInfo
	{
	};

	typedef byte visdata_t;

	struct GfxVisArray
	{
		uint visDataCount;
		visdata_t* visData;
	};

	struct srfTriangles_t
	{
		vec3_t mins;
		int vertexDataOffset0;
		vec3_t maxs;
		int vertexDataOffset1;
		int firstVertex;
		vec_t himipRadiusInvSq;
		unsigned __int16 vertexCount;
		unsigned __int16 triCount;
		int baseIndex;
	};

	struct GfxSurface
	{
		srfTriangles_t tris;
		vec3_t bounds[2];
		Material* material;
		byte flags;
		byte decalSort;
	};

	struct GfxPackedPlacement
	{
		vec3_t origin;
		vec3_t axis[3];
		float scale;
	};

	struct GfxStaticModelBone
	{
		vec4_t m[3];
		vec4_t extra;
	};

	struct GfxStaticModelDrawInst
	{
		GfxPackedPlacement placement;
		vec3_t center;
		vec3_t mins;
		vec3_t maxs;
		XModel* model;
		uint16_t flags;
		int16_t umbraGateId;
		float invScaleSq;
		uint smid;
		ScrString_t targetname;
		byte hidden;
		GfxStaticModelBone* posedBones;
		uint numPosedBones;
		float scaledRadius;
		union
		{
			//GfxSiegeAnimState* sanimPlayback;
			//GfxSiegeAnim* sanim;
			void* sanim;
		};
		XModelLod cachedLod[4];
	};

	struct GfxWorldDpvsStatic
	{
		int smodelUpdateFrame;
		uint smodelCount;
		uint staticSurfaceCount;
		uint gbufferSurfsBegin;
		uint gbufferSurfsEnd;
		uint gbufferDecalSurfsBegin;
		uint gbufferDecalSurfsEnd;
		uint litForwardSurfsBegin;
		uint litForwardSurfsEnd;
		uint litTransSurfsBegin;
		uint litTransSurfsEnd;
		GfxVisArray smodelVisData[4];
		GfxVisArray surfaceVisData[4];
		GfxVisArray volDecalVisData[4];
		GfxVisArray smodelVisDataCameraSaved;
		GfxVisArray surfaceVisDataCameraSaved;
		ushort* sortedSurfIndex;
		ushort* umbraIdToSmodelIndex;
		uint semiStaticModelCount;
		uint fullyStaticModelCount;
		ushort* semiStaticModelList;
		ushort* fullyStaticModelList;
		GfxSurface* surfaces;
		GfxStaticModelDrawInst* smodelDrawInsts;
		GfxSortKey* surfaceMaterials;
		GfxVisArray surfaceCastsSunShadow;
		GfxVisArray surfaceCastsShadow;
		GfxVisArray smodelCastsShadow;
	}; assert_sizeof(GfxWorldDpvsStatic, 0x180);

	struct GfxVisDynamic
	{
		GfxVisArray visArray[2];
	};

	struct GfxWorldDpvsDynamic
	{
		uint dynEntClientCount[2];
		GfxVisDynamic dynEntVisData[4];
		GfxVisDynamic umbraDynEntVisData;
	};

	struct Occluder
	{
		uint flags;
		char name[16];
		vec3_t points[4];
	};

	struct GfxUmbraParameterVolume
	{
		vec3_t min;
		vec3_t max;
		int priority;
		float distanceScale;
		float accurateOcclusionThreshold;
		int minimumContributionThreshold;
	};

	struct GfxUmbraTome
	{
		uint tomeSize;
		byte16* tome;
		void* tomePtr;
		uint numParameterVolumes;
		GfxUmbraParameterVolume* parameterVolumes;
	};

	struct SiegeAutoPlay
	{
		uint smid;
		uint worldSanimIndex;
		float timeOffset;
		ScrString_t shotName;
		uint16_t flags;
	};

	struct GfxCameraLens
	{
		uint32_t cFlags;
		float minfocalLength;
		float maxfocalLength;
		float minfstop;
		float maxfstop;
		float minfocusDistance;
		float maxfocusDistance;
		float opticalLength;
		float abbe;
		float distortionK1;
		float distortionK2;
		float distortionK3;
		float distortionKS;
		char name[32];
	};

	struct GfxConfig_LitFogBank
	{
		float3 extcolor;
		float4 suncoloroverride;
		float basedist;
		float halfdist;
		float baseheight;
		float halfheight;
		float probebakelitfogdensityscaler;
		float distribution;
		float albedo;
		float densityScaler;
		float probeContributionScaler;
		float sunintensityscale;
		float maxlitsunfogdistance;
		float maxlitomnispotfogdistance;
	};

	struct GfxLitFogVolume
	{
		vec3_t mins;
		vec3_t maxs;
		vec3_t ambientColor;
		uint16_t control;
		uint16_t priority;
		uint scriptid;
		float fogtime;
		char __pad0[8];
		GfxConfig_LitFogBank bank[4];
	};

	struct GfxVolumeDecal
	{
		vec3_t localToWld[4];
		vec3_t wldToLocal[4];
		vec3_t halfExtents;
		vec3_t edgeFeather;
		vec4_t revealTextureUV;
		vec4_t uvBaseAndScale;
		MaterialHandle material;
		uint decalSort;
		float boundSphereRadius;
		float himipInvSqRadius;
		ScrString_t targetName;
		bool hidden;
	};

	struct GfxWorldWaterDisk
	{
		int totalWaterCount;
		int interactiveWaveWaterCount;
		void* interactiveWaveWaters; //GfxInteractiveWaveWaterDisk* interactiveWaveWaters;
		int gridMemSize;
		int gridCount;
		byte* gridMem;
	};

	struct GfxWorld
	{
		const char* name;
		const char* baseName;
		int planeCount;
		int nodeCount;
		int surfaceCount;
		GfxWorldStreamInfo streamInfo;
		WorldSpawnConfig worldSpawnConfig;
		uint coronaCount;
		GfxLightCorona* coronas;
		uint sunVolumeCount;
		GfxSunVolume* sunVolumes;
		uint sunVolumePlanesCount;
		vec4_t* sunVolumePlanes;
		uint outdoorVolumeCount;
		GfxOutdoorVolume* outdoorVolumes;
		uint outdoorVolumePlanesCount;
		vec4_t* outdoorVolumePlanes;
		uint exposureVolumeCount;
		GfxExposureVolume* exposureVolumes;
		uint exposureVolumePlaneCount;
		vec4_t* exposureVolumePlanes;
		uint worldFogVolumeCount;
		GfxWorldFogVolume* worldFogVolumes;
		uint worldFogVolumePlaneCount;
		vec4_t* worldFogVolumePlanes;
		uint worldFogModifierVolumeCount;
		GfxWorldFogModifierVolume* worldFogModifierVolumes;
		uint worldFogModifierVolumePlaneCount;
		GfxVolumePlane* worldFogModifierVolumePlanes;
		uint lutVolumeCount;
		GfxLutVolume* lutVolumes;
		uint lutVolumePlaneCount;
		GfxVolumePlane* lutVolumePlanes;
		uint weatherGrimeVolumeCount;
		GfxWeatherGrimeVolume* weatherGrimeVolumes;
		uint weatherGrimeVolumePlaneCount;
		vec4_t* weatherGrimeVolumePlanes;
		uint genericVolumeCount;
		GfxGenericVolume* genericVolumes;
		uint genericVolumePlaneCount;
		vec4_t* genericVolumePlanes;
		uint unkCount;
		void* unk;
		uint unk2Count;
		vec4_t* unk2;
		uint attenuationVolumeCount;
		GfxConfig_LightAttenuationVolume* attenuationVolumes;
		GfxSkyDynamicIntensity skyDynIntensity;
		GfxWorldDpvsPlanes dpvsPlanes;
		int cellBitsCount;
		GfxCell* cells;
		GfxWorldDraw draw;
		int modelCount;
		GfxBrushModel* models;
		vec3_t mins;
		vec3_t maxs;
		unsigned int checksum;
		int materialMemoryCount;
		MaterialMemory* materialMemory;
		uint* cellCasterBits;
		GfxSceneDynModel* sceneDynModel;
		GfxWorldDpvsStatic dpvs;
		GfxWorldDpvsDynamic dpvsDyn;
		const Material* coronaMaterial;
		const Material* ropeMaterial;
		const Material* lutMaterial;
		const Material* lutMaterialALT;
		uint numOccluders;
		Occluder* occluders;
		uint lightingFlags;
		int lightingQuality;
		int umbraNumTomes;
		GfxUmbraTome* umbraTomes;
		int umbraNumTomes2;
		GfxUmbraTome* umbraTomes2;
		bool umbraTomeIncludesLightsAndFx;
		uint numSiegeAnims;
		void* siegeAnims; //GfxSiegeAnimPtr* siegeAnims;
		uint numSiegeAnimsAutoPlay;
		SiegeAutoPlay* siegeAnimsAutoPlay;
		uint lensPackCount;
		GfxCameraLens* lensPackData;
		uint litFogVolumeCount;
		GfxLitFogVolume* litFogVolumes;
		uint unk3Count;
		void* unk3;
		uint volumeDecalCount;
		GfxVolumeDecal* volumeDecals;
		GfxImagePtr volumeDecalRevealTexture;
		GfxWorldWaterDisk water;
		char __pad0[6714];
		//GfxVATLASUKKOGPU vukkoLVX[32];
		//uint baseGfxVUKKO[4];
		//uint nGfxModOverVUKKO;
		//GfxModVUKKO* pGfxModOverVUKKO;
		//uint nGfxBoxOverVUKKO;
		//GfxBoxVUKKO* pGfxBoxOverVUKKO;
		//uint nGfxBoxBaseVUKKO;
		//GfxBoxVUKKO* pGfxBoxBaseVUKKO;
		//GfxVBLOOMGPU baseGfxVBLOOMGPU[4];
		//uint nGfxModOverVBLOOM;
		//GfxModVBLOOM* pGfxModOverVBLOOM;
		//uint nGfxBoxOverVBLOOM;
		//GfxBoxVBLOOM* pGfxBoxOverVBLOOM;
		//uint nGfxBoxBaseVBLOOM;
		//GfxBoxVBLOOM* pGfxBoxBaseVBLOOM;
		//uint baseGfxVLUT[4];
		//uint nGfxModOverVLUT;
		//GfxModVLUT* pGfxModOverVLUT;
		//uint nGfxBoxOverVLUT;
		//GfxBoxVLUT* pGfxBoxOverVLUT;
		//uint nGfxBoxBaseVLUT;
		//GfxBoxVLUT* pGfxBoxBaseVLUT;
		//GfxVCOLORGPU baseGfxVCOLORGPU[4];
		//uint nGfxModOverVCOLOR;
		//GfxModVCOLOR* pGfxModOverVCOLOR;
		//uint nGfxBoxOverVCOLOR;
		//GfxBoxVCOLOR* pGfxBoxOverVCOLOR;
		//uint nGfxBoxBaseVCOLOR;
		//GfxBoxVCOLOR* pGfxBoxBaseVCOLOR;
		//uint nGfxBoxOverUNDERWATER;
		//GfxBoxUNDERWATER* pGfxBoxOverUNDERWATER;
		//uint nGfxBoxBaseUNDERWATER;
		//GfxBoxUNDERWATER* pGfxBoxBaseUNDERWATER;
	}; assert_sizeof(GfxWorld, 0x2040);
	assert_offsetof(GfxWorld, streamInfo, 32);
	assert_offsetof(GfxWorld, coronas, 224);
	assert_offsetof(GfxWorld, lutVolumes, 400);
	assert_offsetof(GfxWorld, genericVolumes, 464);
	assert_offsetof(GfxWorld, genericVolumePlanes, 480);
	assert_offsetof(GfxWorld, unk, 496);
	assert_offsetof(GfxWorld, unk2, 512);
	assert_offsetof(GfxWorld, umbraTomes, 1376);
	assert_offsetof(GfxWorld, unk3, 1472);

	struct GfxLightDef
	{
		const char* name;
		float angle;
		float rotation;
		float2 offset;
		float2 scroll;
		GfxImage* gfxImage;
	};

	union XAssetHeader
	{
		PhysPreset* physPreset;
		PhysConstraints* physConstraints;
		//DestructibleDef* destructibleDef;
		XAnimParts* parts;
		XModel* model;
		XModelMesh* modelMesh;
		Material* material;
		MaterialComputeShaderSet* computeShaderSet;
		MaterialTechniqueSet* techniqueSet;
		GfxImage* image;
		//SndBank* sound;
		//SndPatch* soundPatch;
		clipMap_t* clipMap;
		ComWorld* comWorld;
		GameWorld* gameWorld;
		MapEnts* mapEnts;
		GfxWorld* gfxWorld;
		GfxLightDef* lightDef;
		//GfxLensFlareDef* lensFlareDef;
		//Font* font;
		//FontIcon* fontIcon;
		//LocalizeEntry* localize;
		//WeaponVariantDef* weapon;
		//WeaponAttachment* attachment;
		//WeaponAttachmentUnique* attachmentUnique;
		//WeaponCamo* weaponCamo;
		//CustomizationTable* customizationTable;
		//CustomizationColorInfo* customizationColorInfo;
		//SndDriverGlobals* sndDriverGlobals;
		//FxEffectDefHandleRaw fx;
		//TagFxSet* tagFX;
		//FxLensFlareDefPtr newLensFlareDef;
		//FxImpactTable* impactFx;
		//SoundsImpactTable* impactSounds;
		//CgMediaTable* cgMedia;
		//PlayerSoundsTable* playerSounds;
		//PlayerFXTable* playerFX;
		//SharedWeaponSounds* sharedWeaponSounds;
		//RawFile* rawfile;
		//StringTable* stringTable;
		//StructuredTable* structuredTable;
		//LeaderboardDef* leaderboardDef;
		//DDLRoot* ddlRoot;
		//Glasses* glasses;
		//TextureList* textureList;
		//ScriptParseTree* scriptParseTree;
		//KeyValuePairs* keyValuePairs;
		//VehicleDef* vehicleDef;
		//AddonMapEnts* addonMapEnts;
		//TracerDef* tracerDef;
		//Qdb* qdb;
		//Slug* slug;
		//SurfaceFXTableDef* surfaceFXTableDef;
		//SurfaceSoundDef* surfaceSoundDef;
		//FootstepTableDef* footstepTableDef;
		//EntitySoundImpacts* entitySoundImpacts;
		//EntityFxImpacts* entityFxImpacts;
		//ZBarrierDef* zbarrierDef;
		//VehicleFxDef* vehicleFxDef;
		//VehicleSoundDef* vehicleSoundDef;
		//ArchiveTypeInfoArray* typeInfo;
		//ScriptBundle* scriptBundle;
		//ScriptBundleList* scriptBundleList;
		//RumbleInfo* rumble;
		//BulletPenetrationTable* bulletPenetration;
		//LocDmgTable* locDmgTable;
		//AimTable* aimTable;
		//XModelAlias* xModelAlias;
		//Character* character;
		//AIType* aiType;
		//PlayerCharacter* player_character;
		//AnimSelectorTableSet* animSelectorTableSet;
		//AnimMappingTable* animMappingTable;
		//AnimStateMachine* animStateMachine;
		//BehaviorTree* behaviorTree;
		//BehaviorStateMachine* behaviorStateMachine;
		//TTFDef* ttfDef;
		//GfxSiegeAnim* sanim;
		//GfxLightDescription* lightDescription;
		//ShellshockParams* shellshock;
		//XCam* xcam;
		//BGCacheInfo* bgCache;
		//TextureCombo* textureCombo;
		//FlameTable* flameTable;
		//Bitfield* bitfield;
		//AttachmentCosmeticVariant* attachmentCosmeticVariant;
		//MapTable* mapTable;
		//Medal* medal;
		//MedalTable* medalTable;
		//Objective* objective;
		//ObjectiveList* objectiveList;
		//NavMeshData* navMesh;
		//NavVolumeData* navVolume;
		//BinaryHTML* binaryHTML;
		//LaserDef* laserDef;
		//BeamDef* beamDef;
		//StreamerHint* streamerHint;
		void* data;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct ScriptStringList
	{
		int count;
		const char** strings;
	};

	struct XAssetList
	{
		ScriptStringList stringList;
		int dependCount;
		const char** depends;
		int assetCount;
		XAsset* assets;
	};

	enum XFILE_BLOCK : int32_t
	{
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x1,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x2,
		XFILE_BLOCK_DELAY_VIRTUAL = 0x3,
		XFILE_BLOCK_DELAY_PHYSICAL = 0x4,
		XFILE_BLOCK_VIRTUAL = 0x5,
		XFILE_BLOCK_PHYSICAL = 0x6,
		XFILE_BLOCK_STREAMER_RESERVE = 0x7,
		XFILE_BLOCK_STREAMER = 0x8,
		XFILE_BLOCK_MEMMAPPED = 0x9,
		MAX_XFILE_COUNT = 0xA,
	};

	struct XZoneInfo
	{
		const char* name;
		uint64_t allocFlags;
		uint64_t freeFlags;
		uint64_t allocSlot;
		uint64_t freeSlot;
	};

	struct XBlock
	{
		byte* data;
		uint64_t size;
	};

	struct XZone
	{
		unsigned __int16 index;
		int flags;
		int slot;
		XBlock blocks[10];
	};

	enum XZoneState : uint32_t
	{
		XZONE_UNLOADING = 0xFFFFFFFF,
		XZONE_EMPTY = 0x0,
		XZONE_LOADING = 0x1,
		XZONE_LOADED = 0x2,
		XZONE_COMPLETE = 0x3,
		XZONE_FAILED = 0x4,
	};

	struct XZoneName
	{
		char name[64];
		int flags;
		int slot;
		int unk1;
		int pad;
		void* unk2;
		XZoneState state;
		bool streamPreloaded;
	};

	struct XAssetEntry
	{
		XAsset asset;
		byte zoneIndex;
		bool inuse;
		uint32_t nextHash;
		uint32_t nextType;
		uint32_t nextOverride;
	};
}