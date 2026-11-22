#pragma once

namespace zonetool::iw4
{
	enum FxElemType : char
	{
		FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
		FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
		FX_ELEM_TYPE_TAIL = 0x2,
		FX_ELEM_TYPE_TRAIL = 0x3,
		FX_ELEM_TYPE_CLOUD = 0x4,
		FX_ELEM_TYPE_SPARKCLOUD = 0x5,
		FX_ELEM_TYPE_SPARKFOUNTAIN = 0x6,
		FX_ELEM_TYPE_MODEL = 0x7,
		FX_ELEM_TYPE_OMNI_LIGHT = 0x8,
		FX_ELEM_TYPE_SPOT_LIGHT = 0x9,
		FX_ELEM_TYPE_SOUND = 0xA,
		FX_ELEM_TYPE_DECAL = 0xB,
		FX_ELEM_TYPE_RUNNER = 0xC,
		FX_ELEM_TYPE_COUNT = 0xD,
		FX_ELEM_TYPE_LAST_SPRITE = 0x3,
		FX_ELEM_TYPE_LAST_DRAWN = 0x9,
	};

	typedef float vec3_t[3];

	typedef float vec2_t[2];

	template <std::size_t N>
	struct VecInternal
	{
		float data[N];
	};

	struct RawFile
	{
		x86::pointer<const char> name;
		int compressedLen;
		int len;
		x86::pointer<const char> buffer;
	};

	enum PathNodeErrorCode : __int32
	{
		PNERR_NONE = 0x0,
		PNERR_INSOLID = 0x1,
		PNERR_FLOATING = 0x2,
		PNERR_NOLINK = 0x3,
		PNERR_DUPLICATE = 0x4,
		PNERR_NOSTANCE = 0x5,
		PNERR_INVALIDDOOR = 0x6,
		PNERR_NOANGLES = 0x7,
		PNERR_BADPLACEMENT = 0x8,
		NUM_PATH_NODE_ERRORS = 0x9,
	};

	union $4FF1D8B92DD4A7AD33E4615607EF0247
	{
		float minUseDistSq;
		PathNodeErrorCode error;
	};

	enum nodeType : __int32
	{
		NODE_ERROR = 0x0,
		NODE_PATHNODE = 0x1,
		NODE_COVER_STAND = 0x2,
		NODE_COVER_CROUCH = 0x3,
		NODE_COVER_CROUCH_WINDOW = 0x4,
		NODE_COVER_PRONE = 0x5,
		NODE_COVER_RIGHT = 0x6,
		NODE_COVER_LEFT = 0x7,
		NODE_AMBUSH = 0x8,
		NODE_EXPOSED = 0x9,
		NODE_CONCEALMENT_STAND = 0xA,
		NODE_CONCEALMENT_CROUCH = 0xB,
		NODE_CONCEALMENT_PRONE = 0xC,
		NODE_DOOR = 0xD,
		NODE_DOOR_INTERIOR = 0xE,
		NODE_SCRIPTED = 0xF,
		NODE_NEGOTIATION_BEGIN = 0x10,
		NODE_NEGOTIATION_END = 0x11,
		NODE_TURRET = 0x12,
		NODE_GUARD = 0x13,
		NODE_NUMTYPES = 0x14,
		NODE_DONTLINK = 0x14,
	};

	struct pathlink_s
	{
		float fDist;
		unsigned __int16 nodeNum;
		unsigned __int8 disconnectCount;
		unsigned __int8 negotiationLink;
		unsigned __int8 flags;
		unsigned __int8 ubBadPlaceCount[3];
	};

	struct pathnode_constant_t
	{
		nodeType type;
		unsigned __int16 spawnflags;
		unsigned __int16 targetname;
		unsigned __int16 script_linkName;
		unsigned __int16 script_noteworthy;
		unsigned __int16 target;
		unsigned __int16 animscript;
		int animscriptfunc;
		float vOrigin[3];
		float fAngle;
		float forward[2];
		float fRadius;
		$4FF1D8B92DD4A7AD33E4615607EF0247 ___u12;
		__int16 wOverlapNode[2];
		unsigned __int16 totalLinkCount;
		x86::pointer<pathlink_s> Links;
	};

	struct SentientHandle
	{
		unsigned __int16 number;
		unsigned __int16 infoIndex;
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
		unsigned __int8 userCount;
		bool hasBadPlaceLink;
	};

	struct pathnode_t;

	union $73F238679C0419BE2C31C6559E8604FC
	{
		float nodeCost;
		int linkIndex;
	};

	struct pathnode_transient_t
	{
		int iSearchFrame;
		x86::pointer<pathnode_t> pNextOpen;
		x86::pointer<pathnode_t> pPrevOpen;
		x86::pointer<pathnode_t> pParent;
		float fCost;
		float fHeuristic;
		$73F238679C0419BE2C31C6559E8604FC ___u6;
	};

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
		pathnode_transient_t transient;
	};

	struct pathbasenode_t
	{
		float vOrigin[3];
		unsigned int type;
	};

	struct pathnode_tree_nodes_t
	{
		int nodeCount;
		x86::pointer<unsigned __int16> nodes;
	};

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

	struct PathData
	{
		unsigned int nodeCount;
		x86::pointer<pathnode_t> nodes;
		x86::pointer<pathbasenode_t> basenodes;
		unsigned int chainNodeCount;
		x86::pointer<unsigned __int16> chainNodeForNode;
		x86::pointer<unsigned __int16> nodeForChainNode;
		int visBytes;
		x86::pointer<unsigned __int8> pathVis;
		int nodeTreeCount;
		x86::pointer<pathnode_tree_t> nodeTree;
	};

#pragma pack(push, 4)
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

#pragma pack(pop)
	struct XModelAngle
	{
		short x;
		short y;
		short z;
		short base;
	};

	struct XModelTagPos
	{
		float x;
		float y;
		float z;
	};

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};

#pragma pack(push, 4)
	struct MaterialStreamRouting
	{
		char source;
		char dest;
	};

	struct VertexDecl
	{
		x86::pointer<const char> name;
		char streamCount;
		bool hasOptionalSource;
		char pad[2];
		MaterialStreamRouting streams[13];
		x86::pointer<void> declarations[16];
	};

#pragma pack(pop)
	struct PixelShader
	{
		x86::pointer<const char> name;
		x86::pointer<void> shader;
		x86::pointer<DWORD> bytecode;
		short codeLen;
	};

	struct VertexShader
	{
		x86::pointer<const char> name;
		x86::pointer<void> shader;
		x86::pointer<DWORD> bytecode;
		short codeLen;
	};

	struct MaterialArgumentCodeConst
	{
		unsigned __int16 index;
		char firstRow;
		char rowCount;
	};

	union MaterialArgumentDef
	{
		x86::pointer<float> literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	struct ShaderArgumentDef
	{
		short type;
		short dest;
		MaterialArgumentDef u;
	};

#pragma pack(push, 4)
	struct MaterialPass
	{
		x86::pointer<VertexDecl> vertexDecl;
		x86::pointer<VertexShader> vertexShader;
		x86::pointer<PixelShader> pixelShader;
		char perPrimArgCount;
		char perObjArgCount;
		char stableArgCount;
		char customSamplerFlags;
		x86::pointer<ShaderArgumentDef> argumentDef;
	};

#pragma pack(pop)
	struct MaterialTechniqueHeader
	{
		x86::pointer<const char> name;
		unsigned __int16 flags;
		unsigned __int16 passCount;
	};

	struct MaterialTechnique
	{
		MaterialTechniqueHeader hdr;
		MaterialPass pass[1];
	};

	struct MaterialTechniqueSet
	{
		x86::pointer<const char> name;
		int pad;
		x86::pointer<MaterialTechniqueSet> remappedTechniques;
		x86::pointer<MaterialTechnique> techniques[48];
	};

	struct MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		float literal[4];
	};

	struct GfxStateBits
	{
		unsigned int loadBits[2];
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char pad[3];
		int flags;
		int format;
		int resourceSize;
		char data[1];
	};

	enum GfxImageCategory : unsigned char
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

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct GfxImage
	{
		x86::pointer<GfxImageLoadDef> texture;
		unsigned char mapType;
		unsigned char semantic;
		unsigned char category;
		bool useSrgbReads;
		Picmip picmip;
		bool noPicmip;
		char track;
		CardMemory cardMemory;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		bool delayLoadPixels;
		x86::pointer<const char> name;
	};

	struct WaterWritable
	{
		float floatTime;
	};

	struct complex_s
	{
		float real;
		float imag;
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

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		unsigned char samplerState;
		unsigned char semantic;
		MaterialTextureDefInfo u;
	};

	enum GfxCameraRegionType : std::uint16_t
	{
		CAMERA_REGION_LIT_OPAQUE = 0x0,
		CAMERA_REGION_LIT_TRANS = 0x1,
		CAMERA_REGION_EMISSIVE = 0x2,
		CAMERA_REGION_DEPTH_HACK = 0x3,
		CAMERA_REGION_COUNT = 0x4,
		CAMERA_REGION_NONE = 0x4,
	};

	struct GfxDrawSurfFields
	{
		unsigned __int64 objectId : 16;
		unsigned __int64 reflectionProbeIndex : 8;
		unsigned __int64 hasGfxEntIndex : 1;
		unsigned __int64 customIndex : 5;
		unsigned __int64 materialSortedIndex : 12;
		unsigned __int64 prepass : 2;
		unsigned __int64 useHeroLighting : 1;
		unsigned __int64 sceneLightIndex : 8;
		unsigned __int64 surfType : 4;
		unsigned __int64 primarySortKey : 6;
		unsigned __int64 unused : 1;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		unsigned __int64 packed;
	};

	struct Material
	{
		x86::pointer<const char> name;
		char gameFlags;
		char sortKey;
		char textureAtlasRowCount;
		char textureAtlasColumnCount;
		GfxDrawSurf drawSurf;
		int surfaceTypeBits;
		unsigned __int16 hashIndex;
		unsigned __int16 pad;
		char stateBitsEntry[48];
		char textureCount;
		char constantCount;
		char stateBitsCount;
		char stateFlags;
		char cameraRegion;
		x86::pointer<MaterialTechniqueSet> techniqueSet;
		x86::pointer<MaterialTextureDef> textureTable;
		x86::pointer<MaterialConstantDef> constantTable;
		x86::pointer<GfxStateBits> stateBitsTable;
	};

	struct Bounds
	{
		vec3_t midPoint; // mins
		vec3_t halfSize; // maxs

		void compute(vec3_t mins, vec3_t maxs);
		void max(vec3_t& out);
		void min(vec3_t& out);
		bool overlaps(const Bounds& other);
		bool contains(const vec3_t& point);
	};

	struct XBoneInfo
	{
		union
		{
			Bounds packedBounds;
			float bounds[2][3];
		};

		float radiusSquared;
	};

	struct Face
	{
		unsigned short v1;
		unsigned short v2;
		unsigned short v3;
	};

	struct XSurfaceVertexInfo
	{
		__int16 vertCount[4];
		x86::pointer<unsigned __int16> vertsBlend;

		
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
		unsigned char array[4];
	};

	struct GfxPackedVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct XSurfaceCollisionAabb
	{
		unsigned __int16 mins[3];
		unsigned __int16 maxs[3];
	};

	struct XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		unsigned __int16 childBeginIndex;
		unsigned __int16 childCount;
	};

	struct XSurfaceCollisionLeaf
	{
		unsigned __int16 triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		x86::pointer<XSurfaceCollisionNode> nodes;
		unsigned int leafCount;
		x86::pointer<XSurfaceCollisionLeaf> leafs;
	};

	struct XRigidVertList
	{
		unsigned __int16 boneOffset;
		unsigned __int16 vertCount;
		unsigned __int16 triOffset;
		unsigned __int16 triCount;
		x86::pointer<XSurfaceCollisionTree> collisionTree;
	};

	struct XSurface
	{
		char tileMode;
		char deformed;
		unsigned short vertCount;
		unsigned short triCount;
		unsigned char streamHandle;
		char zoneHandle;
		unsigned __int16 baseTriIndex;
		unsigned __int16 baseVertIndex;
		x86::pointer<Face> triIndices;
		XSurfaceVertexInfo vertexInfo;
		x86::pointer<GfxPackedVertex> verticies;
		int vertListCount;
		x86::pointer<XRigidVertList> rigidVertLists;
		int partBits[6];
	};

	struct XModelCollTri_s
	{
		float plane[4];
		float svec[4];
		float tvec[4];
	};

	struct XModelSurfs
	{
		x86::pointer<const char> name;
		x86::pointer<XSurface> surfs;
		unsigned __int16 numsurfs;
		unsigned __int16 pad;
		int partBits[6];
	};

	struct XModelLodInfo
	{
		float dist;
		short numSurfacesInLod;
		short surfIndex;
		x86::pointer<XModelSurfs> surfaces;
		int partBits[6];
		x86::pointer<XSurface> surfs;
		char lod;
		char smcBaseIndexPlusOne;
		char smcSubIndexMask;
		char smcBucket;
	};

	struct XModelCollSurf_s
	{
		x86::pointer<XModelCollTri_s> collTris;
		int numCollTris;
		Bounds bounds;
		int boneIdx;
		int contents;
		int surfFlags;
	};

	struct PhysCollmap;

	struct XModel
	{
		x86::pointer<char> name;
		char numBones;
		char numRootBones;
		unsigned char numSurfaces;
		char lodRampType;
		float scale;
		unsigned int noScalePartBits[6];
		x86::pointer<short> boneNames;
		x86::pointer<unsigned char> parentList;
		x86::pointer<XModelAngle> tagAngles;
		x86::pointer<XModelTagPos> tagPositions;
		x86::pointer<char> partClassification;
		x86::pointer<DObjAnimMat> animMatrix;
		x86::pointer<x86::pointer<Material>> materials;
		XModelLodInfo lods[4];
		char maxLoadedLod;
		char numLods;
		char collLod;
		char flags;
		x86::pointer<XModelCollSurf_s> collSurfs;
		int numColSurfs;
		int contents;
		x86::pointer<XBoneInfo> boneInfo;
		float radius;
		Bounds bounds;
		int memUsage;
		bool bad;
		char pad[3];
		x86::pointer<PhysPreset> physPreset;
		x86::pointer<PhysCollmap> physCollmap;
	}; // total size 304

	enum weapFireType_t : int
	{
		WEAPON_FIRETYPE_FULLAUTO = 0x0,
		WEAPON_FIRETYPE_SINGLESHOT = 0x1,
		WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
		WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
		WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
		WEAPON_FIRETYPE_DOUBLE_BARREL = 0x5,
		WEAPON_FIRETYPE_MAX
	};

	enum weapInventoryType_t : int
	{
		WEAPINVENTORY_PRIMARY = 0,
		WEAPINVENTORY_OFFHAND = 1,
		WEAPINVENTORY_ITEM = 2,
		WEAPINVENTORY_ALTMODE = 3,
		WEAPINVENTORY_EXCLUSIVE = 4,
		WEAPINVENTORY_SCAVENGER = 5,
		WEAPINVENTORY_MAX
	};

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,
		PENETRATE_TYPE_COUNT = 0x4
	};

	enum activeReticleType_t : int
	{
		VEH_ACTIVE_RETICLE_NONE = 0,
		VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 1,
		VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 2,
		VEH_ACTIVE_RETICLE_MAX
	};

	enum weapType_t : int
	{
		WEAPTYPE_BULLET = 0,
		WEAPTYPE_GRENADE = 1,
		WEAPTYPE_PROJECTILE = 2,
		WEAPTYPE_RIOTSHIELD = 3,
		WEAPTYPE_MAX
	};

	enum weapClass_t : int
	{
		WEAPCLASS_RIFLE = 0,
		WEAPCLASS_SNIPER = 1,
		WEAPCLASS_MG = 2,
		WEAPCLASS_SMG = 3,
		WEAPCLASS_SPREAD = 4,
		WEAPCLASS_PISTOL = 5,
		WEAPCLASS_GRENADE = 6,
		WEAPCLASS_ROCKETLAUNCHER = 7,
		WEAPCLASS_TURRET = 8,
		WEAPCLASS_THROWINGKNIFE = 9,
		WEAPCLASS_NON_PLAYER = 10,
		WEAPCLASS_ITEM = 11,
		WEAPCLASS_MAX
	};

	enum OffhandClass : int
	{
		OFFHAND_CLASS_NONE = 0,
		OFFHAND_CLASS_FRAG_GRENADE = 1,
		OFFHAND_CLASS_SMOKE_GRENADE = 2,
		OFFHAND_CLASS_FLASH_GRENADE = 3,
		OFFHAND_CLASS_MAX
	};

	enum playerAnimType_t : int
	{
		PLAER_ANIM_TYPE_NONE = 0x0,
		PLAER_ANIM_TYPE_OTHER = 0x1,
		PLAER_ANIM_TYPE_PISTOL = 0x2,
		PLAER_ANIM_TYPE_SMG = 0x3,
		PLAER_ANIM_TYPE_AUTORIFLE = 0x4,
		PLAER_ANIM_TYPE_MG = 0x5,
		PLAER_ANIM_TYPE_SNIPER = 0x6,
		PLAER_ANIM_TYPE_ROCKETLAUNCHER = 0x7,
		PLAER_ANIM_TYPE_EXPLOSIVE = 0x8,
		PLAER_ANIM_TYPE_GRENADE = 0x9,
		PLAER_ANIM_TYPE_TURRET = 0xA,
		PLAER_ANIM_TYPE_C4 = 0xB,
		PLAER_ANIM_TYPE_M203 = 0xC,
		PLAER_ANIM_TYPE_HOLD = 0xD,
		PLAER_ANIM_TYPE_BRIEFCASE = 0xE,
		PLAER_ANIM_TYPE_RIOTSHIELD = 0xF,
		PLAER_ANIM_TYPE_LAPTOP = 0x10,
		PLAER_ANIM_TYPE_THROWINGKNIFE = 0x11
	};

	enum weapProjExplosion_t
	{
		WEAPPROJEXP_GRENADE = 0x0,
		WEAPPROJEXP_ROCKET = 0x1,
		WEAPPROJEXP_FLASHBANG = 0x2,
		WEAPPROJEXP_NONE = 0x3,
		WEAPPROJEXP_DUD = 0x4,
		WEAPPROJEXP_SMOKE = 0x5,
		WEAPPROJEXP_HEAVY = 0x6,
		WEAPPROJEXP_NUM = 0x7
	};

	enum WeapStickinessType
	{
		WEAPSTICKINESS_NONE = 0x0,
		WEAPSTICKINESS_ALL = 0x1,
		WEAPSTICKINESS_ALL_ORIENT = 0x2,
		WEAPSTICKINESS_GROUND = 0x3,
		WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
		WEAPSTICKINESS_KNIFE = 0x5,
		WEAPSTICKINESS_COUNT = 0x6
	};

	enum weaponIconRatioType_t
	{
		WEAPON_ICON_RATIO_1TO1 = 0x0,
		WEAPON_ICON_RATIO_2TO1 = 0x1,
		WEAPON_ICON_RATIO_4TO1 = 0x2,
		WEAPON_ICON_RATIO_COUNT = 0x3
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
		AMMO_COUNTER_CLIP_COUNT = 0x7
	};

	enum weapOverlayReticle_t
	{
		WEAPOVERLAYRETICLE_NONE = 0x0,
		WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
		WEAPOVERLAYRETICLE_NUM = 0x2
	};

	enum weapOverlayInterface_t
	{
		WEAPOVERLAYINTERFACE_NONE = 0x0,
		WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
		WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
		WEAPOVERLAYINTERFACECOUNT = 0x3
	};

	enum weapStance_t
	{
		WEAPSTANCE_STAND = 0x0,
		WEAPSTANCE_DUCK = 0x1,
		WEAPSTANCE_PRONE = 0x2,
		WEAPSTANCE_NUM = 0x3
	};

	enum ImpactType
	{
		IMPACT_TYPE_NONE = 0,
		IMPACT_TYPE_BULLET_SMALL = 1,
		IMPACT_TYPE_BULLET_LARGE = 2,
		IMPACT_TYPE_BULLET_AP = 3,
		IMPACT_TYPE_SHOTGUN_FMJ = 4,
		IMPACT_TYPE_SHOTGUN = 5,
		IMPACT_TYPE_GRENADE_BOUNCE = 7,
		IMPACT_TYPE_GRENADE_EXPLODE = 8,
		IMPACT_TYPE_ROCKET_EXPLODE = 9,
		IMPACT_TYPE_PROJECTILE_DUD = 10,
		IMPACT_TYPE_MAX
	};

	enum guidedMissileType_t
	{
		MISSILE_GUIDANCE_NONE = 0x0,
		MISSILE_GUIDANCE_SIDEWINDER = 0x1,
		MISSILE_GUIDANCE_HELLFIRE = 0x2,
		MISSILE_GUIDANCE_JAVELIN = 0x3,
		MISSILE_GUIDANCE_MAX
	};

#pragma pack(push, 4)
	struct SndCurve
	{
		x86::pointer<const char> filename;
		unsigned __int16 knotCount;
		vec2_t knots[16];
	};

#pragma pack(pop)
        struct _AILSOUNDINFO
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

#pragma pack(push, 4)
	struct MssSound
	{
            _AILSOUNDINFO info;
            x86::pointer<char> data;
	};

#pragma pack(pop)
	struct LoadedSound
	{
		x86::pointer<const char> name;
            MssSound sound;
	};

	struct SpeakerLevels
	{
		int speaker;
		int numLevels;
		float levels[2];
	};

	struct ChannelMap
	{
		int entryCount; // how many entries are used
		SpeakerLevels speakers[6];
	};

	struct SpeakerMap
	{
		bool isDefault;
		char _pad[3];
		x86::pointer<const char> name;
		ChannelMap channelMaps[2][2];
	};

	enum snd_alias_type_t : char
	{
		SAT_UNKNOWN = 0x0,
		SAT_LOADED = 0x1,
		SAT_STREAMED = 0x2,
		SAT_PRIMED = 0x3,
		SAT_COUNT = 0x4,
	};

	struct StreamFileNamePacked
	{
		unsigned __int64 offset;
		unsigned __int64 length;
	};

	struct StreamFileNameRaw
	{
		x86::pointer<const char> dir;
		x86::pointer<const char> name;
	};

	union StreamFileInfo
	{
		StreamFileNameRaw raw;
		StreamFileNamePacked packed;
	};

	struct StreamFileName
	{
		unsigned __int16 isLocalized;
		unsigned __int16 fileIndex;
		StreamFileInfo info;
	};

	struct StreamedSound
	{
		x86::pointer<const char> dir;
		x86::pointer<const char> name;
	};

	struct PrimedSound
	{
		StreamFileName filename;
		x86::pointer<LoadedSound> loadedPart;
		int dataOffset;
		int totalSize;
		unsigned int primedCrc;
	};

	union SoundData
	{
		x86::pointer<LoadedSound> loadSnd; // SoundFile->type == SAT_LOADED
		StreamedSound streamSnd; // SoundFile->type == SAT_STREAMED
		//PrimedSound primedSnd;	// SoundFile->type == SAT_PRIMED
	};

	struct SoundFile // 0x10
	{
		char type;
		bool exists;
		char _pad[2];
		SoundData sound;
	};

#pragma pack(push, 4)
	union $C8D87EB0090687D323381DFB7A82089C
	{
		float slavePercentage;
		float masterPercentage;
	};

	enum SoundChannel : std::uint32_t
	{
		SND_CHANNEL_PHYSICS,
		SND_CHANNEL_AMBDIST1,
		SND_CHANNEL_AMBDIST2,
		SND_CHANNEL_AUTO,
		SND_CHANNEL_AUTO2,
		SND_CHANNEL_AUTODOG,
		SND_CHANNEL_BULLETIMPACT,
		SND_CHANNEL_BULLETWHIZBY,
		SND_CHANNEL_EXPLOSIVEIMPACT,
		SND_CHANNEL_ELEMENT,
		SND_CHANNEL_AUTO2D,
		SND_CHANNEL_VEHICLE,
		SND_CHANNEL_VEHICLELIMITED,
		SND_CHANNEL_MENU,
		SND_CHANNEL_BODY,
		SND_CHANNEL_BODY2D,
		SND_CHANNEL_RELOAD,
		SND_CHANNEL_RELOAD2D,
		SND_CHANNEL_ITEM,
		SND_CHANNEL_EFFECTS1,
		SND_CHANNEL_EFFECTS2,
		SND_CHANNEL_WEAPON,
		SND_CHANNEL_WEAPON2D,
		SND_CHANNEL_NONSHOCK,
		SND_CHANNEL_VOICE,
		SND_CHANNEL_LOCAL,
		SND_CHANNEL_LOCAL2,
		SND_CHANNEL_LOCAL3,
		SND_CHANNEL_AMBIENT,
		SND_CHANNEL_HURT,
		SND_CHANNEL_PLAYER1,
		SND_CHANNEL_PLAYER2,
		SND_CHANNEL_MUSIC,
		SND_CHANNEL_MUSICNOPAUSE,
		SND_CHANNEL_MISSION,
		SND_CHANNEL_ANNOUNCER,
		SND_CHANNEL_SHELLSHOCK,

		SND_CHANNEL_COUNT
	};

	struct snd_alias_t
	{
		x86::pointer<const char> aliasName;
		x86::pointer<const char> subtitle;
		x86::pointer<const char> secondaryAliasName;
		x86::pointer<const char> chainAliasName;
		x86::pointer<const char> mixerGroup;
		x86::pointer<SoundFile> soundFile;
		int sequence;
		float volMin;
		float volMax;
		float pitchMin;
		float pitchMax;
		float distMin;
		float distMax;
		float velocityMin;
		int flags;
		$C8D87EB0090687D323381DFB7A82089C ___u15;
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

#pragma pack(pop)
	struct snd_alias_list_t
	{
		union
		{
			x86::pointer<const char> aliasName;
			x86::pointer<const char> name;
		};

		x86::pointer<snd_alias_t> head;
		int count;
	};

#pragma pack(push, 4)
	struct cplane_s
	{
		float normal[3];
		float dist;
		char type;
		char signbits;
	};
#pragma pack(pop)

	struct cbrushside_t
	{
		x86::pointer<cplane_s> plane;
		unsigned __int16 materialNum;
		char firstAdjacentSideOffset;
		char edgeCount;
	};

	struct cbrush_t
	{
		unsigned __int16 numsides;
		unsigned __int16 glassPieceIndex;
		x86::pointer<cbrushside_t> sides;
		x86::pointer<unsigned char> baseAdjacentSide;
		__int16 axialMaterialNum[2][3];
		unsigned char firstAdjacentSideOffsets[2][3];
		unsigned char edgeCount[2][3];
	};

	struct BrushWrapper
	{
		Bounds bounds;
		cbrush_t brush;
		int totalEdgeCount;
		x86::pointer<cplane_s> planes;
	};

	enum PhysicsGeomType : int
	{
		PHYS_GEOM_NONE = 0x0,
		PHYS_GEOM_BOX = 0x1,
		PHYS_GEOM_BRUSHMODEL = 0x2,
		PHYS_GEOM_BRUSH = 0x3,
		PHYS_GEOM_COLLMAP = 0x4,
		PHYS_GEOM_CYLINDER = 0x5,
		PHYS_GEOM_CAPSULE = 0x6,
		PHYS_GEOM_GLASS = 0x7,
		PHYS_GEOM_COUNT = 0x8,
	};

	struct PhysGeomInfo
	{
		x86::pointer<BrushWrapper> brush;
		int type;
		float orientation[3][3];
		Bounds bounds;
	};

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
		// int contents;
	};

	struct PhysCollmap
	{
		x86::pointer<const char> name;
		unsigned int count;
		x86::pointer<PhysGeomInfo> geoms;
		PhysMass mass;
		Bounds bounds;
	};

	struct G_GlassName
	{
		x86::pointer<char> nameStr;
		unsigned __int16 name;
		unsigned __int16 pieceCount;
		x86::pointer<unsigned __int16> pieceIndices;
	};

#pragma pack(push, 2)
	struct G_GlassPiece
	{
		unsigned __int16 damageTaken;
		unsigned __int16 collapseTime;
		int lastStateChangeTime;
		char impactDir;
		char impactPos[2];
	};

#pragma pack(pop)
	struct G_GlassData
	{
		x86::pointer<G_GlassPiece> glassPieces;
		unsigned int pieceCount;
		unsigned __int16 damageToWeaken;
		unsigned __int16 damageToDestroy;
		unsigned int glassNameCount;
		x86::pointer<G_GlassName> glassNames;
		char pad[108];
	};

	struct GameWorldMp
	{
		x86::pointer<const char> name;
		x86::pointer<G_GlassData> g_glassData;
	};

	struct VehicleTrack
	{
		x86::pointer<void> segments;
		unsigned int segmentCount;
	};

	struct GameWorldSp
	{
		x86::pointer<const char> name;
		PathData path;
		VehicleTrack vehicleTrack;
		x86::pointer<G_GlassData> g_glassData;
	};

#pragma pack(push, 4)
	struct FxGlassDef
	{
		float halfThickness;
		float texVecs[2][2];
		GfxColor color;
		x86::pointer<Material> material;
		x86::pointer<Material> materialShattered;
		x86::pointer<PhysPreset> physPreset;
	};

	struct FxSpatialFrame
	{
		float quat[4];
		float origin[3];
	};

	union FxGlassPiecePlace
	{
		struct
		{
			FxSpatialFrame frame;
			float radius;
		};

		unsigned int nextFree;
	};

	struct FxGlassPieceState
	{
		float texCoordOrigin[2];
		unsigned int supportMask;
		unsigned __int16 initIndex;
		unsigned __int16 geoDataStart;
		unsigned __int16 lightingIndex;
		char defIndex;
		char pad[3];
		char vertCount;
		char holeDataCount;
		char crackDataCount;
		char fanDataCount;
		unsigned __int16 flags;
		float areaX2;
	};

	struct FxGlassPieceDynamics
	{
		int fallTime;
		__int32 physObjId;
		__int32 physJointId;
		float vel[3];
		float avel[3];
	};

	struct FxGlassVertex
	{
		__int16 x;
		__int16 y;
	};

	struct FxGlassHoleHeader
	{
		unsigned __int16 uniqueVertCount;
		char touchVert;
		char pad[1];
	};

	struct FxGlassCrackHeader
	{
		unsigned __int16 uniqueVertCount;
		char beginVertIndex;
		char endVertIndex;
	};

	union FxGlassGeometryData
	{
		FxGlassVertex vert;
		FxGlassHoleHeader hole;
		FxGlassCrackHeader crack;
		char asBytes[4];
		__int16 anonymous[2];
	};

	struct FxGlassInitPieceState //Note, on MW3 this is missing 4 bytes, just not sure whats missing yet
	{
		FxSpatialFrame frame;
		float radius;
		float texCoordOrigin[2];
		unsigned int supportMask;
		//float areaX2; // Commented out a random thing so the size fits. Most probably wrong since it was random.
		unsigned __int16 lightingIndex;
		char defIndex;
		char vertCount;
		char fanDataCount;
		char pad[1];
	};

	struct FxGlassSystem
	{
		int time; // 4
		int prevTime; // 4
		unsigned int defCount; // 4
		unsigned int pieceLimit; // 4
		unsigned int pieceWordCount; // 4
		unsigned int initPieceCount; // 4
		unsigned int cellCount; // 4
		unsigned int activePieceCount; // 4
		unsigned int firstFreePiece; // 4
		unsigned int geoDataLimit; // 4
		unsigned int geoDataCount; // 4
		unsigned int initGeoDataCount; // 4
		x86::pointer<FxGlassDef> defs; // 4
		x86::pointer<FxGlassPiecePlace> piecePlaces; // 4
		x86::pointer<FxGlassPieceState> pieceStates; // 4
		x86::pointer<FxGlassPieceDynamics> pieceDynamics; // 4
		x86::pointer<FxGlassGeometryData> geoData; // 4
		x86::pointer<unsigned int> isInUse; // 4
		x86::pointer<unsigned int> cellBits; // 4
		x86::pointer<char> visData; // 4
		x86::pointer<VecInternal<3>> linkOrg;
		x86::pointer<float> halfThickness; // 4
		x86::pointer<unsigned __int16> lightingHandles; // 4
		x86::pointer<FxGlassInitPieceState> initPieceStates; // 4
		x86::pointer<FxGlassGeometryData> initGeoData; // 4
		bool needToCompactData; // 1
		char initCount;
		short pad;
		float effectChanceAccum; // 4
		int lastPieceDeletionTime; // 4
	};

	struct FxWorld
	{
		x86::pointer<const char> name;
		FxGlassSystem glassSys;
	};

#pragma pack(pop)
	struct TriggerModel
	{
		int contents;
		unsigned short hullCount;
		unsigned short firstHull;
	};

	struct TriggerHull
	{
		Bounds bounds;
		int contents;
		unsigned short slabCount;
		unsigned short firstSlab;
	};

	struct TriggerSlab
	{
		vec3_t dir;
		float midPoint;
		float halfSize;
	};

	struct MapTriggers
	{
		int count;
		x86::pointer<TriggerModel> models; // sizeof 8
		int hullCount;
		x86::pointer<TriggerHull> hulls; // sizeof 32
		int slabCount;
		x86::pointer<TriggerSlab> slabs; // sizeof 20
	};

	struct Stage
	{
		x86::pointer<char> name;
		float origin[3];
		unsigned __int16 triggerIndex;
		char sunPrimaryLightIndex;
		char pad;
	};

	struct MapEnts
	{
		x86::pointer<const char> name;
		x86::pointer<char> entityString;
		int numEntityChars;
		MapTriggers trigger;
		x86::pointer<Stage> stages;
		char stageCount;
	};

#pragma pack(push, 1)
	struct ComPrimaryLight
	{
		union
		{
			char _portpad0[28];

			struct
			{
				char type;
				char canUseShadowMap;
				char exponent;
				char unused;
				float color[3];
				float dir[3];
			};
		};

		union
		{
			char _portpad1[40];

			struct
			{
				float origin[3];
				float radius;
				float cosHalfFovOuter;
				float cosHalfFovInner;
				float cosHalfFovExpanded;
				float rotationLimit;
				float translationLimit;
				x86::pointer<const char> defName;
			};
		};
	};

#pragma pack(pop)
	struct ComWorld
	{
		x86::pointer<const char> name;
		int isInUse;
		unsigned int primaryLightCount;
		x86::pointer<ComPrimaryLight> primaryLights;
	};

	union XAnimIndices
	{
		x86::pointer<char> _1;
		x86::pointer<unsigned __int16> _2;
		x86::pointer<void> data;
	};

	union XAnimDynamicFrames
	{
		x86::pointer<char[3]> _1;
		x86::pointer<unsigned __int16[3]> _2;
	};

	union XAnimDynamicIndices
	{
		char _1[1];
		unsigned __int16 _2[1];
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
		unsigned __int16 size;
		char smallTrans;
		__declspec(align(2)) XAnimPartTransData u;
	};

#pragma pack(push, 4)
	struct XAnimDeltaPartQuatDataFrames2
	{
		x86::pointer<__int16[2]> frames;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData2
	{
		XAnimDeltaPartQuatDataFrames2 frames;
		__int16 frame0[2];
	};

	struct XAnimDeltaPartQuat2
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData2 u;
	};

	struct XAnimDeltaPartQuatDataFrames
	{
		x86::pointer<__int16[4]> frames;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		__int16 frame0[4];
	};

	struct XAnimDeltaPartQuat
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData u;
	};

	struct XAnimDeltaPart
	{
		x86::pointer<XAnimPartTrans> trans;
		x86::pointer<XAnimDeltaPartQuat2> quat2;
		x86::pointer<XAnimDeltaPartQuat> quat;
	};

	struct XAnimNotifyInfo
	{
		short name;
		float time;
	};

        enum XAnimPartsFlags
        {
            ANIM_LOOP = 0x1,
            ANIM_DELTA = 0x2,
            ANIM_DELTA_3D = 0x4,
        };

	struct XAnimParts
	{
		x86::pointer<char> name; // 0
		unsigned short dataByteCount; // 4
		unsigned short dataShortCount; // 6
		unsigned short dataIntCount; // 8
		unsigned short randomDataByteCount; // 10 - 0xA
		unsigned short randomDataIntCount; // 12 - 0xC
		unsigned short framecount; // 14 - 0xE
		char flags; // 16
		char boneCount[10]; // 17
		char notifyCount; // 27
		char pad1; // 28
		char bDelta; // 29
		char assetType; // 30
		bool isDefault; // 31
		unsigned int randomDataShortCount; // 32 - 0x20
		unsigned int indexcount; // 36 - 0x24
		float framerate; // 40 - 0x28
		float frequency; // 44 - 0x2C
		x86::pointer<unsigned short> tagnames; // 48 - 0x30
		x86::pointer<char> dataByte; // 52 - 0x34
		x86::pointer<short> dataShort; // 56 - 0x38
		x86::pointer<int> dataInt; // 60 - 0x3C
		x86::pointer<short> randomDataShort; // 64 - 0x40
		x86::pointer<char> randomDataByte; // 68 - 0x44
		x86::pointer<int> randomDataInt; // 72 - 0x48
		XAnimIndices indices; // 76 - 0x4C
		x86::pointer<XAnimNotifyInfo> notify; // 80 - 0x50
		x86::pointer<XAnimDeltaPart> delta; // 84 - 0x54
		// 88 - 0x58
	};

#pragma pack(pop)
#pragma pack(push, 4)
	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		char horzAlign;
		char vertAlign;
	};

#pragma pack(pop)
	struct expression_s;

	struct statement_s;

	struct menuDef_t;

	struct MenuEventHandlerSet;

	struct Statement_s;

	enum operationEnum
	{
		OP_NOOP = 0x0,
		OP_RIGHTPAREN = 0x1,
		OP_MULTIPLY = 0x2,
		OP_DIVIDE = 0x3,
		OP_MODULUS = 0x4,
		OP_ADD = 0x5,
		OP_SUBTRACT = 0x6,
		OP_NOT = 0x7,
		OP_LESSTHAN = 0x8,
		OP_LESSTHANEQUALTO = 0x9,
		OP_GREATERTHAN = 0xA,
		OP_GREATERTHANEQUALTO = 0xB,
		OP_EQUALS = 0xC,
		OP_NOTEQUAL = 0xD,
		OP_AND = 0xE,
		OP_OR = 0xF,
		OP_LEFTPAREN = 0x10,
		OP_COMMA = 0x11,
		OP_BITWISEAND = 0x12,
		OP_BITWISEOR = 0x13,
		OP_BITWISENOT = 0x14,
		OP_BITSHIFTLEFT = 0x15,
		OP_BITSHIFTRIGHT = 0x16,
		OP_FIRSTFUNCTIONCALL = 0x17,
		OP_STATICDVARINT = 0x17,
		OP_STATICDVARBOOL = 0x18,
		OP_STATICDVARFLOAT = 0x19,
		OP_STATICDVARSTRING = 0x1A,
		OP_INT = 0x1B,
		OP_STRING = 0x1C,
		OP_FLOAT = 0x1D,
		OP_SIN = 0x1E,
		OP_COS = 0x1F,
		OP_MIN = 0x20,
		OP_MAX = 0x21,
		OP_MILLISECONDS = 0x22,
		OP_DVARINT = 0x23,
		OP_DVARBOOL = 0x24,
		OP_DVARFLOAT = 0x25,
		OP_DVARSTRING = 0x26,
		OP_STAT = 0x27,
		OP_UIACTIVE = 0x28,
		OP_FLASHBANGED = 0x29,
		OP_USINGVEHICLE = 0x2A,
		OP_MISSILECAM = 0x2B,
		OP_SCOPED = 0x2C,
		OP_SCOPEDTHERMAL = 0x2D,
		OP_SCOREBOARDVISIBLE = 0x2E,
		OP_INKILLCAM = 0x2F,
		OP_INKILLCAMNPC = 0x30,
		OP_PLAYERFIELD = 0x31,
		OP_GETPERK = 0x32,
		OP_SELECTINGLOCATION = 0x33,
		OP_SELECTINGDIRECTION = 0x34,
		OP_TEAMFIELD = 0x35,
		OP_OTHERTEAMFIELD = 0x36,
		OP_MARINESFIELD = 0x37,
		OP_OPFORFIELD = 0x38,
		OP_MENUISOPEN = 0x39,
		OP_WRITINGDATA = 0x3A,
		OP_INLOBBY = 0x3B,
		OP_INPRIVATEPARTY = 0x3C,
		OP_PRIVATEPARTYHOST = 0x3D,
		OP_PRIVATEPARTYHOSTINLOBBY = 0x3E,
		OP_ALONEINPARTY = 0x3F,
		OP_ADSJAVELIN = 0x40,
		OP_WEAPLOCKBLINK = 0x41,
		OP_WEAPATTACKTOP = 0x42,
		OP_WEAPATTACKDIRECT = 0x43,
		OP_WEAPLOCKING = 0x44,
		OP_WEAPLOCKED = 0x45,
		OP_WEAPLOCKTOOCLOSE = 0x46,
		OP_WEAPLOCKSCREENPOSX = 0x47,
		OP_WEAPLOCKSCREENPOSY = 0x48,
		OP_SECONDSASTIME = 0x49,
		OP_TABLELOOKUP = 0x4A,
		OP_TABLELOOKUPBYROW = 0x4B,
		OP_TABLEGETROWNUM = 0x4C,
		OP_LOCALIZESTRING = 0x4D,
		OP_LOCALVARINT = 0x4E,
		OP_LOCALVARBOOL = 0x4F,
		OP_LOCALVARFLOAT = 0x50,
		OP_LOCALVARSTRING = 0x51,
		OP_TIMELEFT = 0x52,
		OP_SECONDSASCOUNTDOWN = 0x53,
		OP_GAMEMSGWNDACTIVE = 0x54,
		OP_GAMETYPENAME = 0x55,
		OP_GAMETYPE = 0x56,
		OP_GAMETYPEDESCRIPTION = 0x57,
		OP_SCORE = 0x58,
		OP_FRIENDSONLINE = 0x59,
		OP_SPECTATINGCLIENT = 0x5A,
		OP_SPECTATINGFREE = 0x5B,
		OP_STATRANGEBITSSET = 0x5C,
		OP_KEYBINDING = 0x5D,
		OP_ACTIONSLOTUSABLE = 0x5E,
		OP_HUDFADE = 0x5F,
		OP_MAXRECOMMENDEDPLAYERS = 0x60,
		OP_ACCEPTINGINVITE = 0x61,
		OP_ISINTERMISSION = 0x62,
		OP_GAMEHOST = 0x63,
		OP_PARTYISMISSINGMAPPACK = 0x64,
		OP_PARTYMISSINGMAPPACKERROR = 0x65,
		OP_ANYNEWMAPPACKS = 0x66,
		OP_AMISELECTED = 0x67,
		OP_PARTYSTATUSSTRING = 0x68,
		OP_ATTACHEDCONTROLLERCOUNT = 0x69,
		OP_ISSPLITSCREENONLINEPOSSIBLE = 0x6A,
		OP_SPLITSCREENPLAYERCOUNT = 0x6B,
		OP_GETPLAYERDATA = 0x6C,
		OP_GETPLAYERDATASPLITSCREEN = 0x6D,
		OP_EXPERIENCEFORLEVEL = 0x6E,
		OP_LEVELFOREXPERIENCE = 0x6F,
		OP_ISITEMUNLOCKED = 0x70,
		OP_ISITEMUNLOCKEDSPLITSCREEN = 0x71,
		OP_DEBUGPRINT = 0x72,
		OP_GETPLAYERDATAANYBOOLTRUE = 0x73,
		OP_WEAPONCLASSNEW = 0x74,
		OP_WEAPONNAME = 0x75,
		OP_ISRELOADING = 0x76,
		OP_SAVEGAMEAVAILABLE = 0x77,
		OP_UNLOCKEDITEMCOUNT = 0x78,
		OP_UNLOCKEDITEMCOUNTSPLITSCREEN = 0x79,
		OP_UNLOCKEDITEM = 0x7A,
		OP_UNLOCKEDITEMSPLITSCREEN = 0x7B,
		OP_MAILSUBJECT = 0x7C,
		OP_MAILFROM = 0x7D,
		OP_MAILRECEIVED = 0x7E,
		OP_MAILBODY = 0x7F,
		OP_MAILLOOTLOCALIZED = 0x80,
		OP_MAILGIVESLOOT = 0x81,
		OP_ANYNEWMAIL = 0x82,
		OP_MAILTIMETOFOLLOWUP = 0x83,
		OP_MAILLOOTTYPE = 0x84,
		OP_MAILRANLOTTERY = 0x85,
		OP_LOTTERYLOOTLOCALIZED = 0x86,
		OP_RADARISJAMMED = 0x87,
		OP_RADARJAMINTENSITY = 0x88,
		OP_RADARISENABLED = 0x89,
		OP_ISEMPJAMMED = 0x8A,
		OP_PLAYERADS = 0x8B,
		OP_WEAPONHEATACTIVE = 0x8C,
		OP_WEAPONHEATVALUE = 0x8D,
		OP_WEAPONHEATOVERHEATED = 0x8E,
		OP_GETSPLASHTEXT = 0x8F,
		OP_GETSPLASHDESCRIPTION = 0x90,
		OP_GETSPLASHMATERIAL = 0x91,
		OP_SPLASHHASICON = 0x92,
		OP_SPLASHROWNUM = 0x93,
		OP_GETFOCUSEDITEMNAME = 0x94,
		OP_GETFOCUSEDITEMX = 0x95,
		OP_GETFOCUSEDITEMY = 0x96,
		OP_GETFOCUSEDITEMWIDTH = 0x97,
		OP_GETFOCUSEDITEMHEIGHT = 0x98,
		OP_GETITEMX = 0x99,
		OP_GETITEMY = 0x9A,
		OP_GETITEMWIDTH = 0x9B,
		OP_GETITEMHEIGHT = 0x9C,
		OP_PLAYLIST = 0x9D,
		OP_SCOREBOARDEXTERNALMUTENOTICE = 0x9E,
		OP_GETCLIENTMATCHDATA = 0x9F,
		OP_GETCLIENTMATCHDATADEF = 0xA0,
		OP_GETMAPNAME = 0xA1,
		OP_GETMAPIMAGE = 0xA2,
		OP_GETMAPCUSTOM = 0xA3,
		OP_GETMIGRATIONSTATUS = 0xA4,
		OP_GETPLAYERCARDINFO = 0xA5,
		OP_ISOFFLINEPROFILESELECTED = 0xA6,
		OP_COOPPLAYER = 0xA7,
		OP_ISCOOP = 0xA8,
		OP_GETPARTYSTATUS = 0xA9,
		OP_GETSEARCHPARAMS = 0xAA,
		OP_GETTIMEPLAYED = 0xAB,
		OP_ISSELECTEDPLAYERFRIEND = 0xAC,
		OP_GETCHARBYINDEX = 0xAD,
		OP_GETPROFILEDATA = 0xAE,
		OP_ISPROFILESIGNEDIN = 0xAF,
		OP_GETWAITPOPUPSTATUS = 0xB0,
		OP_GETNATTYPE = 0xB1,
		OP_GETLOCALIZEDNATTYPE = 0xB2,
		OP_GETADJUSTEDSAFEAREAHORIZONTAL = 0xB3,
		OP_GETADJUSTEDSAFEAREAVERTICAL = 0xB4,
		OP_CONNECTIONINFO = 0xB5,
		OP_OFFLINEPROFILECANSAVE = 0xB6,
		OP_ALLSPLITSCREENPROFILESCANSAVE = 0xB7,
		OP_ALLSPLITSCREENPROFILESARESIGNEDIN = 0xB8,
		OP_DOWEHAVEMAPPACK = 0xB9,
	};

	struct UIFunctionList
	{
		int totalFunctions;
		x86::pointer<x86::pointer<Statement_s>> functions;
	};

	struct StaticDvar
	{
		
		x86::pointer<void> dvar;
		x86::pointer<const char> dvarName;
	};

	struct StaticDvarList
	{
		int numStaticDvars;
		x86::pointer<x86::pointer<StaticDvar>> staticDvars;
	};

	struct StringList
	{
		int totalStrings;
		x86::pointer<x86::pointer<const char>> strings;
	};

	struct ExpressionSupportingData
	{
		UIFunctionList uifunctions;
		StaticDvarList staticDvarList;
		StringList uiStrings;
	};

	enum expDataType : int
	{
		VAL_INT = 0x0,
		VAL_FLOAT = 0x1,
		VAL_STRING = 0x2,
		VAL_FUNCTION = 0x3,
	};

	struct ExpressionString
	{
		x86::pointer<const char> string;
	};

	union operandInternalDataUnion
	{
		int intVal;
		float floatVal;
		ExpressionString stringVal;
		x86::pointer<Statement_s> function;
	};

	struct Operand
	{
		expDataType dataType;
		operandInternalDataUnion internals;
	};

	union entryInternalData
	{
		operationEnum op; 
		Operand operand;
	};

	struct expressionEntry	// 0xC
	{
		int type;
		entryInternalData data;
	};

	struct Statement_s	// 0x18
	{
		int numEntries;
		x86::pointer<expressionEntry> entries;
		x86::pointer<ExpressionSupportingData> supportingData;
		int lastExecuteTime;
		Operand lastResult;
	};

	struct SetLocalVarData
	{
		x86::pointer<const char> localVarName;
		x86::pointer<Statement_s> expression;
	};

	struct ConditionalScript
	{
		x86::pointer<MenuEventHandlerSet> eventHandlerSet;
		x86::pointer<Statement_s> eventExpression;  // loads this first
	};

	union EventData
	{
		x86::pointer<const char> unconditionalScript;
		x86::pointer<ConditionalScript> conditionalScript;
		x86::pointer<MenuEventHandlerSet> elseScript;
		x86::pointer<SetLocalVarData> setLocalVarData;
	};

	enum EventType
	{
		EVENT_UNCONDITIONAL = 0x0,
		EVENT_IF = 0x1,
		EVENT_ELSE = 0x2,
		EVENT_SET_LOCAL_VAR_BOOL = 0x3,
		EVENT_SET_LOCAL_VAR_INT = 0x4,
		EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
		EVENT_SET_LOCAL_VAR_STRING = 0x6,
		EVENT_COUNT = 0x7,
	};

	struct MenuEventHandler
	{
		EventData eventData;
		EventType eventType;
	};

	struct MenuEventHandlerSet
	{
		int eventHandlerCount;
		x86::pointer<x86::pointer<MenuEventHandler>> eventHandlers;
	};

	struct ItemKeyHandler
	{
		int key;
		x86::pointer<MenuEventHandlerSet> action;
		x86::pointer<ItemKeyHandler> next;
	};

	struct windowDef_t // 0xA4
	{
		x86::pointer<const char> name;	// 0x00
		rectDef_s rect;
		rectDef_s rectClient;
		x86::pointer<char> group;		// 0x2C
		int style;			// 0x30
		int border;			// 0x34
		int ownerDraw;		// 0x38
		int ownerDrawFlags;	// 0x3C
		float borderSize;	// 0x40
		int staticFlags;	// 0x44
		int dynamicFlags;	// 0x48
		int nextTime;		// 0x4C
		float foreColor[4];	// 0x50
		float backColor[4];	// 0x60
		float borderColor[4];// 0x70
		float outlineColor[4];// 0x80
		float disableColor[4];// 0x90
		x86::pointer<Material> background;	// 0xA0
	};

	enum ItemFloatExpressionTarget
	{
		ITEM_FLOATEXP_TGT_RECT_X = 0x0,
		ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
		ITEM_FLOATEXP_TGT_RECT_W = 0x2,
		ITEM_FLOATEXP_TGT_RECT_H = 0x3,
		ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
		ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
		ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
		ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
		ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
		ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
		ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
		ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
		ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
		ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,
		ITEM_FLOATEXP_TGT__COUNT = 0x13,
	};

	struct ItemFloatExpression
	{
		ItemFloatExpressionTarget target;
		x86::pointer<Statement_s> expression;
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

	struct multiDef_s	// 0x188
	{
		x86::pointer<const char> dvarList[32];
		x86::pointer<const char> dvarStr[32];
		float dvarValue[32];
		int count;
		int strDef;
	};

	struct columnInfo_s
	{
		int xpos;
		int width;
		int maxChars;
		int alignment;
	};

	struct listBoxDef_s	// 0x144
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
		x86::pointer<MenuEventHandlerSet> doubleClick;	// 0xC8
		int notselectable;
		int noscrollbars;
		int usepaging;
		float selectBorder[4];
		x86::pointer<Material> selectIcon;
	};

	struct newsTickerDef_s
	{
		int feedId;
		int speed;
		int spacing;
		int lastTime;
		int start;
		int end;
		float x;
	};

	struct textScrollDef_s
	{
		int startTime;
	};

	union itemDefData_t
	{
		x86::pointer<listBoxDef_s> listBox;
		x86::pointer<editFieldDef_s> editField;
		x86::pointer<newsTickerDef_s> ticker;
		x86::pointer<multiDef_s> multiDef;
		x86::pointer<const char> enumDvarName;
		x86::pointer<textScrollDef_s> scroll;
		x86::pointer<void> data;
	};

	struct itemDef_t
	{
		windowDef_t window;
		rectDef_s textRect[1];
		int type;
		int dataType;
		int alignment;
		int fontEnum;
		int textAlignMode;
		float textAlignX;
		float textAlignY;
		float textScale;
		int textStyle;
		int gameMsgWindowIndex;
		int gameMsgWindowMode;
		x86::pointer<const char> text;
		int textSaveGameInfo;
		int parent;
		x86::pointer<MenuEventHandlerSet> mouseEnterText;
		x86::pointer<MenuEventHandlerSet> mouseExitText;
		x86::pointer<MenuEventHandlerSet> mouseEnter;
		x86::pointer<MenuEventHandlerSet> mouseExit;
		x86::pointer<MenuEventHandlerSet> action;
		x86::pointer<MenuEventHandlerSet> accept;
		x86::pointer<MenuEventHandlerSet> onFocus;
		x86::pointer<MenuEventHandlerSet> leaveFocus;
		x86::pointer<const char> dvar;
		x86::pointer<const char> dvarTest;
		x86::pointer<ItemKeyHandler> onKey;
		x86::pointer<const char> enableDvar;
		x86::pointer<const char> localVar;
		int dvarFlags;
		x86::pointer<const char> focusSound;
		float special;
		int cursorPos[1];
		itemDefData_t typeData;
		int imageTrack;
		int floatExpressionCount;
		x86::pointer<ItemFloatExpression> floatExpressions;
		x86::pointer<Statement_s> visibleExp;
		x86::pointer<Statement_s> disabledExp;
		x86::pointer<Statement_s> textExp;
		x86::pointer<Statement_s> materialExp;
		float glowColor[4];
		bool decayActive;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int lastSoundPlayedTime;
	};

	struct menuTransition	// 0x1C
	{
		int transitionType;
		int targetField;
		int startTime;
		float startVal;
		float endVal;
		float time;
		int endTriggerType;
	};

	struct menuDef_t
	{
		windowDef_t window;
		int font;
		int fullscreen;
		int itemCount;
		int fontIndex;
		int cursorItems;
		int fadeCycle;
		float fadeClamp;
		float fadeAmount;
		float fadeInAmount;
		float blurRadius;
		x86::pointer<MenuEventHandlerSet> onOpen;
		x86::pointer<MenuEventHandlerSet> onRequestClose;
		x86::pointer<MenuEventHandlerSet> onClose;
		x86::pointer<MenuEventHandlerSet> onEsc;
		x86::pointer<ItemKeyHandler> onKey;
		x86::pointer<Statement_s> visibleExp;
		x86::pointer<const char> allowedBinding;
		x86::pointer<const char> soundLoop;
		int imageTrack;
		float focusColor[4];
		x86::pointer<Statement_s> rectXExp;
		x86::pointer<Statement_s> rectYExp;
		x86::pointer<Statement_s> rectWExp;
		x86::pointer<Statement_s> rectHExp;
		x86::pointer<Statement_s> openSoundExp;
		x86::pointer<Statement_s> closeSoundExp;
		x86::pointer<x86::pointer<itemDef_t>> items;
		menuTransition scaleTransition[1];
		menuTransition alphaTransition[1];
		menuTransition xTransition[1];
		menuTransition yTransition[1];
		x86::pointer<ExpressionSupportingData> expressionData;
	};

	struct StringTableCell
	{
		x86::pointer<char> string; // 0
		int hash; // 4
	};

	struct StringTable
	{
		x86::pointer<const char> name; // 0
		int columns; // 4
		int rows; // 8
		x86::pointer<StringTableCell> strings; // 12
	};

	struct FxEffectDef;

	struct FxElemMarkVisuals
	{
		x86::pointer<Material> materials[2];
	};

	union FxEffectDefRef
	{
		x86::pointer<FxEffectDef> handle;
		x86::pointer<const char> name;
	};

	union FxElemVisuals
	{
		x86::pointer<const void> anonymous;
		x86::pointer<Material> material;
		x86::pointer<XModel> xmodel;
		x86::pointer<FxEffectDefRef> effectDef;
		x86::pointer<const char> soundName;
	};

	union FxElemDefVisuals
	{
		FxElemVisuals instance;
		x86::pointer<FxElemVisuals> array;
		x86::pointer<FxElemMarkVisuals> markArray;
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
		float invSplitDist;
		float invSplitArcDist;
		float invSplitTime;
		int vertCount;
		x86::pointer<FxTrailVertex> verts;
		int indCount;
		x86::pointer<unsigned __int16> inds;
	};

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

	struct FxSpotLightDef
	{
		float fovInnerFraction;
		float startRadius;
		float endRadius;
		float brightness;
		float maxLength;
		int exponent;
	};

	union FxElemExtendedDefPtr
	{
		x86::pointer<FxTrailDef> trailDef;
		x86::pointer<FxSparkFountainDef> sparkFountain;
		x86::pointer<FxSpotLightDef> spotLightDef;
	};

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
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
		__int16 entryCount;
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

	const struct FxElemVelStateSample
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

	const struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
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
		FX_ELEM_RUN_MASK = 0xC0,
		FX_ELEM_USE_COLLISION = 0x100,
		FX_ELEM_DIE_ON_TOUCH = 0x200,
		FX_ELEM_DRAW_PAST_FOG = 0x400,
		FX_ELEM_DRAW_WITH_VIEWMODEL = 0x800,
		FX_ELEM_BLOCK_SIGHT = 0x1000,
		FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY = 0x2000,
		FX_ELEM_TRAIL_ORIENT_BY_VELOCITY = 0x4000,
		FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
		FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
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
		FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
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
		Bounds collBounds;
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		FxElemExtendedDefPtr extended;
		char sortOrder;
		char lightingFrac;
		char useItemClip;
		char fadeInfo;
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

#pragma pack(push, 4)
	struct GfxLightImage
	{
		x86::pointer<GfxImage> image;
		char samplerState;
	};

#pragma pack(pop)
	struct GfxLightDef
	{
		x86::pointer<const char> name;
		GfxLightImage attenuation;
		int lmapLookupStart;
	};

	struct GfxSky
	{
		int skySurfCount;
		x86::pointer<std::uint32_t> skyStartSurfs;
		x86::pointer<GfxImage> skyImage;
		char skySamplerState;
		char pad[3];
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount;
		x86::pointer<cplane_s> planes;
		x86::pointer<unsigned __int16> nodes;
		x86::pointer<unsigned char> sceneEntCellBits; //Size = cellCount << 11
	};

	struct GfxAabbTree
	{
		Bounds bounds;
		int unkn;
		unsigned __int16 childCount; // 2
		unsigned __int16 surfaceCount; // 2
		unsigned __int16 startSurfIndex; // 2
		unsigned __int16 smodelIndexCount; // 2
		x86::pointer<unsigned __int16> smodelIndexes; // 4
		int childrenOffset; // 4
	}; // Size: 0x2C

	struct GfxCellTree
	{
		// Best struct ever
		x86::pointer<GfxAabbTree> aabbtree;
	};

#pragma pack(push, 4)
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

	struct DpvsPlane
	{
		float coeffs[4];
	};

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		x86::pointer<float[3]> vertices;
		unsigned __int16 cellIndex;
		char vertexCount;
		float hullAxis[2][3];
	};

#pragma pack(pop)
#pragma pack(push, 4)
	struct GfxCell
	{
		Bounds bounds;
		int portalCount;
		x86::pointer<GfxPortal> portals;
		char reflectionProbeCount;
		x86::pointer<char> reflectionProbes;
	};
#pragma pack(pop)

	struct GfxReflectionProbe
	{
		float offset[3];
	};

	typedef char GfxTexture[0x04];

	struct GfxLightmapArray
	{
		x86::pointer<GfxImage> primary;
		x86::pointer<GfxImage> secondary;
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
		x86::pointer<void> worldVb; // D3DVertexBuffer
	};

	struct GfxWorldVertexLayerData
	{
		x86::pointer<char> data;
		x86::pointer<void> layerVb; // D3DVertexBuffer
	};

	struct GfxWorldDraw
	{
		union
		{
			char _portpad0[16];

			struct
			{
				unsigned int reflectionProbeCount; // 4
				x86::pointer<x86::pointer<GfxImage>> reflectionImages; // 4
				x86::pointer<GfxReflectionProbe> reflectionProbes; // 4
				x86::pointer<GfxTexture> reflectionProbeTextures; //Count = reflectionProbeCount // 4
			};
		};

		union
		{
			char _portpad1[56];

			struct
			{
				int lightmapCount; // 4
				x86::pointer<GfxLightmapArray> lightmaps; // 4
				x86::pointer<GfxTexture> lightmapPrimaryTextures; //Count = lightmapCount // 4
				x86::pointer<GfxTexture> lightmapSecondaryTextures; //Count = lightmapCount // 4
				x86::pointer<GfxImage> skyImage; // 4
				x86::pointer<GfxImage> outdoorImage; // 4
				unsigned int vertexCount; // 4
				GfxWorldVertexData vd;
				unsigned int vertexLayerDataSize;
				GfxWorldVertexLayerData vld;
				int indexCount;
				x86::pointer<unsigned __int16> indices;
			};
		};
	};

	struct GfxLightGridEntry
	{
		unsigned __int16 colorsIndex;
		char primaryLightIndex;
		char needsTrace;
	};

	struct GfxLightGridColors
	{
		char rgb[56][3];
	};

	struct GfxLightGrid
	{
		bool hasLightRegions; // 4
		unsigned int sunPrimaryLightIndex; // 4
		unsigned __int16 mins[3]; // 6
		unsigned __int16 maxs[3]; // 6
		unsigned int rowAxis; // 4
		unsigned int colAxis; // 4
		x86::pointer<unsigned __int16> rowDataStart;
		// Size: (varGfxLightGrid->maxs[varGfxLightGrid->rowAxis] - varGfxLightGrid->mins[varGfxLightGrid->rowAxis] + 1) * 2
		unsigned int rawRowDataSize;
		x86::pointer<char> rawRowData;
		unsigned int entryCount;
		x86::pointer<GfxLightGridEntry> entries;
		unsigned int colorCount;
		x86::pointer<GfxLightGridColors> colors;
	};

	struct GfxBrushModelWritable
	{
		Bounds bounds;
	};

	struct GfxBrushModel
	{
		GfxBrushModelWritable writable;
		Bounds bounds;
		float radius;
		unsigned short surfaceCount;
		unsigned short startSurfIndex;
		unsigned short surfaceCountNoDecal;
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
		unsigned __int16 lod;
		unsigned __int16 surfId;
	};

	struct GfxSceneDynModel
	{
		XModelDrawInfo info;
		unsigned __int16 dynEntId;
	};

	struct BModelDrawInfo
	{
		unsigned __int16 surfId;
	};

	struct GfxSceneDynBrush
	{
		BModelDrawInfo info;
		unsigned __int16 dynEntId;
	};

	struct GfxShadowGeometry
	{
		unsigned __int16 surfaceCount;
		unsigned __int16 smodelCount;
		x86::pointer<unsigned __int16> sortedSurfIndex;
		x86::pointer<unsigned __int16> smodelIndex;
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

	struct GfxStaticModelInst
	{
		union
		{
			Bounds bounds;

			struct
			{
				float mins[3];
				float maxs[3];
			};
		};
		float lightingOrigin[3];
	};

	struct srfTriangles_t
	{
		unsigned int vertexLayerData;
		unsigned int firstVertex;
		unsigned __int16 vertexCount;
		unsigned __int16 triCount;
		unsigned int baseIndex;
	};

	struct GfxSurface
	{
		srfTriangles_t tris;
		x86::pointer<Material> material;
		char lightmapIndex;
		char reflectionProbeIndex;
		char primaryLightIndex;
		char flags;
	};

	struct GfxSurfaceBounds
	{
		union
		{
			Bounds bounds;

			struct
			{
				float mins[3];
				float maxs[3];
			};
		};
	};

#pragma pack(push, 4)
	struct GfxPackedPlacement
	{
		float origin[3];
		float axis[3][3];
		float scale;
	};

	struct GfxStaticModelDrawInst
	{
		GfxPackedPlacement placement;
		x86::pointer<XModel> model;
		unsigned __int16 cullDist;
		unsigned __int16 lightingHandle;
		char reflectionProbeIndex;
		char primaryLightIndex;
		char flags;
		char firstMtlSkinIndex;
		GfxColor groundLighting;
		unsigned __int16 cacheId[4];
	};

	struct GfxWorldDpvsDynamic
	{
		unsigned int dynEntClientWordCount[2];
		unsigned int dynEntClientCount[2];
		x86::pointer<unsigned int> dynEntCellBits[2];
		x86::pointer<char> dynEntVisData[2][3];
	};

#pragma pack(pop)
	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount;
		unsigned int staticSurfaceCount;
		unsigned int staticSurfaceCountNoDecal;
		unsigned int litOpaqueSurfsBegin;
		unsigned int litOpaqueSurfsEnd;
		unsigned int litTransSurfsBegin;
		unsigned int litTransSurfsEnd;
		unsigned int shadowCasterSurfsBegin;
		unsigned int shadowCasterSurfsEnd;
		unsigned int emissiveSurfsBegin;
		unsigned int emissiveSurfsEnd;
		unsigned int smodelVisDataCount;
		unsigned int surfaceVisDataCount;
		x86::pointer<char> smodelVisData[3];
		x86::pointer<char> surfaceVisData[3];
		x86::pointer<unsigned __int16> sortedSurfIndex;
		x86::pointer<GfxStaticModelInst> smodelInsts;
		x86::pointer<GfxSurface> surfaces;
		x86::pointer<GfxSurfaceBounds> surfacesBounds;
		x86::pointer<GfxStaticModelDrawInst> smodelDrawInsts;
		x86::pointer<GfxDrawSurf> surfaceMaterials;
		x86::pointer<unsigned int> surfaceCastsSunShadow;
		volatile int usageCount;
	};

	struct GfxHeroLight
	{
		char type;
		char pad[3];
		float color[3];
		float dir[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		int exponent;
	};

	struct GfxCellTreeCount
	{
		int aabbTreeCount;
	};

#pragma pack(push, 4)
	struct GfxWorld
	{
		x86::pointer<const char> name; // 4
		x86::pointer<const char> baseName; // 4
		int planeCount; // 4
		int nodeCount; // 4 // = 16
		std::uint32_t surfaceCount; // 4
		int skyCount; // 4
		x86::pointer<GfxSky> skies; // 4
		std::uint32_t lastSunPrimaryLightIndex;
		std::uint32_t primaryLightCount;
		std::uint32_t sortKeyLitDecal;
		std::uint32_t sortKeyEffectDecal;
		std::uint32_t sortKeyEffectAuto;
		std::uint32_t sortKeyDistortion;
		GfxWorldDpvsPlanes dpvsPlanes; // 16
		x86::pointer<GfxCellTreeCount> aabbTreeCounts; // Size: 4 * dpvsPlanes.cellCount // 4
		x86::pointer<GfxCellTree> aabbTree; // 4
		x86::pointer<GfxCell> cells; // 4  // = 80
		GfxWorldDraw worldDraw; // 72
		GfxLightGrid lightGrid; // 56 // = 208
		int modelCount; // 4
		x86::pointer<GfxBrushModel> models; // 4 // = 216
		Bounds bounds;
		std::uint32_t checksum; // 4
		int materialMemoryCount; // 4 // = 248
		x86::pointer<MaterialMemory> materialMemory; // 4
		sunflare_t sun; // 96 // = 348
		float outdoorLookupMatrix[4][4]; // 64
		x86::pointer<GfxImage> outdoorImage; // 4 // = 416
		x86::pointer<std::uint32_t> cellCasterBits[2]; // 8
		x86::pointer<GfxSceneDynModel> sceneDynModel; // 4
		x86::pointer<GfxSceneDynBrush> sceneDynBrush; // 4 // = 432
		x86::pointer<std::uint32_t> primaryLightEntityShadowVis;
		x86::pointer<std::uint32_t> primaryLightDynEntShadowVis[2];
		x86::pointer<unsigned char> primaryLightForModelDynEnt;
		x86::pointer<GfxShadowGeometry> shadowGeom;
		x86::pointer<GfxLightRegion> lightRegion;
		GfxWorldDpvsStatic dpvs;
		GfxWorldDpvsDynamic dpvsDyn;
		std::uint32_t mapVtxChecksum;
		std::uint32_t heroLightCount;
		x86::pointer<GfxHeroLight> heroLights;
		unsigned char fogTypesAllowed;
	};
#pragma pack(pop)

	enum CSurfaceFlags : std::uint32_t
	{
		SURF_FLAG_DEFAULT = 0x00000000,
		SURF_FLAG_BARK = 0x00100000,
		SURF_FLAG_BRICK = 0x00200000,
		SURF_FLAG_CARPET = 0x00300000,
		SURF_FLAG_CLOTH = 0x00400000,
		SURF_FLAG_CONCRETE = 0x00500000,
		SURF_FLAG_DIRT = 0x00600000,
		SURF_FLAG_FLESH = 0x00700000,
		SURF_FLAG_FOLIAGE = 0x00800000,
		SURF_FLAG_GLASS = 0x00900000,
		SURF_FLAG_GRASS = 0x00A00000,
		SURF_FLAG_GRAVEL = 0x00B00000,
		SURF_FLAG_ICE = 0x00C00000,
		SURF_FLAG_METAL = 0x00D00000,
		SURF_FLAG_MUD = 0x00E00000,
		SURF_FLAG_PAPER = 0x00F00000,
		SURF_FLAG_PLASTER = 0x01000000,
		SURF_FLAG_ROCK = 0x01100000,
		SURF_FLAG_SAND = 0x01200000,
		SURF_FLAG_SNOW = 0x01300000,
		SURF_FLAG_WATER = 0x01400000,
		SURF_FLAG_WOOD = 0x01500000,
		SURF_FLAG_ASPHALT = 0x01600000,
		SURF_FLAG_CERAMIC = 0x01700000,
		SURF_FLAG_PLASTIC = 0x01800000,
		SURF_FLAG_RUBBER = 0x01900000,
		SURF_FLAG_CUSHION = 0x01A00000,
		SURF_FLAG_FRUIT = 0x01B00000,
		SURF_FLAG_PAINTEDMETAL = 0x01C00000,
		SURF_FLAG_RIOTSHIELD = 0x01D00000,
		SURF_FLAG_SLUSH = 0x01E00000,
		SURF_FLAG_OPAQUEGLASS = 0x00900000,
		SURF_FLAG_CLIPMISSILE = 0x00000000,
		SURF_FLAG_AI_NOSIGHT = 0x00000000,
		SURF_FLAG_CLIPSHOT = 0x00000000,
		SURF_FLAG_PLAYERCLIP = 0x00000000,
		SURF_FLAG_MONSTERCLIP = 0x00000000,
		SURF_FLAG_AICLIPALLOWDEATH = 0x00000000,
		SURF_FLAG_VEHICLECLIP = 0x00000000,
		SURF_FLAG_ITEMCLIP = 0x00000000,
		SURF_FLAG_NODROP = 0x00000000,
		SURF_FLAG_NONSOLID = 0x00004000,
		SURF_FLAG_DETAIL = 0x00000000,
		SURF_FLAG_STRUCTURAL = 0x00000000,
		SURF_FLAG_PORTAL = 0x80000000,
		SURF_FLAG_CANSHOOTCLIP = 0x00000000,
		SURF_FLAG_ORIGIN = 0x00000000,
		SURF_FLAG_SKY = 0x00000004,
		SURF_FLAG_NOCASTSHADOW = 0x00040000,
		SURF_FLAG_PHYSICSGEOM = 0x00000000,
		SURF_FLAG_LIGHTPORTAL = 0x00000000,
		SURF_FLAG_OUTDOORBOUNDS = 0x00000000,
		SURF_FLAG_SLICK = 0x00000002,
		SURF_FLAG_NOIMPACT = 0x00000010,
		SURF_FLAG_NOMARKS = 0x00000020,
		SURF_FLAG_NOPENETRATE = 0x00000100,
		SURF_FLAG_LADDER = 0x00000008,
		SURF_FLAG_NODAMAGE = 0x00000001,
		SURF_FLAG_MANTLEON = 0x02000000,
		SURF_FLAG_MANTLEOVER = 0x04000000,
		SURF_FLAG_STAIRS = 0x00000200,
		SURF_FLAG_SOFT = 0x00001000,
		SURF_FLAG_NOSTEPS = 0x00002000,
		SURF_FLAG_NODRAW = 0x00000080,
		SURF_FLAG_NOLIGHTMAP = 0x00000400,
		SURF_FLAG_NODLIGHT = 0x00020000,
	};

	struct dmaterial_t
	{
		x86::pointer<char> material;
		int surfaceFlags;
		int contentFlags;
	};

	struct cNode_t
	{
		x86::pointer<cplane_s> plane;
		__int16 children[2];
	};

	struct cLeaf_t
	{
		unsigned __int16 firstCollAabbIndex;
		unsigned __int16 collAabbCount;
		int brushContents;
		int terrainContents;
		Bounds bounds;
		int leafBrushNode;
	};

	struct cLeafBrushNodeLeaf_t
	{
		x86::pointer<unsigned __int16> brushes;
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
		char axis;
		short leafBrushCount;
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
		char triCount;
		char borderCount;
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
		float midPoint[3];
		unsigned __int16 materialIndex;
		unsigned __int16 childCount;
		float halfSize[3];
		CollisionAabbTreeIndex u;
	};

	enum DynEntityType
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_DESTRUCT = 0x2,
		DYNENT_TYPE_COUNT = 0x3,
	};

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	struct DynEntityDef
	{
		DynEntityType type;
		GfxPlacement pose;
		x86::pointer<XModel> xModel;
		unsigned __int16 brushModel;
		unsigned __int16 physicsBrushModel;
		x86::pointer<FxEffectDef> destroyFx;
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
		unsigned __int16 flags;
		unsigned __int16 lightingHandle;
		int health;
		int contents;
	};

	struct DynEntityColl
	{
		unsigned __int16 sector;
		unsigned __int16 nextEntInSector;
		float linkMins[2];
		float linkMaxs[2];
	};

	struct cmodel_t
	{
		Bounds bounds;
		float radius;
		cLeaf_t leaf;
	};

	struct SModelAabbNode
	{
		Bounds bounds;
		short firstChild;
		short childCount;
	};

	struct cStaticModel_t
	{
		x86::pointer<XModel> xmodel;
		float origin[3];
		float invScaledAxis[3][3];
		Bounds absBounds;
	};

	struct clipMap_t
	{
		x86::pointer<const char> name;
		int isInUse;
		int planeCount;
		x86::pointer<cplane_s> planes;
		unsigned int numStaticModels;
		x86::pointer<cStaticModel_t> staticModelList;
		unsigned int numMaterials;
		x86::pointer<dmaterial_t> materials;
		unsigned int numBrushSides;
		x86::pointer<cbrushside_t> brushsides;
		unsigned int numBrushEdges;
		x86::pointer<unsigned char> brushEdges;
		unsigned int numNodes;
		x86::pointer<cNode_t> nodes;
		unsigned int numLeafs;
		x86::pointer<cLeaf_t> leafs;
		unsigned int leafbrushNodesCount;
		x86::pointer<cLeafBrushNode_s> leafbrushNodes;
		unsigned int numLeafBrushes;
		x86::pointer<unsigned short> leafbrushes;
		unsigned int numLeafSurfaces;
		x86::pointer<unsigned int> leafsurfaces;
		unsigned int vertCount;
		x86::pointer<vec3_t> verts;
		int triCount;
		x86::pointer<unsigned short> triIndices;
		x86::pointer<char> triEdgeIsWalkable;
		int borderCount;
		x86::pointer<CollisionBorder> borders;
		int partitionCount;
		x86::pointer<CollisionPartition> partitions;
		int aabbTreeCount;
		x86::pointer<CollisionAabbTree> aabbTrees;
		unsigned int numSubModels;
		x86::pointer<cmodel_t> cmodels;
		unsigned short numBrushes;
		x86::pointer<cbrush_t> brushes;
		x86::pointer<Bounds> brushBounds;
		x86::pointer<int> brushContents;
		x86::pointer<struct MapEnts> mapEnts;
		unsigned short smodelNodeCount;
		x86::pointer<SModelAabbNode> smodelNodes;
		unsigned short dynEntCount[2];
		x86::pointer<DynEntityDef> dynEntDefList[2];
		x86::pointer<DynEntityPose> dynEntPoseList[2];
		x86::pointer<DynEntityClient> dynEntClientList[2];
		x86::pointer<DynEntityColl> dynEntCollList[2];
		unsigned int checksum;
		char padding[48];
	};

	enum StructuredDataTypeCategory
	{
		DATA_INT = 0x0,
		DATA_BYTE = 0x1,
		DATA_BOOL = 0x2,
		DATA_STRING = 0x3,
		DATA_ENUM = 0x4,
		DATA_STRUCT = 0x5,
		DATA_INDEXED_ARRAY = 0x6,
		DATA_ENUM_ARRAY = 0x7,
		DATA_FLOAT = 0x8,
		DATA_SHORT = 0x9,
		DATA_COUNT = 0xA,
	};

	enum MaterialTechniqueType
	{
		TECHNIQUE_DEPTH_PREPASS = 0x0,
		TECHNIQUE_BUILD_FLOAT_Z = 0x1,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
		TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
		TECHNIQUE_UNLIT = 0x4,
		TECHNIQUE_EMISSIVE = 0x5,
		TECHNIQUE_EMISSIVE_DFOG = 0x6,
		TECHNIQUE_EMISSIVE_SHADOW = 0x7,
		TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x8,
		TECHNIQUE_LIT_BEGIN = 0x9,
		TECHNIQUE_LIT = 0x9,
		TECHNIQUE_LIT_DFOG = 0xA,
		TECHNIQUE_LIT_SUN = 0xB,
		TECHNIQUE_LIT_SUN_DFOG = 0xC,
		TECHNIQUE_LIT_SUN_SHADOW = 0xD,
		TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xE,
		TECHNIQUE_LIT_SPOT = 0xF,
		TECHNIQUE_LIT_SPOT_DFOG = 0x10,
		TECHNIQUE_LIT_SPOT_SHADOW = 0x11,
		TECHNIQUE_LIT_SPOT_SHADOW_DFOG = 0x12,
		TECHNIQUE_LIT_OMNI = 0x13,
		TECHNIQUE_LIT_OMNI_DFOG = 0x14,
		TECHNIQUE_LIT_OMNI_SHADOW = 0x15,
		TECHNIQUE_LIT_OMNI_SHADOW_DFOG = 0x16,
		TECHNIQUE_LIT_INSTANCED = 0x17,
		TECHNIQUE_LIT_INSTANCED_DFOG = 0x18,
		TECHNIQUE_LIT_INSTANCED_SUN = 0x19,
		TECHNIQUE_LIT_INSTANCED_SUN_DFOG = 0x1A,
		TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x1B,
		TECHNIQUE_LIT_INSTANCED_SUN_SHADOW_DFOG = 0x1C,
		TECHNIQUE_LIT_INSTANCED_SPOT = 0x1D,
		TECHNIQUE_LIT_INSTANCED_SPOT_DFOG = 0x1E,
		TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x1F,
		TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW_DFOG = 0x20,
		TECHNIQUE_LIT_INSTANCED_OMNI = 0x21,
		TECHNIQUE_LIT_INSTANCED_OMNI_DFOG = 0x22,
		TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x23,
		TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW_DFOG = 0x24,
		TECHNIQUE_LIT_END = 0x25,
		TECHNIQUE_LIGHT_SPOT = 0x25,
		TECHNIQUE_LIGHT_OMNI = 0x26,
		TECHNIQUE_LIGHT_SPOT_SHADOW = 0x27,
		TECHNIQUE_FAKELIGHT_NORMAL = 0x28,
		TECHNIQUE_FAKELIGHT_VIEW = 0x29,
		TECHNIQUE_SUNLIGHT_PREVIEW = 0x2A,
		TECHNIQUE_CASE_TEXTURE = 0x2B,
		TECHNIQUE_WIREFRAME_SOLID = 0x2C,
		TECHNIQUE_WIREFRAME_SHADED = 0x2D,
		TECHNIQUE_DEBUG_BUMPMAP = 0x2E,
		TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x2F,
		TECHNIQUE_COUNT = 0x30,
		TECHNIQUE_TOTAL_COUNT = 0x31,
		TECHNIQUE_NONE = 0x32,
	};

	enum $949C82572B6A70952C455E749D3B3D56
	{
		CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
		CONST_SRC_CODE_LIGHT_POSITION = 0x0,
		CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
		CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
		CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
		CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
		CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0x5,
		CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x6,
		CONST_SRC_CODE_GAMETIME = 0x7,
		CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x8,
		CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x8,
		CONST_SRC_CODE_PIXEL_COST_FRACS = 0x8,
		CONST_SRC_CODE_PIXEL_COST_DECODE = 0x9,
		CONST_SRC_CODE_FILTER_TAP_0 = 0xA,
		CONST_SRC_CODE_FILTER_TAP_1 = 0xB,
		CONST_SRC_CODE_FILTER_TAP_2 = 0xC,
		CONST_SRC_CODE_FILTER_TAP_3 = 0xD,
		CONST_SRC_CODE_FILTER_TAP_4 = 0xE,
		CONST_SRC_CODE_FILTER_TAP_5 = 0xF,
		CONST_SRC_CODE_FILTER_TAP_6 = 0x10,
		CONST_SRC_CODE_FILTER_TAP_7 = 0x11,
		CONST_SRC_CODE_COLOR_MATRIX_R = 0x12,
		CONST_SRC_CODE_COLOR_MATRIX_G = 0x13,
		CONST_SRC_CODE_COLOR_MATRIX_B = 0x14,
		CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x15,
		CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x16,
		CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x17,
		CONST_SRC_CODE_FIXED_PS_BEGIN = 0x17,
		CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x17,
		CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x18,
		CONST_SRC_CODE_DOF_LERP_SCALE = 0x19,
		CONST_SRC_CODE_DOF_LERP_BIAS = 0x1A,
		CONST_SRC_CODE_DOF_ROW_DELTA = 0x1B,
		CONST_SRC_CODE_MOTION_MATRIX_X = 0x1C,
		CONST_SRC_CODE_MOTION_MATRIX_Y = 0x1D,
		CONST_SRC_CODE_MOTION_MATRIX_W = 0x1E,
		CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x1F,
		CONST_SRC_CODE_SHADOWMAP_SCALE = 0x20,
		CONST_SRC_CODE_ZNEAR = 0x21,
		CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x22,
		CONST_SRC_CODE_DEBUG_BUMPMAP = 0x23,
		CONST_SRC_CODE_MATERIAL_COLOR = 0x24,
		CONST_SRC_CODE_FOG = 0x25,
		CONST_SRC_CODE_FOG_COLOR_LINEAR = 0x26,
		CONST_SRC_CODE_FOG_COLOR_GAMMA = 0x27,
		CONST_SRC_CODE_FOG_SUN_CONSTS = 0x28,
		CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR = 0x29,
		CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA = 0x2A,
		CONST_SRC_CODE_FOG_SUN_DIR = 0x2B,
		CONST_SRC_CODE_GLOW_SETUP = 0x2C,
		CONST_SRC_CODE_GLOW_APPLY = 0x2D,
		CONST_SRC_CODE_COLOR_BIAS = 0x2E,
		CONST_SRC_CODE_COLOR_TINT_BASE = 0x2F,
		CONST_SRC_CODE_COLOR_TINT_DELTA = 0x30,
		CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA = 0x31,
		CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x32,
		CONST_SRC_CODE_ENVMAP_PARMS = 0x33,
		CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST = 0x34,
		CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x35,
		CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x36,
		CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x37,
		CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x38,
		CONST_SRC_CODE_FRAMEBUFFER_READ = 0x39,
		CONST_SRC_CODE_FIXED_PS_END = 0x3A,
		CONST_SRC_CODE_NON_PS_BEGIN = 0x3A,
		CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x3A,
		CONST_SRC_CODE_LIGHT_PROBE_AMBIENT = 0x3B,
		CONST_SRC_CODE_NEARPLANE_ORG = 0x3C,
		CONST_SRC_CODE_NEARPLANE_DX = 0x3D,
		CONST_SRC_CODE_NEARPLANE_DY = 0x3E,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x3F,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x40,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0 = 0x41,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1 = 0x42,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2 = 0x43,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0 = 0x44,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1 = 0x45,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2 = 0x46,
		CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0 = 0x47,
		CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1 = 0x48,
		CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x49,
		CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x4A,
		CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x4B,
		CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x4B,
		CONST_SRC_CODE_NON_PS_END = 0x4C,
		CONST_SRC_CODE_COUNT_FLOAT4 = 0x4C,
		CONST_SRC_FIRST_CODE_MATRIX = 0x4C,
		CONST_SRC_CODE_VIEW_MATRIX = 0x4C,
		CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x4D,
		CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x4E,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x4F,
		CONST_SRC_CODE_PROJECTION_MATRIX = 0x50,
		CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x51,
		CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x52,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x53,
		CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x54,
		CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x55,
		CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x56,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x57,
		CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x58,
		CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x59,
		CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5A,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x5B,
		CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5C,
		CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5D,
		CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5E,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x5F,
		CONST_SRC_CODE_WORLD_MATRIX0 = 0x60,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX0 = 0x61,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0 = 0x62,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0 = 0x63,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX0 = 0x64,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0 = 0x65,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x66,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0 = 0x67,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x68,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x69,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6A,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0 = 0x6B,
		CONST_SRC_CODE_WORLD_MATRIX1 = 0x6C,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX1 = 0x6D,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1 = 0x6E,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1 = 0x6F,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX1 = 0x70,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1 = 0x71,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x72,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1 = 0x73,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x74,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x75,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x76,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1 = 0x77,
		CONST_SRC_CODE_WORLD_MATRIX2 = 0x78,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX2 = 0x79,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2 = 0x7A,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2 = 0x7B,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX2 = 0x7C,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2 = 0x7D,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7E,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2 = 0x7F,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x80,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x81,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x82,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2 = 0x83,
		CONST_SRC_TOTAL_COUNT = 0x84,
		CONST_SRC_NONE = 0x85,
	};
}
