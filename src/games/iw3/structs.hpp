#pragma once
#pragma warning(disable: 4324)

namespace zonetool::iw3
{
	enum XAssetType : std::int32_t
	{
		ASSET_TYPE_XMODELPIECES = 0x0,
		ASSET_TYPE_PHYSPRESET = 0x1,
		ASSET_TYPE_XANIMPARTS = 0x2,
		ASSET_TYPE_XMODEL = 0x3,
		ASSET_TYPE_MATERIAL = 0x4,
		ASSET_TYPE_TECHNIQUE_SET = 0x5,
		ASSET_TYPE_IMAGE = 0x6,
		ASSET_TYPE_SOUND = 0x7,
		ASSET_TYPE_SOUND_CURVE = 0x8,
		ASSET_TYPE_LOADED_SOUND = 0x9,
		ASSET_TYPE_CLIPMAP = 0xA,
		ASSET_TYPE_CLIPMAP_PVS = 0xB,
		ASSET_TYPE_COMWORLD = 0xC,
		ASSET_TYPE_GAMEWORLD_SP = 0xD,
		ASSET_TYPE_GAMEWORLD_MP = 0xE,
		ASSET_TYPE_MAP_ENTS = 0xF,
		ASSET_TYPE_GFXWORLD = 0x10,
		ASSET_TYPE_LIGHT_DEF = 0x11,
		ASSET_TYPE_UI_MAP = 0x12,
		ASSET_TYPE_FONT = 0x13,
		ASSET_TYPE_MENULIST = 0x14,
		ASSET_TYPE_MENU = 0x15,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
		ASSET_TYPE_WEAPON = 0x17,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x18,
		ASSET_TYPE_FX = 0x19,
		ASSET_TYPE_IMPACT_FX = 0x1A,
		ASSET_TYPE_AITYPE = 0x1B,
		ASSET_TYPE_MPTYPE = 0x1C,
		ASSET_TYPE_CHARACTER = 0x1D,
		ASSET_TYPE_XMODELALIAS = 0x1E,
		ASSET_TYPE_RAWFILE = 0x1F,
		ASSET_TYPE_STRINGTABLE = 0x20,
		ASSET_TYPE_COUNT = 0x21,
	};

	enum XFileBlock
	{
		XFILE_BLOCK_TEMP,
		XFILE_BLOCK_RUNTIME,
		XFILE_BLOCK_LARGE_RUNTIME,
		XFILE_BLOCK_PHYSICAL_RUNTIME,
		XFILE_BLOCK_VIRTUAL,
		XFILE_BLOCK_LARGE,
		XFILE_BLOCK_PHYSICAL,
		XFILE_BLOCK_VERTEX,
		XFILE_BLOCK_INDEX,

		MAX_XFILE_COUNT
	};

	struct XModelPieces;
	struct PhysPreset;
	struct XAnimParts;
	struct XModel;
	struct Material;
	struct MaterialPixelShader;
	struct MaterialVertexShader;
	struct MaterialTechniqueSet;
	struct GfxImage;
	struct snd_alias_list_t;
	struct SndCurve;
	struct LoadedSound;
	struct clipMap_t;
	struct ComWorld;
	struct GameWorldSp;
	struct GameWorldMp;
	struct MapEnts;
	struct GfxWorld;
	struct GfxLightDef;
	struct Font_s;
	struct MenuList;
	struct menuDef_t;
	struct LocalizeEntry;
	struct WeaponDef;
	struct SndDriverGlobals;
	struct FxEffectDef;
	struct FxImpactTable;
	struct RawFile;
	struct StringTable;

	typedef unsigned short ScriptString;
	typedef char raw_byte16;

	union XAssetHeader
	{
		// XModelPieces *xmodelPieces; // NOT AN ASSET
		x86::pointer<PhysPreset> physPreset;
		x86::pointer<XAnimParts> parts;
		x86::pointer<XModel> model;
		x86::pointer<Material> material;
		x86::pointer<MaterialPixelShader> pixelShader;
		x86::pointer<MaterialVertexShader> vertexShader;
		x86::pointer<MaterialTechniqueSet> techniqueSet;
		x86::pointer<GfxImage> image;
		x86::pointer<snd_alias_list_t> sound;
		x86::pointer<SndCurve> sndCurve;
		x86::pointer<LoadedSound> loadSnd;
		x86::pointer<clipMap_t> clipMap;
		x86::pointer<ComWorld> comWorld;
		x86::pointer<GameWorldSp> gameWorldSp;
		x86::pointer<GameWorldMp> gameWorldMp;
		x86::pointer<MapEnts> mapEnts;
		x86::pointer<GfxWorld> gfxWorld;
		x86::pointer<GfxLightDef> lightDef;
		x86::pointer<Font_s> font;
		x86::pointer<MenuList> menuList;
		x86::pointer<menuDef_t> menu;
		x86::pointer<LocalizeEntry> localize;
		x86::pointer<WeaponDef> weapon;
		x86::pointer<SndDriverGlobals> sndDriverGlobals;
		x86::pointer<FxEffectDef> fx;
		x86::pointer<FxImpactTable> impactFx;
		x86::pointer<RawFile> rawfile;
		x86::pointer<StringTable> stringTable;
		x86::pointer<void> data;
	};

	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	typedef char cbrushedge_t;
	typedef unsigned int raw_uint128;




	struct XModelPiece
	{
		x86::pointer<XModel> model;
		float offset[3];
	};

	struct XModelPieces
	{
		x86::pointer<const char> name;
		int numpieces;
		x86::pointer<XModelPiece> pieces;
	};

	struct PhysPreset
	{
		x86::pointer<const char> name;
		int type;
		float mass;
		float bounce;
		float friction;
		float bulletForceScale;
		float explosiveForceScale;
		x86::pointer<const char> sndAliasPrefix;
		float piecesSpreadFraction;
		float piecesUpwardVelocity;
		bool tempDefaultToCylinder;
	};

	struct PhysPresetInfo
	{
		float mass;
		float bounce;
		float friction;
		int isFrictionInfinity;
		float bulletForceScale;
		float explosiveForceScale;
		x86::pointer<const char> sndAliasPrefix;
		float piecesSpreadFraction;
		float piecesUpwardVelocity;
		int tempDefaultToCylinder;
	};

	union XAnimIndices
	{
		x86::pointer<unsigned char> _1;
		x86::pointer<uint16_t> _2;
		x86::pointer<void> data;
	};

	struct XAnimNotifyInfo
	{
		ScriptString name;
		float time;
	};

	typedef unsigned char ByteVec[3];
	typedef unsigned short UShortVec[3];

	union XAnimDynamicIndicesTrans
	{
		unsigned char _1[1];
		uint16_t _2[1];
	};

	union XAnimDynamicFrames
	{
		x86::pointer<ByteVec> _1;
		x86::pointer<UShortVec> _2;
	};

	struct XAnimPartTransFrames
	{
		vec3_t mins;
		vec3_t size;
		XAnimDynamicFrames frames;
		XAnimDynamicIndicesTrans indices;
	};

	union XAnimPartTransData
	{
		XAnimPartTransFrames frames;
		vec3_t frame0;
	};

	struct XAnimPartTrans
	{
		uint16_t size;
		unsigned char smallTrans;
		XAnimPartTransData u;
	};

	struct alignas(4) XQuat
	{
		int16_t value[4];
	};

	struct alignas(4) XQuat2
	{
		int16_t value[2];
	};

	union XAnimDynamicIndicesQuat
	{
		unsigned char _1[1];
		uint16_t _2[1];
	};

	struct XAnimDeltaPartQuatDataFrames
	{
		x86::pointer<XQuat2> frames;
		XAnimDynamicIndicesQuat indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		XQuat2 frame0;
	};

	struct XAnimDeltaPartQuat
	{
		uint16_t size;
		XAnimDeltaPartQuatData u;
	};

	struct XAnimDeltaPart
	{
		x86::pointer<XAnimPartTrans> trans;
		x86::pointer<XAnimDeltaPartQuat> quat;
	};

	enum XAnimPartType
	{
		PART_TYPE_NO_QUAT = 0x0,
		PART_TYPE_HALF_QUAT = 0x1,
		PART_TYPE_FULL_QUAT = 0x2,
		PART_TYPE_HALF_QUAT_NO_SIZE = 0x3,
		PART_TYPE_FULL_QUAT_NO_SIZE = 0x4,
		PART_TYPE_SMALL_TRANS = 0x5,
		PART_TYPE_TRANS = 0x6,
		PART_TYPE_TRANS_NO_SIZE = 0x7,
		PART_TYPE_NO_TRANS = 0x8,
		PART_TYPE_ALL = 0x9,

		PART_TYPE_COUNT
	};

	struct XAnimParts
	{
		x86::pointer<const char> name;
		uint16_t dataByteCount;
		uint16_t dataShortCount;
		uint16_t dataIntCount;
		uint16_t randomDataByteCount;
		uint16_t randomDataIntCount;
		uint16_t numframes;
		bool bLoop;
		bool bDelta;
		unsigned char boneCount[PART_TYPE_COUNT];
		unsigned char notifyCount;
		unsigned char assetType;
		bool isDefault;
		unsigned int randomDataShortCount;
		unsigned int indexCount;
		float framerate;
		float frequency;
		x86::pointer<ScriptString> names;
		x86::pointer<unsigned char> dataByte;
		x86::pointer<int16_t> dataShort;
		x86::pointer<int> dataInt;
		x86::pointer<int16_t> randomDataShort;
		x86::pointer<unsigned char> randomDataByte;
		x86::pointer<int> randomDataInt;
		XAnimIndices indices;
		x86::pointer<XAnimNotifyInfo> notify;
		x86::pointer<XAnimDeltaPart> deltaPart;
	};

	struct DObjSkelMat
	{
		float axis[3][4];
		float origin[4];
	};

	struct DObjAnimMat
	{
		vec4_t quat;
		vec3_t trans;
		float transWeight;
	};

	struct XSurfaceCollisionAabb
	{
		uint16_t mins[3];
		uint16_t maxs[3];
	};

	struct alignas(16) XSurfaceCollisionNode
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
		unsigned int nodeCount;
		x86::pointer<XSurfaceCollisionNode> nodes;
		unsigned int leafCount;
		x86::pointer<XSurfaceCollisionLeaf> leafs;
	};

	struct XRigidVertList
	{
		uint16_t boneOffset;
		uint16_t vertCount;
		uint16_t triOffset;
		uint16_t triCount;
		x86::pointer<XSurfaceCollisionTree> collisionTree;
	};

	union GfxColor
	{
		unsigned int packed;
		char array[4];
	};

	union PackedTexCoords
	{
		unsigned int packed;
	};

	union PackedUnitVec
	{
		unsigned int packed;
		char array[4];
	};

	struct alignas(16) GfxPackedVertex
	{
		vec3_t xyz;
		float binormalSign;
		GfxColor color;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct XSurfaceVertexInfo
	{
		int16_t vertCount[4];
		x86::pointer<uint16_t> vertsBlend;
	};

	struct XSurfaceTri
	{
		uint16_t i[3];
	};

	typedef XSurfaceTri XSurfaceTri16;

	struct XSurface
	{
		char tileMode;
		bool deformed;
		uint16_t vertCount;
		uint16_t triCount;
		char zoneHandle;
		uint16_t baseTriIndex;
		uint16_t baseVertIndex;
		x86::pointer<XSurfaceTri16> triIndices;
		XSurfaceVertexInfo vertInfo;
		x86::pointer<GfxPackedVertex> verts0;
		unsigned int vertListCount;
		x86::pointer<XRigidVertList> vertList;
		int partBits[4];
	};

	struct XModelLodInfo
	{
		float dist;
		uint16_t numsurfs;
		uint16_t surfIndex;
		int partBits[4];
		char lod;
		char smcIndexPlusOne;
		char smcAllocBits;
		char unused;
	};

	struct XModelCollTri_s
	{
		float plane[4];
		float svec[4];
		float tvec[4];
	};

	struct XModelCollSurf_s
	{
		x86::pointer<XModelCollTri_s> collTris;
		int numCollTris;
		float mins[3];
		float maxs[3];
		int boneIdx;
		int contents;
		int surfFlags;
	};

	struct XBoneInfo
	{
		vec3_t bounds[2];
		vec3_t offset;
		float radiusSquared;
	};

	struct cplane_s
	{
		float normal[3];
		float dist;
		char type;
		char signbits;
		char pad[2];
	};

	struct cbrushside_t
	{
		x86::pointer<cplane_s> plane;
		unsigned int materialNum;
		int16_t firstAdjacentSideOffset;
		char edgeCount;
	};

	struct BrushWrapper
	{
		float mins[3];
		int contents;
		float maxs[3];
		unsigned int numsides;
		x86::pointer<cbrushside_t> sides;
		int16_t axialMaterialNum[2][3];
		x86::pointer<cbrushedge_t> baseAdjacentSide;
		int16_t firstAdjacentSideOffsets[2][3];
		char edgeCount[2][3];
		int totalEdgeCount;
		x86::pointer<cplane_s> planes;
	};

	struct PhysGeomInfo
	{
		x86::pointer<BrushWrapper> brush;
		int type;
		float orientation[3][3];
		float offset[3];
		float halfLengths[3];
	};

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
	};

	struct PhysGeomList
	{
		unsigned int count;
		x86::pointer<PhysGeomInfo> geoms;
		PhysMass mass;
	};

	struct XModelStreamInfo
	{
		char pad;
	};

	struct XModelQuat
	{
		int16_t v[4];
	};

	struct XModel
	{
		x86::pointer<const char> name;
		unsigned char numBones;
		unsigned char numRootBones;
		unsigned char numsurfs;
		char lodRampType;
		x86::pointer<ScriptString> boneNames;
		x86::pointer<unsigned char> parentList;
		x86::pointer<XModelQuat> quats;
		x86::pointer<float> trans;
		x86::pointer<unsigned char> partClassification;
		x86::pointer<DObjAnimMat> baseMat;
		x86::pointer<XSurface> surfs;
		x86::pointer<x86::pointer<Material>> materialHandles;
		XModelLodInfo lodInfo[4];
		x86::pointer<XModelCollSurf_s> collSurfs;
		int numCollSurfs;
		int contents;
		x86::pointer<XBoneInfo> boneInfo;
		float radius;
		vec3_t mins;
		vec3_t maxs;
		uint16_t numLods;
		int16_t collLod;
		XModelStreamInfo streamInfo;
		int memUsage;
		unsigned char flags;
		bool bad;
		x86::pointer<PhysPreset> physPreset;
		x86::pointer<PhysGeomList> physGeoms;
	};

	enum GfxBlend
	{
		GFXS_BLEND_DISABLED = 0x0,
		GFXS_BLEND_ZERO = 0x1,
		GFXS_BLEND_ONE = 0x2,
		GFXS_BLEND_SRCCOLOR = 0x3,
		GFXS_BLEND_INVSRCCOLOR = 0x4,
		GFXS_BLEND_SRCALPHA = 0x5,
		GFXS_BLEND_INVSRCALPHA = 0x6,
		GFXS_BLEND_DESTALPHA = 0x7,
		GFXS_BLEND_INVDESTALPHA = 0x8,
		GFXS_BLEND_DESTCOLOR = 0x9,
		GFXS_BLEND_INVDESTCOLOR = 0xA,

		GFXS_BLEND_COUNT
	};

	enum GfxBlendOp
	{
		GFXS_BLENDOP_DISABLED = 0x0,
		GFXS_BLENDOP_ADD = 0x1,
		GFXS_BLENDOP_SUBTRACT = 0x2,
		GFXS_BLENDOP_REVSUBTRACT = 0x3,
		GFXS_BLENDOP_MIN = 0x4,
		GFXS_BLENDOP_MAX = 0x5,

		GFXS_BLENDOP_COUNT
	};

	enum GfxAlphaTest_e
	{
		GFXS_ALPHA_TEST_GT_0 = 1,
		GFXS_ALPHA_TEST_LT_128 = 2,
		GFXS_ALPHA_TEST_GE_128 = 3,

		GFXS_ALPHA_TEST_COUNT
	};

	enum GfxCullFace_e
	{
		GFXS_CULL_NONE = 1,
		GFXS_CULL_BACK = 2,
		GFXS_CULL_FRONT = 3,
	};

	enum GfxDepthTest_e
	{
		GFXS_DEPTHTEST_ALWAYS = 0,
		GFXS_DEPTHTEST_LESS = 1,
		GFXS_DEPTHTEST_EQUAL = 2,
		GFXS_DEPTHTEST_LESSEQUAL = 3
	};

	enum GfxPolygonOffset_e
	{
		GFXS_POLYGON_OFFSET_0 = 0,
		GFXS_POLYGON_OFFSET_1 = 1,
		GFXS_POLYGON_OFFSET_2 = 2,
		GFXS_POLYGON_OFFSET_SHADOWMAP = 3
	};

	enum GfxStencilOp
	{
		GFXS_STENCILOP_KEEP = 0x0,
		GFXS_STENCILOP_ZERO = 0x1,
		GFXS_STENCILOP_REPLACE = 0x2,
		GFXS_STENCILOP_INCRSAT = 0x3,
		GFXS_STENCILOP_DECRSAT = 0x4,
		GFXS_STENCILOP_INVERT = 0x5,
		GFXS_STENCILOP_INCR = 0x6,
		GFXS_STENCILOP_DECR = 0x7
	};

	enum GfxStencilFunc
	{
		GFXS_STENCILFUNC_NEVER = 0x0,
		GFXS_STENCILFUNC_LESS = 0x1,
		GFXS_STENCILFUNC_EQUAL = 0x2,
		GFXS_STENCILFUNC_LESSEQUAL = 0x3,
		GFXS_STENCILFUNC_GREATER = 0x4,
		GFXS_STENCILFUNC_NOTEQUAL = 0x5,
		GFXS_STENCILFUNC_GREATEREQUAL = 0x6,
		GFXS_STENCILFUNC_ALWAYS = 0x7
	};

	enum GfxStateBitsEnum : unsigned int
	{
		GFXS0_SRCBLEND_RGB_SHIFT = 0x0,
		GFXS0_SRCBLEND_RGB_MASK = 0xF,

		GFXS0_DSTBLEND_RGB_SHIFT = 0x4,
		GFXS0_DSTBLEND_RGB_MASK = 0xF0,

		GFXS0_BLENDOP_RGB_SHIFT = 0x8,
		GFXS0_BLENDOP_RGB_MASK = 0x700,
		GFXS0_BLEND_RGB_MASK = 0x7FF,

		GFXS0_ATEST_DISABLE = 0x800,
		GFXS0_ATEST_GT_0 = 0x1000,
		GFXS0_ATEST_LT_128 = 0x2000,
		GFXS0_ATEST_GE_128 = 0x3000,
		GFXS0_ATEST_MASK = 0x3000,

		GFXS0_CULL_NONE = 0x4000,
		GFXS0_CULL_BACK = 0x8000,
		GFXS0_CULL_FRONT = 0xC000,
		GFXS0_CULL_SHIFT = 0xE,
		GFXS0_CULL_MASK = 0xC000,

		GFXS0_SRCBLEND_ALPHA_SHIFT = 0x10,
		GFXS0_SRCBLEND_ALPHA_MASK = 0xF0000,

		GFXS0_DSTBLEND_ALPHA_SHIFT = 0x14,
		GFXS0_DSTBLEND_ALPHA_MASK = 0xF00000,

		GFXS0_BLENDOP_ALPHA_SHIFT = 0x18,
		GFXS0_BLENDOP_ALPHA_MASK = 0x7000000,
		GFXS0_BLEND_ALPHA_MASK = 0x7FF0000,

		GFXS0_COLORWRITE_RGB = 0x8000000,
		GFXS0_COLORWRITE_ALPHA = 0x10000000,
		GFXS0_COLORWRITE_MASK = 0x18000000,

		GFXS0_POLYMODE_LINE = 0x80000000,

		GFXS1_DEPTHWRITE = 0x1,

		GFXS1_DEPTHTEST_DISABLE = 0x2,
		GFXS1_DEPTHTEST_ALWAYS = 0x0,
		GFXS1_DEPTHTEST_LESS = 0x4,
		GFXS1_DEPTHTEST_EQUAL = 0x8,
		GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
		GFXS1_DEPTHTEST_SHIFT = 0x2,
		GFXS1_DEPTHTEST_MASK = 0xC,

		GFXS1_POLYGON_OFFSET_0 = 0x0,
		GFXS1_POLYGON_OFFSET_1 = 0x10,
		GFXS1_POLYGON_OFFSET_2 = 0x20,
		GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
		GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
		GFXS1_POLYGON_OFFSET_MASK = 0x30,

		GFXS1_STENCIL_FRONT_ENABLE = 0x40,
		GFXS1_STENCIL_BACK_ENABLE = 0x80,
		GFXS1_STENCIL_MASK = 0xC0,

		GFXS1_STENCIL_FRONT_PASS_SHIFT = 0x8,
		GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
		GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
		GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
		GFXS1_STENCIL_FRONT_MASK = 0xFFF00,

		GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
		GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
		GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
		GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
		GFXS1_STENCIL_BACK_MASK = 0xFFF00000,

		GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
		GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
	};

	struct GfxStateBitsLoadBitsStructured
	{
		// Byte 0
		unsigned int srcBlendRgb : 4;       // 0-3
		unsigned int dstBlendRgb : 4;       // 4-7
		unsigned int blendOpRgb : 3;        // 8-10
		unsigned int alphaTestDisabled : 1; // 11
		unsigned int alphaTest : 2;         // 12-13
		unsigned int cullFace : 2;          // 14-15
		unsigned int srcBlendAlpha : 4;     // 16-19
		unsigned int dstBlendAlpha : 4;     // 20-23
		unsigned int blendOpAlpha : 3;      // 24-26
		unsigned int colorWriteRgb : 1;     // 27
		unsigned int colorWriteAlpha : 1;   // 28
		unsigned int unused1 : 2;           // 29-30
		unsigned int polymodeLine : 1;      // 31

		// Byte 1
		unsigned int depthWrite : 1;          // 0
		unsigned int depthTestDisabled : 1;   // 1
		unsigned int depthTest : 2;           // 2-3
		unsigned int polygonOffset : 2;       // 4-5
		unsigned int stencilFrontEnabled : 1; // 6
		unsigned int stencilBackEnabled : 1;  // 7
		unsigned int stencilFrontPass : 3;    // 8-10
		unsigned int stencilFrontFail : 3;    // 11-13
		unsigned int stencilFrontZFail : 3;   // 14-16
		unsigned int stencilFrontFunc : 3;    // 17-19
		unsigned int stencilBackPass : 3;     // 20-22
		unsigned int stencilBackFail : 3;     // 23-25
		unsigned int stencilBackZFail : 3;    // 26-28
		unsigned int stencilBackFunc : 3;     // 29-31
	};

	union GfxStateBitsLoadBits
	{
		unsigned int raw[2];
		GfxStateBitsLoadBitsStructured structured;
	};

#ifndef __zonecodegenerator
	static_assert(sizeof(GfxStateBitsLoadBits) == 8);
	static_assert(sizeof(GfxStateBitsLoadBitsStructured) == 8);
#endif

	struct GfxStateBits
	{
		GfxStateBitsLoadBits loadBits;
	};

	struct alignas(16) MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		vec4_t literal;
	};

	struct complex_s
	{
		float real;
		float imag;
	};

	struct WaterWritable
	{
		float floatTime;
	};

	struct water_t
	{
		WaterWritable writable;
		x86::pointer<complex_s> H0;
		x86::pointer<float> wTerm;
		int M;
		int N;
		float Lx;
		float Lz;
		float gravity;
		float windvel;
		float winddir[2];
		float amplitude;
		float codeConstant[4];
		x86::pointer<GfxImage> image;
	};

	union MaterialTextureDefInfo
	{
		x86::pointer<GfxImage> image;
		x86::pointer<water_t> water;
	};

	enum TextureFilter
	{
		TEXTURE_FILTER_DISABLED = 0x0,
		TEXTURE_FILTER_NEAREST = 0x1,
		TEXTURE_FILTER_LINEAR = 0x2,
		TEXTURE_FILTER_ANISO2X = 0x3,
		TEXTURE_FILTER_ANISO4X = 0x4,

		TEXTURE_FILTER_COUNT
	};

	enum SamplerStateBitsMipMap_e
	{
		SAMPLER_MIPMAP_ENUM_DISABLED,
		SAMPLER_MIPMAP_ENUM_NEAREST,
		SAMPLER_MIPMAP_ENUM_LINEAR,

		SAMPLER_MIPMAP_ENUM_COUNT
	};

	enum SamplerStateBits_e
	{
		SAMPLER_FILTER_SHIFT = 0x0,
		SAMPLER_FILTER_NEAREST = 0x1,
		SAMPLER_FILTER_LINEAR = 0x2,
		SAMPLER_FILTER_ANISO2X = 0x3,
		SAMPLER_FILTER_ANISO4X = 0x4,
		SAMPLER_FILTER_MASK = 0x7,

		SAMPLER_MIPMAP_SHIFT = 0x3,
		SAMPLER_MIPMAP_DISABLED = 0x0,
		SAMPLER_MIPMAP_NEAREST = 0x8,
		SAMPLER_MIPMAP_LINEAR = 0x10,
		SAMPLER_MIPMAP_COUNT = 0x3,
		SAMPLER_MIPMAP_MASK = 0x18,

		SAMPLER_CLAMP_U_SHIFT = 0x5,
		SAMPLER_CLAMP_V_SHIFT = 0x6,
		SAMPLER_CLAMP_W_SHIFT = 0x7,
		SAMPLER_CLAMP_U = 0x20,
		SAMPLER_CLAMP_V = 0x40,
		SAMPLER_CLAMP_W = 0x80,
		SAMPLER_CLAMP_MASK = 0xE0,
	};

	struct MaterialTextureDefSamplerState
	{
		unsigned char filter : 3;
		unsigned char mipMap : 2;
		unsigned char clampU : 1;
		unsigned char clampV : 1;
		unsigned char clampW : 1;
	};

#ifndef __zonecodegenerator
	static_assert(sizeof(MaterialTextureDefSamplerState) == 1u);
#endif

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		MaterialTextureDefSamplerState samplerState; // SamplerStateBits_e
		unsigned char semantic;                      // TextureSemantic
		MaterialTextureDefInfo u;
	};

	struct alignas(8) GfxDrawSurfFields
	{
		uint64_t objectId : 16;
		uint64_t reflectionProbeIndex : 8;
		uint64_t customIndex : 5;
		uint64_t materialSortedIndex : 11;
		uint64_t prepass : 2;
		uint64_t primaryLightIndex : 8;
		uint64_t surfType : 4;
		uint64_t primarySortKey : 6;
		uint64_t unused : 4;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		uint64_t packed;
	};

	enum materialSurfType_t
	{
		SURF_TYPE_DEFAULT,
		SURF_TYPE_BARK,
		SURF_TYPE_BRICK,
		SURF_TYPE_CARPET,
		SURF_TYPE_CLOTH,
		SURF_TYPE_CONCRETE,
		SURF_TYPE_DIRT,
		SURF_TYPE_FLESH,
		SURF_TYPE_FOLIAGE,
		SURF_TYPE_GLASS,
		SURF_TYPE_GRASS,
		SURF_TYPE_GRAVEL,
		SURF_TYPE_ICE,
		SURF_TYPE_METAL,
		SURF_TYPE_MUD,
		SURF_TYPE_PAPER,
		SURF_TYPE_PLASTER,
		SURF_TYPE_ROCK,
		SURF_TYPE_SAND,
		SURF_TYPE_SNOW,
		SURF_TYPE_WATER,
		SURF_TYPE_WOOD,
		SURF_TYPE_ASPHALT,
		SURF_TYPE_CERAMIC,
		SURF_TYPE_PLASTIC,
		SURF_TYPE_RUBBER,
		SURF_TYPE_CUSHION,
		SURF_TYPE_FRUIT,
		SURF_TYPE_PAINTED_METAL,

		SURF_TYPE_NUM
	};

	// Unknown if this is the right amount of flags
	enum MaterialGameFlags
	{
		MTL_GAMEFLAG_1 = 0x1,
		MTL_GAMEFLAG_2 = 0x2,
		MTL_GAMEFLAG_4 = 0x4,
		MTL_GAMEFLAG_8 = 0x8,
		MTL_GAMEFLAG_10 = 0x10,
		MTL_GAMEFLAG_20 = 0x20,
		MTL_GAMEFLAG_CASTS_SHADOW = 0x40,
		MTL_GAMEFLAG_80 = 0x80,
		MTL_GAMEFLAG_100 = 0x100,
		MTL_GAMEFLAG_200 = 0x200,
		MTL_GAMEFLAG_400 = 0x400,
		MTL_GAMEFLAG_800 = 0x800,
		MTL_GAMEFLAG_1000 = 0x1000,
	};

	struct MaterialInfo
	{
		x86::pointer<const char> name;
		unsigned char gameFlags;
		unsigned char sortKey;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		GfxDrawSurf drawSurf;
		unsigned int surfaceTypeBits;
		uint16_t hashIndex;
	};

	enum GfxCameraRegionType
	{
		CAMERA_REGION_LIT = 0x0,
		CAMERA_REGION_DECAL = 0x1,
		CAMERA_REGION_EMISSIVE = 0x2,

		CAMERA_REGION_COUNT,
		CAMERA_REGION_NONE = CAMERA_REGION_COUNT,
	};

	enum MaterialStateFlags
	{
		STATE_FLAG_CULL_BACK = 0x1,
		STATE_FLAG_CULL_FRONT = 0x2,
		STATE_FLAG_DECAL = 0x4,
		STATE_FLAG_WRITES_DEPTH = 0x8,
		STATE_FLAG_USES_DEPTH_BUFFER = 0x10,
		STATE_FLAG_USES_STENCIL_BUFFER = 0x20,
		STATE_FLAG_CULL_BACK_SHADOW = 0x40,
		STATE_FLAG_CULL_FRONT_SHADOW = 0x80,
	};

	struct Material
	{
		MaterialInfo info;
		char stateBitsEntry[34];
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char stateBitsCount;
		unsigned char stateFlags;
		unsigned char cameraRegion;
		x86::pointer<MaterialTechniqueSet> techniqueSet;
		x86::pointer<MaterialTextureDef> textureTable;
		x86::pointer<MaterialConstantDef> constantTable;
		x86::pointer<GfxStateBits> stateBitsTable;
	};

	struct MaterialArgumentCodeConst
	{
		uint16_t index;
		unsigned char firstRow;
		unsigned char rowCount;
	};

	union MaterialArgumentDef
	{
		x86::pointer<const float[4]> literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	enum MaterialShaderArgumentType
	{
		MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
		MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
		MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x2,

		MTL_ARG_CODE_PRIM_BEGIN = 0x3,

		MTL_ARG_CODE_VERTEX_CONST = 0x3,
		MTL_ARG_CODE_PIXEL_SAMPLER = 0x4,
		MTL_ARG_CODE_PIXEL_CONST = 0x5,

		MTL_ARG_CODE_PRIM_END = 0x6,

		MTL_ARG_MATERIAL_PIXEL_CONST = 0x6,
		MTL_ARG_LITERAL_PIXEL_CONST = 0x7,

		MTL_ARG_COUNT
	};

	enum MaterialType
	{
		MTL_TYPE_DEFAULT = 0x0,
		MTL_TYPE_MODEL = 0x1,         // m_
		MTL_TYPE_MODEL_VERTCOL = 0x2, // mc_
		MTL_TYPE_WORLD = 0x3,         // w_
		MTL_TYPE_WORLD_VERTCOL = 0x4, // wc_

		MTL_TYPE_COUNT,
	};

	struct MaterialTypeInfo
	{
		x86::pointer<const char> materialPrefix;
		x86::pointer<const char> techniqueSetPrefix;
	};

	enum ShaderCodeConstants
	{
		CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,

		CONST_SRC_CODE_LIGHT_POSITION = 0x0,
		CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
		CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
		CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
		CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
		CONST_SRC_CODE_NEARPLANE_ORG = 0x5,
		CONST_SRC_CODE_NEARPLANE_DX = 0x6,
		CONST_SRC_CODE_NEARPLANE_DY = 0x7,
		CONST_SRC_CODE_SHADOW_PARMS = 0x8,
		CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x9,
		CONST_SRC_CODE_RENDER_TARGET_SIZE = 0xA,
		CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0xB,
		CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0xC,
		CONST_SRC_CODE_DOF_EQUATION_SCENE = 0xD,
		CONST_SRC_CODE_DOF_LERP_SCALE = 0xE,
		CONST_SRC_CODE_DOF_LERP_BIAS = 0xF,
		CONST_SRC_CODE_DOF_ROW_DELTA = 0x10,
		CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x11,
		CONST_SRC_CODE_GAMETIME = 0x12,

		CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x13,
		CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x13,

		CONST_SRC_CODE_PIXEL_COST_FRACS = 0x13,
		CONST_SRC_CODE_PIXEL_COST_DECODE = 0x14,
		CONST_SRC_CODE_FILTER_TAP_0 = 0x15,
		CONST_SRC_CODE_FILTER_TAP_1 = 0x16,
		CONST_SRC_CODE_FILTER_TAP_2 = 0x17,
		CONST_SRC_CODE_FILTER_TAP_3 = 0x18,
		CONST_SRC_CODE_FILTER_TAP_4 = 0x19,
		CONST_SRC_CODE_FILTER_TAP_5 = 0x1A,
		CONST_SRC_CODE_FILTER_TAP_6 = 0x1B,
		CONST_SRC_CODE_FILTER_TAP_7 = 0x1C,
		CONST_SRC_CODE_COLOR_MATRIX_R = 0x1D,
		CONST_SRC_CODE_COLOR_MATRIX_G = 0x1E,
		CONST_SRC_CODE_COLOR_MATRIX_B = 0x1F,

		CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x20,
		CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x20,

		CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x20,
		CONST_SRC_CODE_SHADOWMAP_SCALE = 0x21,
		CONST_SRC_CODE_ZNEAR = 0x22,
		CONST_SRC_CODE_SUN_POSITION = 0x23,
		CONST_SRC_CODE_SUN_DIFFUSE = 0x24,
		CONST_SRC_CODE_SUN_SPECULAR = 0x25,
		CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x26,
		CONST_SRC_CODE_DEBUG_BUMPMAP = 0x27,
		CONST_SRC_CODE_MATERIAL_COLOR = 0x28,
		CONST_SRC_CODE_FOG = 0x29,
		CONST_SRC_CODE_FOG_COLOR = 0x2A,
		CONST_SRC_CODE_GLOW_SETUP = 0x2B,
		CONST_SRC_CODE_GLOW_APPLY = 0x2C,
		CONST_SRC_CODE_COLOR_BIAS = 0x2D,
		CONST_SRC_CODE_COLOR_TINT_BASE = 0x2E,
		CONST_SRC_CODE_COLOR_TINT_DELTA = 0x2F,
		CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x30,
		CONST_SRC_CODE_ENVMAP_PARMS = 0x31,
		CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x32,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x33,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x34,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x35,
		CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x36,
		CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x37,
		CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x38,

		CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x38,

		CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x39,

		CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0x3A,
		CONST_SRC_CODE_COUNT_FLOAT4 = 0x3A,
		CONST_SRC_FIRST_CODE_MATRIX = 0x3A,

		CONST_SRC_CODE_WORLD_MATRIX = 0x3A,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0x3B,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0x3C,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0x3D,
		CONST_SRC_CODE_VIEW_MATRIX = 0x3E,
		CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x3F,
		CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x40,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x41,
		CONST_SRC_CODE_PROJECTION_MATRIX = 0x42,
		CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x43,
		CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x44,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x45,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0x46,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0x47,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x48,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x49,
		CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x4A,
		CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x4B,
		CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4C,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4D,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0x4E,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x4F,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x50,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x51,
		CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x52,
		CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x53,
		CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x54,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x55,
		CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x56,
		CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x57,
		CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x58,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x59,

		CONST_SRC_TOTAL_COUNT,
		CONST_SRC_NONE
	};

	enum MaterialTextureSource
	{
		TEXTURE_SRC_CODE_BLACK = 0x0,
		TEXTURE_SRC_CODE_WHITE = 0x1,
		TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
		TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
		TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
		TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
		TEXTURE_SRC_CODE_SHADOWCOOKIE = 0x6,
		TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x7,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x8,
		TEXTURE_SRC_CODE_FEEDBACK = 0x9,
		TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0xA,
		TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xB,
		TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xC,
		TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xD,
		TEXTURE_SRC_CODE_SKY = 0xE,
		TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0xF,
		TEXTURE_SRC_CODE_DYNAMIC_SHADOWS = 0x10,
		TEXTURE_SRC_CODE_OUTDOOR = 0x11,
		TEXTURE_SRC_CODE_FLOATZ = 0x12,
		TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x13,
		TEXTURE_SRC_CODE_RAW_FLOATZ = 0x14,
		TEXTURE_SRC_CODE_CASE_TEXTURE = 0x15,
		TEXTURE_SRC_CODE_CINEMATIC_Y = 0x16,
		TEXTURE_SRC_CODE_CINEMATIC_CR = 0x17,
		TEXTURE_SRC_CODE_CINEMATIC_CB = 0x18,
		TEXTURE_SRC_CODE_CINEMATIC_A = 0x19,
		TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1A,

		TEXTURE_SRC_CODE_COUNT
	};

	enum CustomSamplers
	{
		CUSTOM_SAMPLER_REFLECTION_PROBE = 0x0,
		CUSTOM_SAMPLER_LIGHTMAP_PRIMARY = 0x1,
		CUSTOM_SAMPLER_LIGHTMAP_SECONDARY = 0x2,

		CUSTOM_SAMPLER_COUNT
	};

	enum MaterialUpdateFrequency
	{
		MTL_UPDATE_PER_PRIM = 0x0,
		MTL_UPDATE_PER_OBJECT = 0x1,
		MTL_UPDATE_RARELY = 0x2,
		MTL_UPDATE_CUSTOM = 0x3,
	};

	struct MaterialShaderArgument
	{
		uint16_t type;
		uint16_t dest;
		MaterialArgumentDef u;
	};

	enum MaterialStreamStreamSource_e
	{
		STREAM_SRC_POSITION = 0x0,
		STREAM_SRC_COLOR = 0x1,
		STREAM_SRC_TEXCOORD_0 = 0x2,
		STREAM_SRC_NORMAL = 0x3,

		STREAM_SRC_PRE_OPTIONAL_BEGIN = 0x4,

		STREAM_SRC_TANGENT = 0x4,

		STREAM_SRC_OPTIONAL_BEGIN = 0x5,

		STREAM_SRC_TEXCOORD_1 = 0x5,
		STREAM_SRC_TEXCOORD_2 = 0x6,
		STREAM_SRC_NORMAL_TRANSFORM_0 = 0x7,
		STREAM_SRC_NORMAL_TRANSFORM_1 = 0x8,

		STREAM_SRC_COUNT
	};

	enum MaterialStreamDestination_e
	{
		STREAM_DST_POSITION = 0x0,
		STREAM_DST_NORMAL = 0x1,
		STREAM_DST_COLOR_0 = 0x2,
		STREAM_DST_COLOR_1 = 0x3,
		STREAM_DST_TEXCOORD_0 = 0x4,
		STREAM_DST_TEXCOORD_1 = 0x5,
		STREAM_DST_TEXCOORD_2 = 0x6,
		STREAM_DST_TEXCOORD_3 = 0x7,
		STREAM_DST_TEXCOORD_4 = 0x8,
		STREAM_DST_TEXCOORD_5 = 0x9,
		STREAM_DST_TEXCOORD_6 = 0xA,
		STREAM_DST_TEXCOORD_7 = 0xB,
		STREAM_DST_COUNT
	};

	struct MaterialStreamRouting
	{
		unsigned char source;
		unsigned char dest;
	};

	enum MaterialVertexDeclType
	{
		VERTDECL_GENERIC = 0x0,
		VERTDECL_PACKED = 0x1,
		VERTDECL_WORLD = 0x2,
		VERTDECL_WORLD_T1N0 = 0x3,
		VERTDECL_WORLD_T1N1 = 0x4,
		VERTDECL_WORLD_T2N0 = 0x5,
		VERTDECL_WORLD_T2N1 = 0x6,
		VERTDECL_WORLD_T2N2 = 0x7,
		VERTDECL_WORLD_T3N0 = 0x8,
		VERTDECL_WORLD_T3N1 = 0x9,
		VERTDECL_WORLD_T3N2 = 0xA,
		VERTDECL_WORLD_T4N0 = 0xB,
		VERTDECL_WORLD_T4N1 = 0xC,
		VERTDECL_WORLD_T4N2 = 0xD,
		VERTDECL_POS_TEX = 0xE,
		VERTDECL_STATICMODELCACHE = 0xF,

		VERTDECL_COUNT
	};

	struct MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[16];
		x86::pointer<void> decl[VERTDECL_COUNT];
	};

	struct MaterialVertexDeclaration
	{
		unsigned char streamCount;
		bool hasOptionalSource;
		bool isLoaded;
		MaterialVertexStreamRouting routing;
	};

	struct GfxVertexShaderLoadDef
	{
		x86::pointer<unsigned int> program;
		uint16_t programSize;
		uint16_t loadForRenderer;
	};

	struct MaterialVertexShaderProgram
	{
		x86::pointer<void> vs;
		GfxVertexShaderLoadDef loadDef;
	};

	struct MaterialVertexShader
	{
		x86::pointer<const char> name;
		MaterialVertexShaderProgram prog;
	};

	struct GfxPixelShaderLoadDef
	{
		x86::pointer<unsigned int> program;
		uint16_t programSize;
		uint16_t loadForRenderer;
	};

	struct MaterialPixelShaderProgram
	{
		x86::pointer<void> ps;
		GfxPixelShaderLoadDef loadDef;
	};

	struct MaterialPixelShader
	{
		x86::pointer<const char> name;
		MaterialPixelShaderProgram prog;
	};

	struct MaterialPass
	{
		x86::pointer<MaterialVertexDeclaration> vertexDecl;
		x86::pointer<MaterialVertexShader> vertexShader;
		x86::pointer<MaterialPixelShader> pixelShader;
		char perPrimArgCount;
		char perObjArgCount;
		char stableArgCount;
		char customSamplerFlags;
		x86::pointer<MaterialShaderArgument> args;
	};

	enum TechniqueFlags
	{
		MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN = 0x1,
		MTL_TECHFLAG_NEEDS_RESOLVED_SCENE = 0x2,
		MTL_TECHFLAG_ZPREPASS = 0x4,

		MTL_TECHFLAG_DECL_HAS_OPTIONAL_SOURCE = 0x8,

		MTL_TECHFLAG_USES_LIGHT_SPOT_FACTORS = 0x10,
		MTL_TECHFLAG_USES_FLOATZ = 0x20,
	};

	struct MaterialTechnique
	{
		x86::pointer<const char> name;
		uint16_t flags;
		uint16_t passCount;
		MaterialPass passArray[1];
	};

	enum MaterialTechniqueType
	{
		TECHNIQUE_DEPTH_PREPASS = 0x0,
		TECHNIQUE_BUILD_FLOAT_Z = 0x1,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
		TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
		TECHNIQUE_UNLIT = 0x4,
		TECHNIQUE_EMISSIVE = 0x5,
		TECHNIQUE_EMISSIVE_SHADOW = 0x6,

		TECHNIQUE_LIT_BEGIN = 0x7,

		TECHNIQUE_LIT = 0x7,
		TECHNIQUE_LIT_SUN = 0x8,
		TECHNIQUE_LIT_SUN_SHADOW = 0x9,
		TECHNIQUE_LIT_SPOT = 0xA,
		TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
		TECHNIQUE_LIT_OMNI = 0xC,
		TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
		TECHNIQUE_LIT_INSTANCED = 0xE,
		TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
		TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
		TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
		TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
		TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
		TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,

		TECHNIQUE_LIT_END = 0x15,

		TECHNIQUE_LIGHT_SPOT = 0x15,
		TECHNIQUE_LIGHT_OMNI = 0x16,
		TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
		TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
		TECHNIQUE_FAKELIGHT_VIEW = 0x19,
		TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
		TECHNIQUE_CASE_TEXTURE = 0x1B,
		TECHNIQUE_WIREFRAME_SOLID = 0x1C,
		TECHNIQUE_WIREFRAME_SHADED = 0x1D,
		TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
		TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
		TECHNIQUE_DEBUG_BUMPMAP = 0x20,
		TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
		TECHNIQUE_COUNT = 0x22,
		TECHNIQUE_TOTAL_COUNT = 0x23,
		TECHNIQUE_NONE = 0x24,
	};

	enum MaterialWorldVertexFormat : unsigned char
	{
		MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
		MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
		MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
		MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
		MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
		MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
		MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
		MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
		MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
		MTL_WORLDVERT_TEX_5_NRM_1 = 0x9,
		MTL_WORLDVERT_TEX_5_NRM_2 = 0xA,
		MTL_WORLDVERT_TEX_5_NRM_3 = 0xB,
	};

	struct MaterialTechniqueSet
	{
		x86::pointer<const char> name;
		MaterialWorldVertexFormat worldVertFormat;
		bool hasBeenUploaded;
		char unused[1];
		x86::pointer<MaterialTechniqueSet> remappedTechniqueSet;
		x86::pointer<MaterialTechnique> techniques[TECHNIQUE_COUNT];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct Picmip
	{
		char platform[2];
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char flags;
		uint16_t dimensions[3];
		int format;
		unsigned int resourceSize;
		char data[1];
	};

	union GfxTexture
	{
		// void/*IDirect3DBaseTexture9*/* basemap;
		// void/*IDirect3DTexture9*/* map;
		// void/*IDirect3DVolumeTexture9*/* volmap;
		// void/*IDirect3DCubeTexture9*/* cubemap;
		x86::pointer<GfxImageLoadDef> loadDef;
	};

	enum MapType
	{
		MAPTYPE_NONE = 0x0,
		MAPTYPE_INVALID1 = 0x1,
		MAPTYPE_INVALID2 = 0x2,
		MAPTYPE_2D = 0x3,
		MAPTYPE_3D = 0x4,
		MAPTYPE_CUBE = 0x5,

		MAPTYPE_COUNT
	};

	enum TextureSemantic
	{
		TS_2D = 0x0,
		TS_FUNCTION = 0x1,
		TS_COLOR_MAP = 0x2,
		TS_UNUSED_1 = 0x3,
		TS_UNUSED_2 = 0x4,
		TS_NORMAL_MAP = 0x5,
		TS_UNUSED_3 = 0x6,
		TS_UNUSED_4 = 0x7,
		TS_SPECULAR_MAP = 0x8,
		TS_UNUSED_5 = 0x9,
		TS_UNUSED_6 = 0xA,
		TS_WATER_MAP = 0xB,
	};

	enum ImageCategory
	{
		IMG_CATEGORY_UNKNOWN = 0x0,
		IMG_CATEGORY_AUTO_GENERATED = 0x1,
		IMG_CATEGORY_LIGHTMAP = 0x2,
		IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
		IMG_CATEGORY_RAW = 0x4,
		IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
		IMG_CATEGORY_WATER = 0x5,
		IMG_CATEGORY_RENDERTARGET = 0x6,
		IMG_CATEGORY_TEMP = 0x7,
	};

	struct GfxImage
	{
		MapType mapType;
		GfxTexture texture;
		Picmip picmip;
		bool noPicmip;
		char semantic;
		char track;
		CardMemory cardMemory;
		uint16_t width;
		uint16_t height;
		uint16_t depth;
		char category;
		bool delayLoadPixels;
		x86::pointer<const char> name;
	};

	struct SndCurve
	{
		x86::pointer<const char> filename;
		int knotCount;
		float knots[8][2];
	};

	struct MSSSpeakerLevels
	{
		int speaker;
		int numLevels;
		float levels[2];
	};

	struct MSSChannelMap
	{
		int speakerCount;
		MSSSpeakerLevels speakers[6];
	};

	struct SpeakerMap
	{
		bool isDefault;
		x86::pointer<const char> name;
		MSSChannelMap channelMaps[2][2];
	};

	struct StreamedSound
	{
		x86::pointer<const char> dir;
		x86::pointer<const char> name;
	};

	union SoundFileRef
	{
		x86::pointer<LoadedSound> loadSnd;
		StreamedSound streamSnd;
	};

	enum snd_alias_type_t
	{
		SAT_UNKNOWN = 0x0,
		SAT_LOADED = 0x1,
		SAT_STREAMED = 0x2,
		SAT_COUNT = 0x3,
	};

	struct SoundFile
	{
		char type;
		char exists;
		SoundFileRef u;
	};

	struct snd_alias_t
	{
		x86::pointer<const char> aliasName;
		x86::pointer<const char> subtitle;
		x86::pointer<const char> secondaryAliasName;
		x86::pointer<const char> chainAliasName;
		x86::pointer<SoundFile> soundFile;
		int sequence;
		float volMin;
		float volMax;
		float pitchMin;
		float pitchMax;
		float distMin;
		float distMax;
		int flags;
		float slavePercentage;
		float probability;
		float lfePercentage;
		float centerPercentage;
		int startDelay;
		x86::pointer<SndCurve> volumeFalloffCurve;
		float envelopMin;
		float envelopMax;
		float envelopPercentage;
		x86::pointer<SpeakerMap> speakerMap;
	};

	struct snd_alias_list_t
	{
		x86::pointer<const char> aliasName;
		x86::pointer<snd_alias_t> head;
		int count;
	};

	struct AILSOUNDINFO
	{
		int format;
		x86::pointer<const void> data_ptr;
		unsigned int data_len;
		unsigned int rate;
		int bits;
		int channels;
		unsigned int samples;
		unsigned int block_size;
		x86::pointer<const void> initial_ptr;
	};

	struct MssSound
	{
		AILSOUNDINFO info;
		x86::pointer<char> data;
	};

	struct LoadedSound
	{
		x86::pointer<const char> name;
		MssSound sound;
	};

	struct cStaticModelWritable
	{
		uint16_t nextModelInWorldSector;
	};

	struct cStaticModel_s
	{
		cStaticModelWritable writable;
		x86::pointer<XModel> xmodel;
		float origin[3];
		float invScaledAxis[3][3];
		float absmin[3];
		float absmax[3];
	};

	struct dmaterial_t
	{
		char material[64];
		int surfaceFlags;
		int contentFlags;
	};

	struct cNode_t
	{
		x86::pointer<cplane_s> plane;
		int16_t children[2];
	};

	struct cLeaf_t
	{
		uint16_t firstCollAabbIndex;
		uint16_t collAabbCount;
		int brushContents;
		int terrainContents;
		float mins[3];
		float maxs[3];
		int leafBrushNode;
		int16_t cluster;
	};

	struct cLeafBrushNodeChildren_t
	{
		float dist;
		float range;
		uint16_t childOffset[2];
	};

	struct cLeafBrushNodeLeaf_t
	{
		x86::pointer<uint16_t> brushes;
	};

	union cLeafBrushNodeData_t
	{
		cLeafBrushNodeLeaf_t leaf;
		cLeafBrushNodeChildren_t children;
	};

	struct cLeafBrushNode_s
	{
		char axis;
		int16_t leafBrushCount;
		int contents;
		cLeafBrushNodeData_t data;
	};

	struct CollisionBorder
	{
		float distEq[3];
		float zBase;
		float zSlope;
		float start;
		float length;
	};

	struct CollisionPartition
	{
		unsigned char triCount;
		unsigned char borderCount;
		int firstTri;
		x86::pointer<CollisionBorder> borders;
	};

	union CollisionAabbTreeIndex
	{
		int firstChildIndex;
		int partitionIndex;
	};

	struct CollisionAabbTree
	{
		float origin[3];
		float halfSize[3];
		uint16_t materialIndex;
		uint16_t childCount;
		CollisionAabbTreeIndex u;
	};

	struct cmodel_t
	{
		float mins[3];
		float maxs[3];
		float radius;
		cLeaf_t leaf;
	};

	struct alignas(16) cbrush_t
	{
		float mins[3];
		int contents;
		float maxs[3];
		unsigned int numsides;
		x86::pointer<cbrushside_t> sides;
		int16_t axialMaterialNum[2][3];
		x86::pointer<cbrushedge_t> baseAdjacentSide;
		int16_t firstAdjacentSideOffsets[2][3];
		char edgeCount[2][3];
	};

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	enum DynEntityDrawType
	{
		DYNENT_DRAW_MODEL = 0x0,
		DYNENT_DRAW_BRUSH = 0x1,

		DYNENT_DRAW_COUNT,
	};

	enum DynEntityCollType
	{
		DYNENT_COLL_CLIENT_FIRST = 0x0,
		DYNENT_COLL_CLIENT_MODEL = 0x0,
		DYNENT_COLL_CLIENT_BRUSH = 0x1,

		DYNENT_COLL_COUNT,
	};

	enum DynEntityType
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_DESTRUCT = 0x2,
		DYNENT_TYPE_COUNT = 0x3,
	};

	struct DynEntityDef
	{
		DynEntityType type;
		GfxPlacement pose;
		x86::pointer<XModel> xModel;
		uint16_t brushModel;
		uint16_t physicsBrushModel;
		x86::pointer<FxEffectDef> destroyFx;
		x86::pointer<XModelPieces> destroyPieces;
		x86::pointer<PhysPreset> physPreset;
		int health;
		PhysMass mass;
		int contents;
	};

	struct DynEntityPose
	{
		GfxPlacement pose;
		float radius;
	};

	struct DynEntityClient
	{
		int physObjId;
		uint16_t flags;
		uint16_t lightingHandle;
		int health;
	};

	struct DynEntityColl
	{
		uint16_t sector;
		uint16_t nextEntInSector;
		float linkMins[2];
		float linkMaxs[2];
	};

	typedef unsigned short LeafBrush;

	typedef cbrush_t cbrush_array_t;

	struct clipMap_t
	{
		x86::pointer<const char> name;
		int isInUse;
		int planeCount;
		x86::pointer<cplane_s> planes;
		unsigned int numStaticModels;
		x86::pointer<cStaticModel_s> staticModelList;
		unsigned int numMaterials;
		x86::pointer<dmaterial_t> materials;
		unsigned int numBrushSides;
		x86::pointer<cbrushside_t> brushsides;
		unsigned int numBrushEdges;
		x86::pointer<cbrushedge_t> brushEdges;
		unsigned int numNodes;
		x86::pointer<cNode_t> nodes;
		unsigned int numLeafs;
		x86::pointer<cLeaf_t> leafs;
		unsigned int leafbrushNodesCount;
		x86::pointer<cLeafBrushNode_s> leafbrushNodes;
		unsigned int numLeafBrushes;
		x86::pointer<LeafBrush> leafbrushes;
		unsigned int numLeafSurfaces;
		x86::pointer<unsigned int> leafsurfaces;
		unsigned int vertCount;
		x86::pointer<vec3_t> verts;
		int triCount;
		x86::pointer<uint16_t> triIndices;
		x86::pointer<char> triEdgeIsWalkable;
		int borderCount;
		x86::pointer<CollisionBorder> borders;
		int partitionCount;
		x86::pointer<CollisionPartition> partitions;
		int aabbTreeCount;
		x86::pointer<CollisionAabbTree> aabbTrees;
		unsigned int numSubModels;
		x86::pointer<cmodel_t> cmodels;
		uint16_t numBrushes;
		x86::pointer<cbrush_t> brushes;
		int numClusters;
		int clusterBytes;
		x86::pointer<char> visibility;
		int vised;
		x86::pointer<MapEnts> mapEnts;
		x86::pointer<cbrush_t> box_brush;
		cmodel_t box_model;
		uint16_t dynEntCount[DYNENT_COLL_COUNT];
		x86::pointer<DynEntityDef> dynEntDefList[DYNENT_DRAW_COUNT];
		x86::pointer<DynEntityPose> dynEntPoseList[DYNENT_DRAW_COUNT];
		x86::pointer<DynEntityClient> dynEntClientList[DYNENT_DRAW_COUNT];
		x86::pointer<DynEntityColl> dynEntCollList[DYNENT_COLL_COUNT];
		unsigned int checksum;
	};

	struct ComPrimaryLight
	{
		char type;
		char canUseShadowMap;
		char exponent;
		char unused;
		float color[3];
		float dir[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float cosHalfFovExpanded;
		float rotationLimit;
		float translationLimit;
		x86::pointer<const char> defName;
	};

	struct ComWorld
	{
		x86::pointer<const char> name;
		int isInUse;
		unsigned int primaryLightCount;
		x86::pointer<ComPrimaryLight> primaryLights;
	};

	struct pathnode_tree_nodes_t
	{
		int nodeCount;
		x86::pointer<uint16_t> nodes;
	};

	struct pathnode_t;
	struct pathnode_tree_t;

	union pathnode_tree_info_t
	{
		x86::pointer<pathnode_tree_t> child[2];
		pathnode_tree_nodes_t s;
	};

	struct pathnode_tree_t
	{
		int axis;
		float dist;
		pathnode_tree_info_t u;
	};

	struct alignas(16) pathbasenode_t
	{
		float vOrigin[3];
		unsigned int type;
	};

	struct pathnode_transient_t
	{
		int iSearchFrame;
		x86::pointer<pathnode_t> pNextOpen;
		x86::pointer<pathnode_t> pPrevOpen;
		x86::pointer<pathnode_t> pParent;
		float fCost;
		float fHeuristic;
		float costFactor;
	};

	struct pathnode_dynamic_t
	{
		x86::pointer<void> pOwner;
		int iFreeTime;
		int iValidTime[3];
		int inPlayerLOSTime;
		int16_t wLinkCount;
		int16_t wOverlapCount;
		int16_t turretEntNumber;
		int16_t userCount;
	};

	struct pathlink_s
	{
		float fDist;
		uint16_t nodeNum;
		char disconnectCount;
		char negotiationLink;
		char ubBadPlaceCount[4];
	};

	enum nodeType
	{
		NODE_BADNODE = 0x0,
		NODE_PATHNODE = 0x1,
		NODE_COVER_STAND = 0x2,
		NODE_COVER_CROUCH = 0x3,
		NODE_COVER_CROUCH_WINDOW = 0x4,
		NODE_COVER_PRONE = 0x5,
		NODE_COVER_RIGHT = 0x6,
		NODE_COVER_LEFT = 0x7,
		NODE_COVER_WIDE_RIGHT = 0x8,
		NODE_COVER_WIDE_LEFT = 0x9,
		NODE_CONCEALMENT_STAND = 0xA,
		NODE_CONCEALMENT_CROUCH = 0xB,
		NODE_CONCEALMENT_PRONE = 0xC,
		NODE_REACQUIRE = 0xD,
		NODE_BALCONY = 0xE,
		NODE_SCRIPTED = 0xF,
		NODE_NEGOTIATION_BEGIN = 0x10,
		NODE_NEGOTIATION_END = 0x11,
		NODE_TURRET = 0x12,
		NODE_GUARD = 0x13,
		NODE_NUMTYPES = 0x14,
		NODE_DONTLINK = 0x14,
	};

	struct pathnode_constant_t
	{
		nodeType type;
		uint16_t spawnflags;
		uint16_t targetname;
		uint16_t script_linkName;
		uint16_t script_noteworthy;
		uint16_t target;
		uint16_t animscript;
		int animscriptfunc;
		float vOrigin[3];
		float fAngle;
		float forward[2];
		float fRadius;
		float minUseDistSq;
		int16_t wOverlapNode[2];
		int16_t wChainId;
		int16_t wChainDepth;
		int16_t wChainParent;
		uint16_t totalLinkCount;
		x86::pointer<pathlink_s> Links;
	};

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
		pathnode_transient_t transient;
	};

	struct PathData
	{
		unsigned int nodeCount;
		x86::pointer<pathnode_t> nodes;
		x86::pointer<pathbasenode_t> basenodes;
		unsigned int chainNodeCount;
		x86::pointer<uint16_t> chainNodeForNode;
		x86::pointer<uint16_t> nodeForChainNode;
		int visBytes;
		x86::pointer<char> pathVis;
		int nodeTreeCount;
		x86::pointer<pathnode_tree_t> nodeTree;
	};

	struct GameWorldSp
	{
		x86::pointer<const char> name;
		PathData path;
	};

	struct GameWorldMp
	{
		x86::pointer<const char> name;
	};

	struct MapEnts
	{
		x86::pointer<const char> name;
		x86::pointer<char> entityString;
		int numEntityChars;
	};

	struct GfxWorldVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		float texCoord[2];
		float lmapCoord[2];
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct GfxWorldVertexData
	{
		x86::pointer<GfxWorldVertex> vertices;
		x86::pointer<void> worldVb;
	};

	struct GfxWorldVertexLayerData
	{
		x86::pointer<char> data;
		x86::pointer<void> layerVb;
	};

	struct SunLightParseParams
	{
		char name[64];
		float ambientScale;
		float ambientColor[3];
		float diffuseFraction;
		float sunLight;
		float sunColor[3];
		float diffuseColor[3];
		bool diffuseColorHasBeenSet;
		float angles[3];
	};

	struct GfxLightImage
	{
		x86::pointer<GfxImage> image;
		char samplerState;
	};

	struct GfxLightDef
	{
		x86::pointer<const char> name;
		GfxLightImage attenuation;
		int lmapLookupStart;
	};

	struct GfxLight
	{
		char type;
		char canUseShadowMap;
		char unused[2];
		float color[3];
		float dir[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		int exponent;
		unsigned int spotShadowIndex;
		x86::pointer<GfxLightDef> def;
	};

	struct GfxReflectionProbe
	{
		float origin[3];
		x86::pointer<GfxImage> reflectionImage;
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount;
		x86::pointer<cplane_s> planes;
		x86::pointer<uint16_t> nodes;
		x86::pointer<unsigned int> sceneEntCellBits;
	};

	struct DpvsPlane
	{
		float coeffs[4];
		char side[3];
		char pad;
	};

	struct GfxCell;
	struct GfxPortal;

	struct GfxPortalWritable
	{
		bool isQueued;
		bool isAncestor;
		char recursionDepth;
		char hullPointCount;
		x86::pointer<float[2]> hullPoints;
		x86::pointer<GfxPortal> queuedParent;
	};

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		x86::pointer<GfxCell> cell;
		x86::pointer<vec3_t> vertices;
		char vertexCount;
		float hullAxis[2][3];
	};

	struct GfxAabbTree
	{
		float mins[3];
		float maxs[3];
		uint16_t childCount;
		uint16_t surfaceCount;
		uint16_t startSurfIndex;
		uint16_t surfaceCountNoDecal;
		uint16_t startSurfIndexNoDecal;
		uint16_t smodelIndexCount;
		x86::pointer<uint16_t> smodelIndexes;
		int childrenOffset;
	};

	struct GfxCell
	{
		float mins[3];
		float maxs[3];
		int aabbTreeCount;
		x86::pointer<GfxAabbTree> aabbTree;
		int portalCount;
		x86::pointer<GfxPortal> portals;
		int cullGroupCount;
		x86::pointer<int> cullGroups;
		char reflectionProbeCount;
		x86::pointer<char> reflectionProbes;
	};

	struct GfxLightmapArray
	{
		x86::pointer<GfxImage> primary;
		x86::pointer<GfxImage> secondary;
	};

	struct alignas(4) GfxLightGridEntry
	{
		uint16_t colorsIndex;
		char primaryLightIndex;
		char needsTrace;
	};

	struct alignas(4) GfxLightGridColors
	{
		char rgb[56][3];
	};

	struct GfxLightGrid
	{
		bool hasLightRegions;
		unsigned int sunPrimaryLightIndex;
		uint16_t mins[3];
		uint16_t maxs[3];
		unsigned int rowAxis;
		unsigned int colAxis;
		x86::pointer<uint16_t> rowDataStart;
		unsigned int rawRowDataSize;
		x86::pointer<char> rawRowData;
		unsigned int entryCount;
		x86::pointer<GfxLightGridEntry> entries;
		unsigned int colorCount;
		x86::pointer<GfxLightGridColors> colors;
	};

	struct GfxBrushModelWritable
	{
		float mins[3];
		float maxs[3];
	};

	struct GfxBrushModel
	{
		GfxBrushModelWritable writable;
		float bounds[2][3];
		uint16_t surfaceCount;
		uint16_t startSurfIndex;
		uint16_t surfaceCountNoDecal;
	};

	struct MaterialMemory
	{
		x86::pointer<Material> material;
		int memory;
	};

	struct sunflare_t
	{
		bool hasValidData;
		x86::pointer<Material> spriteMaterial;
		x86::pointer<Material> flareMaterial;
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

	struct XModelDrawInfo
	{
		uint16_t lod;
		uint16_t surfId;
	};

	struct GfxSceneDynModel
	{
		XModelDrawInfo info;
		uint16_t dynEntId;
	};

	struct BModelDrawInfo
	{
		uint16_t surfId;
	};

	struct alignas(4) GfxSceneDynBrush
	{
		BModelDrawInfo info;
		uint16_t dynEntId;
	};

	struct GfxShadowGeometry
	{
		uint16_t surfaceCount;
		uint16_t smodelCount;
		x86::pointer<uint16_t> sortedSurfIndex;
		x86::pointer<uint16_t> smodelIndex;
	};

	struct GfxLightRegionAxis
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct GfxLightRegionHull
	{
		float kdopMidPoint[9];
		float kdopHalfSize[9];
		unsigned int axisCount;
		x86::pointer<GfxLightRegionAxis> axis;
	};

	struct GfxLightRegion
	{
		unsigned int hullCount;
		x86::pointer<GfxLightRegionHull> hulls;
	};

	struct GfxPackedPlacement
	{
		float origin[3];
		float axis[3][3];
		float scale;
	};

	struct GfxStaticModelDrawInst
	{
		float cullDist;
		GfxPackedPlacement placement;
		x86::pointer<XModel> model;
		uint16_t smodelCacheIndex[4];
		char reflectionProbeIndex;
		char primaryLightIndex;
		uint16_t lightingHandle;
		char flags;
	};

	struct GfxStaticModelInst
	{
		float mins[3];
		float maxs[3];
		GfxColor groundLighting;
	};

	struct srfTriangles_t
	{
		int vertexLayerData;
		int firstVertex;
		uint16_t vertexCount;
		uint16_t triCount;
		int baseIndex;
	};

	struct GfxSurface
	{
		srfTriangles_t tris;
		x86::pointer<Material> material;
		char lightmapIndex;
		char reflectionProbeIndex;
		char primaryLightIndex;
		char flags;
		float bounds[2][3];
	};

	struct GfxCullGroup
	{
		float mins[3];
		float maxs[3];
		int surfaceCount;
		int startSurfIndex;
	};

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount;
		unsigned int staticSurfaceCount;
		unsigned int staticSurfaceCountNoDecal;
		unsigned int litSurfsBegin;
		unsigned int litSurfsEnd;
		unsigned int decalSurfsBegin;
		unsigned int decalSurfsEnd;
		unsigned int emissiveSurfsBegin;
		unsigned int emissiveSurfsEnd;
		unsigned int smodelVisDataCount;
		unsigned int surfaceVisDataCount;
		x86::pointer<char> smodelVisData[3];
		x86::pointer<char> surfaceVisData[3];
		x86::pointer<raw_uint128> lodData;
		x86::pointer<uint16_t> sortedSurfIndex;
		x86::pointer<GfxStaticModelInst> smodelInsts;
		x86::pointer<GfxSurface> surfaces;
		x86::pointer<GfxCullGroup> cullGroups;
		x86::pointer<GfxStaticModelDrawInst> smodelDrawInsts;
		x86::pointer<GfxDrawSurf> surfaceMaterials;
		x86::pointer<raw_uint128> surfaceCastsSunShadow;
		volatile int usageCount;
	};

	struct GfxWorldDpvsDynamic
	{
		unsigned int dynEntClientWordCount[DYNENT_DRAW_COUNT];
		unsigned int dynEntClientCount[DYNENT_DRAW_COUNT];
		x86::pointer<unsigned int> dynEntCellBits[DYNENT_DRAW_COUNT];
		x86::pointer<raw_byte16> dynEntVisData[DYNENT_DRAW_COUNT][3];
	};

	struct GfxWorldStreamInfo
	{
		char pad;
	};

	typedef GfxSceneDynModel GfxSceneDynModel4;

	struct GfxWorld
	{
		x86::pointer<const char> name;
		x86::pointer<const char> baseName;
		int planeCount;
		int nodeCount;
		int indexCount;
		x86::pointer<uint16_t> indices;
		int surfaceCount;
		GfxWorldStreamInfo streamInfo;
		int skySurfCount;
		x86::pointer<int> skyStartSurfs;
		x86::pointer<GfxImage> skyImage;
		char skySamplerState;
		unsigned int vertexCount;
		GfxWorldVertexData vd;
		unsigned int vertexLayerDataSize;
		GfxWorldVertexLayerData vld;
		SunLightParseParams sunParse;
		x86::pointer<GfxLight> sunLight;
		float sunColorFromBsp[3];
		unsigned int sunPrimaryLightIndex;
		unsigned int primaryLightCount;
		int cullGroupCount;
		unsigned int reflectionProbeCount;
		x86::pointer<GfxReflectionProbe> reflectionProbes;
		x86::pointer<GfxTexture> reflectionProbeTextures;
		GfxWorldDpvsPlanes dpvsPlanes;
		int cellBitsCount;
		x86::pointer<GfxCell> cells;
		int lightmapCount;
		x86::pointer<GfxLightmapArray> lightmaps;
		GfxLightGrid lightGrid;
		x86::pointer<GfxTexture> lightmapPrimaryTextures;
		x86::pointer<GfxTexture> lightmapSecondaryTextures;
		int modelCount;
		x86::pointer<GfxBrushModel> models;
		float mins[3];
		float maxs[3];
		unsigned int checksum;
		int materialMemoryCount;
		x86::pointer<MaterialMemory> materialMemory;
		sunflare_t sun;
		float outdoorLookupMatrix[4][4];
		x86::pointer<GfxImage> outdoorImage;
		x86::pointer<unsigned int> cellCasterBits;
		x86::pointer<GfxSceneDynModel4> sceneDynModel;
		x86::pointer<GfxSceneDynBrush> sceneDynBrush;
		x86::pointer<unsigned int> primaryLightEntityShadowVis;
		x86::pointer<unsigned int> primaryLightDynEntShadowVis[DYNENT_DRAW_COUNT];
		x86::pointer<char> nonSunPrimaryLightForModelDynEnt;
		x86::pointer<GfxShadowGeometry> shadowGeom;
		x86::pointer<GfxLightRegion> lightRegion;
		GfxWorldDpvsStatic dpvs;
		GfxWorldDpvsDynamic dpvsDyn;
	};

	struct Glyph
	{
		uint16_t letter;
		char x0;
		char y0;
		unsigned char dx;
		unsigned char pixelWidth;
		unsigned char pixelHeight;
		float s0;
		float t0;
		float s1;
		float t1;
	};

	struct Font_s
	{
		x86::pointer<const char> fontName;
		int pixelHeight;
		int glyphCount;
		x86::pointer<Material> material;
		x86::pointer<Material> glowMaterial;
		x86::pointer<Glyph> glyphs;
	};

	struct MenuList
	{
		x86::pointer<const char> name;
		int menuCount;
		x86::pointer<x86::pointer<menuDef_t>> menus;
	};

	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		int horzAlign;
		int vertAlign;
	};

	enum WindowDefStaticFlag : unsigned int
	{
		WINDOW_FLAG_DECORATION = 0x100000,
		WINDOW_FLAG_HORIZONTAL_SCROLL = 0x200000,
		WINDOW_FLAG_AUTO_WRAPPED = 0x800000,
		WINDOW_FLAG_POPUP = 0x1000000,
		WINDOW_FLAG_OUT_OF_BOUNDS_CLICK = 0x2000000,
		WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE = 0x4000000,
		WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG = 0x10000000,
		WINDOW_FLAG_HIDDEN_DURING_SCOPE = 0x20000000,
		WINDOW_FLAG_HIDDEN_DURING_UI = 0x40000000,
	};

	enum WindowDefDynamicFlag : unsigned int
	{
		WINDOW_FLAG_MOUSE_OVER = 0x1,
		WINDOW_FLAG_FOCUSED = 0x2,
		WINDOW_FLAG_VISIBLE = 0x4,
		WINDOW_FLAG_FADING_OUT = 0x10,
		WINDOW_FLAG_FADING_IN = 0x20,
		WINDOW_FLAG_MOUSE_OVER_TEXT = 0x40,
		WINDOW_FLAG_LISTBOX_PREV_ARROW = 0x100,
		WINDOW_FLAG_LISTBOX_NEXT_ARROW = 0x200,
		WINDOW_FLAG_LISTBOX_THUMB = 0x400,
		WINDOW_FLAG_LISTBOX_PREV_PAGE = 0x800,
		WINDOW_FLAG_LISTBOX_NEXT_PAGE = 0x1000,
		WINDOW_FLAG_FORCED_VISIBLE = 0x4000,
		WINDOW_FLAG_NON_DEFAULT_BACKCOLOR = 0x8000,
		WINDOW_FLAG_NON_DEFAULT_FORECOLOR = 0x10000,
	};

	struct windowDef_t
	{
		x86::pointer<const char> name;
		rectDef_s rect;
		rectDef_s rectClient;
		x86::pointer<const char> group;
		int style;
		int border;
		int ownerDraw;
		unsigned int ownerDrawFlags;
		float borderSize;
		unsigned int staticFlags;
		unsigned int dynamicFlags[1];
		int nextTime;
		float foreColor[4];
		float backColor[4];
		float borderColor[4];
		float outlineColor[4];
		x86::pointer<Material> background;
	};

	struct ItemKeyHandler
	{
		int key;
		x86::pointer<const char> action;
		x86::pointer<ItemKeyHandler> next;
	};

	union operandInternalDataUnion
	{
		int intVal;
		float floatVal;
		x86::pointer<const char> stringVal;
	};

	enum expDataType
	{
		VAL_INT = 0x0,
		VAL_FLOAT = 0x1,
		VAL_STRING = 0x2,
	};

	struct Operand
	{
		expDataType dataType;
		operandInternalDataUnion internals;
	};

	enum operationEnum : int
	{
		OP_NOOP,
		OP_RIGHTPAREN,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_MODULUS,
		OP_ADD,
		OP_SUBTRACT,
		OP_NOT,
		OP_LESSTHAN,
		OP_LESSTHANEQUALTO,
		OP_GREATERTHAN,
		OP_GREATERTHANEQUALTO,
		OP_EQUALS,
		OP_NOTEQUAL,
		OP_AND,
		OP_OR,
		OP_LEFTPAREN,
		OP_COMMA,
		OP_BITWISEAND,
		OP_BITWISEOR,
		OP_BITWISENOT,
		OP_BITSHIFTLEFT,
		OP_BITSHIFTRIGHT,

		OP_FIRSTFUNCTIONCALL,
		OP_SIN = OP_FIRSTFUNCTIONCALL,
		OP_COS,
		OP_MIN,
		OP_MAX,
		OP_MILLISECONDS,
		OP_DVARINT,
		OP_DVARBOOL,
		OP_DVARFLOAT,
		OP_DVARSTRING,
		OP_STAT,
		OP_UIACTIVE,
		OP_FLASHBANGED,
		OP_SCOPED,
		OP_SCOREBOARDVISIBLE,
		OP_INKILLCAM,
		OP_PLAYERFIELD,
		OP_SELECTINGLOCATION,
		OP_TEAMFIELD,
		OP_OTHERTEAMFIELD,
		OP_MARINESFIELD,
		OP_OPFORFIELD,
		OP_MENUISOPEN,
		OP_WRITINGDATA,
		OP_INLOBBY,
		OP_INPRIVATEPARTY,
		OP_PRIVATEPARTYHOST,
		OP_PRIVATEPARTYHOSTINLOBBY,
		OP_ALONEINPARTY,
		OP_ADSJAVELIN,
		OP_WEAPLOCKBLINK,
		OP_WEAPATTACKTOP,
		OP_WEAPATTACKDIRECT,
		OP_SECONDSASTIME,
		OP_TABLELOOKUP,
		OP_LOCALIZESTRING,
		OP_LOCALVARINT,
		OP_LOCALVARBOOL,
		OP_LOCALVARFLOAT,
		OP_LOCALVARSTRING,
		OP_TIMELEFT,
		OP_SECONDSASCOUNTDOWN,
		OP_GAMEMSGWNDACTIVE,
		OP_TOINT,
		OP_TOSTRING,
		OP_TOFLOAT,
		OP_GAMETYPENAME,
		OP_GAMETYPE,
		OP_GAMETYPEDESCRIPTION,
		OP_SCORE,
		OP_FRIENDSONLINE,
		OP_FOLLOWING,
		OP_STATRANGEBITSSET,
		OP_KEYBINDING,
		OP_ACTIONSLOTUSABLE,
		OP_HUDFADE,
		OP_MAXPLAYERS,
		OP_ACCEPTINGINVITE,
		OP_ISINTERMISSION,

		NUM_OPERATORS,
	};

	union entryInternalData
	{
		Operand operand;
		operationEnum op;
	};

	enum expressionEntryType : int
	{
		EET_OPERATOR = 0x0,
		EET_OPERAND = 0x1,
	};

	struct expressionEntry
	{
		int type;
		entryInternalData data;
	};

	struct statement_s
	{
		int numEntries;
		x86::pointer<x86::pointer<expressionEntry>> entries;
	};

	struct columnInfo_s
	{
		int pos;
		int width;
		int maxChars;
		int alignment;
	};

	struct listBoxDef_s
	{
		int mousePos;
		int startPos[1];
		int endPos[1];
		int drawPadding;
		float elementWidth;
		float elementHeight;
		int elementStyle;
		int numColumns;
		columnInfo_s columnInfo[16];
		x86::pointer<const char> onDoubleClick;
		int notselectable;
		int noScrollBars;
		int usePaging;
		float selectBorder[4];
		float disableColor[4];
		x86::pointer<Material> selectIcon;
	};

	struct editFieldDef_s
	{
		float minVal;
		float maxVal;
		float defVal;
		float range;
		int maxChars;
		int maxCharsGotoNext;
		int maxPaintChars;
		int paintOffset;
	};

	struct multiDef_s
	{
		x86::pointer<const char> dvarList[32];
		x86::pointer<const char> dvarStr[32];
		float dvarValue[32];
		int count;
		int strDef;
	};

	union itemDefData_t
	{
		x86::pointer<listBoxDef_s> listBox;
		x86::pointer<editFieldDef_s> editField;
		x86::pointer<multiDef_s> multi;
		x86::pointer<const char> enumDvarName;
		x86::pointer<void> data;
	};

	enum ItemDefType
	{
		ITEM_TYPE_TEXT = 0x0,
		ITEM_TYPE_BUTTON = 0x1,
		ITEM_TYPE_RADIOBUTTON = 0x2,
		ITEM_TYPE_CHECKBOX = 0x3,
		ITEM_TYPE_EDITFIELD = 0x4,
		ITEM_TYPE_COMBO = 0x5,
		ITEM_TYPE_LISTBOX = 0x6,
		ITEM_TYPE_MODEL = 0x7,
		ITEM_TYPE_OWNERDRAW = 0x8,
		ITEM_TYPE_NUMERICFIELD = 0x9,
		ITEM_TYPE_SLIDER = 0xA,
		ITEM_TYPE_YESNO = 0xB,
		ITEM_TYPE_MULTI = 0xC,
		ITEM_TYPE_DVARENUM = 0xD,
		ITEM_TYPE_BIND = 0xE,
		ITEM_TYPE_MENUMODEL = 0xF,
		ITEM_TYPE_VALIDFILEFIELD = 0x10,
		ITEM_TYPE_DECIMALFIELD = 0x11,
		ITEM_TYPE_UPREDITFIELD = 0x12,
		ITEM_TYPE_GAME_MESSAGE_WINDOW = 0x13
	};

	enum ItemDefFlag : unsigned int
	{
		ITEM_FLAG_SAVE_GAME_INFO = 0x1,
		ITEM_FLAG_CINEMATIC_SUBTITLE = 0x2,
	};

	enum ItemDefDvarFlag
	{
		ITEM_DVAR_FLAG_ENABLE = 0x1,
		ITEM_DVAR_FLAG_DISABLE = 0x2,
		ITEM_DVAR_FLAG_SHOW = 0x4,
		ITEM_DVAR_FLAG_HIDE = 0x8,
		ITEM_DVAR_FLAG_FOCUS = 0x10,
	};

	struct itemDef_s
	{
		windowDef_t window;
		rectDef_s textRect[1];
		int type;
		int dataType;
		int alignment;
		int fontEnum;
		int textAlignMode;
		float textalignx;
		float textaligny;
		float textscale;
		int textStyle;
		int gameMsgWindowIndex;
		int gameMsgWindowMode;
		x86::pointer<const char> text;
		unsigned int itemFlags;
		x86::pointer<menuDef_t> parent;
		x86::pointer<const char> mouseEnterText;
		x86::pointer<const char> mouseExitText;
		x86::pointer<const char> mouseEnter;
		x86::pointer<const char> mouseExit;
		x86::pointer<const char> action;
		x86::pointer<const char> onAccept;
		x86::pointer<const char> onFocus;
		x86::pointer<const char> leaveFocus;
		x86::pointer<const char> dvar;
		x86::pointer<const char> dvarTest;
		x86::pointer<ItemKeyHandler> onKey;
		x86::pointer<const char> enableDvar;
		int dvarFlags;
		x86::pointer<snd_alias_list_t> focusSound;
		float special;
		int cursorPos[1];
		itemDefData_t typeData;
		int imageTrack;
		statement_s visibleExp;
		statement_s textExp;
		statement_s materialExp;
		statement_s rectXExp;
		statement_s rectYExp;
		statement_s rectWExp;
		statement_s rectHExp;
		statement_s forecolorAExp;
	};

	struct menuDef_t
	{
		windowDef_t window;
		x86::pointer<const char> font;
		int fullScreen;
		int itemCount;
		int fontIndex;
		int cursorItem[1];
		int fadeCycle;
		float fadeClamp;
		float fadeAmount;
		float fadeInAmount;
		float blurRadius;
		x86::pointer<const char> onOpen;
		x86::pointer<const char> onClose;
		x86::pointer<const char> onESC;
		x86::pointer<ItemKeyHandler> onKey;
		statement_s visibleExp;
		x86::pointer<const char> allowedBinding;
		x86::pointer<const char> soundName;
		int imageTrack;
		float focusColor[4];
		float disableColor[4];
		statement_s rectXExp;
		statement_s rectYExp;
		x86::pointer<x86::pointer<itemDef_s>> items;
	};

	struct LocalizeEntry
	{
		x86::pointer<const char> value;
		x86::pointer<const char> name;
	};

	enum weapType_t
	{
		WEAPTYPE_BULLET = 0x0,
		WEAPTYPE_GRENADE = 0x1,
		WEAPTYPE_PROJECTILE = 0x2,
		WEAPTYPE_BINOCULARS = 0x3,

		WEAPTYPE_NUM,
	};

	enum weapClass_t
	{
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_MG = 0x1,
		WEAPCLASS_SMG = 0x2,
		WEAPCLASS_SPREAD = 0x3,
		WEAPCLASS_PISTOL = 0x4,
		WEAPCLASS_GRENADE = 0x5,
		WEAPCLASS_ROCKETLAUNCHER = 0x6,
		WEAPCLASS_TURRET = 0x7,
		WEAPCLASS_NON_PLAYER = 0x8,
		WEAPCLASS_ITEM = 0x9,

		WEAPCLASS_NUM,
	};

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,

		PENETRATE_TYPE_NUM,
	};

	enum ImpactType
	{
		IMPACT_TYPE_NONE = 0x0,
		IMPACT_TYPE_BULLET_SMALL = 0x1,
		IMPACT_TYPE_BULLET_LARGE = 0x2,
		IMPACT_TYPE_BULLET_AP = 0x3,
		IMPACT_TYPE_SHOTGUN = 0x4,
		IMPACT_TYPE_GRENADE_BOUNCE = 0x5,
		IMPACT_TYPE_GRENADE_EXPLODE = 0x6,
		IMPACT_TYPE_ROCKET_EXPLODE = 0x7,
		IMPACT_TYPE_PROJECTILE_DUD = 0x8,

		IMPACT_TYPE_NUM,
	};

	enum weapInventoryType_t
	{
		WEAPINVENTORY_PRIMARY = 0x0,
		WEAPINVENTORY_OFFHAND = 0x1,
		WEAPINVENTORY_ITEM = 0x2,
		WEAPINVENTORY_ALTMODE = 0x3,

		WEAPINVENTORY_NUM,
	};

	enum weapFireType_t
	{
		WEAPON_FIRETYPE_FULLAUTO = 0x0,
		WEAPON_FIRETYPE_SINGLESHOT = 0x1,
		WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
		WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
		WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,

		WEAPON_FIRETYPE_NUM,
	};

	enum OffhandClass
	{
		OFFHAND_CLASS_NONE = 0x0,
		OFFHAND_CLASS_FRAG_GRENADE = 0x1,
		OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
		OFFHAND_CLASS_FLASH_GRENADE = 0x3,

		OFFHAND_CLASS_NUM,
	};

	enum weapStance_t
	{
		WEAPSTANCE_STAND = 0x0,
		WEAPSTANCE_DUCK = 0x1,
		WEAPSTANCE_PRONE = 0x2,

		WEAPSTANCE_NUM,
	};

	enum activeReticleType_t
	{
		VEH_ACTIVE_RETICLE_NONE = 0x0,
		VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
		VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,

		VEH_ACTIVE_RETICLE_NUM,
	};

	enum weaponIconRatioType_t
	{
		WEAPON_ICON_RATIO_1TO1 = 0x0,
		WEAPON_ICON_RATIO_2TO1 = 0x1,
		WEAPON_ICON_RATIO_4TO1 = 0x2,

		WEAPON_ICON_RATIO_NUM,
	};

	enum ammoCounterClipType_t
	{
		AMMO_COUNTER_CLIP_NONE = 0x0,
		AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
		AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
		AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
		AMMO_COUNTER_CLIP_ROCKET = 0x4,
		AMMO_COUNTER_CLIP_BELTFED = 0x5,
		AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,

		AMMO_COUNTER_CLIP_NUM,
	};

	enum weapOverlayReticle_t
	{
		WEAPOVERLAYRETICLE_NONE = 0x0,
		WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,

		WEAPOVERLAYRETICLE_NUM,
	};

	enum WeapOverlayInterface_t
	{
		WEAPOVERLAYINTERFACE_NONE = 0x0,
		WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
		WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,

		WEAPOVERLAYINTERFACE_NUM,
	};

	enum weapProjExposion_t
	{
		WEAPPROJEXP_GRENADE = 0x0,
		WEAPPROJEXP_ROCKET = 0x1,
		WEAPPROJEXP_FLASHBANG = 0x2,
		WEAPPROJEXP_NONE = 0x3,
		WEAPPROJEXP_DUD = 0x4,
		WEAPPROJEXP_SMOKE = 0x5,
		WEAPPROJEXP_HEAVY = 0x6,

		WEAPPROJEXP_NUM,
	};

	enum WeapStickinessType
	{
		WEAPSTICKINESS_NONE = 0x0,
		WEAPSTICKINESS_ALL = 0x1,
		WEAPSTICKINESS_GROUND = 0x2,
		WEAPSTICKINESS_GROUND_WITH_YAW = 0x3,

		WEAPSTICKINESS_NUM,
	};

	enum guidedMissileType_t
	{
		MISSILE_GUIDANCE_NONE = 0x0,
		MISSILE_GUIDANCE_SIDEWINDER = 0x1,
		MISSILE_GUIDANCE_HELLFIRE = 0x2,
		MISSILE_GUIDANCE_JAVELIN = 0x3,

		MISSILE_GUIDANCE_NUM,
	};

	enum WeaponAnimSlot
	{
		WEAP_ANIM_ROOT = 0x0,
		WEAP_ANIM_IDLE = 0x1,
		WEAP_ANIM_EMPTY_IDLE = 0x2,
		WEAP_ANIM_FIRE = 0x3,
		WEAP_ANIM_HOLD_FIRE = 0x4,
		WEAP_ANIM_LASTSHOT = 0x5,
		WEAP_ANIM_RECHAMBER = 0x6,
		WEAP_ANIM_MELEE = 0x7,
		WEAP_ANIM_MELEE_CHARGE = 0x8,
		WEAP_ANIM_RELOAD = 0x9,
		WEAP_ANIM_RELOAD_EMPTY = 0xA,
		WEAP_ANIM_RELOAD_START = 0xB,
		WEAP_ANIM_RELOAD_END = 0xC,
		WEAP_ANIM_RAISE = 0xD,
		WEAP_ANIM_FIRST_RAISE = 0xE,
		WEAP_ANIM_DROP = 0xF,
		WEAP_ANIM_ALT_RAISE = 0x10,
		WEAP_ANIM_ALT_DROP = 0x11,
		WEAP_ANIM_QUICK_RAISE = 0x12,
		WEAP_ANIM_QUICK_DROP = 0x13,
		WEAP_ANIM_EMPTY_RAISE = 0x14,
		WEAP_ANIM_EMPTY_DROP = 0x15,
		WEAP_ANIM_SPRINT_IN = 0x16,
		WEAP_ANIM_SPRINT_LOOP = 0x17,
		WEAP_ANIM_SPRINT_OUT = 0x18,
		WEAP_ANIM_DETONATE = 0x19,
		WEAP_ANIM_NIGHTVISION_WEAR = 0x1A,
		WEAP_ANIM_NIGHTVISION_REMOVE = 0x1B,
		WEAP_ANIM_ADS_FIRE = 0x1C,
		WEAP_ANIM_ADS_LASTSHOT = 0x1D,
		WEAP_ANIM_ADS_RECHAMBER = 0x1E,
		WEAP_ANIM_ADS_UP = 0x1F,
		WEAP_ANIM_ADS_DOWN = 0x20,

		NUM_WEAP_ANIMS,
	};

	enum hitLocation_t
	{
		HITLOC_NONE = 0x0,
		HITLOC_HELMET = 0x1,
		HITLOC_HEAD = 0x2,
		HITLOC_NECK = 0x3,
		HITLOC_TORSO_UPR = 0x4,
		HITLOC_TORSO_LWR = 0x5,
		HITLOC_R_ARM_UPR = 0x6,
		HITLOC_L_ARM_UPR = 0x7,
		HITLOC_R_ARM_LWR = 0x8,
		HITLOC_L_ARM_LWR = 0x9,
		HITLOC_R_HAND = 0xA,
		HITLOC_L_HAND = 0xB,
		HITLOC_R_LEG_UPR = 0xC,
		HITLOC_L_LEG_UPR = 0xD,
		HITLOC_R_LEG_LWR = 0xE,
		HITLOC_L_LEG_LWR = 0xF,
		HITLOC_R_FOOT = 0x10,
		HITLOC_L_FOOT = 0x11,
		HITLOC_GUN = 0x12,

		HITLOC_COUNT,
	};

	struct snd_alias_list_name
	{
		x86::pointer<const char> soundName;
	};

	union SndAliasCustom
	{
		x86::pointer<snd_alias_list_name> name;
		x86::pointer<snd_alias_list_t> sound;
	};

	enum weaponAltModel_t
	{
		WEAP_ALT_MODEL0 = 0x0,
		WEAP_ALT_MODEL1 = 0x1,
		WEAP_ALT_MODEL2 = 0x2,
		WEAP_ALT_MODEL3 = 0x3,
		WEAP_ALT_MODEL4 = 0x4,
		WEAP_ALT_MODEL5 = 0x5,
		WEAP_ALT_MODEL6 = 0x6,
		WEAP_ALT_MODEL7 = 0x7,
		WEAP_ALT_MODEL8 = 0x8,
		WEAP_ALT_MODEL9 = 0x9,
		WEAP_ALT_MODEL10 = 0xA,
		WEAP_ALT_MODEL11 = 0xB,
		WEAP_ALT_MODEL12 = 0xC,
		WEAP_ALT_MODEL13 = 0xD,
		WEAP_ALT_MODEL14 = 0xE,
		WEAP_ALT_MODEL15 = 0xF,

		NUM_WEAP_ALTMODELS,
	};

	struct WeaponDef
	{
		x86::pointer<const char> szInternalName;
		x86::pointer<const char> szDisplayName;
		x86::pointer<const char> szOverlayName;
		x86::pointer<XModel> gunXModel[NUM_WEAP_ALTMODELS];
		x86::pointer<XModel> handXModel;
		x86::pointer<const char> szXAnims[NUM_WEAP_ANIMS];
		x86::pointer<const char> szModeName;
		uint16_t hideTags[8];
		uint16_t notetrackSoundMapKeys[16];
		uint16_t notetrackSoundMapValues[16];
		int playerAnimType;
		weapType_t weapType;
		weapClass_t weapClass;
		PenetrateType penetrateType;
		ImpactType impactType;
		weapInventoryType_t inventoryType;
		weapFireType_t fireType;
		OffhandClass offhandClass;
		weapStance_t stance;
		x86::pointer<FxEffectDef> viewFlashEffect;
		x86::pointer<FxEffectDef> worldFlashEffect;
		SndAliasCustom pickupSound;
		SndAliasCustom pickupSoundPlayer;
		SndAliasCustom ammoPickupSound;
		SndAliasCustom ammoPickupSoundPlayer;
		SndAliasCustom projectileSound;
		SndAliasCustom pullbackSound;
		SndAliasCustom pullbackSoundPlayer;
		SndAliasCustom fireSound;
		SndAliasCustom fireSoundPlayer;
		SndAliasCustom fireLoopSound;
		SndAliasCustom fireLoopSoundPlayer;
		SndAliasCustom fireStopSound;
		SndAliasCustom fireStopSoundPlayer;
		SndAliasCustom fireLastSound;
		SndAliasCustom fireLastSoundPlayer;
		SndAliasCustom emptyFireSound;
		SndAliasCustom emptyFireSoundPlayer;
		SndAliasCustom meleeSwipeSound;
		SndAliasCustom meleeSwipeSoundPlayer;
		SndAliasCustom meleeHitSound;
		SndAliasCustom meleeMissSound;
		SndAliasCustom rechamberSound;
		SndAliasCustom rechamberSoundPlayer;
		SndAliasCustom reloadSound;
		SndAliasCustom reloadSoundPlayer;
		SndAliasCustom reloadEmptySound;
		SndAliasCustom reloadEmptySoundPlayer;
		SndAliasCustom reloadStartSound;
		SndAliasCustom reloadStartSoundPlayer;
		SndAliasCustom reloadEndSound;
		SndAliasCustom reloadEndSoundPlayer;
		SndAliasCustom detonateSound;
		SndAliasCustom detonateSoundPlayer;
		SndAliasCustom nightVisionWearSound;
		SndAliasCustom nightVisionWearSoundPlayer;
		SndAliasCustom nightVisionRemoveSound;
		SndAliasCustom nightVisionRemoveSoundPlayer;
		SndAliasCustom altSwitchSound;
		SndAliasCustom altSwitchSoundPlayer;
		SndAliasCustom raiseSound;
		SndAliasCustom raiseSoundPlayer;
		SndAliasCustom firstRaiseSound;
		SndAliasCustom firstRaiseSoundPlayer;
		SndAliasCustom putawaySound;
		SndAliasCustom putawaySoundPlayer;
		x86::pointer<SndAliasCustom> bounceSound;
		x86::pointer<FxEffectDef> viewShellEjectEffect;
		x86::pointer<FxEffectDef> worldShellEjectEffect;
		x86::pointer<FxEffectDef> viewLastShotEjectEffect;
		x86::pointer<FxEffectDef> worldLastShotEjectEffect;
		x86::pointer<Material> reticleCenter;
		x86::pointer<Material> reticleSide;
		int iReticleCenterSize;
		int iReticleSideSize;
		int iReticleMinOfs;
		activeReticleType_t activeReticleType;
		float vStandMove[3];
		float vStandRot[3];
		float vDuckedOfs[3];
		float vDuckedMove[3];
		float vDuckedRot[3];
		float vProneOfs[3];
		float vProneMove[3];
		float vProneRot[3];
		float fPosMoveRate;
		float fPosProneMoveRate;
		float fStandMoveMinSpeed;
		float fDuckedMoveMinSpeed;
		float fProneMoveMinSpeed;
		float fPosRotRate;
		float fPosProneRotRate;
		float fStandRotMinSpeed;
		float fDuckedRotMinSpeed;
		float fProneRotMinSpeed;
		x86::pointer<XModel> worldModel[NUM_WEAP_ALTMODELS];
		x86::pointer<XModel> worldClipModel;
		x86::pointer<XModel> rocketModel;
		x86::pointer<XModel> knifeModel;
		x86::pointer<XModel> worldKnifeModel;
		x86::pointer<Material> hudIcon;
		weaponIconRatioType_t hudIconRatio;
		x86::pointer<Material> ammoCounterIcon;
		weaponIconRatioType_t ammoCounterIconRatio;
		ammoCounterClipType_t ammoCounterClip;
		int iStartAmmo;
		x86::pointer<const char> szAmmoName;
		int iAmmoIndex;
		x86::pointer<const char> szClipName;
		int iClipIndex;
		int iMaxAmmo;
		int iClipSize;
		int shotCount;
		x86::pointer<const char> szSharedAmmoCapName;
		int iSharedAmmoCapIndex;
		int iSharedAmmoCap;
		int damage;
		int playerDamage;
		int iMeleeDamage;
		int iDamageType;
		int iFireDelay;
		int iMeleeDelay;
		int meleeChargeDelay;
		int iDetonateDelay;
		int iFireTime;
		int iRechamberTime;
		int iRechamberBoltTime;
		int iHoldFireTime;
		int iDetonateTime;
		int iMeleeTime;
		int meleeChargeTime;
		int iReloadTime;
		int reloadShowRocketTime;
		int iReloadEmptyTime;
		int iReloadAddTime;
		int iReloadStartTime;
		int iReloadStartAddTime;
		int iReloadEndTime;
		int iDropTime;
		int iRaiseTime;
		int iAltDropTime;
		int iAltRaiseTime;
		int quickDropTime;
		int quickRaiseTime;
		int iFirstRaiseTime;
		int iEmptyRaiseTime;
		int iEmptyDropTime;
		int sprintInTime;
		int sprintLoopTime;
		int sprintOutTime;
		int nightVisionWearTime;
		int nightVisionWearTimeFadeOutEnd;
		int nightVisionWearTimePowerUp;
		int nightVisionRemoveTime;
		int nightVisionRemoveTimePowerDown;
		int nightVisionRemoveTimeFadeInStart;
		int fuseTime;
		int aiFuseTime;
		int requireLockonToFire;
		int noAdsWhenMagEmpty;
		int avoidDropCleanup;
		float autoAimRange;
		float aimAssistRange;
		float aimAssistRangeAds;
		float aimPadding;
		float enemyCrosshairRange;
		int crosshairColorChange;
		float moveSpeedScale;
		float adsMoveSpeedScale;
		float sprintDurationScale;
		float fAdsZoomFov;
		float fAdsZoomInFrac;
		float fAdsZoomOutFrac;
		x86::pointer<Material> overlayMaterial;
		x86::pointer<Material> overlayMaterialLowRes;
		weapOverlayReticle_t overlayReticle;
		WeapOverlayInterface_t overlayInterface;
		float overlayWidth;
		float overlayHeight;
		float fAdsBobFactor;
		float fAdsViewBobMult;
		float fHipSpreadStandMin;
		float fHipSpreadDuckedMin;
		float fHipSpreadProneMin;
		float hipSpreadStandMax;
		float hipSpreadDuckedMax;
		float hipSpreadProneMax;
		float fHipSpreadDecayRate;
		float fHipSpreadFireAdd;
		float fHipSpreadTurnAdd;
		float fHipSpreadMoveAdd;
		float fHipSpreadDuckedDecay;
		float fHipSpreadProneDecay;
		float fHipReticleSidePos;
		int iAdsTransInTime;
		int iAdsTransOutTime;
		float fAdsIdleAmount;
		float fHipIdleAmount;
		float adsIdleSpeed;
		float hipIdleSpeed;
		float fIdleCrouchFactor;
		float fIdleProneFactor;
		float fGunMaxPitch;
		float fGunMaxYaw;
		float swayMaxAngle;
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
		int bRifleBullet;
		int armorPiercing;
		int bBoltAction;
		int aimDownSight;
		int bRechamberWhileAds;
		float adsViewErrorMin;
		float adsViewErrorMax;
		int bCookOffHold;
		int bClipOnly;
		int adsFireOnly;
		int cancelAutoHolsterWhenEmpty;
		int suppressAmmoReserveDisplay;
		int enhanced;
		int laserSightDuringNightvision;
		x86::pointer<Material> killIcon;
		weaponIconRatioType_t killIconRatio;
		int flipKillIcon;
		x86::pointer<Material> dpadIcon;
		weaponIconRatioType_t dpadIconRatio;
		int bNoPartialReload;
		int bSegmentedReload;
		int iReloadAmmoAdd;
		int iReloadStartAdd;
		x86::pointer<const char> szAltWeaponName;
		unsigned int altWeaponIndex;
		int iDropAmmoMin;
		int iDropAmmoMax;
		int blocksProne;
		int silenced;
		int iExplosionRadius;
		int iExplosionRadiusMin;
		int iExplosionInnerDamage;
		int iExplosionOuterDamage;
		float damageConeAngle;
		int iProjectileSpeed;
		int iProjectileSpeedUp;
		int iProjectileSpeedForward;
		int iProjectileActivateDist;
		float projLifetime;
		float timeToAccelerate;
		float projectileCurvature;
		x86::pointer<XModel> projectileModel;
		weapProjExposion_t projExplosion;
		x86::pointer<FxEffectDef> projExplosionEffect;
		int projExplosionEffectForceNormalUp;
		x86::pointer<FxEffectDef> projDudEffect;
		SndAliasCustom projExplosionSound;
		SndAliasCustom projDudSound;
		int bProjImpactExplode;
		WeapStickinessType stickiness;
		int hasDetonator;
		int timedDetonation;
		int rotate;
		int holdButtonToThrow;
		int freezeMovementWhenFiring;
		float lowAmmoWarningThreshold;
		float parallelBounce[SURF_TYPE_NUM];
		float perpendicularBounce[SURF_TYPE_NUM];
		x86::pointer<FxEffectDef> projTrailEffect;
		float vProjectileColor[3];
		guidedMissileType_t guidedMissileType;
		float maxSteeringAccel;
		int projIgnitionDelay;
		x86::pointer<FxEffectDef> projIgnitionEffect;
		SndAliasCustom projIgnitionSound;
		float fAdsAimPitch;
		float fAdsCrosshairInFrac;
		float fAdsCrosshairOutFrac;
		int adsGunKickReducedKickBullets;
		float adsGunKickReducedKickPercent;
		float fAdsGunKickPitchMin;
		float fAdsGunKickPitchMax;
		float fAdsGunKickYawMin;
		float fAdsGunKickYawMax;
		float fAdsGunKickAccel;
		float fAdsGunKickSpeedMax;
		float fAdsGunKickSpeedDecay;
		float fAdsGunKickStaticDecay;
		float fAdsViewKickPitchMin;
		float fAdsViewKickPitchMax;
		float fAdsViewKickYawMin;
		float fAdsViewKickYawMax;
		float fAdsViewKickCenterSpeed;
		float fAdsViewScatterMin;
		float fAdsViewScatterMax;
		float fAdsSpread;
		int hipGunKickReducedKickBullets;
		float hipGunKickReducedKickPercent;
		float fHipGunKickPitchMin;
		float fHipGunKickPitchMax;
		float fHipGunKickYawMin;
		float fHipGunKickYawMax;
		float fHipGunKickAccel;
		float fHipGunKickSpeedMax;
		float fHipGunKickSpeedDecay;
		float fHipGunKickStaticDecay;
		float fHipViewKickPitchMin;
		float fHipViewKickPitchMax;
		float fHipViewKickYawMin;
		float fHipViewKickYawMax;
		float fHipViewKickCenterSpeed;
		float fHipViewScatterMin;
		float fHipViewScatterMax;
		float fightDist;
		float maxDist;
		x86::pointer<const char> aiVsAiAccuracyGraphName;
		x86::pointer<const char> aiVsPlayerAccuracyGraphName;
		x86::pointer<vec2_t> aiVsAiAccuracyGraphKnots;
		x86::pointer<vec2_t> aiVsPlayerAccuracyGraphKnots;
		x86::pointer<vec2_t> originalAiVsAiAccuracyGraphKnots;
		x86::pointer<vec2_t> originalAiVsPlayerAccuracyGraphKnots;
		int aiVsAiAccuracyGraphKnotCount;
		int aiVsPlayerAccuracyGraphKnotCount;
		int originalAiVsAiAccuracyGraphKnotCount;
		int originalAiVsPlayerAccuracyGraphKnotCount;
		int iPositionReloadTransTime;
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
		float maxRange;
		float fAnimHorRotateInc;
		float fPlayerPositionDist;
		x86::pointer<const char> szUseHintString;
		x86::pointer<const char> dropHintString;
		int iUseHintStringIndex;
		int dropHintStringIndex;
		float horizViewJitter;
		float vertViewJitter;
		x86::pointer<const char> szScript;
		float fOOPosAnimLength[2];
		int minDamage;
		int minPlayerDamage;
		float fMaxDamageRange;
		float fMinDamageRange;
		float destabilizationRateTime;
		float destabilizationCurvatureMax;
		int destabilizeDistance;
		float locationDamageMultipliers[HITLOC_COUNT];
		x86::pointer<const char> fireRumble;
		x86::pointer<const char> meleeImpactRumble;
		float adsDofStart;
		float adsDofEnd;
	};

	struct SndDriverGlobals
	{
		x86::pointer<const char> name;
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	struct FxSpawnDefOneShot
	{
		FxIntRange count;
	};

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
	};

	union FxSpawnDef
	{
		FxSpawnDefLooping looping;
		FxSpawnDefOneShot oneShot;
	};

	struct FxElemAtlas
	{
		char behavior;
		char index;
		char fps;
		char loopCount;
		char colIndexBits;
		char rowIndexBits;
		int16_t entryCount;
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
		FxElemVelStateInFrame local;
		FxElemVelStateInFrame world;
	};

	struct FxElemVisualState
	{
		char color[4];
		float rotationDelta;
		float rotationTotal;
		float size[2];
		float scale;
	};

	struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
	};

	union FxEffectDefRef
	{
		x86::pointer<FxEffectDef> handle;
		x86::pointer<const char> name;
	};

	enum FxElemType
	{
		FX_ELEM_TYPE_SPRITE_BILLBOARD,
		FX_ELEM_TYPE_SPRITE_ORIENTED,
		FX_ELEM_TYPE_TAIL,
		FX_ELEM_TYPE_TRAIL,
		FX_ELEM_TYPE_CLOUD,
		FX_ELEM_TYPE_MODEL,
		FX_ELEM_TYPE_OMNI_LIGHT,
		FX_ELEM_TYPE_SPOT_LIGHT,
		FX_ELEM_TYPE_SOUND,
		FX_ELEM_TYPE_DECAL,
		FX_ELEM_TYPE_RUNNER,

		FX_ELEM_TYPE_COUNT,
		FX_ELEM_TYPE_LAST_SPRITE = FX_ELEM_TYPE_TRAIL,
		FX_ELEM_TYPE_LAST_DRAWN = FX_ELEM_TYPE_SPOT_LIGHT,
	};

	union FxElemVisuals
	{
		x86::pointer<const void> anonymous;
		x86::pointer<Material> material;
		x86::pointer<XModel> model;
		FxEffectDefRef effectDef;
		x86::pointer<const char> soundName;
	};

	struct FxElemMarkVisuals
	{
		x86::pointer<Material> materials[2];
	};

	union FxElemDefVisuals
	{
		x86::pointer<FxElemMarkVisuals> markArray;
		x86::pointer<FxElemVisuals> array;
		FxElemVisuals instance;
	};

	struct FxTrailVertex
	{
		float pos[2];
		float normal[2];
		float texCoord;
	};

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		int splitDist;
		int vertCount;
		x86::pointer<FxTrailVertex> verts;
		int indCount;
		x86::pointer<uint16_t> inds;
	};

	struct FxElemDef
	{
		int flags;
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
		FxElemAtlas atlas;
		char elemType;
		char visualCount;
		char velIntervalCount;
		char visStateIntervalCount;
		x86::pointer<FxElemVelStateSample> velSamples;
		x86::pointer<FxElemVisStateSample> visSamples;
		FxElemDefVisuals visuals;
		float collMins[3];
		float collMaxs[3];
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		x86::pointer<FxTrailDef> trailDef;
		char sortOrder;
		char lightingFrac;
		char useItemClip;
		char unused[1];
	};

	struct FxEffectDef
	{
		x86::pointer<const char> name;
		int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		x86::pointer<FxElemDef> elemDefs;
	};

	enum FxFleshType
	{
		FX_FLESHTYPE_BODY_NONFATAL = 0x0,
		FX_FLESHTYPE_BODY_FATAL = 0x1,
		FX_FLESHTYPE_HEAD_NONFATAL = 0x2,
		FX_FLESHTYPE_HEAD_FATAL = 0x3,

		FX_FLESHTYPE_COUNT,
	};

	struct FxImpactEntry
	{
		x86::pointer<FxEffectDef> nonflesh[SURF_TYPE_NUM];
		x86::pointer<FxEffectDef> flesh[FX_FLESHTYPE_COUNT];
	};

	struct FxImpactTable
	{
		x86::pointer<const char> name;
		x86::pointer<FxImpactEntry> table;
	};

	struct RawFile
	{
		x86::pointer<const char> name;
		int len;
		x86::pointer<const char> buffer;
	};

	struct StringTable
	{
		x86::pointer<const char> name;
		int columnCount;
		int rowCount;
		x86::pointer<x86::pointer<const char>> values;
	};

	struct ScriptStringList
	{
		std::int32_t count;
		x86::pointer<x86::pointer<const char>> strings;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetList
	{
		ScriptStringList stringList;
		std::int32_t assetCount;
		x86::pointer<XAsset> assets;
	};

	static_assert(sizeof(BrushWrapper) == 80);
	static_assert(sizeof(CollisionPartition) == 12);
	static_assert(sizeof(ComPrimaryLight) == 68);
	static_assert(sizeof(ComWorld) == 16);
	static_assert(sizeof(DynEntityDef) == 96);
	static_assert(sizeof(Font_s) == 24);
	static_assert(sizeof(FxEffectDef) == 32);
	static_assert(sizeof(FxEffectDefRef) == 4);
	static_assert(sizeof(FxElemDef) == 252);
	static_assert(sizeof(FxElemDefVisuals) == 4);
	static_assert(sizeof(FxElemMarkVisuals) == 8);
	static_assert(sizeof(FxElemVisuals) == 4);
	static_assert(sizeof(FxImpactEntry) == 132);
	static_assert(sizeof(FxImpactTable) == 8);
	static_assert(sizeof(FxTrailDef) == 28);
	static_assert(sizeof(GameWorldMp) == 4);
	static_assert(sizeof(GameWorldSp) == 44);
	static_assert(sizeof(GfxAabbTree) == 44);
	static_assert(sizeof(GfxCell) == 56);
	static_assert(sizeof(GfxImage) == 36);
	static_assert(sizeof(GfxLight) == 64);
	static_assert(sizeof(GfxLightDef) == 16);
	static_assert(sizeof(GfxLightGrid) == 56);
	static_assert(sizeof(GfxLightImage) == 8);
	static_assert(sizeof(GfxLightRegion) == 8);
	static_assert(sizeof(GfxLightRegionHull) == 80);
	static_assert(sizeof(GfxLightmapArray) == 8);
	static_assert(sizeof(GfxPixelShaderLoadDef) == 8);
	static_assert(sizeof(GfxPortal) == 68);
	static_assert(sizeof(GfxReflectionProbe) == 16);
	static_assert(sizeof(GfxShadowGeometry) == 12);
	static_assert(sizeof(GfxStaticModelDrawInst) == 76);
	static_assert(sizeof(GfxSurface) == 48);
	static_assert(sizeof(GfxTexture) == 4);
	static_assert(sizeof(GfxVertexShaderLoadDef) == 8);
	static_assert(sizeof(GfxWorld) == 732);
	static_assert(sizeof(GfxWorldDpvsDynamic) == 48);
	static_assert(sizeof(GfxWorldDpvsPlanes) == 16);
	static_assert(sizeof(GfxWorldDpvsStatic) == 104);
	static_assert(sizeof(GfxWorldVertexData) == 8);
	static_assert(sizeof(GfxWorldVertexLayerData) == 8);
	static_assert(sizeof(ItemKeyHandler) == 12);
	static_assert(sizeof(LoadedSound) == 44);
	static_assert(sizeof(LocalizeEntry) == 8);
	static_assert(sizeof(MapEnts) == 12);
	static_assert(sizeof(Material) == 80);
	static_assert(sizeof(MaterialArgumentDef) == 4);
	static_assert(sizeof(MaterialInfo) == 24);
	static_assert(sizeof(MaterialMemory) == 8);
	static_assert(sizeof(MaterialPass) == 20);
	static_assert(sizeof(MaterialPixelShader) == 16);
	static_assert(sizeof(MaterialPixelShaderProgram) == 12);
	static_assert(sizeof(MaterialShaderArgument) == 8);
	static_assert(sizeof(MaterialTechniqueSet) == 148);
	static_assert(sizeof(MaterialTextureDef) == 12);
	static_assert(sizeof(MaterialTextureDefInfo) == 4);
	static_assert(sizeof(MaterialVertexShader) == 16);
	static_assert(sizeof(MaterialVertexShaderProgram) == 12);
	static_assert(sizeof(MenuList) == 12);
	static_assert(sizeof(MssSound) == 40);
	static_assert(sizeof(Operand) == 8);
	static_assert(sizeof(PathData) == 40);
	static_assert(sizeof(PhysGeomInfo) == 68);
	static_assert(sizeof(PhysGeomList) == 44);
	static_assert(sizeof(PhysPreset) == 44);
	static_assert(sizeof(RawFile) == 12);
	static_assert(sizeof(SndAliasCustom) == 4);
	static_assert(sizeof(SndCurve) == 72);
	static_assert(sizeof(SoundFile) == 12);
	static_assert(sizeof(SoundFileRef) == 8);
	static_assert(sizeof(SpeakerMap) == 408);
	static_assert(sizeof(StreamedSound) == 8);
	static_assert(sizeof(StringTable) == 16);
	static_assert(sizeof(WeaponDef) == 2168);
	static_assert(sizeof(XAnimDeltaPart) == 8);
	static_assert(sizeof(XAnimDynamicFrames) == 4);
	static_assert(sizeof(XAnimIndices) == 4);
	static_assert(sizeof(XAnimParts) == 88);
	static_assert(sizeof(XModel) == 220);
	static_assert(sizeof(XModelCollSurf_s) == 44);
	static_assert(sizeof(XModelPiece) == 16);
	static_assert(sizeof(XModelPieces) == 12);
	static_assert(sizeof(XRigidVertList) == 12);
	static_assert(sizeof(XSurface) == 56);
	static_assert(sizeof(XSurfaceCollisionTree) == 40);
	static_assert(sizeof(XSurfaceVertexInfo) == 12);
	static_assert(sizeof(cLeafBrushNodeData_t) == 12);
	static_assert(sizeof(cLeafBrushNodeLeaf_t) == 4);
	static_assert(sizeof(cLeafBrushNode_s) == 20);
	static_assert(sizeof(cNode_t) == 8);
	static_assert(sizeof(cStaticModel_s) == 80);
	static_assert(sizeof(cbrush_t) == 80);
	static_assert(sizeof(cbrushside_t) == 12);
	static_assert(sizeof(clipMap_t) == 284);
	static_assert(sizeof(entryInternalData) == 8);
	static_assert(sizeof(expressionEntry) == 12);
	static_assert(sizeof(itemDefData_t) == 4);
	static_assert(sizeof(itemDef_s) == 372);
	static_assert(sizeof(listBoxDef_s) == 340);
	static_assert(sizeof(menuDef_t) == 284);
	static_assert(sizeof(multiDef_s) == 392);
	static_assert(sizeof(operandInternalDataUnion) == 4);
	static_assert(sizeof(pathnode_constant_t) == 68);
	static_assert(sizeof(pathnode_t) == 128);
	static_assert(sizeof(pathnode_tree_info_t) == 8);
	static_assert(sizeof(pathnode_tree_nodes_t) == 8);
	static_assert(sizeof(pathnode_tree_t) == 16);
	static_assert(sizeof(snd_alias_list_name) == 4);
	static_assert(sizeof(snd_alias_list_t) == 12);
	static_assert(sizeof(snd_alias_t) == 92);
	static_assert(sizeof(statement_s) == 8);
	static_assert(sizeof(sunflare_t) == 96);
	static_assert(sizeof(water_t) == 68);
	static_assert(sizeof(windowDef_t) == 156);
	static_assert(sizeof(XAsset) == 8);
	static_assert(sizeof(XAssetList) == 16);
}
