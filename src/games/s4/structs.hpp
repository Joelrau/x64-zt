#pragma once

namespace zonetool::s4
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	typedef std::int32_t scr_string_t;

	enum XFileBlock : std::uint8_t
	{
		XFILE_BLOCK_TEMP_ADDITIONAL = 0,
		XFILE_BLOCK_TEMP = 1,
		XFILE_BLOCK_TEMP_PRELOAD = 2,
		XFILE_BLOCK_UNKNOWN_3 = 3,
		XFILE_BLOCK_UNKNOWN_4 = 4,
		XFILE_BLOCK_UNKNOWN_5 = 5,
		XFILE_BLOCK_RUNTIME = 6,
		XFILE_BLOCK_VIRTUAL = 7,
		XFILE_BLOCK_SCRIPT = 8,
		XFILE_BLOCK_PHYSICAL = 9,
		MAX_XFILE_COUNT = 10,
	};

	enum XAssetType : std::int32_t
	{
		ASSET_TYPE_PHYSICSLIBRARY = 0,
		ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET = 1,
		ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET = 2,
		ASSET_TYPE_PHYSICSASSET = 3,
		ASSET_TYPE_PHYSICS_FX_PIPELINE = 4,
		ASSET_TYPE_PHYSICS_FX_SHAPE = 5,
		ASSET_TYPE_PHYSICS_DEBUG_DATA = 6,
		ASSET_TYPE_XANIMPARTS = 7,
		ASSET_TYPE_XMODEL_SURFS = 8,
		ASSET_TYPE_XMODEL = 9,
		ASSET_TYPE_MAYHEM = 10,
		ASSET_TYPE_MATERIAL = 11,
		ASSET_TYPE_COMPUTESHADER = 12,
		ASSET_TYPE_LIBSHADER = 13,
		ASSET_TYPE_VERTEXSHADER = 14,
		ASSET_TYPE_HULLSHADER = 15,
		ASSET_TYPE_DOMAINSHADER = 16,
		ASSET_TYPE_PIXELSHADER = 17,
		ASSET_TYPE_TECHNIQUE_SET = 18,
		ASSET_TYPE_IMAGE = 19,
		ASSET_TYPE_SOUND_GLOBALS = 20,
		ASSET_TYPE_SOUND_MODE_GLOBALS = 21,
		ASSET_TYPE_SOUND_BANK = 22,
		ASSET_TYPE_SOUND_BANK_TRANSIENT = 23,
		ASSET_TYPE_CLIPMAP = 24,
		ASSET_TYPE_COMWORLD = 25,
		ASSET_TYPE_GLASSWORLD = 26,
		ASSET_TYPE_PATHDATA = 27,
		ASSET_TYPE_NAVMESH = 28,
		ASSET_TYPE_TACGRAPH = 29,
		ASSET_TYPE_AI_REGION_GRAPHS = 30,
		ASSET_TYPE_MAP_ENTS = 31,
		ASSET_TYPE_FXWORLD = 32,
		ASSET_TYPE_GFXWORLD = 33,
		ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE = 34,
		ASSET_TYPE_IESPROFILE = 35,
		ASSET_TYPE_LIGHT_DEF = 36,
		ASSET_TYPE_GRADING_CLUT = 37,
		ASSET_TYPE_UI_MAP = 38,
		ASSET_TYPE_FOG_SPLINE = 39,
		ASSET_TYPE_ANIMCLASS = 40,
		ASSET_TYPE_PLAYERANIM = 41,
		ASSET_TYPE_GESTURE = 42,
		ASSET_TYPE_LOCALIZE_ENTRY = 43,
		ASSET_TYPE_ATTACHMENT = 44,
		ASSET_TYPE_WEAPON = 45,
		ASSET_TYPE_VFX = 46,
		ASSET_TYPE_IMPACT_FX = 47,
		ASSET_TYPE_SURFACE_FX = 48,
		ASSET_TYPE_AITYPE = 49,
		ASSET_TYPE_MPTYPE = 50,
		ASSET_TYPE_CHARACTER = 51,
		ASSET_TYPE_XMODELALIAS = 52,
		ASSET_TYPE_RAWFILE = 53,
		ASSET_TYPE_SCRIPTFILE = 54,
		ASSET_TYPE_SCRIPT_DEBUG_DATA = 55,
		ASSET_TYPE_STRINGTABLE = 56,
		ASSET_TYPE_LEADERBOARD = 57,
		ASSET_TYPE_VIRTUAL_LEADERBOARD = 58,
		ASSET_TYPE_DDL = 59,
		ASSET_TYPE_TRACER = 60,
		ASSET_TYPE_VEHICLE = 61,
		ASSET_TYPE_ADDON_MAP_ENTS = 62,
		ASSET_TYPE_NET_CONST_STRINGS = 63,
		ASSET_TYPE_LUA_FILE = 64,
		ASSET_TYPE_SCRIPTABLE = 65,
		ASSET_TYPE_EQUIPMENT_SND_TABLE = 66,
		ASSET_TYPE_VECTORFIELD = 67,
		ASSET_TYPE_PARTICLE_SIM_ANIMATION = 68,
		ASSET_TYPE_STREAMING_INFO = 69,
		ASSET_TYPE_LASER = 70,
		ASSET_TYPE_GAME_PROPS = 71,
		ASSET_TYPE_MATERIAL_STANDARD = 72,
		ASSET_TYPE_BEAM = 73,
		ASSET_TYPE_TTF = 74,
		ASSET_TYPE_SUIT = 75,
		ASSET_TYPE_SUITANIMPACKAGE = 76,
		ASSET_TYPE_CAMERA = 77,
		ASSET_TYPE_HUDOUTLINE = 78,
		ASSET_TYPE_RUMBLE = 79,
		ASSET_TYPE_RUMBLE_GRAPH = 80,
		ASSET_TYPE_ANIM_PACKAGE = 81,
		ASSET_TYPE_SFX_PACKAGE = 82,
		ASSET_TYPE_VFX_PACKAGE = 83,
		ASSET_TYPE_FOOTSTEP_VFX = 84,
		ASSET_TYPE_BEHAVIOR_TREE = 85,
		ASSET_TYPE_BEHAVIOR_SEQUENCER = 86,
		ASSET_TYPE_SIGHT_CONFIG = 87,
		ASSET_TYPE_AI_ANIM_SET = 88,
		ASSET_TYPE_AI_ASM = 89,
		ASSET_TYPE_XANIM_PROCEDURALBONES = 90,
		ASSET_TYPE_XANIM_DYNAMICBONES = 91,
		ASSET_TYPE_RETICLE = 92,
		ASSET_TYPE_XANIMCURVE = 93,
		ASSET_TYPE_COVER_SELECTOR = 94,
		ASSET_TYPE_ENEMY_SELECTOR = 95,
		ASSET_TYPE_CLIENT_CHARACTER = 96,
		ASSET_TYPE_CLOTH_ASSET = 97,
		ASSET_TYPE_CINEMATIC_MOTION = 98,
		ASSET_TYPE_ACCESSORY = 99,
		ASSET_TYPE_LOC_DMG_TABLE = 100,
		ASSET_TYPE_BULLET_PENETRATION = 101,
		ASSET_TYPE_SCRIPT_BUNDLE = 102,
		ASSET_TYPE_BLEND_SPACE_2D = 103,
		ASSET_TYPE_XCAM = 104,
		ASSET_TYPE_CAMO = 105,
		ASSET_TYPE_XCOMPOSITEMODEL = 106,
		ASSET_TYPE_XMODEL_DETAIL_COLLISION = 107,
		ASSET_TYPE_STREAM_KEY = 108,
		ASSET_TYPE_STREAM_TREE_OVERRIDE = 109,
		ASSET_TYPE_KEY_VALUE_PAIRS = 110,
		ASSET_TYPE_ST_TERRAIN = 111,
		ASSET_TYPE_NATIVE_SCRIPT_PATCH = 112,
		ASSET_TYPE_COLLISION_TILE = 113,
		ASSET_TYPE_EXECUTION = 114,
		ASSET_TYPE_CARRY_OBJECT = 115,
		ASSET_TYPE_SOUND_BANK_LIST = 116,
		ASSET_TYPE_WEAPON_ACCURACY = 117,
		ASSET_TYPE_DECAL_VOLUME_MATERIAL = 118,
		ASSET_TYPE_DECAL_VOLUME_MASK = 119,
		ASSET_TYPE_DYN_ENTITY_LIST = 120,
		ASSET_TYPE_FXWORLD_TRANSIENT_ZONE = 121,
		ASSET_TYPE_VOLUMETRIC_HEIGHTMAP = 122,
		ASSET_TYPE_DLOG_SCHEMA = 123,
		ASSET_TYPE_EDGE_LIST = 124,
		ASSET_TYPE_XBONESET = 125,
		ASSET_TYPE_RAGDOLL_ASSET = 126,
		ASSET_TYPE_PHYSICS_BONE_GRAPH = 127,
		ASSET_TYPE_CURVE = 128,
		ASSET_TYPE_SKELETON_CONSTRAINTS = 129,
		ASSET_TYPE_TRIGGER_EFFECT = 130,
		ASSET_TYPE_WEAPON_TRIGGER = 131,
		ASSET_TYPE_VOLUMETRIC_CLOUD = 132,
		ASSET_TYPE_CODCASTER_DATA = 133,
		ASSET_TYPE_WATER_SYSTEM = 134,
		ASSET_TYPE_WATER_BUOYANCY = 135,
		ASSET_TYPE_KEYBINDS = 136,
		ASSET_TYPE_CALLOUT_MARKER_PING = 137,
		ASSET_TYPE_LIGHT_STATE = 138,
		ASSET_TYPE_RADIANT_TELEMETRY = 139,
		ASSET_TYPE_AI_MARKUP = 140,
		ASSET_TYPE_SCENARIO = 141,
		ASSET_TYPE_LEAGUE_PLAY_SEASONS = 142,
		ASSET_TYPE_REACTIVE_STAGE_SET = 143,
		ASSET_TYPE_COUNT = 144,
	};

	struct RawFile
	{
		const char* name;
		int compressedLen;
		int len;
		const char* buffer;
	}; assert_sizeof(RawFile, 0x18);

	struct ScriptFile
	{
		const char* name;
		std::uint64_t unknown;
		int compressedLen;
		int len;
		int bytecodeLen;
		const char* buffer;
		char* bytecode;
	}; assert_sizeof(ScriptFile, 0x30);
	assert_offsetof(ScriptFile, buffer, 0x20);

	enum NetConstStringType : std::int32_t
	{
		NETCONSTSTRINGTYPE_XMODEL = 0x0,
		NETCONSTSTRINGTYPE_MATERIAL = 0x1,
		NETCONSTSTRINGTYPE_IMAGE = 0x2,
		NETCONSTSTRINGTYPE_RUMBLE = 0x3,
		NETCONSTSTRINGTYPE_RUMBLEGRAPH = 0x4,
		NETCONSTSTRINGTYPE_VEHICLES = 0x5,
		NETCONSTSTRINGTYPE_VFX = 0x6,
		NETCONSTSTRINGTYPE_LOCSTRING = 0x7,
		NETCONSTSTRINGTYPE_SHOCK = 0x8,
		NETCONSTSTRINGTYPE_CLIENT_TAGS = 0x9,
		NETCONSTSTRINGTYPE_STATUSICON = 0xA,
		NETCONSTSTRINGTYPE_HEADICON = 0xB,
		NETCONSTSTRINGTYPE_OBJECTIVEICON = 0xC,
		NETCONSTSTRINGTYPE_MINIMAPICON = 0xD,
		NETCONSTSTRINGTYPE_WEAPON = 0xE,
		NETCONSTSTRINGTYPE_HINTSTRING = 0xF,
		NETCONSTSTRINGTYPE_ANIM = 0x10,
		NETCONSTSTRINGTYPE_ANIMCLASS = 0x11,
		NETCONSTSTRINGTYPE_ANIMSET = 0x12,
		NETCONSTSTRINGTYPE_LUI = 0x13,
		NETCONSTSTRINGTYPE_SUIT = 0x14,
		NETCONSTSTRINGTYPE_CAMERA = 0x15,
		NETCONSTSTRINGTYPE_HUDOUTLINE = 0x16,
		NETCONSTSTRINGTYPE_GESTURE = 0x17,
		NETCONSTSTRINGTYPE_SPACESHIPTARGET = 0x18,
		NETCONSTSTRINGTYPE_VISIONSET = 0x19,
		NETCONSTSTRINGTYPE_ATR = 0x1A,
		NETCONSTSTRINGTYPE_CLIENTCHARACTER = 0x1B,
		NETCONSTSTRINGTYPE_XANIMCURVE = 0x1C,
		NETCONSTSTRINGTYPE_CINEMATICMOTION = 0x1D,
		NETCONSTSTRINGTYPE_ACCESSORY = 0x1E,
		NETCONSTSTRINGTYPE_BLENDSPACE2D = 0x1F,
		NETCONSTSTRINGTYPE_CAMO = 0x20,
		NETCONSTSTRINGTYPE_PLAYERANIMSET = 0x21,
		NETCONSTSTRINGTYPE_AITYPE = 0x22,
		NETCONSTSTRINGTYPE_SCRIPTABLEDEF = 0x23,
		NETCONSTSTRINGTYPE_XCOMPOSITEMODEL = 0x24,
		NETCONSTSTRINGTYPE_EXECUTION = 0x25,
		NETCONSTSTRINGTYPE_CARRYOBJECT = 0x26,
		NETCONSTSTRINGTYPE_STICKER = 0x27,
		NETCONSTSTRINGTYPE_VEHICLE_CAMO = 0x28,
		NETCONSTSTRINGTYPE_COUNT = 0x29,
	};

	enum NetConstStringSource : std::int32_t
	{
		NETCONSTSTRINGSOURCE_GLOBAL = 0x0,
		NETCONSTSTRINGSOURCE_COMMON = 0x1,
		NETCONSTSTRINGSOURCE_PRE_MAP = 0x2,
		NETCONSTSTRINGSOURCE_MAP = 0x3,
		NETCONSTSTRINGSOURCE_MYCHANGES = 0x4,
		NETCONSTSTRINGSOURCE_NONE = 0x5,
	};

	struct NetConstStrings
	{
		const char* name;
		NetConstStringType stringType;
		NetConstStringSource sourceType;
		unsigned int flags;
		unsigned int entryCount;
		const char** stringList;
	}; assert_sizeof(NetConstStrings, 0x20);

	struct KeyValuePair
	{
		unsigned int keyHash;
		const char* value;
	}; assert_sizeof(KeyValuePair, 0x10);

	struct KeyValuePairs
	{
		const char* name;
		int numVariables;
		KeyValuePair* keyValuePairs;
	}; assert_sizeof(KeyValuePairs, 0x18);

	struct PhysicsSFXEventAssetEntryA
	{
		const char* name;
		char data[0x40];
	}; assert_sizeof(PhysicsSFXEventAssetEntryA, 0x48);

	struct PhysicsSFXEventAssetListA
	{
		std::uint64_t unknown;
		int count;
		PhysicsSFXEventAssetEntryA* entries;
	}; assert_sizeof(PhysicsSFXEventAssetListA, 0x18);

	struct PhysicsSFXEventAssetEntryB
	{
		const char* name;
		char data[0x20];
	}; assert_sizeof(PhysicsSFXEventAssetEntryB, 0x28);

	struct PhysicsSFXEventAssetListB
	{
		int count;
		PhysicsSFXEventAssetEntryB* entries;
	}; assert_sizeof(PhysicsSFXEventAssetListB, 0x10);

	struct PhysicsSFXEventAssetRules
	{
		PhysicsSFXEventAssetListA* a;
		PhysicsSFXEventAssetListB* b;
	}; assert_sizeof(PhysicsSFXEventAssetRules, 0x10);

	struct PhysicsSFXEventAsset
	{
		const char* name;
		PhysicsSFXEventAssetRules* rules;
	}; assert_sizeof(PhysicsSFXEventAsset, 0x10);

	struct PhysicsVFXEventAssetEntryA
	{
		const char* name;
		char data[0x18];
	}; assert_sizeof(PhysicsVFXEventAssetEntryA, 0x20);

	struct PhysicsVFXEventAssetListA
	{
		std::uint64_t unknown;
		int count;
		PhysicsVFXEventAssetEntryA* entries;
	}; assert_sizeof(PhysicsVFXEventAssetListA, 0x18);

	struct PhysicsVFXEventAssetEntryB
	{
		const char* name;
		char data[0x8];
	}; assert_sizeof(PhysicsVFXEventAssetEntryB, 0x10);

	struct PhysicsVFXEventAssetListB
	{
		int count;
		PhysicsVFXEventAssetEntryB* entries;
	}; assert_sizeof(PhysicsVFXEventAssetListB, 0x10);

	struct PhysicsVFXEventAssetRules
	{
		PhysicsVFXEventAssetListA* a;
		PhysicsVFXEventAssetListB* b;
	}; assert_sizeof(PhysicsVFXEventAssetRules, 0x10);

	struct PhysicsVFXEventAsset
	{
		const char* name;
		PhysicsVFXEventAssetRules* rules;
	}; assert_sizeof(PhysicsVFXEventAsset, 0x10);

	enum PhysicsAssetUseCategory : std::int32_t
	{
		PhysicsAssetUseCategory_StaticModel = 0x0,
		PhysicsAssetUseCategory_CommonEntity = 0x1,
		PhysicsAssetUseCategory_Vehicle = 0x2,
		PhysicsAssetUseCategory_Dynent = 0x3,
		PhysicsAssetUseCategory_EntityOrDynent = 0x4,
		PhysicsAssetUseCategory_Ragdoll = 0x5,
		PhysicsAssetUseCategory_Particle = 0x6,
		PhysicsAssetUseCategory_Custom = 0x7,
	};

	enum PhysicsAssetSimulationCategory : std::int32_t
	{
		PhysicsAssetSimulationCategory_Static = 0x0,
		PhysicsAssetSimulationCategory_KeyframedEntity = 0x1,
		PhysicsAssetSimulationCategory_KeyframedAnimated = 0x2,
		PhysicsAssetSimulationCategory_KeyframedBoneOffset = 0x3,
		PhysicsAssetSimulationCategory_DynamicEntity = 0x4,
		PhysicsAssetSimulationCategory_DynamicDebris = 0x5,
		PhysicsAssetSimulationCategory_Ragdoll = 0x6,
		PhysicsAssetSimulationCategory_VehicleChassis = 0x7,
		PhysicsAssetSimulationCategory_VehicleChassisNoForce = 0x8,
		PhysicsAssetSimulationCategory_Custom = 0x9,
	};

	struct PhysicsAssetUsageCounter
	{
		int serverEnt;
		int clientEnt;
		int dynEnt;
	};

	struct PhysicsAsset
	{
		const char* name;
		char* havokData;
		unsigned int havokDataSize;
		PhysicsAssetUseCategory useCategory;
		int numRigidBodies;
		int numConstraints;
		PhysicsAssetSimulationCategory* simulationCategories;
		unsigned int* bodyContents;
		int numSFXEventAssets;
		int numVFXEventAssets;
		PhysicsSFXEventAsset** sfxEventAssets;
		PhysicsVFXEventAsset** vfxEventAssets;
		int numSFXEventAssets2;
		int numVFXEventAssets2;
		PhysicsSFXEventAsset** sfxEventAssets2;
		PhysicsVFXEventAsset** vfxEventAssets2;
		PhysicsAssetUsageCounter usageCounter;
		bool containsDynamicBodies;
		char unknown[0xB];
	}; assert_sizeof(PhysicsAsset, 0x78);

	struct RagdollAsset
	{
		const char* name;
		PhysicsAsset* physicsAsset;
		char unknown[0x50];
	}; assert_sizeof(RagdollAsset, 0x60);

	struct PhysicsBoneGraphNode
	{
		scr_string_t name;
		char unknown[0xC];
	}; assert_sizeof(PhysicsBoneGraphNode, 0x10);

	struct PhysicsBoneGraphGroup
	{
		scr_string_t name;
		scr_string_t* nodeNames;
		unsigned short nodeNameCount;
		char unknown[6];
	}; assert_sizeof(PhysicsBoneGraphGroup, 0x18);

	struct PhysicsBoneGraph
	{
		const char* name;
		PhysicsBoneGraphNode* nodes;
		unsigned short* indices;
		unsigned short nodeCount;
		unsigned short indexCount;
		PhysicsBoneGraphGroup* groups;
		unsigned short groupCount;
	}; assert_sizeof(PhysicsBoneGraph, 0x30);
	assert_offsetof(PhysicsBoneGraph, groupCount, 0x28);

	enum PhysicsFXPipelineIntegratorType : std::int32_t
	{
		Types_PhysicsFXPipelineIntegrator_Basic = 0x0,
		Types_PhysicsFXPipelineIntegrator_LinAngDamping = 0x1,
		Types_PhysicsFXPipelineIntegratorCount = 0x2,
	};

	struct PhysicsFXPipelineIntegrator_Basic
	{
	};

	struct PhysicsFXPipelineIntegrator_LinVelDamping
	{
		PhysicsFXPipelineIntegrator_Basic basic;
		float linearDamping;
		float angularDamping;
	}; assert_sizeof(PhysicsFXPipelineIntegrator_LinVelDamping, 0xC);

	union PhysicsFXPipelineIntegratorUnion
	{
		PhysicsFXPipelineIntegrator_Basic basicIntegrator;
		PhysicsFXPipelineIntegrator_LinVelDamping linVelDampingIntegrator;
	};

	struct PhysicsFXPipelineIntegrator
	{
		PhysicsFXPipelineIntegratorType type;
		PhysicsFXPipelineIntegratorUnion u;
	}; assert_sizeof(PhysicsFXPipelineIntegrator, 0x10);

	struct PhysicsFXPipeline
	{
		const char* name;
		int numParticlesPerInstance;
		float friction;
		float restitution;
		bool particleParticleCollisions;
		bool raiseCollisionCallbacks;
		float callbackImpulseThreshold;
		PhysicsFXPipelineIntegrator integrator;
	}; assert_sizeof(PhysicsFXPipeline, 0x30);

	struct PhysicsFXShape
	{
		const char* name;
		unsigned int havokDataSize;
		char* havokData;
		const char* soundAlias;
		float soundMinImpulse;
		float soundMaxImpulse;
		float soundMinImpulseVolume;
		float soundMaxImpulseVolume;
		char unknown[8];
	}; assert_sizeof(PhysicsFXShape, 0x38);

	union XAnimIndices
	{
		unsigned char* _1;
		unsigned short* _2;
		void* data;
	};

	struct XAnimNotifyInfo
	{
		scr_string_t name;
		float time;
		char unknown[8];
	}; assert_sizeof(XAnimNotifyInfo, 0x10);

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
		unsigned short size;
		unsigned char smallTrans;
		XAnimPartTransData u;
	};

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

	struct XAnimDeltaPart
	{
		XAnimPartTrans* trans;
		XAnimDeltaPartQuat2* quat2;
		XAnimDeltaPartQuat* quat;
	};

	struct XAnimExtraChannels
	{
		scr_string_t* names;
		unsigned int* values;
		unsigned char* frameData;
		char unknown[0x12];
		unsigned short nameCount;
		unsigned short valueCount;
		unsigned short frameDataStride;
	}; assert_sizeof(XAnimExtraChannels, 0x30);

	enum XAnimPartType
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

	enum XAnimAssetType : std::uint8_t
	{
		XANIM_ASSET_TYPE_ABSOLUTE = 1,
		XANIM_ASSET_TYPE_ADDITIVE = 6,
	};

	enum XAnimPartsFlags : std::uint8_t
	{
		ANIM_LOOP = 0x1,
		ANIM_DELTA = 0x2,
		ANIM_DELTA_3D = 0x4,
		ANIM_DEFAULT = 0x8,
		ANIM_SHMEM = 0x10,
	};

	struct XAnimParts
	{
		const char* name;
		scr_string_t* names;
		char* dataByte;
		short* dataShort;
		int* dataInt;
		short* randomDataShort;
		char* randomDataByte;
		int* randomDataInt;
		XAnimIndices indices;
		XAnimNotifyInfo* notify;
		XAnimDeltaPart* deltaPart;
		XAnimExtraChannels* extraChannels;
		unsigned int randomDataShortCount;
		unsigned int randomDataByteCount;
		unsigned int indexCount;
		float framerate;
		float frequency;
		unsigned int dataByteCount;
		unsigned short dataShortCount;
		unsigned short dataIntCount;
		unsigned short randomDataIntCount;
		unsigned short numframes;
		unsigned char flags;
		unsigned char boneCount[10];
		unsigned char notifyCount;
		unsigned char assetType;
		unsigned char ikType;
		unsigned char fingerPoseType;
		unsigned short blendShapeWeightCount;
		scr_string_t* blendShapeWeightNames;
		unsigned short* blendShapeWeights;
	}; assert_sizeof(XAnimParts, 0xA8);
	assert_offsetof(XAnimParts, boneCount, 0x81);
	assert_offsetof(XAnimParts, blendShapeWeightCount, 0x90);

	struct ID3D12Resource;

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	}; assert_sizeof(Bounds, 0x18);

	struct DObjPartBits
	{
		unsigned int array[8];
	}; assert_sizeof(DObjPartBits, 0x20);

	struct streamer_handle_t
	{
		unsigned __int64 data;
	};

	struct XPakEntryInfo
	{
		unsigned __int64 key;
		__int64 offset;
		unsigned __int64 size;
		unsigned __int64 xpakIndex : 8;
		unsigned __int64 compressed : 1;
		unsigned __int64 valid : 1;
		unsigned __int64 adjacentLeftType : 3;
		unsigned __int64 adjacentRightType : 3;
		unsigned __int64 adjacentLeft : 19;
		unsigned __int64 adjacentRight : 19;
		unsigned __int64 padding : 10;
	}; assert_sizeof(XPakEntryInfo, 0x20);

	struct XSubFileHeader
	{
		unsigned int magic;
		unsigned short unknown;
		unsigned short version;
		unsigned __int64 unknown2;
		unsigned __int64 type;
		unsigned __int64 fileSize;
		unsigned char unknown3[0x768];
		unsigned __int64 fileCount;
		unsigned __int64 dataOffset;
		unsigned __int64 dataSize;
		unsigned __int64 hashCount;
		unsigned __int64 hashOffset;
		unsigned __int64 hashSize;
	}; assert_sizeof(XSubFileHeader, 0x7B8);
	assert_offsetof(XSubFileHeader, fileCount, 0x788);

	struct XSubHashEntry
	{
		unsigned __int64 key;
		unsigned __int64 compressed : 1;
		unsigned __int64 size : 30;
		unsigned __int64 unknownFlag : 1;
		unsigned __int64 offset : 32;
	}; assert_sizeof(XSubHashEntry, 0x10);

	enum XSubBlockType : unsigned char
	{
		XSUB_BLOCK_UNCOMPRESSED = 0x0,
		XSUB_BLOCK_OODLE = 0x6,
	};

#pragma pack(push, 1)
	struct XSubBlock
	{
		XSubBlockType type;
		unsigned int compressedSize;
		unsigned int decompressedSize;
		unsigned int compressedOffset;
		unsigned int decompressedOffset;
		unsigned int unknown;
	}; assert_sizeof(XSubBlock, 0x15);

	struct XSubBlockHeader
	{
		unsigned short magic;
		unsigned __int64 key;
		unsigned int offset;
		unsigned int nextOffset;
		unsigned short chunkCount;
		unsigned short chunkIndex;
		unsigned char blockCount;
	}; assert_sizeof(XSubBlockHeader, 0x17);
#pragma pack(pop)

	enum XSurfaceSharedFlags : std::uint32_t
	{
		XSURFACE_SHARED_STREAMED = 0x1,
	};

	union XSurfaceSharedData
	{
		streamer_handle_t streamedDataHandle;
		char* residentData;
	};

	struct XSurfaceShared
	{
		XSurfaceSharedData data;
		unsigned int dataSize;
		XSurfaceSharedFlags flags;
	}; assert_sizeof(XSurfaceShared, 0x10);

	struct PackedLmapCoords
	{
		unsigned int packed;
	};

	struct XRigidVertList
	{
		unsigned short boneIndexOffset;
		unsigned short vertCount;
		unsigned short triOffset;
		unsigned short triCount;
	}; assert_sizeof(XRigidVertList, 0x8);

	struct XSurfaceSubdivLevel
	{
		unsigned int unknown[4];
		unsigned int facePointCount;
		unsigned int facePointValence4Count;
		unsigned int facePointBufferSize;
		unsigned int edgePointCount;
		unsigned int edgePointSmoothEnd;
		unsigned int edgePointUVBorderEnd;
		unsigned int vertexPointCount;
		unsigned int vertexPointValence4Count;
		unsigned int vertexPointBufferSize;
		unsigned int normalCount;
		unsigned int normalBufferSize;
		unsigned int unknown2;
		unsigned int* facePoints;
		unsigned int* edgePoints;
		unsigned int* vertexPoints;
		unsigned int* normals;
		char runtimeBuffers[0x100];
	}; assert_sizeof(XSurfaceSubdivLevel, 0x160);
	assert_offsetof(XSurfaceSubdivLevel, facePoints, 0x40);

	struct BlendShapesPerVert
	{
		unsigned __int64* perVertBlendShapeData;
		unsigned short* surfBlendShapesData;
		unsigned int perVertBlendShapeDataSize;
		unsigned short surfBlendShapeCount;
		unsigned short modelWeightsCount;
		unsigned short tangentVertCount;
		unsigned short noTangentVertCount;
		unsigned short blendMapVertCount;
		unsigned short surfFirstBlendMapShapeIndex;
	}; assert_sizeof(BlendShapesPerVert, 0x20);

	struct BlendShapesRecalcTangentFrameData
	{
		unsigned int bufferSize;
		unsigned short* topologicalData;
	}; assert_sizeof(BlendShapesRecalcTangentFrameData, 0x10);

	enum XSurfaceFlags : std::uint16_t
	{
		SURF_FLAG_VERTCOL = 0x1,
		SURF_FLAG_SKINNED = 0x2,
		SURF_FLAG_REACTIVE_MOTION = 0x4,
		SURF_FLAG_LIGHTMAP_COORDS = 0x8,
		SURF_FLAG_SUBDIV = 0x10,
		SURF_FLAG_UNKNOWN_20 = 0x20,
		SURF_FLAG_SECONDUV = 0x40,
		SURF_FLAG_UNKNOWN_200 = 0x200,
	};

	struct Face
	{
		unsigned short v1;
		unsigned short v2;
		unsigned short v3;
	}; assert_sizeof(Face, 0x6);

#pragma pack(push, 4)
	struct XSurfaceTriCluster
	{
		unsigned __int64 mins;
		unsigned __int64 maxs;
		unsigned char cone[12];
	}; assert_sizeof(XSurfaceTriCluster, 0x1C);
#pragma pack(pop)

	struct XSurfaceChildBounds
	{
		unsigned short midPoint[3];
		unsigned short halfSize[3];
	}; assert_sizeof(XSurfaceChildBounds, 0xC);

	struct XSurface
	{
		unsigned short flags;
		unsigned short vertCount;
		unsigned short triCount;
		unsigned short blendShapeTargetCount;
		unsigned char rigidVertListCount;
		unsigned char subdivLevelCount;
		unsigned short ugbID;
		unsigned int hash;
		unsigned int unknownVertCount;
		unsigned int unknownTriCount;
		unsigned int unknownHash;
		unsigned short blendVertCounts[8];
		unsigned int blendVertSize;
		float xyzScale;
		unsigned int sharedXyzDataOffset;
		unsigned int sharedTexCoordDataOffset;
		unsigned int sharedTangentFrameDataOffset;
		unsigned int sharedIndexDataOffset;
		unsigned int sharedTriClusterDataOffset;
		unsigned int sharedSelfVisibilityDataOffset;
		unsigned int sharedColorDataOffset;
		unsigned int sharedSecondUVDataOffset;
		unsigned int sharedNormalTransformDataOffset;
		XSurfaceShared* shared;
		PackedLmapCoords* lmapCoords;
		XRigidVertList* rigidVertLists;
		unsigned short* blendVerts;
		XSurfaceSubdivLevel* subdivLevels;
		DObjPartBits partBits;
		Bounds surfBounds;
		XSurfaceChildBounds* childBounds;
		BlendShapesPerVert* blendShapesPerVert;
		BlendShapesRecalcTangentFrameData* blendShapesRecalcTangentFrameData;
	}; assert_sizeof(XSurface, 0xD0);
	assert_offsetof(XSurface, blendVertSize, 0x2C);
	assert_offsetof(XSurface, shared, 0x58);
	assert_offsetof(XSurface, childBounds, 0xB8);

	struct XModelSurfs
	{
		const char* name;
		XSurface* surfs;
		unsigned __int64 xpakKey;
		char unknown[0x8];
		XSurfaceShared* shared;
		unsigned short numsurfs;
		unsigned char ugbState[1];
		DObjPartBits partBits;
	}; assert_sizeof(XModelSurfs, 0x50);
	assert_offsetof(XModelSurfs, numsurfs, 0x28);

	enum GfxTextureId : std::uint32_t
	{
		NULLID = 0x0,
	};

	enum GfxPixelFormat : std::uint8_t
	{
		GFX_PF_INVALID = 0x0,
		GFX_PF_R8 = 0x1,
		GFX_PF_A8 = 0x2,
		GFX_PF_L8 = 0x3,
		GFX_PF_R8G8 = 0x4,
		GFX_PF_L8A8 = 0x5,
		GFX_PF_R8G8B8A8 = 0x6,
		GFX_PF_R8G8B8A8_SRGB = 0x7,
		GFX_PF_R8_SNORM = 0x8,
		GFX_PF_R8G8_SNORM = 0x9,
		GFX_PF_R16 = 0xA,
		GFX_PF_R16G16 = 0xB,
		GFX_PF_R16G16B16A16 = 0xC,
		GFX_PF_R16_SNORM = 0xD,
		GFX_PF_UNKNOWN_E = 0xE,
		GFX_PF_UNKNOWN_F = 0xF,
		GFX_PF_R16F = 0x10,
		GFX_PF_R16G16F = 0x11,
		GFX_PF_R16G16B16A16F = 0x12,
		GFX_PF_R32F = 0x13,
		GFX_PF_R32G32F = 0x14,
		GFX_PF_R32G32B32A32F = 0x15,
		GFX_PF_D16 = 0x16,
		GFX_PF_D32F = 0x17,
		GFX_PF_D32F_S8 = 0x18,
		GFX_PF_R8_UINT = 0x19,
		GFX_PF_R16_UINT = 0x1A,
		GFX_PF_R32_UINT = 0x1B,
		GFX_PF_R32G32_UINT = 0x1C,
		GFX_PF_R32G32B32A32_UINT = 0x1D,
		GFX_PF_R10G10B10A2_UINT = 0x1E,
		GFX_PF_R5G6B5 = 0x1F,
		GFX_PF_R10G10B10A2 = 0x20,
		GFX_PF_R9G9B9E5_SHAREDEXP = 0x21,
		GFX_PF_R11G11B10F = 0x22,
		GFX_PF_BC1 = 0x23,
		GFX_PF_BC1_SRGB = 0x24,
		GFX_PF_BC2 = 0x25,
		GFX_PF_BC2_SRGB = 0x26,
		GFX_PF_BC3 = 0x27,
		GFX_PF_BC3_SRGB = 0x28,
		GFX_PF_BC4 = 0x29,
		GFX_PF_BC5 = 0x2A,
		GFX_PF_BC5S = 0x2B,
		GFX_PF_BC6H = 0x2C,
		GFX_PF_BC6HS = 0x2D,
		GFX_PF_BC7 = 0x2E,
		GFX_PF_BC7_SRGB = 0x2F,
	};

	enum GfxImageFlags : std::uint32_t
	{
		IMG_FLAG_NONE = 0x0,
		IMG_DISK_FLAG_NOPICMIP = 0x1,
		IMG_DISK_FLAG_NOMIPMAPS = 0x2,
		IMG_DISK_FLAG_UNUSED = 0x4,
		IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS = 0x8,
		IMG_DISK_FLAG_CLAMP_U = 0x10,
		IMG_DISK_FLAG_CLAMP_V = 0x20,
		IMG_DISK_FLAG_STREAMED = 0x40,
		IMG_DISK_FLAG_USE_OODLE_COMPRESSION = 0x80,
		IMG_DISK_FLAG_GAMMA_SRGB = 0x100,
		IMG_DISK_FLAG_PACKED_ATLAS = 0x200,
		IMG_CREATE_FLAG_UNTILED = 0x400,
		IMG_CREATE_FLAG_CPU_READ = 0x800,
		IMG_CREATE_FLAG_CPU_WRITE = 0x1000,
		IMG_DISK_FLAG_AUTOMETALNESS = 0x2000,
		IMG_DISK_FLAG_AUTODISPLACEMENT = 0x4000,
		IMG_DISK_FLAG_MAPTYPE_2D = 0x0,
		IMG_DISK_FLAG_MAPTYPE_CUBE = 0x8000,
		IMG_DISK_FLAG_MAPTYPE_3D = 0x10000,
		IMG_DISK_FLAG_MAPTYPE_1D = 0x18000,
		IMG_DISK_FLAG_MAPTYPE_ARRAY = 0x20000,
		IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY = 0x28000,
		IMG_DISK_FLAG_MAPTYPE_MASK = 0x38000,
		IMG_DISK_FLAG_INVERT_ALPHA = 0x40000,
		IMG_DISK_FLAG_PREMUL_ALPHA = 0x80000,
		IMG_DISK_FLAG_MIPGEN_ORIGINAL = 0x0,
		IMG_DISK_FLAG_MIPGEN_LANCZOS3 = 0x100000,
		IMG_DISK_FLAG_MIPGEN_CATMULL_ROM = 0x200000,
		IMG_DISK_FLAG_MIPGEN_CUBIC_BSPLINE = 0x300000,
		IMG_DISK_FLAG_MIPGEN_BOX = 0x400000,
		IMG_DISK_FLAG_MIPGEN_COVERAGE_PRESERVING = 0x500000,
		IMG_CREATE_FLAG_RW_VIEW = 0x800000,
		IMG_CREATE_FLAG_DYNAMIC = 0x1000000,
		IMG_DISK_FLAG_PREMUL_KEEP_ALPHA = 0x2000000,
		IMG_DISK_FLAG_RTT = 0x4000000,
		IMG_DISK_FLAG_EXTRACT_ALPHA = 0x8000000,
		IMG_DISK_FLAG_OCTAHEDRON = 0x10000000,
		IMG_CREATE_FLAG_STAGING = 0x20000000,
		IMG_CREATE_FLAG_VOLUMETRIC_LAYOUT_OVERRIDE = 0x40000000,
		IMG_CREATE_FLAG_TYPELESS = 0x80000000,
	};

	enum TextureSemantic : std::uint8_t
	{
		TS_2D = 0x0,
		TS_FUNCTION = 0x1,
		TS_COLOR_MAP = 0x2,
		TS_GRAY_MAP = 0x3,
		TS_SIGNED_VELOCITY_MAP = 0x4,
		TS_NORMAL_MAP = 0x5,
		TS_METALNESS_MAP = 0x6,
		TS_NORMAL_OCCLUSION_GLOSS_MAP = 0x7,
		TS_SIGNED_DISTANCE_FIELD = 0x8,
		TS_CARD_IMPOSTER_NORMAL = 0x9,
		TS_SKY = 0xA,
	};

	union GfxImageSemanticSpecific
	{
		float atlasFps;
		unsigned int albedoMapScaleBias;
		unsigned int normalMapScaleBias;
		unsigned int maxMipMap;
	};

	struct GfxImageAtlasSize
	{
		unsigned char rowCount;
		unsigned char colCount;
	};

	union GfxImageAtlasInfo
	{
		GfxImageAtlasSize atlasSize;
		unsigned short packedAtlasDataSize;
	};

	struct GfxImageStreamData
	{
		unsigned __int64 xpakKey;
		char unknown[0x8];
		unsigned int levelCount : 4;
		unsigned int pixelSize : 27;
		unsigned int unknownFlag : 1;
		unsigned short width;
		unsigned short height;
	}; assert_sizeof(GfxImageStreamData, 0x18);

	struct GfxImageFallbackData
	{
		unsigned short width;
		unsigned short height;
		unsigned short imageCount;
		unsigned short levelCount;
		unsigned int unknown;
		char unknown2[0x4];
		char* pixels;
		unsigned int size;
		char unknown3[0x14];
	}; assert_sizeof(GfxImageFallbackData, 0x30);

	struct GfxImageFallback
	{
		GfxImageFallbackData* data;
		unsigned __int64 imageIndex;
	}; assert_sizeof(GfxImageFallback, 0x10);

	union GfxImagePixels
	{
		streamer_handle_t streamedDataHandle;
		char* residentData;
	};

	struct GfxImage
	{
		const char* name;
		char* packedAtlasData;
		unsigned int unknown;
		GfxImageFlags flags;
		unsigned int totalSize;
		GfxImageSemanticSpecific semanticSpecific;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		GfxImageAtlasInfo atlasInfo;
		GfxPixelFormat format;
		unsigned char semantic : 4;
		unsigned char category : 4;
		unsigned char levelCount;
		unsigned char unknown3;
		unsigned char streamCount;
		unsigned char unknown4;
		char unknown5[0x8];
		GfxImageStreamData* streams;
		GfxImageFallback* fallback;
		GfxImagePixels pixels;
	}; assert_sizeof(GfxImage, 0x50);
	assert_offsetof(GfxImage, flags, 0x14);
	assert_offsetof(GfxImage, atlasInfo, 0x28);
	assert_offsetof(GfxImage, format, 0x2A);
	assert_offsetof(GfxImage, streamCount, 0x2E);
	assert_offsetof(GfxImage, streams, 0x38);

	enum ComPrimaryLightSpawnFlags : unsigned char
	{
		PRIMARY_LIGHT_SPAWN_NO_SHADOW = 0x8,
		PRIMARY_LIGHT_SPAWN_STATIC_SHADOW = 0x20,
	};

	enum GfxLightType : std::uint8_t
	{
		GFX_LIGHT_TYPE_NONE = 0x0,
		GFX_LIGHT_TYPE_DIR = 0x1,
		GFX_LIGHT_TYPE_SPOT = 0x2,
		GFX_LIGHT_TYPE_OMNI = 0x3,
	};

	struct ComPrimaryLight
	{
		unsigned char spawnFlags;
		unsigned char type;
		unsigned char canUseShadowMap;
		unsigned char needsDynamicShadows;
		unsigned char isVolumetric;
		unsigned int transientZoneList;
		unsigned int entityId;
		float intensity;
		float uvIntensity;
		float irIntensity;
		float heatIntensity;
		vec3_t colorLinearSrgb;
		vec3_t dir;
		vec3_t up;
		vec3_t origin;
		float radius;
		vec2_t fadeOffset;
		float bulbRadius;
		vec3_t bulbLength;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float shadowNearPlaneBias;
		float shadowSoftness;
		float shadowBias;
		float shadowArea;
		float distanceFalloff;
		float cosHalfFovCollimation;
		float rotationLimit;
		float translationLimit;
		const char* defName;
	}; assert_sizeof(ComPrimaryLight, 0xA0);

	struct ComChangeListInfo
	{
		unsigned int changeListNumber;
		int time;
		const char* userName;
	}; assert_sizeof(ComChangeListInfo, 0x10);

	struct ComWorld
	{
		const char* name;
		int isInUse;
		int useForwardPlus;
		unsigned int bakeQuality;
		int proxyLODQuality;
		float trVisRadii[5];
		float trVisFacingDistAdd;
		unsigned int primaryLightCount;
		ComPrimaryLight* primaryLights;
		unsigned int scriptablePrimaryLightCount;
		unsigned int firstScriptablePrimaryLight;
		unsigned int transientTableSize;
		unsigned short* transientTable;
		ComChangeListInfo changeListInfo;
		const char* unknownString;
		unsigned int numUmbraGates;
		const char** umbraGateNames;
		unsigned int umbraGateInitialStates[12];
	}; assert_sizeof(ComWorld, 0xB0);
	assert_offsetof(ComWorld, umbraGateNames, 0x78);

	struct SpatialPartition_Tree
	{
		char* tree;
		unsigned int* alwaysList;
		unsigned int treeSize;
		unsigned int alwaysListLength;
	}; assert_sizeof(SpatialPartition_Tree, 0x18);

	struct SpatialPartition_TreeNodeHeader
	{
		unsigned int unused : 27;
		unsigned int count : 4;
		unsigned int isLeaf : 1;
	}; assert_sizeof(SpatialPartition_TreeNodeHeader, 0x4);

	struct G_GlassPiece
	{
		unsigned short damageTaken;
		unsigned short collapseTime;
		int lastStateChangeTime;
		char impactDir;
		char impactPos[2];
	}; assert_sizeof(G_GlassPiece, 0xC);

	struct G_GlassName
	{
		char* nameStr;
		scr_string_t name;
		unsigned short pieceCount;
		unsigned short* pieceIndices;
	}; assert_sizeof(G_GlassName, 0x18);

	struct G_GlassData
	{
		G_GlassPiece* glassPieces;
		unsigned int pieceCount;
		unsigned short damageToWeaken;
		unsigned short damageToDestroy;
		unsigned int glassNameCount;
		G_GlassName* glassNames;
		SpatialPartition_Tree* spatialTree;
	}; assert_sizeof(G_GlassData, 0x28);

	struct GlassWorld
	{
		const char* name;
		G_GlassData* g_glassData;
	}; assert_sizeof(GlassWorld, 0x10);

	struct pathlink_s
	{
		float fDist;
		unsigned short nodeNum;
		char disconnectCount;
		char negotiationLink;
		char flags;
		char ubBadPlaceCount[3];
	}; assert_sizeof(pathlink_s, 0xC);

	union PathNodeParentUnion
	{
		scr_string_t name;
		unsigned short index;
	};

	struct tacpoint_ref_t
	{
		unsigned short m_PointID;
		char m_GraphIDPlusOne;
	};

	struct pathnode_constant_t
	{
		unsigned int spawnflags;
		scr_string_t targetname;
		scr_string_t script_linkName;
		scr_string_t script_noteworthy;
		scr_string_t target;
		scr_string_t animscript;
		int animscriptfunc;
		scr_string_t interactable;
		vec3_t vLocalOrigin;
		vec3_t angles;
		PathNodeParentUnion parent;
		float minUseDistSq;
		unsigned short wOverlapNode[2];
		tacpoint_ref_t tacpointRef;
		pathlink_s* Links;
		unsigned short totalLinkCount;
		unsigned short type;
	}; assert_sizeof(pathnode_constant_t, 0x58);
	assert_offsetof(pathnode_constant_t, Links, 0x48);

	struct pathnode_t
	{
		pathnode_constant_t constant;
		char __pad0[0x68];
	}; assert_sizeof(pathnode_t, 0xC0);

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
	}; assert_sizeof(pathnode_tree_t, 0x18);

	struct PathDynamicNodeGroup
	{
		unsigned short parentIndex;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
	}; assert_sizeof(PathDynamicNodeGroup, 0x10);

	struct PathData
	{
		const char* name;
		unsigned int nodeCount;
		pathnode_t* nodes;
		bool parentIndexResolved;
		unsigned short version;
		int visBytes;
		char* pathVis;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
		int dynamicNodeGroupCount;
		PathDynamicNodeGroup* dynamicNodeGroups;
		bool usePathExtraData;
		int exposureBytes;
		char* pathExposure;
		int noPeekVisBytes;
		char* pathNoPeekVis;
		int zoneCount;
		int zonesBytes;
		char* pathZones;
		int fixedNodeCount;
		int maxDynamicSpawnedNodeCount;
		int dynStatesBytes;
		char* pathDynStates;
		int visDistDataSize;
	}; assert_sizeof(PathData, 0x98);

	struct nav_space_s;

	struct nav_boundary_plane_s
	{
		vec3_t m_Normal;
		float m_DistFromCenter;
	}; assert_sizeof(nav_boundary_plane_s, 0x10);

	struct nav_resource_s
	{
		scr_string_t targetName;
		int modelIdx;
		int targetEntNum;
		nav_space_s* pSpace;
		int graphSize;
		vec3_t localOffsetPos;
		vec4_t localOffsetRot;
		int offsetLayer;
		nav_resource_s* pPrev;
		nav_resource_s* pNext;
		int buildError;
		unsigned int layerFlags;
		bool bDockable;
		bool bIsVolume;
		char* pGraphBuffer;
		char* pWorkingGraph;
	}; assert_sizeof(nav_resource_s, 0x70);
	assert_offsetof(nav_resource_s, bDockable, 0x58);

	struct nav_obstacle_hull_s
	{
		nav_boundary_plane_s* m_Boundaries;
		int m_NumBoundaries;
		float m_BoundsX;
		float m_BoundsY;
		float m_BoundsZ;
		vec3_t m_Offset;
		bool m_bUseBounds;
	}; assert_sizeof(nav_obstacle_hull_s, 0x28);

	struct nav_obstacle_bounds_s
	{
		nav_obstacle_hull_s* m_Hulls;
		int m_NumHulls;
	}; assert_sizeof(nav_obstacle_bounds_s, 0x10);

	struct nav_glass_bounds_s
	{
		nav_boundary_plane_s* m_Boundaries;
		int m_NumBoundaries;
		vec3_t m_Pos;
		unsigned int m_GlassPieceIndex;
	}; assert_sizeof(nav_glass_bounds_s, 0x20);

	struct nav_modifier_s
	{
		nav_boundary_plane_s* m_Boundaries;
		int m_NumBoundaries;
		vec3_t m_Pos;
		scr_string_t m_TargetName;
		scr_string_t m_Target;
		scr_string_t m_ScriptNoteworthy;
		scr_string_t m_ScriptLinkName;
		unsigned int m_UserFlags;
		unsigned int m_LayerFlags;
		float m_Weight;
		bool m_bActiveOnLoad;
	}; assert_sizeof(nav_modifier_s, 0x38);

	struct nav_link_point_s
	{
		vec3_t m_Pt1;
		vec3_t m_Pt2;
	};

	struct nav_link_creation_data_s
	{
		nav_link_point_s m_Start;
		nav_link_point_s m_End;
		scr_string_t m_Animscript;
		scr_string_t m_Target;
		scr_string_t m_Parent;
		unsigned int m_UsageFlags;
		float m_PenaltyMult;
		bool m_bBidirectional;
	}; assert_sizeof(nav_link_creation_data_s, 0x48);

	struct nav_raw_volume_s
	{
		vec3_t m_MidPoint;
		vec3_t m_HalfSize;
		float m_MinVoxelSize;
		float m_ClearanceXY;
		float m_ClearanceZ;
	}; assert_sizeof(nav_raw_volume_s, 0x24);

	struct nav_raw_custom_volume_tri_s
	{
		vec3_t m_Verts[3];
	}; assert_sizeof(nav_raw_custom_volume_tri_s, 0x24);

	struct nav_raw_custom_volume_s
	{
		nav_raw_custom_volume_tri_s* m_Tris;
		int m_NumTris;
	}; assert_sizeof(nav_raw_custom_volume_s, 0x10);

	struct nav_misc_s
	{
		vec3_t m_StaticBoundsHalfSize;
		vec3_t m_StaticBoundsMidPoint;
	};

	struct NavMeshData
	{
		const char* name;
		int version;
		int hasExposureData;
		int numNavResources;
		nav_resource_s* navResources;
		int numObstacleBounds;
		nav_obstacle_bounds_s* obstacleBounds;
		int numGlassBounds;
		nav_glass_bounds_s* glassBounds;
		int numModifiers;
		nav_modifier_s* modifiers;
		int numLinkCreationData;
		nav_link_creation_data_s* linkCreationData;
		int numVolumeSeeds;
		vec3_t* volumeSeeds;
		int numRawVolumes;
		nav_raw_volume_s* rawVolumes;
		int numRawCustomVolumes;
		nav_raw_custom_volume_s* rawCustomVolumes;
		nav_misc_s miscData;
	}; assert_sizeof(NavMeshData, 0xA8);

	enum bfx_resource_section_type : std::uint32_t
	{
		BFX_SECTION_GRAPH_2D = 0x10000,
		BFX_SECTION_GRAPH_VOLUME = 0x20000,
	};

	enum bfx_kd_tree_node_flag : std::uint32_t
	{
		BFX_KD_TREE_LEAF = 0x80000000,
	};

	struct bfx_resource_header
	{
		std::uint32_t endian;
		std::uint32_t version;
		std::uint32_t body_size;
		std::uint32_t checksum;
		std::uint32_t used;
		std::uint32_t flags;
	}; assert_sizeof(bfx_resource_header, 0x18);

	struct bfx_section_header
	{
		std::uint32_t type;
		std::uint32_t size;
		std::uint32_t count;
	}; assert_sizeof(bfx_section_header, 0xC);

	struct bfx_graph_header
	{
		std::uint32_t endian;
		std::uint32_t version;
		std::uint32_t layer_count;
	}; assert_sizeof(bfx_graph_header, 0xC);

	struct bfx_box
	{
		float mins[3];
		float maxs[3];
	}; assert_sizeof(bfx_box, 0x18);

	struct bfx_layer_header
	{
		std::uint32_t version;
		std::uint32_t index;
		std::uint32_t area_bytes;
		std::uint32_t kd_tree_bytes;
		std::uint32_t record_bytes;
		std::uint32_t unknown_14;
		std::uint32_t link_stride;
		std::uint32_t link_count;
		std::uint32_t layer_bytes;
		std::uint32_t unknown_24;
		float agent_params[5];
		bfx_box bounds;
		std::uint32_t unknown_54;
		char unknown_58[0xFC];
	}; assert_sizeof(bfx_layer_header, 0x154);

	struct bfx_area
	{
		char runtime[0x20];
		float center[3];
		float radius;
		std::uint32_t unknown_30;
		std::uint32_t unknown_34;
		std::uint32_t edge_info; // bits 0-6 edge count
		std::uint32_t unknown_3c;
		std::uint32_t unknown_40;
		std::uint32_t unknown_44;
	}; assert_sizeof(bfx_area, 0x48);

	struct bfx_area_edge
	{
		std::uint64_t neighbor; // area offset (0 = none)
		float vertex[3];
		std::uint32_t flags; // bits 18-31 index (0x3FFF = none)
		std::uint32_t portal;
		std::uint32_t unknown_1c;
	}; assert_sizeof(bfx_area_edge, 0x20);

	struct bfx_kd_tree_header
	{
		bfx_box bounds;
		std::uint32_t node_bytes;
	}; assert_sizeof(bfx_kd_tree_header, 0x1C);

	struct bfx_kd_tree_split
	{
		std::uint32_t info; // bits 28-29 axis, bits 0-27 right child offset
		float left_max;
		float right_min;
	}; assert_sizeof(bfx_kd_tree_split, 0xC);

	namespace bfx
	{
		struct AreaHandle
		{
			void* m_handleImpl;
			void* m_pSpace;
		};
	}

	struct tacpoint_t
	{
		vec3_t m_Pos;
		short m_VerticalClearance;
		char m_GraphIdx;
		char m_Type;
		bfx::AreaHandle m_hArea;
		unsigned short m_SearchNext;
		unsigned short m_SearchPrev;
		int m_Padding;
	}; assert_sizeof(tacpoint_t, 0x28);

	struct TacVisGraphRow
	{
		unsigned short m_Start;
		unsigned short m_End;
		char* m_Vis;
	}; assert_sizeof(TacVisGraphRow, 0x10);

	struct tacpoint_search_node_t
	{
		vec2_t m_MidPoint;
		vec2_t m_HalfSize;
		unsigned short m_PointListRoot;
		tacpoint_search_node_t** m_ChildNodes;
	}; assert_sizeof(tacpoint_search_node_t, 0x20);

	struct TacticalGraphIndexList
	{
		unsigned short* indices;
		unsigned short count;
	}; assert_sizeof(TacticalGraphIndexList, 0x10);

	struct TacticalGraph
	{
		tacpoint_t* m_Points;
		unsigned short m_NumPoints;
		unsigned short m_NumVisRows;
		int m_NumVisBytes;
		TacVisGraphRow* m_VisGraph;
		tacpoint_search_node_t m_SearchRoot;
		TacticalGraphIndexList* indexList;
		scr_string_t name;
	}; assert_sizeof(TacticalGraph, 0x48);
	assert_offsetof(TacticalGraph, m_SearchRoot, 0x18);

	struct TacticalGraphData
	{
		const char* name;
		TacticalGraph* m_TacGraphs;
		int m_NumTacGraphs;
	}; assert_sizeof(TacticalGraphData, 0x18);

	struct AIRegionGraphNode;

	struct AIRegionGraphLink
	{
		AIRegionGraphNode* node;
		char unknown[0x10];
	}; assert_sizeof(AIRegionGraphLink, 0x18);

	struct AIRegionGraphNode
	{
		vec3_t* point;
		AIRegionGraphLink* links;
		int linkCount;
		char unknown[0x114];
	}; assert_sizeof(AIRegionGraphNode, 0x128);

	struct AIRegionGraph
	{
		scr_string_t name;
		AIRegionGraphNode** nodes;
		int nodeCount;
	}; assert_sizeof(AIRegionGraph, 0x18);

	struct AIRegionGraphs
	{
		const char* name;
		vec3_t** points;
		int pointCount;
		AIRegionGraph* graphs;
		int graphCount;
	}; assert_sizeof(AIRegionGraphs, 0x28);

	struct GfxWrappedBaseBuffer
	{
		char __pad0[0x30];
	}; assert_sizeof(GfxWrappedBaseBuffer, 0x30);

	struct GfxWrappedBuffer
	{
		char __pad0[0x40];
	};

	struct GfxImageDesc
	{
		GfxPixelFormat format;
		GfxImageFlags flags;
		unsigned int totalSize;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		unsigned char levelCount;
	}; assert_sizeof(GfxImageDesc, 0x18);

	struct GfxImageRaw
	{
		GfxImageDesc desc;
		GfxTextureId textureId;
		char* pixels;
	}; assert_sizeof(GfxImageRaw, 0x28);

	struct GfxWorldDrawVertsUnknown
	{
		char unknown[0x1C];
	}; assert_sizeof(GfxWorldDrawVertsUnknown, 0x1C);

	struct GfxWorldDrawVerts
	{
		unsigned int posDataSize;
		unsigned int indexCount;
		unsigned int unknownCount;
		char* posData;
		unsigned short* indices;
		GfxWorldDrawVertsUnknown* unknown;
	}; assert_sizeof(GfxWorldDrawVerts, 0x28);
	assert_offsetof(GfxWorldDrawVerts, posData, 0x10);

	struct GfxAabbTree
	{
		Bounds bounds;
		unsigned int surfaceCount;
		unsigned int startSurfIndex;
		int childrenOffset;
		unsigned short childCount;
		unsigned short smodelIndexCount;
		unsigned short* smodelIndexes;
	}; assert_sizeof(GfxAabbTree, 0x30);

	struct GfxCellTreeCount
	{
		int aabbTreeCount;
	};

	struct GfxCellTree
	{
		GfxAabbTree* aabbTree;
	};

	struct GfxWorldDrawCells
	{
		unsigned int cellCount;
		GfxCellTreeCount* aabbTreeCounts;
		GfxCellTree* aabbTrees;
	}; assert_sizeof(GfxWorldDrawCells, 0x18);

	enum GfxLightGridType : std::uint8_t
	{
		GFX_LIGHT_GRID_TYPE_NONE = 0x0,
		GFX_LIGHT_GRID_TYPE_GPU = 0x1,
	};

	enum GfxLightmapType : std::uint32_t
	{
		GFX_LIGHTMAP_TYPE_REGULAR = 0x0,
		GFX_LIGHTMAP_TYPE_REGULAR_UNCOMPRESSED = 0x1,
		GFX_LIGHTMAP_TYPE_AB_LIGHTING = 0x2,
		GFX_LIGHTMAP_TYPE_COUNT = 0x3,
	};

	struct GfxLightmapRaw
	{
		GfxImageRaw textures[3];
	};

	struct GfxLightmapAtlasDataTemp
	{
		unsigned int transientZone;
		unsigned int numLightmaps;
		unsigned int firstLightmapIndex;
		GfxLightmapType lightmapType;
		GfxLightmapRaw lightmaps[32];
	}; assert_sizeof(GfxLightmapAtlasDataTemp, 0xF10);

	struct GfxSHCompressedProbeData
	{
		unsigned int l0; // R11G11B10F
		unsigned char shRatios[3][8];
		unsigned char skyVisibility;
		unsigned char gpuVisibility;
		unsigned char pad[2];
	}; assert_sizeof(GfxSHCompressedProbeData, 0x20);

	struct GfxSHProbeData
	{
		unsigned short coeffs[29];
		unsigned short pad[3];
	};

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
	}; assert_sizeof(GfxGpuLightGridZone, 0x5C);

	struct GfxVoxelTreeHeader
	{
		int rootNodeDimension[4];
		int nodeCoordBitShift[4];
		vec4_t boundMin;
		vec4_t boundMax;
	}; assert_sizeof(GfxVoxelTreeHeader, 0x40);

	struct GfxVoxelTopDownViewNode
	{
		int firstNodeIndex;
		int zMin;
		int zMax;
	};

	struct GfxVoxelInternalNode
	{
		int firstNodeIndex[2];
		int childNodeMask[2];
	};

	struct GfxGpuLightGridVoxelTree
	{
		int topDownViewNodeCount;
		int internalNodeCount;
		char unknown[0x18];
		GfxVoxelTreeHeader* treeHeader;
		GfxVoxelTopDownViewNode* topDownViewNodes;
		GfxVoxelInternalNode* internalNodes;
		GfxWrappedBuffer treeHeaderBuffer;
		GfxWrappedBuffer topDownViewNodeBuffer;
		GfxWrappedBuffer internalNodeBuffer;
	}; assert_sizeof(GfxGpuLightGridVoxelTree, 0xF8);

	struct GfxGpuLightGridTetrahedron
	{
		unsigned int indexFlags[4];
	};

	struct GfxGpuLightGridTetrahedronNeighbors
	{
		unsigned int neighbors[4];
	};

	struct GfxGpuLightGridTetrahedronVisibility
	{
		unsigned int visibility[16];
	};

	struct GfxGpuLightGridVoxelStartTetrahedron
	{
		unsigned int index;
	};

	struct GfxGpuLightGrid
	{
		unsigned int probeCount;
		bool fallbackProbeUseCustomSample;
		vec3_t fallbackProbeCustomSamplePos;
		unsigned int zoneCount;
		GfxGpuLightGridZone* zones;
		GfxGpuLightGridVoxelTree* zoneVoxelTrees;
		unsigned int tetrahedronCount;
		unsigned int tetrahedronCountVisible;
		unsigned int voxelStartTetrahedronCount;
		GfxSHCompressedProbeData* probes;
		vec3_t* probePositions;
		GfxGpuLightGridTetrahedron* tetrahedrons;
		GfxGpuLightGridTetrahedronNeighbors* tetrahedronNeighbors;
		GfxGpuLightGridTetrahedronVisibility* tetrahedronVisibility;
		GfxGpuLightGridVoxelStartTetrahedron* voxelStartTetrahedron;
		GfxWrappedBuffer probesBuffer;
		char unknown2[0x28];
		GfxWrappedBuffer probePositionsBuffer;
		GfxWrappedBuffer tetrahedronBuffer;
		GfxWrappedBuffer tetrahedronNeighborsBuffer;
		GfxWrappedBuffer tetrahedronVisibilityBuffer;
		GfxWrappedBuffer voxelStartTetrahedronBuffer;
		vec4_t boundingVolume[7];
	}; assert_sizeof(GfxGpuLightGrid, 0x280);
	assert_offsetof(GfxGpuLightGrid, probes, 0x38);
	assert_offsetof(GfxGpuLightGrid, probePositionsBuffer, 0xD0);

	struct GfxStaticModelLightGridVolume
	{
		unsigned int firstProbe;
		unsigned short m_volumeSize[3];
		unsigned char flags[2];
		unsigned int singleProbe;
		vec4_t volumeTransform[3];
	}; assert_sizeof(GfxStaticModelLightGridVolume, 0x40);

	struct GfxGpuDynLightsetsLGVReference
	{
		unsigned int volumeIndex;
		unsigned int volumeProbeIndex;
	};

	struct GfxDynamicLightsetGChunkInfo
	{
		unsigned int numRecords;
		unsigned int bfStart;
		unsigned int orgStart;
		unsigned int tmpStart;
	};

	struct GfxDynamicLightsetGChunk
	{
		GfxDynamicLightsetGChunkInfo chunkInfo;
		unsigned char flags[2];
		unsigned short whichLM;
		unsigned int bfChunkStart;
	}; assert_sizeof(GfxDynamicLightsetGChunk, 0x18);

	struct GfxDynamicLightsetProbeData
	{
		unsigned int numGChunk;
		GfxDynamicLightsetGChunk* chunkG;
		unsigned int numGChunkBF;
		unsigned short* chunkGBF;
		unsigned int origStart;
		unsigned int numOrigProbes;
		GfxSHCompressedProbeData* origProbes;
		GfxWrappedBuffer origProbesBuffer;
		unsigned int numBFProbes;
		GfxSHCompressedProbeData* bfProbes;
		GfxWrappedBuffer bfProbesBuffer;
		unsigned int numSkipRecords;
		unsigned short* skipRecords;
		GfxWrappedBuffer skipRecordsBuffer;
		unsigned int* palletteRecords;
		GfxWrappedBuffer palletteRecordsBuffer;
		unsigned int* tempRemap;
		GfxWrappedBuffer tempRemapBuffer;
	}; assert_sizeof(GfxDynamicLightsetProbeData, 0x1A0);
	assert_offsetof(GfxDynamicLightsetProbeData, numSkipRecords, 0xC0);

	struct GfxDynamicLightsetLGppZoneInfo
	{
		unsigned int numOrigSMLGppProbes;
		GfxGpuDynLightsetsLGVReference* lgvReferences;
		GfxWrappedBuffer lgvReferencesBuffer;
		GfxDynamicLightsetProbeData lgvProbeData;
	}; assert_sizeof(GfxDynamicLightsetLGppZoneInfo, 0x1F0);

	struct LightGridVolumePackingParams
	{
		vec3_t m_uvScale;
		unsigned char m_volumeSize[3];
		unsigned char m_basisType;
		vec3_t m_uvBias;
		unsigned int m_firstStaticProbeIndex;
	}; assert_sizeof(LightGridVolumePackingParams, 0x20);

	struct LightGridVolumeTransformParams
	{
		vec4_t m_volumeTransform[3];
		unsigned int m_historyEntryOrStaticFlags;
		unsigned int m_lastFrameHistoryEntryAndSetup;
		unsigned int m_lodAndSamplingOffsetZ;
		unsigned int m_samplingOffsetXY;
	}; assert_sizeof(LightGridVolumeTransformParams, 0x40);

	struct GfxLightGridVolumeAtlasDataTemp
	{
		unsigned int m_widthAndHeight;
		unsigned int m_depth;
		unsigned int m_numVolumes;
		unsigned short m_worldTransient;
		GfxImageRaw m_textures[2];
		LightGridVolumePackingParams* m_packingParams;
		GfxWrappedBuffer m_packingParamsBuffer;
		LightGridVolumeTransformParams* m_transformParams;
		GfxWrappedBuffer m_transformParamsBuffer;
	}; assert_sizeof(GfxLightGridVolumeAtlasDataTemp, 0xF0);

	struct GfxGpuLightGridTransientData
	{
		GfxGpuLightGrid* gpuLightGrid;
		unsigned int staticModelVolumeCount;
		GfxStaticModelLightGridVolume* staticModelVolumes;
		GfxDynamicLightsetLGppZoneInfo* SMLGppZone;
		GfxLightGridVolumeAtlasDataTemp* tempLightGridVolumeData;
	}; assert_sizeof(GfxGpuLightGridTransientData, 0x28);

	struct GfxImageReflectionProbe
	{
		unsigned int totalSize;
		unsigned int elementCount;
		GfxTextureId textureId;
		char* pixels;
	}; assert_sizeof(GfxImageReflectionProbe, 0x18);

	struct GfxReflectionProbeTransientData
	{
		unsigned short reflectionProbeInstanceCount;
		unsigned short exclusiveProbeBucketCount;
		unsigned int exclusiveProbeCount;
		unsigned short* reflectionProbeInstances;
		GfxImageReflectionProbe* exclusiveProbeBuckets;
	}; assert_sizeof(GfxReflectionProbeTransientData, 0x18);

	struct GfxStaticDecalVolumePacked
	{
		vec3_t obbCenter;
		unsigned int obbQuat;
		unsigned int obbHalfSize;
		unsigned int opacityParams;
		unsigned int uvMatrixAndOffset[3];
		unsigned int packedMaterialInfo;
		unsigned int reserved;
	}; assert_sizeof(GfxStaticDecalVolumePacked, 0x2C);

	struct GfxStaticDecalVolumeUid
	{
		unsigned int uid;
	};

	struct GfxDecalVolumeTransientData
	{
		unsigned int decalVolumeCount;
		unsigned short firstCollection;
		unsigned short collectionCount;
		GfxStaticDecalVolumePacked* decalVolumes;
		GfxWrappedBuffer decalVolumesBuffer;
		Bounds* collectionBounds;
		GfxStaticDecalVolumeUid* decalVolumeUids;
	}; assert_sizeof(GfxDecalVolumeTransientData, 0x60);

	struct GfxCompressedSunShadowDataTemp
	{
		unsigned short zoneIndex;
		unsigned short tilesCount;
		unsigned int* tilesIndices;
		unsigned int* tilesOffsets;
		unsigned int tilesDataSize;
		char* tilesData;
		GfxWrappedBuffer tilesDataBuffer;
	}; assert_sizeof(GfxCompressedSunShadowDataTemp, 0x68);

	struct GfxWorldTransientZone
	{
		const char* name;
		std::uint64_t* runtimeData;
		char unknown[0x8];
		GfxWorldDrawVerts drawVerts;
		GfxLightmapAtlasDataTemp* tempLightmapData;
		GfxGpuLightGridTransientData gpuLightGrid;
		GfxReflectionProbeTransientData* reflectionProbes;
		GfxDecalVolumeTransientData* decalVolumes;
		GfxCompressedSunShadowDataTemp* compressedSunShadow;
	}; assert_sizeof(GfxWorldTransientZone, 0x88);
	assert_offsetof(GfxWorldTransientZone, reflectionProbes, 0x70);

	struct GfxLightDef
	{
		const char* name;
		const char* iesProfile;
		unsigned int firstPixel;
		float coordOffset;
		float coordScale;
	}; assert_sizeof(GfxLightDef, 0x20);

	enum GfxHdrGradingClutInputColorimetry : std::uint8_t
	{
		HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT2020_LOG2 = 0x0,
		HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT2020_PQ = 0x1,
		HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT709_PQ = 0x2,
		HDR_GRADING_CLUT_INPUT_COLORIMETRY_UNKNOWN = 0xFF,
	};

	enum GfxHdrGradingClutOutputColorimetry : std::uint8_t
	{
		HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_BT2020_LINEAR = 0x0,
		HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_BT2020_PQ = 0x1,
		HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_UNKNOWN = 0xFF,
	};

	struct GfxGradingClut
	{
		const char* name;
		GfxHdrGradingClutInputColorimetry inputColorimetry;
		GfxHdrGradingClutOutputColorimetry outputColorimetry;
		unsigned int clutSize;
		GfxImage* hdrClutImage;
	}; assert_sizeof(GfxGradingClut, 0x18);

	struct GfxFogSplineData
	{
		float fogStartDist;
		float fogHalfPlaneDistance;
		float fogMaxDistance;
	};

	struct GfxFogSpline
	{
		const char* name;
		GfxImage* image;
		GfxFogSplineData data;
	}; assert_sizeof(GfxFogSpline, 0x20);

	struct StringTable
	{
		const char* name;
		int columnCount;
		int rowCount;
		int uniqueCellCount;
		unsigned short* cellIndices;
		int* hashes;
		const char** strings;
	}; assert_sizeof(StringTable, 0x30);

	enum CameraProfile : std::int32_t
	{
		CAMERAPROFILE_FIRST_PERSON_VIEW = 0x0,
		CAMERAPROFILE_ORBIT = 0x1,
		CAMERAPROFILE_SENTRY_DRONE = 0x2,
		CAMERAPROFILE_KILLCAM_VEHICLE = 0x3,
		CAMERAPROFILE_FIRST_PERSON_VEHICLE = 0x4,
		CAMERAPROFILE_MAX = 0x5,
	};

	enum LensProfileMode : std::int32_t
	{
		LENS_PROFILE_NONE = 0x0,
		LENS_PROFILE_COMPACT_PORTABLE = 0x1,
		LENS_PROFILE_SMARTPHONE = 0x2,
		LENS_PROFILE_CINEMATIC_PRIME = 0x3,
		LENS_PROFILE_ACTIONCAM_1 = 0x4,
		LENS_PROFILE_ACTIONCAM_2 = 0x5,
		LENS_PROFILE_DRONECAM = 0x6,
		LENS_PROFILE_TELESCOPIC = 0x7,
		LENS_PROFILE_COUNT = 0x8,
	};

	enum CameraTransitionCurve : std::int32_t
	{
		CAMTCURVE_NONE = 0x0,
		CAMTCURVE_LINEAR = 0x1,
		CAMTCURVE_SMOOTHSTEP = 0x2,
		CAMTCURVE_SMOOTHERSTEP = 0x3,
	};

	struct CameraFirstPersonProfileDef
	{
		bool applyOffset;
		vec3_t offsetTranslation;
		bool disableFOVWeaponOffset;
		float cameraAnimationTransScale;
		float cameraAnimationRotScale;
		float handheldCameraTransScale;
		float handheldCameraRotScale;
		float handheldCameraImpulseTransScale;
		float handheldCameraImpulseRotScale;
	}; assert_sizeof(CameraFirstPersonProfileDef, 0x2C);

	struct CameraOrbitProfileDef
	{
		float lerpSpeed;
		vec3_t pivotOffset;
		float initialAzimuth;
		float initialPolar;
		float polarMin;
		float polarMax;
		float range;
		float returnAzimuthSpeed;
		float returnPolarSpeed;
		float easeInFactor;
		float returnTimeTo;
		float turningSpeedPolar;
		float turningSpeedAzimuth;
		float traceCollRadius;
		float mouseInputMultiplier;
		float rollContrib;
		float pitchContrib;
		int buttonToggleReturn;
		int buttonCyclePerspective;
		int buttonRecenter;
		bool initialReturn;
		bool returnAzimuth;
		bool returnPolar;
		float alignToMovement;
		bool interpolateFocus;
		bool cameraReturnOnGas;
		float cameraReturnOnSpeed;
		float yawTurnAdjust;
	}; assert_sizeof(CameraOrbitProfileDef, 0x6C);

	struct CameraSentryDroneDef
	{
		float range;
		float thresHitToSpeedUp;
		float speedUpFactor;
		float lerpSpeedEye;
		float lerpSpeedFocus;
		float lerpSpeedUp;
		float focusTraceLen;
		float eyeHeight;
		float horizontalOffset;
		float upOffsetFactorIncl;
	};

	struct CameraKillCamVehicleDef
	{
		float lookAtRadius;
		float fov;
		float distBack;
		float distUp;
		float distBackRadiusMod;
		float distUpRadiusMod;
		float fovRadiusMod;
		float sideMag;
		float traceCollRad;
	};

	struct CameraTransitionDef
	{
		bool active;
		float time;
		float timewaitEyepos;
		float timewaitAxis;
		CameraTransitionCurve curveEyepos;
		CameraTransitionCurve curveAxis;
	}; assert_sizeof(CameraTransitionDef, 0x18);

	struct CameraDef
	{
		const char* name;
		CameraProfile profile;
		bool keepShakeCam;
		bool serverFeedback;
		bool behaveAsRemoteCont;
		bool interpolateViewTransition;
		bool overrideFOV;
		float FOV;
		LensProfileMode overrideLensProfile;
		float lensFocalLength;
		float lensFStop;
		float lensScale;
		float lensUVScale;
		bool overrideRadialBlur;
		float radialBlurRadius;
		float radialBlurStrength;
		bool overrideZPlanes;
		float zPlaneVMNear;
		float zPlaneSceneNear;
		CameraFirstPersonProfileDef firstPerson;
		CameraOrbitProfileDef orbit;
		CameraSentryDroneDef sentryDrone;
		CameraKillCamVehicleDef kcamVeh;
		CameraTransitionDef transitionIn;
		CameraTransitionDef transitionOut;
		vec3_t fpveh_offsetLs;
		float fpveh_offsetYaw;
		float fpveh_offsetPitch;
		float fpveh_rollContrib;
		float fpveh_rollMax;
		bool fpveh_freeLook;
		bool fpveh_freeLookReturn;
		bool dlcBool[1];
		bool fadeOutPlayerOnTransitionIn;
	}; assert_sizeof(CameraDef, 0x178);

	struct HudOutlineDef
	{
		const char* name;
		char unknown[0x68];
	}; assert_sizeof(HudOutlineDef, 0x70);

	enum XAnimUpVectorType : std::int32_t
	{
		XANIM_UP_VECTOR_TYPE_WORLD_SPACE = 0x0,
		XANIM_UP_VECTOR_TYPE_BONE_RELATIVE = 0x1,
		XANIM_UP_VECTOR_TYPE_AIM_AT_BONE = 0x2,
	};

	struct XAnimAimConstraint
	{
		vec3_t aimTargetLocalOffset;
		vec3_t localAimVector;
		vec3_t localUpVector;
		vec3_t axisMask;
		vec3_t upVector;
		XAnimUpVectorType upVectorType;
	};

	struct XAnimRotationConstraint
	{
		vec4_t rotationOffsetQuat;
		vec3_t axisMask;
		float blendFraction;
	};

	struct XAnimPositionConstraint
	{
		float blendFraction;
		vec3_t minLocalTranslationLimit;
		vec3_t maxLocalTranslationLimit;
		vec3_t offset;
		vec3_t sourceBoneOffsets[2];
		vec3_t axisMask;
	};

	struct XAnimDistanceDrivenOffsetConstraint
	{
		vec4_t minLocalOffset;
		vec4_t maxLocalOffset;
		vec3_t sourceBoneOffsets[2];
		float distanceScale;
		float distanceBias;
		unsigned char curveType;
	};

	union XAnimConstraintUnion
	{
		XAnimPositionConstraint positionConstraint;
		XAnimRotationConstraint rotationConstraint;
		XAnimAimConstraint aimConstraint;
		XAnimDistanceDrivenOffsetConstraint distanceDrivenOffsetConstraint;
	};

	struct XAnimConstraint
	{
		unsigned char type;
		char localTargetBoneIndex;
		scr_string_t sourceBoneNames[2];
		XAnimConstraintUnion u;
	}; assert_sizeof(XAnimConstraint, 0x58);

	struct XAnimProceduralBonesUnknownRecord
	{
		char unknown[0x68];
	}; assert_sizeof(XAnimProceduralBonesUnknownRecord, 0x68);

	struct XAnimProceduralBones
	{
		const char* name;
		XAnimConstraint* constraints;
		XAnimProceduralBonesUnknownRecord* unknownRecords;
		unsigned short numConstraints;
		unsigned short numUnknownBoneNames;
		unsigned short numTargetBones;
		unsigned char numUnknownRecords;
		unsigned char unknown;
		scr_string_t* targetBoneNames;
		scr_string_t* unknownBoneNames;
	}; assert_sizeof(XAnimProceduralBones, 0x30);

	struct WeaponAnimPackageStateTimers
	{
		char __pad0[0x222];
	}; assert_sizeof(WeaponAnimPackageStateTimers, 0x222);

	struct FootstepTime
	{
		float time;
		bool isLeft;
	};

	struct MovementTime
	{
		float time;
		bool isLeadIn;
	};

	struct FootstepAnim
	{
		int leftCount;
		FootstepTime step[32];
		MovementTime movement[32];
	}; assert_sizeof(FootstepAnim, 0x204);

	struct BlendSpace2DAnimEntry
	{
		XAnimParts* anim;
		vec2_t animCoords;
	}; assert_sizeof(BlendSpace2DAnimEntry, 0x10);

	struct BlendSpace2DDef
	{
		const char* name;
		vec2_t rangeX;
		vec2_t rangeY;
		BlendSpace2DAnimEntry* animEntries;
		unsigned int numAnimEntries;
		unsigned int* triangleCoords;
		unsigned int numTriangleCoords;
		int blendTimeMs;
		bool allAnimsNotify;
	}; assert_sizeof(BlendSpace2DDef, 0x40);

	enum weapAnimFiles_t
	{
		WEAP_ANIM_ADDITIVE_ADS_UP = 31,
		WEAP_ANIM_SPRINT_LOOP = 65,
		WEAP_ANIM_ADDITIVE_JUMP = 68,
		WEAP_ANIM_ADDITIVE_JUMP_LAND = 71,
		WEAP_ANIM_ADDITIVE_CRAWL_IN = 108,
		WEAP_ANIM_ADDITIVE_CRAWL_LOOP = 110,
		WEAP_ANIM_ADDITIVE_PRONE_DROP = 116,
		WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT = 118,
		WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT = 120,
		WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_SAFE = 122,
		WEAP_ANIM_ADDITIVE_SETTLE_ADS = 164,
		WEAP_ANIM_ADDITIVE_RECOIL = 176,
		WEAP_ANIM_IDLE = 236,
		WEAP_ANIM_EMPTY_IDLE = 237,
		WEAP_ANIM_FIRE = 239,
		WEAP_ANIM_LASTSHOT = 241,
		WEAP_ANIM_MELEE_MISS = 262,
		WEAP_ANIM_MELEE_HIT = 263,
		WEAP_ANIM_MELEE_FATAL = 264,
		WEAP_ANIM_RELOAD = 286,
		WEAP_ANIM_RELOAD_ADS = 287,
		WEAP_ANIM_RELOAD_EMPTY = 289,
		WEAP_ANIM_RELOAD_EMPTY_ADS = 290,
		WEAP_ANIM_FAST_RELOAD = 304,
		WEAP_ANIM_FAST_RELOAD_ADS = 305,
		WEAP_ANIM_FAST_RELOAD_EMPTY = 307,
		WEAP_ANIM_FAST_RELOAD_EMPTY_ADS = 308,
		WEAP_ANIM_RAISE = 322,
		WEAP_ANIM_FIRST_RAISE = 323,
		WEAP_ANIM_DROP = 325,
		WEAP_ANIM_QUICK_RAISE = 333,
		WEAP_ANIM_QUICK_DROP = 334,
		WEAP_ANIM_EMPTY_RAISE = 335,
		WEAP_ANIM_EMPTY_DROP = 336,
		WEAP_ANIM_ADS_FIRE = 346,
		WEAP_ANIM_ADS_LASTSHOT = 348,
		WEAP_ANIM_RECOIL_SETTLE = 373,
		WEAP_ANIM_ADS_UP = 405,
		WEAP_ANIM_ADS_DOWN = 407,
		WEAP_ANIM_3P_FINGER_POSE_LEFT = 609,
		WEAP_ANIM_3P_FINGER_POSE_RIGHT = 611,
		NUM_WEAP_ANIMS = 677,
	};

	struct WeaponAnimPackage
	{
		const char* name;
		XAnimParts** anims;
		WeaponAnimPackageStateTimers* timers;
		int meleeAnimPrimaryType;
		int meleeAnimPrimaryCount;
		unsigned char meleeAnimPrimarySet;
		bool meleeRandomizeAnims;
		int meleeAnimAltType;
		int meleeAnimAltCount;
		unsigned char meleeAnimAltSet;
		bool altMeleeRandomizeAnims;
		BlendSpace2DDef** blendSpaces;
		FootstepAnim* footstep;
	}; assert_sizeof(WeaponAnimPackage, 0x40);

	struct XModelDetailCollision
	{
		const char* name;
		unsigned int physicsLODDataSize;
		char* physicsLODData;
		unsigned int physicsLODDataNameCount;
		scr_string_t* physicsLODDataNames;
	}; assert_sizeof(XModelDetailCollision, 0x28);

	enum StreamKeyFlags : std::uint8_t
	{
		STREAM_KEY_RESIDENT = 0x1,
	};

	union StreamKeyData
	{
		streamer_handle_t dataHandle;
		char* residentData;
		char* residentDataGPU;
	};

	union StreamKeyBehavior
	{
		void* behaviorUserPtr;
		unsigned __int64 assetHash;
	};

	struct StreamKey
	{
		const char* name;
		char unknown[0x10];
		StreamKeyData data;
		StreamKeyBehavior behavior;
		unsigned int dataSize;
		unsigned char behaviorIndex;
		unsigned char flags;
	}; assert_sizeof(StreamKey, 0x30);
	assert_offsetof(StreamKey, flags, 0x2D);

	enum ADSRCurve : std::int32_t
	{
		ADSR_CURVE_LINEAR = 0x0,
		ADSR_CURVE_LOG_FAST = 0x1,
		ADSR_CURVE_LOG_SLOW = 0x2,
		ADSR_CURVE_FLAT_ENDED = 0x3,
		ADSR_CURVE_FLAT_MIDDLE = 0x4,
	};

	struct ADSRSetting
	{
		const char* name;
		std::uint64_t id;
		char unknown[0x18];
	}; assert_sizeof(ADSRSetting, 0x28);

	struct AmbientDef
	{
		const char* name;
		std::uint64_t id;
		char unknown[0x18];
	}; assert_sizeof(AmbientDef, 0x28);

	struct AmbientEvent
	{
		std::uint64_t id;
		unsigned short ambientElementIndex;
		float weight;
	}; assert_sizeof(AmbientEvent, 0x10);

	struct AmbientElement
	{
		std::uint64_t id;
		char unknown[0x10];
	}; assert_sizeof(AmbientElement, 0x18);

	enum SND_EQTYPE : std::int32_t
	{
		SND_EQTYPE_FIRST = 0x0,
		SND_EQTYPE_LOWPASS = 0x0,
		SND_EQTYPE_HIGHPASS = 0x1,
		SND_EQTYPE_LOWSHELF = 0x2,
		SND_EQTYPE_HIGHSHELF = 0x3,
		SND_EQTYPE_BELL = 0x4,
		SND_EQTYPE_LAST = 0x4,
		SND_EQTYPE_COUNT = 0x5,
		SND_EQTYPE_INVALID = 0x5,
	};

	struct FilterDef
	{
		std::uint64_t id;
		char entChannelIdx;
		char band;
		SND_EQTYPE type;
		float freq;
		float gain;
		float q;
	}; assert_sizeof(FilterDef, 0x20);

	struct MixDef
	{
		std::uint64_t id;
		unsigned short volModIndex;
		float volume;
		float fade;
	}; assert_sizeof(MixDef, 0x18);

	struct AdsrZoneEntry
	{
		const char* name;
		std::uint64_t id;
		const char* weaponName;
		int adsrIdx;
		unsigned int weaponIdx;
	}; assert_sizeof(AdsrZoneEntry, 0x20);

	struct OccludeDef
	{
		std::uint64_t id;
		char unknown[0x38];
	}; assert_sizeof(OccludeDef, 0x40);

	struct ReverbDef
	{
		std::uint64_t id;
		std::uint64_t roomTypeId;
		char roomType[64];
		float dryLevel;
		float wetLevel;
		float fadeTime;
	}; assert_sizeof(ReverbDef, 0x60);

	struct TimescaleEntry
	{
		std::uint64_t id;
		std::uint64_t presetName;
		int entChannelIdx;
		float scale;
	}; assert_sizeof(TimescaleEntry, 0x18);

	struct ZoneDef
	{
		std::uint64_t id;
		std::uint64_t stateId;
		short reverbIndex;
		short startOcclusionIndex;
		short numOcclusion;
		short startFilterIndex;
		short numFilter;
		short startMixIndex;
		short numMix;
		const char* ambientStream;
		short startAdsrZoneIndex;
		short numAdsrZone;
		short startNPCAdsrZoneIndex;
		short numNPCAdsrZone;
		short ambientDefIndex;
		short startFullOccIndex;
		short numDisableFullOcc;
		short startPlayerBreathStateIndex;
		short numPlayerBreathStates;
		std::uint64_t weapReflId;
		bool exterior;
		const char* zoneName;
		const char* stateName;
		const char* ambientName;
		const char* mixName;
		const char* reverbName;
		const char* filterName;
		const char* occlusionName;
		const char* plrAdsrName;
		const char* npcAdsrName;
		const char* weapReflName;
		const char* fullOccName;
		const char* playerBreathStateName;
		std::uint64_t contextType1;
		std::uint64_t contextValue1;
		std::uint64_t contextType2;
		std::uint64_t contextValue2;
		std::uint64_t contextType3;
		std::uint64_t contextValue3;
		std::uint64_t contextType4;
		std::uint64_t contextValue4;
		std::uint64_t duck;
	}; assert_sizeof(ZoneDef, 0xF8);
	assert_offsetof(ZoneDef, ambientStream, 0x20);
	assert_offsetof(ZoneDef, zoneName, 0x50);

	struct FullOcclusionDef
	{
		std::uint64_t id;
		std::uint64_t presetName;
		int entChannelIdx;
	}; assert_sizeof(FullOcclusionDef, 0x18);

	struct PlayerBreathStateDef
	{
		std::uint64_t stateType;
		const char* inhaleAlias;
		const char* exhaleAlias;
		int minBreaths;
	}; assert_sizeof(PlayerBreathStateDef, 0x20);

	struct SoundTable
	{
		std::uint64_t id;
		ADSRSetting* adsrSettings;
		unsigned int adsrCount;
		AmbientDef* ambientDefs;
		unsigned int ambientDefCount;
		AmbientEvent* ambientEvents;
		unsigned int ambientEventCount;
		AmbientElement* ambientElements;
		unsigned int ambientElementsCount;
		FilterDef* filters;
		unsigned int filterCount;
		MixDef* mixes;
		unsigned int mixCount;
		AdsrZoneEntry* npcADSRZones;
		unsigned int npcADSRZoneCount;
		OccludeDef* occlusionFilters;
		unsigned int occlusionFilterCount;
		AdsrZoneEntry* playerADSRZones;
		unsigned int playerADSRZoneCount;
		ReverbDef* reverbs;
		unsigned int reverbCount;
		TimescaleEntry* timeScaleSettings;
		unsigned int timeScaleSettingCount;
		ZoneDef* zones;
		unsigned int zoneCount;
		FullOcclusionDef* fullOcclusionDefs;
		unsigned int fullOcclusionDefCount;
		PlayerBreathStateDef* plrBreathStateDefs;
		unsigned int plrBreathStateDefCount;
	}; assert_sizeof(SoundTable, 0xE8);

	struct SndAliasLookup
	{
		std::uint64_t hash;
	}; assert_sizeof(SndAliasLookup, 0x8);

	struct SndBankStreamEntry
	{
		char unknown[0x38];
	}; assert_sizeof(SndBankStreamEntry, 0x38);

	struct SndBankStreamInfo
	{
		StreamKey* loadedStreamKey;
		SndBankStreamEntry* entries;
		unsigned int entryCount;
	}; assert_sizeof(SndBankStreamInfo, 0x18);

	struct SndIndexEntry
	{
		unsigned short value;
		unsigned short next;
	}; assert_sizeof(SndIndexEntry, 0x4);

	struct SndDuck
	{
		char name[64];
		std::uint64_t id;
		float fadeIn;
		float fadeOut;
		float startDelay;
		float minDistance;
		float distance;
		float length;
		std::uint64_t fadeInCurve;
		std::uint64_t fadeOutCurve;
		float* curves[6];
		std::uint64_t* focusCone;
		char unknown[0x18];
		std::uint64_t duckAlias;
	}; assert_sizeof(SndDuck, 0xC8);
	assert_offsetof(SndDuck, curves, 0x70);

	enum SndMusicStateStatus : std::int32_t
	{
		SND_MUSIC_STATE_INACTIVE = 0x0,
		SND_MUSIC_STATE_ACTIVE = 0x1,
	};

	enum WeapReflRoomType : std::int32_t
	{
		SND_WR_ROOM_EXTERIOR = 0x0,
		SND_WR_ROOM_INTERIOR = 0x1,
	};

	struct SndMusicAsset
	{
		char alias[64];
		std::uint64_t aliasId;
		bool inactive;
		bool completeLoop;
		bool removeAfterPlay;
		bool firstRandom;
		bool startSync;
		bool stopSync;
		bool completeOnStop;
		char __pad0[0x31];
	}; assert_sizeof(SndMusicAsset, 0x80);

	struct SndMusicState
	{
		char name[64];
		std::uint64_t id;
		SndMusicAsset intro;
		SndMusicAsset exit;
		unsigned short loopCount;
		SndMusicAsset* loops;
		unsigned short* loopValues;
		char unknown[0x20];
	}; assert_sizeof(SndMusicState, 0x180);
	assert_offsetof(SndMusicState, loopCount, 0x148);

	struct SndMusicSet
	{
		char name[64];
		std::uint64_t id;
		unsigned int stateCount;
		SndMusicState* states;
	}; assert_sizeof(SndMusicSet, 0x58);

	struct WeaponReflectionDef
	{
		const char* name;
		std::uint64_t id;
		const char* weaponClass;
		std::uint64_t weaponClassId;
		const char* type;
		WeapReflRoomType zoneType;
		float zoneTypeScalar;
		float closeMedThreshold;
		float medFarThreshold;
		float farThreshold;
		float npcCloseMedThreshold;
		float npcMedFarThreshold;
		float npcFarThreshold;
		float speedOfSound;
		float maxPitchClose;
		float pitchDistClose;
		float maxPitchMed;
		float pitchDistMed;
		float maxPitchFar;
		float pitchDistFar;
		float npcMaxPitchMed;
		float npcPitchDistMed;
		float npcMaxPitchFar;
		float npcPitchDistFar;
		float frontVol;
		float frontEq;
		float sideVol;
		float sideEq;
		float rearVol;
		float rearEq;
	}; assert_sizeof(WeaponReflectionDef, 0x90);

	struct DopplerPreset
	{
		char name[64];
		std::uint64_t id;
		char unknown[0x10];
	}; assert_sizeof(DopplerPreset, 0x58);

	struct OcclusionShape
	{
		char name[64];
		std::uint64_t id;
		float innerAngleVolume;
		float outerAngleVolume;
		float occludedVolume;
		float innerAngleLPF;
		float outerAngleLPF;
		float occludedLPF;
		float yawOffsetAngle;
		float pitchOffsetAngle;
	}; assert_sizeof(OcclusionShape, 0x68);

	struct FocusCone
	{
		char name[64];
		std::uint64_t id;
		float innerAngle;
		float outerAngle;
		float innerDist;
		float outerDist;
		float attenuation;
	}; assert_sizeof(FocusCone, 0x60);

	struct SndCurve
	{
		char name[64];
		std::uint64_t id;
		int numPoints;
		vec2_t points[16];
	}; assert_sizeof(SndCurve, 0xD0);

	struct SpeakerMap
	{
		char name[64];
		std::uint64_t id;
		float monoToStereo[2];
		float stereoToStereo[4];
		float monoTo51[6];
		float stereoTo51[12];
		float monoTo71[8];
		float stereoTo71[16];
		float monoTo7144[16];
		float stereoTo7144[32];
	}; assert_sizeof(SpeakerMap, 0x1C8);

	struct SndFutz
	{
		char name[64];
		std::uint64_t id;
		bool enable;
		float bpfF;
		float bpfQ;
		float lsG;
		float lsF;
		float lsQ;
		float dist;
		float preG;
		float postG;
		float th;
		float tg;
		float clippre;
		float clippost;
		float blend;
		std::uint64_t startAliasId;
		std::uint64_t stopAliasId;
		std::uint64_t loopAliasId;
	}; assert_sizeof(SndFutz, 0x98);

	enum SndAliasLoadType : std::uint8_t
	{
		SAT_LOADED = 0x0,
		SAT_HYBRID_PCM = 0x2,
		SAT_STREAMED = 0x3,
		SAT_PRIMED = 0x4,
	};

	struct SndAliasFlags
	{
		unsigned int looping : 3;
		unsigned int reverb : 1;
		unsigned int shape : 3;
		unsigned int type : 3;
		unsigned int channel : 8;
		unsigned int unknown : 14;
	}; assert_sizeof(SndAliasFlags, 0x4);

#pragma pack(push, 1)
	struct SndAssetBankHeader
	{
		unsigned int magic;
		unsigned int version;
		unsigned int entrySize;
		unsigned int checksumSize;
		unsigned int dependencySize;
		unsigned int entryCount;
		unsigned int dependencyCount;
		unsigned int buildVersion;
		std::uint64_t fileSize;
		std::uint64_t entryOffset;
		std::uint64_t checksumOffset;
		char checksumChecksum[16];
		char dependencies[512];
		std::uint64_t SourceChecksumOffset;
		std::uint64_t AssetNameOffset;
		char zoneName[64];
		char platform[8];
		char language[3];
		unsigned int convertedAssetVersion;
		unsigned int assetSectionSize;
		char padding[5];
	}; assert_sizeof(SndAssetBankHeader, 0x2B0);

	struct SndAssetBankEntry
	{
		unsigned int id;
		unsigned int size;
		unsigned int seekTableSize;
		unsigned int frameCount;
		unsigned int hybridPcmSize;
		unsigned __int64 offset;
		int frameRate;
		char channelCount;
		char looping;
		char format;
		unsigned short EnvelopeTime1;
		unsigned short EnvelopeTime2;
		char EnvelopeLoudness0;
		char EnvelopeLoudness1;
		char EnvelopeLoudness2;
		char EnvelopeLoudness3;
		char BlocksizeKB;
	}; assert_sizeof(SndAssetBankEntry, 44);
#pragma pack(pop)

	struct SndAliasList;

	struct SndAlias
	{
		const char* aliasName;
		const char* subtitle;
		std::uint64_t id;
		std::uint64_t secondaryId;
		std::uint64_t assetId;
		std::uint64_t contextType;
		std::uint64_t contextValue;
		std::uint64_t context2Type;
		std::uint64_t context2Value;
		std::uint64_t duck;
		char unknown0[0x50];
		std::uint64_t volumeFalloffCurve;
		std::uint64_t lpfCurve;
		std::uint64_t hpfCurve;
		std::uint64_t reverbSendCurve;
		char unknown1[0x10];
		std::uint64_t speakerMap;
		char unknown2[0x8];
		std::uint64_t occlusionShape;
		char unknown3[0x38];
		SndAliasList* aliasList;
	}; assert_sizeof(SndAlias, 0x128);
	assert_offsetof(SndAlias, volumeFalloffCurve, 0xA0);
	assert_offsetof(SndAlias, aliasList, 0x120);

	struct SndAliasList
	{
		SndAlias* head;
		std::uint64_t id;
		int count;
		char unknown[0x40];
	}; assert_sizeof(SndAliasList, 0x58);

	struct SndBank
	{
		const char* name;
		const char* zone;
		const char* gameLanguage;
		const char* soundLanguage;
		unsigned int aliasCount;
		SndAliasList* alias;
		SndIndexEntry* aliasIndex;
		SoundTable soundTable;
		unsigned int duckCount;
		SndDuck* ducks;
		unsigned int musicSetCount;
		SndMusicSet* musicSets;
		unsigned int weapReflectionCount;
		WeaponReflectionDef* weapReflections;
		unsigned int dopplerPresetCount;
		DopplerPreset* dopplerPresets;
		unsigned int occlusionShapeCount;
		OcclusionShape* occlusionShapes;
		unsigned int focusConeCount;
		FocusCone* focusCones;
		unsigned int vfCurveCount;
		SndCurve* vfCurves;
		unsigned int lpfCurveCount;
		SndCurve* lpfCurves;
		unsigned int hpfCurveCount;
		SndCurve* hpfCurves;
		unsigned int rvbCurveCount;
		SndCurve* rvbCurves;
		unsigned int speakerMapCount;
		SpeakerMap* speakerMaps;
		unsigned int futzCount;
		SndFutz* futzs;
		SndBankStreamInfo streamInfo;
	}; assert_sizeof(SndBank, 0x1F8);
	assert_offsetof(SndBank, ducks, 0x128);

	struct TriggerDisconnectSoundData
	{
		float analogTriggerValue;
		bool enabledForFireType[6];
	};

	enum ImpactType : std::int32_t
	{
		IMPACT_TYPE_NONE = 0,
		IMPACT_TYPE_BULLET_SMALL = 1,
		IMPACT_TYPE_BULLET_MEDIUM = 2,
		IMPACT_TYPE_BULLET_LARGE = 3,
		IMPACT_TYPE_BULLET_AP = 4,
		IMPACT_TYPE_BULLET_EXPLODE = 5,
		IMPACT_TYPE_SHOTGUN = 6,
		IMPACT_TYPE_SHOTGUN_EXPLODE = 7,
		IMPACT_TYPE_KNIFE_STICK = 8,
		IMPACT_TYPE_GRENADE_BOUNCE = 9,
		IMPACT_TYPE_GRENADE_EXPLODE = 10,
		IMPACT_TYPE_GRENADE_EXPLODE_SMALL = 11,
		IMPACT_TYPE_ROCKET_EXPLODE = 12,
		IMPACT_TYPE_ROCKET_EXPLODE_SMALL = 13,
		IMPACT_TYPE_PROJECTILE_DUD = 14,
		IMPACT_TYPE_VEH_BULLET_MEDIUM = 15,
		IMPACT_TYPE_VEH_BULLET_EXPLODE = 16,
		IMPACT_TYPE_VEH_MISSILE_EXPLODE = 17,
		IMPACT_TYPE_BULLET_RICOCHET_SMALL = 18,
		IMPACT_TYPE_REGISTERED_COUNT = 42,
	};

	enum MeleeImpactType : std::int32_t
	{
		MELEE_IMPACT_TYPE_NONE = 0x0,
		MELEE_IMPACT_TYPE_GUN_SMALL = 0x1,
		MELEE_IMPACT_TYPE_GUN_MEDIUM = 0x2,
		MELEE_IMPACT_TYPE_GUN_LARGE = 0x3,
		MELEE_IMPACT_TYPE_KNIFE = 0x4,
		MELEE_IMPACT_TYPE_SHIELD = 0x5,
		MELEE_IMPACT_TYPE_USER1 = 0x6,
		MELEE_IMPACT_TYPE_USER2 = 0x7,
		MELEE_IMPACT_TYPE_USER3 = 0x8,
		MELEE_IMPACT_TYPE_USER4 = 0x9,
		MELEE_IMPACT_TYPE_USER5 = 0xA,
		MELEE_IMPACT_TYPE_USER6 = 0xB,
		MELEE_IMPACT_TYPE_USER7 = 0xC,
		MELEE_IMPACT_TYPE_USER8 = 0xD,
		MELEE_IMPACT_TYPE_COUNT = 0xE,
	};


	struct WeaponSFXPackageSounds
	{
		const char* name;
		SndAliasLookup sounds[117];
	}; assert_sizeof(WeaponSFXPackageSounds, 0x3B0);

	struct SndBankTransient
	{
		union
		{
			SndBank bank;
			const char* name;
		};
	};

	struct SndBankResident
	{
		union
		{
			SndBank bank;
			const char* name;
		};
	};

	struct WeaponSFXPackage
	{
		const char* name;
		WeaponSFXPackageSounds* sounds;
		WeaponSFXPackage* transientFallbackPackage;
		SndBankTransient* detailSoundBankNPC;
		SndBankTransient* detailSoundBankPlayer;
		char __pad0[0x8];
		const char* szAdsrBaseSetting;
		const char* szWeapSndReflectionClass;
		const char* szWeapSndReflFrontOverride;
		const char* szWeapSndReflDistantOverride;
		char __pad1[0x20];
		SndAliasLookup unknownSound0;
		char __pad2[0x10];
		const char* szMountEnterExitString;
		const char* szMountImpactString;
		SndAliasLookup unknownSounds0[3];
		char __pad3[0x10];
		SndAliasLookup unknownSounds1[5];
		char __pad4[0x18];
	}; assert_sizeof(WeaponSFXPackage, 0x100);

	enum GfxDecalVolumeMaterialFlags : std::uint32_t
	{
		DECAL_VOLUME_MATERIAL_FLAG_COLOR = 0x4,
		DECAL_VOLUME_MATERIAL_FLAG_NORMAL = 0x8,
		DECAL_VOLUME_MATERIAL_FLAG_EMISSIVE = 0x10,
		DECAL_VOLUME_MATERIAL_FLAG_UNK_40 = 0x40, // TODO: stock data only
		DECAL_VOLUME_MATERIAL_FLAG_UNK_400 = 0x400, // TODO: stock data only
	};

	enum GfxDecalVolumeChannel
	{
		DECAL_VOLUME_CHANNEL_COLOR_SPECULAR = 0x0,
		DECAL_VOLUME_CHANNEL_NORMAL_GLOSS = 0x1,
		DECAL_VOLUME_CHANNEL_OPACITY = 0x2,
		DECAL_VOLUME_CHANNEL_EMISSIVE = 0x3,
	};

	struct GfxDecalVolumeMaterial
	{
		const char* name;
		GfxImage* channels[6];
		char unknown[0x18];
		unsigned int flags;
		vec3_t colorTint;
		float alphaDissolveParms;
		unsigned short packedEmissiveScale;
		unsigned short unknown2;
		unsigned int packedDisplacementScaleAndBias;
		float displacementCutoffDistance;
		float displacementCutoffFalloff;
		unsigned int packedTemperatureBaseAndScale;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		unsigned char unknown3;
		unsigned char padding[5];
	}; assert_sizeof(GfxDecalVolumeMaterial, 0x80);

	struct GfxDecalVolumeMask
	{
		const char* name;
		GfxImage* blendMap;
		char unknown[8];
	}; assert_sizeof(GfxDecalVolumeMask, 0x18);

	enum MaterialGeometryType : std::uint32_t
	{
		MTL_TYPE_DEFAULT = 0x0,
		MTL_TYPE_MODEL = 0x80000,
		MTL_TYPE_MODEL_TENSION = 0x80001,
		MTL_TYPE_MODEL_MAYHEM = 0x80002,
		MTL_TYPE_MODEL_VERTCOL = 0x80004,
		MTL_TYPE_MODEL_VERTCOL_TENSION = 0x80005,
		MTL_TYPE_MODEL_LMAP = 0x80008,
		MTL_TYPE_MODEL_LMAP_VERTCOL = 0x8000C,
		MTL_TYPE_MODEL_SELFVIS = 0x80010,
		MTL_TYPE_MODEL_VERTCOL_SELFVIS = 0x80014,
		MTL_TYPE_MODEL_LMAP_SELFVIS = 0x80018,
		MTL_TYPE_MODEL_TENSION_SELFVIS = 0x80011,
		MTL_TYPE_MODEL_SECONDUV_SELFVIS = 0x80030,
		MTL_TYPE_MODEL_SECONDUV_VERTCOL_SELFVIS = 0x80034,
		MTL_TYPE_MODEL_SUBDIV = 0x88000,
		MTL_TYPE_MODEL_SUBDIV_VERTCOL = 0x88004,
		MTL_TYPE_MODEL_SUBDIV_TENSION = 0x88001,
		MTL_TYPE_MODEL_SUBDIV_MAYHEM = 0x88002,
		MTL_TYPE_MODEL_SUBDIV_LMAP = 0x88008,
		MTL_TYPE_MODEL_SUBDIV_LMAP_VERTCOL = 0x8800C,
		MTL_TYPE_MODEL_SUBDIV_SELFVIS = 0x88010,
		MTL_TYPE_MODEL_SUBDIV_VERTCOL_SELFVIS = 0x88014,
		MTL_TYPE_MODEL_SUBDIV_TENSION_SELFVIS = 0x88011,
		MTL_TYPE_MODEL_SUBDIV_LMAP_SELFVIS = 0x88018,
		MTL_TYPE_STATIC_MODEL = 0x90000,
		MTL_TYPE_STATIC_MODEL_TENSION = 0x90001,
		MTL_TYPE_STATIC_MODEL_MAYHEM = 0x90002,
		MTL_TYPE_STATIC_MODEL_VERTCOL = 0x90004,
		MTL_TYPE_STATIC_MODEL_VERTCOL_TENSION = 0x90005,
		MTL_TYPE_STATIC_MODEL_LMAP = 0x90008,
		MTL_TYPE_STATIC_MODEL_LMAP_VERTCOL = 0x9000C,
		MTL_TYPE_STATIC_MODEL_SELFVIS = 0x90010,
		MTL_TYPE_STATIC_MODEL_VERTCOL_SELFVIS = 0x90014,
		MTL_TYPE_STATIC_MODEL_LMAP_SELFVIS = 0x90018,
		MTL_TYPE_STATIC_MODEL_TENSION_SELFVIS = 0x90011,
		MTL_TYPE_STATIC_MODEL_SECONDUV_SELFVIS = 0x90030,
		MTL_TYPE_STATIC_MODEL_SECONDUV_VERTCOL_SELFVIS = 0x90034,
		MTL_TYPE_STATIC_MODEL_SUBDIV = 0x98000,
		MTL_TYPE_STATIC_MODEL_SUBDIV_VERTCOL = 0x98004,
		MTL_TYPE_STATIC_MODEL_SUBDIV_TENSION = 0x98001,
		MTL_TYPE_STATIC_MODEL_SUBDIV_MAYHEM = 0x98002,
		MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP = 0x98008,
		MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP_VERTCOL = 0x9800C,
		MTL_TYPE_STATIC_MODEL_SUBDIV_SELFVIS = 0x98010,
		MTL_TYPE_STATIC_MODEL_SUBDIV_VERTCOL_SELFVIS = 0x98014,
		MTL_TYPE_STATIC_MODEL_SUBDIV_TENSION_SELFVIS = 0x98011,
		MTL_TYPE_STATIC_MODEL_SUBDIV_LMAP_SELFVIS = 0x98018,
		MTL_TYPE_SPLINED_MODEL = 0xA0000,
		MTL_TYPE_SPLINED_MODEL_TENSION = 0xA0001,
		MTL_TYPE_SPLINED_MODEL_MAYHEM = 0xA0002,
		MTL_TYPE_SPLINED_MODEL_VERTCOL = 0xA0004,
		MTL_TYPE_SPLINED_MODEL_VERTCOL_TENSION = 0xA0005,
		MTL_TYPE_SPLINED_MODEL_LMAP = 0xA0008,
		MTL_TYPE_SPLINED_MODEL_LMAP_VERTCOL = 0xA000C,
		MTL_TYPE_SPLINED_MODEL_SELFVIS = 0xA0010,
		MTL_TYPE_SPLINED_MODEL_VERTCOL_SELFVIS = 0xA0014,
		MTL_TYPE_SPLINED_MODEL_LMAP_SELFVIS = 0xA0018,
		MTL_TYPE_SPLINED_MODEL_TENSION_SELFVIS = 0xA0011,
		MTL_TYPE_SPLINED_MODEL_SECONDUV_SELFVIS = 0xA0030,
		MTL_TYPE_SPLINED_MODEL_SECONDUV_VERTCOL_SELFVIS = 0xA0034,
		MTL_TYPE_SPLINED_MODEL_SUBDIV = 0xA8000,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_VERTCOL = 0xA8004,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_TENSION = 0xA8001,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_MAYHEM = 0xA8002,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP = 0xA8008,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP_VERTCOL = 0xA800C,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_SELFVIS = 0xA8010,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_VERTCOL_SELFVIS = 0xA8014,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_TENSION_SELFVIS = 0xA8011,
		MTL_TYPE_SPLINED_MODEL_SUBDIV_LMAP_SELFVIS = 0xA8018,
		MTL_TYPE_SPLINED_STATIC_MODEL = 0xB0000,
		MTL_TYPE_SPLINED_STATIC_MODEL_TENSION = 0xB0001,
		MTL_TYPE_SPLINED_STATIC_MODEL_MAYHEM = 0xB0002,
		MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL = 0xB0004,
		MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL_TENSION = 0xB0005,
		MTL_TYPE_SPLINED_STATIC_MODEL_LMAP = 0xB0008,
		MTL_TYPE_SPLINED_STATIC_MODEL_LMAP_VERTCOL = 0xB000C,
		MTL_TYPE_SPLINED_STATIC_MODEL_SELFVIS = 0xB0010,
		MTL_TYPE_SPLINED_STATIC_MODEL_VERTCOL_SELFVIS = 0xB0014,
		MTL_TYPE_SPLINED_STATIC_MODEL_LMAP_SELFVIS = 0xB0018,
		MTL_TYPE_SPLINED_STATIC_MODEL_TENSION_SELFVIS = 0xB0011,
		MTL_TYPE_SPLINED_STATIC_MODEL_SECONDUV_SELFVIS = 0xB0030,
		MTL_TYPE_SPLINED_STATIC_MODEL_SECONDUV_VERTCOL_SELFVIS = 0xB0034,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV = 0xB8000,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_VERTCOL = 0xB8004,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_TENSION = 0xB8001,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_MAYHEM = 0xB8002,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP = 0xB8008,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP_VERTCOL = 0xB800C,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_SELFVIS = 0xB8010,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_VERTCOL_SELFVIS = 0xB8014,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_TENSION_SELFVIS = 0xB8011,
		MTL_TYPE_SPLINED_STATIC_MODEL_SUBDIV_LMAP_SELFVIS = 0xB8018,
		MTL_TYPE_GLASS = 0x100000,
		MTL_TYPE_GLASS_VERTCOL = 0x100004,
		MTL_TYPE_WORLD = 0x210000,
		MTL_TYPE_WORLD_VERTCOL = 0x210004,
		MTL_TYPE_WORLD_LAYERED = 0x210040,
		MTL_TYPE_WORLD_LAYERED_VERTCOL = 0x210044,
		MTL_TYPE_BMODEL = 0x200000,
		MTL_TYPE_BMODEL_VERTCOL = 0x200004,
		MTL_TYPE_BMODEL_LAYERED = 0x200040,
		MTL_TYPE_SUPERTERRAIN = 0x210080,
		MTL_TYPE_SUPERTERRAIN_GPUGEN = 0x210180,
		MTL_TYPE_SUPERTERRAIN_VERTEX_LAYER_MASK = 0x210880,
		MTL_TYPE_SUPERTERRAIN_GPUGEN_VERTEX_LAYER_MASK = 0x210980,
		MTL_TYPE_AVTTERRAIN = 0x210200,
		MTL_TYPE_SUPERTERRAIN_3D_VERT_POS = 0x210480,
		MTL_TYPE_EFFECT_LMAP = 0x40000C,
		MTL_TYPE_EFFECT_VERTLIT = 0x401004,
		MTL_TYPE_EFFECT_QUAD = 0x40200C,
		MTL_TYPE_EFFECT_FLARE = 0x404004,
		MTL_TYPE_INFO_CARRIER = 0x800000,
	};

	enum MaterialTechniqueType : std::uint32_t
	{
		TECHNIQUE_FIRST = 0x0,
		TECHNIQUE_DEPTH_PREPASS = 0x0,
		TECHNIQUE_DEPTH_PREPASS_VRS_1X1 = 0x1,
		TECHNIQUE_DEPTH_PREPASS_VRS_2X2 = 0x2,
		TECHNIQUE_DEPTH_PREPASS_VELRGD = 0x3,
		TECHNIQUE_DEPTH_PREPASS_VELRGD_VRS_1X1 = 0x4,
		TECHNIQUE_DEPTH_PREPASS_VELRGD_VRS_2X2 = 0x5,
		TECHNIQUE_DEPTH_PREPASS_VELSKN = 0x6,
		TECHNIQUE_DEPTH_PREPASS_VELSKN_VRS_1X1 = 0x7,
		TECHNIQUE_DEPTH_PREPASS_VELSKN_VRS_2X2 = 0x8,
		TECHNIQUE_DEPTH_PREPASS_STENCIL = 0x9,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VRS_1X1 = 0xA,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VRS_2X2 = 0xB,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELRGD = 0xC,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELRGD_VRS_1X1 = 0xD,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELRGD_VRS_2X2 = 0xE,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELSKN = 0xF,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELSKN_VRS_1X1 = 0x10,
		TECHNIQUE_DEPTH_PREPASS_STENCIL_VELSKN_VRS_2X2 = 0x11,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID = 0x12,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VRS_1X1 = 0x13,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VRS_2X2 = 0x14,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELRGD = 0x15,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_1X1 = 0x16,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_2X2 = 0x17,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELSKN = 0x18,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_1X1 = 0x19,
		TECHNIQUE_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_2X2 = 0x1A,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x1B,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH_BACK_FACE = 0x1C,
		TECHNIQUE_BUILD_TRANS_SHADOWMAP = 0x1D,
		TECHNIQUE_EMISSIVE = 0x1E,
		TECHNIQUE_EMISSIVE_VRS_1X1 = 0x1F,
		TECHNIQUE_EMISSIVE_VRS_2X2 = 0x20,
		TECHNIQUE_EMISSIVE_DEBUG = 0x21,
		TECHNIQUE_LIT_FORWARDPLUS_BITMASK = 0x22,
		TECHNIQUE_LIT_FORWARDPLUS_BITMASK_VRS_1X1 = 0x23,
		TECHNIQUE_LIT_FORWARDPLUS_BITMASK_VRS_2X2 = 0x24,
		TECHNIQUE_LIT_FORWARDPLUS_BITMASK_DEBUG = 0x25,
		TECHNIQUE_WIREFRAME_SOLID = 0x26,
		TECHNIQUE_LAST = 0x26,
		TECHNIQUE_SUBDIV_FIRST = 0x27,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS = 0x27,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VRS_1X1 = 0x28,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VRS_2X2 = 0x29,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELRGD = 0x2A,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELRGD_VRS_1X1 = 0x2B,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELRGD_VRS_2X2 = 0x2C,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELSKN = 0x2D,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELSKN_VRS_1X1 = 0x2E,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_VELSKN_VRS_2X2 = 0x2F,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL = 0x30,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VRS_1X1 = 0x31,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VRS_2X2 = 0x32,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELRGD = 0x33,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELRGD_VRS_1X1 = 0x34,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELRGD_VRS_2X2 = 0x35,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELSKN = 0x36,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELSKN_VRS_1X1 = 0x37,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_STENCIL_VELSKN_VRS_2X2 = 0x38,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID = 0x39,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VRS_1X1 = 0x3A,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VRS_2X2 = 0x3B,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELRGD = 0x3C,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_1X1 = 0x3D,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_2X2 = 0x3E,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELSKN = 0x3F,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_1X1 = 0x40,
		TECHNIQUE_SUBDIV_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_2X2 = 0x41,
		TECHNIQUE_SUBDIV_BUILD_SHADOWMAP_DEPTH = 0x42,
		TECHNIQUE_SUBDIV_BUILD_SHADOWMAP_DEPTH_BACK_FACE = 0x43,
		TECHNIQUE_SUBDIV_BUILD_TRANS_SHADOWMAP = 0x44,
		TECHNIQUE_SUBDIV_EMISSIVE = 0x45,
		TECHNIQUE_SUBDIV_EMISSIVE_VRS_1X1 = 0x46,
		TECHNIQUE_SUBDIV_EMISSIVE_VRS_2X2 = 0x47,
		TECHNIQUE_SUBDIV_EMISSIVE_DEBUG = 0x48,
		TECHNIQUE_SUBDIV_LIT_FORWARDPLUS_BITMASK = 0x49,
		TECHNIQUE_SUBDIV_LIT_FORWARDPLUS_BITMASK_VRS_1X1 = 0x4A,
		TECHNIQUE_SUBDIV_LIT_FORWARDPLUS_BITMASK_VRS_2X2 = 0x4B,
		TECHNIQUE_SUBDIV_LIT_FORWARDPLUS_BITMASK_DEBUG = 0x4C,
		TECHNIQUE_SUBDIV_WIREFRAME_SOLID = 0x4D,
		TECHNIQUE_SUBDIV_LAST = 0x4D,
		TECHNIQUE_SUBDIV_PATCH_FIRST = 0x4E,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS = 0x4E,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VRS_1X1 = 0x4F,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VRS_2X2 = 0x50,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELRGD = 0x51,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELRGD_VRS_1X1 = 0x52,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELRGD_VRS_2X2 = 0x53,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELSKN = 0x54,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELSKN_VRS_1X1 = 0x55,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_VELSKN_VRS_2X2 = 0x56,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL = 0x57,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VRS_1X1 = 0x58,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VRS_2X2 = 0x59,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELRGD = 0x5A,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELRGD_VRS_1X1 = 0x5B,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELRGD_VRS_2X2 = 0x5C,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELSKN = 0x5D,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELSKN_VRS_1X1 = 0x5E,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_STENCIL_VELSKN_VRS_2X2 = 0x5F,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID = 0x60,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VRS_1X1 = 0x61,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VRS_2X2 = 0x62,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELRGD = 0x63,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_1X1 = 0x64,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_2X2 = 0x65,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELSKN = 0x66,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_1X1 = 0x67,
		TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_2X2 = 0x68,
		TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_DEPTH = 0x69,
		TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_DEPTH_BACK_FACE = 0x6A,
		TECHNIQUE_SUBDIV_PATCH_BUILD_TRANS_SHADOWMAP = 0x6B,
		TECHNIQUE_SUBDIV_PATCH_EMISSIVE = 0x6C,
		TECHNIQUE_SUBDIV_PATCH_EMISSIVE_VRS_1X1 = 0x6D,
		TECHNIQUE_SUBDIV_PATCH_EMISSIVE_VRS_2X2 = 0x6E,
		TECHNIQUE_SUBDIV_PATCH_EMISSIVE_DEBUG = 0x6F,
		TECHNIQUE_SUBDIV_PATCH_LIT_FORWARDPLUS_BITMASK = 0x70,
		TECHNIQUE_SUBDIV_PATCH_LIT_FORWARDPLUS_BITMASK_VRS_1X1 = 0x71,
		TECHNIQUE_SUBDIV_PATCH_LIT_FORWARDPLUS_BITMASK_VRS_2X2 = 0x72,
		TECHNIQUE_SUBDIV_PATCH_LIT_FORWARDPLUS_BITMASK_DEBUG = 0x73,
		TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SOLID = 0x74,
		TECHNIQUE_SUBDIV_PATCH_LAST = 0x74,
		TECHNIQUE_WORLDTESS_FIRST = 0x75,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS = 0x75,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VRS_1X1 = 0x76,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VRS_2X2 = 0x77,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELRGD = 0x78,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELRGD_VRS_1X1 = 0x79,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELRGD_VRS_2X2 = 0x7A,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELSKN = 0x7B,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELSKN_VRS_1X1 = 0x7C,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_VELSKN_VRS_2X2 = 0x7D,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL = 0x7E,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VRS_1X1 = 0x7F,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VRS_2X2 = 0x80,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELRGD = 0x81,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELRGD_VRS_1X1 = 0x82,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELRGD_VRS_2X2 = 0x83,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELSKN = 0x84,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELSKN_VRS_1X1 = 0x85,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_STENCIL_VELSKN_VRS_2X2 = 0x86,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID = 0x87,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VRS_1X1 = 0x88,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VRS_2X2 = 0x89,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELRGD = 0x8A,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_1X1 = 0x8B,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_2X2 = 0x8C,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELSKN = 0x8D,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_1X1 = 0x8E,
		TECHNIQUE_WORLDTESS_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_2X2 = 0x8F,
		TECHNIQUE_WORLDTESS_BUILD_SHADOWMAP_DEPTH = 0x90,
		TECHNIQUE_WORLDTESS_BUILD_SHADOWMAP_DEPTH_BACK_FACE = 0x91,
		TECHNIQUE_WORLDTESS_BUILD_TRANS_SHADOWMAP = 0x92,
		TECHNIQUE_WORLDTESS_EMISSIVE = 0x93,
		TECHNIQUE_WORLDTESS_EMISSIVE_VRS_1X1 = 0x94,
		TECHNIQUE_WORLDTESS_EMISSIVE_VRS_2X2 = 0x95,
		TECHNIQUE_WORLDTESS_EMISSIVE_DEBUG = 0x96,
		TECHNIQUE_WORLDTESS_LIT_FORWARDPLUS_BITMASK = 0x97,
		TECHNIQUE_WORLDTESS_LIT_FORWARDPLUS_BITMASK_VRS_1X1 = 0x98,
		TECHNIQUE_WORLDTESS_LIT_FORWARDPLUS_BITMASK_VRS_2X2 = 0x99,
		TECHNIQUE_WORLDTESS_LIT_FORWARDPLUS_BITMASK_DEBUG = 0x9A,
		TECHNIQUE_WORLDTESS_WIREFRAME_SOLID = 0x9B,
		TECHNIQUE_WORLDTESS_LAST = 0x9B,
		TECHNIQUE_TERRAIN_TESS_FIRST = 0x9C,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS = 0x9C,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VRS_1X1 = 0x9D,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VRS_2X2 = 0x9E,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELRGD = 0x9F,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELRGD_VRS_1X1 = 0xA0,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELRGD_VRS_2X2 = 0xA1,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELSKN = 0xA2,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELSKN_VRS_1X1 = 0xA3,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_VELSKN_VRS_2X2 = 0xA4,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL = 0xA5,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VRS_1X1 = 0xA6,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VRS_2X2 = 0xA7,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELRGD = 0xA8,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELRGD_VRS_1X1 = 0xA9,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELRGD_VRS_2X2 = 0xAA,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELSKN = 0xAB,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELSKN_VRS_1X1 = 0xAC,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_STENCIL_VELSKN_VRS_2X2 = 0xAD,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID = 0xAE,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VRS_1X1 = 0xAF,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VRS_2X2 = 0xB0,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELRGD = 0xB1,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_1X1 = 0xB2,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELRGD_VRS_2X2 = 0xB3,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELSKN = 0xB4,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELSKN_VRS_1X1 = 0xB5,
		TECHNIQUE_TERRAIN_TESS_DEPTH_PREPASS_OBJECTID_VELSKN_BITMASK_VRS_2X2 = 0xB6,
		TECHNIQUE_TERRAIN_TESS_BUILD_SHADOWMAP_DEPTH = 0xB7,
		TECHNIQUE_TERRAIN_TESS_BUILD_SHADOWMAP_DEPTH_BACK_FACE = 0xB8,
		TECHNIQUE_TERRAIN_TESS_BUILD_TRANS_SHADOWMAP = 0xB9,
		TECHNIQUE_TERRAIN_TESS_EMISSIVE = 0xBA,
		TECHNIQUE_TERRAIN_TESS_EMISSIVE_VRS_1X1 = 0xBB,
		TECHNIQUE_TERRAIN_TESS_EMISSIVE_VRS_2X2 = 0xBC,
		TECHNIQUE_TERRAIN_TESS_EMISSIVE_DEBUG = 0xBD,
		TECHNIQUE_TERRAIN_TESS_LIT_FORWARDPLUS_BITMASK = 0xBE,
		TECHNIQUE_TERRAIN_TESS_LIT_FORWARDPLUS_BITMASK_VRS_1X1 = 0xBF,
		TECHNIQUE_TERRAIN_TESS_LIT_FORWARDPLUS_BITMASK_VRS_2X2 = 0xC0,
		TECHNIQUE_TERRAIN_TESS_LIT_FORWARDPLUS_BITMASK_DEBUG = 0xC1,
		TECHNIQUE_TERRAIN_TESS_WIREFRAME_SOLID = 0xC2,
		TECHNIQUE_TERRAIN_TESS_LAST = 0xC2,
		TECHNIQUE_COUNT = 0xC3,
		TECHNIQUE_NONE = 0xC4,
		TECHNIQUE_PREPASS_BEGIN = 0x0,
		TECHNIQUE_PREPASS_END = 0x1A,
		TECHNIQUE_EMISSIVE_BEGIN = 0x1E,
		TECHNIQUE_EMISSIVE_END = 0x21,
		TECHNIQUE_LIT_BEGIN = 0x22,
		TECHNIQUE_LIT_END = 0x26,
		TECHNIQUE_LIT = 0x22,
		TECHNIQUE_DEFAULT = 0x1E,
	};

	struct Packed128
	{
		unsigned __int64 p0;
		unsigned __int64 p1;
	};

	struct GfxShaderLoadDef
	{
		char* program;
		unsigned int programSize;
	}; assert_sizeof(GfxShaderLoadDef, 0x10);

	struct MaterialShaderProgram
	{
		void* shader;
		GfxShaderLoadDef loadDef;
		char unknown[0x18];
	}; assert_sizeof(MaterialShaderProgram, 0x30);

	struct MaterialLibShader
	{
		const char* name;
		const char* debugName;
		MaterialShaderProgram prog;
	}; assert_sizeof(MaterialLibShader, 0x40);

	struct ComputeShaderLoadDef
	{
		char* program;
		unsigned int programSize;
		char unknown[0xC];
	}; assert_sizeof(ComputeShaderLoadDef, 0x18);

	struct ComputeShaderProgram
	{
		const char* name;
		const char* debugName;
		void* shader;
		ComputeShaderLoadDef loadDef;
		char unknown[0x20];
	}; assert_sizeof(ComputeShaderProgram, 0x50);

	struct ComputeShader
	{
		const char* name;
		const char* debugName;
		ComputeShaderProgram* programs[2];
		unsigned int variantCount;
		ComputeShaderProgram** variants;
	}; assert_sizeof(ComputeShader, 0x30);

	struct MaterialVertexShader
	{
		const char* name;
		const char* debugName;
		MaterialShaderProgram prog;
	}; assert_sizeof(MaterialVertexShader, 0x40);

	struct MaterialHullShader
	{
		const char* name;
		const char* debugName;
		MaterialShaderProgram prog;
	}; assert_sizeof(MaterialHullShader, 0x40);

	struct MaterialDomainShader
	{
		const char* name;
		const char* debugName;
		MaterialShaderProgram prog;
	}; assert_sizeof(MaterialDomainShader, 0x40);

	struct MaterialPixelShader
	{
		const char* name;
		const char* debugName;
		MaterialShaderProgram prog;
	}; assert_sizeof(MaterialPixelShader, 0x40);

	enum MaterialArgumentType : std::uint8_t
	{
		MTL_ARG_CODE_CONST = 0x0,
		MTL_ARG_CODE_TEXTURE = 0x1,
		MTL_ARG_CODE_PERSISTENT_TEXTURE = 0x2,
		MTL_ARG_CODE_BUFFER = 0x3,
		MTL_ARG_CODE_PERSISTENT_BUFFER = 0x4,
		MTL_ARG_MATERIAL_TEXTURE = 0x5,
		MTL_ARG_CODE_RWTEXTURE = 0x6,
		MTL_ARG_CODE_RWBUFFER = 0x7,
		MTL_ARG_CODE_CBUFFER = 0x8,
		MTL_ARG_MATERIAL_CONST = 0x9,
		MTL_ARG_COUNT = 0xA,
	};

	enum MaterialShaderStageFlags : std::uint8_t
	{
		MTL_SHADER_STAGE_VS = 0x2,
		MTL_SHADER_STAGE_PS = 0x10,
	};

	enum ShaderCodeConstants : std::uint16_t
	{
		CONST_SRC_CODE_CAMERA_POSITION = 0x0,
		CONST_SRC_CODE_CAMERA_OFFSET = 0x1,
		CONST_SRC_CODE_GAME_TIME = 0x2,
		CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x4,
		CONST_SRC_CODE_RESOLUTION_SCALE = 0x67,
		CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0xA4,
	};

	enum ShaderCodeBuffers : std::uint16_t
	{
		BUFFER_SRC_CODE_LIGHTMAP_PACKING = 0x28,
		BUFFER_SRC_CODE_SCREEN_TILES = 0x2C,
		BUFFER_SRC_CODE_WORLD_SURFACE_DATA = 0x2D,
	};

	struct MaterialShaderArgument
	{
		char unknown[8];
	}; assert_sizeof(MaterialShaderArgument, 0x8);

	struct MaterialRenderTargetFormat
	{
		unsigned char dsFormat;
		unsigned char colorRtFormats[4];
	}; assert_sizeof(MaterialRenderTargetFormat, 0x5);

	enum GfxRasterizerBits : std::uint8_t
	{
		GFX_RASTERIZER_CULL_NONE = 0x0,
		GFX_RASTERIZER_CULL_BACK = 0x1,
		GFX_RASTERIZER_CULL_FRONT = 0x2,
		GFX_RASTERIZER_CULL_MASK = 0x3,
	};

	struct GfxStateBits
	{
		unsigned __int64 depthStencilRasterizerBits;
		unsigned __int64 blendBits;
	}; assert_sizeof(GfxStateBits, 0x10);

	struct MaterialInputLayout
	{
		char unknown[9];
	}; assert_sizeof(MaterialInputLayout, 0x9);

	enum MaterialConstantLayout : std::uint8_t
	{
		MTL_CONSTANT_LAYOUT_NONE = 0x0,
		MTL_CONSTANT_LAYOUT_SMODEL = 0x1,
		MTL_CONSTANT_LAYOUT_SMODEL_LIT_PROBE = 0x2,
		MTL_CONSTANT_LAYOUT_SMODEL_LIT_LMAP = 0x3,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV = 0x4,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV_LIT_PROBE = 0x5,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV_LIT_LMAP = 0x6,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV_PATCH = 0x7,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV_PATCH_LIT_PROBE = 0x8,
		MTL_CONSTANT_LAYOUT_SMODEL_SUBDIV_PATCH_LIT_LMAP = 0x9,
		MTL_CONSTANT_LAYOUT_XMODEL = 0xA,
		MTL_CONSTANT_LAYOUT_XMODEL_VEL = 0xB,
		MTL_CONSTANT_LAYOUT_XMODEL_VEL_SKIN = 0xC,
		MTL_CONSTANT_LAYOUT_XMODEL_LIT_PROBE = 0xD,
		MTL_CONSTANT_LAYOUT_XMODEL_LIT_LMAP = 0xE,
		MTL_CONSTANT_LAYOUT_XMODEL_LIT_PROBE_OBJ = 0xF,
		MTL_CONSTANT_LAYOUT_XMODEL_LIT_LMAP_OBJ = 0x10,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV = 0x11,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_VEL = 0x12,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_VEL_SKIN = 0x13,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_LIT_PROBE = 0x14,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_LIT_LMAP = 0x15,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_LIT_PROBE_OBJ = 0x16,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_LIT_LMAP_OBJ = 0x17,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH = 0x18,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_VEL = 0x19,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_VEL_SKIN = 0x1A,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_LIT_PROBE = 0x1B,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_LIT_LMAP = 0x1C,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_LIT_PROBE_OBJ = 0x1D,
		MTL_CONSTANT_LAYOUT_XMODEL_SUBDIV_PATCH_LIT_LMAP_OBJ = 0x1E,
		MTL_CONSTANT_LAYOUT_GLASS = 0x1F,
		MTL_CONSTANT_LAYOUT_WORLD = 0x20,
		MTL_CONSTANT_LAYOUT_WORLD_AUX = 0x21,
		MTL_CONSTANT_LAYOUT_WORLD_AUX_QUERY = 0x22,
		MTL_CONSTANT_LAYOUT_WORLD_LIT_LMAP = 0x23,
		MTL_CONSTANT_LAYOUT_WORLD_TESS = 0x24,
		MTL_CONSTANT_LAYOUT_WORLD_TESS_QUERY = 0x25,
		MTL_CONSTANT_LAYOUT_WORLD_TESS_LIT_LMAP = 0x26,
		MTL_CONSTANT_LAYOUT_BMODEL = 0x27,
		MTL_CONSTANT_LAYOUT_BMODEL_AUX = 0x28,
		MTL_CONSTANT_LAYOUT_BMODEL_AUX_QUERY = 0x29,
		MTL_CONSTANT_LAYOUT_BMODEL_VEL = 0x2A,
		MTL_CONSTANT_LAYOUT_BMODEL_LIT_LMAP = 0x2B,
		MTL_CONSTANT_LAYOUT_BMODEL_TESS = 0x2C,
		MTL_CONSTANT_LAYOUT_BMODEL_TESS_QUERY = 0x2D,
		MTL_CONSTANT_LAYOUT_BMODEL_TESS_VEL = 0x2E,
		MTL_CONSTANT_LAYOUT_BMODEL_TESS_LIT_LMAP = 0x2F,
	};

	struct MaterialTechnique
	{
		const char* name;
		MaterialTechniqueType type;
		unsigned short flags;
		unsigned char pipelineStateInputLayoutCount;
		char unknown0[9];
		Packed128* pipelineStates;
		char unknown1[0x20];
		MaterialInputLayout* inputLayout;
		MaterialVertexShader* vertexShader;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		MaterialPixelShader* pixelShader;
		char unknown2[4];
		unsigned char argCount;
		char unknown3[8];
		unsigned char renderTargetFormatCount;
		char unknown4[2];
		MaterialRenderTargetFormat* renderTargetFormats;
		char unknown5[0x10];
		GfxStateBits stateBits;
		MaterialShaderArgument* args;
	}; assert_sizeof(MaterialTechnique, 0xA8);
	assert_offsetof(MaterialTechnique, stateBits, 0x90);
	assert_offsetof(MaterialTechnique, pipelineStateInputLayoutCount, 0xE);
	assert_offsetof(MaterialTechnique, vertexShader, 0x48);
	assert_offsetof(MaterialTechnique, argCount, 0x6C);
	assert_offsetof(MaterialTechnique, renderTargetFormatCount, 0x75);
	assert_offsetof(MaterialTechnique, renderTargetFormats, 0x78);

	struct TechniqueMask
	{
		unsigned __int64 mask[2];
	};

	struct MaterialTechniqueSet
	{
		const char* name;
		unsigned __int64 flags;
		MaterialGeometryType materialType;
		TechniqueMask techniqueMask;
		MaterialTechnique** maskedTechniques;
	}; assert_sizeof(MaterialTechniqueSet, 0x30);

	struct MaterialTextureDef
	{
		unsigned char index;
		GfxImage* image;
	}; assert_sizeof(MaterialTextureDef, 0x10);

	struct MaterialConstantDef
	{
		unsigned char index;
		vec4_t literal;
	}; assert_sizeof(MaterialConstantDef, 0x14);

	enum MaterialConstantIndex : std::uint8_t
	{
		MATERIAL_CONSTANT_SPRITE_BLEND_PARMS = 0x9,
		MATERIAL_CONSTANT_TRANSMISSION_TINT = 18,
		MATERIAL_CONSTANT_SPRITE_BLEND_TINT = 0x12,
		MATERIAL_CONSTANT_FROST = 27,
		MATERIAL_CONSTANT_EFFECT_COLOR_TINT = 36,
		MATERIAL_CONSTANT_UV_ANIM = 0x3F,
		MATERIAL_CONSTANT_FLAG_PARMS = 0x4F,
		MATERIAL_CONSTANT_FLAG_WAVE_0 = 0x50,
		MATERIAL_CONSTANT_FLAG_WAVE_1 = 0x51,
		MATERIAL_CONSTANT_FLAG_WAVE_2 = 0x52,
		MATERIAL_CONSTANT_LAYER_COUNT = 0x58,
		MATERIAL_CONSTANT_ATLAS_SIZE = 0x5E,
		MATERIAL_CONSTANT_ATLAS_INDEXING = 0x5F,
		MATERIAL_CONSTANT_MODEL_EFFECT_PARMS = 0x72,
		MATERIAL_CONSTANT_DISTORTION_SCALE = 0x7F,
		MATERIAL_CONSTANT_2D_COLOR_TINT = 126,
		MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_0 = 0xE0, // TODO: id not proven
		MATERIAL_CONSTANT_OCEAN_SCROLL_NORMAL_1 = 0xE1, // TODO: id not proven
		MATERIAL_CONSTANT_FALLOFF_PARMS = 0xE2, // TODO: id not proven
		MATERIAL_CONSTANT_FALLOFF_BEGIN_COLOR = 0xE3, // TODO: id not proven
		MATERIAL_CONSTANT_FALLOFF_END_COLOR = 0xE4, // TODO: id not proven
		MATERIAL_CONSTANT_FEATHER_PARMS = 0xE5, // TODO: id not proven
		MATERIAL_CONSTANT_EYE_OFFSET_PARMS = 0xE6, // TODO: id not proven
	};

	enum MaterialTechniqueSetFeature : std::uint64_t // first hex token after the alpha token of the name
	{
		TECHSET_FEATURE_TRANSMISSION = 0x1,
		TECHSET_FEATURE_WIND = 0x4000000000000,
	};

	enum CameraRegion : std::uint8_t
	{
		CAMERA_REGION_LIT_OPAQUE = 0,
		CAMERA_REGION_LIT_DECAL = 2,
		CAMERA_REGION_LIT_TRANS = 3,
		CAMERA_REGION_EMISSIVE = 4,
	};

	enum SurfaceFlags : std::uint32_t
	{
		SURFACE_FLAG_SKY = 0x4,
		SURFACE_FLAG_LADDER = 0x8,
		SURFACE_FLAG_NOIMPACT = 0x10,
		SURFACE_FLAG_NODRAW = 0x80,
		SURFACE_FLAG_NOPENETRATE = 0x100,
		SURFACE_FLAG_STAIRS = 0x200,
	};

	enum MaterialLayerFlags : std::uint32_t
	{
		MATERIAL_LAYER_BLEND_NORMAL = 0x1,
		MATERIAL_LAYER_BLEND_OCCLUSION = 0x2,
		MATERIAL_LAYER_BLEND_GLOSS = 0x4,
		MATERIAL_LAYER_BLEND_MASK = 0x8,
		MATERIAL_LAYER_BLEND_HEIGHT = 0x10,
		MATERIAL_LAYER_TINT_MASK = 0x40,
		MATERIAL_LAYER_COLOR_MULTIPLY = 0x80,
	};

	struct MaterialStandard
	{
		const char* name;
		char unknown[0x68];
	}; assert_sizeof(MaterialStandard, 0x70);

	struct MaterialImageTable
	{
		char unknown[0x40];
		GfxImage* images[11];
	}; assert_sizeof(MaterialImageTable, 0x98);

	struct Material
	{
		const char* name;
		unsigned int contents;
		unsigned int surfaceFlags;
		float maxDisplacement;
		MaterialGeometryType materialType;
		CameraRegion cameraRegion;
		unsigned char sortKey;
		unsigned short flags;
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char layerCount;
		unsigned char unknown0;
		unsigned short packedAtlasDataSize;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		char unknown1[4];
		char* packedAtlasData;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		MaterialConstantDef* constantTable;
		MaterialStandard* materialStandard;
		GfxDecalVolumeMaterial* decalVolumeMaterial;
		MaterialImageTable* imageTable;
		char unknown2[8];
		Packed128 drawSurf;
		const char** subMaterials;
	}; assert_sizeof(Material, 0x80);
	assert_offsetof(Material, textureCount, 0x1C);
	assert_offsetof(Material, packedAtlasData, 0x28);
	assert_offsetof(Material, drawSurf, 0x68);

	struct GfxPlacement
	{
		vec4_t quat;
		vec3_t origin;
	}; assert_sizeof(GfxPlacement, 0x1C);

	struct StVertexData0
	{
		char* data;
		char __pad0[0x20];
	}; assert_sizeof(StVertexData0, 0x28);

	struct StMeshLayerMaskData
	{
		char* data;
		GfxWrappedBuffer buffer;
	}; assert_sizeof(StMeshLayerMaskData, 0x48);

	enum StMeshFlags : std::uint8_t
	{
		ST_MESH_CLIPPED_OUT = 0x1,
		ST_MESH_RENDERABLE = 0x2,
		ST_MESH_GPU_GENERATE = 0x4,
		ST_MESH_VERT_POS_3D = 0x8,
		ST_MESH_LAYER_MASK = 0x10,
	};

	struct StMesh
	{
		StVertexData0 vd0;
		StMeshLayerMaskData layerMask;
		char __pad0[0x20];
		unsigned short* indices;
		StreamKey* streamKey;
		char __pad1[0x80];
		unsigned short vertexCount;
		unsigned short triCount;
		unsigned short cutoutTriCount;
		unsigned char flags;
	}; assert_sizeof(StMesh, 0x128);
	assert_offsetof(StMesh, indices, 0x90);
	assert_offsetof(StMesh, vertexCount, 0x120);

	struct StTerrainNode
	{
		StMesh mesh;
		Bounds wldBounds;
		vec2_t compressBase;
		vec2_t compressScale;
		unsigned int layerMask;
		unsigned short xIndex;
		unsigned short yIndex;
	}; assert_sizeof(StTerrainNode, 0x158);

	struct StTerrainGridLevel
	{
		unsigned int width;
		unsigned int height;
		StTerrainNode* nodes;
	}; assert_sizeof(StTerrainGridLevel, 0x10);

	struct StTerrainGrid
	{
		unsigned int numLevels;
		StTerrainGridLevel* levels;
	}; assert_sizeof(StTerrainGrid, 0x10);

	struct TerrainLayerDataCode
	{
		vec4_t textureTransform[2];
		unsigned int flags;
		unsigned int alphaLayer;
		vec2_t textureScale;
		vec2_t detailScale;
		vec3_t colorTint;
		float alphaRevealSoftEdge;
		float detailScaleHeight;
		unsigned int normalMapPackedScaleBias;
		unsigned int albedoMapPackedScaleBias;
		unsigned int colorSampler;
		unsigned int colorMap;
		unsigned int tintMask;
		unsigned int normalSampler;
		unsigned int normalMap;
		unsigned int revealSampler;
		unsigned int revealMap;
		unsigned int emissiveMap;
		unsigned int transmittanceMap;
		unsigned int thicknessMap;
		unsigned int displacementMap;
		float displacementScale;
		float grassHeight;
		float grassHeightVariance;
		float grassWidth;
		float grassWidthVariance;
		float grassTilt;
		float grassTiltVariance;
		float grassBend;
	}; assert_sizeof(TerrainLayerDataCode, 0xA0);

	struct StTerrainMaterialLayer
	{
		Material* material;
		unsigned short* materialTextureIdxs;
		vec4_t stFromXYZ[2];
		float himipRadiusInvSq;
		int layer;
		int decalSort;
		bool dynamicUvs;
		TerrainLayerDataCode layerData;
		char unknown[0x18];
	}; assert_sizeof(StTerrainMaterialLayer, 0xF8);

	struct StTerrainDisplacementScale
	{
		unsigned int mask;
		float scale;
	};

	struct StTerrainClutterLayer
	{
		unsigned int sampleOffset;
		unsigned int bitmaskOffset;
		unsigned int tileCount[2];
		vec2_t tileSizeUV;
		vec2_t layerSize;
		vec3_t angleMin;
		vec3_t angleMax;
		vec2_t zOffsetMinMax;
		vec2_t scaleMinMax;
		unsigned int maxInstanceCount;
		unsigned int tileInstanceCountOffset;
		unsigned int useSurfaceNormal;
	}; assert_sizeof(StTerrainClutterLayer, 0x54);

	struct StClutterSampleBitmask
	{
		StreamKey* streamKey;
		unsigned int wordCount;
		unsigned int* bitmask;
		GfxWrappedBuffer bitmaskBuffer;
		GfxWrappedBuffer relocatedBitmaskBuffer;
	}; assert_sizeof(StClutterSampleBitmask, 0x98);

	struct StDiskTerrainSurface
	{
		unsigned int highLODIndexCount;
		unsigned short* highLODIndices;
		char __pad0[0x20];
		StTerrainGrid grid;
		GfxPlacement objToWld;
		Bounds wldBounds;
		unsigned int layerCount;
		unsigned int surfaceMapSetIndex;
		unsigned int surfaceMapXCount;
		unsigned int surfaceMapYCount;
		unsigned int tileVertXCount;
		vec2_t surfaceMapOrigin;
		float surfaceMapWorldSize;
		float heightOffset;
		float heightScale;
		float halfWldSize;
		unsigned int unknown0;
		unsigned int layerMaterialsCount;
		StTerrainMaterialLayer* layerMaterials;
		unsigned int layerDisplacementMask;
		unsigned int displacementScalesCount;
		StTerrainDisplacementScale* displacementScales;
		unsigned int clutterLayerCount;
		StTerrainClutterLayer* clutterLayers;
		GfxWrappedBuffer clutterLayersBuffer;
		StClutterSampleBitmask clutterSampleBitmask;
		unsigned int clutterTileCount;
		unsigned int* clutterTileInstanceCounts;
		unsigned int layerMaskMapIdxsCount;
		unsigned short* layerMaskMapIdxs;
		GfxImage* heightMap;
		GfxImage* cutoutMap;
		GfxImage* combinedAlbedoMap;
		GfxImage* combinedNogMap;
		float combinedMapStreamRadiusInvSq;
		unsigned int lightmapIndex;
		char unknown1[0x18];
	}; assert_sizeof(StDiskTerrainSurface, 0x208);
	assert_offsetof(StDiskTerrainSurface, layerMaterialsCount, 0xA4);
	assert_offsetof(StDiskTerrainSurface, layerMaterials, 0xA8);
	assert_offsetof(StDiskTerrainSurface, clutterSampleBitmask, 0x110);
	assert_offsetof(StDiskTerrainSurface, heightMap, 0x1C8);
	assert_offsetof(StDiskTerrainSurface, clutterTileCount, 0x1A8);

	struct StTerrainMaps
	{
		int mapResolution;
		int paddedResolution;
	};

	struct StClutterSamplePoints
	{
		unsigned int sampleCount;
		vec2_t* samplePoints;
		GfxWrappedBuffer samplePointBuffer;
	}; assert_sizeof(StClutterSamplePoints, 0x50);

	struct StLightmap
	{
		GfxImage* images[3];
	};

	struct StTerrainUnknownRecord
	{
		char unknown[0x20];
	}; assert_sizeof(StTerrainUnknownRecord, 0x20);

	struct StTerrain
	{
		const char* name;
		unsigned int surfaceCount;
		StDiskTerrainSurface* surfaces;
		unsigned int mapsCount;
		StTerrainMaps* maps;
		char unknown0[0x18];
		unsigned int flattenedImagesCount;
		GfxImage** flattenedImages;
		unsigned int unknownImagesCount;
		GfxImage** unknownImages;
		StClutterSamplePoints clutterSamplePoints;
		GfxLightmapType lightmapType;
		unsigned int lightmapCount;
		StLightmap* lightmaps;
		unsigned int unknownRecordCount;
		StTerrainUnknownRecord* unknownRecords;
		char unknown1[0x38];
	}; assert_sizeof(StTerrain, 0x108);
	assert_offsetof(StTerrain, flattenedImages, 0x48);
	assert_offsetof(StTerrain, clutterSamplePoints, 0x60);
	assert_offsetof(StTerrain, lightmaps, 0xB8);
	assert_offsetof(StTerrain, unknownRecords, 0xC8);

	struct XAnimCurveControlPoint
	{
		float time;
		float value;
		float invTimeDelta;
	}; assert_sizeof(XAnimCurveControlPoint, 0xC);

	struct XAnimCurve
	{
		const char* name;
		XAnimCurveControlPoint* controlPoints;
		int numControlPoints;
		unsigned char curveType;
		float duration;
	}; assert_sizeof(XAnimCurve, 0x20);

	struct XAnimDynamicBoneNoiseAxis
	{
		XAnimCurve* curve;
		float strength;
		float rate;
		float start;
	}; assert_sizeof(XAnimDynamicBoneNoiseAxis, 0x18);

	struct XAnimDynamicBoneNoiseProperties
	{
		unsigned char relative;
		XAnimDynamicBoneNoiseAxis axis[3];
	}; assert_sizeof(XAnimDynamicBoneNoiseProperties, 0x50);

	struct XAnimDynamicBoneNoise
	{
		bool enable;
		XAnimDynamicBoneNoiseProperties translation;
		XAnimDynamicBoneNoiseProperties rotation;
	}; assert_sizeof(XAnimDynamicBoneNoise, 0xA8);

	struct XAnimRotationalSpringProperties
	{
		float swingStrength;
		float twistStrength;
	};

	struct XAnimTranslationalSpringProperties
	{
		vec3_t strength;
		vec3_t damping;
		vec3_t minLimit;
		vec3_t maxLimit;
	};

	struct XAnimDynamicBoneProperties
	{
		scr_string_t boneName;
		int parentDynamicBoneIndex;
		int dynamicBoneChainRootIndex;
		float chainLength;
		float invMass;
		vec3_t centerOfMass;
		float invInertiaTensor;
		vec3_t twistAxis;
		float minTwistAngle;
		float maxTwistAngle;
		float damping;
		float gravityScale;
		float cosMaxSwingAngle;
		vec4_t localBasePoseQuat;
		vec3_t localBasePoseTrans;
		unsigned int firstCollisionShapeIndex;
		unsigned int numCollisionShapes;
		SndAliasLookup lightHitSound;
		SndAliasLookup heavyHitSound;
		float lightHitThreshold;
		float heavyHitThreshold;
		bool enableSwingLimit;
		bool enableTwistLimit;
		bool enableRotationalSpring;
		bool enableTranslationalSpring;
		XAnimRotationalSpringProperties rotationalSpring;
		XAnimTranslationalSpringProperties translationalSpring;
		XAnimDynamicBoneNoise noise;
	}; assert_sizeof(XAnimDynamicBoneProperties, 0x168);
	assert_offsetof(XAnimDynamicBoneProperties, lightHitSound, 0x68);
	assert_offsetof(XAnimDynamicBoneProperties, noise, 0xC0);

	struct XAnimCollidableBone
	{
		scr_string_t boneName;
		unsigned int firstCollisionShapeIndex;
		unsigned int numCollisionShapes;
	}; assert_sizeof(XAnimCollidableBone, 0xC);

	struct XAnimSphereCollisionShape
	{
		vec3_t center;
		float radius;
	};

	struct XAnimCapsuleCollisionShape
	{
		vec3_t start;
		vec3_t end;
		float radius;
	};

	struct XAnimBoxCollisionShape
	{
		vec3_t center;
		vec3_t extents;
	};

	union XAnimCollisionShapeUnion
	{
		XAnimSphereCollisionShape sphere;
		XAnimCapsuleCollisionShape capsule;
		XAnimBoxCollisionShape box;
	};

	struct XAnimCollisionShape
	{
		unsigned char type;
		XAnimCollisionShapeUnion u;
		unsigned int unknown;
	}; assert_sizeof(XAnimCollisionShape, 0x24);

	struct XAnimCustomPinConstraint
	{
		int dynamicBoneIndexA;
		int dynamicBoneIndexB;
		int boneNameIndexA;
		vec3_t localTranslationA;
		vec3_t localTranslationB;
		float distance;
		float stiffness;
	}; assert_sizeof(XAnimCustomPinConstraint, 0x2C);

	struct XAnimDynamicBones
	{
		const char* name;
		unsigned int numDynamicBones;
		XAnimDynamicBoneProperties* dynamicBoneProperties;
		unsigned int numCollidableBones;
		XAnimCollidableBone* collidableBones;
		unsigned int numCollidableBoneCollisionShapes;
		unsigned int numCollisionShapes;
		XAnimCollisionShape* collisionShapes;
		unsigned int numCustomPinConstraints;
		XAnimCustomPinConstraint* customPinConstraints;
		unsigned int numPinConstraintBoneNames;
		scr_string_t* pinConstraintBoneNames;
	}; assert_sizeof(XAnimDynamicBones, 0x58);

	struct ScriptableDef;
	struct ClothAsset;

	struct XModelPhysicsUsageCounter
	{
		int serverEnt;
		int clientEnt;
		int dynEnt;
	};

	struct DObjAnimMat
	{
		vec4_t quat;
		vec3_t trans;
		float transWeight;
	}; assert_sizeof(DObjAnimMat, 0x20);

	union XBoneInfoRadius
	{
		float radiusSquared;
		unsigned int radiusSquaredAsInt;
	};

	struct XBoneInfo
	{
		Bounds bounds;
		XBoneInfoRadius radius;
	}; assert_sizeof(XBoneInfo, 0x1C);

	struct ReactiveMotionWindParams
	{
		float windAreaScale;
		float windAmplitudeScale;
		float windFrequencyScale;
		float unused0;
	};

	struct ReactiveMotionModelPartTweaks
	{
		vec4_t ampScale;
		vec4_t freqScale;
	}; assert_sizeof(ReactiveMotionModelPartTweaks, 0x20);

	struct ReactiveMotionModelPart
	{
		vec3_t centerMS;
		unsigned short stiffness;
		unsigned short parentIdx;
		vec3_t pivotPosMS;
		unsigned int pivotQuatMS;
	}; assert_sizeof(ReactiveMotionModelPart, 0x20);

	struct ReactiveMotionModelInfo
	{
		ReactiveMotionWindParams windParams;
		ReactiveMotionModelPartTweaks* modelPartTweaks;
		ReactiveMotionModelPart* modelParts;
		char __pad0[0x30];
		unsigned char numParts;
		unsigned char flags;
		char pad[2];
		char __pad1[0xC];
	}; assert_sizeof(ReactiveMotionModelInfo, 0x60);
	assert_offsetof(ReactiveMotionModelInfo, numParts, 0x50);

	struct XModelLodInfo
	{
		XModelSurfs* modelSurfsStaging;
		XSurface* surfs;
		float dist;
		float unknownDist;
		unsigned short numsurfs;
		unsigned short surfIndex;
		DObjPartBits partBits;
		char unknown[4];
	}; assert_sizeof(XModelLodInfo, 0x40);

	struct BlendShapeWeightMap
	{
		unsigned short weightIndex;
		unsigned short targetIndex;
		float fullWeight;
	};

	struct XModelBlendShapeInfo
	{
		unsigned short targetCount;
		unsigned short numberOfWeights;
		unsigned short numberOfWeightMaps;
		scr_string_t* weightNames;
		BlendShapeWeightMap* weightMaps;
	}; assert_sizeof(XModelBlendShapeInfo, 0x18);

	struct ExtentBounds
	{
		vec3_t mins;
		vec3_t maxs;
	};

	struct MdaoVolume
	{
		ExtentBounds bounds;
		unsigned short cellCount[3];
		unsigned short parentBoneIndex;
		GfxImage* volumeData;
	}; assert_sizeof(MdaoVolume, 0x28);

	struct GfxDecalVolumeObb
	{
		vec3_t center;
		vec3_t axes[3];
		vec3_t halfSize;
	}; assert_sizeof(GfxDecalVolumeObb, 0x3C);

	struct XModelDecalVolume
	{
		GfxDecalVolumeObb obb;
		Material* material;
		GfxDecalVolumeMask* blendMapOverride;
		int boneIndex;
		vec2_t uvOffset;
		vec4_t uvMatrix;
	}; assert_sizeof(XModelDecalVolume, 0x70);

	struct XModelDecalVolumesInfo
	{
		XModelDecalVolume* decalVolumes;
		unsigned int numDecalVolumes;
	}; assert_sizeof(XModelDecalVolumesInfo, 0x10);

	enum XModelFlags : std::uint32_t
	{
		XMODEL_FLAG_SKINNED = 0x40000,
		XMODEL_FLAG_RIGID_VERT_LIST_CHILDREN = 0x80000,
		XMODEL_FLAG_STREAMED_IMAGES = 0x2000000,
	};

	enum XModelPhysicsType : std::uint8_t
	{
		XMODEL_PHYSICS_RAGDOLL = 4,
	};

	struct XModel
	{
		const char* name;
		unsigned short numsurfs;
		unsigned char numLods;
		unsigned char unknown[3];
		unsigned short mdaoVolumeCount;
		unsigned char unknownArrayCount;
		unsigned char unknown2;
		XModelPhysicsType physicsType;
		unsigned char unknown3;
		unsigned char numAimAssistBones;
		unsigned char unknown4[2];
		unsigned char numClothAssets;
		unsigned char numBones;
		unsigned char numRootBones;
		unsigned short numClientBones;
		unsigned int flags;
		int contents;
		float scale;
		float radius;
		Bounds bounds;
		float edgeLength;
		unsigned int lgvData;
		XModelPhysicsUsageCounter physicsUsageCounter;
		unsigned int* noScalePartBits;
		ScriptableDef* scriptableMoverDef;
		XAnimProceduralBones* proceduralBones;
		XAnimDynamicBones* dynamicBones;
		scr_string_t* aimAssistBones;
		scr_string_t* boneNames;
		unsigned short* parentList;
		short* quats;
		float* trans;
		unsigned char* partClassification;
		DObjAnimMat* baseMat;
		vec3_t* ikHingeAxis;
		ReactiveMotionModelInfo* reactiveMotionInfo;
		Material** materialHandles;
		XModelLodInfo* lodInfo;
		unsigned long long* unknownArray;
		XBoneInfo* boneInfo;
		float* himipRadiusInvSq;
		union
		{
			PhysicsAsset* physicsAsset;
			RagdollAsset* ragdollAsset;
		};
		PhysicsFXShape* physicsFXShape;
		XModelDetailCollision* detailCollision;
		PhysicsBoneGraph* physicsBoneGraph;
		ClothAsset** clothAssets;
		XModelBlendShapeInfo* blendShapeInfo;
		char unknown5[0x8];
		MdaoVolume* mdaoVolumes;
		XModelDecalVolumesInfo* decalVolumesInfo;
		unsigned int unknown6;
		scr_string_t unknownStrings[21];
	}; assert_sizeof(XModel, 0x188);
	assert_offsetof(XModel, physicsType, 0x12);
	assert_offsetof(XModel, numClientBones, 0x1A);
	assert_offsetof(XModel, noScalePartBits, 0x58);
	assert_offsetof(XModel, lodInfo, 0xC8);
	assert_offsetof(XModel, physicsBoneGraph, 0x100);
	assert_offsetof(XModel, mdaoVolumes, 0x120);
	assert_offsetof(XModel, unknownStrings, 0x134);

	struct FxParticleSimAnimation;
	struct VectorField;
	struct MayhemData;
	struct ParticleEmitterDef;
	struct ParticleCurveControlPointDef;
	struct ParticleStateDef;
	struct ParticleModuleGroupDef;
	struct ParticleModuleDef;
	union ParticleLinkedAssetDef;
	struct ParticleSpawnMeshAssetDef;

	enum ParticlePhaseOptions : std::int32_t
	{
		PARTICLE_PHASE_OPTION_PHASE_NEVER = 0x0,
		PARTICLE_PHASE_OPTION_PHASE_ONLY = 0x1,
		PARTICLE_PHASE_OPTION_ALWAYS = 0x2,
		PARTICLE_PHASE_OPTION_COUNT = 0x3,
	};

	struct alignas(16) float4
	{
		float v[4];
	}; assert_sizeof(float4, 0x10);

	struct ParticleSystemDef
	{
		const char* name;
		ParticleEmitterDef* emitterDefs;
		int version;
		int numEmitters;
		unsigned int flags;
		int occlusionOverrideEmitterIndex;
		ParticlePhaseOptions phaseOptions;
		float drawFrustumCullRadius;
		float updateFrustumCullRadius;
		float sunDistance;
		int preRollMSec;
		unsigned short unknown;
		unsigned short numScrStrings;
		scr_string_t* scrStrings;
	}; assert_sizeof(ParticleSystemDef, 0x40);
	assert_offsetof(ParticleSystemDef, numEmitters, 0x14);
	assert_offsetof(ParticleSystemDef, numScrStrings, 0x36);

	struct ParticleFloatRange
	{
		float min;
		float max;
	}; assert_sizeof(ParticleFloatRange, 0x8);

	struct ParticleIntRange
	{
		int min;
		int max;
	}; assert_sizeof(ParticleIntRange, 0x8);

	struct ParticleCurveDef
	{
		ParticleCurveControlPointDef* controlPoints;
		int numControlPoints;
		float scale;
	}; assert_sizeof(ParticleCurveDef, 0x10);

	enum ParticleGravityOptions : std::int32_t
	{
		PARTICLE_GRAVITY_OPTION_NONE = 0x0,
		PARTICLE_GRAVITY_OPTION_GRAVITY_ONLY = 0x1,
		PARTICLE_GRAVITY_OPTION_GRAVITY_NEVER = 0x2,
		PARTICLE_GRAVITY_OPTION_COUNT = 0x3,
	};

	enum ParticleInstancePoolAction : std::int32_t
	{
		PARTICLE_INSTANCE_POOL_ACTION_NO_SPAWN = 0x0,
		PARTICLE_INSTANCE_POOL_ACTION_KILL_OLDEST_AND_SPAWN = 0x1,
		PARTICLE_INSTANCE_POOL_ACTION_COUNT = 0x2,
	};

	enum ParticleDataFlags : std::int32_t
	{
		USE_NONE = 0x0,
		USE_POSITION = 0x1,
		USE_VELOCITY = 0x2,
		USE_COLOR = 0x4,
		USE_ROTATION_ANGLE = 0x8,
		USE_ROTATION_RATE = 0x10,
		USE_SPAWN_POS = 0x20,
		USE_SPAWN_QUAT = 0x40,
		USE_SIZE = 0x80,
		USE_EMISSIVE = 0x100,
		USE_PARENT_VELOCITY = 0x200,
		USE_CHILD_SYSTEM = 0x400,
		USE_SPAWN_TIME = 0x800,
		USE_LIFE = 0x1000,
		USE_STATE = 0x2000,
		USE_RANDOM_SEED = 0x4000,
		USE_FLAGS = 0x8000,
		USE_MODULE_TESTS = 0x10000,
		USE_HANDLE = 0x20000,
		USE_INTENSITY = 0x40000,
		USE_TEMPERATURE = 0x80000,
		USE_CAMERA_DISTANCE = 0x100000,
		USE_INHERIT_PERCENTAGE = 0x200000,
		USE_RAY_CAST_HANDLE = 0x400000,
		USE_ATLAS_INDEX = 0x800000,
		USE_VECTOR_FIELD_SCALE = 0x1000000,
		USE_LIGHTING_FRAC = 0x2000000,
		USE_PREV_PLACEMENT = 0x4000000,
		USE_SHADER_PARAMS = 0x8000000,
		CHECK_FIELD_COUNT = 0x8000001,
		USE_ALL = 0xFFFFFFF,
		USE_BASE = 0x192F8FF,
		USE_EMISSIVE_CURVES = 0x2040100,
	};

	struct ParticleEmitterDef
	{
		ParticleStateDef* stateDefs;
		int numStates;
		ParticleFloatRange particleSpawnRate;
		ParticleFloatRange particleLife;
		ParticleFloatRange particleDelay;
		unsigned int particleCountMax;
		ParticleIntRange particleBurstCount;
		ParticleFloatRange emitterLife;
		ParticleFloatRange emitterDelay;
		int randomSeed;
		ParticleFloatRange spawnRangeSq;
		float fadeOutMaxDistance;
		ParticleCurveDef fadeCurveDef;
		float spawnFrustumCullRadius;
		unsigned int unknown;
		unsigned int flags;
		unsigned char unknown2;
		unsigned char gravityOptions;
		unsigned char instanceAction;
		unsigned char instancePool;
		unsigned int groupIDs[4];
		ParticleFloatRange emitByDistanceDensity;
		unsigned int m_dataFlags;
		unsigned int unknown3[2];
		unsigned int soloInstanceMax;
		unsigned int unknown4[2];
	}; assert_sizeof(ParticleEmitterDef, 0xA0);
	assert_offsetof(ParticleEmitterDef, fadeCurveDef, 0x50);

	enum ParticleElementType : std::uint32_t
	{
		PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE = 0x0,
		PARTICLE_ELEMENT_TYPE_BEAM = 0x1,
		PARTICLE_ELEMENT_TYPE_DECAL = 0x2,
		PARTICLE_ELEMENT_TYPE_FLARE = 0x3,
		PARTICLE_ELEMENT_TYPE_GEO_TRAIL = 0x4,
		PARTICLE_ELEMENT_TYPE_LIGHT_OMNI = 0x5,
		PARTICLE_ELEMENT_TYPE_LIGHT_SPOT = 0x6,
		PARTICLE_ELEMENT_TYPE_MODEL = 0x7,
		PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE = 0x8,
		PARTICLE_ELEMENT_TYPE_RUNNER = 0x9,
		PARTICLE_ELEMENT_TYPE_TAIL = 0xA,
		PARTICLE_ELEMENT_TYPE_VECTOR_FIELD = 0xB,
		PARTICLE_ELEMENT_TYPE_VOLUMETRIC = 0xC,
	};

	enum ParticleStateDefFlags : std::uint64_t
	{
		PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE = 0x4,
		PARTICLE_STATE_DEF_FLAG_HAS_EMISSIVE = 0x400000,
		PARTICLE_STATE_DEF_FLAG_HAS_INTENSITY = 0x800000,
		PARTICLE_STATE_DEF_FLAG_PLAY_SOUNDS = 0x8000000,
		PARTICLE_STATE_DEF_FLAG_HAS_LIGHTING_FRACTION = 0x40000000000,
		PARTICLE_STATE_DEF_FLAG_HAS_SIZE_CURVE = 0x8000000000,
	};

	struct ParticleStateDef
	{
		ParticleModuleGroupDef* moduleGroupDefs;
		unsigned int elementType;
		unsigned long long flags;
		unsigned int pad[2];
	}; assert_sizeof(ParticleStateDef, 0x20);

	struct ParticleModuleGroupDef
	{
		ParticleModuleDef* moduleDefs;
		int numModules;
		bool disabled;
	}; assert_sizeof(ParticleModuleGroupDef, 0x10);

	enum PARTICLE_MODULE_FLAG : std::uint32_t
	{
		PARTICLE_MODULE_FLAG_UNKNOWN_200 = 0x200,
		PARTICLE_MODULE_FLAG_USE_OCCLUSION_QUERY_WORLD_SIZE = 0x4000,
	};

	struct ParticleModule
	{
		unsigned int m_flags;
	}; assert_sizeof(ParticleModule, 0x4);

	struct ParticleModuleInitAtlas : ParticleModule
	{
		int m_startFrame;
		int m_loopCount;
		bool m_randomIndex;
		bool m_playOverLife;
		unsigned char unknown[2];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleInitAtlas, 0x30);

	struct ParticleModuleInitAttributes : ParticleModule
	{
		bool m_useNonUniformInterpolationForColor;
		bool m_useNonUniformInterpolationForSize;
		unsigned char unknown[10];
		float4 m_sizeMin;
		float4 m_sizeMax;
		float4 m_colorMin;
		float4 m_colorMax;
		float4 m_velocityMin;
		float4 m_velocityMax;
	}; assert_sizeof(ParticleModuleInitAttributes, 0x70);

	enum ParticleModuleInitBeamFlags : std::int32_t
	{
		PARTICLE_MODULE_INIT_BEAM_FLAG_TILE = 0x1,
		PARTICLE_MODULE_INIT_BEAM_FLAG_USE_CURVE_POINTS = 0x2,
		PARTICLE_MODULE_INIT_BEAM_FLAG_CAMERA_FACING = 0x4,
	};

	struct ParticleLinkedAssetListDef
	{
		ParticleLinkedAssetDef* assetList;
		int numAssets;
	}; assert_sizeof(ParticleLinkedAssetListDef, 0x10);

	struct ParticleModuleInitBeam : ParticleModule
	{
		ParticleModuleInitBeamFlags m_beamFlags;
		ParticleLinkedAssetListDef m_linkedAssetList;
		float m_splitDistance;
		float m_scrollRateUV;
		unsigned int unknown[8];
		float4 m_endPos;
		unsigned int unknown1[4];
		float4 m_curvePoint1;
		unsigned int unknown2[4];
	}; assert_sizeof(ParticleModuleInitBeam, 0x80);

	struct ParticleModuleInitCameraOffset : ParticleModule
	{
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleInitCameraOffset, 0x10);

	struct ParticleModuleInitCloud : ParticleModule
	{
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleInitCloud, 0x10);

	struct ParticleModuleInitDecal : ParticleModule
	{
		unsigned short m_fadeInTime;
		unsigned short m_fadeOutTime;
		unsigned short m_stoppableFadeOutTime;
		unsigned short m_lerpWaitTime;
		unsigned short m_lerpTime;
		bool m_dynamicDecal;
		unsigned char m_projectionAxis;
		ParticleLinkedAssetListDef m_linkedAssetList;
		bool m_bypassStackingLimiter;
		unsigned char unknown[15];
		float4 m_lerpColor;
	}; assert_sizeof(ParticleModuleInitDecal, 0x40);

	struct ParticleModuleInitFlare : ParticleModule
	{
		float m_position;
		unsigned int unknown;
		unsigned int m_flareFlags;
		float4 m_direction;
		ParticleFloatRange m_depthScaleRange;
		ParticleFloatRange m_depthScaleValue;
		ParticleFloatRange m_radialRot;
		ParticleFloatRange m_radialScaleX;
		ParticleFloatRange m_radialScaleY;
		unsigned int unknown1[2];
		ParticleCurveDef m_curves[4];
	}; assert_sizeof(ParticleModuleInitFlare, 0x90);

	struct ParticleModuleInitGeoTrail : ParticleModule
	{
		unsigned int m_numPointsMax;
		float m_splitDistance;
		float m_splitAngle;
		float m_centerOffset;
		unsigned int m_numSheets;
		float m_fadeInDistance;
		float m_fadeOutDistance;
		float m_tileDistance;
		ParticleFloatRange m_tileOffset;
		float m_scrollTime;
		bool m_useLocalVelocity;
		bool m_useVerticalTexture;
		bool m_cameraFacing;
		bool m_fixLeadingEdge;
		bool m_clampUVs;
		unsigned char unknown[11];
	}; assert_sizeof(ParticleModuleInitGeoTrail, 0x40);

	struct ParticleModuleInitKillWrapBox : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_bounds;
		float4 m_offset;
	}; assert_sizeof(ParticleModuleInitKillWrapBox, 0x30);

	struct ParticleModuleInitLightOmni : ParticleModule
	{
		bool m_disableVolumetric;
		bool m_disableShadowMap;
		bool m_disableDynamicShadows;
		bool m_scriptScale;
		float m_toneMappingScaleFactor;
		unsigned int unknown;
		ParticleLinkedAssetListDef m_linkedAssetList;
		float m_fovOuter;
		float m_fovInner;
		float m_bulbRadius;
		float m_bulbLength;
		float m_distanceFalloff;
		float m_brightness;
		float m_intensityUV;
		float m_intensityIR;
		float m_intensityHeat;
		unsigned int unknown1[2];
		float m_shadowBias;
	}; assert_sizeof(ParticleModuleInitLightOmni, 0x50);

	struct ParticleModuleInitLightSpot : ParticleModule
	{
		bool m_disableVolumetric;
		bool m_disableShadowMap;
		bool m_disableDynamicShadows;
		bool m_scriptScale;
		ParticleLinkedAssetListDef m_linkedAssetList;
		float m_fovOuter;
		float m_fovInner;
		float m_bulbRadius;
		float m_bulbLength;
		float m_distanceFalloff;
		float m_fovCollimation;
		float m_brightness;
		float m_intensityUV;
		float m_intensityIR;
		unsigned int unknown[3];
		float m_shadowBias;
		float m_shadowNearPlane;
		float m_toneMappingScaleFactor;
		unsigned int unknown1[3];
	}; assert_sizeof(ParticleModuleInitLightSpot, 0x60);

	enum ParticleRenderOptions : std::int32_t
	{
		PARTICLE_RENDER_OPTION_NONE = 0x0,
		PARTICLE_RENDER_OPTION_THERMAL_VIEW_ONLY = 0x1,
		PARTICLE_RENDER_OPTION_THERMAL_VIEW_NEVER = 0x2,
		PARTICLE_RENDER_OPTION_COUNT = 0x3,
	};

	enum ParticleShaderGraphOptions : std::int32_t
	{
		PARTICLE_SHADER_GRAPH_NONE = 0x0,
		PARTICLE_SHADER_GRAPH_DEPTHSCAN = 0x1,
		PARTICLE_SHADER_GRAPH_SPHERICALZ = 0x2,
		PARTICLE_SHADER_GRAPH_OPTION_COUNT = 0x3,
	};

	struct Particle_MaterialData
	{
		vec4_t falloffParms;
		vec4_t opaqueColor;
		vec4_t transparentColor;
		vec2_t translucentShadowParams;
		vec2_t beamParms;
		vec4_t eyeOffsetParms;
		vec4_t alphaDissolveParms;
		vec4_t featherParms0;
		vec4_t featherParms1;
		vec4_t regionHighlightParms;
		vec4_t depthScanColor;
		vec4_t depthScanOutlineColor;
		vec2_t thermalParams;
		float anisotropy;
		unsigned int featureEnabledFlags;
	}; assert_sizeof(Particle_MaterialData, 0xC0);

	struct ParticleModuleInitMaterial : ParticleModule
	{
		ParticleRenderOptions m_renderOptions;
		ParticleShaderGraphOptions m_shaderGraphOptions;
		unsigned int unknown;
		ParticleLinkedAssetListDef m_linkedAssetList;
		Particle_MaterialData m_materialData;
	}; assert_sizeof(ParticleModuleInitMaterial, 0xE0);

	enum ParticleMirrorTextureType : std::int32_t
	{
		PARTICLE_MIRROR_TEXTURE_TYPE_NONE = 0x0,
		PARTICLE_MIRROR_TEXTURE_TYPE_STANDARD = 0x1,
		PARTICLE_MIRROR_TEXTURE_TYPE_RANDOM = 0x2,
		PARTICLE_MIRROR_TEXTURE_TYPE_COUNT = 0x3,
	};

	struct ParticleModuleInitMirrorTexture : ParticleModule
	{
		ParticleMirrorTextureType m_mirrorHorizontal;
		ParticleMirrorTextureType m_mirrorVertical;
		unsigned int unknown;
	}; assert_sizeof(ParticleModuleInitMirrorTexture, 0x10);

	struct ParticleModuleInitModel : ParticleModule
	{
		unsigned int unknown[3];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitModel, 0x20);

	struct ParticleModuleInitOcclusionQuery : ParticleModule
	{
		float m_depthBias;
		float m_fadeInTime;
		float m_fadeOutTime;
		ParticleFloatRange m_scale;
		vec2_t m_worldSize;
		unsigned int unknown[4];
	}; assert_sizeof(ParticleModuleInitOcclusionQuery, 0x30);

	struct ParticleModuleInitOrientedSprite : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_orientationQuat;
	}; assert_sizeof(ParticleModuleInitOrientedSprite, 0x20);

	struct ParticleModuleInitParticleSim : ParticleModule
	{
		unsigned int unknown[3];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitParticleSim, 0x20);

	struct ParticleModuleUnknown11 : ParticleModule
	{
		unsigned char unknown[0x2C];
	}; assert_sizeof(ParticleModuleUnknown11, 0x30);

	struct ParticleModuleInitPlayerFacing : ParticleModule
	{
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleInitPlayerFacing, 0x10);

	enum ParticleRelativeVelocityType : std::int32_t
	{
		PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL = 0x0,
		PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD = 0x1,
		PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN = 0x2,
		PARTICLE_RELATIVE_VELOCITY_TYPE_COUNT = 0x3,
		PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO = 0x3,
		PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO = 0x4,
		PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN_WITH_BOLT_INFO = 0x5,
	};

	struct ParticleModuleInitRelativeVelocity : ParticleModule
	{
		ParticleRelativeVelocityType m_velocityType;
		bool m_useBoltInfo;
		unsigned char unknown[7];
	}; assert_sizeof(ParticleModuleInitRelativeVelocity, 0x10);

	struct ParticleModuleInitRotation : ParticleModule
	{
		ParticleFloatRange m_rotationAngle;
		ParticleFloatRange m_rotationRate;
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleInitRotation, 0x20);

	struct ParticleModuleInitRotation3D : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_rotationAngleMin;
		float4 m_rotationAngleMax;
		float4 m_rotationRateMin;
		float4 m_rotationRateMax;
	}; assert_sizeof(ParticleModuleInitRotation3D, 0x50);

	struct ParticleModifier
	{
		float4 min;
		float4 max;
	}; assert_sizeof(ParticleModifier, 0x20);

	struct ParticleModuleInitRunner : ParticleModule
	{
		bool m_attachToParent;
		bool m_stopChildOnDeath;
		bool m_killChildOnDeath;
		bool m_legacyOrientationVelocity;
		bool m_legacyOrientationRotation;
		unsigned char unknown;
		unsigned char m_useOrientationOptions;
		unsigned char m_useScaleOptions;
		unsigned char m_useVelocityOptions;
		unsigned char unknown1[3];
		ParticleModifier m_scaleModifier;
		ParticleModifier m_velocityModifier;
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitRunner, 0x60);

	struct ParticleModuleInitSound : ParticleModule
	{
		unsigned int unknown[3];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitSound, 0x20);

	struct ParticleModuleInitSpawn : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[1];
	}; assert_sizeof(ParticleModuleInitSpawn, 0x20);

	struct ParticleModuleInitSpawnShape : ParticleModule
	{
		unsigned char m_axisFlags;
		unsigned char m_spawnFlags;
		unsigned char m_normalAxis;
		unsigned char m_spawnType;
		float m_volumeCubeRoot;
		unsigned int unknown;
		float4 m_calculationOffset;
		float4 m_offset;
	}; assert_sizeof(ParticleModuleInitSpawnShape, 0x30);

	struct ParticleModuleInitSpawnShapeBox : ParticleModuleInitSpawnShape
	{
		float4 m_dimensionsMin;
		float4 m_dimensionsMax;
		ParticleCurveDef m_curves[6];
		unsigned int unknown[4];
	}; assert_sizeof(ParticleModuleInitSpawnShapeBox, 0xC0);
	assert_offsetof(ParticleModuleInitSpawnShapeBox, m_curves, 0x50);

	struct ParticleModuleInitSpawnShapeCylinder : ParticleModuleInitSpawnShape
	{
		bool m_hasRotation;
		bool m_rotateCalculatedOffset;
		unsigned char unknown[2];
		float m_halfHeight;
		ParticleFloatRange m_radius;
		float4 m_directionQuat;
		ParticleCurveDef m_curves[5];
	}; assert_sizeof(ParticleModuleInitSpawnShapeCylinder, 0xA0);

	struct ParticleModuleInitSpawnShapeEllipsoid : ParticleModuleInitSpawnShape
	{
		float4 m_radiusMin;
		float4 m_radiusMax;
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModuleInitSpawnShapeEllipsoid, 0xB0);

	struct ParticleModuleInitSpawnShapeMesh : ParticleModuleInitSpawnShape
	{
		ParticleLinkedAssetListDef m_linkedAssetList;
		ParticleSpawnMeshAssetDef* m_meshAssetData;
		unsigned int m_numMeshAssets;
		unsigned int unknown;
		ParticleCurveDef m_curves[3];
	}; assert_sizeof(ParticleModuleInitSpawnShapeMesh, 0x80);
	assert_offsetof(ParticleModuleInitSpawnShapeMesh, m_linkedAssetList, 0x30);

	struct ParticleModuleInitSpawnShapeSphere : ParticleModuleInitSpawnShape
	{
		ParticleFloatRange m_radius;
		unsigned int unknown[2];
		ParticleCurveDef m_curves[4];
	}; assert_sizeof(ParticleModuleInitSpawnShapeSphere, 0x80);

	struct ParticleModuleInitTail : ParticleModule
	{
		unsigned short m_averagePastVelocities;
		unsigned short m_maxParentSpeed;
		bool m_tailLeading;
		bool m_scaleWithVelocity;
		unsigned char unknown[22];
	}; assert_sizeof(ParticleModuleInitTail, 0x20);

	struct ParticleModuleInitVectorField : ParticleModule
	{
		unsigned int unknown[7];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitVectorField, 0x30);

	struct ParticleModuleInitVolumetric : ParticleModule
	{
		unsigned int unknown[7];
	}; assert_sizeof(ParticleModuleInitVolumetric, 0x20);

	struct ParticleModuleInitDismember : ParticleModule
	{
		unsigned int m_chance;
		unsigned int unknown[2];
	}; assert_sizeof(ParticleModuleInitDismember, 0x10);

	struct ParticleModuleUnknown22 : ParticleModule
	{
		unsigned char unknown[0x1C];
	}; assert_sizeof(ParticleModuleUnknown22, 0x20);

	struct ParticleModuleUnknown23 : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleUnknown23, 0x40);

	struct ParticleModuleUnknown24 : ParticleModule
	{
		unsigned char unknown[0x1C];
	}; assert_sizeof(ParticleModuleUnknown24, 0x20);

	struct ParticleModuleUnknown25 : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[8];
		unsigned char unknown2[0x20];
	}; assert_sizeof(ParticleModuleUnknown25, 0xB0);

	struct ParticleModuleColorGraph : ParticleModule
	{
		bool firstCurve;
		bool m_modulateColorByAlpha;
		unsigned char unknown[10];
		ParticleCurveDef m_curves[8];
	}; assert_sizeof(ParticleModuleColorGraph, 0x90);

	struct ParticleModuleColorLerp : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_colorBegin;
		float4 m_colorEnd;
	}; assert_sizeof(ParticleModuleColorLerp, 0x30);

	struct ParticleModuleEmissionGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleEmissionGraph, 0x30);

	struct ParticleModuleEmissiveGraph : ParticleModule
	{
		bool firstCurve;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[10];
	}; assert_sizeof(ParticleModuleEmissiveGraph, 0xB0);

	struct ParticleModuleShaderGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[12];
	}; assert_sizeof(ParticleModuleShaderGraph, 0xD0);

	struct ParticleModuleForce : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_forceMin;
		float4 m_forceMax;
	}; assert_sizeof(ParticleModuleForce, 0x30);

	struct ParticleModuleForceDragGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleForceDragGraph, 0x30);

	struct ParticleModuleGravity : ParticleModule
	{
		ParticleFloatRange m_gravityPercentage;
		unsigned int unknown;
	}; assert_sizeof(ParticleModuleGravity, 0x10);

	struct ParticleModuleIntensityGraph : ParticleModule
	{
		bool firstCurve;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleIntensityGraph, 0x30);

	struct ParticleModuleTemperatureGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleTemperatureGraph, 0x30);

	struct ParticleModuleParentVelocityGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleParentVelocityGraph, 0x30);

	struct ParticleModuleUnknown2C : ParticleModule
	{
		unsigned char unknown[0x2C];
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModuleUnknown2C, 0x90);

	struct ParticleModulePhysicsLight : ParticleModule
	{
		bool m_ignoreEmitterOrientation;
		bool m_useSurfaceType;
		unsigned char unknown[10];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModulePhysicsLight, 0x20);

	struct ParticleModulePhysicsRayCast : ParticleModule
	{
		ParticleFloatRange m_bounce;
		Bounds m_bounds;
		bool m_useItemClip;
		bool m_useSurfaceType;
		bool m_collideWithWater;
		bool m_ignoreContentItem;
		unsigned int unknown[2];
	}; assert_sizeof(ParticleModulePhysicsRayCast, 0x30);

	struct ParticleModulePositionGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModulePositionGraph, 0x70);

	struct ParticleModuleRotationGraph : ParticleModule
	{
		bool m_useRotationRate;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleRotationGraph, 0x30);

	struct ParticleModuleRotationGraph3D : ParticleModule
	{
		bool m_useRotationRate;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModuleRotationGraph3D, 0x70);

	struct ParticleModuleScaleByDistance : ParticleModule
	{
		float m_maxDistance;
		unsigned int unknown[2];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleScaleByDistance, 0x30);

	struct ParticleModuleSizeGraph : ParticleModule
	{
		bool firstCurve;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[6];
		float4 m_sizeBegin;
		float4 m_sizeEnd;
	}; assert_sizeof(ParticleModuleSizeGraph, 0x90);

	struct ParticleModuleSizeLerp : ParticleModule
	{
		unsigned int unknown[3];
		float4 m_sizeBegin;
		float4 m_sizeEnd;
	}; assert_sizeof(ParticleModuleSizeLerp, 0x30);

	struct ParticleModuleVelocityGraph : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[6];
		float4 m_velocityBegin;
		float4 m_velocityEnd;
	}; assert_sizeof(ParticleModuleVelocityGraph, 0x90);

	struct ParticleModuleVectorFieldGraph : ParticleModule
	{
		bool firstCurve;
		unsigned char unknown[11];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleVectorFieldGraph, 0x30);

	struct ParticleModuleUnknown3C : ParticleModule
	{
		unsigned int unknown[3];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleUnknown3C, 0x30);

	struct ParticleModuleTestEventHandlerData
	{
		ParticleLinkedAssetListDef m_linkedAssetList;
		unsigned int m_nextState;
		bool m_kill;
		unsigned int unknown[2];
	}; assert_sizeof(ParticleModuleTestEventHandlerData, 0x20);

	struct ParticleModuleTest : ParticleModule
	{
		unsigned short m_moduleIndex;
		unsigned char m_useOrientationOptions;
		unsigned char m_useScaleOptions;
		unsigned char m_useVelocityOptions;
		unsigned char unknown[7];
		ParticleModifier m_scaleModifier;
		ParticleModifier m_velocityModifier;
		ParticleModuleTestEventHandlerData m_eventHandlerData;
	}; assert_sizeof(ParticleModuleTest, 0x70);
	assert_offsetof(ParticleModuleTest, m_eventHandlerData, 0x50);

	struct ParticleModuleTestAge : ParticleModuleTest
	{
		float m_age;
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleTestAge, 0x80);

	struct ParticleModuleTestBirth : ParticleModuleTest
	{
	}; assert_sizeof(ParticleModuleTestBirth, 0x70);

	struct ParticleModuleTestDeath : ParticleModuleTest
	{
	}; assert_sizeof(ParticleModuleTestDeath, 0x70);

	struct ParticleModuleTestGravity : ParticleModuleTest
	{
		unsigned int unknown[4];
	}; assert_sizeof(ParticleModuleTestGravity, 0x80);

	struct ParticleModuleTestImpact : ParticleModuleTest
	{
		unsigned int m_impactDirection;
		unsigned int unknown[3];
	}; assert_sizeof(ParticleModuleTestImpact, 0x80);

	struct ParticleModuleTestPos : ParticleModuleTest
	{
		unsigned int unknown[4];
	}; assert_sizeof(ParticleModuleTestPos, 0x80);

	struct ParticleModuleTestTimeInState : ParticleModuleTest
	{
		ParticleFloatRange m_timeInState;
		unsigned int unknown[2];
	}; assert_sizeof(ParticleModuleTestTimeInState, 0x80);

	union ParticleModuleTypeDef
	{
		ParticleModule moduleBase;
		ParticleModuleInitAtlas initAtlas;
		ParticleModuleInitAttributes initAttributes;
		ParticleModuleInitBeam initBeam;
		ParticleModuleInitCameraOffset initCameraOffset;
		ParticleModuleInitCloud initCloud;
		ParticleModuleInitDecal initDecal;
		ParticleModuleInitFlare initFlare;
		ParticleModuleInitGeoTrail initGeoTrail;
		ParticleModuleInitKillWrapBox initKillWrapBox;
		ParticleModuleInitLightOmni initLightOmni;
		ParticleModuleInitLightSpot initLightSpot;
		ParticleModuleInitMaterial initMaterial;
		ParticleModuleInitMirrorTexture initMirrorTexture;
		ParticleModuleInitModel initModel;
		ParticleModuleInitOcclusionQuery initOcclusionQuery;
		ParticleModuleInitOrientedSprite initOrientedSprite;
		ParticleModuleInitParticleSim initParticleSim;
		ParticleModuleUnknown11 unknown11;
		ParticleModuleInitPlayerFacing initPlayerFacing;
		ParticleModuleInitRelativeVelocity initRelativeVelocity;
		ParticleModuleInitRotation initRotation;
		ParticleModuleInitRotation3D initRotation3D;
		ParticleModuleInitRunner initRunner;
		ParticleModuleInitSound initSound;
		ParticleModuleInitSpawn initSpawn;
		ParticleModuleInitSpawnShapeBox initSpawnShapeBox;
		ParticleModuleInitSpawnShapeCylinder initSpawnShapeCylinder;
		ParticleModuleInitSpawnShapeEllipsoid initSpawnShapeEllipsoid;
		ParticleModuleInitSpawnShapeMesh initSpawnShapeMesh;
		ParticleModuleInitSpawnShapeSphere initSpawnShapeSphere;
		ParticleModuleInitTail initTail;
		ParticleModuleInitVectorField initVectorField;
		ParticleModuleInitVolumetric initVolumetric;
		ParticleModuleInitDismember initDismember;
		ParticleModuleUnknown22 unknown22;
		ParticleModuleUnknown23 unknown23;
		ParticleModuleUnknown24 unknown24;
		ParticleModuleUnknown25 unknown25;
		ParticleModuleColorGraph colorGraph;
		ParticleModuleColorLerp colorLerp;
		ParticleModuleEmissionGraph emissionGraph;
		ParticleModuleEmissiveGraph emissiveGraph;
		ParticleModuleShaderGraph shaderGraph;
		ParticleModuleForce force;
		ParticleModuleForceDragGraph forceDragGraph;
		ParticleModuleGravity gravity;
		ParticleModuleIntensityGraph intensityGraph;
		ParticleModuleTemperatureGraph temperatureGraph;
		ParticleModuleParentVelocityGraph parentVelocityGraph;
		ParticleModuleUnknown2C unknown2C;
		ParticleModulePhysicsLight physicsLight;
		ParticleModulePhysicsRayCast physicsRayCast;
		ParticleModulePositionGraph positionGraph;
		ParticleModuleRotationGraph rotationGraph;
		ParticleModuleRotationGraph3D rotationGraph3D;
		ParticleModuleScaleByDistance scaleByDistance;
		ParticleModuleSizeGraph sizeGraph;
		ParticleModuleSizeLerp sizeLerp;
		ParticleModuleVelocityGraph velocityGraph;
		ParticleModuleVectorFieldGraph vectorFieldGraph;
		ParticleModuleUnknown3C unknown3C;
		ParticleModuleTestAge testAge;
		ParticleModuleTestBirth testBirth;
		ParticleModuleTestDeath testDeath;
		ParticleModuleTestGravity testGravity;
		ParticleModuleTestImpact testImpact;
		ParticleModuleTestPos testPos;
		ParticleModuleTestTimeInState testTimeInState;
	}; assert_sizeof(ParticleModuleTypeDef, 0xE0);

	enum ParticleModuleType : std::uint16_t
	{
		PARTICLE_MODULE_INIT_ATLAS = 0x0,
		PARTICLE_MODULE_INIT_ATTRIBUTES = 0x1,
		PARTICLE_MODULE_INIT_BEAM = 0x2,
		PARTICLE_MODULE_INIT_CAMERA_OFFSET = 0x3,
		PARTICLE_MODULE_INIT_CLOUD = 0x4,
		PARTICLE_MODULE_INIT_DECAL = 0x5,
		PARTICLE_MODULE_INIT_FLARE = 0x6,
		PARTICLE_MODULE_INIT_GEO_TRAIL = 0x7,
		PARTICLE_MODULE_INIT_KILL_WRAP_BOX = 0x8,
		PARTICLE_MODULE_INIT_LIGHT_OMNI = 0x9,
		PARTICLE_MODULE_INIT_LIGHT_SPOT = 0xA,
		PARTICLE_MODULE_INIT_MATERIAL = 0xB,
		PARTICLE_MODULE_INIT_MIRROR_TEXTURE = 0xC,
		PARTICLE_MODULE_INIT_MODEL = 0xD,
		PARTICLE_MODULE_INIT_OCCLUSION_QUERY = 0xE,
		PARTICLE_MODULE_INIT_ORIENTED_SPRITE = 0xF,
		PARTICLE_MODULE_INIT_PARTICLE_SIM = 0x10,
		PARTICLE_MODULE_UNKNOWN_11 = 0x11,
		PARTICLE_MODULE_INIT_PLAYER_FACING = 0x12,
		PARTICLE_MODULE_INIT_RELATIVE_VELOCITY = 0x13,
		PARTICLE_MODULE_INIT_ROTATION = 0x14,
		PARTICLE_MODULE_INIT_ROTATION_3D = 0x15,
		PARTICLE_MODULE_INIT_RUNNER = 0x16,
		PARTICLE_MODULE_INIT_SOUND = 0x17,
		PARTICLE_MODULE_INIT_SPAWN = 0x18,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_FIRST = 0x19,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX = 0x19,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER = 0x1A,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID = 0x1B,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH = 0x1C,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE = 0x1D,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_LAST = 0x1D,
		PARTICLE_MODULE_INIT_TAIL = 0x1E,
		PARTICLE_MODULE_INIT_VECTOR_FIELD = 0x1F,
		PARTICLE_MODULE_INIT_VOLUMETRIC = 0x20,
		PARTICLE_MODULE_INIT_DISMEMBER = 0x21,
		PARTICLE_MODULE_UNKNOWN_22 = 0x22,
		PARTICLE_MODULE_UNKNOWN_23 = 0x23,
		PARTICLE_MODULE_UNKNOWN_24 = 0x24,
		PARTICLE_MODULE_UNKNOWN_25 = 0x25,
		PARTICLE_MODULE_COLOR_GRAPH = 0x26,
		PARTICLE_MODULE_COLOR_LERP = 0x27,
		PARTICLE_MODULE_EMISSION_GRAPH = 0x28,
		PARTICLE_MODULE_EMISSIVE_GRAPH = 0x29,
		PARTICLE_MODULE_SHADER_GRAPH = 0x2A,
		PARTICLE_MODULE_FORCE = 0x2B,
		PARTICLE_MODULE_UNKNOWN_2C = 0x2C,
		PARTICLE_MODULE_FORCE_DRAG_GRAPH = 0x2D,
		PARTICLE_MODULE_GRAVITY = 0x2E,
		PARTICLE_MODULE_INTENSITY_GRAPH = 0x2F,
		PARTICLE_MODULE_TEMPERATURE_GRAPH = 0x30,
		PARTICLE_MODULE_PARENT_VELOCITY_GRAPH = 0x31,
		PARTICLE_MODULE_PHYSICS_LIGHT = 0x32,
		PARTICLE_MODULE_PHYSICS_RAY_CAST = 0x33,
		PARTICLE_MODULE_POSITION_GRAPH = 0x34,
		PARTICLE_MODULE_ROTATION_GRAPH = 0x35,
		PARTICLE_MODULE_ROTATION_GRAPH_3D = 0x36,
		PARTICLE_MODULE_SCALE_BY_DISTANCE = 0x37,
		PARTICLE_MODULE_SIZE_GRAPH = 0x38,
		PARTICLE_MODULE_SIZE_LERP = 0x39,
		PARTICLE_MODULE_VELOCITY_GRAPH = 0x3A,
		PARTICLE_MODULE_VECTOR_FIELD_GRAPH = 0x3B,
		PARTICLE_MODULE_UNKNOWN_3C = 0x3C,
		PARTICLE_MODULE_TEST_BEGIN = 0x3D,
		PARTICLE_MODULE_TEST_AGE = 0x3D,
		PARTICLE_MODULE_TEST_BIRTH = 0x3E,
		PARTICLE_MODULE_TEST_DEATH = 0x3F,
		PARTICLE_MODULE_TEST_GRAVITY = 0x40,
		PARTICLE_MODULE_TEST_IMPACT = 0x41,
		PARTICLE_MODULE_TEST_POS = 0x42,
		PARTICLE_MODULE_TEST_TIME_IN_STATE = 0x43,
		PARTICLE_MODULE_TEST_END = 0x43,
		PARTICLE_MODULE_COUNT = 0x44,
	};

	struct ParticleModuleDef
	{
		ParticleModuleType moduleType;
		unsigned int m_pad[2];
		ParticleModuleTypeDef moduleData;
	}; assert_sizeof(ParticleModuleDef, 0xF0);
	assert_offsetof(ParticleModuleDef, moduleData, 0x10);

	struct ParticlePhysicsFXData
	{
		PhysicsFXPipeline* physicsFXPipeline;
		PhysicsFXShape* physicsFXShape;
	}; assert_sizeof(ParticlePhysicsFXData, 0x10);

	struct ParticleMarkVisuals
	{
		Material* materials[3];
	}; assert_sizeof(ParticleMarkVisuals, 0x18);

	union ParticleLinkedAssetDef
	{
		Material* material;
		XModel* model;
		const ParticleSystemDef* particleSystem;
		ParticlePhysicsFXData physicsFXData;
		FxParticleSimAnimation* particleSim;
		ParticleMarkVisuals decal;
		const char* sound;
		VectorField* vectorField;
		GfxLightDef* lightDef;
		MayhemData* mayhem;
		char pad[32];
	}; assert_sizeof(ParticleLinkedAssetDef, 0x20);

	struct ParticleCurveControlPointDef
	{
		float time;
		float value;
		float invTimeDelta;
		unsigned int pad[1];
	}; assert_sizeof(ParticleCurveControlPointDef, 0x10);

	struct ParticleSpawnMeshAssetDef
	{
		unsigned int m_numVertsTotal;
		unsigned int m_numTrisTotal;
		unsigned int m_pad[2];
	}; assert_sizeof(ParticleSpawnMeshAssetDef, 0x10);

	struct WeaponCompleteDef;
	struct RumbleInfo;

	struct MayhemModel
	{
		XModel* model;
	}; assert_sizeof(MayhemModel, 0x8);

	struct MayhemTransBounds
	{
		float mins[3];
		float size[3];
	}; assert_sizeof(MayhemTransBounds, 0x18);

	struct MayhemAnimFramesSplineCompressed
	{
		unsigned int quatStride;
		unsigned int posStride;
		unsigned int totalCompressedQuatFrames;
		unsigned int totalCompressedPosFrames;
		char* diskQuat;
		char* diskPos;
		unsigned short* diskQuatFrames;
		unsigned short* diskPosFrames;
		unsigned short* numDiskQuatFrames;
		unsigned short* numDiskPosFrames;
		MayhemTransBounds* transBounds;
	}; assert_sizeof(MayhemAnimFramesSplineCompressed, 0x48);

	struct MayhemAnimFramesUncompressed
	{
		unsigned int quatStride;
		unsigned int posStride;
		char* diskQuat;
		char* diskPos;
		MayhemTransBounds* transBounds;
	}; assert_sizeof(MayhemAnimFramesUncompressed, 0x20);

	struct MayhemDataKeysSplineCompressed
	{
		char* keys;
		unsigned short* numKeys;
		unsigned short* keyFrames;
		unsigned int totalCompressedKeyFrames;
		unsigned int numStreams;
		unsigned int keyStride;
	}; assert_sizeof(MayhemDataKeysSplineCompressed, 0x28);

	struct MayhemDataKeysUncompressed
	{
		char* keys;
		unsigned int numStreams;
		unsigned int keyStride;
	}; assert_sizeof(MayhemDataKeysUncompressed, 0x10);

	union MayhemFramesUnion
	{
		MayhemAnimFramesSplineCompressed* splineCompressedFrames;
		MayhemAnimFramesUncompressed* uncompressedFrames;
	};

	union MayhemDataChannelsUnion
	{
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
		char __pad0[0x10];
	}; assert_sizeof(MayhemAnim, 0x48);

	struct MayhemObject
	{
		unsigned int modelIndex;
		unsigned int animIndex;
	}; assert_sizeof(MayhemObject, 0x8);

	struct MayhemData
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
	}; assert_sizeof(MayhemData, 0x50);

	struct SoundBankListDef
	{
		const char* name;
		unsigned short soundBankCount;
		std::uint64_t* soundBankNames;
	}; assert_sizeof(SoundBankListDef, 0x18);

	struct FootstepVFX;
	struct XCompositeModelDef;
	struct ScriptableEventDef;
	struct ScriptablePartDef;

	struct XCompositeSubmodel
	{
		char* modelName;
		vec3_t translation;
		vec3_t angles;
		scr_string_t parentBoneName;
		int parentSubmodelSlot;
		XModel* model;
	}; assert_sizeof(XCompositeSubmodel, 0x30);

	struct XCompositeSubCompositeModel
	{
		char* compositeModelName;
		vec3_t translation;
		vec3_t angles;
		scr_string_t parentBoneName;
		int parentSubmodelSlot;
		XCompositeModelDef* compositeModel;
	}; assert_sizeof(XCompositeSubCompositeModel, 0x30);

	struct XCompositeModelDef
	{
		const char* name;
		XCompositeSubmodel* submodels;
		XCompositeSubCompositeModel* subCompositeModels;
	}; assert_sizeof(XCompositeModelDef, 0x18);

	struct FxCombinedDef
	{
		const ParticleSystemDef* particleSystemDef;
	};

	struct ScriptableEventBaseDef
	{
		const char* name;
		int flags;
	}; assert_sizeof(ScriptableEventBaseDef, 0x10);

	enum Scriptable_EventType : std::int32_t
	{
		Scriptable_EventType_StateChange = 0x0,
		Scriptable_EventType_Wait = 0x1,
		Scriptable_EventType_Unknown2 = 0x2,
		Scriptable_EventType_Unknown3 = 0x3,
		Scriptable_EventType_Random = 0x4,
		Scriptable_EventType_Script = 0x5,
		Scriptable_EventType_Model = 0x6,
		Scriptable_EventType_Collision = 0x7,
		Scriptable_EventType_Unknown8 = 0x8,
		Scriptable_EventType_Unknown9 = 0x9,
		Scriptable_EventType_Animation = 0xA,
		Scriptable_EventType_HideShowBone = 0xB,
		Scriptable_EventType_DisablePhysicsSubShape = 0xC,
		Scriptable_EventType_NoteTrack = 0xD,
		Scriptable_EventType_ChunkDynent = 0xE,
		Scriptable_EventType_SpawnDynent = 0xF,
		Scriptable_EventType_PFX = 0x10,
		Scriptable_EventType_Sound = 0x11,
		Scriptable_EventType_Explosion = 0x12,
		Scriptable_EventType_Light = 0x13,
		Scriptable_EventType_Sun = 0x14,
		Scriptable_EventType_Rumble = 0x15,
		Scriptable_EventType_Screenshake = 0x16,
		Scriptable_EventType_PartDamage = 0x17,
		Scriptable_EventType_SetMayhem = 0x18,
		Scriptable_EventType_PlayMayhem = 0x19,
		Scriptable_EventType_ViewmodelShaderParam = 0x1A,
		Scriptable_EventType_ViewmodelChangeImage = 0x1B,
		Scriptable_EventType_ClientViewSelector = 0x1C,
		Scriptable_EventType_TeamSelector = 0x1D,
		Scriptable_EventType_AddModel = 0x1E,
		Scriptable_EventType_ApplyForce = 0x1F,
		Scriptable_EventType_ApplyAngularForce = 0x20,
		Scriptable_EventType_ApplyConstantForce = 0x21,
		Scriptable_EventType_ApplyConstantAngularForce = 0x22,
		Scriptable_EventType_CompassIcon = 0x23,
		Scriptable_EventType_MaterialOverride = 0x24,
		Scriptable_EventType_DynamicBoneNoiseCurve = 0x25,
		Scriptable_EventType_Move = 0x26,
		Scriptable_EventType_Footstep = 0x27,
		Scriptable_EventType_Unknown28 = 0x28,
		Scriptable_EventType_GravityArc = 0x29,
		Scriptable_EventType_ViewTrigger = 0x2A,
		Scriptable_EventType_Objective = 0x2B,
		Scriptable_EventType_SpatialDisable = 0x2C,
		Scriptable_EventType_Hover = 0x2D,
		Scriptable_EventType_Unknown2E = 0x2E,
		Scriptable_EventType_Unknown2F = 0x2F,
		Scriptable_EventType_Unknown30 = 0x30,
		Scriptable_EventType_Unknown31 = 0x31,
		Scriptable_EventType_Unknown32 = 0x32,
		Scriptable_EventType_CalloutMarkerPing = 0x33,
		Scriptable_EventType_Unknown34 = 0x34,
		Scriptable_EventType_Unknown35 = 0x35,
		Scriptable_EventType_Unknown36 = 0x36,
		Scriptable_EventType_Count = 0x37,
	};

	enum Scriptable_MayhemAction : std::int32_t
	{
		Scriptable_MayhemAction_Play = 0x0,
		Scriptable_MayhemAction_Pause = 0x1,
		Scriptable_MayhemAction_Count = 0x2,
	};

	enum Scriptable_ShaderParamType : std::int32_t
	{
		Scriptable_ShaderParamType_Unknown = 0x0,
		Scriptable_ShaderParamType_ScrollRate_X = 0x1,
		Scriptable_ShaderParamType_ScrollRate_Y = 0x2,
		Scriptable_ShaderParamType_ScrollRate_R = 0x3,
		Scriptable_ShaderParamType_Rotation = 0x4,
		Scriptable_ShaderParamType_Tiling_X = 0x5,
		Scriptable_ShaderParamType_Tiling_Y = 0x6,
		Scriptable_ShaderParamType_Alpha = 0x7,
		Scriptable_ShaderParamType_Emissive = 0x8,
		Scriptable_ShaderParamType_AtlasFrame = 0x9,
		Scriptable_ShaderParamType_Placeholder1 = 0xA,
		Scriptable_ShaderParamType_Placeholder2 = 0xB,
		Scriptable_ShaderParamType_Placeholder3 = 0xC,
		Scriptable_ShaderParamType_Placeholder4 = 0xD,
		Scriptable_ShaderParamType_Count = 0xE,
	};

	enum Scriptable_TeamFilter : std::int32_t
	{
		Scriptable_Team_Filter_All = 0x0,
		Scriptable_Team_Filter_Bad = 0x1,
		Scriptable_Team_Filter_Axis = 0x2,
		Scriptable_Team_Filter_Allies = 0x3,
		Scriptable_Team_Filter_Three = 0x4,
		Scriptable_Team_Filter_Neutral = 0x5,
		Scriptable_Team_Filter_Dead = 0x6,
		Scriptable_Team_Filter_My_Team = 0x7,
		Scriptable_Team_Filter_Enemy_Team = 0x8,
		Scriptable_Team_Filter_All_But_My_Team = 0x9,
		Scriptable_Team_Filter_All_But_Enemy_Team = 0xA,
		Scriptable_Team_Filter_Enemy_Team_And_Me = 0xB,
		Scriptable_Team_Filter_Just_Me = 0xC,
		Scriptable_Team_Filter_Count = 0xD,
	};

	enum Scriptable_MaterialOverrideType : std::int32_t
	{
		Scriptable_MaterialOverrideType_Off = 0x0,
		Scriptable_MaterialOverrideType_Refraction = 0x1,
		Scriptable_MaterialOverrideType_Dissolve = 0x2,
		Scriptable_MaterialOverrideType_FlagAmplitudeScale = 0x3,
		Scriptable_MaterialOverrideType_Count = 0x4,
	};

	enum Scriptable_DamageCause : std::int32_t
	{
		Scriptable_DamageCause_All = 0x0,
		Scriptable_DamageCause_SplashOnly = 0x1,
		Scriptable_DamageCause_NoSplash = 0x2,
		Scriptable_DamageCause_NoMelee = 0x3,
		Scriptable_DamageCause_None = 0x4,
		Scriptable_DamageCause_MeleeOnly = 0x5,
		Scriptable_DamageCause_MeleeAndSplashOnly = 0x6,
		Scriptable_DamageCause_CrushOnly = 0x7,
		Scriptable_DamageCause_CrushAndSplashOnly = 0x8,
		Scriptable_DamageCause_Count = 0x9,
	};

	enum Scriptable_Attackers : std::int32_t
	{
		Scriptable_Attackers_All = 0x0,
		Scriptable_Attackers_NoPlayer = 0x1,
		Scriptable_Attackers_PlayerOnly = 0x2,
		Scriptable_Attackers_NoAI = 0x3,
		Scriptable_Attackers_AIOnly = 0x4,
		Scriptable_Attackers_None = 0x5,
		Scriptable_Attackers_Count = 0x6,
	};

	enum weapClass_t : std::int32_t
	{
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_SNIPER = 0x1,
		WEAPCLASS_MG = 0x2,
		WEAPCLASS_SMG = 0x3,
		WEAPCLASS_SPREAD = 0x4,
		WEAPCLASS_PISTOL = 0x5,
		WEAPCLASS_GRENADE = 0x6,
		WEAPCLASS_ROCKETLAUNCHER = 0x7,
		WEAPCLASS_TURRET = 0x8,
		WEAPCLASS_THROWINGKNIFE = 0x9,
		WEAPCLASS_NON_PLAYER = 0xA,
		WEAPCLASS_ITEM = 0xB,
		WEAPCLASS_BEAM = 0xC,
		WEAPCLASS_BALL = 0xD,
		WEAPCLASS_NONE = 0xE,
		WEAPCLASS_NUM = 0xF,
	};

	union ScriptablePartReference
	{
		unsigned int flatId;
		unsigned int partStringHash;
	};

	struct scr_anim_index
	{
		unsigned short index;
		unsigned short tree;
	};

	union scr_anim_t
	{
		scr_anim_index anim;
		unsigned long long linkPointer;
	};

	union ScriptableModelUnion
	{
		XCompositeModelDef* compositeModel;
		XModel* model;
	};

	enum ScriptableModelDataType : std::uint8_t
	{
		SCRIPTABLE_MODEL_DATA_MODEL = 0x1,
		SCRIPTABLE_MODEL_DATA_COMPOSITE_MODEL = 0x2,
	};

	struct ObjectiveSettings
	{
		unsigned char flags[2];
		unsigned char state;
		unsigned char background;
		int zOffset;
		bool cached;
		bool alwaysShowInWorld;
		bool alwaysSnapToScreenEdge;
		unsigned char size;
		unsigned int label;
		unsigned int description;
		unsigned int icon;
	}; assert_sizeof(ObjectiveSettings, 0x18);

	struct CurveSegment
	{
		unsigned char numPoints;
		vec2_t* points;
		unsigned int dataSize;
		unsigned char* data;
	}; assert_sizeof(CurveSegment, 0x20);

	struct Curve
	{
		const char* name;
		char __pad0[0x8];
		CurveSegment* segments;
		unsigned char numSegments;
	}; assert_sizeof(Curve, 0x20);

	struct CalloutMarkerPing
	{
		const char* name;
		char unknown[0x30];
	}; assert_sizeof(CalloutMarkerPing, 0x38);

	enum ScriptableConditionType : std::int32_t
	{
		SCRIPTABLE_CONDITION_GROUP = 0x2,
	};

	struct ScriptableCondition;

	struct ScriptableConditionGroup
	{
		int unknown;
		unsigned int numConditions;
		ScriptableCondition* conditions;
	}; assert_sizeof(ScriptableConditionGroup, 0x10);

	struct ScriptableCondition
	{
		ScriptableConditionType type;
		ScriptableConditionGroup group;
		char unknown[8];
	}; assert_sizeof(ScriptableCondition, 0x20);

	enum ScriptableExtraValueType : std::int32_t
	{
		SCRIPTABLE_EXTRA_VALUE_STRING = 0x3,
		SCRIPTABLE_EXTRA_VALUE_STRING_LIST = 0x4,
	};

	struct ScriptableExtraStringList
	{
		unsigned int count;
		const char** strings;
	}; assert_sizeof(ScriptableExtraStringList, 0x10);

	union ScriptableExtraValueData
	{
		const char* string;
		ScriptableExtraStringList strings;
	};

	struct ScriptableExtraValue
	{
		unsigned long long hash;
		ScriptableExtraValueType type;
		ScriptableExtraValueData data;
	}; assert_sizeof(ScriptableExtraValue, 0x20);

	struct ScriptableExtraValueList
	{
		unsigned int numValues;
		ScriptableExtraValue* values;
	}; assert_sizeof(ScriptableExtraValueList, 0x10);

	struct ScriptableExtraValueSet
	{
		unsigned long long unknown;
		ScriptableExtraValueList list;
	}; assert_sizeof(ScriptableExtraValueSet, 0x18);

	struct ScriptableExtraSharedEntry
	{
		unsigned long long unknown;
		unsigned long long hash;
		ScriptableExtraValueSet* set;
		char unknown2[8];
	}; assert_sizeof(ScriptableExtraSharedEntry, 0x20);

	struct ScriptableExtraInlineEntry
	{
		unsigned long long unknown;
		unsigned long long hash;
		ScriptableExtraValueSet set;
	}; assert_sizeof(ScriptableExtraInlineEntry, 0x28);

	struct ScriptableExtraDef
	{
		unsigned long long hash;
		unsigned int numSharedEntries;
		ScriptableExtraSharedEntry* sharedEntries;
		unsigned int numInlineEntries;
		ScriptableExtraInlineEntry* inlineEntries;
		char unknown[8];
	}; assert_sizeof(ScriptableExtraDef, 0x30);

	struct ScriptableEventAnonymousDef
	{
		ScriptableEventBaseDef* base;
		char buffer[144];
	};

	struct ScriptableEventStateChangeDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		unsigned int stateIdx;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventStateChangeDef, 0x18);

	struct ScriptableEventWaitDef
	{
		ScriptableEventBaseDef* base;
		float delayMin;
		float delayMax;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventWaitDef, 0x18);

	struct ScriptableEventUnknown2Def
	{
		ScriptableEventBaseDef* base;
		char unknown[4];
		unsigned int numConditions;
		ScriptableCondition* conditions;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
		char unknown2[4];
	}; assert_sizeof(ScriptableEventUnknown2Def, 0x20);

	struct ScriptableEventUnknown3Def
	{
		ScriptableEventBaseDef* base;
		char unknown[4];
		unsigned int numConditions;
		ScriptableCondition* conditions;
		unsigned int eventACount;
		ScriptableEventDef* eventsA;
		unsigned int eventBCount;
		ScriptableEventDef* eventsB;
		char unknown2[8];
	}; assert_sizeof(ScriptableEventUnknown3Def, 0x40);
	assert_offsetof(ScriptableEventUnknown3Def, eventsB, 0x30);

	struct ScriptableEventRandomDef
	{
		ScriptableEventBaseDef* base;
		float threshold;
		unsigned int eventACount;
		ScriptableEventDef* eventsA;
		unsigned int eventBCount;
		ScriptableEventDef* eventsB;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventRandomDef, 0x30);

	struct ScriptableEventScriptDef
	{
		ScriptableEventBaseDef* base;
		const char* notification;
		scr_string_t scrNotification;
		int param;
		bool doNotifyCallback;
	}; assert_sizeof(ScriptableEventScriptDef, 0x20);

	struct ScriptableEventModelDef
	{
		ScriptableEventBaseDef* base;
		ScriptableModelDataType dataType;
		ScriptableModelUnion data;
		HudOutlineDef* hudOutline;
		bool neverMoves;
		bool dynamicSimulation;
		bool activatePhysics;
		char unknown[5];
	}; assert_sizeof(ScriptableEventModelDef, 0x28);

	struct ScriptableEventCollisionDef
	{
		ScriptableEventBaseDef* base;
		char unknown[8];
		const char* collmapName;
		char unknown2[8];
		XModel* model;
	}; assert_sizeof(ScriptableEventCollisionDef, 0x28);

	struct ScriptableEventUnknown8Def
	{
		ScriptableEventBaseDef* base;
		bool aiObstacle;
		bool neverMoves;
		bool canPush;
		bool canTouch;
		bool destroyOnExit;
		const char* collmapName;
		int clipmapCModelIndex;
		char unknown[4];
	}; assert_sizeof(ScriptableEventUnknown8Def, 0x20);

	struct ScriptableEventUnknown9Def
	{
		ScriptableEventBaseDef* base;
		bool aiObstacle;
		bool neverMoves;
		bool canPush;
		bool canTouch;
		bool destroyOnExit;
		XModel* model;
	}; assert_sizeof(ScriptableEventUnknown9Def, 0x18);

	struct ScriptableEventAnimationDef
	{
		ScriptableEventBaseDef* base;
		XAnimParts* animation;
		bool stateful;
		bool overrideAnimation;
		bool pauseOnExit;
		bool clientOnlyInMP;
		float startTimeMin;
		float startTimeMax;
		bool startTimeNormalized;
		float playbackRateMin;
		float playbackRateMax;
		float blendTime;
		unsigned int eventAtEndCount;
		ScriptableEventDef* eventsAtEnd;
		unsigned short eventStreamBufferOffsetClient;
		unsigned short eventStreamBufferOffsetServer;
		scr_anim_t animationIndex[2];
	}; assert_sizeof(ScriptableEventAnimationDef, 0x50);
	assert_offsetof(ScriptableEventAnimationDef, eventsAtEnd, 0x30);

	struct ScriptableEventHideShowBoneDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		bool hide;
		bool allowMissingTag;
		bool hideShowChildren;
	}; assert_sizeof(ScriptableEventHideShowBoneDef, 0x18);

	struct ScriptableEventDisablePhysicsSubShapeDef
	{
		ScriptableEventBaseDef* base;
		const char* mutableShapeName;
		unsigned long long mutableShapeHash;
		bool allowMissingShape;
	}; assert_sizeof(ScriptableEventDisablePhysicsSubShapeDef, 0x20);

	struct ScriptableNoteTrackDef
	{
		int flags;
		const char* noteTrackName;
		scr_string_t scrNoteTrackName;
		unsigned int numEvents;
		ScriptableEventDef* events;
	}; assert_sizeof(ScriptableNoteTrackDef, 0x20);

	struct ScriptableEventNoteTrackDef
	{
		ScriptableEventBaseDef* base;
		unsigned int noteTrackCount;
		ScriptableNoteTrackDef* noteTracks;
	}; assert_sizeof(ScriptableEventNoteTrackDef, 0x18);

	struct ScriptableEventChunkDynentDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		bool stateful;
		bool allowCauseVelocity;
		vec3_t launchLinVel;
		vec3_t launchAngVel;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventChunkDynentDef, 0x30);

	struct ScriptableEventSpawnDynentDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		XModel* model;
		const char* tagName;
		scr_string_t scrTagName;
		bool allowCauseVelocity;
		bool allowNearestVehicleVelocity;
		bool worldSpaceLinVel;
		bool worldSpaceAngVel;
		bool randomRangeLinVel;
		bool randomRangeAngVel;
		vec3_t launchLinVel;
		vec3_t launchAngVel;
		vec3_t launchLinVel2;
		vec3_t launchAngVel2;
		unsigned short eventStreamBufferOffsetClient;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		const char* effectAlias;
		FxCombinedDef effectDef;
	}; assert_sizeof(ScriptableEventSpawnDynentDef, 0x70);
	assert_offsetof(ScriptableEventSpawnDynentDef, effectAlias, 0x60);

	struct ScriptableEventPFXDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		scr_string_t* scrTagNames;
		unsigned int scrTagCount;
		scr_string_t* scrEndTagNames;
		unsigned int scrEndTagCount;
		bool useTagAngles;
		const char* effectAlias;
		FxCombinedDef effectDef;
		float loopRate;
		bool oneshotLooping;
		unsigned short eventStreamBufferOffsetClient;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		bool allowNearestVehicleVelocity;
		bool isViewmodel;
		bool killOnExit;
		bool useDynamicScriptedBeamLength;
		int beamBoneAxis;
		float beamLength;
		char unknown[8];
	}; assert_sizeof(ScriptableEventPFXDef, 0x60);
	assert_offsetof(ScriptableEventPFXDef, beamLength, 0x54);
	assert_offsetof(ScriptableEventPFXDef, effectAlias, 0x30);

	struct ScriptableEventSoundDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		bool looping;
		bool externalPlay;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		bool unknown;
		float groundTraceLength;
		vec3_t worldOffset;
		scr_string_t scrTagName;
		const char* tagName;
		const char* soundAlias;
		const char* explosionReflClass;
		char unknown2[0x10];
	}; assert_sizeof(ScriptableEventSoundDef, 0x50);
	assert_offsetof(ScriptableEventSoundDef, tagName, 0x28);

	struct ScriptableEventExplosionDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		WeaponCompleteDef* weapon;
		float radius;
		float dmgOuter;
		float dmgInner;
		bool stateful;
		bool allowMissingTag;
		bool useRootOnMissingTag;
	}; assert_sizeof(ScriptableEventExplosionDef, 0x30);

	struct ScriptableEventLightDef
	{
		ScriptableEventBaseDef* base;
		float intensityScaleMin;
		float intensityScaleMax;
		Curve* curve0;
		char unknown[4];
		float radiusMin;
		float radiusMax;
		Curve* curve1;
		bool useColor;
		vec4_t colorLinearSrgb;
		Curve* curve2;
		char unknown2[0x10];
		Curve* curve3;
		char unknown3[8];
	}; assert_sizeof(ScriptableEventLightDef, 0x70);
	assert_offsetof(ScriptableEventLightDef, curve1, 0x28);
	assert_offsetof(ScriptableEventLightDef, curve2, 0x48);
	assert_offsetof(ScriptableEventLightDef, curve3, 0x60);

	struct ScriptableEventSunDef
	{
		ScriptableEventBaseDef* base;
		bool useIntensity;
		float intensityOverrideMin;
		float intensityOverrideMax;
		bool useColor;
		vec4_t colorLinearSrgb;
		bool useStateTransitionTime;
		float transitionMin;
		float transitionMax;
		float pitchMin;
		float pitchMax;
		float headingMin;
		float headingMax;
		bool useDirection;
	}; assert_sizeof(ScriptableEventSunDef, 0x48);

	struct ScriptableEventRumbleDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		const char* tagName;
		scr_string_t scrTagName;
		const char* rumble;
		RumbleInfo* rumbleAsset;
		bool allowMissingTag;
		bool useRootOnMissingTag;
	}; assert_sizeof(ScriptableEventRumbleDef, 0x38);

	struct ScriptableEventScreenshakeDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		bool isEarthquake;
		bool isScreenShake;
		const char* tagName;
		scr_string_t scrTagName;
		float duration;
		float durationFadeUp;
		float durationFadeDown;
		int radius;
		float scaleEarthquake;
		int scalePitch;
		int scaleYaw;
		int scaleRoll;
		int frequencyPitch;
		int frequencyYaw;
		int frequencyRoll;
		bool allowMissingTag;
		bool useRootOnMissingTag;
	}; assert_sizeof(ScriptableEventScreenshakeDef, 0x50);

	struct ScriptableEventPartDamageDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		float amount;
		bool destroy;
		float amountDoT;
		float intervalDoT;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventPartDamageDef, 0x28);

	struct ScriptableEventSetMayhemDef
	{
		ScriptableEventBaseDef* base;
		MayhemData* mayhem;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventSetMayhemDef, 0x18);

	struct ScriptableEventPlayMayhemDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_MayhemAction action;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventPlayMayhemDef, 0x10);

	struct ScriptableEventViewmodelShaderParamDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_ShaderParamType shaderParam;
		float floatValue;
		float placeHolderValue1;
		float placeHolderValue2;
		float placeHolderValue3;
		float placeHolderValue4;
		float transitionTime;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventViewmodelShaderParamDef, 0x28);

	struct ScriptableEventViewmodelChangeImageDef
	{
		ScriptableEventBaseDef* base;
		int imageId;
		int imageValue;
	}; assert_sizeof(ScriptableEventViewmodelChangeImageDef, 0x10);

	struct ScriptableEventClientViewSelectorDef
	{
		ScriptableEventBaseDef* base;
		unsigned int event1pCount;
		ScriptableEventDef* events1p;
		unsigned int event3pCount;
		ScriptableEventDef* events3p;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventClientViewSelectorDef, 0x30);

	struct ScriptableEventTeamSelectorDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_TeamFilter teamFilter;
		bool testLocalPlayer;
		unsigned int eventPassCount;
		ScriptableEventDef* eventsPass;
		unsigned int eventFailCount;
		ScriptableEventDef* eventsFail;
		unsigned short eventStreamBufferOffsetClient;
		unsigned short eventStreamBufferOffsetServer;
	}; assert_sizeof(ScriptableEventTeamSelectorDef, 0x38);

	struct ScriptableEventAddModelDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		XModel* model;
	}; assert_sizeof(ScriptableEventAddModelDef, 0x20);

	struct ScriptableEventApplyForceDef
	{
		ScriptableEventBaseDef* base;
		bool worldSpace;
		bool randomRange;
		vec3_t forcePos;
		vec3_t forceVector;
		vec3_t forcePos2;
		vec3_t forceVector2;
	}; assert_sizeof(ScriptableEventApplyForceDef, 0x40);

	struct ScriptableEventApplyAngularForceDef
	{
		ScriptableEventBaseDef* base;
		bool worldSpace;
		bool randomRange;
		vec3_t forceVector;
		vec3_t forceVector2;
	}; assert_sizeof(ScriptableEventApplyAngularForceDef, 0x28);

	struct ScriptableEventApplyConstantForceDef
	{
		ScriptableEventBaseDef* base;
		bool worldSpace;
		bool randomRange;
		vec3_t forcePos;
		vec3_t forceVector;
		vec3_t forcePos2;
		vec3_t forceVector2;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventApplyConstantForceDef, 0x40);

	struct ScriptableEventApplyConstantAngularForceDef
	{
		ScriptableEventBaseDef* base;
		bool worldSpace;
		bool randomRange;
		vec3_t forceVector;
		vec3_t forceVector2;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventApplyConstantAngularForceDef, 0x28);

	struct ScriptableEventCompassIconDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		bool friendlyCompassIconAlwaysVisible;
		bool enemyCompassIconAlwaysVisible;
		Material* friendlyArrow;
		Material* friendlyFiring;
		Material* friendlyChatting;
		Material* friendlyYelling;
		Material* partyArrow;
		Material* partyFiring;
		Material* partyChatting;
		Material* partyYelling;
		Material* squadArrow;
		Material* squadFiring;
		Material* enemyCompassIconQuiet[3];
		Material* enemyCompassIconFiring[3];
		Material* enemyCompassIconDirectional;
	}; assert_sizeof(ScriptableEventCompassIconDef, 0x98);

	struct ScriptableEventMaterialOverrideDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_MaterialOverrideType materialOverrideType;
		int transitionTime;
		char finalValue;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventMaterialOverrideDef, 0x18);

	struct ScriptableEventDynamicBoneNoiseCurveDef
	{
		ScriptableEventBaseDef* base;
		bool stateful;
		XAnimCurve* curve;
		float strengthMultiplier;
		float rateMultiplier;
		bool isViewmodel;
	}; assert_sizeof(ScriptableEventDynamicBoneNoiseCurveDef, 0x28);

	struct ScriptableEventMoveDef
	{
		ScriptableEventBaseDef* base;
		vec3_t originOffset;
		vec3_t angleOffset;
		float seconds;
		float secondsAccel;
		float secondsDecel;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventMoveDef, 0x30);

	struct ScriptableEventFootstepDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		const char* soundAlias;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		unsigned char flags;
		FootstepVFX* rightFootstepVFX;
		FootstepVFX* leftFootstepVFX;
	}; assert_sizeof(ScriptableEventFootstepDef, 0x38);

	struct ScriptableEventGravityArcDef
	{
		ScriptableEventBaseDef* base;
		vec3_t angleOffset;
		float gravityScale;
		float upwardVelocity;
		float yawOffsetForXYDirection;
		bool endAtInitialPose;
		bool randomSpin;
		bool playLootTrailFX;
		unsigned int eventAtEndCount;
		ScriptableEventDef* eventsAtEnd;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventGravityArcDef, 0x38);
	assert_offsetof(ScriptableEventGravityArcDef, eventsAtEnd, 0x28);

	struct ScriptableEventViewTriggerDef
	{
		ScriptableEventBaseDef* base;
		float triggerDistanceSq;
		float triggerFOVCosHalfAngle;
		unsigned short eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventViewTriggerDef, 0x18);

	struct ScriptableEventObjectiveDef
	{
		ScriptableEventBaseDef* base;
		char unknown[0x28];
		const char* description;
		const char* icon;
		const char* label;
		GfxImage* iconImage;
	}; assert_sizeof(ScriptableEventObjectiveDef, 0x50);

	struct ScriptableEventSpatialDisableDef
	{
		ScriptableEventBaseDef* base;
		bool disabled;
	}; assert_sizeof(ScriptableEventSpatialDisableDef, 0x10);

	struct ScriptableEventHoverDef
	{
		ScriptableEventBaseDef* base;
		float amplitudeUpDown;
		float frequencyUpDown;
		float frequencyPitch;
		float frequencyYaw;
	}; assert_sizeof(ScriptableEventHoverDef, 0x18);

	struct ScriptableEventUnknown2FDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
	}; assert_sizeof(ScriptableEventUnknown2FDef, 0x18);

	struct ScriptableEventUnknown30Def
	{
		ScriptableEventBaseDef* base;
		scr_string_t scrTagName;
	}; assert_sizeof(ScriptableEventUnknown30Def, 0x10);

	struct ScriptableEventCalloutMarkerPingDef
	{
		ScriptableEventBaseDef* base;
		char unknown[8];
		CalloutMarkerPing* calloutMarkerPing;
	}; assert_sizeof(ScriptableEventCalloutMarkerPingDef, 0x18);

	struct ScriptableEventUnknown35Def
	{
		ScriptableEventBaseDef* base;
		unsigned int eventACount;
		ScriptableEventDef* eventsA;
		unsigned int eventBCount;
		ScriptableEventDef* eventsB;
		char unknown[8];
	}; assert_sizeof(ScriptableEventUnknown35Def, 0x30);

	union ScriptableEventDefUnion
	{
		ScriptableEventAnonymousDef anonymous;
		ScriptableEventStateChangeDef stateChange;
		ScriptableEventWaitDef wait;
		ScriptableEventUnknown2Def unknown2;
		ScriptableEventUnknown3Def unknown3;
		ScriptableEventRandomDef random;
		ScriptableEventScriptDef script;
		ScriptableEventModelDef model;
		ScriptableEventCollisionDef collision;
		ScriptableEventUnknown8Def unknown8;
		ScriptableEventUnknown9Def unknown9;
		ScriptableEventAnimationDef animation;
		ScriptableEventHideShowBoneDef hideShowBone;
		ScriptableEventDisablePhysicsSubShapeDef disablePhysicsSubShape;
		ScriptableEventNoteTrackDef noteTrack;
		ScriptableEventChunkDynentDef chunkDynent;
		ScriptableEventSpawnDynentDef spawnDynent;
		ScriptableEventPFXDef particleFX;
		ScriptableEventSoundDef sound;
		ScriptableEventExplosionDef explosion;
		ScriptableEventLightDef light;
		ScriptableEventSunDef sun;
		ScriptableEventRumbleDef rumble;
		ScriptableEventScreenshakeDef screenshake;
		ScriptableEventPartDamageDef partDamage;
		ScriptableEventSetMayhemDef setMayhem;
		ScriptableEventPlayMayhemDef playMayhem;
		ScriptableEventViewmodelShaderParamDef viewmodelShaderParam;
		ScriptableEventViewmodelChangeImageDef viewmodelChangeImage;
		ScriptableEventClientViewSelectorDef clientViewSelector;
		ScriptableEventTeamSelectorDef teamSelector;
		ScriptableEventAddModelDef addModel;
		ScriptableEventApplyForceDef applyForce;
		ScriptableEventApplyAngularForceDef applyAngularForce;
		ScriptableEventApplyConstantForceDef applyConstantForce;
		ScriptableEventApplyConstantAngularForceDef applyConstantAngularForce;
		ScriptableEventCompassIconDef compassIcon;
		ScriptableEventMaterialOverrideDef materialOverride;
		ScriptableEventDynamicBoneNoiseCurveDef dynamicBoneNoiseCurve;
		ScriptableEventMoveDef move;
		ScriptableEventFootstepDef footstep;
		ScriptableEventGravityArcDef gravityArc;
		ScriptableEventViewTriggerDef viewTrigger;
		ScriptableEventObjectiveDef objective;
		ScriptableEventSpatialDisableDef spatialDisable;
		ScriptableEventHoverDef hover;
		ScriptableEventUnknown2FDef unknown2F;
		ScriptableEventUnknown30Def unknown30;
		ScriptableEventCalloutMarkerPingDef calloutMarkerPing;
		ScriptableEventUnknown35Def unknown35;
	};

	struct ScriptableEventDef
	{
		ScriptableEventBaseDef base;
		Scriptable_EventType type;
		ScriptableEventDefUnion data;
	}; assert_sizeof(ScriptableEventDef, 0xB0);

	struct ScriptableStateBaseDef
	{
		const char* name;
		int flags;
		unsigned int numEvents;
		ScriptableEventDef* events;
	}; assert_sizeof(ScriptableStateBaseDef, 0x18);

	enum Scriptable_StateType : std::int32_t
	{
		Scriptable_StateType_Simple = 0x0,
		Scriptable_StateType_Health = 0x1,
		Scriptable_StateType_Scripted = 0x2,
		Scriptable_StateType_Usable = 0x3,
		Scriptable_StateType_Count = 0x4,
	};

	struct ScriptableStateSimpleDef
	{
		ScriptableStateBaseDef* base;
	};

	struct ScriptableStateHealthDef
	{
		ScriptableStateBaseDef* base;
		int health;
		float unknown[10];
		int unknown2[2];
		unsigned int numWeaponClasses;
		weapClass_t* weaponClasses;
		unsigned int numWeapons;
		WeaponCompleteDef** weapons;
		int minimumDamage;
		float damagePropagationToParent;
		float damagePropagationToChild;
		float damagePropagationFromParent;
		float damagePropagationFromChild;
		const char* script_id;
		scr_string_t scrScript_id;
		char unknown3[4];
	}; assert_sizeof(ScriptableStateHealthDef, 0x80);
	assert_offsetof(ScriptableStateHealthDef, weaponClasses, 0x40);
	assert_offsetof(ScriptableStateHealthDef, script_id, 0x70);

	struct ScriptableStateScriptedDef
	{
		ScriptableStateBaseDef* base;
		const char* script_id;
		scr_string_t scrScript_id;
	}; assert_sizeof(ScriptableStateScriptedDef, 0x18);

	struct ScriptableStateUsableDef
	{
		ScriptableStateBaseDef* base;
		const char* script_id;
		scr_string_t scrScript_id;
		float faceFOV;
		float useFOV;
		float useRadius;
		float useRadiusAuto;
		float useRadiusDisplay;
		int unknown;
		bool useRadiusMinimized;
		bool gotoNextStateOnUse;
		bool allowMissingTag;
		bool usableWhenObstructed;
		char unknown1[4];
		vec3_t useOffset;
		vec3_t angleOffset;
		unsigned int hintHoldDurationMs;
		char priority;
		bool use2DFOV;
		short hintBinding;
		scr_string_t hintTag;
		const char* hintString;
		const char* hintStringDisabled;
		const char* hintIcon;
		GfxImage* hintIconImage;
		char unknown2[0xC];
		scr_string_t scrUnknown;
		char unknown3[8];
	}; assert_sizeof(ScriptableStateUsableDef, 0x90);
	assert_offsetof(ScriptableStateUsableDef, useOffset, 0x34);
	assert_offsetof(ScriptableStateUsableDef, hintHoldDurationMs, 0x4C);
	assert_offsetof(ScriptableStateUsableDef, hintTag, 0x54);
	assert_offsetof(ScriptableStateUsableDef, scrUnknown, 0x84);

	union ScriptableStateDefUnion
	{
		ScriptableStateSimpleDef simple;
		ScriptableStateHealthDef health;
		ScriptableStateScriptedDef scripted;
		ScriptableStateUsableDef usable;
	};

	struct ScriptableStateDef
	{
		ScriptableStateBaseDef base;
		Scriptable_StateType type;
		ScriptableStateDefUnion data;
	}; assert_sizeof(ScriptableStateDef, 0xB0);

	struct ScriptableDamageTagOverride
	{
		const char* tag;
		scr_string_t scrTag;
	}; assert_sizeof(ScriptableDamageTagOverride, 0x10);

	struct ScriptablePartDef
	{
		const char* name;
		scr_string_t scrName;
		int flags;
		unsigned char flatId;
		unsigned char unknown[7];
		const char* tagName;
		scr_string_t scrTagName;
		unsigned int numStates;
		ScriptableStateDef* states;
		ScriptablePartDef* parentPart;
		unsigned int numChildParts;
		ScriptablePartDef* childParts;
		unsigned int numDamageTagOverrides;
		ScriptableDamageTagOverride* damageTagOverrides;
		unsigned short eventStreamBufferOffsetServer;
		unsigned short eventStreamBufferOffsetClient;
		char unknown2[4];
	}; assert_sizeof(ScriptablePartDef, 0x60);
	assert_offsetof(ScriptablePartDef, states, 0x28);
	assert_offsetof(ScriptablePartDef, damageTagOverrides, 0x50);

	struct XAnim_s;

	struct ScriptableDef
	{
		const char* name;
		ScriptableDef* nextScriptableDef;
		int flags;
		unsigned int numParts;
		ScriptablePartDef* parts;
		unsigned int maxNumDynEntsRequired;
		unsigned char unknown[8];
		unsigned int maxNumDynEntPartsBase;
		unsigned int maxNumDynEntPartsForSpawning;
		unsigned short eventStreamSizeRequiredServer;
		unsigned short eventStreamSizeRequiredClient;
		unsigned int usablePartCount;
		unsigned int ffMemCost;
		scr_string_t animationTreeName;
		char unknown2[0x14];
		float viewRadius;
		bool networkLODRangeOverride;
		float networkLODRangeOverrideDistance;
		unsigned int numXModels;
		XModel** models;
		char unknown3[8];
		ScriptableExtraDef* extra;
	}; assert_sizeof(ScriptableDef, 0x80);
	assert_offsetof(ScriptableDef, animationTreeName, 0x40);
	assert_offsetof(ScriptableDef, viewRadius, 0x58);
	assert_offsetof(ScriptableDef, networkLODRangeOverrideDistance, 0x60);
	assert_offsetof(ScriptableDef, models, 0x68);

	struct WeaponAttachment;
	struct WeaponDef;
	struct NoteTrackToSoundEntry;
	struct NoteTrackToSuitSoundEntry;
	struct RumbleGraph;
	struct WeaponOffsetPatternScale;
	struct BallisticInfoCalculated;
	struct WeaponVFXPackage;
	struct WeaponOffsetPatternDescription;
	struct TracerDef;
	struct LaserDef;
	struct Gesture;
	struct AnimOverride;
	struct SFXOverride;
	struct VFXOverride;
	struct WeaponEntityNotify;
	struct OverrideBinding;
	struct OverrideBindingList;

	enum EnergyType : std::int32_t
	{
		ENERGY_TYPE_MOVEMENT = 0x0,
		ENERGY_TYPE_DODGE = 0x1,
		ENERGY_TYPE_COUNT = 0x2,
	};

	struct AttachmentList
	{
		unsigned int attachmentCount;
		WeaponAttachment** attachments;
	}; assert_sizeof(AttachmentList, 0x10);

	enum AdsOffsetInterpolationType : std::int32_t
	{
		ADS_OFFSET_INTERP_TYPE_LINEAR = 0x0,
		ADS_OFFSET_INTERP_TYPE_EASEINOUTSINE = 0x1,
		ADS_OFFSET_INTERP_TYPE_EASEINOUTSMOOTHERSTEP = 0x2,
		ADS_OFFSET_INTERP_TYPE_EASEINCUBIC = 0x3,
		ADS_OFFSET_INTERP_TYPE_EASEOUTCUBIC = 0x4,
		ADS_OFFSET_INTERP_TYPE_EASEINQUARTIC = 0x5,
		ADS_OFFSET_INTERP_TYPE_EASEOUTQUARTIC = 0x6,
		ADS_OFFSET_INTERP_TYPE_EASEINEXPO = 0x7,
		ADS_OFFSET_INTERP_TYPE_EASEOUTEXPO = 0x8,
		ADS_OFFSET_INTERP_TYPE_COUNT = 0x9,
	};

	struct SceneZoomSettings
	{
		int adsZoomCount;
		float adsZoomFov[3];
		float adsReloadFov[3];
		int adsZoomFovTransitionTime[3];
		AdsOffsetInterpolationType adsZoomFovLerpType;
		float adsZoomInFrac;
		float adsZoomOutFrac;
		float adsFOVNVG;
		float adsReloadFovNVG;
		bool adsZoomLevelIndexResetOnToggle;
	}; assert_sizeof(SceneZoomSettings, 0x40);

	struct WeaponZoomSettings
	{
		float adsZoomFov;
		AdsOffsetInterpolationType adsZoomFovLerpType;
		float adsZoomFovXOffset;
		float adsReloadZoomFov;
		float adsReloadZoomFovXOffset;
		float adsNVGZoomFov;
		float adsNVGZoomFovXOffset;
		float adsNVGReloadZoomFov;
		float adsNVGReloadZoomFovXOffset;
		float adsZoomInFrac;
		float adsZoomOutFrac;
		bool adsZoomFovXOffsetOverride;
		bool adsReloadFovXOffsetOverride;
	}; assert_sizeof(WeaponZoomSettings, 0x30);

	struct ZoomSettings
	{
		SceneZoomSettings scene;
		WeaponZoomSettings weapon;
	}; assert_sizeof(ZoomSettings, 0x70);

	struct Beam
	{
		const char* name;
		char __pad0[0x58];
		FxCombinedDef effects[9];
		XModel* models[2];
		char __pad1[0x58];
		SndAliasLookup sounds[4];
		char __pad2[0x8];
		SndAliasLookup sounds2[2];
		char __pad3[0x8];
	}; assert_sizeof(Beam, 0x150);

	struct TriggerEffect
	{
		const char* name;
		SndAliasLookup sound;
		char __pad0[0x8];
	}; assert_sizeof(TriggerEffect, 0x18);

	struct WeaponTrigger
	{
		const char* name;
		char __pad0[0x10];
		RumbleInfo* rumbles[2];
		char __pad1[0x10];
	}; assert_sizeof(WeaponTrigger, 0x38);

	struct ReactiveStageSet
	{
		const char* name;
		char __pad0[0x218];
	}; assert_sizeof(ReactiveStageSet, 0x220);

	struct WeaponAccuracy
	{
		const char* name;
		unsigned short knotCount;
		vec2_t* knots;
	}; assert_sizeof(WeaponAccuracy, 0x18);

	struct WeaponCompleteDef
	{
		const char* name;
		WeaponDef* weapDef;
		const char* szDisplayName;
		const char* szLootTable;
		scr_string_t* hideTags;
		AttachmentList attachments[15];
		unsigned int numLootVariants;
		unsigned int numNotetrackSoundMappings;
		scr_string_t* notetrackSoundMapKeys;
		scr_string_t* notetrackSoundMapValues;
		unsigned int numNotetrackOverrides;
		char __pad0[0x4];
		NoteTrackToSoundEntry* notetrackOverrides;
		unsigned int numNotetrackSuitEntries;
		char __pad1[0x4];
		NoteTrackToSuitSoundEntry* notetrackSuitEntries;
		char __pad2[0x98];
		Material* killIconMat;
		Material* dpadIconMat;
		Material* dpadIconInactiveMat;
		GfxImage* killIconImg;
		GfxImage* dpadIconImg;
		char __pad3[0x8];
		GfxImage* dpadIconInactiveImg;
		GfxImage* unknownIconImg;
		char __pad4[0x20];
		const char* szAltModeInactiveName;
		const char* dynamicIconAnimationName;
		char __pad5[0x8];
		const char* szLUIWeaponInfoWidgetName;
		const char* szLUIWeaponInfoWidgetTag;
		const char* szLUICrosshairWidget;
	}; assert_sizeof(WeaponCompleteDef, 0x278);

	struct WeaponDamageCommon
	{
		int minDamage;
		int mid1Damage;
		int mid2Damage;
		int mid3Damage;
		int damage;
		float maxDamageRange;
		float mid1DamageRange;
		float mid2DamageRange;
		float mid3DamageRange;
		float minDamageRange;
	}; assert_sizeof(WeaponDamageCommon, 0x28);

	enum AnimDamageType : std::int32_t
	{
		ANIM_DAMAGETYPE_NONE = 0x0,
		ANIM_DAMAGETYPE_EXPLOSION_LIGHT = 0x1,
		ANIM_DAMAGETYPE_EXPLOSION = 0x2,
		ANIM_DAMAGETYPE_MELEE = 0x3,
		ANIM_DAMAGETYPE_MELEE_DOG = 0x4,
		ANIM_DAMAGETYPE_MELEE_ALIEN = 0x5,
		ANIM_DAMAGETYPE_ENERGY_BULLET = 0x6,
		ANIM_DAMAGETYPE_FIRE_EXPLOSION = 0x7,
		ANIM_DAMAGETYPE_BULLET_SM = 0x8,
		ANIM_DAMAGETYPE_BULLET_MD = 0x9,
		ANIM_DAMAGETYPE_BULLET_LG = 0xA,
		ANIM_DAMAGETYPE_FALL = 0xB,
		ANIM_DAMAGETYPE_COUNT = 0xC,
	};

	struct WeaponDamageInfo
	{
		WeaponDamageCommon damageData[3];
		AnimDamageType deathAnimDamageType;
		float terminateAtRange;
		Curve* damageCurve;
		char __pad0[0x8];
	}; assert_sizeof(WeaponDamageInfo, 0x90);

	enum scriptedAnimEvent_t : std::int32_t
	{
		SCRIPTEDANIMEVENT_POWER_ACTIVE = 0x0,
		SCRIPTEDANIMEVENT_POWER_ACTIVE_CP = 0x1,
		SCRIPTEDANIMEVENT_COUNT = 0x2,
	};

	enum weapType_t : std::int32_t
	{
		WEAPTYPE_NONE = 0x0,
		WEAPTYPE_MODEL_ONLY = 0x1,
		WEAPTYPE_BULLET = 0x2,
		WEAPTYPE_GRENADE = 0x3,
		WEAPTYPE_PROJECTILE = 0x4,
		WEAPTYPE_RIOTSHIELD = 0x5,
		WEAPTYPE_SCRIPT = 0x6,
		WEAPTYPE_SHIELD = 0x7,
		WEAPTYPE_CHARGE_SHIELD = 0x8,
		WEAPTYPE_LOCATION_SELECT = 0x9,
		WEAPTYPE_EQUIP_DEPLOY = 0xA,
		WEAPTYPE_NUM = 0xB,
	};

	enum PenetrateType : std::int32_t
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,
		PENETRATE_TYPE_MAXIMUM = 0x4,
		PENETRATE_TYPE_TABLE_ENTRY_COUNT = 0x4,
		PENETRATE_TYPE_RICOCHET = 0x5,
		PENETRATE_TYPE_COUNT = 0x6,
	};

	enum weapInventoryType_t : std::int32_t
	{
		WEAPINVENTORY_PRIMARY = 0x0,
		WEAPINVENTORY_OFFHAND = 0x1,
		WEAPINVENTORY_ITEM = 0x2,
		WEAPINVENTORY_ALTMODE = 0x3,
		WEAPINVENTORY_EXCLUSIVE = 0x4,
		WEAPINVENTORY_SCAVENGER = 0x5,
		WEAPINVENTORY_MODEL_ONLY = 0x6,
		WEAPINVENTORYCOUNT = 0x7,
	};

	enum weapFireType_t : std::int32_t
	{
		WEAPON_FIRETYPE_FULLAUTO = 0x0,
		WEAPON_FIRETYPE_SINGLESHOT = 0x1,
		WEAPON_FIRETYPE_BURST = 0x2,
		WEAPON_FIRETYPE_DOUBLEBARREL = 0x3,
		WEAPON_FIRETYPE_DOUBLEBARREL_DUALTRIGGER = 0x4,
		WEAPON_FIRETYPE_BEAM = 0x5,
		WEAPON_FIRETYPECOUNT = 0x6,
	};

	enum turretFireType_t : std::int32_t
	{
		TURRET_SINGLE_FIRE = 0x0,
		TURRET_ALTERNATE_FIRE = 0x1,
		TURRET_FIRE_TYPE_COUNT = 0x2,
	};

	enum targetAssistType_t : std::int32_t
	{
		TARGET_ASSISTTYPPE_DEFAULT = 0x0,
		TARGET_ASSISTTYPPE_CONE = 0x1,
		TARGET_ASSISTTYPPE_COUNT = 0x2,
	};

	enum targetAssistBehavior_t : std::int32_t
	{
		TARGET_ASSISTBEHAVE_DEFAULT = 0x0,
		TARGET_ASSISTBEHAVE_DIRECT_DAMAGE = 0x1,
		TARGET_ASSISTBEHAVE_TARGET_ONLY = 0x2,
		TARGET_ASSISTBEHAVE_BEAM = 0x3,
		TARGET_ASSISTBEHAVE_SMART_PELLET = 0x4,
		TARGET_ASSISTBEHAVE_COUNT = 0x5,
	};

	enum ReloadType : std::int32_t
	{
		RELOAD_TYPE_NORMAL = 0x0,
		RELOAD_TYPE_SEGMENTED = 0x1,
		RELOAD_TYPE_MULTIPLE_BULLET = 0x2,
		RELOAD_TYPE_MULTIPLE_PERCENTAGE = 0x3,
		RELOAD_TYPE_COUNT = 0x4,
	};

	enum OffhandClass : std::int32_t
	{
		OFFHAND_CLASS_NONE = 0x0,
		OFFHAND_CLASS_FRAG_GRENADE = 0x1,
		OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
		OFFHAND_CLASS_FLASH_GRENADE = 0x3,
		OFFHAND_CLASS_MOLOTOV = 0x4,
		OFFHAND_CLASS_THROWINGKNIFE = 0x5,
		OFFHAND_CLASS_SHIELD = 0x6,
		OFFHAND_CLASS_OTHER = 0x7,
		OFFHAND_CLASS_COUNT = 0x8,
	};

	enum weapStance_t : std::int32_t
	{
		WEAPSTANCE_STAND = 0x0,
		WEAPSTANCE_DUCK = 0x1,
		WEAPSTANCE_PRONE = 0x2,
		WEAPSTANCE_NUM = 0x3,
	};

	enum ReticleType : std::int32_t
	{
		RETICLE_TYPE_FOUR_SIDED = 0x0,
		RETICLE_TYPE_ONE_PIECE = 0x1,
		RETICLE_TYPE_COUNT = 0x2,
	};

	enum HitmarkerType : std::int32_t
	{
		HITMARKER_TYPE_SMALL = 0x0,
		HITMARKER_TYPE_MEDIUM = 0x1,
		HITMARKER_TYPE_LARGE = 0x2,
		HITMARKER_TYPE_COUNT = 0x3,
	};

	enum activeReticleType_t : std::int32_t
	{
		VEH_ACTIVE_RETICLE_NONE = 0x0,
		VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
		VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
		VEH_ACTIVE_RETICLE_COUNT = 0x3,
	};

	enum turret3pAnimType_t : std::int32_t
	{
		TURRET_3PANIM_MOUNTED = 0x0,
		TURRET_3PANIM_MOUNTED_INPLACE = 0x1,
		TURRET_3PANIM_MOUNTED_INPLACE_LEAN = 0x2,
		TURRET_3PANIM_COUNT = 0x3,
	};

	enum ammoCounterClipType_t : std::int32_t
	{
		AMMO_COUNTER_CLIP_NONE = 0x0,
		AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
		AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
		AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
		AMMO_COUNTER_CLIP_ROCKET = 0x4,
		AMMO_COUNTER_CLIP_BELTFED = 0x5,
		AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
		AMMO_COUNTER_CLIP_COUNT = 0x7,
	};

	enum weapOverlayReticle_t : std::int32_t
	{
		WEAPOVERLAYRETICLE_NONE = 0x0,
		WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
		WEAPOVERLAYRETICLE_NUM = 0x2,
	};

	struct ADSOverlay
	{
		Material* shaderMat;
		Material* shaderLowResMat;
		Material* shaderEMPMat;
		Material* shaderEMPLowResMat;
		GfxImage* shaderImg;
		GfxImage* shaderLowResImg;
		GfxImage* shaderEMPImg;
		GfxImage* shaderEMPLowResImg;
		bool applyVisionSet;
		weapOverlayReticle_t reticle;
		float width;
		float height;
		float widthSplitscreen;
		float heightSplitscreen;
		const char* visionSetName;
		float visionSetADSFraction;
		int visionSetBlendInTimeMs;
		int visionSetBlendOutTimeMs;
	}; assert_sizeof(ADSOverlay, 0x70);

	enum WeapOverlayInteface_t : std::int32_t
	{
		WEAPOVERLAYINTERFACE_NONE = 0x0,
		WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
		WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
		WEAPOVERLAYINTERFACE_BINOCULARS = 0x3,
		WEAPOVERLAYINTERFACECOUNT = 0x4,
	};

	struct CommonSwaySettings
	{
		float maxAngle;
		float lerpSpeed;
		float pitchScale;
		float yawScale;
		float horizScale;
		float vertScale;
		float gunAngleScale;
	}; assert_sizeof(CommonSwaySettings, 0x1C);

	struct HipSwaySettings
	{
		CommonSwaySettings common;
		float maxAngleSteadyAim;
	}; assert_sizeof(HipSwaySettings, 0x20);

	struct AdsSwaySettings
	{
		CommonSwaySettings common;
		float swayTransitionLerpSpeed;
		float adsSwayScale[3];
	}; assert_sizeof(AdsSwaySettings, 0x2C);

	struct AdvancedHipSwaySettings
	{
		bool enabled;
		float torsoGoalSmoothSpeed;
		int torsoGoalViewSmoothDurationMs;
		vec2_t torsoGoalDeadzoneAdjustSpeed;
		RumbleGraph* torsoGoalViewSpeedToMaxDeadzone_graph;
		vec2_t torsoGoalViewSpeedToMaxDeadzone_viewspeed;
		vec2_t torsoGoalViewSpeedToMaxDeadzone_maxDeadzone;
		vec2_t torsoMass;
		vec2_t torsoSpring;
		vec2_t torsoDamper;
		int gunGoalViewSmoothDurationMs;
		RumbleGraph* gunGoalViewSpeedToOffset_graph;
		vec2_t gunGoalViewSpeedToOffset_viewspeed;
		vec2_t gunGoalViewSpeedToOffset_offset;
		vec2_t gunMass;
		vec2_t gunSpring;
		vec2_t gunDamper;
		vec3_t gunPivotPoint;
		float gunYawToRollScale;
		int fireDurationMs;
		int fireStartBlendDurationMs;
		int fireFinishBlendDurationMs;
		float fireTorsoGoalSmoothSpeed;
		float fireTorsoDeadzoneScale;
		float fireTorsoToGunDirScale;
	}; assert_sizeof(AdvancedHipSwaySettings, 0xA8);

	struct SwaySettings
	{
		HipSwaySettings hip;
		AdsSwaySettings ads;
		AdvancedHipSwaySettings adv;
		float shellShockScale;
		bool overrideHip;
		bool overrideAds;
	}; assert_sizeof(SwaySettings, 0x100);

	enum weaponIconRatioType_t : std::int32_t
	{
		WEAPON_ICON_RATIO_1TO1 = 0x0,
		WEAPON_ICON_RATIO_2TO1 = 0x1,
		WEAPON_ICON_RATIO_4TO1 = 0x2,
		WEAPON_ICON_RATIO_COUNT = 0x3,
	};

	enum weapProjExposion_t : std::int32_t
	{
		WEAPPROJEXP_GRENADE = 0x0,
		WEAPPROJEXP_ROCKET = 0x1,
		WEAPPROJEXP_FLASHBANG = 0x2,
		WEAPPROJEXP_MOLOTOV = 0x3,
		WEAPPROJEXP_NONE = 0x4,
		WEAPPROJEXP_DUD = 0x5,
		WEAPPROJEXP_SMOKE = 0x6,
		WEAPPROJEXP_HEAVY = 0x7,
		WEAPPROJEXP_NUM = 0x8,
	};

	enum WeapStickinessType : std::int32_t
	{
		WEAPSTICKINESS_NONE = 0x0,
		WEAPSTICKINESS_ALL = 0x1,
		WEAPSTICKINESS_ALL_ORIENT = 0x2,
		WEAPSTICKINESS_GROUND = 0x3,
		WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
		WEAPSTICKINESS_GROUND_ORIENT = 0x5,
		WEAPSTICKINESS_KNIFE = 0x6,
		WEAPSTICKINESS_COUNT = 0x7,
	};

	enum guidedMissileType_t : std::int32_t
	{
		MISSILE_GUIDANCE_NONE = 0x0,
		MISSILE_GUIDANCE_SIDEWINDER = 0x1,
		MISSILE_GUIDANCE_HELLFIRE = 0x2,
		MISSILE_GUIDANCE_JAVELIN = 0x3,
		MISSILE_GUIDANCE_ROBOTECH = 0x4,
		MISSILE_GUIDANCE_SOFTLOCKON = 0x5,
		MISSILE_GUIDANCE_COUNT = 0x6,
	};

	enum WeaponOffsetBlendInterpolationType : std::int32_t
	{
		WOBIT_LINEAR = 0x0,
		WOBIT_CUBIC_EASE_IN = 0x1,
		WOBIT_CUBIC_EASE_OUT = 0x2,
		WOBIT_QUARTIC_EASE_IN = 0x3,
		WOBIT_QUARTIC_EASE_OUT = 0x4,
		WOBIT_EXPONENTIAL_EASE_IN = 0x5,
		WOBIT_EXPONENTIAL_EASE_OUT = 0x6,
		NUM_WEAPON_OFFSET_BLEND_INTERPOLATION_TYPES = 0x7,
	};

	struct WeaponOffsetCurveDescription
	{
		float blendTime;
		float decayTime;
		float shotDecayFireTimeFrac;
		float holdTime;
		float adsFractionBegin;
		float adsFractionEnd;
		WeaponOffsetBlendInterpolationType interpType;
		WeaponOffsetBlendInterpolationType interpTypeOut;
	}; assert_sizeof(WeaponOffsetCurveDescription, 0x20);

	enum tracerStyle_t : std::int32_t
	{
		TRACERSTYLE_TRACER = 0x0,
		TRACERSTYLE_BEAM = 0x1,
		TRACERSTYLE_NUM = 0x2,
	};

	enum offhandShieldMaxSpeed_t : std::int32_t
	{
		WEAP_OFFHAND_SHIELD_SPEED_WALK = 0x0,
		WEAP_OFFHAND_SHIELD_SPEED_RUN = 0x1,
		WEAP_OFFHAND_SHIELD_SPEED_SPRINT = 0x2,
		WEAP_OFFHAND_SHIELD_SPEED_COUNT = 0x3,
	};

	enum offhandShieldButton_t : std::int32_t
	{
		WEAP_OFFHAND_SHIELD_BUTTON_DEFAULT = 0x0,
		WEAP_OFFHAND_SHIELD_BUTTON_ADS = 0x1,
		WEAP_OFFHAND_SHIELD_BUTTON_COUNT = 0x2,
	};

	struct LaserSettings
	{
		bool forceLaserOn;
		bool localPlayerADSLaserEnabled;
		bool localPlayerHipLaserEnabled;
		bool localPlayerNVGADSLaserEnabled;
		bool localPlayerNVGHipLaserEnabled;
		bool remotePlayerADSLaserEnabled;
		bool remotePlayerHipLaserEnabled;
		bool remotePlayerNVGADSLaserEnabled;
		bool remotePlayerNVGHipLaserEnabled;
		bool laserViewCenterInAds;
	}; assert_sizeof(LaserSettings, 0xA);

	enum DualWieldType : std::int32_t
	{
		DUAL_WIELD_TYPE_DEFAULT = 0x0,
		DUAL_WIELD_TYPE_ALT_MODE = 0x1,
		DUAL_WIELD_TYPE_COUNT = 0x2,
	};

	enum WeapDetonateType : std::int32_t
	{
		DETONATE_TYPE_NONE = 0x0,
		DETONATE_TYPE_GESTURE = 0x1,
		DETONATE_TYPE_VIEWMODEL = 0x2,
		DETONATE_TYPE_COUNT = 0x3,
	};

	enum GrenadeRotationDirection : std::int32_t
	{
		GRD_RANDOM = 0x0,
		GRD_CLOCKWISE = 0x1,
		GRD_COUNTERCLOCKWISE = 0x2,
		GRD_COUNT = 0x3,
	};

	struct GrenadeRotationParams
	{
		float initialPitch;
		float initialYaw;
		float initialRoll;
		GrenadeRotationDirection rotationPitchDir;
		int rotationPitchMin;
		int rotationPitchMax;
		GrenadeRotationDirection rotationYawDir;
		int rotationYawMin;
		int rotationYawMax;
		GrenadeRotationDirection rotationRollDir;
		int rotationRollMin;
		int rotationRollMax;
		bool rotate;
	}; assert_sizeof(GrenadeRotationParams, 0x34);

	enum WeaponSlot : std::int32_t
	{
		WEAPON_SLOT_NONE = 0x0,
		WEAPON_SLOT_PRIMARY = 0x1,
		WEAPON_SLOT_HEAVY = 0x2,
		WEAPON_SLOT_MELEE = 0x3,
		WEAPON_SLOT_EXECUTION = 0x4,
		WEAPON_SLOT_ACCESSORY = 0x5,
		WEAPON_SLOT_NUM = 0x6,
	};

	enum CarryObjectType : std::int32_t
	{
		CARRYOBJECTTYPE_GENERIC = 0x0,
		CARRYOBJECTTYPE_HUMAN = 0x1,
		CARRYOBJECTTYPE_COUNT = 0x2,
	};

	struct CarryObjectDef
	{
		const char* name;
		CarryObjectType type;
		XModel* carryViewModel;
		XModel* carryWorldModel;
		Gesture* gesture;
		scr_string_t attachTag;
		bool allowADS;
	}; assert_sizeof(CarryObjectDef, 0x30);

	struct CarryAnimOverride
	{
		CarryObjectType carryObjectType;
		WeaponAnimPackage* animPackage;
		WeaponAnimPackage* animPackageAlt;
	}; assert_sizeof(CarryAnimOverride, 0x18);

	struct WeaponOffsetPatternScaleInfo
	{
		int numPatternScales;
		WeaponOffsetPatternScale* patternScales;
	}; assert_sizeof(WeaponOffsetPatternScaleInfo, 0x10);

	struct AdvancedIdleSettings
	{
		bool useAdvancedIdleSettings;
		bool useRandomPointsAlgorithm;
		float breathGaspScaleOverride;
		float idleSwaySetting1_HipBulletDirScale;
		float idleSwaySetting1_HipIdleSpeed;
		float idleSwaySetting1_HipWeaponMagnitudeX;
		float idleSwaySetting1_HipWeaponMagnitudeY;
		float idleSwaySetting1_HipWeaponMagnitudeZ;
		float idleSwaySetting1_HipWeaponMagnitudeF;
		float idleSwaySetting1_HipWeaponRotationOffset;
		float idleSwaySetting1_HipViewMagnitudeX;
		float idleSwaySetting1_HipViewMagnitudeY;
		float idleSwaySetting1_AdsBulletDirScale;
		float idleSwaySetting1_AdsIdleSpeed;
		float idleSwaySetting1_AdsWeaponMagnitudeX;
		float idleSwaySetting1_AdsWeaponMagnitudeY;
		float idleSwaySetting1_AdsWeaponMagnitudeZ;
		float idleSwaySetting1_AdsWeaponMagnitudeF;
		float idleSwaySetting1_AdsWeaponRotationOffset;
		float idleSwaySetting1_AdsViewMagnitudeX;
		float idleSwaySetting1_AdsViewMagnitudeY;
		float idleSwaySetting2_HipBulletDirScale;
		float idleSwaySetting2_HipIdleSpeed;
		float idleSwaySetting2_HipWeaponMagnitudeX;
		float idleSwaySetting2_HipWeaponMagnitudeY;
		float idleSwaySetting2_HipWeaponMagnitudeZ;
		float idleSwaySetting2_HipWeaponMagnitudeF;
		float idleSwaySetting2_HipWeaponRotationOffset;
		float idleSwaySetting2_HipViewMagnitudeX;
		float idleSwaySetting2_HipViewMagnitudeY;
		float idleSwaySetting2_AdsBulletDirScale;
		float idleSwaySetting2_AdsIdleSpeed;
		float idleSwaySetting2_AdsWeaponMagnitudeX;
		float idleSwaySetting2_AdsWeaponMagnitudeY;
		float idleSwaySetting2_AdsWeaponMagnitudeZ;
		float idleSwaySetting2_AdsWeaponMagnitudeF;
		float idleSwaySetting2_AdsWeaponRotationOffset;
		float idleSwaySetting2_AdsViewMagnitudeX;
		float idleSwaySetting2_AdsViewMagnitudeY;
	}; assert_sizeof(AdvancedIdleSettings, 0x98);

	struct BallisticInfo
	{
		float muzzleVelocity;
		float ballisticCoefficient;
		float diameter;
		float mass;
		float gravityFactor;
		float zeroingDistance;
		BallisticInfoCalculated* calculated;
		bool enableBallisticTrajectory;
		int lifeTimeMs;
	}; assert_sizeof(BallisticInfo, 0x28);

	struct HyperBurstInfo
	{
		float fireTimeFraction;
		int coolDownMs;
		bool enabled;
	}; assert_sizeof(HyperBurstInfo, 0xC);

	struct WeaponDef
	{
		const char* szOverlayName;
		XModel* gunXModel;
		XModel* gunXModelLeftHand;
		XModel* gunXModelRightHand;
		XModel* defaultViewModel;
		XModel* defaultWorldModelLeftHand;
		XModel* defaultWorldModelRightHand;
		XModel* worldModel;
		XModel* worldXModelLeftHand;
		XModel* worldXModelRightHand;
		XModel* defaultWorldModel;
		XModel* censorshipWorldModel;
		XModel* censorshipWorldModelLeftHand;
		XModel* censorshipWorldModelRightHand;
		XModel* playerShadowModel;
		XModel* playerShadowModelLeftHand;
		XModel* playerShadowModelRightHand;
		XModel* handXModel;
		WeaponAnimPackage* szXAnims;
		WeaponAnimPackage* szXAnimsRightHanded;
		WeaponAnimPackage* szXAnimsLeftHanded;
		XAnimParts* turretRaiseAnim;
		XAnimParts* turretIdleAnim;
		XAnimParts* turretFireAnim;
		WeaponDamageInfo damageInfo;
		char __pad0[0x18];
		const char* szModeName;
		scr_string_t* notetrackRumbleMapKeys;
		scr_string_t* notetrackRumbleMapValues;
		scr_string_t* notetrackFXMapKeys;
		FxCombinedDef* notetrackFXMapValues;
		scr_string_t* notetrackFXMapTagValues;
		char __pad1[0x88];
		WeaponVFXPackage* vfxPackage;
		WeaponSFXPackage* sfxPackage;
		char __pad2[0x10];
		Material* reticleCenter;
		Material* reticleSide;
		Material* reticleOnePiece;
		char __pad3[0x120];
		XModel* worldClipModel;
		XModel* rocketModel;
		GfxImage* iconImages[5];
		Material* iconMaterials[3];
		char __pad4[0x18];
		const char* szAmmoName;
		char __pad5[0x98];
		const char* szSharedAmmoCapName;
		char __pad6[0x68];
		ADSOverlay overlay;
		char __pad7[0xD0];
		SwaySettings swaySettings;
		char __pad8[0xC0];
		XModel* projectileModel;
		char __pad9[0x8];
		Curve* projectileCurve;
		FxCombinedDef projExplosionEffect;
		FxCombinedDef projDudEffect;
		FxCombinedDef projTimeoutEffect;
		char __pad10[0x30];
		float* parallelBounce;
		float* perpendicularBounce;
		FxCombinedDef vmProjBodyEffect;
		FxCombinedDef projBodyEffect;
		FxCombinedDef projTrailEffect;
		FxCombinedDef projBeaconEffect;
		FxCombinedDef unknownProjEffect;
		char __pad11[0x18];
		FxCombinedDef projIgnitionEffect;
		char __pad12[0x438];
		int numWeaponOffsetPatterns;
		char __pad13[0x4];
		WeaponOffsetPatternDescription* weaponOffsetPatterns;
		int numWeaponOffsetPatternsKickOrSnapDecay;
		char __pad14[0x4];
		WeaponOffsetPatternDescription** weaponOffsetPatternsKickOrSnapDecay;
		char __pad15[0x8];
		WeaponAccuracy* accuracy[2];
		char __pad16[0x68];
		const char* szUseHintString;
		const char* dropHintString;
		char __pad17[0x30];
		const char* szScript;
		char __pad18[0x30];
		Beam* beams[2];
		char __pad19[0x38];
		float* locationDamageMultipliers;
		unsigned char* hitLocPriorityMap;
		char __pad20[0x20];
		RumbleInfo* fireRumble;
		RumbleInfo* meleeImpactRumble;
		TriggerEffect* triggerEffects[2];
		char __pad21[0x18];
		TracerDef* tracerType;
		TracerDef* overchargeTracerType;
		LaserDef* laserTypeViewModel;
		LaserDef* laserTypeViewModelAlt;
		LaserDef* laserTypeFriendly;
		LaserDef* laserTypeEnemy;
		char __pad22[0x20];
		SndAliasLookup turretOverheatSound;
		FxCombinedDef turretOverheatEffect;
		RumbleInfo* turretBarrelSpinRumble;
		char __pad23[0x10];
		SndAliasLookup turretBarrelSpinMaxSnd;
		SndAliasLookup turretBarrelSpinUpSnd[4];
		SndAliasLookup turretBarrelSpinDownSnd[4];
		SndAliasLookup missileConeSoundAlias;
		SndAliasLookup missileConeSoundAliasAtBase;
		char __pad24[0x78];
		Gesture* shieldDeployGesture;
		Gesture* shieldFireWeapGesture;
		Gesture* shieldDeployWhileFiring;
		Gesture* shieldRetractWhileFiring;
		Gesture* shieldBashGesture;
		FxCombinedDef shieldMeleeFx;
		char __pad25[0x10];
		RumbleInfo* shieldHitRumble;
		char __pad26[0x10];
		RumbleInfo* shieldMissRumble;
		char __pad27[0xA0];
		XModel* detonatorWorldModel;
		char __pad28[0xD8];
		Gesture* gestureAnimation;
		char __pad29[0x8];
		Gesture* gesturePullback;
		Gesture* gesturePullbackAlt;
		char __pad30[0x10];
		Gesture* gestureThrow;
		Gesture* gestureThrowAlt;
		char __pad31[0x8];
		Gesture* gestureDetonate;
		Gesture* gestureDetonateAlt;
		char __pad32[0x8];
		FxCombinedDef cameraFireEffect;
		char __pad33[0x30];
		FxCombinedDef chargeMeterEffect;
		SndAliasLookup chargeUpSound;
		SndAliasLookup chargeDownSound;
		SndAliasLookup chargeUpSoundPlayer;
		SndAliasLookup chargeDownSoundPlayer;
		SndAliasLookup chargeDownToUpSound;
		SndAliasLookup chargeDownToUpSoundPlayer;
		SndAliasLookup chargeUpToDownSound;
		SndAliasLookup chargeUpToDownSoundPlayer;
		SndAliasLookup chargeMaxSound;
		SndAliasLookup chargeMaxSoundPlayer;
		char __pad34[0x8];
		RumbleInfo* chargeRumble;
		scr_string_t stowTag;
		char __pad35[0x4];
		XModel* stowOffsetModel;
		scr_string_t stowOffsetAttachTag;
		char __pad36[0x6C];
		FxCombinedDef deployEffect;
		char __pad37[0x10];
		unsigned int numAnimOverrides;
		char __pad38[0x4];
		AnimOverride* animOverrides;
		CarryAnimOverride carryAnimOverrides[5];
		unsigned int numSfxOverrides;
		char __pad39[0x4];
		SFXOverride* sfxOverrides;
		unsigned int numVfxOverrides;
		char __pad40[0x4];
		VFXOverride* vfxOverrides;
		char __pad41[0xA8];
		Curve* mountCurves[2];
		RumbleInfo* mountRumble;
		WeaponOffsetPatternScaleInfo mountWeaponOffsetPatternScaleInfo;
		char __pad42[0xB8];
		WeaponOffsetPatternScaleInfo crouchedWeaponOffsetPatternScaleInfo;
		WeaponOffsetPatternScaleInfo proneWeaponOffsetPatternScaleInfo;
		BallisticInfo ballisticInfo;
		char __pad43[0x10];
		WeaponEntityNotify* notifyTypes[2];
		char __pad44[0xC8];
		WeaponTrigger* weaponTrigger;
		char __pad45[0x10];
		ReactiveStageSet* reactiveStageSet;
		char __pad46[0x8];
	}; assert_sizeof(WeaponDef, 0x16F0);

	struct NoteTrackToSoundEntry
	{
		unsigned int numSoundMappings;
		scr_string_t attachment;
		scr_string_t* notetrackSoundMapKeys;
		scr_string_t* notetrackSoundMapValues;
	}; assert_sizeof(NoteTrackToSoundEntry, 0x18);

	enum SuitAnimType : std::int32_t
	{
		ANIM_SUIT_HUMAN = 0x0,
		ANIM_SUIT_ROBOTIC = 0x1,
		ANIM_SUIT_C8 = 0x2,
		ANIM_SUIT_REAPER = 0x3,
		NUM_ANIM_SUIT_STATE = 0x4,
		ANIM_SUIT_INVALID = 0x4,
	};

	struct NoteTrackToSuitSoundEntry
	{
		unsigned int numSoundMappings;
		SuitAnimType suitAnimState;
		scr_string_t* notetrackSoundMapKeys;
		scr_string_t* notetrackSoundMapValues;
	}; assert_sizeof(NoteTrackToSuitSoundEntry, 0x18);

	enum WeaponOffsetBlendCurveId : std::int32_t
	{
		WOBC_HOLD_FIRE_BLEND_SLOW = 0x0,
		WOBC_HOLD_FIRE_BLEND_FAST = 0x1,
		WOBC_KICK_BLEND = 0x2,
		WOBC_SNAP_DECAY_BLEND = 0x3,
		WOBC_ADS_BLEND = 0x4,
		WOBC_ALWAYS_ON = 0x5,
		NUM_WEAPON_OFFSET_BLEND_CURVES = 0x6,
	};

	enum WeaponOffsetPatternId : std::int32_t
	{
		WOP_KEYFRAME = 0x0,
		WOP_NOISY_SINE = 0x1,
		WOP_RANDOM_SQUARE = 0x2,
		WOP_SINE = 0x3,
		NUM_WEAPON_OFFSET_PATTERNS = 0x4,
	};

	enum WeaponOffsetTransformType : std::int32_t
	{
		WOTT_VIEW_ORIGIN = 0x0,
		WOTT_VIEW_ANGLES = 0x1,
		WOTT_WEAPON_ORIGIN = 0x2,
		WOTT_WEAPON_ANGLES = 0x3,
		NUM_WEAPON_OFFSET_TRANSFORM_TYPES = 0x4,
	};

	struct WeaponOffsetPatternDescription
	{
		bool active;
		scr_string_t patternKey;
		WeaponOffsetBlendCurveId curveType;
		WeaponOffsetPatternId patternType;
		WeaponOffsetTransformType transformType;
		float frequency;
		float blendTime;
		vec3_t magnitude;
		float hipScale;
		float rotationOffset;
		float bulletDirScale;
		float fullAutoScale;
		int fullAutoBullets;
		float fullAutoDecay;
		int referenceIndex;
		int kickOrSnapDecayIndex;
	}; assert_sizeof(WeaponOffsetPatternDescription, 0x48);

	struct AnimOverride
	{
		unsigned int numBindings;
		OverrideBinding* bindings;
		WeaponAnimPackage* overrides;
		WeaponAnimPackage* overridesAlt;
	}; assert_sizeof(AnimOverride, 0x20);

	struct SFXOverride
	{
		unsigned int numBindings;
		OverrideBinding* bindings;
		WeaponSFXPackage* overrides;
		WeaponSFXPackage* overridesAlt;
	}; assert_sizeof(SFXOverride, 0x20);

	struct VFXOverride
	{
		unsigned int numBindings;
		OverrideBinding* bindings;
		WeaponVFXPackage* overrides;
		WeaponVFXPackage* overridesAlt;
	}; assert_sizeof(VFXOverride, 0x20);

	enum WeaponEntityNotifyFlags : std::int32_t
	{
		WEAP_NOTIFY_FLAG_NONE = 0x0,
		WEAP_NOFITY_FLAG_CHECK_TEAM = 0x1,
		WEAP_NOFITY_FLAG_CHECK_VISIBLE = 0x2,
	};

	struct WeaponEntityNotify
	{
		WeaponEntityNotifyFlags flags;
		float radius;
		float height;
		float minSpeed;
	}; assert_sizeof(WeaponEntityNotify, 0x10);

	struct WeaponOffsetPatternScale
	{
		scr_string_t patternKey;
		vec3_t scale;
	}; assert_sizeof(WeaponOffsetPatternScale, 0x10);

	struct BallisticInfoCalculated
	{
		float* distances;
		float zeroingAngle;
		int numDistanceEntries;
	}; assert_sizeof(BallisticInfoCalculated, 0x10);

	struct OverrideBinding
	{
		unsigned int numBindingLists;
		OverrideBindingList* bindingLists;
		int attachmentVarIndex;
	}; assert_sizeof(OverrideBinding, 0x18);

	enum AttachmentSlot : std::int32_t
	{
		ATT_SLOT_RECEIVER = 0x0,
		ATT_SLOT_FRONT_PIECE = 0x1,
		ATT_SLOT_BACK_PIECE = 0x2,
		ATT_SLOT_MAGAZINE = 0x3,
		ATT_SLOT_OVERRIDE = 0x4,
		ATT_SLOT_MUZZLE = 0x5,
		ATT_SLOT_REAR_GRIP = 0x6,
		ATT_SLOT_TRIGGER = 0x7,
		ATT_SLOT_EXTRA = 0x8,
		ATT_SLOT_SCOPE = 0x9,
		ATT_SLOT_UNDERBARREL = 0xA,
		ATT_SLOT_MODIFIER = 0xB,
		ATT_SLOT_VISUAL = 0xC,
		ATT_SLOT_OTHER = 0xD,
		ATT_SLOT_COUNT = 0xE,
		ATT_SLOT_PRIMARY_COUNT = 0xD,
		ATT_SLOT_OTHER_COUNT = 0x10,
	};

	struct OverrideBindingList
	{
		AttachmentSlot slot;
		unsigned int attachments[16];
	}; assert_sizeof(OverrideBindingList, 0x44);

	struct WeaponVFXPackageDefs
	{
		FxCombinedDef viewFlashEffect;
		FxCombinedDef worldFlashEffect;
		FxCombinedDef viewFlashADSEffect;
		FxCombinedDef viewFlashFirstEffect;
		FxCombinedDef worldFlashFirstEffect;
		FxCombinedDef viewFlashFirstADSEffect;
		FxCombinedDef viewFlashOverchargeEffect;
		FxCombinedDef viewFlashOverchargeADSEffect;
		FxCombinedDef worldFlashOverchargeEffect;
		FxCombinedDef viewShellEjectEffect;
		FxCombinedDef worldShellEjectEffect;
		FxCombinedDef viewLastShotEjectEffect;
		FxCombinedDef worldLastShotEjectEffect;
		FxCombinedDef viewMagEjectEffect;
		FxCombinedDef viewMuzzleSmokeEffect;
		FxCombinedDef worldMuzzleSmokeEffect;
		FxCombinedDef viewBarrelSmokeEffect;
		FxCombinedDef worldBarrelSmokeEffect;
		FxCombinedDef viewEjectionPortSmokeEffect;
		FxCombinedDef worldEjectionPortSmokeEffect;
		FxCombinedDef viewDlcEffect0;
		FxCombinedDef worldDlcEffect0;
		FxCombinedDef viewDlcEffect1;
		FxCombinedDef worldDlcEffect1;
		FxCombinedDef viewDlcEffect2;
		FxCombinedDef worldDlcEffect2;
		FxCombinedDef viewDlcEffect3;
		FxCombinedDef worldDlcEffect3;
	}; assert_sizeof(WeaponVFXPackageDefs, 0xE0);

	struct WeaponVFXPackage
	{
		const char* name;
		WeaponVFXPackageDefs* vfx;
	}; assert_sizeof(WeaponVFXPackage, 0x10);

	enum VectorFieldType : std::int32_t
	{
		VECTOR_FIELD_TYPE_NONE = 0x0,
		VECTOR_FIELD_TYPE_FORCE = 0x1,
		VECTOR_FIELD_TYPE_VELOCITY = 0x2,
		VECTOR_FIELD_NUM_TYPES = 0x3,
	};

	struct VectorSubField
	{
		vec4_t* linearData;
		ExtentBounds worldBounds;
		vec3_t ds;
		unsigned int numEntries[3];
		unsigned int flags;
		VectorFieldType type;
		unsigned int pad[3];
	}; assert_sizeof(VectorSubField, 0x50);

	struct VectorField
	{
		const char* name;
		VectorSubField* subFields;
		ExtentBounds worldBounds;
		vec3_t localOrigin;
		unsigned int numSubFields;
		unsigned int pad[2];
	}; assert_sizeof(VectorField, 0x40);
	assert_offsetof(VectorField, numSubFields, 0x34);

	struct RumbleGraph
	{
		const char* name;
		vec2_t knots[16];
		unsigned short knotCount;
	}; assert_sizeof(RumbleGraph, 0x90);

	struct RumbleInfo
	{
		const char* name;
		int duration;
		float range;
		RumbleGraph* highRumbleGraph;
		RumbleGraph* lowRumbleGraph;
		RumbleGraph* leftTriggerRumbleGraph;
		RumbleGraph* rightTriggerRumbleGraph;
		bool swapTriggerRumblesOnLefty;
		bool panTriggerRumbles;
		bool fadeWithDistance;
		bool broadcast;
	}; assert_sizeof(RumbleInfo, 0x38);

	struct FootstepVFX
	{
		const char* name;
		FxCombinedDef footstepVFX[64];
		FxCombinedDef smallFootstepVFX[64];
	}; assert_sizeof(FootstepVFX, 0x408);

	struct AlwaysloadedFlagSet
	{
		GfxImage** images;
		unsigned int* imageFlags;
		XModelSurfs** modelSurfs;
		unsigned int imageCount;
		unsigned int modelSurfCount;
		unsigned int unknownImageCount;
	}; assert_sizeof(AlwaysloadedFlagSet, 0x28);

	struct TransientSets
	{
		unsigned int count;
		const char** names;
		unsigned int* masks;
	}; assert_sizeof(TransientSets, 0x18);

	struct StreamingItemContents
	{
		unsigned short* contents;
	};

	struct StreamingContentSet
	{
		unsigned short* contentCounts;
		StreamingItemContents* contents;
	}; assert_sizeof(StreamingContentSet, 0x10);

	struct StreamingItemCost
	{
		unsigned int costInBytes;
	};

	struct TransientCosts
	{
		StreamingContentSet streamContents;
		StreamingItemCost* streamingCosts;
		unsigned short* residentPageSizes;
		unsigned short* tempPageSizes;
		unsigned int streamingItemCount;
		unsigned int transientCostCount;
	}; assert_sizeof(TransientCosts, 0x30);

	struct ForcedStubAssetReference
	{
		const char* name;
		char type;
	}; assert_sizeof(ForcedStubAssetReference, 0x10);

	struct TransientFileReference
	{
		const char* name;
		char type;
	}; assert_sizeof(TransientFileReference, 0x10);

	struct TransientAssetReference
	{
		const char* name;
		char type;
		unsigned int mpParentIndex;
	}; assert_sizeof(TransientAssetReference, 0x10);

	struct TransientWorldGridTileMap
	{
		unsigned short* spaceToTransientIndex;
		unsigned int indexCount;
		unsigned int level;
		unsigned int lodCount;
		unsigned int gridCountX;
		unsigned int gridCountY;
		float cellSize;
		float minX;
		float minY;
	}; assert_sizeof(TransientWorldGridTileMap, 0x28);

	struct bitarray_dynamic
	{
		unsigned int* array;
		unsigned int wordCount;
		unsigned int bitCount;
	}; assert_sizeof(bitarray_dynamic, 0x10);

	struct TransientInfo
	{
		TransientSets transientSets;
		TransientCosts transientCosts;
		ForcedStubAssetReference* forcedStubAssets;
		TransientFileReference* transientFiles;
		TransientAssetReference* transientAssets;
		TransientWorldGridTileMap* transientWorldGridTileMaps;
		SpatialPartition_Tree* transientWorldGridSpatial;
		Bounds* transientWorldGridBounds;
		unsigned short* transientWorldGridHighLodLookup;
		unsigned int transientWorldGridBoundsCount;
		unsigned int transientWorldGridHighLodCount;
		unsigned int transientWorldGridHighLodLookupCount;
		unsigned char transientInfoFlags;
		bitarray_dynamic transientSkipLoading;
		unsigned int forcedStubAssetCount;
		unsigned int transientFileCount;
		unsigned int transientAssetCount;
		unsigned int transientWorldGridTileMapCount;
		unsigned int transientCollisionTileCount;
		unsigned int transientMPCommonCount;
	}; assert_sizeof(TransientInfo, 0xB8);
	assert_offsetof(TransientInfo, transientSkipLoading, 0x90);

	struct StreamingInfo
	{
		const char* name;
		AlwaysloadedFlagSet alwaysloadedFlagSet;
		TransientInfo* transientInfo;
	}; assert_sizeof(StreamingInfo, 0x38);

	enum VehiclePhysicsNetcodeType : std::int32_t
	{
		VEH_NETCODE_CLIENT_PRED = 0x0,
		VEH_NETCODE_CLIENT_AUTH = 0x1,
		VEH_NETCODE_SERVER_AUTH = 0x2,
		VEH_NETCODE_CLIENT_PRED_SB = 0x3,
	};

	enum VehiclePhysicsAnimProfile : std::int32_t
	{
		VEH_ANIMPROFILE_ATANGO = 0x0,
		VEH_ANIMPROFILE_TROMEO = 0x1,
		VEH_ANIMPROFILE_COSCAR = 0x2,
		VEH_ANIMPROFILE_STANGO = 0x3,
		VEH_ANIMPROFILE_VINDIA = 0x4,
		VEH_ANIMPROFILE_BROMEO = 0x5,
		VEH_ANIMPROFILE_BUNIFORM = 0x6,
		VEH_ANIMPROFILE_SKILO = 0x7,
		VEH_ANIMPROFILE_FOSCAR = 0x8,
		VEH_ANIMPROFILE_LBRAVO = 0x9,
		VEH_ANIMPROFILE_BLIMA = 0xA,
		VEH_ANIMPROFILE_DECHO = 0xB,
		VEH_ANIMPROFILE_TECHO = 0xC,
		VEH_ANIMPROFILE_PINDIA = 0xD,
		VEH_ANIMPROFILE_HINDIA = 0xE,
		VEH_ANIMPROFILE_ASIERRA = 0xF,
		VEH_ANIMPROFILE_TROMEOTRAC = 0x10,
		VEH_ANIMPROFILE_UMIKE = 0x11,
		VEH_ANIMPROFILE_ZUNIFORM = 0x12,
		VEH_ANIMPROFILE_MKILO = 0x13,
		VEH_ANIMPROFILE_PALFA = 0x14,
		VEH_ANIMPROFILE_WHOTEL = 0x15,
		VEH_ANIMPROFILE_FOXTROT = 0x16,
		VEH_ANIMPROFILE_ZOSCAR = 0x17,
		VEH_ANIMPROFILE_SCOOTER = 0x18,
		VEH_ANIMPROFILE_EXPANSION3 = 0x19,
		VEH_ANIMPROFILE_EXPANSION4 = 0x1A,
		VEH_ANIMPROFILE_COUNT = 0x1B,
	};

	enum VehicleWheelSpinBoneType : std::int32_t
	{
		VEH_WHEELSPIN_NONE = 0x0,
		VEH_WHEELSPIN_CHILDBONE = 0x1,
		VEH_WHEELSPIN_BONE = 0x2,
		VEH_WHEELSPIN_COUNT = 0x3,
	};

	struct SkeletonConstraintUnknown0
	{
		scr_string_t unknownString;
		char unknown[0x64];
	}; assert_sizeof(SkeletonConstraintUnknown0, 0x68);

	struct SkeletonConstraintUnknown1
	{
		scr_string_t unknownString;
		char unknown[0x40];
	}; assert_sizeof(SkeletonConstraintUnknown1, 0x44);

	union SkeletonConstraintUnion
	{
		SkeletonConstraintUnknown0 unknown0;
		SkeletonConstraintUnknown1 unknown1;
	};

	struct SkeletonConstraint
	{
		unsigned char type;
		SkeletonConstraintUnion u;
	}; assert_sizeof(SkeletonConstraint, 0x6C);

	struct SkeletonConstraints
	{
		const char* name;
		SkeletonConstraint* constraints;
		unsigned int numConstraints;
	}; assert_sizeof(SkeletonConstraints, 0x18);

	struct VehiclePhysicsSoundGroup
	{
		SndAliasLookup alias;
		float threshold;
		float vmin;
		float vmax;
		float pmin;
		float pmax;
	}; assert_sizeof(VehiclePhysicsSoundGroup, 0x20);

	struct VehiclePhysicsVfxGroup
	{
		FootstepVFX* surfaceEffects;
		FootstepVFX* unknownSurfaceEffects;
		char unknown[0x10];
	}; assert_sizeof(VehiclePhysicsVfxGroup, 0x20);

	struct VehiclePhysicsDef
	{
		char unknown0[0x24];
		scr_string_t physics_wheelBones[12];
		scr_string_t unknownStrings0[2];
		char unknown1[0x4];
		scr_string_t unknownStrings1[2];
		char unknown2[0xF8];
		SndAliasLookup unknownSound0;
		char unknown3[0xE8];
		vec3_t* unknownPoints;
		unsigned int numUnknownPoints;
		char unknown4[0x54];
		scr_string_t unknownStrings2[2];
		char unknown5[0x68];
		SkeletonConstraints* skeletonConstraints;
		SkeletonConstraints* unknownSkeletonConstraints[10];
		char unknown6[0x18];
		VehiclePhysicsSoundGroup soundGroup0;
		char unknown7[0x8];
		VehiclePhysicsSoundGroup soundGroups1[2];
		SndAliasLookup unknownSound1;
		VehiclePhysicsSoundGroup soundGroup2;
		SndAliasLookup unknownSound2;
		VehiclePhysicsSoundGroup soundGroups3[10];
		VehiclePhysicsVfxGroup vfxGroups[12];
	}; assert_sizeof(VehiclePhysicsDef, 0x6E8);
	assert_offsetof(VehiclePhysicsDef, unknownPoints, 0x250);
	assert_offsetof(VehiclePhysicsDef, skeletonConstraints, 0x320);
	assert_offsetof(VehiclePhysicsDef, soundGroup0, 0x390);
	assert_offsetof(VehiclePhysicsDef, soundGroups3, 0x428);
	assert_offsetof(VehiclePhysicsDef, vfxGroups, 0x568);

	struct VehicleUnknownPoints
	{
		char unknown0[0x30];
		vec3_t* points;
		unsigned int numPoints;
		char unknown1[0xC];
	}; assert_sizeof(VehicleUnknownPoints, 0x48);

	struct VehicleUnknownSound
	{
		SndAliasLookup alias;
		char unknown[0x8];
	}; assert_sizeof(VehicleUnknownSound, 0x10);

	struct VehicleUnknownString
	{
		const char* string;
		char unknown[0x10];
	}; assert_sizeof(VehicleUnknownString, 0x18);

	struct VehicleDef
	{
		const char* name;
		char unknown0[0x8];
		const char* unknownString0;
		char unknown1[0x1C];
		scr_string_t unknownStrings0[4];
		char unknown2[0x4];
		const char* unknownString1;
		CameraDef* cameraDef;
		CameraDef* killCamDef;
		char unknown3[0x100];
		const char* unknownString2;
		VehiclePhysicsDef vehiclePhysicsDef;
		char unknown4[0x80];
		FxCombinedDef unknownFx0[2];
		char unknown5[0x1D8];
		const char* turretWeaponName;
		WeaponCompleteDef* turretWeapon;
		char unknown6[0x28];
		SndAliasLookup unknownSounds0[2];
		char unknown7[0x14];
		scr_string_t unknownStrings1[4];
		char unknown8[0x4];
		FxCombinedDef unknownFx1[2];
		Material* unknownMaterials[6];
		char unknown9[0x8];
		SndBankTransient* soundBanks[2];
		SndAliasLookup unknownSounds1[8];
		char unknown10[0x4];
		scr_string_t unknownString3;
		SndAliasLookup unknownSounds2[11];
		char unknown11[0x8];
		SndAliasLookup unknownSounds3[2];
		char unknown12[0x8];
		VehicleUnknownPoints unknownPoints;
		SndAliasLookup unknownSound4;
		char unknown13[0x8];
		SndAliasLookup unknownSounds5[4];
		char unknown14[0x8];
		VehicleUnknownSound unknownSounds6[5];
		VehicleUnknownString unknownStrings2[2];
		const char* unknownStrings3[3];
		char unknown15[0x20];
	}; assert_sizeof(VehicleDef, 0xD78);
	assert_offsetof(VehicleDef, vehiclePhysicsDef, 0x168);
	assert_offsetof(VehicleDef, unknownFx0, 0x8D0);
	assert_offsetof(VehicleDef, turretWeaponName, 0xAB8);
	assert_offsetof(VehicleDef, unknownStrings1, 0xB14);
	assert_offsetof(VehicleDef, unknownMaterials, 0xB38);
	assert_offsetof(VehicleDef, soundBanks, 0xB70);
	assert_offsetof(VehicleDef, unknownString3, 0xBC4);
	assert_offsetof(VehicleDef, unknownSounds3, 0xC28);
	assert_offsetof(VehicleDef, unknownPoints, 0xC40);
	assert_offsetof(VehicleDef, unknownSounds5, 0xC98);
	assert_offsetof(VehicleDef, unknownSounds6, 0xCC0);
	assert_offsetof(VehicleDef, unknownStrings2, 0xD10);
	assert_offsetof(VehicleDef, unknownStrings3, 0xD40);

	struct FxImpactPack
	{
		FxCombinedDef nonflesh[64];
		FxCombinedDef flesh[23];
	}; assert_sizeof(FxImpactPack, 0x2B8);

	struct FxImpactEntry
	{
		FxImpactPack* normal;
		FxImpactPack* exit;
	}; assert_sizeof(FxImpactEntry, 0x10);

	struct FxImpactTable
	{
		const char* name;
		unsigned int impactTypeCount;
		FxImpactEntry* table;
		unsigned int* genericToRegistered;
		int* registeredToGeneric;
	}; assert_sizeof(FxImpactTable, 0x28);

	struct SurfaceFxEntry
	{
		FxCombinedDef baseEffect;
		FxCombinedDef surfaceEffect[64];
	}; assert_sizeof(SurfaceFxEntry, 0x208);

	struct SurfaceFxTable
	{
		const char* name;
		SurfaceFxEntry* table;
		int numTableEntry;
		char* mapPoolBuffer;
		int mapPoolBufferSize;
	}; assert_sizeof(SurfaceFxTable, 0x28);

	struct GfxWrappedRWBuffer
	{
		char __pad0[0x68];
	};

	enum BspVersion : std::int32_t
	{
		BSP_VERSION_NAVPOWER_5_4_0 = 0xA4,
		BSP_VERSION_HAVOK_2016_2_0 = 0xA5,
		BSP_VERSION_HAVOK_VERTEX_ERROR_THRESHOLD = 0xA6,
		BSP_VERSION_EDGE_DATA = 0xA7,
		BSP_VERSION_IR_LIGHT = 0xA8,
		BSP_VERSION_CONTENTS_TO_TOOLS = 0xA9,
		BSP_VERSION_AUDIO_PROPAGATION = 0xAA,
		BSP_VERSION_EDGE_OCTREE = 0xAB,
		BSP_VERSION_HAVOK_2017_1_0 = 0xAC,
		BSP_VERSION_CHANGE_STAIRS_OVERRIDE = 0xAD,
		BSP_VERSION_SUPPORT_ALIGNED_LUMP = 0xAE,
		BSP_VERSION_EDGE_ADJACENCY = 0xAF,
		BSP_VERSION_EDGE_MANTLE_ON_OVER_FLAGS = 0xB0,
		BSP_VERSION_EDGE_QUERY_BUCKETS = 0xB1,
		BSP_VERSION_PHYSICS_DECAL = 0xB2,
		BSP_VERSION_DYNAMIC_EDGES = 0xB3,
		BSP_VERSION_DISK_AUDIO_PA_SPEAKERS = 0xB4,
		BSP_VERSION_DYNAMICLIGHTSETS = 0xB5,
		BSP_VERSION_SKYDOME_KVP_REMOVED = 0xB6,
		BSP_VERSION_DECAL_VOLUME_BLEND_MAP = 0xB7,
		BSP_VERSION_DECAL_VOLUME_NORMAL_BLEND_MODE = 0xB8,
		BSP_VERSION_HAVOK_2018_1_0 = 0xB9,
		BSP_VERSION_UMBRA_OCCLUDER_METADATA = 0xBA,
		BSP_VERSION_DECAL_VOLUME_UV_TRANFORM = 0xBB,
		BSP_VERSION_MDAO_VOLUMES = 0xBC,
		BSP_VERSION_LIGHT_COLOR_INTENSITY = 0xBD,
		BSP_VERSION_DISK_TRIANGULE_SOUP = 0xBE,
		BSP_VERSION_COLLISON_PARTITION_FIX = 0xBF,
		BSP_VERSION_RADIOMETRIC_SCALED_REMOVED = 0xC0,
		BSP_VERSION_TRANSIENT_ASSETS_REMOVED = 0xC1,
		BSP_VERSION_LGV = 0xC2,
		BSP_VERSION_TRANSIENT_GRID = 0xC3,
		BSP_VERSION_INCREASE_MAX_EDGE_OCTREE_NODE = 0xC4,
		BSP_VERSION_TRANSIENT_GLASSDEF = 0xC5,
		BSP_VERSION_LGV_ONLY = 0xC6,
		BSP_VERSION_STATIC_MODEL_COLLECTIONS = 0xC7,
		BSP_VERSION_32BIT_STATIC_MODEL_ID = 0xC8,
		BSP_VERSION_SKIPPED = 0xC9,
		BSP_VERSION_SUPER_TERRAIN = 0xCA,
		BSP_VERSION_EDGE_INDEX_INCREASE = 0xCB,
		BSP_VERSION_MORE_CLIENT_TRIGGERS = 0xCC,
		BSP_VERSION_USHORT_LIGHTMAPINDEX = 0xCD,
		BSP_VERSION_EDGE_ADJACENCY_CORRUPTION_FIX = 0xCE,
		BSP_VERSION_NORMALIZED_REFLECTION_PROBE = 0xCF,
		BSP_VERSION_CLIENT_SCRIPT_TRIGGERS = 0xD0,
		BSP_VERSION_CLIENT_TRIGGER_PRIORITY = 0xD1,
		BSP_VERSION_SURFACE_INCREASE = 0xD2,
		BSP_VERSION_HAVOK_2018_2_0 = 0xD3,
		BSP_VERSION_LIGHT_NEAR_PLANE_BIAS = 0xD4,
		BSP_VERSION_CLIENT_TRIGGER_LOCALIZATION = 0xD5,
		BSP_VERSION_SOUNDCLIP_WEAPON_ITEM_CLIP = 0xD6,
		BSP_VERSION_BSP_CURVATURE = 0xD7,
		BSP_VERSION_COLLISIONTILES = 0xD8,
		BSP_VERSION_TRISOUP_TREE = 0xD9,
		BSP_VERSION_VOLUMETRIC_FOG_HEIGHTFADE = 0xDA,
		BSP_VERSION_RUNTIME_CLUTTER = 0xDB,
		BSP_VERSION_SMODEL_COLLECTIONS_ADAPTIVE = 0xDC,
		BSP_VERSION_COMPRESSED_SUN_SHADOW = 0xDD,
		BSP_VERSION_REFLECTION_PROBE_OCTAHEDRON = 0xDE,
		BSP_VERSION_LIGHT_COLLIMATION_FACTOR = 0xDF,
		BSP_VERSION_SUPER_TERRAIN_LIGHTMAPS = 0xE0,
		BSP_VERSION_TRISOUP_TREE_ARRAYS = 0xE1,
		BSP_VERSION_SMODEL_COLLECTIONS_MODEL_VALID = 0xE2,
		BSP_VERSION_EDGE_ADJACENCY_LAYOUT = 0xE3,
		BSP_VERSION_MAP_ASSETS_TRI_CHART_INFO = 0xE4,
		BSP_VERSION_REFLECTION_PROBE_TRANSIENTS = 0xE5,
		BSP_VERSION_RPROBE_DLS_FLAG = 0xE6,
		BSP_VERSION_SKYLIGHT = 0xE7,
		BSP_VERSION_BSPINFO_EXPANDED = 0xE8,
		BSP_VERSION_SMALLER_COLLISION_TILES = 0xE9,
		BSP_VERSION_RUNTIME_CLUTTER_BOUNDS = 0xEA,
		BSP_VERSION_TRANSIENT_GLASSPIECE = 0xEB,
		BSP_VERSION_EDGE_TRANSIENTS = 0xEC,
		BSP_VERSION_PHYSICS_TRIANGLEOBJ = 0xED,
		BSP_VERSION_DECAL_VOLUME_TRANSIENTS = 0xEE,
		BSP_VERSION_CLIENT_TRIGGER_SOUNDBANK = 0xEF,
		BSP_VERSION_SMODEL_COLLECTIONS_MODEL_VALID2 = 0xF0,
		BSP_VERSION_SIMPLE_LAYERING = 0xF1,
		BSP_VERSION_CRC_FLUSH = 0xF2,
		BSP_VERSION_VOLUMETRIC_ANISO_COLOR = 0xF3,
		BSP_VERSION_WORLD_TRANSIENT_METADATA = 0xF4,
		BSP_VERSION_SUN_VOLUME = 0xF5,
		BSP_VERSION_INVALID = -1,
		OLDEST_BSP_VERSION = 0xCF,
		BSP_VERSION = 0xF3,
	};

	struct GfxWorldUnknownListEntry
	{
		char unknown[0x2E];
		unsigned short count;
		unsigned short* data;
	}; assert_sizeof(GfxWorldUnknownListEntry, 0x38);

	struct GfxWorldUnknownList
	{
		unsigned int count;
		GfxWorldUnknownListEntry* entries;
	}; assert_sizeof(GfxWorldUnknownList, 0x10);

	struct GfxSurface
	{
		unsigned int posOffset;
		unsigned int baseIndex;
		unsigned int firstTriCluster;
		unsigned short triCount;
		unsigned short vertexCount;
		unsigned int ugbIndex;
		unsigned int materialIndex;
		unsigned short unknown0;
		unsigned char unknown1;
		unsigned char flags;
		unsigned short unknown2;
		unsigned short unknown3;
	}; assert_sizeof(GfxSurface, 0x20);

	struct GfxWorldUgbSurfData
	{
		float xyzScale;
		vec3_t xyzOrigin;
		unsigned int unknown0;
		unsigned short unknown1;
		unsigned int unknown2;
		unsigned int layerCount;
		unsigned int vertexCount;
		unsigned int xyzOffset;
		unsigned int tangentFrameOffset;
		unsigned int lmapCoordOffset;
		unsigned int colorOffset;
		unsigned int texCoordOffset;
		unsigned int unknownOffset;
		unsigned int normalTransformOffset[7];
		unsigned int unknownOffsets[8];
		char unknown3[8];
	}; assert_sizeof(GfxWorldUgbSurfData, 0x80);

	struct GfxWorldSurfData
	{
		char unknown[0x5C];
	}; assert_sizeof(GfxWorldSurfData, 0x5C);

	struct GfxWorldUnknownC
	{
		char unknown[0xC];
	}; assert_sizeof(GfxWorldUnknownC, 0xC);

	struct GfxWorldUnknown10
	{
		char unknown[0x10];
	}; assert_sizeof(GfxWorldUnknown10, 0x10);

	struct GfxWorldUnknown14
	{
		char unknown[0x14];
	}; assert_sizeof(GfxWorldUnknown14, 0x14);

	struct GfxSurfaceLodGroup
	{
		unsigned int lowSurface;
		unsigned int highSurface;
		unsigned int highSurfaceCount;
		float unknown[2];
	}; assert_sizeof(GfxSurfaceLodGroup, 0x14);

	struct GfxWorldUnknown20
	{
		char unknown[0x20];
	}; assert_sizeof(GfxWorldUnknown20, 0x20);

	struct GfxWorldUnknown70
	{
		StreamKey* streamKey;
		char unknown[0x68];
	}; assert_sizeof(GfxWorldUnknown70, 0x70);

	struct GfxWorldUnknown80
	{
		char unknown[0x80];
	}; assert_sizeof(GfxWorldUnknown80, 0x80);

	struct GfxWorldSurfacesUnknownIndices
	{
		char unknown[8];
		unsigned int countA;
		unsigned int countB;
		char unknown2[0x10];
		unsigned int* dataA;
		unsigned int* dataB;
	}; assert_sizeof(GfxWorldSurfacesUnknownIndices, 0x30);

	struct GfxWorldSurfaces
	{
		unsigned int count;
		unsigned int ugbCount;
		unsigned int materialCount;
		unsigned int surfDataCount;
		char unknown[0x10];
		unsigned int litOpaqueSurfsBegin;
		unsigned int litOpaqueSurfsEnd;
		unsigned int litDecalSurfsBegin;
		unsigned int litDecalSurfsEnd;
		unsigned int litTransSurfsBegin;
		unsigned int litTransSurfsEnd;
		unsigned int emissiveSurfsBegin;
		unsigned int emissiveSurfsEnd;
		char unknown3[0x10];
		GfxWorldSurfData* surfData;
		GfxSurface* surfaces;
		GfxWrappedBuffer surfacesBuffer;
		unsigned short* surfaceUnknown;
		Bounds* surfaceBounds;
		GfxWrappedBuffer surfaceBoundsBuffer;
		Material** materials;
		unsigned int* materialRuntime;
		GfxWorldUgbSurfData* ugbData;
		GfxWorldUnknown10* ugbData2;
		GfxWorldUnknown80* ugbData3;
		GfxWrappedRWBuffer ugbData3Buffer;
		GfxWorldUnknownC* surfaceData;
		GfxWrappedRWBuffer surfaceDataBuffer;
		GfxWorldSurfacesUnknownIndices indices;
		unsigned int unknownCountA;
		unsigned int unknownCountB;
		GfxWorldUnknown20* unknownB;
		GfxWorldUnknown70* unknownA;
		unsigned int* unknownABits;
		unsigned int lodGroupCount;
		GfxSurfaceLodGroup* lodGroups;
		char unknown2[0x10];
		unsigned int unknownCountD;
		unsigned int* materialData;
		GfxWorldUnknown10* unknownD;
		GfxWrappedBuffer unknownDBuffer;
	}; assert_sizeof(GfxWorldSurfaces, 0x2B8);
	assert_offsetof(GfxWorldSurfaces, surfaceUnknown, 0xA0);
	assert_offsetof(GfxWorldSurfaces, materials, 0xF0);
	assert_offsetof(GfxWorldSurfaces, ugbData3Buffer, 0x118);
	assert_offsetof(GfxWorldSurfaces, indices, 0x1F0);
	assert_offsetof(GfxWorldSurfaces, unknownCountA, 0x220);
	assert_offsetof(GfxWorldSurfaces, lodGroupCount, 0x240);
	assert_offsetof(GfxWorldSurfaces, unknownCountD, 0x260);
	assert_offsetof(GfxWorldSurfaces, unknownDBuffer, 0x278);

	struct GfxStaticModelSurface
	{
		unsigned short smodelIndex;
		unsigned char lodIndex;
		unsigned char surfIndex;
	}; assert_sizeof(GfxStaticModelSurface, 0x4);

	struct GfxStaticModel
	{
		XModel* model;
		unsigned char flags;
		unsigned char firstMtlSkinIndex;
		unsigned short firstStaticModelSurfaceIndex;
	}; assert_sizeof(GfxStaticModel, 0x10);

	struct GfxClutterCollection
	{
		unsigned int maxInstanceCount;
		unsigned short smodelCollectionIndex;
		unsigned short terrainSurfaceIndex;
		unsigned short clutterLayerIndex;
		unsigned short pad;
	}; assert_sizeof(GfxClutterCollection, 0xC);

	struct GfxStaticModelCollection
	{
		unsigned int firstInstance;
		unsigned int instanceCount;
		unsigned short smodelIndex;
		unsigned short transientGfxWorldPlaced;
		unsigned short clutterIndex;
		unsigned char flags;
		char pad;
	}; assert_sizeof(GfxStaticModelCollection, 0x10);

	struct GfxStaticModelInstance
	{
		unsigned int lgvIndex_transientIndex;
	}; assert_sizeof(GfxStaticModelInstance, 0x4);

	struct GfxModelStaticIndirection
	{
		unsigned int lightingInstanceIndirection;
		unsigned short splinedModelInstanceIndex;
		unsigned short worldTransient;
	}; assert_sizeof(GfxModelStaticIndirection, 0x8);

	struct GfxLightmapTransformPacked
	{
		unsigned int topLeft_indexLow_packed;
		unsigned int bottomRight_indexHigh_packed;
	}; assert_sizeof(GfxLightmapTransformPacked, 0x8);

	struct GfxBaseSModelSurfData
	{
		vec4_t boundingBoxData;
		vec3_t boundingBoxHalfSize;
		float drawDistance;
	}; assert_sizeof(GfxBaseSModelSurfData, 0x20);

	struct GfxSModelInstanceData
	{
		int translation[3];
		unsigned short orientation[4];
		float scale;
	}; assert_sizeof(GfxSModelInstanceData, 0x18);

	struct GfxClutterCollectionGPUData
	{
		unsigned int firstInstance;
		unsigned int instanceCount;
	}; assert_sizeof(GfxClutterCollectionGPUData, 0x8);

	struct GfxStaticModelLOD
	{
		unsigned char surfOffset;
		unsigned char surfCount;
	};

	struct GfxStaticModelExpansion
	{
		unsigned short firstStaticModelSurfaceIndex;
		unsigned char numSurfs;
		unsigned char numLods;
		Bounds modelBounds;
		GfxStaticModelLOD lodInfo[7];
		float lodDist[7];
	}; assert_sizeof(GfxStaticModelExpansion, 0x48);

	enum GfxSModelSurfCullType : std::uint8_t
	{
		SMODEL_SURF_CULL_BACK_FACE = 0x1,
		SMODEL_SURF_CULL_OCCLUSION = 0x2,
		SMODEL_SURF_CULL_FRUSTUM = 0x4,
		SMODEL_SURF_CULL_SMALL_TRIANGLE = 0x8,
		SMODEL_SURF_CULL_DECAL_DEPTH_BIAS = 0x10,
		SMODEL_SURF_CULL_ENABLED = 0x20,
		SMODEL_SURF_CULL_CLUSTER = 0x40,
		SMODEL_SURF_CULL_DOUBLE_SIDED = 0x6E,
		SMODEL_SURF_CULL_DECAL_DOUBLE_SIDED = 0x7E,
		SMODEL_SURF_CULL_OPAQUE = 0xEF,
		SMODEL_SURF_CULL_DECAL = 0xFF,
	};

	enum GfxSModelSurfEmitType : std::uint8_t
	{
		SMODEL_SURF_EMIT_MAIN_VIEW = 0x1,
		SMODEL_SURF_EMIT_SHADOW = 0x2,
		SMODEL_SURF_EMIT_NONE = 0xC,
	};

	struct GfxSModelSurfMatIndirection
	{
		unsigned short smodelMaterialIndex;
		unsigned char cullType;
		unsigned char emitType;
	}; assert_sizeof(GfxSModelSurfMatIndirection, 0x4);

	struct GpuSplinedModelInstance
	{
		float modelXExtent;
		float instanceLength;
		vec3_t modelSplineOrigin;
		float distFromStartNode;
		float modelScale;
		float upDownOffset;
		float leftRightOffset;
		unsigned int segmentBegin;
		unsigned int segmentEnd;
		unsigned int spliningAxisType;
		unsigned int unknown[4];
	}; assert_sizeof(GpuSplinedModelInstance, 0x40);

	struct GpuSplineSegment
	{
		float distFromStartNode;
		float length;
		vec3_t bezierEval[4];
		vec3_t entBezierDerEval[3];
		vec3_t entToAligned[3];
		vec3_t modelToWld[3];
		vec3_t modelToWld_origin;
		vec4_t bankAngleBezierEval;
		vec4_t unknownQuat[2];
	}; assert_sizeof(GpuSplineSegment, 0xE0);

	struct GfxWorldStaticModels
	{
		unsigned int surfaceCount;
		unsigned int modelCount;
		unsigned int collectionCount;
		unsigned int clutterCollectionCount;
		unsigned int instanceCount;
		unsigned int clutterTintCount;
		unsigned int lmapTransformsCount;
		unsigned int materialCount;
		char unknown[0x18];
		unsigned int collectionVisDataCount;
		unsigned int splinedModelInstanceCount;
		unsigned int splineSegmentCount;
		char unknown2[0x1C];
		GfxStaticModelSurface* surfaces;
		GfxStaticModel* models;
		GfxClutterCollection* clutterCollections;
		GfxStaticModelCollection* collections;
		GfxStaticModelInstance* instances;
		unsigned char* instanceFlags;
		Bounds* collectionBounds;
		Material** materials;
		GfxModelStaticIndirection* modelStaticIndirection;
		GfxWrappedBuffer modelStaticIndirectionBuffer;
		GfxLightmapTransformPacked* lmapTransforms;
		GfxWrappedBuffer lmapTransformsBuffer;
		GfxBaseSModelSurfData* smodelSurfData;
		GfxWrappedBuffer smodelSurfDataBuffer;
		GfxSModelInstanceData* smodelInstanceData;
		GfxWrappedRWBuffer smodelInstanceDataBuffer;
		GfxClutterCollectionGPUData* clutterCollectionGPUData;
		GfxWrappedRWBuffer clutterCollectionGPUDataBuffer;
		vec3_t* clutterTintData;
		GfxWrappedRWBuffer clutterTintDataBuffer;
		GfxStaticModelExpansion* smodelExpansionData;
		GfxWrappedBuffer smodelExpansionDataBuffer;
		GfxSModelSurfMatIndirection* smodelSurfMatIndirection;
		GfxWrappedBuffer smodelSurfMatIndirectionBuffer;
		GfxWrappedBuffer clutterCollectionsDataBuffer;
		GfxWrappedBuffer smodelCollectionsDataBuffer;
		unsigned int* smodelSurfUGBIndirection;
		GfxWrappedRWBuffer smodelSurfUGBIndirectionBuffer;
		GpuSplinedModelInstance* splinedModelInstanceData;
		GfxWrappedBuffer splinedModelInstanceBuffer;
		GpuSplineSegment* splineSegmentData;
		GfxWrappedBuffer splineSegmentBuffer;
		Bounds* splinedModelInstanceBounds;
		GfxWrappedBuffer splinedModelInstanceBoundsBuffer;
		bitarray_dynamic collectionTransientZonePrevFrameVisibility;
		unsigned int* collectionVisData;
		unsigned char* collectionVisRefCount;
		unsigned int unknownCount;
		char unknown3[0x3000];
		unsigned short* unknownData;
	}; assert_sizeof(GfxWorldStaticModels, 0x3550);
	assert_offsetof(GfxWorldStaticModels, surfaces, 0x60);
	assert_offsetof(GfxWorldStaticModels, smodelInstanceDataBuffer, 0x180);
	assert_offsetof(GfxWorldStaticModels, smodelSurfUGBIndirection, 0x3D8);
	assert_offsetof(GfxWorldStaticModels, splinedModelInstanceData, 0x448);
	assert_offsetof(GfxWorldStaticModels, collectionTransientZonePrevFrameVisibility, 0x520);
	assert_offsetof(GfxWorldStaticModels, unknownCount, 0x540);

	struct GfxDynamicLightsetBFData
	{
		float curValue[3];
		float lastValue[3];
		unsigned char bfType[2];
		unsigned short numRec;
		unsigned int startRec;
	}; assert_sizeof(GfxDynamicLightsetBFData, 0x20);

	struct GfxDynamicLightsetLightInfo
	{
		float baseIntensity[3];
		float lastIntensity[3];
		unsigned int whichPrimary;
	}; assert_sizeof(GfxDynamicLightsetLightInfo, 0x1C);

	struct GfxDynamicLightsetProductGIData
	{
		unsigned short whichDLS;
		unsigned short whichPortal;
	}; assert_sizeof(GfxDynamicLightsetProductGIData, 0x4);

	struct GfxDynamicLightsetUnknown20
	{
		char unknown[0x20];
	}; assert_sizeof(GfxDynamicLightsetUnknown20, 0x20);

	struct GfxDynamicLightsetLMData
	{
		unsigned int numGChunkLM;
		GfxDynamicLightsetGChunk* chunkGLM;
		unsigned int numGChunkLMBF;
		unsigned short* chunkGLMBF;
		unsigned int numLMHave;
		unsigned short* chunkWhichLM;
		unsigned int* chunkLMStart;
		unsigned int numOrigTexels;
		unsigned int* origTexels;
		GfxWrappedBuffer origTexelsBuffer;
		unsigned int* origRecords;
		GfxWrappedBuffer origRecordsBuffer;
		unsigned int numBFLMTexels;
		unsigned int* bfTexels;
		GfxWrappedBuffer bfTexelsBuffer;
		unsigned int numSkipRecords;
		unsigned short* skipRecords;
		GfxWrappedBuffer skipRecordsBuffer;
		unsigned int* palletteRecords;
		GfxWrappedBuffer palletteRecordsBuffer;
		unsigned int* backingChunkData;
		GfxWrappedBuffer backingChunkDataBuffer;
	}; assert_sizeof(GfxDynamicLightsetLMData, 0x200);
	assert_offsetof(GfxDynamicLightsetLMData, numSkipRecords, 0x120);
	assert_offsetof(GfxDynamicLightsetLMData, backingChunkData, 0x1B8);

	struct GfxDynamicLightset
	{
		unsigned int numBasisFunctions;
		GfxDynamicLightsetBFData* basisFunctions;
		unsigned int numBasisFunctionPrimaryLights;
		GfxDynamicLightsetLightInfo* basisFunctionLights;
		unsigned int unknownCount;
		unsigned int* unknownData;
		unsigned int unknownCount2;
		GfxDynamicLightsetUnknown20* unknownData2;
		unsigned int numProductGI;
		GfxDynamicLightsetProductGIData* productGIData;
		GfxDynamicLightsetLMData lmData;
		GfxDynamicLightsetProbeData lgProbeData;
		char unknown[8];
	}; assert_sizeof(GfxDynamicLightset, 0x3F8);
	assert_offsetof(GfxDynamicLightset, lmData, 0x50);
	assert_offsetof(GfxDynamicLightset, lgProbeData, 0x250);

	struct GfxVoxelTree
	{
		Bounds zoneBound;
		int voxelTopDownViewNodeCount;
		int voxelInternalNodeCount;
		GfxVoxelTreeHeader* voxelTreeHeader;
		GfxVoxelTopDownViewNode* voxelTopDownViewNodeArray;
		GfxVoxelInternalNode* voxelInternalNodeArray;
	}; assert_sizeof(GfxVoxelTree, 0x38);

	struct GfxMayhemSelfVis
	{
		unsigned int selfVisBufferSize;
		unsigned char* selfVis;
		GfxWrappedRWBuffer selfVisBuffer;
	}; assert_sizeof(GfxMayhemSelfVis, 0x78);

	struct GfxFrustumLightUnknown
	{
		char unknown[0x18];
	}; assert_sizeof(GfxFrustumLightUnknown, 0x18);

	struct GfxFrustumLights
	{
		unsigned int lightCount;
		unsigned int totalIndicesCount;
		unsigned int totalVerticesCount;
		unsigned int* indicesOffsets;
		unsigned int* verticesOffsets;
		unsigned short* indicesCounts;
		unsigned short* verticesCounts;
		unsigned short* indices;
		unsigned char* vertices;
		GfxWrappedBuffer indexBuffer;
		GfxWrappedBuffer vertexBuffer;
		GfxFrustumLightUnknown* lightUnknown;
	}; assert_sizeof(GfxFrustumLights, 0xC8);

	struct GfxLightViewFrustum
	{
		unsigned char planeCount;
		unsigned char indexCount;
		unsigned char vertexCount;
		vec4_t* planes;
		unsigned char* indices;
		vec3_t* vertices;
	}; assert_sizeof(GfxLightViewFrustum, 0x20);

	struct GfxHeightfield
	{
		Bounds bounds;
		vec4_t lookupMatrix[4];
		GfxImage* image;
	}; assert_sizeof(GfxHeightfield, 0x60);

	struct GfxWorldDpvsUnknown98
	{
		char unknown[0x98];
	}; assert_sizeof(GfxWorldDpvsUnknown98, 0x98);

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelVisDataCount;
		unsigned int surfaceVisDataCount;
		unsigned int count8;
		unsigned int countC;
		unsigned int count10;
		unsigned int count14;
		unsigned int count18;
		unsigned int count1C;
		unsigned int count20;
		unsigned int count24;
		unsigned int count28;
		unsigned int count2C;
		unsigned int* sortedSurfIndex;
		unsigned short* sortedSmodelIndices;
		unsigned int* smodelVisData[37];
		unsigned int* surfaceVisData[37];
		unsigned int* visData8;
		unsigned int* visData14;
		unsigned int* visData18;
		unsigned int* visData28;
		unsigned int* visData2C;
		unsigned int* visData4;
		unsigned int* visData10x0C;
		GfxWorldDpvsUnknown98* unknownData;
		unsigned int* data1C;
		unsigned int* data20;
		unsigned int* data24;
		unsigned int count2E8;
		unsigned int count2EC;
		unsigned int count2F0;
		unsigned int count2F4;
		unsigned int count2F8;
		unsigned int count2FC;
		unsigned int count300;
		unsigned int count304;
		unsigned int* data2E8;
		unsigned int* data2EC;
		unsigned int* data2F8;
		unsigned int* data2FC;
		unsigned int* data300;
		unsigned int* data304;
		char unknown[0x60];
		unsigned short* data2F0[12];
		unsigned short* data2F4[12];
		GfxWorldUnknownC* extraData2F0;
		GfxWorldUnknownC* extraData2F4;
	}; assert_sizeof(GfxWorldDpvsStatic, 0x468);
	assert_offsetof(GfxWorldDpvsStatic, surfaceVisData, 0x168);
	assert_offsetof(GfxWorldDpvsStatic, visData8, 0x290);
	assert_offsetof(GfxWorldDpvsStatic, unknownData, 0x2C8);
	assert_offsetof(GfxWorldDpvsStatic, count2E8, 0x2E8);
	assert_offsetof(GfxWorldDpvsStatic, data2E8, 0x308);
	assert_offsetof(GfxWorldDpvsStatic, data2F0, 0x398);
	assert_offsetof(GfxWorldDpvsStatic, data2F4, 0x3F8);
	assert_offsetof(GfxWorldDpvsStatic, extraData2F0, 0x458);

	struct GfxSky
	{
		GfxImage* images[4];
		char unknown[0xD0];
	}; assert_sizeof(GfxSky, 0xF0);

	struct GfxMapEntLookup
	{
		unsigned int lmapLookup;
		vec3_t probesOffset;
		unsigned int flags;
		unsigned int lgvData;
		int portalGISlot;
	}; assert_sizeof(GfxMapEntLookup, 0x1C);

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
		vec3_t sunFxPosition;
	}; assert_sizeof(sunflare_t, 0x70);

	struct GfxShadowGeometry
	{
		unsigned short count0;
		unsigned short count2;
		unsigned short count4;
		unsigned short count6;
		unsigned short* data0;
		unsigned int* data2;
		unsigned int* data4;
		unsigned int* data6;
	}; assert_sizeof(GfxShadowGeometry, 0x28);

	struct GfxLightAABBNode
	{
		char unknown[8];
	}; assert_sizeof(GfxLightAABBNode, 0x8);

	struct GfxLightAABB
	{
		unsigned short nodeCount;
		unsigned short lightCount;
		unsigned int unknownCount;
		char unknown[0x18];
		GfxLightAABBNode* nodeArray;
		unsigned int* lightArray;
		unsigned short* unknownData;
	}; assert_sizeof(GfxLightAABB, 0x38);

	struct UmbraGate
	{
		unsigned int objID;
		Bounds bounds;
		float closeDistance;
	}; assert_sizeof(UmbraGate, 0x20);

	enum UmbraUserIdType : unsigned int
	{
		UMBRA_USER_ID_SURFACE = 0x0,
		UMBRA_USER_ID_SMODEL_COLLECTION = 0x10000000,
		UMBRA_USER_ID_VOLUMETRIC = 0x30000000,
		UMBRA_USER_ID_PRIMARY_LIGHT = 0x40000000,
		UMBRA_USER_ID_REFLECTION_PROBE = 0x50000000,
		UMBRA_USER_ID_DECAL_VOLUME = 0x60000000,
		UMBRA_USER_ID_SURFACE_LOD_GROUP = 0x70000000,
		UMBRA_USER_ID_TYPE_MASK = 0x70000000,
		UMBRA_USER_ID_INDEX_MASK = 0xFFFFFF,
	};

	enum StaticModelFlags : std::uint8_t
	{
		STATIC_MODEL_FLAG_LIGHTPROBE_LIGHTING = 0x1,
		STATIC_MODEL_FLAG_REACTIVEMOTION = 0x8,
	};

	struct StreamMtlImgInfo
	{
		float minHimipInvSqRadii;
		unsigned short assetIndex;
		unsigned short volumeCount;
	}; assert_sizeof(StreamMtlImgInfo, 0x8);

	struct StreamTreeNode
	{
		StreamKey* volumesKey;
		StreamMtlImgInfo* infos;
		unsigned short childNodes[2];
		unsigned short mtlInfoCount;
		unsigned short imgInfoCount;
		unsigned short transientZone;
		unsigned short unused;
		ExtentBounds aabb;
		float minLomipInvSqRadiiOfChildren;
	}; assert_sizeof(StreamTreeNode, 0x38);

	struct StreamMtlGroup
	{
		StreamKey* volumesKey;
		StreamMtlImgInfo* infos;
		unsigned short mtlInfoCount;
		unsigned short imgInfoCount;
		unsigned short transientZone;
		unsigned short unused;
	}; assert_sizeof(StreamMtlGroup, 0x18);

	struct StreamTree
	{
		StreamTreeNode* nodes;
		Material** materials;
		GfxImage** images;
		unsigned int nodeCount;
		unsigned int materialCount;
		unsigned int imageCount;
		StreamMtlGroup largeMtlGroup;
		unsigned int memorySize;
		vec3_t mins;
		vec3_t maxs;
	}; assert_sizeof(StreamTree, 0x60);

	struct StreamTreeGrid
	{
		StreamKey** cellKeys;
		Material** materials;
		GfxImage** images;
		XModel** xmodels;
		unsigned short* smodelCollectionIndices;
		ExtentBounds bounds;
		unsigned int version;
		unsigned short rowCount;
		unsigned short columnCount;
		unsigned int materialCount;
		unsigned int imageCount;
		unsigned int xmodelCount;
		unsigned int smodelCollectionIndexCount;
	}; assert_sizeof(StreamTreeGrid, 0x58);

	struct WaterSystemUnknown30
	{
		GfxImage* images[6];
	}; assert_sizeof(WaterSystemUnknown30, 0x30);

	struct WaterSystemUnknown4C
	{
		char unknown[0x4C];
	}; assert_sizeof(WaterSystemUnknown4C, 0x4C);

	struct WaterSystemUnknownC
	{
		char unknown[0xC];
	}; assert_sizeof(WaterSystemUnknownC, 0xC);

	struct WaterSystemData
	{
		char unknown[0x1C];
		unsigned int countA;
		unsigned int countB;
		unsigned int count;
		char unknown2[8];
		WaterSystemUnknown30* imageSets;
		unsigned int* grid;
		GfxWrappedBuffer gridBuffer;
		WaterSystemUnknown4C* data;
		GfxWrappedBuffer dataBuffer;
		WaterSystemUnknownC* data2;
	}; assert_sizeof(WaterSystemData, 0xD0);
	assert_offsetof(WaterSystemData, imageSets, 0x30);
	assert_offsetof(WaterSystemData, data, 0x80);
	assert_offsetof(WaterSystemData, data2, 0xC8);

	struct WaterSystem
	{
		const char* name;
		char unknown[8];
		Material* material;
		GfxImage* image;
		WaterSystemData data;
	}; assert_sizeof(WaterSystem, 0xF0);

	struct WaterBuoyancy
	{
		const char* name;
		char unknown[0x500];
	}; assert_sizeof(WaterBuoyancy, 0x508);

	struct KeyBindName
	{
		const char* name;
		char unknown[0x8];
	}; assert_sizeof(KeyBindName, 0x10);

	struct KeyBindCommand
	{
		const char* strings[5];
		char unknown[0x18];
	}; assert_sizeof(KeyBindCommand, 0x40);

	struct KeyBinds
	{
		const char* name;
		int unknown1;
		int nameCount;
		KeyBindName* names;
		int commandCount;
		KeyBindCommand* commands;
		char unknown2[0x808];
	}; assert_sizeof(KeyBinds, 0x830);
	assert_offsetof(KeyBinds, commands, 0x20);

	struct GfxWaterUnknown74
	{
		char unknown[0x74];
	}; assert_sizeof(GfxWaterUnknown74, 0x74);

	struct GfxWaterUnknown2C
	{
		char unknown[0x2C];
	}; assert_sizeof(GfxWaterUnknown2C, 0x2C);

	struct GfxWaterTessellation
	{
		unsigned int count;
		unsigned int count2;
		unsigned char* data;
		unsigned int* data2;
		unsigned int* data3;
		GfxWrappedBuffer buffer;
		GfxWrappedBuffer buffer2;
		GfxWrappedBuffer buffer3;
	}; assert_sizeof(GfxWaterTessellation, 0xE0);

	struct GfxWorldWater
	{
		char unknown[0x28];
		Material* material;
		WaterSystem* waterSystem;
		unsigned int materialCount;
		Material** materials;
		GfxWaterUnknown74* materialData;
		GfxWrappedBuffer materialDataBuffer;
		unsigned int count;
		unsigned int count2;
		GfxWaterTessellation tessellation;
		GfxWaterUnknown2C* data;
		GfxWrappedBuffer dataBuffer;
	}; assert_sizeof(GfxWorldWater, 0x1C0);
	assert_offsetof(GfxWorldWater, count, 0x90);
	assert_offsetof(GfxWorldWater, data, 0x178);

	struct GfxWorldUnknownGrid
	{
		char unknown[0x20];
		GfxImage* image;
		unsigned int dims[3];
		unsigned int dims2[3];
		unsigned int* data;
		unsigned int* data2;
	}; assert_sizeof(GfxWorldUnknownGrid, 0x50);

	struct GfxReflectionProbe
	{
		char* livePath;
		vec3_t origin;
		vec3_t angles;
		unsigned short* probeInstances;
		unsigned short probeInstanceCount;
		unsigned short probeRelightingIndex;
		unsigned int probeImageIndirection;
	}; assert_sizeof(GfxReflectionProbe, 0x30);

	struct GfxReflectionProbeRelightingData
	{
		unsigned int reflectionProbeIndex;
		unsigned short relightingFlags;
		unsigned short zoneCount;
		unsigned int gBufferAlbedoImageIndex;
		unsigned int gBufferNormalImageIndex;
		unsigned int gBufferSecondaryDiffuseImageIndex;
		float relightingScale;
		unsigned short* zones;
	}; assert_sizeof(GfxReflectionProbeRelightingData, 0x20);

	struct GfxReflectionProbeObb
	{
		vec3_t center;
		vec3_t xAxis;
		vec3_t yAxis;
		vec3_t zAxis;
		vec3_t halfSize;
	}; assert_sizeof(GfxReflectionProbeObb, 0x3C);

	enum GfxReflectionProbeInstanceFlags : std::uint8_t
	{
		REFLECTION_PROBE_INSTANCE_NO_PARALLAX = 0x1,
	};

	struct GfxReflectionProbeInstance
	{
		char* livePath;
		vec3_t probePosition;
		unsigned short probeImageIndex;
		unsigned char lodLevel;
		unsigned char flags;
		vec4_t probeRotation;
		GfxReflectionProbeObb volumeObb;
		float priority;
		vec3_t feather;
		vec3_t expandProjectionNeg;
		vec3_t expandProjectionPos;
	}; assert_sizeof(GfxReflectionProbeInstance, 0x90);

	struct GfxSH3Vec
	{
		float coeffs[9];
	}; assert_sizeof(GfxSH3Vec, 0x24);

	struct GfxWorldReflectionProbeData
	{
		unsigned int reflectionProbeCount;
		unsigned int sharedReflectionProbeCount;
		GfxImage** reflectionProbeImages;
		GfxImage** reflectionProbeImages_octahedron;
		GfxReflectionProbe* reflectionProbes;
		GfxImage* reflectionProbeArrayImage;
		GfxImage* reflectionProbeArrayImage_octahedron;
		unsigned int probeRelightingCount;
		GfxReflectionProbeRelightingData* probeRelightingData;
		unsigned int reflectionProbeGBufferImageCount;
		GfxImage** reflectionProbeGBufferImages;
		GfxImage** reflectionProbeGBufferImages_octahedron;
		unsigned int reflectionProbeInstanceCount;
		GfxReflectionProbeInstance* reflectionProbeInstances;
		GfxSH3Vec* reflectionProbeSH;
		GfxWrappedRWBuffer reflectionProbeSHBuffer;
	}; assert_sizeof(GfxWorldReflectionProbeData, 0xD8);

	struct GfxWorldLightmapReindexDataElement
	{
		unsigned int index;
		unsigned int posX;
		unsigned int posY;
		unsigned int width;
		unsigned int height;
	}; assert_sizeof(GfxWorldLightmapReindexDataElement, 0x14);

	struct GfxWorldPackedLightmap
	{
		unsigned int imageWidth;
		unsigned int imageHeight;
	}; assert_sizeof(GfxWorldPackedLightmap, 0x8);

	struct GfxWorldLightmapReindexData
	{
		unsigned int imagePixelSize;
		unsigned int reindexCount;
		GfxWorldLightmapReindexDataElement* reindexElement;
		unsigned int packedLightmapCount;
		GfxWorldPackedLightmap* packedLightmap;
	}; assert_sizeof(GfxWorldLightmapReindexData, 0x20);

	struct GfxDecalVolumeCollection
	{
		unsigned short transientZoneIndex;
		unsigned short decalCount;
		unsigned int firstZoneDecal;
	}; assert_sizeof(GfxDecalVolumeCollection, 0x8);

	struct GfxWorldDecalVolumes
	{
		unsigned int mapDecalVolumeCount;
		unsigned int decalVolumeMaterialCount;
		unsigned int decalVolumeMaskCount;
		unsigned int decalVolumeCollectionCount;
		Material** decalVolumeMaterials;
		GfxDecalVolumeMask** decalVolumeMasks;
		GfxDecalVolumeCollection* decalVolumeCollections;
		unsigned int* materialAndMaskIndirection;
		GfxWrappedBuffer materialAndMaskIndirectionBuffer;
		char unknown[8];
	}; assert_sizeof(GfxWorldDecalVolumes, 0x78);

	enum VolumetricMaskType : std::int32_t
	{
		VOLUMETRIC_MASK_UNUSED = 0x0,
		VOLUMETRIC_MASK_HEIGHTMAP = 0x1,
		VOLUMETRIC_MASK_OPACITYMAP = 0x2,
		VOLUMETRIC_MASK_TYPE_COUNT = 0x3,
	};

	enum VolumetricAxis : std::int32_t
	{
		VOLUMETRIC_X_AXIS = 0x0,
		VOLUMETRIC_Y_AXIS = 0x1,
		VOLUMETRIC_Z_AXIS = 0x2,
		VOLUMETRIC_AXIS_COUNT = 0x3,
		VOLUMETRIC_NEGATVE_AXIS_FLAG = 0x4,
	};

	struct GfxVolumetricMask
	{
		VolumetricMaskType type;
		VolumetricAxis axis;
		GfxImage* image;
		vec2_t scale;
		vec2_t offset;
		vec2_t scroll;
	}; assert_sizeof(GfxVolumetricMask, 0x28);

	struct GfxVolumetric
	{
		char* livePath;
		char unknown[0x70];
		GfxVolumetricMask masks[4];
	}; assert_sizeof(GfxVolumetric, 0x118);
	assert_offsetof(GfxVolumetric, masks, 0x78);

	struct GfxWorldVolumetrics
	{
		unsigned int volumetricCount;
		GfxVolumetric* volumetrics;
		GfxImage* volumetricMaskAtlasImage;
	}; assert_sizeof(GfxWorldVolumetrics, 0x18);

	struct GfxCompressedSunShadow
	{
		unsigned int compressedSunShadowSize;
		unsigned int count;
		unsigned int count2;
		unsigned char* compressedSunShadow;
		GfxWrappedRWBuffer compressedSunShadowBuffer;
		char unknown[0x28];
		unsigned int* data;
		unsigned char* data2;
	}; assert_sizeof(GfxCompressedSunShadow, 0xB8);
	assert_offsetof(GfxCompressedSunShadow, data, 0xA8);

	struct GfxWorld
	{
		const char* name;
		const char* baseName;
		char unknown[8];
		unsigned int unknownVersion;
		unsigned int lastSunPrimaryLightIndex;
		unsigned int primaryLightCount;
		unsigned int firstMutablePrimaryLight;
		unsigned int mutablePrimaryLightCount;
		unsigned int firstStaticScriptablesPrimaryLight;
		unsigned int staticScriptablesPrimaryLightCount;
		unsigned int firstStaticScriptablePrimaryLight;
		unsigned int staticScriptablePrimaryLightCount;
		unsigned int firstMovingScriptablePrimaryLight;
		unsigned int movingScriptablePrimaryLightCount;
		Bounds bounds;
		Bounds unknownBounds;
		char unknown2[4];
		GfxWorldUnknownList unknownList;
		GfxWorldSurfaces surfaces;
		GfxWorldStaticModels smodels;
		GfxDynamicLightset dynamicLightset;
		int voxelTreeCount;
		GfxVoxelTree* voxelTree;
		GfxMayhemSelfVis mayhemSelfVis;
		GfxFrustumLights frustumLights;
		GfxLightViewFrustum* lightViewFrustums;
		int heightfieldCount;
		GfxHeightfield* heightfields;
		GfxWorldDpvsStatic dpvs;
		GfxSky sky;
		unsigned int gfxMapEntLookupCount;
		GfxMapEntLookup* gfxMapEntLookup;
		char unknown3[8];
		sunflare_t sun;
		vec4_t outdoorLookupMatrix[4];
		GfxImage* outdoorImage;
		Material* dustMaterial;
		char unknown4[0x10];
		GfxShadowGeometry* shadowGeomOptimized;
		GfxLightAABB lightAABB;
		unsigned int numUmbraGates;
		UmbraGate* umbraGates;
		unsigned int umbraTomeSize;
		char* umbraTomeData;
		void* umbraTome;
		unsigned int mdaoVolumeCount;
		MdaoVolume* mdaoVolumes;
		bool enableMdao;
		StreamTree streamTree;
		StreamTreeGrid materialStreamTreeGrid;
		StreamTreeGrid xmodelStreamTreeGrid;
		GfxWorldWater water;
		GfxWorldUnknownGrid unknownGrid;
		GfxImage* unknownImage;
		char unknown5[0x18];
		unsigned int unknownCount;
		unsigned int unknownCount2;
		unsigned int unknownCount3;
		GfxWorldUnknown14* unknownData;
		GfxWorldUnknown10* unknownData2;
		GfxWorldUnknown80* unknownData3;
		GfxWorldReflectionProbeData reflectionProbeData;
		GfxWorldLightmapReindexData lightmapReindexData;
		GfxImage* iesLookupTexture;
		GfxWorldDecalVolumes decalVolumes;
		unsigned int unknown6;
		unsigned int unknown7;
		int lightmapCount;
		unsigned int unknown8;
		char unknown9[8];
		GfxWorldTransientZone* transientZones[1536];
		unsigned int* lightmapTransientIndex;
		GfxWorldVolumetrics volumetrics;
		GfxCompressedSunShadow compressedSunShadow;
	}; assert_sizeof(GfxWorld, 0x7A80);
	assert_offsetof(GfxWorld, primaryLightCount, 0x20);
	assert_offsetof(GfxWorld, unknownList, 0x78);
	assert_offsetof(GfxWorld, surfaces, 0x88);
	assert_offsetof(GfxWorld, smodels, 0x340);
	assert_offsetof(GfxWorld, dynamicLightset, 0x3890);
	assert_offsetof(GfxWorld, voxelTreeCount, 0x3C88);
	assert_offsetof(GfxWorld, mayhemSelfVis, 0x3C98);
	assert_offsetof(GfxWorld, frustumLights, 0x3D10);
	assert_offsetof(GfxWorld, lightViewFrustums, 0x3DD8);
	assert_offsetof(GfxWorld, dpvs, 0x3DF0);
	assert_offsetof(GfxWorld, sky, 0x4258);
	assert_offsetof(GfxWorld, gfxMapEntLookupCount, 0x4348);
	assert_offsetof(GfxWorld, sun, 0x4360);
	assert_offsetof(GfxWorld, outdoorImage, 0x4410);
	assert_offsetof(GfxWorld, shadowGeomOptimized, 0x4430);
	assert_offsetof(GfxWorld, numUmbraGates, 0x4470);
	assert_offsetof(GfxWorld, umbraTome, 0x4490);
	assert_offsetof(GfxWorld, mdaoVolumeCount, 0x4498);
	assert_offsetof(GfxWorld, streamTree, 0x44B0);
	assert_offsetof(GfxWorld, xmodelStreamTreeGrid, 0x4568);
	assert_offsetof(GfxWorld, water, 0x45C0);
	assert_offsetof(GfxWorld, unknownGrid, 0x4780);
	assert_offsetof(GfxWorld, unknownImage, 0x47D0);
	assert_offsetof(GfxWorld, unknownCount, 0x47F0);
	assert_offsetof(GfxWorld, reflectionProbeData, 0x4818);
	assert_offsetof(GfxWorld, lightmapReindexData, 0x48F0);
	assert_offsetof(GfxWorld, iesLookupTexture, 0x4910);
	assert_offsetof(GfxWorld, decalVolumes, 0x4918);
	assert_offsetof(GfxWorld, lightmapCount, 0x4998);
	assert_offsetof(GfxWorld, transientZones, 0x49A8);
	assert_offsetof(GfxWorld, lightmapTransientIndex, 0x79A8);
	assert_offsetof(GfxWorld, volumetrics, 0x79B0);
	assert_offsetof(GfxWorld, compressedSunShadow, 0x79C8);

	struct ClientEntityLinkToDef
	{
		int anchorIndex;
		vec3_t originOffset;
		vec3_t angleOffset;
	}; assert_sizeof(ClientEntityLinkToDef, 0x1C);

	struct SpatialPartition_Population_Node
	{
		unsigned int spatialNodeNext;
	}; assert_sizeof(SpatialPartition_Population_Node, 0x4);

	struct SpatialPartition_TransientPopulation
	{
		SpatialPartition_Population_Node* buckets;
		SpatialPartition_Population_Node* bucketTails;
		unsigned int* partitionIds;
		SpatialPartition_Population_Node* (*getNodeFromIndex)(const SpatialPartition_TransientPopulation*, unsigned int);
		unsigned int (*getIndexFromNode)(const SpatialPartition_TransientPopulation*, const SpatialPartition_Population_Node*);
		void* userData;
		unsigned int partitionCount;
		unsigned int currentPopulation;
	}; assert_sizeof(SpatialPartition_TransientPopulation, 0x38);

	enum DynEntityType : std::uint8_t
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_CLUTTER_NOSHADOW = 0x2,
		DYNENT_TYPE_SCRIPTABLEINST = 0x3,
		DYNENT_TYPE_SCRIPTABLEPHYSICS = 0x4,
		DYNENT_TYPE_LINKED = 0x5,
		DYNENT_TYPE_LINKED_NOSHADOW = 0x6,
	};

	struct DynEntityDef
	{
		const XModel* baseModel;
		ClientEntityLinkToDef* linkTo;
		GfxPlacement initialPose;
		SpatialPartition_Population_Node spatialNode;
		char clientActiveMask;
		unsigned char basis;
		DynEntityType type;
		unsigned char priority;
		unsigned int spatialPartitionIndex;
		unsigned short clientId[2];
		unsigned int scriptableMapIndex;
		unsigned char dynEntityListId[2];
		unsigned short brushModel;
		unsigned short scriptableSubIndex;
		unsigned short transientIndexStored;
		union
		{
			char scriptableMaxPartCount;
			char physicsAssetRigidBodiesCount;
		};
		bool spawnActive;
		bool noPhysics;
		bool distantShadows;
		bool noSpotShadows;
		char collisionFlags;
		bool isTransient;
		bool transientZoneLoaded;
	}; assert_sizeof(DynEntityDef, 0x50);

	struct DynEntityList
	{
		const char* name;
		unsigned char index[2];
		unsigned int dynEntCount[2];
		unsigned int dynEntCountTotal;
		DynEntityDef* dynEntDefList[2];
		SpatialPartition_TransientPopulation* dynEntSpatialPopulation[2];
		SpatialPartition_Population_Node dynEntNoSpatialList;
		SpatialPartition_Population_Node dynEntNoSpatialListTail;
	}; assert_sizeof(DynEntityList, 0x40);

	union GfxColor
	{
		unsigned char array[4];
		unsigned int packed;
	};

	struct FxGlassDef
	{
		float halfThickness;
		vec2_t texVecs[2];
		GfxColor color;
		Material* material;
		Material* materialShattered;
		PhysicsAsset* physicsAsset;
		FxCombinedDef pieceBreakEffect;
		FxCombinedDef shatterEffect;
		FxCombinedDef shatterSmallEffect;
		FxCombinedDef crackDecalEffect;
		SndAliasLookup damagedSound;
		SndAliasLookup destroyedSound;
		SndAliasLookup destroyedQuietSound;
		float highMipRadiusInvSq;
		float shatteredHighMipRadiusInvSq;
		int numCrackRings;
		unsigned short transientIndex;
	}; assert_sizeof(FxGlassDef, 0x78);

	struct FxGlassTransientLookup
	{
		unsigned short globalPieceStartIndex;
		unsigned short transientIndex;
	}; assert_sizeof(FxGlassTransientLookup, 0x4);

	struct FxGlassGlobals
	{
		unsigned int defCount;
		FxGlassDef* defs;
		unsigned int glassTransientLookupCount;
		FxGlassTransientLookup* glassTransientLookup;
		float* distances;
	}; assert_sizeof(FxGlassGlobals, 0x28);

	struct FxSpatialFrame
	{
		vec4_t quat;
		vec3_t origin;
	}; assert_sizeof(FxSpatialFrame, 0x1C);

	struct FxGlassPiecePlacement
	{
		FxSpatialFrame frame;
		float radius;
	};

	union FxGlassPiecePlace
	{
		FxGlassPiecePlacement placement;
		unsigned int nextFree;
	}; assert_sizeof(FxGlassPiecePlace, 0x20);

	struct FxGlassPieceState
	{
		vec2_t texCoordOrigin;
		unsigned int supportMask;
		unsigned int geoDataStart;
		unsigned short initIndex;
		unsigned char defIndex;
		char pad[1];
		unsigned char vertCount;
		unsigned char holeDataCount;
		unsigned char crackDataCount;
		unsigned char fanDataCount;
		unsigned short flags;
		float areaX2;
	}; assert_sizeof(FxGlassPieceState, 0x20);

	struct FxGlassPieceDynamics
	{
		int fallTime;
		unsigned int physicsInstance;
		unsigned int physicsDetailInstance;
		vec3_t vel;
		vec3_t avel;
		bool pendingCreation;
	}; assert_sizeof(FxGlassPieceDynamics, 0x28);

	struct FxGlassVertex
	{
		short x;
		short y;
	};

	struct FxGlassHoleHeader
	{
		unsigned short uniqueVertCount;
		unsigned char touchVert;
		char pad[1];
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
	}; assert_sizeof(FxGlassGeometryData, 0x4);

	struct FxGlassInitPieceState
	{
		FxSpatialFrame frame;
		float radius;
		vec2_t texCoordOrigin;
		unsigned int supportMask;
		float areaX2;
		unsigned char defIndex;
		unsigned char vertCount;
		unsigned char fanDataCount;
		char pad[1];
	}; assert_sizeof(FxGlassInitPieceState, 0x34);

	struct FxGlassSystem
	{
		int time;
		int prevTime;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int activePieceCount;
		unsigned int firstFreePiece;
		unsigned int geoDataLimit;
		unsigned int geoDataCount;
		unsigned int initGeoDataCount;
		FxGlassPiecePlace* piecePlaces;
		FxGlassPieceState* pieceStates;
		FxGlassPieceDynamics* pieceDynamics;
		FxGlassGeometryData* geoData;
		unsigned int* isInUse;
		unsigned char* visData;
		float* halfThickness;
		FxGlassGeometryData* initGeoData;
		bool needToCompactData;
		unsigned char initCount;
		float effectChanceAccum;
		int lastPieceDeletionTime;
		unsigned int initPieceCount;
		FxGlassInitPieceState* initPieceStates;
	}; assert_sizeof(FxGlassSystem, 0x80);
	assert_offsetof(FxGlassSystem, piecePlaces, 0x28);
	assert_offsetof(FxGlassSystem, initGeoData, 0x60);
	assert_offsetof(FxGlassSystem, initPieceCount, 0x74);

	struct FxWorldTransientZone
	{
		const char* name;
		unsigned int transientZoneIndex;
		FxGlassSystem glassSys;
	}; assert_sizeof(FxWorldTransientZone, 0x90);

	struct FxWorld
	{
		const char* name;
		FxGlassGlobals glassGlob;
		FxGlassSystem glassSys;
		unsigned int transientZoneCount;
		FxWorldTransientZone* transientZones[1536];
		unsigned short activeTransientZones[1536];
		unsigned short activeTransientZoneCount;
	}; assert_sizeof(FxWorld, 0x3CC0);
	assert_offsetof(FxWorld, transientZones, 0xB8);

	enum EdgeQueryType : std::uint8_t
	{
		EDGE_QUERY_LADDER = 2,
	};

	struct FlaggedEdgeMetadata
	{
		float packedAdjacentNormal;
		unsigned short packedOpenAngle;
		unsigned char flags;
	}; assert_sizeof(FlaggedEdgeMetadata, 0x8);

	struct PMROctreeNode
	{
		unsigned short uniformGridIndex[3];
		char depth;
		unsigned char flags;
		union
		{
			unsigned int parentNodeIndex;
			unsigned int qEdgeTableIndex;
		};
		union
		{
			unsigned int childNodeSetIndex;
			unsigned int qEdgeTableCount;
		};
	}; assert_sizeof(PMROctreeNode, 0x10);

	struct PMROctreeMetadata
	{
		float4 nodeHalfWidth[16];
		float4 minExtent;
		PMROctreeNode rootNode;
		unsigned int nodeSetCount;
		float nodeBoundingRadiusSq[16];
		unsigned char edgeBucket;
		unsigned char flags;
		char pad[10];
	}; assert_sizeof(PMROctreeMetadata, 0x170);

	struct PMROctreeNodeSet
	{
		PMROctreeNode node[8];
	}; assert_sizeof(PMROctreeNodeSet, 0x80);

	struct EdgeAdjacencyMetadata
	{
		unsigned int startIndex : 23;
		unsigned int mountCount : 2;
		unsigned int interactionCount : 2;
	}; assert_sizeof(EdgeAdjacencyMetadata, 0x4);

	struct EdgeAdjacency
	{
		unsigned short start;
		unsigned short end;
		unsigned int adjEdge;
		unsigned short adjStart;
		unsigned short adjEnd;
	}; assert_sizeof(EdgeAdjacency, 0xC);

	struct MapEdgeList
	{
		const char* name;
		vec3_t mins;
		vec3_t maxs;
		unsigned int transientIndex;
		unsigned int dynamicQueryTypes;
		unsigned int staticQueryTypes;
		unsigned int numDynamicEdges;
		unsigned int numStaticEdges;
		bool valid;
		unsigned int numEdges;
		float4 (*edges)[2];
		FlaggedEdgeMetadata* edgeMetadata;
		unsigned int numEdgeOctrees;
		PMROctreeMetadata* edgeOctrees;
		unsigned int numEdgeOctreeNodeSet;
		PMROctreeNodeSet* edgeOctreeNodeSets;
		unsigned int numEdgeIndices;
		unsigned int* edgeIndices;
		unsigned int numEdgeAdjacencyMetadata;
		EdgeAdjacencyMetadata* edgeAdjacencyMetadata;
		unsigned int numEdgeAdjacency;
		EdgeAdjacency* edgeAdjacency;
	}; assert_sizeof(MapEdgeList, 0xA0);
	assert_offsetof(MapEdgeList, edges, 0x40);
	assert_offsetof(MapEdgeList, edgeAdjacency, 0x98);

	struct AIMarkupRecord
	{
		char unknown[0x10];
	}; assert_sizeof(AIMarkupRecord, 0x10);

	struct AIMarkup
	{
		const char* name;
		int recordCount;
		AIMarkupRecord* records;
	}; assert_sizeof(AIMarkup, 0x18);

	struct TriggerModel
	{
		int contents;
		unsigned short hullCount;
		unsigned short firstHull;
		unsigned short windingCount;
		unsigned short firstWinding;
		unsigned int flags;
		PhysicsAsset* physicsAsset;
		unsigned short physicsShapeOverrideIdx;
	}; assert_sizeof(TriggerModel, 0x20);

	struct TriggerHull
	{
		Bounds bounds;
		int contents;
		unsigned short slabCount;
		unsigned short firstSlab;
	}; assert_sizeof(TriggerHull, 0x20);

	struct TriggerSlab
	{
		vec3_t dir;
		float midPoint;
		float halfSize;
	}; assert_sizeof(TriggerSlab, 0x14);

	struct TriggerWinding
	{
		vec3_t dir;
		unsigned short pointCount;
		unsigned short firstPoint;
		unsigned int flags;
	}; assert_sizeof(TriggerWinding, 0x14);

	struct TriggerWindingPoint
	{
		vec3_t loc;
	}; assert_sizeof(TriggerWindingPoint, 0xC);

	struct MapTriggers
	{
		unsigned int count;
		TriggerModel* models;
		unsigned int hullCount;
		TriggerHull* hulls;
		unsigned int slabCount;
		TriggerSlab* slabs;
		unsigned int windingCount;
		TriggerWinding* windings;
		unsigned int windingPointCount;
		TriggerWindingPoint* windingPoints;
	}; assert_sizeof(MapTriggers, 0x50);

	struct ClientTriggerModel
	{
		unsigned short hullCount;
		unsigned int firstHull;
		unsigned int flags;
	}; assert_sizeof(ClientTriggerModel, 0xC);

	struct ClientTriggerHull
	{
		Bounds triggerSpaceBounds;
		unsigned short slabCount;
		unsigned int firstSlab;
	}; assert_sizeof(ClientTriggerHull, 0x20);

	struct ClientMapTriggers
	{
		unsigned int count;
		ClientTriggerModel* models;
		unsigned int hullCount;
		ClientTriggerHull* hulls;
		unsigned int slabCount;
		TriggerSlab* slabs;
	}; assert_sizeof(ClientMapTriggers, 0x30);

	struct CTAudRvbPanInfo
	{
		bool hasCustomPosition;
		vec3_t position;
		float maxWallDistance;
		float minReverbVolume;
	}; assert_sizeof(CTAudRvbPanInfo, 0x18);

	struct ClientTriggers
	{
		ClientMapTriggers trigger;
		unsigned int triggerStringLength;
		char* triggerString;
		short* visionSetTriggers;
		unsigned short* triggerType;
		vec3_t* origins;
		float* scriptDelay;
		float* priority;
		short* audioTriggers;
		short* blendLookup;
		short* npcTriggers;
		short* audioStateIds;
		SndBankTransient** detailSoundBank;
		CTAudRvbPanInfo* audioRvbPanInfo;
		short* parent;
		ClientEntityLinkToDef** linkTo;
		SpatialPartition_Tree* spatialTree;
		unsigned int* unknown0;
	}; assert_sizeof(ClientTriggers, 0xB8);

	struct ClientTriggerBlendNode
	{
		vec3_t pointA;
		vec3_t pointB;
		unsigned short triggerA;
		unsigned short triggerB;
	}; assert_sizeof(ClientTriggerBlendNode, 0x1C);

	struct ClientTriggerBlend
	{
		unsigned short numClientTriggerBlendNodes;
		ClientTriggerBlendNode* blendNodes;
	}; assert_sizeof(ClientTriggerBlend, 0x10);

	struct MapEntsUnknown0Entry
	{
		char unknown0[0x28];
		ClientEntityLinkToDef* linkTo;
		char unknown1[0x8];
	}; assert_sizeof(MapEntsUnknown0Entry, 0x38);

	struct MapEntsUnknown0
	{
		char unknown0[0x8];
		unsigned int count;
		MapEntsUnknown0Entry* entries;
	}; assert_sizeof(MapEntsUnknown0, 0x18);

	struct SpawnPointEntityRecord
	{
		unsigned short index;
		scr_string_t name;
		scr_string_t target;
		scr_string_t script_noteworthy;
		vec3_t origin;
		vec3_t angles;
	}; assert_sizeof(SpawnPointEntityRecord, 0x28);

	struct SpawnPointRecordList
	{
		unsigned short spawnsCount;
		SpawnPointEntityRecord* spawns;
	}; assert_sizeof(SpawnPointRecordList, 0x10);

	struct SplinePointEntityRecord
	{
		int splineId;
		int splineNodeId;
		scr_string_t splineNodeLabel;
		scr_string_t targetname;
		scr_string_t target;
		scr_string_t string;
		float speed;
		float splineNodeTension;
		vec3_t origin;
		vec3_t angles;
		float throttle;
		vec2_t corridorDims;
		vec3_t tangent;
		float distToNextNode;
		vec3_t positionCubic[4];
		vec3_t tangentQuadratic[3];
	}; assert_sizeof(SplinePointEntityRecord, 0xA8);

	struct SplinePointRecordList
	{
		unsigned short splinePointCount;
		float splineLength;
		SplinePointEntityRecord* splinePoints;
	}; assert_sizeof(SplinePointRecordList, 0x10);

	struct SplineRecordList
	{
		unsigned short splineCount;
		SplinePointRecordList* splines;
	}; assert_sizeof(SplineRecordList, 0x10);

	struct cmodel_t
	{
		Bounds bounds;
		float radius;
		PhysicsAsset* physicsAsset;
		unsigned short physicsShapeOverrideIdx;
		unsigned short navObstacleIdx;
		unsigned int edgeFirstIndex;
		unsigned int edgeTotalCount;
	}; assert_sizeof(cmodel_t, 0x38);

	struct CM_ClientModel
	{
		vec3_t spawnOrigin;
		vec3_t spawnAngles;
		const XModel* model;
		scr_string_t animationTreeName;
		XAnimParts* animation;
		scr_string_t name;
		bool noPhysics;
		bool noCloth;
	}; assert_sizeof(CM_ClientModel, 0x38);

	enum DynEntityListId : std::uint16_t
	{
		DEFAULT_DYNENTITY_LIST_ID = 0x600,
		DYNENTITY_LIST_ID_INVALID = 0x600,
		DYNENTITY_LIST_ID_UNLOADING = 0x601,
	};

	enum SpatialPartition_PopulationAxis : std::uint32_t
	{
		SPATIAL_PARTITION_POPULATION_LEAF = 0x3,
	};

	struct SpatialPartition_Population_Partition
	{
		float dist;
		unsigned int axis : 2;
		unsigned int rightChild : 29;
	}; assert_sizeof(SpatialPartition_Population_Partition, 0x8);

	struct SpatialPartition_Population_Tree
	{
		SpatialPartition_Population_Partition* partitions;
		ExtentBounds* extents;
		unsigned int partitionCount;
	}; assert_sizeof(SpatialPartition_Population_Tree, 0x18);

	struct SpatialPartition_Population
	{
		SpatialPartition_Population_Tree* tree;
		SpatialPartition_Population_Node* buckets;
		SpatialPartition_Population_Node* (*getNodeFromIndex)(const SpatialPartition_Population*, unsigned int);
		unsigned int (*getIndexFromNode)(const SpatialPartition_Population*, const SpatialPartition_Population_Node*);
		void* userData;
		unsigned int partitionCount;
		unsigned int currentPopulation;
		bool bucketsAreChanging;
	}; assert_sizeof(SpatialPartition_Population, 0x38);

	struct SpatialPartition_Population_TransientMap
	{
		unsigned int transientCount;
		bitarray_dynamic* partitionsForTransient;
	}; assert_sizeof(SpatialPartition_Population_TransientMap, 0x10);

	struct ClientEntAnchor
	{
		scr_string_t name;
		int entNum;
	}; assert_sizeof(ClientEntAnchor, 0x8);

	struct ScriptablePhysicsLimits
	{
		unsigned int clientStandaloneMainRigidBodyCount;
		unsigned int clientStandaloneDetailedRigidBodyCount;
		unsigned int clientStandaloneMotionCount;
		unsigned int serverStandaloneMainRigidBodyCount;
		unsigned int serverStandaloneDetailedRigidBodyCount;
		unsigned int serverStandaloneMotionCount;
	}; assert_sizeof(ScriptablePhysicsLimits, 0x18);

	struct ScriptableInstance
	{
		const ScriptableDef* initialDef;
		vec3_t initialOrigin;
		vec3_t initialAngles;
		scr_string_t classname;
		scr_string_t target;
		scr_string_t targetname;
		scr_string_t script_linkname;
		scr_string_t script_linkto;
		scr_string_t script_noteworthy;
		scr_string_t unknown0;
		char unknown1[2];
		unsigned short unknown2;
		unsigned short lightIndex;
		char unknown3[6];
	}; assert_sizeof(ScriptableInstance, 0x48);

	struct Scriptable_EventSun_Data
	{
		int startTime;
		int transTime;
		vec3_t startColorLinearSrgb;
		float startIntensity;
		float targetIntensity;
		vec3_t startAngles;
		vec3_t targetAngles;
	}; assert_sizeof(Scriptable_EventSun_Data, 0x34);

	struct ScriptableReservedDynent
	{
		unsigned int dynentId;
		short next;
	}; assert_sizeof(ScriptableReservedDynent, 0x8);

	struct ScriptableReservedDynents
	{
		short freeHead;
		unsigned int numReservedDynents;
		ScriptableReservedDynent* reservedDynents;
	}; assert_sizeof(ScriptableReservedDynents, 0x10);

	struct ScriptableMapEntsUnknown10
	{
		float unknown;
		unsigned short primaryLightIndex;
		char unknown2[0xA];
	}; assert_sizeof(ScriptableMapEntsUnknown10, 0x10);

	struct ScriptableMapEntsUnknown28
	{
		unsigned int unknown0Count;
		ScriptableMapEntsUnknown10* unknown0;
		unsigned int unknown1Count;
		std::uint64_t* unknown1;
		char unknown2[0x8];
	}; assert_sizeof(ScriptableMapEntsUnknown28, 0x28);

	struct ScriptableMapEnts
	{
		unsigned int totalInstanceCount;
		unsigned int mapInstanceCount;
		unsigned int reservedInstanceCount;
		unsigned int lootInstanceCount;
		unsigned int serverInstanceCount;
		unsigned int serverFixedPartCount;
		unsigned int serverUsablePartCount;
		char unknown0[0x24];
		ScriptableInstance* mapInstances;
		char unknown1[0x68];
		ScriptableReservedDynents reservedDynents[2];
		SpatialPartition_Population_Tree* spatialPopulationTree;
		unsigned int unknown2Count;
		ScriptableMapEntsUnknown10* unknown2;
		ScriptableMapEntsUnknown28 unknown3;
		char unknown4[0x8];
	}; assert_sizeof(ScriptableMapEnts, 0x118);
	assert_offsetof(ScriptableMapEnts, mapInstances, 0x40);
	assert_offsetof(ScriptableMapEnts, reservedDynents, 0xB0);
	assert_offsetof(ScriptableMapEnts, unknown3, 0xE8);

	struct SpawnGroupPoint
	{
		vec3_t origin;
		vec3_t angles;
		unsigned short cluster;
		unsigned short zone;
	}; assert_sizeof(SpawnGroupPoint, 0x1C);

	struct SpawnGroupCluster
	{
		unsigned short zone;
		unsigned short pointCount;
		unsigned short* points;
		Bounds bounds;
	}; assert_sizeof(SpawnGroupCluster, 0x28);

	struct SpawnGroupZone
	{
		scr_string_t name;
		unsigned short pointCount;
		unsigned short* points;
		unsigned short clusterCount;
		unsigned short* clusters;
		Bounds bounds;
	}; assert_sizeof(SpawnGroupZone, 0x38);

	struct SpawnGroup
	{
		unsigned short pointCount;
		SpawnGroupPoint* points;
		unsigned short clusterCount;
		SpawnGroupCluster* clusters;
		unsigned short zoneCount;
		SpawnGroupZone* zones;
	}; assert_sizeof(SpawnGroup, 0x30);

	struct ClientOneshotEffectDef
	{
		vec3_t origin;
		vec3_t angles;
		float radius;
		int delayMsec;
		FxCombinedDef effect;
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
	}; assert_sizeof(ClientOneshotEffectDef, 0x38);

	struct ClientExploderDef
	{
		vec3_t origin;
		vec3_t angles;
		int delayMsec;
		FxCombinedDef effect;
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
		scr_string_t name;
	}; assert_sizeof(ClientExploderDef, 0x40);

	struct ClientIntervalSoundDef
	{
		vec3_t origin;
		vec3_t angles;
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
		int delayMinMsec;
		int delayMaxMsec;
	}; assert_sizeof(ClientIntervalSoundDef, 0x30);

	struct ClientLoopSoundDef
	{
		vec3_t origin;
		vec3_t angles;
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
	}; assert_sizeof(ClientLoopSoundDef, 0x28);

	struct ClientReactiveEntDef
	{
		vec3_t origin;
		vec3_t angles;
		FxCombinedDef effect;
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
		float radius;
	}; assert_sizeof(ClientReactiveEntDef, 0x38);

	struct ClientSideEffectsUnknown38
	{
		char unknown0[0x18];
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
		char unknown1[0x10];
	}; assert_sizeof(ClientSideEffectsUnknown38, 0x38);

	struct ClientSideEffectsUnknown1C0
	{
		char unknown0[0x18];
		SndAliasLookup effectSound;
		SndAliasList* aliasList;
		char unknown1[0x198];
	}; assert_sizeof(ClientSideEffectsUnknown1C0, 0x1C0);

	struct ClientSideEffectsUnknown20
	{
		scr_string_t name;
		char unknown[0x1C];
	}; assert_sizeof(ClientSideEffectsUnknown20, 0x20);

	struct ClientSideEffects
	{
		unsigned int oneshotEffectCount;
		ClientOneshotEffectDef* oneshotEffects;
		SpatialPartition_Tree* oneshotEffectsTree;
		unsigned int oneshotSoundCount;
		unsigned int* oneshotSoundIndices;
		unsigned int exploderCount;
		ClientExploderDef* exploders;
		unsigned int intervalSoundCount;
		ClientIntervalSoundDef* intervalSounds;
		unsigned int loopSoundCount;
		ClientLoopSoundDef* loopSounds;
		unsigned int unknown0Count;
		ClientSideEffectsUnknown38* unknown0;
		unsigned int unknown1Count;
		ClientSideEffectsUnknown1C0* unknown1;
		SpatialPartition_Tree* soundsSpatialTree;
		unsigned int reactiveEntCount;
		ClientReactiveEntDef* reactiveEnts;
		SpatialPartition_Tree* reactiveSpatialTree;
		unsigned int unknown2Count;
		ClientSideEffectsUnknown20* unknown2;
		unsigned int unknown3Count;
		unsigned short* unknown3;
	}; assert_sizeof(ClientSideEffects, 0xB8);
	assert_offsetof(ClientSideEffects, soundsSpatialTree, 0x78);
	assert_offsetof(ClientSideEffects, reactiveSpatialTree, 0x90);

	struct CreateFxPaletteDef
	{
		scr_string_t aliasName;
		scr_string_t effectPath;
		unsigned short refCount;
	}; assert_sizeof(CreateFxPaletteDef, 0xC);

	struct CreateFxAssetData
	{
		unsigned short fxPaletteCount;
		CreateFxPaletteDef* fxPalette;
	}; assert_sizeof(CreateFxAssetData, 0x10);

	struct ServerExploderDef
	{
		vec3_t origin;
		int delayMsec;
		float damageAmount;
		float damageRadius;
		scr_string_t name;
		scr_string_t earthquakeName;
		scr_string_t rumbleName;
		bool damageEnvironmentOnly;
		bool damageDoOcclusionTraces;
	}; assert_sizeof(ServerExploderDef, 0x28);

	struct ServerSideEffects
	{
		unsigned short exploderCount;
		ServerExploderDef* exploders;
	}; assert_sizeof(ServerSideEffects, 0x10);

	struct MayhemInstance
	{
		GfxPlacement initialPose;
		MayhemData* mayhem;
		ClientEntityLinkToDef* linkTo;
		scr_string_t scriptName;
		float curTime;
		float oldTime;
		unsigned int initialFlags;
		unsigned int flags;
		unsigned int mapEntLookup;
		unsigned short transientIndexStored;
	}; assert_sizeof(MayhemInstance, 0x50);

	struct SpawnerField
	{
		scr_string_t key;
		unsigned int keyCanonical;
		scr_string_t value;
		unsigned char type;
	}; assert_sizeof(SpawnerField, 0x10);

	struct Spawner
	{
		vec3_t origin;
		vec3_t angles;
		SpawnerField* fields;
		int flags;
		int count;
		unsigned int numFields;
		scr_string_t targetname;
		scr_string_t classname;
	}; assert_sizeof(Spawner, 0x38);

	struct SpawnerList
	{
		unsigned int spawnerCount;
		Spawner* spawnerList;
	}; assert_sizeof(SpawnerList, 0x10);

	struct AudioPASpeaker
	{
		char unknown0[0x10];
		std::uint64_t unknown1[2];
	}; assert_sizeof(AudioPASpeaker, 0x20);

	struct AudioPropagationNode
	{
		vec3_t position;
		unsigned short numEdges;
		unsigned int edgeStartIndex;
	}; assert_sizeof(AudioPropagationNode, 0x14);

	struct AudioPropagationEdge
	{
		short neighbor;
		short portalIndex;
	}; assert_sizeof(AudioPropagationEdge, 0x4);

	struct CollMapLookupEntry
	{
		const char* targetName;
		int brushIndex;
	}; assert_sizeof(CollMapLookupEntry, 0x10);

	struct WaterDynamicsData0C
	{
		char unknown[0xC];
	}; assert_sizeof(WaterDynamicsData0C, 0xC);

	struct WaterDynamicsData10
	{
		char unknown[0x10];
	}; assert_sizeof(WaterDynamicsData10, 0x10);

	struct WaterDynamicsData18
	{
		char unknown[0x18];
	}; assert_sizeof(WaterDynamicsData18, 0x18);

	struct WaterDynamicsData1C
	{
		char unknown[0x1C];
	}; assert_sizeof(WaterDynamicsData1C, 0x1C);

	struct WaterDynamicsData24
	{
		char unknown[0x24];
	}; assert_sizeof(WaterDynamicsData24, 0x24);

	struct WaterDynamicsData28
	{
		char unknown[0x28];
	}; assert_sizeof(WaterDynamicsData28, 0x28);

	struct WaterDynamicsData44
	{
		char unknown[0x44];
	}; assert_sizeof(WaterDynamicsData44, 0x44);

	struct WaterDynamicsData134
	{
		char unknown[0x134];
	}; assert_sizeof(WaterDynamicsData134, 0x134);

	struct WaterDynamicsList
	{
		unsigned int count;
		WaterDynamicsData24* entries;
	}; assert_sizeof(WaterDynamicsList, 0x10);

	struct WaterDynamicsUnknown38
	{
		char unknown0[0xC];
		unsigned int unknown1Size;
		char* unknown1;
		unsigned int unknown2Size;
		char* unknown2;
		unsigned int unknown3Count;
		unsigned int* unknown3;
	}; assert_sizeof(WaterDynamicsUnknown38, 0x38);

	struct WaterDynamicsUnknown50
	{
		char unknown0[0x30];
		unsigned int unknown1Size;
		char* unknown1;
		unsigned int unknown2Size;
		char* unknown2;
	}; assert_sizeof(WaterDynamicsUnknown50, 0x50);

	struct WaterDynamics
	{
		unsigned int unknown0;
		unsigned int unknown1Count;
		unsigned int* unknown1;
		unsigned int unknown2Count;
		unsigned int* unknown2;
		unsigned int unknown3Count;
		unsigned int* unknown3;
		WaterDynamicsList* unknown4;
		unsigned int unknown5;
		unsigned int unknown6Count;
		WaterDynamicsData18* unknown6;
		char unknown7[0x4C];
		unsigned int unknown8Count;
		unsigned int unknown9Size;
		char* unknown9;
		WaterDynamicsUnknown38* unknown8;
		char unknown10[0x48];
		unsigned int unknown11Count;
		unsigned int unknown12Size;
		char* unknown12;
		WaterDynamicsUnknown50* unknown11;
		char unknown13[0x8];
	}; assert_sizeof(WaterDynamics, 0x118);
	assert_offsetof(WaterDynamics, unknown6, 0x40);
	assert_offsetof(WaterDynamics, unknown8Count, 0x94);
	assert_offsetof(WaterDynamics, unknown8, 0xA8);
	assert_offsetof(WaterDynamics, unknown11Count, 0xF8);
	assert_offsetof(WaterDynamics, unknown11, 0x108);

	struct WaterDynamicsUnknownEntry
	{
		char unknown0[0x28];
		unsigned int count;
		WaterDynamicsData0C* entries;
	}; assert_sizeof(WaterDynamicsUnknownEntry, 0x38);

	struct WaterParticleSystem
	{
		char unknown0[0x14];
		unsigned int count;
		WaterDynamicsData24* entries;
	}; assert_sizeof(WaterParticleSystem, 0x20);

	struct WaterDynamicsBuffer
	{
		char unknown[0x40];
	}; assert_sizeof(WaterDynamicsBuffer, 0x40);

	struct MapEntsUnknown28
	{
		unsigned int unknown0Count;
		unsigned int unknown1Count;
		unsigned int unknown2Count;
		WaterDynamicsData10* unknown0;
		WaterDynamicsData10* unknown1;
		WaterDynamicsData44* unknown2;
	}; assert_sizeof(MapEntsUnknown28, 0x28);

	struct MapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars;
		MapTriggers trigger;
		ClientTriggers clientTrigger;
		ClientTriggerBlend clientTriggerBlend;
		MapEntsUnknown0 unknown0;
		SpawnPointRecordList spawnList;
		SplineRecordList splineList;
		unsigned int havokEntsShapeDataSize;
		char* havokEntsShapeData;
		unsigned int numSubModels;
		cmodel_t* cmodels;
		unsigned int edgeListUsedQueryTypes;
		unsigned int numEdgeLists;
		MapEdgeList** edgeLists;
		SpatialPartition_Tree* edgeListSpatialTree;
		unsigned int numClientModels;
		CM_ClientModel* clientModels;
		unsigned int dynEntCount[2];
		unsigned int dynEntCountTotal;
		unsigned short dynEntityListsCount;
		DynEntityListId* dynEntListIds;
		unsigned short dynEntPhysicsSetupHead[2][2];
		unsigned short dynEntPhysicsSetupTail[2][2];
		unsigned short dynEntNoSpatialCount[2];
		unsigned short dynEntMaxClientHistoryCount;
		char dynEntMaxPosePartCount;
		unsigned int dynEntsWithExtraPosePartsCount;
		SpatialPartition_Population* dynEntSpatialPopulation[2];
		SpatialPartition_Population_TransientMap* dynEntSpatialTransientMap[2];
		SpatialPartition_Population_Node dynEntNoSpatialList;
		unsigned int clientEntAnchorCount;
		ClientEntAnchor* clientEntAnchors;
		ScriptableMapEnts scriptableMapEnts;
		SpawnGroup spawnGroupLoot;
		ClientSideEffects clientSideEffects;
		CreateFxAssetData createFxAssetData;
		unsigned int exploderNameTotal;
		scr_string_t* exploderNames;
		ServerSideEffects serverSideEffects;
		unsigned int createFxEffectTotal;
		unsigned int numMayhemScenes;
		MayhemInstance* mayhemScenes;
		SpawnerList spawners;
		unsigned int audioPASpeakerCount;
		AudioPASpeaker* audioPASpeakers;
		unsigned int numAudioPropNodes;
		AudioPropagationNode* audioPropNodes;
		unsigned int numAudioPropEdges;
		AudioPropagationEdge* audioPropEdges;
		unsigned int numCollmaps;
		CollMapLookupEntry* collmapLookups;
		unsigned int waterDynamicsCount;
		WaterDynamics* waterDynamics;
		unsigned int unknown1Count;
		WaterDynamicsUnknownEntry* unknown1;
		unsigned int unknown2Count;
		std::uint64_t* unknown2;
		unsigned int waterParticleSystemCount;
		WaterParticleSystem* waterParticleSystems;
		unsigned int waterParticleInstanceCount;
		unsigned int unknown3[2];
		unsigned int waterTileCount;
		WaterDynamicsData134* waterDynamicsGfx;
		std::uint64_t* unknown4;
		WaterDynamicsData24* waterTileGfx;
		WaterDynamicsData28* waterTiles;
		WaterDynamicsData18* waterParticleInstancesGfx;
		WaterDynamicsData1C* waterParticleSystemsGfx;
		WaterDynamicsBuffer waterBuffers[4];
		MapEntsUnknown28* unknown5;
	}; assert_sizeof(MapEnts, 0x630);
	assert_offsetof(MapEnts, clientTrigger, 0x68);
	assert_offsetof(MapEnts, clientTriggerBlend, 0x120);
	assert_offsetof(MapEnts, spawnList, 0x148);
	assert_offsetof(MapEnts, havokEntsShapeData, 0x170);
	assert_offsetof(MapEnts, edgeLists, 0x190);
	assert_offsetof(MapEnts, dynEntListIds, 0x1C0);
	assert_offsetof(MapEnts, dynEntSpatialPopulation, 0x1E8);
	assert_offsetof(MapEnts, clientEntAnchors, 0x210);
	assert_offsetof(MapEnts, scriptableMapEnts, 0x218);
	assert_offsetof(MapEnts, clientSideEffects, 0x360);
	assert_offsetof(MapEnts, createFxAssetData, 0x418);
	assert_offsetof(MapEnts, exploderNames, 0x430);
	assert_offsetof(MapEnts, mayhemScenes, 0x450);
	assert_offsetof(MapEnts, audioPASpeakers, 0x470);
	assert_offsetof(MapEnts, collmapLookups, 0x4A0);
	assert_offsetof(MapEnts, waterDynamics, 0x4B0);
	assert_offsetof(MapEnts, waterParticleSystems, 0x4E0);
	assert_offsetof(MapEnts, waterTileCount, 0x4F4);
	assert_offsetof(MapEnts, waterDynamicsGfx, 0x4F8);
	assert_offsetof(MapEnts, waterBuffers, 0x528);
	assert_offsetof(MapEnts, unknown5, 0x628);

	struct Stage
	{
		vec3_t origin;
		unsigned short triggerIndex;
		unsigned char sunPrimaryLightIndex;
		vec3_t skyRotationAngles;
	}; assert_sizeof(Stage, 0x1C);

	struct PhysicsCapacities
	{
		int maxNumRigidBodiesServer;
		int maxNumDetailRigidBodiesServer;
		int maxNumConstraintsServer;
		int maxNumMotionsServer;
		int maxNumRigidBodiesClient;
		int maxNumDetailRigidBodiesClient;
		int maxNumConstraintsClient;
		int maxNumMotionsClient;
	}; assert_sizeof(PhysicsCapacities, 0x20);

	struct CollisionHeatmapEntry
	{
		int vertexCount;
		float vertexDensity;
		vec3_t minExtent;
		vec3_t maxExtent;
	}; assert_sizeof(CollisionHeatmapEntry, 0x20);

	struct TopDownMapDataHeader
	{
		int version;
		int samplesPerAxis;
		vec2_t bottomLeft;
		vec2_t topRight;
	}; assert_sizeof(TopDownMapDataHeader, 0x18);

	struct TopDownMapDataContent
	{
		unsigned char navMeshVisible[0x2000];
	};

	struct TopDownMapData
	{
		TopDownMapDataHeader header;
		TopDownMapDataContent content;
	}; assert_sizeof(TopDownMapData, 0x2018);

	enum PhysicsContents : std::uint32_t
	{
		PHYSICS_CONTENTS_SOLID = 0x1,
		PHYSICS_CONTENTS_FOLIAGE = 0x2,
		PHYSICS_CONTENTS_VEHICLETRIGGER = 0x8,
		PHYSICS_CONTENTS_GLASS = 0x10,
		PHYSICS_CONTENTS_WATER = 0x20,
		PHYSICS_CONTENTS_AINOSHOOT = 0x40,
		PHYSICS_CONTENTS_MISSILECLIP = 0x80,
		PHYSICS_CONTENTS_ITEM = 0x100,
		PHYSICS_CONTENTS_VEHICLECLIP = 0x200,
		PHYSICS_CONTENTS_ITEMCLIP = 0x400,
		PHYSICS_CONTENTS_SKY = 0x800,
		PHYSICS_CONTENTS_AINOSIGHT = 0x1000,
		PHYSICS_CONTENTS_CLIPSHOT = 0x2000,
		PHYSICS_CONTENTS_ACTOR = 0x4000,
		PHYSICS_CONTENTS_PLAYERNOSIGHT = 0x8000,
		PHYSICS_CONTENTS_PLAYERCLIP = 0x10000,
		PHYSICS_CONTENTS_AICLIP = 0x20000,
		PHYSICS_CONTENTS_SENTIENTTRIGGER = 0x40000,
		PHYSICS_CONTENTS_EXPLOSIONCLIP = 0x100000,
		PHYSICS_CONTENTS_SOUNDCLIP = 0x200000,
		PHYSICS_CONTENTS_NONSENTIENTTRIGGER = 0x400000,
		PHYSICS_CONTENTS_VEHICLE = 0x800000,
		PHYSICS_CONTENTS_MANTLE = 0x1000000,
		PHYSICS_CONTENTS_PLAYER = 0x2000000,
		PHYSICS_CONTENTS_SOUNDREFLECT = 0x4000000,
		PHYSICS_CONTENTS_USECLIP = 0x20000000,
		PHYSICS_CONTENTS_PLAYERTRIGGER = 0x40000000,
		PHYSICS_CONTENTS_SOLID_MASK = 0x2833B6C0,
	};

	enum SurfaceType
	{
		SURFACE_TYPE_NONE = 0,
		SURFACE_TYPE_BARK = 1,
		SURFACE_TYPE_BRICK = 2,
		SURFACE_TYPE_CARPET_SOLID = 3,
		SURFACE_TYPE_CLOTH = 4,
		SURFACE_TYPE_CONCRETE_DRY = 5,
		SURFACE_TYPE_DIRT = 6,
		SURFACE_TYPE_FLESH = 7,
		SURFACE_TYPE_FOLIAGE_DRY = 8,
		SURFACE_TYPE_GLASS_PANE = 9,
		SURFACE_TYPE_GRASS_SHORT = 10,
		SURFACE_TYPE_GRAVEL = 11,
		SURFACE_TYPE_ICE_SOLID = 12,
		SURFACE_TYPE_METAL_THICK = 13,
		SURFACE_TYPE_METAL_GRATE = 14,
		SURFACE_TYPE_MUD = 15,
		SURFACE_TYPE_PAPER = 16,
		SURFACE_TYPE_PLASTER = 17,
		SURFACE_TYPE_ROCK = 18,
		SURFACE_TYPE_SAND = 19,
		SURFACE_TYPE_SNOW = 20,
		SURFACE_TYPE_WATER = 21,
		SURFACE_TYPE_WOOD_SOLID = 22,
		SURFACE_TYPE_ASPHALT_DRY = 23,
		SURFACE_TYPE_CERAMIC = 24,
		SURFACE_TYPE_PLASTIC = 25,
		SURFACE_TYPE_RUBBER = 26,
		SURFACE_TYPE_FRUIT = 27,
		SURFACE_TYPE_METAL_CAR = 28,
		SURFACE_TYPE_RIOTSHIELD = 29,
		SURFACE_TYPE_SLUSH = 30,
		SURFACE_TYPE_ASPHALT_WET = 31,
		SURFACE_TYPE_CARPET_WOOD = 32,
		SURFACE_TYPE_CARPET_METAL = 33,
		SURFACE_TYPE_CONCRETE_WET = 34,
		SURFACE_TYPE_CUSHION = 35,
		SURFACE_TYPE_DEFAULT = 36,
		SURFACE_TYPE_FOLIAGE_WET = 37,
		SURFACE_TYPE_GLASS_BROKEN = 38,
		SURFACE_TYPE_GLASS_SOLID = 39,
		SURFACE_TYPE_GLASS_VEHICLE = 40,
		SURFACE_TYPE_GRASS_TALL = 41,
		SURFACE_TYPE_ICE_THIN = 42,
		SURFACE_TYPE_METAL_THIN = 43,
		SURFACE_TYPE_METAL_HELICOPTER = 44,
		SURFACE_TYPE_TILE = 45,
		SURFACE_TYPE_METAL_TANK = 46,
		SURFACE_TYPE_BODY_ARMOR = 47,
		SURFACE_TYPE_DRYWALL = 48,
		SURFACE_TYPE_MUD_RIVERBED = 49,
		SURFACE_TYPE_WATER_KNEE = 50,
		SURFACE_TYPE_WATER_WAIST = 51,
		SURFACE_TYPE_WOOD_FLOOR = 52,
		SURFACE_TYPE_USER_TERRAIN_1 = 53,
		SURFACE_TYPE_USER_TERRAIN_2 = 54,
		SURFACE_TYPE_USER_TERRAIN_3 = 55,
		SURFACE_TYPE_USER_TERRAIN_4 = 56,
		SURFACE_TYPE_USER_TERRAIN_5 = 57,
		SURFACE_TYPE_USER_TERRAIN_6 = 58,
		SURFACE_TYPE_USER_TERRAIN_7 = 59,
		SURFACE_TYPE_USER_TERRAIN_8 = 60,
		SURFACE_TYPE_USER_TERRAIN_9 = 61,
		SURFACE_TYPE_CINDERBLOCK = 62,
		SURFACE_TYPE_CODE_RESERVED = 63,

		SURFACE_TYPE_COUNT = 64
	};

	struct clipMap_t
	{
		const char* name;
		char unknown0[8];
		MapEnts* mapEnts;
		Stage* stages;
		unsigned char stageCount;
		MapTriggers stageTrigger;
		vec3_t broadphaseMin;
		vec3_t broadphaseMax;
		PhysicsCapacities physicsCapacities;
		unsigned int havokWorldShapeDataSize;
		char* havokWorldShapeData;
		char unknown1[0x2104];
		unsigned int numCollisionHeatmapEntries;
		CollisionHeatmapEntry* collisionHeatmap;
		unsigned int totalGlassInitPieceCount;
		unsigned int totalGlassPieceLimit;
		TopDownMapData* topDownMapData;
		const char* visionSetName;
		unsigned int checksum;
	}; assert_sizeof(clipMap_t, 0x21F0);
	assert_offsetof(clipMap_t, stageTrigger, 0x28);
	assert_offsetof(clipMap_t, havokWorldShapeData, 0xB8);
	assert_offsetof(clipMap_t, collisionHeatmap, 0x21C8);
	assert_offsetof(clipMap_t, topDownMapData, 0x21D8);
	assert_offsetof(clipMap_t, visionSetName, 0x21E0);

	struct LocalizeEntry
	{
		const char* name;
		const char* value;
	}; assert_sizeof(LocalizeEntry, 0x10);

	struct TracerDef
	{
		const char* name;
		FxCombinedDef effects[6];
		char __pad0[0x10];
		Material* material;
		char __pad1[0x60];
	}; assert_sizeof(TracerDef, 0xB0);

	struct ClientCharacter
	{
		const char* name;
		ScriptableDef* scriptableDef;
		const char* animFileName;
		const char* animTreeName;
	}; assert_sizeof(ClientCharacter, 0x20);

	struct CoverSelector
	{
		const char* name;
		scr_string_t scrName;
		float distanceWeight;
		bool useDistanceSquared;
		float distanceMin;
		float distanceOptimal;
		float distanceMax;
		float engagementWeight;
		float dangerousWeight;
		float targetDirWeight;
		float moveTowardsTargetWeight;
		float visibleWeight;
		float notVisibleWeight;
		bool considerCoverNodes;
		bool considerTacPoints;
		float strictCoverNodeWeight;
		float concealNodeWeight;
		float exposedNodeWeight;
		float ambushNodeWeight;
		float nodeAngleWeight;
		float secondaryTargetWeight;
		float playerSightWeight;
		float priorityWeight;
		float lmgMountableWeight;
		float indoorWeight;
		float outdoorWeight;
		float heightWeight;
		float targetHeightWeight;
		float stickinessWeight;
	}; assert_sizeof(CoverSelector, 0x78);

	struct ReticleDef
	{
		const char* name;
		const char* widgetName;
		bool lockReticleToTag;
		bool primaryAffectedByEMP;
		bool secondaryAffectedByEMP;
		float offsetFromTag;
		bool scaleWithFOV;
		float primaryDefaultScale;
		float secondaryDefaultScale;
		const char* secondaryWidgetName;
		bool secondaryLockReticleToTag;
		float secondaryOffsetFromTag;
		float naturalDistance;
		int naturalFOV;
		float minReticleScale;
		float maxReticleScale;
	}; assert_sizeof(ReticleDef, 0x48);

	struct hknpConvexHullFace
	{
		unsigned short firstIndex;
		unsigned char numIndices;
		unsigned char minHalfAngle;
	}; assert_sizeof(hknpConvexHullFace, 0x4);

	struct hknpConvexHullEdge
	{
		unsigned short faceIndex;
		unsigned char edgeIndex;
		unsigned char padding;
	}; assert_sizeof(hknpConvexHullEdge, 0x4);

	struct hkcdSimdTreeNode
	{
		vec4_t lx;
		vec4_t hx;
		vec4_t ly;
		vec4_t hy;
		vec4_t lz;
		vec4_t hz;
		unsigned int data[4];
		bool isLeaf;
		bool isActive;
		char pad[14];
	}; assert_sizeof(hkcdSimdTreeNode, 0x80);

	struct hknpShapeInstance
	{
		vec4_t rotation;
		vec3_t translation;
		unsigned short leafIndex;
		unsigned char flags;
		unsigned char padding1;
		vec3_t scale;
		unsigned short shapeTag;
		unsigned short destructionTag;
		std::int64_t shape;
		unsigned char padding2[8];
	}; assert_sizeof(hknpShapeInstance, 0x40);

	struct PhysicsLibrary
	{
		const char* name;
		bool isMaterialList;
		bool isBodyQualityList;
		bool isMotionPropertiesList;
		bool isGlobalTypeCompendium;
		unsigned int havokDataSize;
		char* havokData;
	}; assert_sizeof(PhysicsLibrary, 0x18);

	struct Camo
	{
		const char* name;
		scr_string_t internalName;
		MaterialTextureDef* textureDefs;
		unsigned int textureDefCount;
		char unknown0[0x3C];
		unsigned int unknownValues[3];
		char unknown1[0xBC];
		FxCombinedDef unknownFx0;
		char unknown2[0x4];
		scr_string_t unknownStrings[4];
		char unknown3[0x4];
		FxCombinedDef unknownFx1;
	}; assert_sizeof(Camo, 0x148);
	assert_offsetof(Camo, unknownValues, 0x58);
	assert_offsetof(Camo, unknownFx0, 0x120);
	assert_offsetof(Camo, unknownStrings, 0x12C);
	assert_offsetof(Camo, unknownFx1, 0x140);

	struct StaticModelCollisionCompressedInstance
	{
		unsigned short origin[3];
		unsigned short angles[3];
		unsigned short scale;
	}; assert_sizeof(StaticModelCollisionCompressedInstance, 0xE);

	struct StaticModelCollisionCompressedModel
	{
		scr_string_t name;
		PhysicsAsset* physicsAsset;
		XModelDetailCollision* detailCollision;
		void* shapes[2];
		unsigned short shapeTags[2];
		unsigned int numInstances;
		StaticModelCollisionCompressedInstance* instances;
	}; assert_sizeof(StaticModelCollisionCompressedModel, 0x38);

	struct StaticModelCollisionCompressedModelList
	{
		unsigned int numModels;
		StaticModelCollisionCompressedModel* models;
		unsigned int structureMemoryCost;
		unsigned int numInstances;
		unsigned int assetCost;
	}; assert_sizeof(StaticModelCollisionCompressedModelList, 0x20);

	struct CollisionTile
	{
		const char* name;
		unsigned int index;
		unsigned int unknown0;
		char gridId[2];
		vec3_t origin;
		char unknown2[8];
		const char* unknown1;
		StaticModelCollisionCompressedModelList* staticModelCollision;
		unsigned int havokTileShapeDataSize;
		char* havokTileShapeData;
		char* havokTileShapePointer;
		unsigned int worldCollisionShapeIdx;
	}; assert_sizeof(CollisionTile, 0x58);
	assert_offsetof(CollisionTile, staticModelCollision, 0x30);
	assert_offsetof(CollisionTile, havokTileShapePointer, 0x48);


	struct BehaviorTreeActionData
	{
		unsigned short initFn;
		unsigned short processFn;
		unsigned short terminateFn;
		unsigned short processArgsFn;
		bool bProcessBuiltin;
		bool bInitBuiltin;
		bool bTerminateBuiltin;
	};

	struct BehaviorTreeNodeChildren
	{
		unsigned short firstChildOffset;
		unsigned short childCount;
	};

	union BehaviorTreeNodeDataUnion
	{
		BehaviorTreeActionData actionData;
		BehaviorTreeNodeChildren children;
	};

	struct BehaviorTreeDecoratorChildren
	{
		unsigned short firstDecorator;
		unsigned short decoratorCount;
	};

	struct BehaviorTreeNode
	{
		const char* name;
		unsigned char type;
		unsigned char resultOverride;
		int schedule;
		BehaviorTreeNodeDataUnion data;
		BehaviorTreeDecoratorChildren decorators;
	}; assert_sizeof(BehaviorTreeNode, 0x20);

	struct BehaviorTreeDecoratorRootInit
	{
		unsigned short initFn;
		bool bInitBuiltin;
	};

	struct BehaviorTreeDecoratorTerminate
	{
		unsigned short terminateFn;
		bool bTerminateBuiltin;
	};

	struct BehaviorTreeDecoratorInit
	{
		unsigned short initFn;
		bool bInitBuiltin;
	};

	union BehaviorTreeDecoratorUnion
	{
		BehaviorTreeDecoratorRootInit rootInit;
		BehaviorTreeDecoratorTerminate terminate;
		BehaviorTreeDecoratorInit init;
	};

	struct BehaviorTreeDecorator
	{
		unsigned char type;
		BehaviorTreeDecoratorUnion data;
	}; assert_sizeof(BehaviorTreeDecorator, 0x6);

	struct BehaviorTree
	{
		const char* name;
		unsigned short nodeCount;
		BehaviorTreeNode* nodes;
		unsigned short numFuncIDs;
		int* funcIDs;
		unsigned short decoratorCount;
		BehaviorTreeDecorator* decorators;
	}; assert_sizeof(BehaviorTree, 0x38);

	struct XHash
	{
		const char* name;
		std::uint64_t hash;
	}; assert_sizeof(XHash, 0x10);

	struct StreamTreeOverrideImageNode
	{
		XHash srcName;
		const GfxImage* desImage;
	}; assert_sizeof(StreamTreeOverrideImageNode, 0x18);

	struct StreamTreeOverride
	{
		const char* name;
		StreamTreeOverrideImageNode* imageNodes;
		unsigned int imageNodeCount;
	}; assert_sizeof(StreamTreeOverride, 0x18);


	struct EquipmentClothData
	{
		char* foleyName;
		char* footstepName;
	};

	struct EquipmentWeaponRattleData
	{
		char* szName;
		float priority;
	};

	struct EquipmentSndChance
	{
		float rattleChance;
		float accentChance;
		float silentChance;
	}; assert_sizeof(EquipmentSndChance, 0xC);

	struct EquipmentChanceMoveTypes
	{
		EquipmentSndChance* chances;
	};

	struct EquipmentChanceRattleTypes
	{
		EquipmentChanceMoveTypes* chances;
	};

	struct EquipmentSoundSet
	{
		SndAliasList* soundPLR;
		SndAliasList* soundNPC;
	}; assert_sizeof(EquipmentSoundSet, 0x10);

	struct EquipSoundSetFootstepActionType
	{
		EquipmentSoundSet left[11];
		EquipmentSoundSet right[11];
	};

	struct EquipSoundSetFootsteps
	{
		EquipSoundSetFootstepActionType actionType[4];
	}; assert_sizeof(EquipSoundSetFootsteps, 0x580);

	struct EquipSoundSetMoveLeadTypes
	{
		EquipmentSoundSet* soundSetsPre;
		EquipmentSoundSet* soundSetsPost;
	};

	struct EquipSoundSetMoveTypes
	{
		EquipmentSoundSet* soundSets;
	};

	struct EquipSoundSetMantleTypes
	{
		EquipmentSoundSet soundSets[35];
	}; assert_sizeof(EquipSoundSetMantleTypes, 0x230);

	struct EquipSoundSetStanceTypes
	{
		EquipmentSoundSet soundSets[4];
	}; assert_sizeof(EquipSoundSetStanceTypes, 0x40);

	struct EquipSoundSetMeleeVM
	{
		EquipmentSoundSet soundSets[0x6C0];
	}; assert_sizeof(EquipSoundSetMeleeVM, 0x6C00);

	struct EquipSoundSetMeleeWM
	{
		EquipmentSoundSet soundSets[0x630];
	}; assert_sizeof(EquipSoundSetMeleeWM, 0x6300);

	struct EquipSoundSetMeleeWorld
	{
		EquipmentSoundSet soundSets[0x210];
	};

	struct EquipmentSoundTable
	{
		const char* szName;
		unsigned int numClothTypes;
		unsigned int numWeaponRattleTypes;
		unsigned int numMoveTypes;
		unsigned int numStanceTypes;
		EquipmentClothData* clothTypes;
		EquipmentWeaponRattleData* weaponRattleTypes;
		EquipmentChanceRattleTypes* chancesPLR;
		EquipmentChanceRattleTypes* chancesNPC;
		EquipSoundSetFootsteps* mvmtClothFootstepSoundSets;
		EquipSoundSetFootsteps* mvmtClothFootstepCeilingSoundSets;
		EquipSoundSetMoveLeadTypes* mvmtClothFoleySoundSets;
		EquipSoundSetMoveTypes* mvmtRattleSoundSets;
		EquipSoundSetMoveTypes mvmtAccentSoundSets;
		EquipSoundSetMantleTypes* mvmtMantleSoundSets;
		EquipSoundSetStanceTypes* mvmtStanceSoundSets;
		EquipSoundSetMeleeVM* meleeAttackVMSoundSets;
		EquipSoundSetMeleeWM* meleeAttackWMSoundSets;
		EquipSoundSetMeleeWM* meleeCharacterSoundSets;
		EquipSoundSetMeleeWorld meleeWorldSoundSets;
	}; assert_sizeof(EquipmentSoundTable, 0x2188);

	enum GestureType : std::int32_t
	{
	};

	enum GesturePriority : std::int32_t
	{
	};

	struct Gesture
	{
		const char* name;
		GestureType type;
		GesturePriority priority;
		bool looping;
		XAnimParts** anims;
		char __pad0[0x268];
		scr_string_t ikTargetEntityBoneName;
	}; assert_sizeof(Gesture, 0x290);

	struct ClothAssetBoneMapping
	{
		scr_string_t* scrTagNames;
		unsigned int scrTagCount;
	}; assert_sizeof(ClothAssetBoneMapping, 0x10);

	struct ClothAssetBoneMappingSet
	{
		ClothAssetBoneMapping* boneMappings;
		unsigned int boneMappingCount;
	}; assert_sizeof(ClothAssetBoneMappingSet, 0x10);

	struct ClothAssetSimulationLOD
	{
		float maxDistanceSq;
		float hkMaxDistanceSq;
		const char* stateName;
		unsigned int stateIdx;
	}; assert_sizeof(ClothAssetSimulationLOD, 0x18);

	struct ClothAssetSound
	{
		float minSpeed;
		float maxSpeed;
		float pitchMinSpeed;
		float pitchMaxSpeed;
		float volumeMinSpeed;
		float volumeMaxSpeed;
		const char* soundAlias;
	}; assert_sizeof(ClothAssetSound, 0x20);

	struct ClothAsset
	{
		const char* name;
		bool standAlone;
		unsigned int havokDataSize;
		char* havokData;
		ClothAssetBoneMappingSet* boneMappingSets;
		unsigned int boneMappingSetCount;
		const char* animationStateName;
		unsigned int animationStateIdx;
		ClothAssetSimulationLOD* simulationLods;
		unsigned int simulationLodCount;
		int surfaceTypeIndex;
		bool vectorFieldSupport;
		float vectorFieldDrag;
		bool bulletSupport;
		int bulletProxyUpdateRate;
		float bulletForce;
		float defaultSpeed;
		float speedBlendSpeed;
		float soundIWMaxDistanceSq;
		float soundHKMaxDistanceSq;
		ClothAssetSound* sounds;
		unsigned int soundCount;
	}; assert_sizeof(ClothAsset, 0x80);

	struct AnimsetAnim
	{
		scr_string_t name;
		float weight;
		scr_string_t aimsetName;
		scr_anim_t anim;
	}; assert_sizeof(AnimsetAnim, 0x18);

	union ASM_Function_Param_Value
	{
		bool m_Bool;
		int m_Int;
		float m_Float;
		scr_string_t m_String;
	};

	enum ASM_Function_Param_Type : std::int32_t
	{
		ParamType_Undefined = 0x0,
		ParamType_Bool = 0x1,
		ParamType_String = 0x2,
		ParamType_Int = 0x3,
		ParamType_Float = 0x4,
	};

	struct ASM_Function_Param
	{
		ASM_Function_Param_Value u;
		ASM_Function_Param_Type m_Type;
	}; assert_sizeof(ASM_Function_Param, 0x8);

	struct ASM_Function
	{
		int m_FuncID;
		int m_NumParams;
		ASM_Function_Param* m_Params;
		bool m_bBuiltin;
		bool m_bNegate;
		char m_Flags;
		unsigned short m_Hash;
		char padding[8];
	}; assert_sizeof(ASM_Function, 0x20);

	struct AIAnimsetAlias
	{
		AnimsetAnim* redAnims;
		unsigned int numRedAnims;
	}; assert_sizeof(AIAnimsetAlias, 0x10);

	struct PlayerAnimsetAddonEntry
	{
		scr_string_t stateName;
		scr_string_t aliasName;
		scr_string_t addonType;
		float blendtime;
		float rate;
		bool restart;
		char padding[8];
	}; assert_sizeof(PlayerAnimsetAddonEntry, 0x20);

	struct PlayerAnimsetAliasPatch
	{
		bool isPatchable;
		scr_string_t* strings;
		unsigned int numStrings;
		char padding[8];
	}; assert_sizeof(PlayerAnimsetAliasPatch, 0x20);

	struct PlayerAnimsetAlias
	{
		char unknown0[0x30];
		scr_string_t aimState;
		scr_string_t aimAlias;
		scr_string_t moveTurnState;
		scr_string_t moveTurnAlias;
		char unknown1[0x18];
		PlayerAnimsetAddonEntry* aliasAddons;
		unsigned int numAliasAddons;
		PlayerAnimsetAliasPatch patchInfo;
		char unknown2[0x10];
	}; assert_sizeof(PlayerAnimsetAlias, 0x98);
	assert_offsetof(PlayerAnimsetAlias, aimState, 0x30);
	assert_offsetof(PlayerAnimsetAlias, aliasAddons, 0x58);
	assert_offsetof(PlayerAnimsetAlias, patchInfo, 0x68);

	union AnimsetAlias_Union
	{
		AIAnimsetAlias* m_AIAnimsetAlias;
		PlayerAnimsetAlias* m_PlayerAnimsetAlias;
	};

	struct AnimsetAliasUnknownEntry
	{
		scr_string_t unknownString0;
		char unknown[0x8];
		scr_string_t unknownString1;
	}; assert_sizeof(AnimsetAliasUnknownEntry, 0x10);

	struct AnimsetAlias
	{
		scr_string_t name;
		AnimsetAnim* anims;
		unsigned int numAnims;
		ASM_Function* funcs;
		unsigned int numFuncs;
		scr_string_t* addons;
		unsigned int numAddons;
		scr_string_t animAlias;
		int blenderIndex;
		float blendtime;
		scr_string_t curveName;
		AnimsetAliasUnknownEntry* unknownEntries;
		unsigned int unknownEntryCount;
		AnimsetAlias_Union u;
	}; assert_sizeof(AnimsetAlias, 0x60);
	assert_offsetof(AnimsetAlias, curveName, 0x40);
	assert_offsetof(AnimsetAlias, unknownEntries, 0x48);
	assert_offsetof(AnimsetAlias, u, 0x58);

	struct scr_animtree_t
	{
		XAnim_s* anims;
	};

	struct AnimsetBlendtreeAlias
	{
		scr_string_t name;
		scr_string_t graftNode;
		scr_string_t blendtreeName;
		ASM_Function* funcs;
		unsigned int numFuncs;
		scr_anim_t graftAnim;
		scr_animtree_t blendtree;
		bool bNotify;
	}; assert_sizeof(AnimsetBlendtreeAlias, 0x38);

	struct AnimsetStateEntryMap
	{
		unsigned short aliasIndex;
		unsigned short aliasAnimEntryIndex;
	};

	struct AIAnimsetState
	{
		char unused;
	};

	struct PlayerAnimsetState_BakedAliasesPerConditionType
	{
		bitarray_dynamic* aliases;
		unsigned short numConditionValues;
		unsigned short conditionType;
	}; assert_sizeof(PlayerAnimsetState_BakedAliasesPerConditionType, 0x10);

	struct PlayerAnimsetState
	{
		char unknown[0x55];
		unsigned char numConditionTypes;
		PlayerAnimsetState_BakedAliasesPerConditionType* aliasesPerConditionType;
	}; assert_sizeof(PlayerAnimsetState, 0x60);
	assert_offsetof(PlayerAnimsetState, numConditionTypes, 0x55);

	union AnimsetState_Union
	{
		AIAnimsetState* m_AIAnimsetState;
		PlayerAnimsetState* m_PlayerAnimsetState;
	};

	struct AnimsetState
	{
		scr_string_t name;
		AnimsetAlias* animAliases;
		unsigned int numAnimAliases;
		AnimsetBlendtreeAlias* blendtreeAliases;
		unsigned int numBlendtreeAliases;
		scr_string_t notify;
		scr_string_t aimsetTarget;
		scr_string_t aimsetRoot;
		scr_anim_t aimsetRootAnim;
		unsigned int numAnims;
		AnimsetStateEntryMap* animEntryMap;
		unsigned char stateType;
		float lookAtMinYaw;
		float lookAtMaxYaw;
		float lookAtMinPitch;
		float lookAtMaxPitch;
		float lookAtNeutralPitch;
		float lookAtNeutralYaw;
		bool restart;
		bool isAimset;
		bool clearAlwaysOn;
		scr_string_t unknownString0;
		scr_string_t unknownString1;
		char unknown[0x8];
		AnimsetState_Union u;
	}; assert_sizeof(AnimsetState, 0x80);
	assert_offsetof(AnimsetState, animEntryMap, 0x40);
	assert_offsetof(AnimsetState, unknownString0, 0x68);
	assert_offsetof(AnimsetState, u, 0x78);

	enum AnimationController : std::int32_t
	{
		ANIMCTRL_NONE = 0x0,
		ANIMCTRL_PLAYER = 0x1,
		ANIMCTRL_DOG = 0x2,
		ANIMCTRL_NUM = 0x3,
	};

	struct AnimationEntry
	{
		scr_string_t animName;
		int aimSetIndex;
	}; assert_sizeof(AnimationEntry, 0x8);

	struct AnimAliasInfo
	{
		char animIndex;
		float animWeight;
	}; assert_sizeof(AnimAliasInfo, 0x8);

	struct AnimAlias
	{
		scr_string_t aliasName;
		char animCount;
		AnimAliasInfo* aliasInfo;
	}; assert_sizeof(AnimAlias, 0x10);

	struct AnimationState
	{
		scr_string_t name;
		scr_string_t notify;
		float blendTime;
		float blendOutTime;
		char flags;
		unsigned char entryCount;
		unsigned char aliasCount;
		int aimSetIndex;
		AnimationEntry* animEntries;
		unsigned __int64* animIndices;
		AnimAlias* aliasList;
	}; assert_sizeof(AnimationState, 0x30);

	struct AnimationAimSet
	{
		scr_string_t name;
		scr_string_t rootName;
		int animCount;
		scr_string_t* animName;
		unsigned __int64 rootIndex;
		unsigned __int64* animIndices;
		unsigned __int64* aimNodeIndices;
	}; assert_sizeof(AnimationAimSet, 0x30);

	struct AnimationStateMachine
	{
		scr_string_t name;
		unsigned short stateCount;
		unsigned short aimSetCount;
		AnimationState* states;
		AnimationAimSet* aimSets;
	}; assert_sizeof(AnimationStateMachine, 0x18);

	struct AnimationClass
	{
		const char* className;
		AnimationStateMachine* stateMachine;
		AnimationController animCtrl;
		scr_string_t animTree;
		unsigned short soundCount;
		unsigned short effectCount;
		scr_string_t* soundNotes;
		scr_string_t* soundNames;
		scr_string_t* soundOptions;
		scr_string_t* effectNotes;
		FxCombinedDef* effectDefs;
		scr_string_t* effectTags;
	}; assert_sizeof(AnimationClass, 0x50);

	enum ASM_Mode : std::int32_t
	{
		ASM_MODE_AI = 0x0,
		ASM_MODE_PLAYER = 0x1,
	};

	struct AIAnimsetNodeOffset
	{
		int hideYaw;
		int snapYaw;
		int leanYaw;
		int leanPitch;
	};

	struct AIAnimset
	{
		AIAnimsetNodeOffset coverLeftOffset;
		AIAnimsetNodeOffset coverLeftCrouchOffset;
		AIAnimsetNodeOffset coverRightOffset;
		AIAnimsetNodeOffset coverRightCrouchOffset;
		AIAnimsetNodeOffset coverCrouchOffset;
		AIAnimsetNodeOffset coverStandOffset;
	}; assert_sizeof(AIAnimset, 0x60);

	struct PlayerAnimset
	{
		unsigned short* packMap;
		unsigned short numPackMapEntries;
		unsigned short* unpackMap;
		unsigned short numUnpackMapEntries;
		unsigned short* unknownMap;
		unsigned short numUnknownMapEntries;
		scr_string_t patchName;
		char unknown[0x20];
	}; assert_sizeof(PlayerAnimset, 0x50);
	assert_offsetof(PlayerAnimset, patchName, 0x2C);

	union Animset_Union
	{
		AIAnimset* m_AIAnimset;
		PlayerAnimset* m_PlayerAnimset;
	};

	struct Animset
	{
		const char* name;
		scr_string_t constName;
		scr_string_t animTree;
		AnimsetState* states;
		unsigned int numStates;
		AnimationController animCtrl;
		ASM_Mode mode;
		scr_string_t startStateName;
		scr_string_t startAliasName;
		scr_string_t clearStateName;
		scr_string_t clearAliasName;
		scr_string_t alwaysOnStateName;
		Animset_Union u;
	}; assert_sizeof(Animset, 0x40);

	struct EnemySelector
	{
		const char* name;
		scr_string_t scrName;
		int threatVisible;
		int threatFullyAware;
		int threatFriendlyTimingOutOnEnemy;
		int threatScarinessScale;
		int threatScarinessMin;
		int threatScarinessMax;
		int threatSharedEnemy;
		int threatSharedEnemyCap;
		int threatSharedMeleeEnemy;
		int threatFriendliesCurrentEnemy;
		int threatFriendliesTimeoutEnemy;
		int threatCurrentEnemy;
		int threatCurrentEnemyPlayerDamaged;
		int threatFlashedEnemy;
		int threatNoCover;
		int threatPlayerAttackedMeRecently;
		int threatKilledTeammateRecently;
		int threatBehindEnemyLinesMax;
		int threatBehindEnemyLinesDist;
		int threatCoveringFire;
		int threatCoverFireCurTarget;
		float threatDistanceMaxRange;
		float threatDistanceMaxThreat;
		float threatMinDistToIgnore;
		float threatMinSuppression;
	}; assert_sizeof(EnemySelector, 0x70);

	struct TTFDef
	{
		const char* name;
		int fileLen;
		const char* file;
		void* ftFace;
	}; assert_sizeof(TTFDef, 0x20);

	struct DLogSerializer
	{
		bool requireSchema;
		const char* name;
		void* serialize;
	}; assert_sizeof(DLogSerializer, 0x18);

	struct DLogEndpoint
	{
		bool requireSchema;
		const char* name;
		void* endpoint;
	}; assert_sizeof(DLogEndpoint, 0x18);

	struct DLogChannel
	{
		const char* name;
		DLogSerializer** serializers;
		int serializerCount;
		DLogSerializer** postSerializers;
		int postSerializerCount;
		DLogEndpoint* endpoint;
		int bufferSize;
		int bufferTimeMilliseconds;
		int bufferFlushSize;
		unsigned int envs;
		unsigned int platforms;
		bool requireSchema;
	}; assert_sizeof(DLogChannel, 0x48);

	struct DLogVar
	{
		const char* name;
		const char* value;
	};

	struct DLogHash
	{
		unsigned int hash;
		int index;
	};

	struct DLogHashTable
	{
		DLogHash* list;
		int count;
		int max;
	}; assert_sizeof(DLogHashTable, 0x10);

	struct DLogEnum
	{
		const char* name;
		DLogHashTable values;
	}; assert_sizeof(DLogEnum, 0x18);

	struct DLogColumn
	{
		const char* name;
		int type;
		int stringSize;
		int arrayCount;
		DLogEnum* enumRef;
	}; assert_sizeof(DLogColumn, 0x20);

	struct DLogRow
	{
		const char* name;
		const char* type;
		DLogColumn* columns;
		int columnCount;
		int arrayCount;
		DLogHashTable columnHashTable;
		void* autoFill;
	}; assert_sizeof(DLogRow, 0x38);

	struct DLogChannelRefSample
	{
		char unknown[0x8];
		const char* name;
	}; assert_sizeof(DLogChannelRefSample, 0x10);

	struct DLogChannelRef
	{
		DLogChannel* channel;
		const char* categoryName;
		DLogChannelRefSample samples[3];
	}; assert_sizeof(DLogChannelRef, 0x40);

	struct DLogSampleGroup
	{
		unsigned char type;
		float rate;
		const char* name;
	}; assert_sizeof(DLogSampleGroup, 0x10);

	struct DLogEvent
	{
		const char* name;
		const char* unknownString;
		DLogRow* rows;
		int rowCount;
		DLogHashTable rowHashTable;
		DLogColumn* columns;
		int columnCount;
		DLogHashTable columnHashTable;
		DLogChannelRef* channelRefs;
		int channelRefCount;
		bool requireSchema;
		DLogSampleGroup sampleGroup;
		std::uint64_t protoGuid;
	}; assert_sizeof(DLogEvent, 0x78);
	assert_offsetof(DLogEvent, channelRefs, 0x50);
	assert_offsetof(DLogEvent, sampleGroup, 0x60);

	struct DLogSchema
	{
		const char* name;
		const char* project;
		DLogChannel* channels;
		int channelCount;
		DLogVar* vars;
		int varCount;
		std::uint64_t guidSeed;
		DLogEvent* events;
		int eventCount;
		DLogHashTable eventHashTable;
	}; assert_sizeof(DLogSchema, 0x58);

	struct AttAmmoGeneral
	{
		weapFireType_t fireType;
		int burstCount;
	}; assert_sizeof(AttAmmoGeneral, 0x8);

	struct AttAmmoType
	{
		const char* szAmmoName;
	}; assert_sizeof(AttAmmoType, 0x8);

	struct AttImpact
	{
		ImpactType vfxImpactType;
	}; assert_sizeof(AttImpact, 0x4);

	struct AttAmmoTracer
	{
		TracerDef* tracerType;
		tracerStyle_t tracerStyle;
	}; assert_sizeof(AttAmmoTracer, 0x10);

	struct AttPenetration
	{
		PenetrateType penetrateType;
		float penetrateMultiplier;
		bool rifleBullet;
		bool armorPiercing;
		bool impaling;
	}; assert_sizeof(AttPenetration, 0xC);

	struct AttSight
	{
		char __pad0[0x90];
	}; assert_sizeof(AttSight, 0x90);

	struct AttHoldBreath
	{
		bool canHoldBreath;
	}; assert_sizeof(AttHoldBreath, 0x1);

	struct AttReload
	{
		char __pad0[0xC];
	}; assert_sizeof(AttReload, 0xC);

	struct AttAddOns
	{
		bool silenced;
		bool riotShield;
	}; assert_sizeof(AttAddOns, 0x2);

	struct AttLaser
	{
		LaserDef* laserTypeViewModel;
		LaserDef* laserTypeViewModelAlt;
		LaserDef* laserTypeFriendly;
		LaserDef* laserTypeEnemy;
		LaserSettings laserSettings;
	}; assert_sizeof(AttLaser, 0x30);

	struct AttPost
	{
		float adsFireMotionBlur;
		int adsFireMotionBlurDurationMs;
		float adsFireMotionBlurDecayExponent;
		float hipFireMotionBlur;
		int hipFireMotionBlurDurationMs;
		float hipFireMotionBlurDecayExponent;
		float dofViewModelPhysicalFstop;
		float dofViewModelPhysicalFocusDistance;
		float dofPhysicalFstop;
		float dofPhysicalFocusDistance;
		float adsCameraShakeRotationScale;
		float adsCameraShakeTranslationScale;
		bool dofViewModelPhysicalFocalTag;
		bool dofSceneIsOverridden;
	}; assert_sizeof(AttPost, 0x34);

	struct AttGeneral
	{
		bool boltAction;
		bool inheritsPerks;
		bool reticleSpin45;
		bool adsShouldShowCrosshair;
		float enemyCrosshairRange;
		ReticleType reticleType;
		Material* reticleCenter;
		Material* reticleSide;
		Material* reticleOnePiece;
		int reticleCenterSize;
		int reticleSideSize;
		const char* szLUICrosshairWidget;
	}; assert_sizeof(AttGeneral, 0x38);

	struct AttAimAssist
	{
		float autoAimRange;
		float aimAssistRange;
		float aimAssistRangeAds;
		float aimAssistPitchSlowdown;
		float aimAssistPitchSlowdownAds;
		float aimAssistYawSlowdown;
		float aimAssistYawSlowdownAds;
		float aimAssistLockonStrength;
	}; assert_sizeof(AttAimAssist, 0x20);

	struct AttTargetAssist
	{
		targetAssistType_t targetAssistType;
		targetAssistBehavior_t targetAssistBehavior;
		float targetAssistRange;
		float targetAssistAngle;
		float targetAssistLosOffsetForward;
		float targetAssistLosOffsetRight;
		float targetAssistLosOffsetUp;
		bool targetAssistOnlyAvailableInAds;
	}; assert_sizeof(AttTargetAssist, 0x20);

	struct AttAmmunition
	{
		char __pad0[0x24];
	}; assert_sizeof(AttAmmunition, 0x24);

	struct AttDamage
	{
		WeaponDamageInfo damageInfo;
		char __pad0[0x8];
	}; assert_sizeof(AttDamage, 0x98);

	struct AttMelee
	{
		int meleeDamage;
		bool hasMeleeFinisher;
		int meleeCountToFinisher;
	}; assert_sizeof(AttMelee, 0xC);

	struct AttIdleSettings
	{
		float hipIdleAmount;
		float hipIdleSpeed;
		float idleCrouchFactor;
		float idleProneFactor;
		float adsIdleLerpStartTime;
		float adsIdleLerpTime;
		bool useUninterruptedAdsIdleMotion;
		AdvancedIdleSettings advancedIdleSettings;
	}; assert_sizeof(AttIdleSettings, 0xB4);

	struct AttADSSettings
	{
		float adsSpread;
		float adsSpreadStartFrac;
		float adsSpreadEndFrac;
		float adsAimPitch;
		float adsTransInTime;
		float adsTransOutTime;
		float adsTransInToOutTime;
		float adsTransOutToInTime;
		int adsReloadTransTime;
		float adsInCrosshairAlphaStart;
		float adsInCrosshairAlphaEnd;
		float adsOutCrosshairAlphaStart;
		float adsOutCrosshairAlphaEnd;
		ZoomSettings zoomSettings;
		float adsFovLerpTime;
		float adsBobFactor;
		float adsViewBobMult;
		float adsFireRateScale;
		float adsDamageRangeScale;
		float adsFireAnimFrac;
		float fireTimerLerpToAdsScale;
		bool alwaysFireAtMaxRangeInAds;
		bool adsAlignEnabled;
		bool disableTagAlignX;
		vec3_t adsAlignOffset;
		AdsOffsetInterpolationType adsAlignOffsetLerpType;
		AdsOffsetInterpolationType adsAlignLerpType;
	}; assert_sizeof(AttADSSettings, 0xD8);

	struct AttADSGunBobSettings
	{
		float fAdsGunBobPitchScale;
		float fAdsGunBobYawScale;
		float fAdsGunBobTiltPitchScale;
		float fAdsGunBobTiltYawScale;
		float fAdsGunBobTiltRollScale;
		float fAdsGunBobTiltBulletDirScale;
		float fAdsGunBobTiltOffset;
	}; assert_sizeof(AttADSGunBobSettings, 0x1C);

	struct AttADSReloadSettings
	{
		bool reloadWhileADS;
	}; assert_sizeof(AttADSReloadSettings, 0x1);

	struct AttScopeDriftSettings
	{
		float fScopeDriftDelay;
		float fScopeDriftLerpInTime;
		float fScopeDriftSteadyTime;
		float fScopeDriftLerpOutTime;
		float fScopeDriftSteadyFactor;
		float fScopeDriftUnsteadyFactor;
	}; assert_sizeof(AttScopeDriftSettings, 0x18);

	struct AttADSStanceScales
	{
		float adsCrouchViewKickFactorScale;
		float adsCrouchGunKickFactorScale;
		float adsProneViewKickFactorScale;
		float adsProneGunKickFactorScale;
		float adsCrouchGunBobFactorScale;
		WeaponOffsetPatternScaleInfo crouchedWeaponOffsetPatternScales;
		WeaponOffsetPatternScaleInfo proneWeaponOffsetPatternScales;
	}; assert_sizeof(AttADSStanceScales, 0x38);

	struct AttHipSpread
	{
		float hipSpreadStandMin;
		float hipSpreadDuckedMin;
		float hipSpreadProneMin;
		float hipSpreadSprintMin;
		float hipSpreadInAirMin;
		float hipSpreadMax;
		float hipSpreadDuckedMax;
		float hipSpreadProneMax;
		float hipSpreadSprintMax;
		float hipSpreadInAirMax;
		float hipSpreadFireAdd;
		float hipSpreadTurnAdd;
		float hipSpreadMoveAdd;
		float hipSpreadDecayRate;
		float hipSpreadDuckedDecay;
		float hipSpreadProneDecay;
		float hipSpreadSprintDecay;
		float hipSpreadInAirDecay;
		float hipSpreadStandMoveMax;
		float hipSpreadDuckedMoveMax;
		float hipSpreadProneMoveMax;
		float hipSpreadSprintMoveMax;
		float hipSpreadInAirMoveMax;
	}; assert_sizeof(AttHipSpread, 0x5C);

	struct AttSlideSpread
	{
		float slideSpreadMin;
		float slideSpreadMax;
		float slideSpreadDecayRate;
		float slideSpreadFireAdd;
		float slideSpreadTurnAdd;
		float slideSpreadMoveMax;
	}; assert_sizeof(AttSlideSpread, 0x18);

	struct AttGunKick
	{
		char __pad0[0x184];
	}; assert_sizeof(AttGunKick, 0x184);

	struct AttGunTilt
	{
		float adsGunTiltPitchFactor;
		float adsGunTiltYawFactor;
		float adsGunTiltRollFactor;
		float adsGunTiltOffset;
		float hipGunTiltPitchFactor;
		float hipGunTiltYawFactor;
		float hipGunTiltRollFactor;
		float hipGunTiltOffset;
	}; assert_sizeof(AttGunTilt, 0x20);

	struct AttKickScaling
	{
		float hipStartingGunKickPercent;
		float hipStartingViewKickPercent;
		int adsStartingKickBullets;
		float adsStartingGunKickPercent;
		float adsStartingViewKickPercent;
		float hipEndingGunKickPercent;
		float hipEndingViewKickPercent;
		int adsEndingKickBullets;
		float adsEndingGunKickPercent;
		float adsEndingViewKickPercent;
	}; assert_sizeof(AttKickScaling, 0x28);

	struct AttViewKick
	{
		char __pad0[0x170];
	}; assert_sizeof(AttViewKick, 0x170);

	struct AttADSOverlay
	{
		ADSOverlay overlay;
		WeaponAttachment* hybridScopeAttachment;
		char __pad0[0x30];
	}; assert_sizeof(AttADSOverlay, 0xA8);

	struct AttOutline
	{
		vec4_t outlineColor;
		vec4_t outlineAltColor;
		vec4_t outlineFillColor0;
		vec4_t outlineFillColor1;
		vec4_t outlineOccludedOutlineColor;
		vec4_t outlineOccludedInlineColor;
		vec4_t outlineOccludedInteriorColor;
		float outlineWidth;
		bool outlineEnemies;
		bool outlineDepthTest;
		bool outlineFill;
		bool outlineScriptControlled;
		bool outlineVehicles;
		bool outlineTurrets;
		bool outlineCharacters;
		bool outlineAgents;
		bool outlineScriptMovers;
		bool outlineLockOn;
	}; assert_sizeof(AttOutline, 0x80);

	struct AttUI
	{
		GfxImage* hudIcon;
		GfxImage* pickupIcon;
		GfxImage* unknownImage0;
		char __pad0[0x8];
		GfxImage* unknownImage1;
		GfxImage* unknownImage2;
		char __pad1[0x8];
		Material* unknownMaterial0;
		char __pad2[0x8];
		Material* unknownMaterial1;
		char __pad3[0x8];
		GfxImage* dpadIconImg;
		GfxImage* dpadIconInactiveImg;
		Material* dpadIconMat;
		Material* dpadIconInactiveMat;
		char __pad4[0x8];
	}; assert_sizeof(AttUI, 0x80);

	struct AttRumbles
	{
		RumbleInfo* fireRumble;
		RumbleInfo* meleeImpactRumble;
	}; assert_sizeof(AttRumbles, 0x10);

	struct AttProjectile
	{
		char __pad0[0x40];
		XModel* projectileModel;
		char __pad1[0x8];
		FxCombinedDef projExplosionEffect;
		char __pad2[0x8];
		SndAliasLookup projExplosionSound;
		SndAliasLookup unknownSound0;
		FxCombinedDef projDudEffect;
		FxCombinedDef projTimeoutEffect;
		SndAliasLookup projDudSound;
		SndAliasLookup projTimeoutSound;
		char __pad3[0x10];
		FxCombinedDef vmProjBodyEffect;
		FxCombinedDef projBodyEffect;
		FxCombinedDef projTrailEffect;
		FxCombinedDef projBeaconEffect;
		char __pad4[0x8];
		FxCombinedDef projIgnitionEffect;
		SndAliasLookup projIgnitionSound;
		char __pad5[0x30];
	}; assert_sizeof(AttProjectile, 0x108);

	struct AttCharged
	{
		unsigned int chargeType;
		float chargeGain;
		float chargeCostPerShot;
		float chargeLossWhenIdle;
		float chargeEmptyCooldown;
		float chargeFireAtMaxDamageMultiplier;
		int chargeMeterEffectBoneCount;
		FxCombinedDef chargeMeterEffect;
		SndAliasLookup chargeUpSound;
		SndAliasLookup chargeDownSound;
		SndAliasLookup chargeDownToUpSound;
		SndAliasLookup chargeUpToDownSound;
		SndAliasLookup chargeMaxSound;
		SndAliasLookup chargeUpSoundPlayer;
		SndAliasLookup chargeDownSoundPlayer;
		SndAliasLookup chargeDownToUpSoundPlayer;
		SndAliasLookup chargeUpToDownSoundPlayer;
		SndAliasLookup chargeMaxSoundPlayer;
		bool chargeHudReveal;
		RumbleInfo* chargeRumble;
	}; assert_sizeof(AttCharged, 0x88);

	enum AdsAltSwitchInterpType : std::int32_t
	{
		ADS_ALT_SWITCH_INTERP_TYPE_LINEAR = 0x0,
		ADS_ALT_SWITCH_INTERP_TYPE_COSINE = 0x1,
		ADS_ALT_SWITCH_INTERP_TYPE_SINE = 0x2,
		ADS_ALT_SWITCH_INTERP_TYPE_COUNT = 0x3,
	};

	struct AdsAltSwitch
	{
		float fovInterpTimeSecMainToAlt;
		float fovInterpDelaySecMainToAlt;
		float dualFovSwitchRateMainToAlt;
		AdsAltSwitchInterpType fovInterpTypeMainToAlt;
		float fovInterpTimeSecAltToMain;
		float fovInterpDelaySecAltToMain;
		float dualFovSwitchRateAltToMain;
		AdsAltSwitchInterpType fovInterpTypeAltToMain;
	}; assert_sizeof(AdsAltSwitch, 0x20);

	struct AttRegeneration
	{
		bool regenerationEnabled;
		int regenerationTimeMs;
		int regenerationAddTimeMs;
		int regenerationAmount;
		bool regenerationConsumeStock;
		bool regenerationDisableWhileFiring;
	}; assert_sizeof(AttRegeneration, 0x14);

	struct AttHeat
	{
		float heatAccumulationPerShot;
		float heatDissipationPerSecond;
		float heatSmokeStartThreshold;
		float heatSmokeStopThreshold;
	}; assert_sizeof(AttHeat, 0x10);

	struct AttMovement
	{
		char __pad0[0x20];
	}; assert_sizeof(AttMovement, 0x20);

	struct AttBurst
	{
		int burstFireCooldown;
		bool burstFireAuto;
	}; assert_sizeof(AttBurst, 0x8);

	struct AttHitmarker
	{
		HitmarkerType hitmarkerType;
	}; assert_sizeof(AttHitmarker, 0x4);

	struct AttAdsMovement
	{
		float forwardOffset;
		float roll;
		bool isOverride;
	}; assert_sizeof(AttAdsMovement, 0xC);

	struct AttDlcOverrides
	{
		char __pad0[0x34];
	}; assert_sizeof(AttDlcOverrides, 0x34);

	struct AttDlcScalars
	{
		char __pad0[0x28];
	}; assert_sizeof(AttDlcScalars, 0x28);

	struct AttUnknownScales
	{
		char __pad0[0x40];
		WeaponOffsetPatternScaleInfo weaponOffsetPatternScales;
	}; assert_sizeof(AttUnknownScales, 0x50);

	struct AttWeaponTrigger
	{
		WeaponTrigger* weaponTrigger;
	}; assert_sizeof(AttWeaponTrigger, 0x8);

	enum AttachmentType : std::int32_t
	{
		ATTACHMENT_SCOPE = 0x0,
		ATTACHMENT_UNDERBARREL = 0x1,
		ATTACHMENT_OTHER = 0x2,
		ATTACHMENT_COUNT = 0x3,
	};

	enum altModePlayerAnimType_t : std::int32_t
	{
		ALTMODEPLAYERANIMTYPE_NONE = 0x0,
		ALTMODEPLAYERANIMTYPE_LAUNCHER = 0x1,
		ALTMODEPLAYERANIMTYPE_SHOTGUN = 0x2,
		ALTMODEPLAYERANIMTYPE_FIRESELECTOR = 0x3,
		NUM_ALTMODEPLAYERANIMTYPES = 0x4,
	};

	enum DynamicAttachmentType : std::int32_t
	{
		DYNAMIC_ATTACHMENT_TYPE_MUZZLE = 0x0,
		DYNAMIC_ATTACHMENT_TYPE_UNDERBARREL = 0x1,
		DYNAMIC_ATTACHMENT_TYPE_LASER = 0x2,
		DYNAMIC_ATTACHMENT_TYPE_GUARD = 0x3,
		DYNAMIC_ATTACHMENT_TYPE_MAGAZINE = 0x4,
		DYNAMIC_ATTACHMENT_TYPE_RECEIVER = 0x5,
		DYNAMIC_ATTACHMENT_TYPE_SCOPE = 0x6,
		DYNAMIC_ATTACHMENT_TYPE_STOCK = 0x7,
		DYNAMIC_ATTACHMENT_TYPE_FIRETYPE = 0x8,
		DYNAMIC_ATTACHMENT_TYPE_BARREL = 0x9,
		DYNAMIC_ATTACHMENT_TYPE_OTHER = 0xA,
		DYNAMIC_ATTACHMENT_TYPE_COUNT = 0xB,
	};

	enum WeaponFlagOverride : std::int32_t
	{
		WEAPON_FLAG_OVERRIDE_NO_OVERRIDE = 0x0,
		WEAPON_FLAG_OVERRIDE_FALSE = 0x1,
		WEAPON_FLAG_OVERRIDE_TRUE = 0x2,
		WEAPON_FLAG_OVERRIDE_NUM = 0x3,
	};

	struct WeaponAttachment
	{
		const char* name;
		const char* szDisplayName;
		scr_string_t internalName;
		scr_string_t overrideCategoryName;
		char __pad0[0x18];
		unsigned int numReticles;
		unsigned int numAltReticles;
		XModel* fallbackWorldModel;
		XModel* fallbackViewModel;
		unsigned int numModelVariations;
		char __pad1[0x4];
		XModel** worldModelVariations;
		XModel** viewModelVariations;
		scr_string_t attachPoint;
		unsigned int numHideTags;
		scr_string_t* hideTags;
		unsigned int numHideTagsNonViewModel;
		char __pad2[0x4];
		scr_string_t* hideTagsNonViewModel;
		unsigned int numShowTags;
		char __pad3[0x4];
		scr_string_t* showTags;
		ReticleDef** reticles;
		ReticleDef** altReticles;
		AttAmmoGeneral* ammogeneral;
		AttAmmoType* ammotype;
		AttImpact* impact;
		FxCombinedDef unknownEffect;
		char __pad4[0x8];
		const char* dynamicAttachmentAnimateSeq;
		const char* dynamicAttachmentIcon;
		char __pad5[0x8];
		AttAmmoTracer* tracer;
		AttPenetration* penetration;
		AttSight* sight;
		AttHoldBreath* holdBreath;
		AttReload* reload;
		AttAddOns* addOns;
		AttLaser* laser;
		AttPost* post;
		AttGeneral* general;
		AttAimAssist* aimAssist;
		AttTargetAssist* targetAssist;
		AttAmmunition* ammunition;
		AttDamage* damage;
		AttMelee* melee;
		AttIdleSettings* idleSettings;
		AttADSSettings* adsSettings;
		AttADSSettings* adsSettingsMain;
		AttADSGunBobSettings* adsGunBobSettings;
		AttADSGunBobSettings* adsGunBobSettingsMain;
		AttADSReloadSettings* adsReloadSettings;
		AttScopeDriftSettings* scopeDriftSettings;
		AttScopeDriftSettings* scopeDriftSettingsMain;
		AttADSStanceScales* adsStanceScales;
		AttUnknownScales* unknownScales;
		AttHipSpread* hipSpread;
		AttHipSpread* unknownSpread;
		AttSlideSpread* slideSpread;
		AttGunKick* gunKick;
		AttGunTilt* gunTilt;
		AttKickScaling* kickScaling;
		AttKickScaling* unknownKickScaling;
		AttViewKick* viewKick;
		AttADSOverlay* adsOverlay;
		AttOutline* outline;
		AttUI* ui;
		AttRumbles* rumbles;
		char __pad6[0x8];
		AttProjectile* projectile;
		AttCharged* charged;
		AdsAltSwitch* adsAltSwitch;
		AttRegeneration* regeneration;
		AttHeat* heat;
		AttMovement* movement;
		AttMovement* movementScale;
		AttMovement* movementScaleMain;
		AttBurst* burst;
		AttHitmarker* hitmarker;
		AttAdsMovement* adsMovement;
		BallisticInfo* ballisticInfo;
		unsigned char* unknownData0;
		SwaySettings* swaySettings;
		WeaponOffsetPatternScaleInfo* weaponOffsetPatternScaleInfo;
		unsigned char* unknownData1;
		unsigned char* unknownData2;
		float* locationDamage;
		float* unknownLocationDamage;
		char __pad7[0x1B0];
		const char* szAltModeName;
		const char* szAltModeInactiveName;
		const char* szLUIWeaponInfoWidgetName;
		const char* szLUIWeaponInfoWidgetTag;
		GfxImage* altModeIcon;
		AttDlcOverrides* dlcOverrides;
		AttDlcScalars* dlcScalars;
		AttWeaponTrigger* weaponTrigger;
		const char* unknownString0;
		const char* unknownString1;
		char __pad8[0x10];
		unsigned char* unknownData5;
		char __pad9[0x8];
	}; assert_sizeof(WeaponAttachment, 0x4C0);

	struct LaserDef
	{
		const char* name;
		Material* laserMaterial;
		Material* laserLightMaterial;
		bool ownerOnly;
		bool nightvisionOnly;
		float range;
		float radius;
		float endOffsetViewmodel;
		float endOffsetOther;
		float flarePct;
		FxCombinedDef beamEffect;
		FxCombinedDef laserEndEffect;
		bool clientCollision;
		vec4_t color;
		vec4_t hdrColorScale;
		bool laserLight;
		bool laserLightNvgOnly;
		float laserLightRadius;
		float laserLightBeginOffset;
		float laserLightEndOffset;
		float laserLightBodyTweak;
		vec4_t laserLightColor;
		vec4_t laserLightHdrColorScale;
		float range_alt;
		float radius_alt;
		float laserLightRadius_alt;
		float flarePct_alt;
	}; assert_sizeof(LaserDef, 0xA8);

	enum CinematicShakeModifierType : std::int32_t
	{
	};

	enum CinematicShakeModifierCombineType : std::int32_t
	{
	};

	struct CinematicShakeModifierDef
	{
		CinematicShakeModifierType type;
		int inBlendInTimeMs;
		int inBlendOutTimeMs;
		vec2_t inSpeedRange;
		vec2_t outAmplitudeScale;
		vec2_t outPersistenceScale;
		vec2_t outSpeedScale;
		int outBlendInTimeMs;
		int outBlendOutTimeMs;
		RumbleGraph* nonLinearMap;
	}; assert_sizeof(CinematicShakeModifierDef, 0x40);

	struct CinematicShakeDef
	{
		bool enabled;
		vec3_t frequency;
		vec3_t amplitude;
		vec3_t persistence;
		CinematicShakeModifierDef moveSpeedModifiers;
		CinematicShakeModifierDef viewSpeedModifiers;
		CinematicShakeModifierCombineType modifierCombineType;
	}; assert_sizeof(CinematicShakeDef, 0xB0);

	struct CinematicImpactShakeDef
	{
		vec3_t frequency;
		vec3_t amplitude;
		vec3_t persistence;
	};

	struct CinematicMotionDef
	{
		const char* name;
		vec3_t camera_spring_velocitySmoothingSec;
		vec3_t camera_spring_velocityScale;
		vec3_t camera_spring_springConstant;
		vec3_t camera_spring_damperConstant;
		vec3_t camera_spring_maxOffset;
		vec3_t viewmodel_spring_velocitySmoothingSec;
		vec3_t viewmodel_spring_velocityScale;
		vec3_t viewmodel_spring_springConstant;
		vec3_t viewmodel_spring_damperConstant;
		vec3_t viewmodel_spring_maxOffset;
		vec3_t viewmodel_spring_offsetToAnglesX;
		vec3_t viewmodel_spring_offsetToAnglesY;
		vec3_t viewmodel_spring_offsetToAnglesZ;
		CinematicShakeDef camera_shake_rotation;
		CinematicShakeDef camera_shake_translation;
		CinematicImpactShakeDef camera_shake_impact_rotation;
		CinematicImpactShakeDef camera_shake_impact_translation;
	}; assert_sizeof(CinematicMotionDef, 0x250);

	struct AccessoryDef
	{
		const char* name;
		scr_string_t VMAttachBone;
		bool VMAttachRightHand;
		Gesture* interactGesture;
		const char* widgetVM;
		int widgetVMRTTWidth;
		int widgetVMRTTHeight;
		int widgetVMRTTMipCount;
	}; assert_sizeof(AccessoryDef, 0x30);

	struct ASM_Transition
	{
		int m_ToStateID;
		ASM_Function m_FuncName;
		int m_Priority;
	}; assert_sizeof(ASM_Transition, 0x30);

	struct AI_ASM_State
	{
		int m_PowerDownState;
		int m_PowerDownNumParams;
		ASM_Function_Param* m_PowerDownParams;
		scr_string_t m_AnimMode;
		scr_string_t m_OrientMode;
		scr_string_t m_AnimModeMP;
		scr_string_t m_OrientModeMP;
	}; assert_sizeof(AI_ASM_State, 0x20);

	struct Player_ASM_State
	{
		ASM_Function m_UpdateFunc;
		int m_ScriptedAnimState;
		int m_ScriptedAnimNumParams;
		ASM_Function_Param* m_ScriptedAnimParams;
		char padding[16];
	}; assert_sizeof(Player_ASM_State, 0x40);

	union ASM_State_Union
	{
		AI_ASM_State* m_AIState;
		Player_ASM_State* m_PlayerState;
	};

	struct ASM_State
	{
		ASM_Transition* m_Transitions;
		int m_NumTransitions;
		scr_string_t m_Name;
		int m_ID;
		int m_Type;
		int m_Stance;
		int m_Flags;
		int m_LogicGroupID;
		scr_string_t* m_Subtrees;
		int m_NumSubtrees;
		ASM_Function m_PlayAnim;
		ASM_Function m_PlayAnimNoteHandler;
		ASM_Function m_ChooseAnimFunc;
		ASM_Function m_TerminateFunc;
		scr_string_t m_ExitNotify;
		scr_string_t m_FacialState;
		int m_DeathState;
		int m_DeathNumParams;
		ASM_Function_Param* m_DeathParams;
		int m_PainState;
		int m_PainNumParams;
		ASM_Function_Param* m_PainParams;
		int m_ExecutionState;
		int m_ExecutionNumParams;
		ASM_Function_Param* m_ExecutionParams;
		ASM_State_Union u;
	}; assert_sizeof(ASM_State, 0xF8);

	struct ASM_LogicGroup
	{
		int m_ID;
		int m_ParentID;
		ASM_Function m_InitFunc;
		ASM_Function m_TerminateFunc;
	}; assert_sizeof(ASM_LogicGroup, 0x48);

	struct AI_ASM
	{
		int* m_Services;
		int m_NumServices;
		int m_ScriptHandler;
	}; assert_sizeof(AI_ASM, 0x10);

	struct Player_ASM
	{
		char unused;
	};

	union ASM_Union
	{
		AI_ASM* m_AIASM;
		Player_ASM* m_PlayerASM;
	};

	struct ASM
	{
		const char* m_szName;
		scr_string_t m_Name;
		ASM_State* m_States;
		int m_NumStates;
		ASM_LogicGroup* m_LogicGroups;
		int m_NumLogicGroups;
		int* m_FuncIDs;
		int m_NumFuncIDs;
		int m_StartState;
		ASM_Mode m_Mode;
		ASM_Union u;
	}; assert_sizeof(ASM, 0x50);

	enum ScriptBundleValueType : unsigned char
	{
		SCRIPT_BUNDLE_VALUE_OBJECT = 5,
		SCRIPT_BUNDLE_VALUE_ARRAY = 6,
	};

	struct ScriptBundleObjectEntry;
	struct ScriptBundleValue;

	struct ScriptBundleObject
	{
		unsigned int count;
		ScriptBundleObjectEntry* entries;
	}; assert_sizeof(ScriptBundleObject, 0x10);

	struct ScriptBundleArray
	{
		unsigned int count;
		ScriptBundleValue* values;
	}; assert_sizeof(ScriptBundleArray, 0x10);

	union ScriptBundleValueData
	{
		ScriptBundleObject object;
		ScriptBundleArray array;
	};

	struct ScriptBundleValue
	{
		ScriptBundleValueType type;
		ScriptBundleValueData data;
	}; assert_sizeof(ScriptBundleValue, 0x18);
	assert_offsetof(ScriptBundleValue, data, 0x8);

	struct ScriptBundleObjectEntry
	{
		SndAliasLookup unknownAlias;
		char unknown[0x8];
		ScriptBundleValue value;
	}; assert_sizeof(ScriptBundleObjectEntry, 0x28);

	struct ScriptBundle
	{
		const char* name;
		int dataSize;
		char* data;
		ScriptBundleObject root;
	}; assert_sizeof(ScriptBundle, 0x28);

	struct CodcasterImageEntry
	{
		const char* name;
		GfxImage* image;
		char unknown[0x40];
	}; assert_sizeof(CodcasterImageEntry, 0x50);

	struct CodcasterStringPair
	{
		const char* key;
		const char* value;
	}; assert_sizeof(CodcasterStringPair, 0x10);

	struct CodcasterData
	{
		const char* name;
		unsigned int imageCount;
		CodcasterImageEntry* images;
		CodcasterImageEntry unknownImage;
		unsigned int stringPairCount;
		CodcasterStringPair* stringPairs;
		char unknown[0x68];
	}; assert_sizeof(CodcasterData, 0xE0);
	assert_offsetof(CodcasterData, unknownImage, 0x18);
	assert_offsetof(CodcasterData, stringPairCount, 0x68);
	assert_offsetof(CodcasterData, stringPairs, 0x70);

	struct SndModeGlobalsSubEntry
	{
		const char* name;
		unsigned int count;
		const char** strings;
		SndAliasLookup* aliases;
	}; assert_sizeof(SndModeGlobalsSubEntry, 0x20);

	struct SndModeGlobalsEntry
	{
		const char* strings[4];
		unsigned int count;
		SndModeGlobalsSubEntry* subEntries;
		SndAliasLookup* aliases;
		unsigned int* values;
	}; assert_sizeof(SndModeGlobalsEntry, 0x40);

	struct SndModeGlobals
	{
		const char* name;
		unsigned int count;
		SndModeGlobalsEntry* entries;
		SndAliasLookup* aliases;
	}; assert_sizeof(SndModeGlobals, 0x20);

	struct SndGlobalsUnknown0
	{
		char unknown0[0x40];
		SndAliasLookup unknownSound0;
		SndAliasLookup unknownSound1;
		SndBankTransient* soundBank;
		char unknown1[0xA0];
	}; assert_sizeof(SndGlobalsUnknown0, 0xF8);

	struct SndGlobalsUnknown1
	{
		char unknown0[0x40];
		SndAliasLookup unknownSound;
		char unknown1[0x8];
	}; assert_sizeof(SndGlobalsUnknown1, 0x50);

	struct SndGlobalsUnknown2
	{
		char unknown0[0x40];
		SndAliasLookup unknownSound;
		char unknown1[0x10];
	}; assert_sizeof(SndGlobalsUnknown2, 0x58);

	struct SndGlobalsUnknown3
	{
		const char* name;
		char unknown[0x18];
	}; assert_sizeof(SndGlobalsUnknown3, 0x20);

	struct SndGlobalsUnknown4
	{
		const char* name;
		SndAliasLookup unknownSound;
	}; assert_sizeof(SndGlobalsUnknown4, 0x10);

	struct SndGlobalsUnknown5
	{
		char unknown0[0x40];
		SndAliasLookup unknownSound;
		char unknown1[0x260];
	}; assert_sizeof(SndGlobalsUnknown5, 0x2A8);

	struct SndGlobalsUnknown6
	{
		char unknown0[0x40];
		SndAliasLookup unknownSound;
		char unknown1[0x40];
	}; assert_sizeof(SndGlobalsUnknown6, 0x88);

	struct SndGlobalsUnknown8
	{
		char unknown[0x88];
	}; assert_sizeof(SndGlobalsUnknown8, 0x88);

	struct SndGlobals
	{
		const char* name;
		char unknown0[0x8];
		unsigned int unknownCount0;
		SndGlobalsUnknown0* unknown0Entries;
		unsigned int unknownCount1;
		SndGlobalsUnknown1* unknown1Entries;
		unsigned int unknownCount2;
		SndGlobalsUnknown2* unknown2Entries;
		unsigned int unknownCount3;
		SndGlobalsUnknown3* unknown3Entries;
		unsigned int unknownCount4;
		SndGlobalsUnknown4* unknown4Entries;
		unsigned int unknownCount5;
		SndGlobalsUnknown5* unknown5Entries;
		unsigned int unknownCount6;
		SndGlobalsUnknown6* unknown6Entries;
		unsigned int unknownCount7;
		SndGlobalsUnknown1* unknown7Entries;
		unsigned int unknownCount8;
		SndGlobalsUnknown8* unknown8Entries;
		SndGlobalsUnknown2* unknown9Entries;
	}; assert_sizeof(SndGlobals, 0xA8);
	assert_offsetof(SndGlobals, unknown0Entries, 0x18);
	assert_offsetof(SndGlobals, unknown5Entries, 0x68);
	assert_offsetof(SndGlobals, unknown8Entries, 0x98);
	assert_offsetof(SndGlobals, unknown9Entries, 0xA0);

	struct SuitAnimEntry
	{
		XAnimParts* anim;
		int time;
		int interruptTime;
		unsigned int animType;
	}; assert_sizeof(SuitAnimEntry, 0x18);

	struct SuitGestureEntry
	{
		Gesture* gesture;
		std::int32_t gestureAnimType;
	}; assert_sizeof(SuitGestureEntry, 0x10);

	struct SuitAnimPerWeapClass
	{
		SuitAnimEntry* anims;
		unsigned int numAnims;
		int weapClasses;
		SuitGestureEntry* gestures;
		unsigned int numGestures;
		CinematicMotionDef* cinematicMotion;
		FootstepAnim footstep[13];
		float skydiveParachuteSlowSoftLandVelocity;
		float skydiveParachuteSlowHardLandVelocity;
		float skydiveParachuteFastLandVelocity;
	}; assert_sizeof(SuitAnimPerWeapClass, 0x1A68);

	struct SuitAnimPackage
	{
		const char* name;
		SuitAnimPerWeapClass* animOverrides;
	}; assert_sizeof(SuitAnimPackage, 0x10);

	struct LocDmgTable
	{
		const char* name;
		float mults[22];
	}; assert_sizeof(LocDmgTable, 0x60);

	struct XCamTargetModelFrame
	{
		vec3_t offset;
		vec4_t axisQuat;
	}; assert_sizeof(XCamTargetModelFrame, 0x1C);

	struct XCamFrame
	{
		int frameNum;
		vec3_t origin;
		vec4_t angleQuat;
		float fov;
		float focalLength;
		float focalDistance;
		float fStop;
	}; assert_sizeof(XCamFrame, 0x30);

	enum LbColType : std::int32_t
	{
		LBCOL_TYPE_NUMBER = 0x0,
		LBCOL_TYPE_TIME = 0x1,
		LBCOL_TYPE_LEVELXP = 0x2,
		LBCOL_TYPE_PRESTIGE = 0x3,
		LBCOL_TYPE_BIGNUMBER = 0x4,
		LBCOL_TYPE_PERCENT = 0x5,
		LBCOL_TYPE_TIME_FULL = 0x6,
		LBCOL_TYPE_COUNT = 0x7,
	};

	enum LbAggType : std::int32_t
	{
		LBAGG_TYPE_MIN = 0x0,
		LBAGG_TYPE_MAX = 0x1,
		LBAGG_TYPE_SUM = 0x2,
		LBAGG_TYPE_LAST = 0x3,
		LBAGG_TYPE_COUNT = 0x4,
	};

	enum XCamApertureType : std::int32_t
	{
		XCAM_FOV_APERTURE = 0x0,
		XCAM_FOCAL_LENGTH_APERTURE = 0x1,
		COUNT_XCAM_APERTURE = 0x2,
	};

	struct BulletPenetration
	{
		const char* name;
		float depthTable[7][64];
	}; assert_sizeof(BulletPenetration, 0x708);

	struct XCamTargetModel
	{
		const char* name;
		int frames;
		XCamTargetModelFrame* animationFrames;
	}; assert_sizeof(XCamTargetModel, 0x18);

	struct XCamCamera
	{
		const char* name;
		int index;
		int frames;
		XCamApertureType apertureType;
		XCamFrame* animationFrames;
		float farZ;
		float focusShiftMultiplier;
	}; assert_sizeof(XCamCamera, 0x28);

	struct XCamCameraSwitchParams
	{
		int frameNum;
		int camera1Index;
		int camera2Index;
		float dissolve;
	}; assert_sizeof(XCamCameraSwitchParams, 0x10);

	struct XCamFrameName
	{
		const char* frameName;
		int frameNum;
	}; assert_sizeof(XCamFrameName, 0x10);

	enum LbUpdateType : std::int32_t
	{
		LBUPDATE_TYPE_NORMAL = 0x0,
		LBUPDATE_TYPE_RANK = 0x1,
		LBUPDATE_TYPE_COMBINE = 0x2,
		LBUPDATE_TYPE_VIRTUAL = 0x3,
		LBUPDATE_TYPE_COUNT = 0x4,
	};

	struct LbColumnDef
	{
		const char* name;
		int id;
		int propertyId;
		bool hidden;
		int statsGroup;
		const char* statName;
		LbColType type;
		int precision;
		LbAggType agg;
		int uiCalColX;
		int uiCalColY;
	}; assert_sizeof(LbColumnDef, 0x38);

	struct XCam
	{
		const char* name;
		const char* parent_scene;
		bool hideHud;
		bool looping;
		bool hideLocalPlayerModel;
		bool useFirstPersonPlayerModel;
		bool disableNearDof;
		bool autoMotionBlur;
		bool easeAnimationsOut;
		bool showNonAnimatedPlayers;
		int cameraCount;
		int frames;
		int frameRate;
		int frameNameCount;
		vec4_t rotationOffsetInv;
		vec3_t offset;
		XCamTargetModel* targetModelBoneRoots;
		XCamCamera* cameras;
		XCamCameraSwitchParams* cameraSwitchParams;
		XCamFrameName* frameNames;
		XAnimNotifyInfo* notify;
		char notifyCount;
		XAnimNotifyInfo* startupNotify;
		char startupNotifyCount;
		XAnimNotifyInfo* shutdownNotify;
		char shutdownNotifyCount;
		vec3_t rightStickRotateOffset;
		vec2_t rightStickRotateMaxDegrees;
		float freeCursorParallax;
	}; assert_sizeof(XCam, 0xB0);

	struct LeaderboardDef
	{
		const char* name;
		int id;
		int sourceLbId;
		int sourceLbWidth;
		int columnCount;
		int xpColId;
		int prestigeColId;
		LbColumnDef* columns;
		LbUpdateType updateType;
		int trackTypes;
		int rankColIdX;
		int rankColIdY;
		int unknown[2];
	}; assert_sizeof(LeaderboardDef, 0x40);
	assert_offsetof(LeaderboardDef, columnCount, 0x14);
	assert_offsetof(LeaderboardDef, columns, 0x20);

	struct LbVrColumnDef
	{
		const char* name;
		int id;
		int uiCalColX;
		int uiCalColY;
	}; assert_sizeof(LbVrColumnDef, 0x18);

	struct VirtualLeaderboardDef
	{
		const char* name;
		const char* sourceName;
		int id;
		int sourceId;
		LbVrColumnDef* columns;
		int columnCount;
		int rankColIdX;
		int rankColIdY;
	}; assert_sizeof(VirtualLeaderboardDef, 0x30);

	struct ExecutionDef
	{
		const char* name;
		unsigned int animType;
		bool usePropWeapon;
		scr_string_t propWeaponAttachTag;
		bool propWeaponInventoryOverrideEnabled;
		bool propWeaponEnableIK;
		weapClass_t propWeaponInventoryOverrideClass;
		int durationVictimStandMs;
		XCam* xcamVictimStand;
		int durationVictimProneMs;
		XCam* xcamVictimProne;
		int durationVictimLastStandMs;
		XCam* xcamVictimLastStand;
		float dlcFloat[8];
	}; assert_sizeof(ExecutionDef, 0x68);

	struct DDLMember
	{
		const char* name;
		char unknown[0x28];
	}; assert_sizeof(DDLMember, 0x30);

	struct DDLHash
	{
		unsigned int hash;
		int index;
	};

	struct DDLHashTable
	{
		DDLHash* list;
		int count;
		int max;
	}; assert_sizeof(DDLHashTable, 0x10);

	struct DDLStruct
	{
		const char* name;
		int bitSize;
		int memberCount;
		DDLMember* members;
		DDLHashTable hashTableUpper;
		DDLHashTable hashTableLower;
	}; assert_sizeof(DDLStruct, 0x38);

	struct DDLEnum
	{
		const char* name;
		int memberCount;
		const char** members;
		DDLHashTable hashTable;
	}; assert_sizeof(DDLEnum, 0x28);

	struct DDLDef
	{
		char* name;
		unsigned short version;
		std::uint64_t guidSeed;
		std::uint64_t guid;
		int bitSize;
		int byteSize;
		DDLStruct* structList;
		int structCount;
		DDLEnum* enumList;
		int enumCount;
		DDLDef* next;
		int headerBitSize;
		int headerByteSize;
		bool paddingUsed;
		bool minimalHeader;
	}; assert_sizeof(DDLDef, 0x60);

	struct DDLFile
	{
		char* name;
		DDLDef* ddlDef;
	}; assert_sizeof(DDLFile, 0x10);

	enum SuitBodyAnimType : std::int32_t
	{
		SUITBODYANIMTYPE_HUMAN = 0x0,
		SUITBODYANIMTYPE_JUGGERNAUT = 0x1,
		SUITBODYANIMTYPE_COUNT = 0x2,
	};

	struct SuitSoundTableEntry
	{
		unsigned int count;
		SndAliasLookup* aliases;
		unsigned int* values;
	}; assert_sizeof(SuitSoundTableEntry, 0x18);

	struct SuitSoundTable
	{
		unsigned int count;
		SuitSoundTableEntry* entries;
		SndAliasLookup* aliases;
		unsigned int* values;
	}; assert_sizeof(SuitSoundTable, 0x20);

	struct SuitDef
	{
		const char* name;
		char unknown0[0x18];
		CameraDef* cam_helmetCam;
		char unknown1[0xA8];
		SndAliasLookup unknownSound0;
		SndAliasLookup unknownSound1;
		SndAliasLookup unknownSound2;
		SndAliasLookup unknownSound3;
		char unknown2[0xC8];
		SuitAnimPackage* animPackage;
		SuitAnimPackage* animPackageL;
		SuitAnimPackage* animPackageR;
		SuitAnimPackage* animPackageRelaxed;
		SuitAnimPackage* animPackageSafe;
		ASM* asmAsset;
		Animset* animsetAsset;
		char unknown3[0x8];
		ScriptableDef* scriptableDef;
		char unknown4[0x100];
		RumbleInfo* groundPound_activationRumble;
		SndAliasLookup unknownSound4;
		SndAliasLookup unknownSound5;
		RumbleInfo* groundPound_landingRumble;
		SndAliasLookup unknownSound6;
		SndAliasLookup unknownSound7;
		RumbleInfo* landing_rumbleLowHeight;
		RumbleInfo* landing_rumbleMediumHeight;
		RumbleInfo* landing_rumbleHighHeight;
		RumbleInfo* landing_rumbleExtremeHeight;
		char unknown5[0x10];
		RumbleInfo* footstep_rumble;
		FootstepVFX* footstepvfx_slide;
		FootstepVFX* footstepvfx_creep_left;
		FootstepVFX* footstepvfx_creep_right;
		FootstepVFX* footstepvfx_walk_left;
		FootstepVFX* footstepvfx_walk_right;
		FootstepVFX* footstepvfx_run_left;
		FootstepVFX* footstepvfx_run_right;
		FootstepVFX* footstepvfx_sprint_left;
		FootstepVFX* footstepvfx_sprint_right;
		char unknown6[0x8];
		XModel* skydive_parachuteViewModel;
		XModel* skydive_parachuteWorldModel;
		char unknown7[0x28];
		CinematicMotionDef* skydive_freefallCinematicMotion;
		char unknown8[0x50];
		RumbleGraph* skydive_freefallLookHorizForceGraph;
		char unknown9[0x8];
		RumbleGraph* skydive_freefallStickHorizForceGraph;
		char unknown10[0x8];
		RumbleGraph* skydive_freefallLookHorizDragGraph;
		char unknown11[0x8];
		RumbleGraph* skydive_freefallStickHorizDragGraph;
		char unknown12[0x8];
		RumbleGraph* skydive_freefallLookVertDragGraph;
		char unknown13[0x8];
		RumbleGraph* skydive_freefallStickVertDragIncreaseGraph;
		char unknown14[0x8];
		RumbleGraph* skydive_freefallStickVertDragReductionGraph;
		char unknown15[0x10];
		RumbleGraph* skydive_freefallTurnMultiplierGraph;
		char unknown16[0x8];
		RumbleGraph* skydive_freefallTurnMultiplierPitchGraph;
		SndAliasLookup unknownSound8;
		char unknown17[0x28];
		SndAliasLookup unknownSound9;
		char unknown18[0x30];
		CinematicMotionDef* skydive_canopyCinematicMotion;
		char unknown19[0x50];
		RumbleGraph* skydive_canopyLookHorizForceGraph;
		char unknown20[0x8];
		RumbleGraph* skydive_canopyStickHorizForceGraph;
		char unknown21[0x8];
		RumbleGraph* skydive_canopyLookHorizDragGraph;
		char unknown22[0x8];
		RumbleGraph* skydive_canopyStickHorizDragGraph;
		char unknown23[0x8];
		RumbleGraph* skydive_canopyLookVertDragGraph;
		char unknown24[0x8];
		RumbleGraph* skydive_canopyStickVertDragIncreaseGraph;
		char unknown25[0x8];
		RumbleGraph* skydive_canopyStickVertDragReductionGraph;
		char unknown26[0x10];
		RumbleGraph* skydive_canopyTurnMultiplierGraph;
		char unknown27[0x8];
		RumbleGraph* skydive_canopyTurnMultiplierPitchGraph;
		SndAliasLookup unknownSound10;
		char unknown28[0x28];
		SndAliasLookup unknownSound11;
		char unknown29[0x38];
		RumbleGraph* skydive_wmAnimFreefallLeftStickXToRollGraph;
		RumbleGraph* skydive_wmAnimFreefallRightStickXToRollGraph;
		char unknown30[0x8];
		RumbleGraph* skydive_camFreefallPitchToDistanceGraph;
		RumbleGraph* skydive_camFreefallPitchToForwardOffsetGraph;
		char unknown31[0x8];
		RumbleGraph* skydive_camParachutePitchToDistanceGraph;
		RumbleGraph* skydive_camParachutePitchToForwardOffsetGraph;
		RumbleGraph* skydive_camParachutePitchToModelPitchForwardOffsetGraph;
		RumbleGraph* skydive_camParachutePitchToModelRollSideOffsetGraph;
		RumbleGraph* skydive_camParachutePitchToModelRollUpOffsetGraph;
		char unknown32[0x8];
		SndAliasLookup unknownSound12;
		Curve* unknownCurves[7];
		char unknown33[0x20];
		ScriptableExtraDef* unknownExtra;
		SuitSoundTable* unknownSoundTable;
		SndAliasLookup unknownSounds0[6];
		char unknown34[0x18];
		SndAliasLookup unknownSounds1[3];
		char unknown35[0x10];
	}; assert_sizeof(SuitDef, 0x7D8);
	assert_offsetof(SuitDef, animPackage, 0x1B8);
	assert_offsetof(SuitDef, unknownSound5, 0x310);
	assert_offsetof(SuitDef, skydive_freefallLookHorizDragGraph, 0x468);
	assert_offsetof(SuitDef, skydive_canopyStickHorizDragGraph, 0x5C8);
	assert_offsetof(SuitDef, unknownExtra, 0x758);
	assert_offsetof(SuitDef, unknownSounds1, 0x7B0);

	struct LuaFile
	{
		const char* name;
		int len;
		char strippingType;
		const char* buffer;
	}; assert_sizeof(LuaFile, 0x18);

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
		unsigned int maxActiveParticles;
		bool evalVisStatePerParticle;
		bool sortParticlesAtRuntime;
		bool hasOrientation3D;
		bool hasIndex;
	}; assert_sizeof(FxParticleSimAnimationHeader, 0x3C);

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
	}; assert_sizeof(FxParticleSimAnimationParticleData, 0x20);

	struct FxParticleSimAnimationParticleDataOrientation3D
	{
		unsigned short yOrientation;
		unsigned short zOrientation;
		short yOrientationDelta;
		short zOrientationDelta;
	}; assert_sizeof(FxParticleSimAnimationParticleDataOrientation3D, 0x8);

	struct FxParticleSimAnimationParticleDataIndex
	{
		char index;
		char nextIndex;
	}; assert_sizeof(FxParticleSimAnimationParticleDataIndex, 0x2);

	struct FxParticleSimAnimationFrame
	{
		unsigned int particleDataOffset;
		unsigned int numActiveParticles;
	}; assert_sizeof(FxParticleSimAnimationFrame, 0x8);

	struct FxParticleSimAnimation
	{
		const char* name;
		Material* material;
		FxParticleSimAnimationHeader header;
		FxParticleSimAnimationParticleData* particleData;
		FxParticleSimAnimationParticleDataOrientation3D* particleDataOrientation3D;
		FxParticleSimAnimationParticleDataIndex* particleDataIndex;
		FxParticleSimAnimationFrame* frames;
		vec4_t* colorTable;
	}; assert_sizeof(FxParticleSimAnimation, 0x78);

	struct XAssetEntry;

	struct LightState
	{
		const char* name;
		char unknown[0x60];
		const char* strings[3];
	}; assert_sizeof(LightState, 0x80);

	struct LeaguePlaySeasons
	{
		const char* name;
		char unknown[0x20];
	}; assert_sizeof(LeaguePlaySeasons, 0x28);

	struct XBoneSet
	{
		const char* name;
		int boneCount;
		scr_string_t* bones;
	}; assert_sizeof(XBoneSet, 0x18);
	assert_offsetof(XBoneSet, bones, 0x10);

	struct VolumetricCloud
	{
		const char* name;
		GfxImage* images[3];
		char unknown[0x58];
	}; assert_sizeof(VolumetricCloud, 0x78);

	struct VolumetricHeightmap
	{
		const char* name;
		GfxImage* images[5];
		char unknown[0x30];
	}; assert_sizeof(VolumetricHeightmap, 0x60);

	struct NativeScriptPatchEntry
	{
		char unknown[0x8];
		const char* name;
	}; assert_sizeof(NativeScriptPatchEntry, 0x10);

	struct NativeScriptPatch
	{
		const char* name;
		char unknown[0x8];
		ScriptFile* scriptFile;
		int entryCount;
		NativeScriptPatchEntry* entries;
	}; assert_sizeof(NativeScriptPatch, 0x28);
	assert_offsetof(NativeScriptPatch, entries, 0x20);

	struct ScriptDebugData
	{
		const char* name;
		int unknown;
		int stringCount;
		const char** strings;
	}; assert_sizeof(ScriptDebugData, 0x18);

	struct RadiantTelemetryUnknown0
	{
		char unknown[0x70];
	};

	struct RadiantTelemetryUnknown1
	{
		char unknown[0x48];
	};

	struct RadiantTelemetryUnknown2
	{
		char unknown[0x11C];
	};

	struct RadiantTelemetryUnknown3
	{
		char unknown[0x148];
	};

	struct RadiantTelemetryUnknown4
	{
		char unknown[0x1C];
	};

	struct RadiantTelemetryUnknown5
	{
		char unknown[0x34];
	};

	struct RadiantTelemetry
	{
		const char* name;
		int unknown0Count;
		RadiantTelemetryUnknown0* unknown0;
		int unknown1Count;
		RadiantTelemetryUnknown1* unknown1;
		int unknown2Count;
		RadiantTelemetryUnknown2* unknown2;
		int unknown3Count;
		RadiantTelemetryUnknown3* unknown3;
		int unknown4Count;
		RadiantTelemetryUnknown4* unknown4;
		int unknown5Count;
		RadiantTelemetryUnknown5* unknown5;
		int unknown6Count;
		char* unknown6;
		int unknown7Count;
		char* unknown7;
		int unknown8Count;
		char* unknown8;
	}; assert_sizeof(RadiantTelemetry, 0x98);
	assert_offsetof(RadiantTelemetry, unknown5, 0x60);
	assert_offsetof(RadiantTelemetry, unknown8, 0x90);

	struct ScenarioUnknown0
	{
		std::uint64_t soundHash;
		const char* name;
		char unknown[0x18];
	}; assert_sizeof(ScenarioUnknown0, 0x28);

	struct ScenarioUnknown1
	{
		std::uint64_t soundHash0;
		const char* name0;
		const char* name1;
		int hashCount;
		std::uint64_t* hashes;
		std::uint64_t soundHash1;
	}; assert_sizeof(ScenarioUnknown1, 0x30);
	assert_offsetof(ScenarioUnknown1, hashes, 0x20);

	struct ScenarioUnknown2
	{
		std::uint64_t soundHash0;
		std::uint64_t soundHash1;
	}; assert_sizeof(ScenarioUnknown2, 0x10);

	struct Scenario
	{
		const char* name;
		int unknown0Count;
		int unknown1Count;
		int unknown2Count;
		ScenarioUnknown0* unknown0;
		ScenarioUnknown1* unknown1;
		ScenarioUnknown2* unknown2;
	}; assert_sizeof(Scenario, 0x30);
	assert_offsetof(Scenario, unknown0, 0x18);

	struct AddonMapEntsModel
	{
		char unknown[0x5C];
	};

	struct AddonMapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars;
		MapTriggers trigger;
		unsigned int numSubModels;
		cmodel_t* cmodels;
		AddonMapEntsModel* models;
	}; assert_sizeof(AddonMapEnts, 0x80);
	assert_offsetof(AddonMapEnts, trigger, 0x18);
	assert_offsetof(AddonMapEnts, numSubModels, 0x68);

	struct GamePropsData
	{
		char unknown0[0x8];
		ParticleSystemDef* vfx0;
		char unknown1[0x8];
		ParticleSystemDef* vfx1;
		const char* string;
		char unknown2[0x58];
	}; assert_sizeof(GamePropsData, 0x80);

	struct GameProps
	{
		const char* name;
		GamePropsData data;
	}; assert_sizeof(GameProps, 0x88);

	struct SightConfigShape
	{
		char unknown[0x18];
	};

	struct SightConfigPoint
	{
		char unknown[0x14];
	};

	struct SightConfigEntryType1
	{
		SightConfigShape* shape;
		char unknown[0x28];
	}; assert_sizeof(SightConfigEntryType1, 0x30);

	struct SightConfigEntryType2
	{
		SightConfigShape* shape;
		int pointCount;
		SightConfigPoint* points;
		char unknown[0x20];
	}; assert_sizeof(SightConfigEntryType2, 0x38);

	union SightConfigEntryData
	{
		SightConfigEntryType1 type1;
		SightConfigEntryType2 type2;
	};

	struct SightConfigEntry
	{
		char unknown[0x18];
		unsigned char type;
		SightConfigEntryData data;
	}; assert_sizeof(SightConfigEntry, 0x58);
	assert_offsetof(SightConfigEntry, data, 0x20);

	struct SightConfig
	{
		const char* name;
		scr_string_t unknown0;
		int entryCount;
		SightConfigEntry* entries;
		char unknown1[0x10];
	}; assert_sizeof(SightConfig, 0x28);

	struct PhysicsDebugShape
	{
		int vertexCount;
		vec3_t* vertices;
		int indexCount;
		unsigned short* indices;
		int unknownCount;
		unsigned int* unknown;
	}; assert_sizeof(PhysicsDebugShape, 0x30);

	struct PhysicsDebugShapeList
	{
		int count;
		PhysicsDebugShape* shapes;
	}; assert_sizeof(PhysicsDebugShapeList, 0x10);

	struct PhysicsDebugSet
	{
		char unknown[0x30];
		PhysicsDebugShapeList lists[5];
	}; assert_sizeof(PhysicsDebugSet, 0x80);

	struct PhysicsDebugPoints
	{
		int count;
		vec3_t* points;
		unsigned char* unknown0;
		unsigned int* unknown1;
	}; assert_sizeof(PhysicsDebugPoints, 0x20);

	struct PhysicsDebugData
	{
		const char* name;
		PhysicsDebugSet set;
		char unknown0[0x30];
		PhysicsDebugPoints points0;
		char unknown1[0x38];
		PhysicsDebugPoints points1;
		char unknown2[0x8];
	}; assert_sizeof(PhysicsDebugData, 0x138);
	assert_offsetof(PhysicsDebugData, points0, 0xB8);
	assert_offsetof(PhysicsDebugData, points1, 0x110);

	struct BehaviorSequencerValue
	{
		unsigned char type;
		scr_string_t string;
		char unknown[0x8];
	}; assert_sizeof(BehaviorSequencerValue, 0x10);

	struct BehaviorSequencerEntry
	{
		scr_string_t name;
		BehaviorSequencerValue value;
		char unknown[0x4];
	}; assert_sizeof(BehaviorSequencerEntry, 0x18);

	struct BehaviorSequencerString
	{
		scr_string_t name;
		char unknown[0x4];
	}; assert_sizeof(BehaviorSequencerString, 0x8);

	struct BehaviorSequencerKeys
	{
		char unknown[0x4];
		scr_string_t keys[4];
	}; assert_sizeof(BehaviorSequencerKeys, 0x14);

	struct BehaviorSequencerValueList
	{
		scr_string_t name;
		int valueCount;
		BehaviorSequencerValue* values;
	}; assert_sizeof(BehaviorSequencerValueList, 0x10);

	struct BehaviorSequencerNamedValueList
	{
		char unknown0[0x8];
		BehaviorSequencerValue* values;
		int valueCount;
		char unknown1[0x4];
		scr_string_t name;
		char unknown2[0x4];
	}; assert_sizeof(BehaviorSequencerNamedValueList, 0x20);
	assert_offsetof(BehaviorSequencerNamedValueList, name, 0x18);

	struct BehaviorSequencerNamedString
	{
		scr_string_t name;
		char unknown[0xC];
	}; assert_sizeof(BehaviorSequencerNamedString, 0x10);

	struct BehaviorSequencerActionType2
	{
		char unknown[0x8];
		BehaviorSequencerValueList lists[8];
	}; assert_sizeof(BehaviorSequencerActionType2, 0x88);

	struct BehaviorSequencerActionType3
	{
		scr_string_t name;
		char unknown[0x10];
	}; assert_sizeof(BehaviorSequencerActionType3, 0x14);

	struct BehaviorSequencerActionType4
	{
		BehaviorSequencerNamedString strings[2];
		char unknown[0xC];
		scr_string_t name0;
		scr_string_t name1;
	}; assert_sizeof(BehaviorSequencerActionType4, 0x34);

	struct BehaviorSequencerActionType5
	{
		char unknown[0x4];
		scr_string_t name0;
		scr_string_t name1;
	}; assert_sizeof(BehaviorSequencerActionType5, 0xC);

	struct BehaviorSequencerActionType6
	{
		BehaviorSequencerNamedValueList lists[2];
	}; assert_sizeof(BehaviorSequencerActionType6, 0x40);

	union BehaviorSequencerActionData
	{
		BehaviorSequencerActionType2 type2;
		BehaviorSequencerActionType3 type3;
		BehaviorSequencerActionType4 type4;
		BehaviorSequencerActionType5 type5;
		BehaviorSequencerActionType6 type6;
	};

	struct BehaviorSequencerAction
	{
		BehaviorSequencerActionData data;
		unsigned char type;
	}; assert_sizeof(BehaviorSequencerAction, 0x90);

	struct BehaviorSequencerState
	{
		char unknown0[0x4];
		BehaviorSequencerKeys keys;
		char unknown1[0x30];
		BehaviorSequencerAction action;
		char unknown2[0x8];
		scr_string_t name;
		char unknown3[0x4];
	}; assert_sizeof(BehaviorSequencerState, 0xE8);
	assert_offsetof(BehaviorSequencerState, action, 0x48);
	assert_offsetof(BehaviorSequencerState, name, 0xE0);

	struct BehaviorSequencerUnknown3
	{
		char unknown[0x3C];
	};

	struct BehaviorSequencerEvent
	{
		BehaviorSequencerNamedString name;
		unsigned char type;
	}; assert_sizeof(BehaviorSequencerEvent, 0x14);

	struct BehaviorSequencer
	{
		const char* name;
		scr_string_t unknown0;
		BehaviorSequencerEntry* entries;
		int entryCount;
		int stringCount;
		BehaviorSequencerString* strings;
		int stateCount;
		BehaviorSequencerState* states;
		int unknown3Count;
		BehaviorSequencerUnknown3* unknown3;
		int vectorCount;
		vec3_t* vectors;
		int eventCount;
		BehaviorSequencerEvent* events;
		scr_string_t unknown1;
		scr_string_t unknown2;
		char unknown4[0x10];
	}; assert_sizeof(BehaviorSequencer, 0x80);
	assert_offsetof(BehaviorSequencer, entries, 0x10);
	assert_offsetof(BehaviorSequencer, states, 0x30);
	assert_offsetof(BehaviorSequencer, events, 0x60);
	assert_offsetof(BehaviorSequencer, unknown1, 0x68);

	struct PlayerAnimEntry
	{
		scr_string_t names[3];
		char unknown[0x5C];
	}; assert_sizeof(PlayerAnimEntry, 0x68);

	struct PlayerAnimIndexSet
	{
		int unknown;
		int count0;
		int count1;
		unsigned int* indices0;
		unsigned int* indices1;
	}; assert_sizeof(PlayerAnimIndexSet, 0x20);

	struct PlayerAnimUnknown1Data
	{
		char unknown[0x24];
	};

	struct PlayerAnimUnknown1Pair
	{
		char unknown[0x6];
	};

	struct PlayerAnimUnknown1
	{
		int dataCount;
		int pairCount;
		PlayerAnimUnknown1Data* data;
		PlayerAnimUnknown1Pair* pairs;
	}; assert_sizeof(PlayerAnimUnknown1, 0x18);

	struct PlayerAnimUnknown4
	{
		int count;
		unsigned short* values;
	}; assert_sizeof(PlayerAnimUnknown4, 0x10);

	struct PlayerAnimUnknown5
	{
		char unknown[0x10];
	};

	struct PlayerAnim
	{
		const char* name;
		int entryCount;
		int unknown1Count;
		int vectorCount;
		int unknown3Count;
		int unknown4Count;
		int unknown5Count;
		int unknown6Count;
		int unknown7Count;
		int unknown8Count;
		PlayerAnimEntry* entries;
		PlayerAnimIndexSet* indexSets0;
		PlayerAnimIndexSet* indexSets1;
		PlayerAnimUnknown1* unknown1;
		vec3_t* vectors;
		unsigned int* unknown3;
		PlayerAnimUnknown4* unknown4;
		PlayerAnimUnknown5* unknown5;
		unsigned int* unknown6;
		unsigned short* entryValues0;
		unsigned short* unknown7;
		unsigned short* entryValues1;
		unsigned short* unknown8;
		int animCount;
		XAnimParts** anims;
	}; assert_sizeof(PlayerAnim, 0xA8);
	assert_offsetof(PlayerAnim, entries, 0x30);
	assert_offsetof(PlayerAnim, unknown8, 0x90);
	assert_offsetof(PlayerAnim, anims, 0xA0);

	union XAssetHeader
	{
		void* data;
		RawFile* rawfile;
		PhysicsLibrary* physicsLibrary;
		ComputeShader* computeShader;
		FxParticleSimAnimation* particleSimAnimation;
		LuaFile* luaFile;
		SuitDef* suit;
		DDLFile* ddlFile;
		VirtualLeaderboardDef* virtualLeaderboardDef;
		ExecutionDef* execution;
		XCam* xcam;
		BulletPenetration* bulletPenetration;
		LeaderboardDef* leaderboardDef;
		SndGlobals* soundGlobals;
		SuitAnimPackage* suitAnimPackage;
		LocDmgTable* locDmgTable;
		MaterialLibShader* libShader;
		LaserDef* laserDef;
		CinematicMotionDef* cinematicMotion;
		AccessoryDef* accessory;
		ASM* asmAsset;
		Gesture* gesture;
		LocalizeEntry* localize;
		WeaponAttachment* attachment;
		TracerDef* tracerDef;
		EquipmentSoundTable* equipSndTable;
		TTFDef* ttfDef;
		BehaviorTree* behaviorTree;
		Animset* animset;
		ReticleDef* reticleDef;
		CoverSelector* coverSelector;
		EnemySelector* enemySelector;
		ClientCharacter* clientCharacter;
		ClothAsset* clothAsset;
		Camo* camo;
		StreamTreeOverride* streamTreeOverride;
		CollisionTile* collisionTile;
		DLogSchema* dlogSchema;
		ScriptFile* scriptfile;
		PhysicsSFXEventAsset* physicsSFXEventAsset;
		PhysicsVFXEventAsset* physicsVFXEventAsset;
		PhysicsAsset* physicsAsset;
		PhysicsFXShape* physicsFXShape;
		PhysicsFXPipeline* physicsFXPipeline;
		XAnimParts* parts;
		XModelSurfs* modelSurfs;
		GfxImage* image;
		ComWorld* comWorld;
		GlassWorld* glassWorld;
		PathData* pathData;
		NavMeshData* navMesh;
		TacticalGraphData* tacGraph;
		clipMap_t* clipMap;
		MapEnts* mapEnts;
		FxWorld* fxWorld;
		GfxWorld* gfxWorld;
		GfxWorldTransientZone* gfxWorldTransientZone;
		DynEntityList* dynEntityList;
		FxWorldTransientZone* fxWorldTransientZone;
		MapEdgeList* mapEdgeList;
		GfxLightDef* lightDef;
		GfxGradingClut* gradingClut;
		GfxFogSpline* fogSpline;
		StringTable* stringTable;
		CameraDef* cameraDef;
		HudOutlineDef* hudOutlineDef;
		XAnimProceduralBones* proceduralBones;
		XAnimDynamicBones* dynamicBones;
		XAnimCurve* animCurve;
		WeaponAnimPackage* weaponAnimPackage;
		BlendSpace2DDef* blendSpace2D;
		XModelDetailCollision* detailCollision;
		StreamKey* streamKey;
		SndBankResident* soundBank;
		SndBankTransient* soundBankTransient;
		WeaponSFXPackage* sfxPackage;
		GfxDecalVolumeMaterial* decalVolumeMaterial;
		GfxDecalVolumeMask* decalVolumeMask;
		Material* material;
		MaterialTechniqueSet* techniqueSet;
		MaterialVertexShader* vertexShader;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		MaterialPixelShader* pixelShader;
		StTerrain* stTerrain;
		XModel* model;
		ScriptableDef* scriptable;
		ParticleSystemDef* vfx;
		WeaponCompleteDef* weapon;
		WeaponVFXPackage* vfxPackage;
		VectorField* vectorField;
		RumbleInfo* rumble;
		RumbleGraph* rumbleGraph;
		FootstepVFX* footstepVfx;
		StreamingInfo* streamingInfo;
		VehicleDef* vehicle;
		FxImpactTable* impactFx;
		SurfaceFxTable* surfaceFx;
		XCompositeModelDef* compositeModel;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct ScriptStringList
	{
		int count;
		bool loaded;
		const char** strings;
	}; assert_sizeof(ScriptStringList, 0x10);

	enum script_string_cipher_type : std::uint8_t
	{
		SCRIPT_STRING_CIPHER_ADD,
		SCRIPT_STRING_CIPHER_SUB,
		SCRIPT_STRING_CIPHER_MUL,
		SCRIPT_STRING_CIPHER_ROTATE,
	};

	struct script_string_cipher
	{
		script_string_cipher_type type;
		std::uint8_t start;
		std::uint32_t key;
		const char* rotations;
	};

	struct script_string_cipher_set
	{
		std::uint8_t id;
		script_string_cipher ciphers[64];
	};

	struct script_string_splitmix_cipher
	{
		std::uint8_t id;
		std::uint64_t seed;
	};

	struct XAssetList
	{
		ScriptStringList stringList;
		unsigned int assetCount;
		unsigned int assetReadPos;
		XAsset* assets;
	}; assert_sizeof(XAssetList, 0x20);
}
