#pragma once
#include <d3d11.h>

namespace zonetool::iw7
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	enum scr_string_t : std::int32_t
	{
	};

	enum XAssetType : std::int32_t
	{
		ASSET_TYPE_PHYSICSLIBRARY = 0,
		ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET = 1,
		ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET = 2,
		ASSET_TYPE_PHYSICSASSET = 3,
		ASSET_TYPE_PHYSICS_FX_PIPELINE = 4,
		ASSET_TYPE_PHYSICS_FX_SHAPE = 5,
		ASSET_TYPE_XANIMPARTS = 6,
		ASSET_TYPE_XMODEL_SURFS = 7,
		ASSET_TYPE_XMODEL = 8,
		ASSET_TYPE_MAYHEM = 9,
		ASSET_TYPE_MATERIAL = 10,
		ASSET_TYPE_COMPUTESHADER = 11,
		ASSET_TYPE_VERTEXSHADER = 12,
		ASSET_TYPE_HULLSHADER = 13,
		ASSET_TYPE_DOMAINSHADER = 14,
		ASSET_TYPE_PIXELSHADER = 15,
		ASSET_TYPE_VERTEXDECL = 16,
		ASSET_TYPE_TECHNIQUE_SET = 17,
		ASSET_TYPE_IMAGE = 18,
		ASSET_TYPE_SOUND_GLOBALS = 19,
		ASSET_TYPE_SOUND_BANK = 20,
		ASSET_TYPE_SOUND_BANK_TRANSIENT = 21,
		ASSET_TYPE_CLIPMAP = 22,
		ASSET_TYPE_COMWORLD = 23,
		ASSET_TYPE_GLASSWORLD = 24,
		ASSET_TYPE_PATHDATA = 25,
		ASSET_TYPE_NAVMESH = 26,
		ASSET_TYPE_MAP_ENTS = 27,
		ASSET_TYPE_FXWORLD = 28,
		ASSET_TYPE_GFXWORLD = 29,
		ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE = 30,
		ASSET_TYPE_IESPROFILE = 31, // unused
		ASSET_TYPE_LIGHT_DEF = 32,
		ASSET_TYPE_UI_MAP = 33, // unused
		ASSET_TYPE_ANIMCLASS = 34,
		ASSET_TYPE_PLAYERANIM = 35,
		ASSET_TYPE_GESTURE = 36,
		ASSET_TYPE_LOCALIZE_ENTRY = 37,
		ASSET_TYPE_ATTACHMENT = 38,
		ASSET_TYPE_WEAPON = 39,
		ASSET_TYPE_VFX = 40,
		ASSET_TYPE_FX = 41,
		ASSET_TYPE_IMPACT_FX = 42,
		ASSET_TYPE_SURFACE_FX = 43,
		ASSET_TYPE_AITYPE = 44, // unused
		ASSET_TYPE_MPTYPE = 45, // unused
		ASSET_TYPE_CHARACTER = 46, // unused
		ASSET_TYPE_XMODELALIAS = 47, // unused
		ASSET_TYPE_RAWFILE = 48,
		ASSET_TYPE_SCRIPTFILE = 49,
		ASSET_TYPE_STRINGTABLE = 50,
		ASSET_TYPE_LEADERBOARD = 51,
		ASSET_TYPE_VIRTUAL_LEADERBOARD = 52,
		ASSET_TYPE_STRUCTUREDDATADEF = 53,
		ASSET_TYPE_DDL = 54,
		ASSET_TYPE_TRACER = 55,
		ASSET_TYPE_VEHICLE = 56,
		ASSET_TYPE_ADDON_MAP_ENTS = 57,
		ASSET_TYPE_NET_CONST_STRINGS = 58,
		ASSET_TYPE_LUA_FILE = 59,
		ASSET_TYPE_SCRIPTABLE = 60,
		ASSET_TYPE_EQUIPMENT_SND_TABLE = 61,
		ASSET_TYPE_VECTORFIELD = 62,
		ASSET_TYPE_PARTICLE_SIM_ANIMATION = 63,
		ASSET_TYPE_STREAMING_INFO = 64,
		ASSET_TYPE_LASER = 65,
		ASSET_TYPE_TTF = 66,
		ASSET_TYPE_SUIT = 67,
		ASSET_TYPE_SUITANIMPACKAGE = 68,
		ASSET_TYPE_SPACESHIPTARGET = 69,
		ASSET_TYPE_RUMBLE = 70,
		ASSET_TYPE_RUMBLE_GRAPH = 71,
		ASSET_TYPE_ANIM_PACKAGE = 72,
		ASSET_TYPE_SFX_PACKAGE = 73,
		ASSET_TYPE_VFX_PACKAGE = 74,
		ASSET_TYPE_BEHAVIOR_TREE = 75,
		ASSET_TYPE_XANIM_ARCHETYPE = 76,
		ASSET_TYPE_XANIM_PROCEDURALBONES = 77,
		ASSET_TYPE_RETICLE = 78,
		ASSET_TYPE_GFXLIGHTMAP = 79,
		ASSET_TYPE_COUNT = 80
	};

	struct PhysicsLibrary;
	struct PhysicsSFXEventAsset;
	struct PhysicsVFXEventAsset;
	struct PhysicsAsset;
	struct PhysicsFXPipeline;
	struct PhysicsFXShape;
	struct XAnimParts;
	struct XModelSurfs;
	struct XModel;
	struct Mayhem;
	struct Material;
	struct ComputeShader;
	struct MaterialVertexShader;
	struct MaterialHullShader;
	struct MaterialDomainShader;
	struct MaterialPixelShader;
	struct MaterialVertexDeclaration;
	struct MaterialTechniqueSet;
	struct GfxImage;
	struct SndGlobals;
	struct SndBank;
	struct SndBankTransient;
	struct clipMap_t;
	struct ComWorld;
	struct GlassWorld;
	struct PathData;
	struct NavMeshData;
	struct MapEnts;
	struct FxWorld;
	struct GfxWorld;
	struct GfxWorldTransientZone;
	//ASSET_TYPE_IESPROFILE = 31, // unused
	struct GfxLightDef;
	//ASSET_TYPE_UI_MAP = 33, // unused
	struct AnimationClass;
	struct PlayerAnimScript;
	struct Gesture;
	struct LocalizeEntry;
	struct WeaponAttachment;
	struct WeaponCompleteDef;
	struct ParticleSystemDef;
	struct FxEffectDef;
	struct FxImpactTable;
	struct SurfaceFxTable;
	//ASSET_TYPE_AITYPE = 44, // unused
	//ASSET_TYPE_MPTYPE = 45, // unused
	//ASSET_TYPE_CHARACTER = 46, // unused
	//ASSET_TYPE_XMODELALIAS = 47, // unused
	struct RawFile;
	struct ScriptFile;
	struct StringTable;
	struct LeaderBoardDef;
	struct VirtualLeaderboardDef;
	struct StructuredDataDefSet;
	struct DDLFile;
	struct TracerDef;
	struct VehicleDef;
	struct AddonMapEnts;
	struct NetConstStrings;
	struct LuaFile;
	struct ScriptableDef;
	struct EquipmentSoundTable;
	struct VectorField;
	struct FxParticleSimAnimation;
	struct StreamingInfo;
	struct LaserDef;
	struct TTFDef;
	struct SuitDef;
	struct SuitAnimPackage;
	struct SpaceshipTargetDef;
	struct RumbleInfo;
	struct RumbleGraph;
	struct WeaponAnimPackage;
	struct WeaponSFXPackage;
	struct WeaponVFXPackage;
	struct BehaviorTree;
	struct XAnimArcheType;
	struct XAnimProceduralBones;
	struct ReticleDef;
	struct GfxLightMap;

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};

	struct ExtentBounds
	{
		vec3_t mins;
		vec3_t maxs;
	};

	union FxCombinedUnion
	{
		FxEffectDef* fx;
		ParticleSystemDef* particleSystemDef;
	};

	enum FxCombinedType : std::uint8_t
	{
		FX_COMBINED_DEFAULT,
		FX_COMBINED_PARTICLE_SYSTEM,
	};

	struct FxCombinedDef
	{
		FxCombinedUnion u;
		FxCombinedType type;
	};

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

	struct PhysicsSFXEventAssetRule;
	struct PhysicsSFXEventAssetRules
	{
		int numrules;
		PhysicsSFXEventAssetRule* rules;
	};

	enum PhysicsSFXEventAssetRuleType
	{
		Types_PhysicsSFXEventRule = 0x0,
		Types_PhysicsSFXEventSoundRule = 0x1,
		Types_PhysicsSFXEventMaterialRule = 0x2,
		Types_PhysicsSFXEventAngleRule = 0x3,
		Types_PhysicsSFXEventSpeedRule = 0x4,
		Types_PhysicsSFXEventMassRule = 0x5,
		Types_PhysicsSFXEventGravityRule = 0x6,
		Types_PhysicsSFXEventAtmosphereRule = 0x7,
		Types_PhysicsSFXEventCameraDistanceRule = 0x8,
		Types_PhysicsSFXEventFrequencyRule = 0x9,
		Types_PhysicsSFXEventRuleCount = 0xA,
	};

	struct PhysicsSFXEventAssetSoundRule
	{
		char* hitSoundAlias;
		char* scrapeSoundAlias;
		float scrapeSoundVolumeSpeedThresholdMin;
		float scrapeSoundVolumeSpeedThresholdMax;
		float scrapeSoundVolumeBlendSpeed;
	};

	struct PhysicsSFXEventAssetMaterialEntry
	{
		int surfaceType;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetMaterialRule
	{
		int numentries;
		PhysicsSFXEventAssetMaterialEntry* entries;
	};

	struct PhysicsSFXEventAssetAngleEntry
	{
		float angle;
		float hitSoundVolumeModifierMin;
		float hitSoundVolumeModifierMax;
		float hitSoundPitchModifierMin;
		float hitSoundPitchModifierMax;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetAngleRule
	{
		int numentries;
		PhysicsSFXEventAssetAngleEntry* entries;
	};

	struct PhysicsSFXEventAssetSpeedEntry
	{
		float speedThreshold;
		float hitSoundVolumeModifierMin;
		float hitSoundVolumeModifierMax;
		float hitSoundPitchModifierMin;
		float hitSoundPitchModifierMax;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetSpeedRule
	{
		int numentries;
		PhysicsSFXEventAssetSpeedEntry* entries;
	};

	struct PhysicsSFXEventAssetMassEntry
	{
		float massThreshold;
		float hitSoundVolumeModifierMin;
		float hitSoundVolumeModifierMax;
		float hitSoundPitchModifierMin;
		float hitSoundPitchModifierMax;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetMassRule
	{
		int numentries;
		PhysicsSFXEventAssetMassEntry* entries;
	};

	struct PhysicsSFXEventAssetGravityEntry
	{
		bool gravity;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetGravityRule
	{
		int numentries;
		PhysicsSFXEventAssetGravityEntry* entries;
	};

	struct PhysicsSFXEventAssetAtmosphereEntry
	{
		bool atmosphere;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetAtmosphereRule
	{
		int numentries;
		PhysicsSFXEventAssetAtmosphereEntry* entries;
	};

	struct PhysicsSFXEventAssetCameraDistanceEntry
	{
		float distance;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetCameraDistanceRule
	{
		int numentries;
		PhysicsSFXEventAssetCameraDistanceEntry* entries;
	};

	struct PhysicsSFXEventAssetFrequencyEntry
	{
		int maxEvents;
		PhysicsSFXEventAssetRules rules;
	};

	struct PhysicsSFXEventAssetFrequencyRule
	{
		int numentries;
		PhysicsSFXEventAssetFrequencyEntry* entries;
	};

	union PhysicsSFXEventAssetRuleUnion
	{
		PhysicsSFXEventAssetSoundRule soundRule;
		PhysicsSFXEventAssetMaterialRule materialRule;
		PhysicsSFXEventAssetAngleRule angleRule;
		PhysicsSFXEventAssetSpeedRule speedRule;
		PhysicsSFXEventAssetMassRule massRule;
		PhysicsSFXEventAssetGravityRule gravityRule;
		PhysicsSFXEventAssetAtmosphereRule atmosphereRule;
		PhysicsSFXEventAssetCameraDistanceRule cameraDistanceRule;
		PhysicsSFXEventAssetFrequencyRule frequencyRule;
	};

	struct PhysicsSFXEventAssetRule
	{
		PhysicsSFXEventAssetRuleType type;
		PhysicsSFXEventAssetRuleUnion u;
	};

	struct PhysicsSFXEventAsset
	{
		const char* name;
		int priority;
		PhysicsSFXEventAssetRules rules;
	}; assert_sizeof(PhysicsSFXEventAsset, 0x20);

	struct PhysicsVFXEventAssetRule;
	struct PhysicsVFXEventAssetRules
	{
		int numrules;
		PhysicsVFXEventAssetRule* rules;
	};

	enum PhysicsVFXEventAssetRuleType
	{
		Types_PhysicsVFXEventRule = 0x0,
		Types_PhysicsVFXEventParticleEffectRule = 0x1,
		Types_PhysicsVFXEventMaterialRule = 0x2,
		Types_PhysicsVFXEventAngleRule = 0x3,
		Types_PhysicsVFXEventSpeedRule = 0x4,
		Types_PhysicsVFXEventMassRule = 0x5,
		Types_PhysicsVFXEventGravityRule = 0x6,
		Types_PhysicsVFXEventAtmosphereRule = 0x7,
		Types_PhysicsVFXEventCameraDistanceRule = 0x8,
		Types_PhysicsVFXEventFrequencyRule = 0x9,
		Types_PhysicsVFXEventRuleCount = 0xA,
	};

	struct PhysicsVFXEventAssetParticleEffectRule
	{
		char* hitParticleEffectAlias;
		char* scrapeParticleEffectAlias;
	};

	struct PhysicsVFXEventAssetMaterialEntry
	{
		int surfaceType;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetMaterialRule
	{
		int numentries;
		PhysicsVFXEventAssetMaterialEntry* entries;
	};

	struct PhysicsVFXEventAssetAngleEntry
	{
		float angle;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetAngleRule
	{
		int numentries;
		PhysicsVFXEventAssetAngleEntry* entries;
	};

	struct PhysicsVFXEventAssetSpeedEntry
	{
		float speedThreshold;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetSpeedRule
	{
		int numentries;
		PhysicsVFXEventAssetSpeedEntry* entries;
	};

	struct PhysicsVFXEventAssetMassEntry
	{
		float massThreshold;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetMassRule
	{
		int numentries;
		PhysicsVFXEventAssetMassEntry* entries;
	};

	struct PhysicsVFXEventAssetGravityEntry
	{
		bool gravity;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetGravityRule
	{
		int numentries;
		PhysicsVFXEventAssetGravityEntry* entries;
	};

	struct PhysicsVFXEventAssetAtmosphereEntry
	{
		bool atmosphere;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetAtmosphereRule
	{
		int numentries;
		PhysicsVFXEventAssetAtmosphereEntry* entries;
	};

	struct PhysicsVFXEventAssetCameraDistanceEntry
	{
		float distance;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetCameraDistanceRule
	{
		int numentries;
		PhysicsVFXEventAssetCameraDistanceEntry* entries;
	};

	struct PhysicsVFXEventAssetFrequencyEntry
	{
		int maxEvents;
		PhysicsVFXEventAssetRules rules;
	};

	struct PhysicsVFXEventAssetFrequencyRule
	{
		int numentries;
		PhysicsVFXEventAssetFrequencyEntry* entries;
	};

	union PhysicsVFXEventAssetRuleUnion
	{
		PhysicsVFXEventAssetParticleEffectRule particleEffectRule;
		PhysicsVFXEventAssetMaterialRule materialRule;
		PhysicsVFXEventAssetAngleRule angleRule;
		PhysicsVFXEventAssetSpeedRule speedRule;
		PhysicsVFXEventAssetMassRule massRule;
		PhysicsVFXEventAssetGravityRule gravityRule;
		PhysicsVFXEventAssetAtmosphereRule atmosphereRule;
		PhysicsVFXEventAssetCameraDistanceRule cameraDistanceRule;
		PhysicsVFXEventAssetFrequencyRule frequencyRule;
	};

	struct PhysicsVFXEventAssetRule
	{
		PhysicsVFXEventAssetRuleType type;
		PhysicsVFXEventAssetRuleUnion u;
	};

	struct PhysicsVFXEventAsset
	{
		const char* name;
		int priority;
		PhysicsVFXEventAssetRules rules;
	}; assert_sizeof(PhysicsVFXEventAsset, 0x20);

	struct PhysicsAsset
	{
		const char* name;
		unsigned int havokDataSize;
		char* havokData;
		char __pad0[8];
		int numConstraints;
		int numSFXEventAssets;
		PhysicsSFXEventAsset** sfxEventAssets;
		int numVFXEventAssets;
		PhysicsVFXEventAsset** vfxEventAssets;
		char __pad1[16];
	}; assert_sizeof(PhysicsAsset, 0x50);

	struct PhysicsFXPipeline
	{
		const char* name;
		char __pad0[24];
	}; assert_sizeof(PhysicsFXPipeline, 0x20);

	struct PhysicsFXShape
	{
		const char* name;
		unsigned int size;
		vec3_t* vecs;
	}; assert_sizeof(PhysicsFXShape, 0x18);

	union XAnimIndices
	{
		char* _1;
		unsigned short* _2;
		void* data;
	};

	struct XAnimNotifyInfo
	{
		scr_string_t name;
		float time;
	};

	union XAnimDynamicFrames
	{
		char(*_1)[3];
		unsigned short(*_2)[3];
	};

	union XAnimDynamicIndices
	{
		char _1[1];
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
		char smallTrans;
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

	struct XAnimParts
	{
		const char* name; // 0
		scr_string_t* names; // 8
		char* dataByte; // 16
		short* dataShort; // 24
		int* dataInt; // 32
		short* randomDataShort; // 40
		char* randomDataByte; // 48
		int* randomDataInt; // 56
		XAnimIndices indices; // 64
		XAnimNotifyInfo* notify; // 72
		XAnimDeltaPart* deltaPart; // 80
		unsigned int randomDataShortCount; // 88
		unsigned int randomDataByteCount; // 92
		unsigned int indexCount; // 96
		float framerate; // 100
		float frequency; // 104
		unsigned short dataByteCount; // 108
		unsigned short dataShortCount; // 110
		unsigned short dataIntCount; // 112
		unsigned short randomDataIntCount; // 114
		unsigned short numframes; // 116
		char flags; // 118
		char boneCount[10]; // 119
		char notifyCount; // 129
		char assetType; // 130
		char ikType; // 131
	}; assert_sizeof(XAnimParts, 0x88);

	struct XSurfaceCollisionAabb
	{
		unsigned short mins[3];
		unsigned short maxs[3];
	};

	struct XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		unsigned short childBeginIndex;
		unsigned short childCount;
	};

	struct XSurfaceCollisionLeaf
	{
		unsigned short triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		XSurfaceCollisionNode* nodes;
		unsigned int leafCount;
		XSurfaceCollisionLeaf* leafs;
	};

	struct XRigidVertList
	{
		unsigned short boneOffset;
		unsigned short vertCount;
		unsigned short triOffset;
		unsigned short triCount;
		XSurfaceCollisionTree* collisionTree;
	};

	union PackedUnitVec
	{
		unsigned int packed;
	};

	union PackedTexCoords
	{
		unsigned int packed;
	};

	union GfxColor
	{
		unsigned char array[4];
		unsigned int packed;
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

	struct GfxPackedMotionVertex
	{
		float xyz[3];
		float binormalSignAndHeight;
		GfxColor pieceIndex;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	union GfxVertexUnion0
	{
		GfxPackedVertex* packedVerts0;
		GfxPackedMotionVertex* packedMotionVerts0;
		void* verts0;
	};

	struct Face
	{
		unsigned short v1;
		unsigned short v2;
		unsigned short v3;
	};

	typedef unsigned int XBlendInfo;

	struct XSubdivRigidVertList
	{
		unsigned int firstFace;
		unsigned int faceCount;
		unsigned int firstRegularPatch;
		unsigned int regularPatchCount;
	};

	struct XSurfaceSubdivLevel
	{
		XSubdivRigidVertList* rigidVertLists;
		unsigned int faceCount;
		unsigned int regularPatchCount;
		unsigned int regularPatchOffset;
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
		unsigned int transitionPointCount;
		unsigned int vertCount;
		unsigned int vertOffset;
		unsigned short* faceIndices;
		unsigned short* regularPatchIndices;
		unsigned int* regularPatchFlags;
		unsigned int* facePoints;
		unsigned int* edgePoints;
		unsigned int* vertexPoints;
		unsigned int* normals;
		unsigned int* transitionPoints;
		float* regularPatchCones;
		ID3D11Buffer* regularPatchIndexBuffer;
		ID3D11Buffer* faceIndexBuffer;
		ID3D11ShaderResourceView* regularPatchIndexBufferView;
		ID3D11ShaderResourceView* regularPatchFlagsView;
		ID3D11ShaderResourceView* facePointsView;
		ID3D11ShaderResourceView* edgePointsView;
		ID3D11ShaderResourceView* vertexPointsView;
		ID3D11ShaderResourceView* normalsView;
		ID3D11ShaderResourceView* transitionPointsView;
		ID3D11ShaderResourceView* regularPatchConesView;
	}; assert_sizeof(XSurfaceSubdivLevel, 0xE8);

	struct GfxSubdivCache
	{
		unsigned int size;
		ID3D11Buffer* subdivCacheBuffer;
		ID3D11ShaderResourceView* subdivCacheView;
	}; assert_sizeof(GfxSubdivCache, 0x18);

	struct XSurfaceSubdivInfo
	{
		XSurfaceSubdivLevel* levels;
		char __pad0[24];
		GfxSubdivCache cache;
	}; assert_sizeof(XSurfaceSubdivInfo, 0x38);

	struct TensionData
	{
		char __pad0[32];
	};

	struct SHProbeSimplexData0
	{
		char __pad0[32];
	};

	struct SHProbeSimplexData1
	{
		char __pad0[8];
	};

	struct SHProbeSimplexData2
	{
		char __pad0[24];
	};

	union SHProbeSimplexDataUnion
	{
		SHProbeSimplexData0 data0;
		SHProbeSimplexData1 data1;
		SHProbeSimplexData2 data2;
	};

	struct XSurface
	{
		unsigned short flags; // 0
		unsigned short vertCount; // 2
		unsigned short triCount; // 4
		char rigidVertListCount; // 6
		char subdivLevelCount; // 7
		short blendVertCounts[8]; // 8
		int unkBlendCount; // 24
		char __pad0[4]; // 28
		GfxVertexUnion0 verts0; // 32
		Face* triIndices; // 40
		ID3D11Buffer* vb0; // 48
		ID3D11ShaderResourceView* vb0View; // 56
		ID3D11Buffer* indexBuffer; // 64
		XRigidVertList* rigidVertLists; // 72
		char* unkBlend; // 80
		ID3D11Buffer* blendVertsBuffer; // 88
		ID3D11ShaderResourceView* blendVertsView; // 96
		__int64* lmapUnwrap; // 104
		ID3D11Buffer* vblmapBuffer; // 112
		ID3D11ShaderResourceView* vblmapView; // 120
		XSurfaceSubdivInfo* subdiv; // 128
		XBlendInfo* blendVerts; // 136
		TensionData* tensionData; // 144
		void* tensionAccumTable; // 152
		ID3D11Buffer* tensionAccumTableBuffer; // 160
		ID3D11ShaderResourceView* tensionAccumTableView; // 168
		ID3D11Buffer* tensionDataBuffer; // 176
		ID3D11ShaderResourceView* tensionDataView; // 184
		ID3D11ShaderResourceView* indexBufferView; // 192
		SHProbeSimplexDataUnion* shProbeSimplexVertData; // 200
		ID3D11Buffer* shProbeSimplexVertBuffer; // 208
		ID3D11ShaderResourceView* shProbeSimplexVertBufferView; // 216
		int partBits[8]; // 224
	}; assert_sizeof(XSurface, 0x100);

	struct XModelSurfs
	{
		const char* name;
		XSurface* surfs;
		unsigned short numsurfs;
		int partBits[8];
	}; assert_sizeof(XModelSurfs, 0x38);

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

	struct ReactiveMotionModelPart
	{
		float center[3];
		float stiffness;
	};

	struct ReactiveMotionModelTweaks
	{
		float scale[4];
	};

	struct XModelLodInfo
	{
		float dist;
		unsigned short numsurfs;
		unsigned short surfIndex;
		XModelSurfs* modelSurfs;
		int partBits[8];
		XSurface* surfs;
		int unk; // subdivLodValidMask?
		char flags;
		char pad[3];
	}; assert_sizeof(XModelLodInfo, 0x40);

	struct XModelCollSurf_s
	{
		Bounds bounds;
		int boneIdx;
		int contents;
		int surfFlags;
	}; assert_sizeof(XModelCollSurf_s, 36);

	struct XBoneInfo
	{
		Bounds bounds;
		union
		{
			float radiusSquared;
			unsigned int radiusSquaredAsInt;
		};
	};

	struct unk_1453E14D8
	{
		char __pad0[24];
	};

	enum XModelCharCollBoundsType : std::uint8_t
	{
		CharCollBoundsType_None = 0x0,
		CharCollBoundsType_Human = 0x1,
		CharCollBoundsType_C12 = 0x2,
		CharCollBoundsType_C8 = 0x3,
		CharCollBoundsType_Seeker = 0x4,
		CharCollBoundsType_Zombie = 0x5,
		CharCollBoundsType_LargeZombie = 0x6,
		CharCollBoundsType_Alien = 0x7,
		CharCollBoundsType_LargeAlien = 0x8,
		CharCollBoundsType_ExtraLargeZombie = 0x9,
		CharCollBoundsType_Num = 0xA,
	};

	struct XModel
	{
		const char* name;
		char unused01;
		unsigned char unknown01;
		char unused02;
		unsigned char numLods;
		unsigned char collLod;
		unsigned char shadowCutoffLod;
		unsigned char characterCollBoundsType;
		int flags;
		unsigned char numBones;
		unsigned char numRootBones;
		unsigned char numReactiveMotionParts;
		unsigned short numClientBones;
		unsigned short numsurfs;
		float scale;
		unsigned short numCollSurfs;
		int contents;
		float radius;
		Bounds bounds;
		int memUsage;
		unsigned int unknown02Count;
		char unk_01[12]; // unknown data
		ScriptableDef* scriptableMoverDef;
		XAnimProceduralBones* proceduralBones;
		scr_string_t* aimAssistBones;
		unsigned char numAimAssistBones;
		unsigned int noScalePartBits[8];
		scr_string_t* boneNames;
		unsigned char* parentList;
		XModelAngle* tagAngles;
		XModelTagPos* tagPositions;
		unsigned char* partClassification;
		DObjAnimMat* baseMat;
		ReactiveMotionModelPart* reactiveMotionParts;
		ReactiveMotionModelTweaks* reactiveMotionTweaks;
		Material** materialHandles;
		XModelLodInfo lodInfo[6];
		XModelCollSurf_s* collSurfs;
		XBoneInfo* boneInfo;
		unsigned short* invHighMipRadius;
		PhysicsAsset* physAsset;
		PhysicsFXShape* physFxShape;
		char* unknown02;
		unsigned short unknownNamesCount;
		char unk_02[6]; // unknown data
		scr_string_t* unknownNames;
		char unk_03[16]; // unknown data
		unsigned char unknown03Count;
		char* unknown03;
		unsigned char unknownIndex;
		unsigned char unknownVec3Count;
		char unk_04[6]; // unknown data
		vec3_t* unknownVec3;
		char unk;
		unsigned char unknown04Count;
		char unk_05[6]; // unknown data
		unk_1453E14D8* unknown04;
	}; assert_sizeof(XModel, 0x2E0); // 736

	struct MayhemModel
	{
		XModel* model;
	};

	struct MayhemTransBounds
	{
		vec3_t mins;
		vec3_t size;
	};

	struct GfxWrappedBaseBuffer
	{
		ID3D11Resource* buffer;
		//ID3D11ShaderResourceView* view;
	};

	struct MayhemAnimFramesSplineCompressed
	{
		char* diskQuat;
		char* diskPos;
		unsigned __int16* diskQuatFrames;
		unsigned __int16* diskPosFrames;
		unsigned __int16* numDiskQuatFrames;
		unsigned __int16* numDiskPosFrames;
		MayhemTransBounds* transBounds;
		GfxWrappedBaseBuffer quat;
		GfxWrappedBaseBuffer pos;
		unsigned int quatStride;
		unsigned int posStride;
		unsigned int totalCompressedQuatFrames;
		unsigned int totalCompressedPosFrames;
	}; assert_sizeof(MayhemAnimFramesSplineCompressed, 0x58);

	struct MayhemAnimFramesUncompressed
	{
		char* diskQuat;
		char* diskPos;
		MayhemTransBounds* transBounds;
		GfxWrappedBaseBuffer quat;
		GfxWrappedBaseBuffer pos;
		unsigned int quatStride;
		unsigned int posStride;
	}; assert_sizeof(MayhemAnimFramesUncompressed, 0x30);

	union MayhemFramesUnion
	{
		void* data;
		MayhemAnimFramesSplineCompressed* splineCompressedFrames;
		MayhemAnimFramesUncompressed* uncompressedFrames;
	};

	struct MayhemDataKeysSplineCompressed
	{
		char* keys;
		unsigned __int16* numKeys;
		unsigned __int16* keyFrames;
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
	}; assert_sizeof(MayhemAnim, 56);

	struct MayhemObject
	{
		unsigned int modelIndex;
		unsigned int animIndex;
	};

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

	enum MaterialTechniqueType
	{
		TECHNIQUE_UNLIT = 8,
		TECHNIQUE_EMISSIVE = 10,
		TECHNIQUE_LIT = 13,
	};

	struct Packed128
	{
		std::uint64_t p0;
		std::uint64_t p1;
	};

	struct GfxDrawSurfFields
	{

	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		Packed128 packed;
	};

	enum SurfaceTypeBits : std::uint64_t
	{

	};

	struct MaterialInfo
	{
		const char* name;
		unsigned char gameFlags;
		unsigned char sortKey;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		unsigned char textureAtlasFrameBlend;
		unsigned char textureAtlasAsArray;
		unsigned char renderFlags;
		GfxDrawSurf drawSurf;
		SurfaceTypeBits surfaceTypeBits;
		unsigned int hashIndex;
	}; assert_sizeof(MaterialInfo, 48);

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		unsigned char samplerState;
		unsigned char semantic;
		GfxImage* image;
	}; assert_sizeof(MaterialTextureDef, 16);

	struct MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		float literal[4];
	}; assert_sizeof(MaterialConstantDef, 32);

	struct GfxStateBits
	{
		unsigned int loadBits[6]; // loadbits[3], blendstatebits[3]
		unsigned short zone;
		unsigned char depthStencilState[14];
		unsigned char blendState;
		unsigned char rasterizerState;
	}; assert_sizeof(GfxStateBits, 44);
	assert_offsetof(GfxStateBits, zone, 24);

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
		ID3D11Buffer* vsConstantBuffer;
		ID3D11Buffer* hsConstantBuffer;
		ID3D11Buffer* dsConstantBuffer;
		ID3D11Buffer* psConstantBuffer;
	};

	struct Material
	{
		union
		{
			const char* name;
			MaterialInfo info;
		};
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char stateFlags;
		unsigned char cameraRegion;
		unsigned char materialType;
		unsigned char layerCount;
		unsigned char assetFlags;
		char pad;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		MaterialConstantDef* constantTable;
		unsigned char stateBitsEntry[152];
		unsigned char stateBitsCount;
		GfxStateBits* stateBitsTable;
		unsigned char* constantBufferIndex; // [152]
		MaterialConstantBufferDef* constantBufferTable;
		unsigned char constantBufferCount;
		const char** subMaterials;
	}; assert_sizeof(Material, 0x118);
	assert_offsetof(Material, techniqueSet, 56);
	assert_offsetof(Material, textureCount, 48);
	assert_offsetof(Material, constantCount, 49);
	assert_offsetof(Material, stateBitsTable, 240);
	assert_offsetof(Material, constantBufferIndex, 248);
	assert_offsetof(Material, stateBitsCount, 232);
	assert_offsetof(Material, constantBufferTable, 256);
	assert_offsetof(Material, constantBufferCount, 264);
	assert_offsetof(Material, subMaterials, 272);
	assert_offsetof(Material, layerCount, 53);

	struct GfxComputeShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
	};

	struct ComputeShaderProgram
	{
		ID3D11ComputeShader* cs;
		GfxComputeShaderLoadDef loadDef;
	};

	struct ComputeShader
	{
		const char* name;
		const char* debugName;
		ComputeShaderProgram prog;
	}; assert_sizeof(ComputeShader, 0x28);

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
		const char* debugName;
		MaterialPixelShaderProgram prog;
	}; assert_sizeof(MaterialPixelShader, 0x28);

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
		const char* debugName;
		MaterialVertexShaderProgram prog;
	}; assert_sizeof(MaterialVertexShader, 0x28);

	struct GfxHullShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
	};

	struct MaterialHullShaderProgram
	{
		ID3D11HullShader* hs;
		GfxHullShaderLoadDef loadDef;
	};

	struct MaterialHullShader
	{
		const char* name;
		const char* debugName;
		MaterialHullShaderProgram prog;
	}; assert_sizeof(MaterialHullShader, 0x28);

	struct GfxDomainShaderLoadDef
	{
		unsigned char* program;
		unsigned int programSize;
	};

	struct MaterialDomainShaderProgram
	{
		ID3D11DomainShader* ds;
		GfxDomainShaderLoadDef loadDef;
	};

	struct MaterialDomainShader
	{
		const char* name;
		const char* debugName;
		MaterialDomainShaderProgram prog;
	}; assert_sizeof(MaterialDomainShader, 0x28);

	struct MaterialStreamRouting
	{
		unsigned char source;
		unsigned char dest;
		unsigned char mask;
	};

	struct MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[34];
		ID3D11InputLayout* decl[270];
	};

	struct MaterialVertexDeclaration
	{
		const char* name;
		unsigned char streamCount;
		bool hasOptionalSource;
		MaterialVertexStreamRouting routing;
	}; assert_sizeof(MaterialVertexDeclaration, 0x8E8);

	struct MaterialArgumentCodeConst
	{
		unsigned short index;
		unsigned char firstRow;
		unsigned char rowCount;
	};

	union MaterialArgumentDef
	{
		const float* literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	enum MaterialShaderArgumentType : std::uint8_t
	{

	};

	struct MaterialShaderArgument
	{
		unsigned char type;
		unsigned char shader;
		unsigned short dest;
		MaterialArgumentDef u;
	}; assert_sizeof(MaterialShaderArgument, 0x10);

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
		unsigned char stableArgCount;
		unsigned short perPrimArgSize;
		unsigned short perObjArgSize;
		unsigned short stableArgSize;
		unsigned short zone;
		char __pad0[44];
		MaterialShaderArgument* args;
	}; assert_sizeof(MaterialPass, 104);
	assert_offsetof(MaterialPass, zone, 50);
	assert_offsetof(MaterialPass, args, 96);

	struct MaterialTechniqueHeader
	{
		const char* name;
		unsigned short flags;
		unsigned short passCount;
	}; assert_sizeof(MaterialTechniqueHeader, 0x10);

	struct MaterialTechnique
	{
		MaterialTechniqueHeader hdr;
		MaterialPass passArray[1];
	};

	struct MaterialTechniqueSet
	{
		const char* name;
		unsigned short flags;
		unsigned char worldVertFormat;
		unsigned char preDisplacementOnlyCount;
		unsigned int techniqueCount;
		char __pad0[32];
		MaterialTechnique* techniques;
	}; assert_sizeof(MaterialTechniqueSet, 0x38);
	assert_offsetof(MaterialTechniqueSet, techniqueCount, 12);
	assert_offsetof(MaterialTechniqueSet, techniques, 48);

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

	enum MapType : std::uint8_t
	{
		MAPTYPE_NONE = 0x0,
		MAPTYPE_INVALID1 = 0x1,
		MAPTYPE_1D = 0x2,
		MAPTYPE_2D = 0x3,
		MAPTYPE_3D = 0x4,
		MAPTYPE_CUBE = 0x5,
		MAPTYPE_ARRAY = 0x6,
		MAPTYPE_CUBE_ARRAY = 0x7,
		MAPTYPE_COUNT = 0x8,
	};

	enum TextureSemantic : std::uint8_t
	{
		TS_2D = 0x0,
		TS_FUNCTION = 0x1,
		TS_COLOR_MAP = 0x2,
		TS_DETAIL_MAP = 0x3,
		TS_UNUSED_4 = 0x4,
		TS_NORMAL_MAP = 0x5,
		TS_UNUSED_6 = 0x6,
		TS_UNUSED_7 = 0x7,
		TS_SPECULAR_MAP = 0x8,
		TS_SPECULAR_OCCLUSION_MAP = 0x9,
		TS_UNUSED_10 = 0xA,
		TS_THINFILM_MAP = 0xB,
		TS_DISPLACEMENT_MAP = 0xC,
		TS_PARALLAX_MAP = 0xD,
		TS_COLOR_SPECULAR_MAP = 0xE,
		TS_NORMAL_OCCLUSSION_GLOSS_MAP = 0xF,
		TS_ALPHA_REVEAL_THICKNESS_MAP = 0x10,
		TS_COUNT = 0x11,
	};

	enum GfxImageCategory : std::uint8_t
	{
		IMG_CATEGORY_UNKNOWN = 0x0,
		IMG_CATEGORY_AUTO_GENERATED = 0x1,
		IMG_CATEGORY_LIGHTMAP = 0x2,
		IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
		IMG_CATEGORY_RAW = 0x4,
		IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
		IMG_CATEGORY_RENDERTARGET = 0x5,
		IMG_CATEGORY_TEMP = 0x6,
	};

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

	struct PicMip
	{
		unsigned char platform[2];
	};

	struct CardMemory
	{
		unsigned char platform[2];
	};

	struct GfxImageStreamLevelCountAndSize
	{
		unsigned int pixelSize : 26;
		unsigned int levelCount : 6;
	};

	struct GfxImageStreamData
	{
		unsigned short width;
		unsigned short height;
		GfxImageStreamLevelCountAndSize levelCountAndSize;
	};

	struct GfxImage
	{
		GfxTexture texture;
		DXGI_FORMAT imageFormat;
		GfxImageFlags flags;
		MapType mapType;
		TextureSemantic sematic;
		GfxImageCategory category;
		PicMip picmip;
		CardMemory cardMemory;
		unsigned int dataLen1;
		unsigned int dataLen2;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		unsigned char levelCount;
		unsigned char streamed;
		unsigned char unknown1;
		unsigned char* pixelData;
		GfxImageStreamData streams[4];
		const char* name;
	}; assert_sizeof(GfxImage, 0x70);

	struct snd_volmod_info_t
	{
		char name[64];
		unsigned int id;
		float value;
	};

	enum SndEntchannelSpaitalType
	{
		SND_ENTCHAN_TYPE_2D = 0x0,
		SND_ENTCHAN_TYPE_3D = 0x1,
		SND_ENTCHAN_TYPE_PA_SPEAKER = 0x2,
	};

	enum SndLimitBehavior
	{
		LIMIT_BEHAVIOR_NO_NEW = 0x0,
		LIMIT_BEHAVIOR_REPLACE_QUIETER = 0x1,
		LIMIT_BEHAVIOR_REPLACE_OLDEST = 0x2,
		LIMIT_BEHAVIOR_REPLACE_FARTHEST = 0x3,
		LIMIT_BEHAVIOR_COUNT = 0x4,
	};

	enum SndFarReverbBehavior
	{
		SND_FAR_REVERB_NONE = 0x0,
		SND_FAR_REVERB_FULL = 0x1,
		SND_FAR_REVERB_WET_ONLY = 0x2,
	};

	struct snd_entchannel_info_t
	{
		char name[64];
		unsigned int id;
		int priority;
		SndEntchannelSpaitalType spatialType;
		bool isRestricted;
		bool isPausable;
		int maxVoices;
		SndLimitBehavior limitBehavior;
		bool centerSpeakerPan;
		SndFarReverbBehavior mpFarReverbBehavior;
		SndFarReverbBehavior spFarReverbBehavior;
		SndFarReverbBehavior cpFarReverbBehavior;
	}; assert_sizeof(snd_entchannel_info_t, 104);

	struct DopplerPreset
	{
		char name[64];
		unsigned int id;
		float speedOfSound;
		float playerVelocityScale;
		float minPitch;
		float maxPitch;
		float smoothing;
	}; assert_sizeof(DopplerPreset, 88);

	struct OcclusionShape
	{
		char name[64];
		unsigned int id;
		float unk;
		float innerAngleVolume;
		float outerAngleVolume;
		float occludedVolume;
		float innerAngleLPF;
		float outerAngleLPF;
		float occludedLPF;
	}; assert_sizeof(OcclusionShape, 96);

	struct SndCurve
	{
		char name[64];
		unsigned int id;
		int numPoints;
		vec2_t points[16];
	};
	assert_sizeof(SndCurve, 200);

	struct SpeakerMap
	{
		char name[64];
		unsigned int id;
		float monoToStereo[2];
		float stereoToStereo[4];
		float monoTo51[6];
		float stereoTo51[12];
		float monoTo71[8];
		float stereoTo71[16];
	}; assert_sizeof(SpeakerMap, 260);

	struct SndContext
	{
		char* type;
		unsigned int typeId;
		int numValues;
		int startIndex;
		bool allowDefaults;
		bool includeDefaultsInRandom;
		bool canBlend;
	}; assert_sizeof(SndContext, 24);

	struct SndContextValue
	{
		char* value;
		unsigned int valueId;
	};

	struct SndMasterEqParam
	{
		int enabled;
		float gain;
		float frequency;
		float q;
	};

	struct SndMasterEqParamBank
	{
		SndMasterEqParam low;
		SndMasterEqParam peak1;
		SndMasterEqParam peak2;
		SndMasterEqParam high;
	};

	struct SndDynamicsParams
	{
		int enable;
		float preGain;
		float postGain;
		float threshold;
		float ratio;
		float timeAttack;
		float timeRelease;
		float spread;
		int linear;
		int ITU1770;
	};

	struct SndMaster
	{
		char name[64];
		unsigned int id;
		SndMasterEqParamBank dialogEq;
		SndMasterEqParamBank weapExpScenesEq;
		SndMasterEqParamBank sfxEq;
		SndMasterEqParamBank mainEq;
		SndDynamicsParams dialogDyn;
		SndDynamicsParams sfxDyn;
		SndDynamicsParams mainDyn;
		float busDialogGain;
		float busWeapExpSceneGain;
		float busSfxGain;
		float busReverbGain;
		float busMusicGain;
		float busMovieGain;
		float busReferenceGain;
		int busDialogEnable;
		int busWeapExpSceneEnable;
		int busSfxEnable;
		int busReverbEnable;
		int busMusicEnable;
		int busMovieEnable;
		int busReferenceEnable;
	}; assert_sizeof(SndMaster, 500);

	struct SndGlobals
	{
		const char* name;
		float globalVolumeModifier;
		float binkVolumeModifier;
		unsigned int volmodinfoCount;
		snd_volmod_info_t* volmodinfo;
		unsigned int entchannelCount;
		snd_entchannel_info_t* entchannelInfo;
		unsigned int dopplerPresetCount;
		DopplerPreset* dopplerPresets;
		unsigned int occlusionShapeCount;
		OcclusionShape* occlusionShapes;
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
		unsigned int contextCount;
		SndContext* contexts;
		unsigned int contextValueCount;
		SndContextValue* contextValues;
		unsigned int masterCount;
		SndMaster* masters;
	}; assert_sizeof(SndGlobals, 0xD0);

	enum SoundVolMod : std::int32_t
	{
		SND_VOLMOD_DEFAULT,
		SND_VOLMOD_HUD,
		SND_VOLMOD_VOICEOVER = 28,
	};

	struct SndAlias
	{
		const char* aliasName;
		const char* subtitle;
		const char* secondaryAliasName;
		const char* unk_24;
		const char* assetFileName;
		unsigned int id;
		unsigned int secondaryId;
		unsigned int unk_48;
		unsigned int assetId;
		unsigned int contextType;
		unsigned int contextValue;
		unsigned int duck;
		int sequence;
		float volMin;
		float volMax;
		int volModIndex;
		float pitchMin;
		float pitchMax;
		float donutFadeEnd;
		float distMin;
		float distMax;
		float velocityMin;
		int flags;
		int masterPriority;
		float masterPercentage;
		float slavePercentage;
		float probability;
		float lfePercentage;
		float centerPercentage;
		int startDelay;
		int volumeFalloffCurve; // guess
		float envelopMin;
		float envelopMax;
		float envelopPercentage;
		int speakerMap; // guess
		float reverbMultiplier;
		float farReverbMultiplier;
		int dopplerPreset; // guess
		float smartPanDistance2d;
		float smartPanDistance3d;
		float smartPanAttenuation2d;
		float stereoSpreadMinDist;
		float stereoSpreadMaxDist;
		float stereoSpreadMidPoint;
		int stereoSpreadMaxAngle;
	}; assert_sizeof(SndAlias, 200);
	assert_offsetof(SndAlias, flags, 108);

	struct SndAliasList
	{
		const char* aliasName;
		unsigned int id;
		SndAlias* head;
		int count;
		int sequence;
	}; assert_sizeof(SndAliasList, 32);
	assert_offsetof(SndAliasList, head, 16);
	assert_offsetof(SndAliasList, count, 24);

	struct SndIndexEntry
	{
		unsigned short value;
		unsigned short next;
	}; assert_sizeof(SndIndexEntry, 4);

	struct SndSendEffectParams
	{
		char name[64];
		unsigned int id;
		float smoothing;
		float earlyTime;
		float lateTime;
		float earlyGain;
		float lateGain;
		float lateGainProx;
		float returnGain;
		float earlyLpf;
		float lateLpf;
		float inputLpf;
		float dampLpf;
		float wallReflect;
		float dryGain;
		float earlySize;
		float lateSize;
		float diffusion;
		float rearLevel;
	}; assert_sizeof(SndSendEffectParams, 136);

	enum ADSRCurve
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
		unsigned int id;
		float attackLength;
		ADSRCurve attackCurve;
		float decayLength;
		ADSRCurve decayCurve;
		float releaseLength;
		ADSRCurve releaseCurve;
		float sustainLevel;
		float sustainLength;
	};

	struct AmbientDef
	{
		const char* name;
		unsigned int id;
		unsigned __int16 ambientEventIndex;
		unsigned __int16 numEvents;
		float timeMin;
		float timeMax;
	};

	struct AmbientEvent
	{
		unsigned int id;
		unsigned __int16 ambientElementIndex;
		float weight;
	};

	struct AmbientElement
	{
		unsigned int id;
		const char* unusedNameField;
		const char* aliasName;
		float rangeMin;
		float rangeMax;
		float coneMin;
		float coneMax;
	};

	enum SND_EQTYPE
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
		unsigned int id;
		char entChannelIdx;
		char band;
		SND_EQTYPE type;
		float freq;
		float gain;
		float q;
	};

	struct MixDef
	{
		unsigned int id;
		unsigned __int16 volModIndex;
		float volume;
		float fade;
	};

	struct AdsrZoneEntry
	{
		const char* name;
		unsigned int id;
		const char* weaponName;
		int adsrIdx;
		unsigned int weaponIdx;
	};

	struct OccludeDef
	{
		unsigned int id;
		unsigned __int16 entChannelIdx;
		float cutoffLevel1;
		float cutoffLevel2;
		float cutoffLevel3;
		float cutoffLevel4;
		float attnLevel1;
		float attnLevel2;
		float attnLevel3;
		float attnLevel4;
	};

	struct ReverbDef
	{
		unsigned int id;
		unsigned int roomTypeId;
		char roomType[64];
		float dryLevel;
		float wetLevel;
		float fadeTime;
	}; assert_sizeof(ReverbDef, 84);

	struct TimescaleEntry
	{
		unsigned int id;
		unsigned int presetName;
		int entChannelIdx;
		float scale;
	}; assert_sizeof(TimescaleEntry, 16);

	struct ZoneDef
	{
		unsigned int id;
		unsigned int stateId;
		__int16 reverbIndex;
		__int16 startOcclusionIndex;
		__int16 numOcclusion;
		__int16 startFilterIndex;
		__int16 numFilter;
		__int16 startMixIndex;
		__int16 numMix;
		const char* ambientStream;
		__int16 startAdsrZoneIndex;
		__int16 numAdsrZone;
		__int16 startNPCAdsrZoneIndex;
		__int16 numNPCAdsrZone;
		__int16 ambientDefIndex;
		__int16 numAmbientDef; // idk
		__int16 startFullOccIndex;
		__int16 numDisableFullOcc;
		__int16 startPlayerBreathStateIndex;
		__int16 numPlayerBreathStates;
		unsigned int weapReflId;
		//bool exterior;
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
		unsigned int contextType1;
		unsigned int contextValue1;
		unsigned int contextType2;
		unsigned int contextValue2;
		unsigned int contextType3;
		unsigned int contextValue3;
		unsigned int contextType4;
		unsigned int contextValue4;
		//unsigned int duck;
	}; assert_sizeof(ZoneDef, 184);

	struct unk_1453E2FD8
	{
		unsigned int id;
		int __pad0[13]; //char __pad0[56];
	}; assert_sizeof(unk_1453E2FD8, 56);

	struct FullOcclusionDef
	{
		unsigned int id;
		unsigned int presetName;
		int entChannelIdx;
	};

	struct PlayerBreathStateDef
	{
		unsigned int stateType;
		const char* inhaleAlias;
		const char* exhaleAlias;
		int minBreaths;
	};

	struct SoundTable
	{
		unsigned int id;
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
		unk_1453E2FD8* unk;
		unsigned int unkCount;
		FullOcclusionDef* fullOcclusionDefs;
		unsigned int fullOcclusionDefCount;
		PlayerBreathStateDef* plrBreathStateDefs;
		unsigned int plrBreathStateDefCount;
	}; assert_sizeof(SoundTable, 248);

	struct SndDuck
	{
		char name[64];
		unsigned int id;
		float fadeIn;
		float fadeOut;
		float startDelay;
		float minDistance;
		float distance;
		float length;
		unsigned int fadeInCurve;
		unsigned int fadeOutCurve;
		float* hpf;
		float* lpf;
		bool updateWhilePaused;
		bool trackAmplitude;
		bool disableInSplitscreen;
		char pad;
		int unk1;
		int unk2;
		float aliasHpf;
		float aliasLpf;
		unsigned int duckAlias;
	}; assert_sizeof(SndDuck, 144);

	enum SndMusicAssetType
	{
		SND_MUSIC_INTRO = 0x0,
		SND_MUSIC_LOOP = 0x1,
		SND_MUSIC_EXIT = 0x2,
	};

	struct SndMusicAsset
	{
		char alias[64];
		unsigned int aliasId;
		int inactive;
		int completeLoop;
		int removeAfterPlay;
		int firstRandom;
		int startSync;
		int stopSync;
		int completeOnStop;
		unsigned int loopStartOffset;
		int bpm;
		int assetType;
		int loopNumber;
		unsigned int sampleCount;
		int startDelayBeats;
		int startFadeBeats;
		int stopDelayBeats;
		int stopFadeBeats;
		//int startOffsetFrames;
		int meter;
		unsigned int sampleRate;
	}; assert_sizeof(SndMusicAsset, 140);

	enum SndMusicStateStatus
	{
		SND_MUSIC_STATE_INACTIVE = 0x0,
		SND_MUSIC_STATE_ACTIVE = 0x1,
	};

	struct SndMusicState
	{
		char name[64];
		unsigned int id;
		SndMusicAsset intro;
		SndMusicAsset exit;
		unsigned int loopCount;
		SndMusicAsset* loops;
		int order;
		int interruptPriority;
		int persistPriority;
		bool isRandom;
		bool isSequential;
		bool skipPreviousExit;
		bool wallClockSync;
		unsigned int refCount;
		SndMusicStateStatus status;
	}; assert_sizeof(SndMusicState, 384);

	struct SndMusicSet
	{
		char name[64];
		unsigned int id;
		unsigned int stateCount;
		SndMusicState* states;
	}; assert_sizeof(SndMusicSet, 80);
	assert_offsetof(SndMusicSet, stateCount, 68);
	assert_offsetof(SndMusicSet, states, 72);

	struct SndBank
	{
		const char* name;
		const char* zone;
		const char* gameLanguage;
		const char* soundLanguage;
		unsigned int aliasCount;
		SndAliasList* alias;
		SndIndexEntry* aliasIndex;
		unsigned int sendEffectCount;
		SndSendEffectParams* sendEffects;
		SoundTable soundTable;
		unsigned int duckCount;
		SndDuck* ducks;
		unsigned int musicSetCount;
		SndMusicSet* musicSets;
	};
	assert_offsetof(SndBank, name, 0);
	assert_offsetof(SndBank, zone, 8);
	assert_offsetof(SndBank, gameLanguage, 16);
	assert_offsetof(SndBank, soundLanguage, 24);
	assert_offsetof(SndBank, aliasCount, 32);
	assert_offsetof(SndBank, aliasIndex, 48);
	assert_offsetof(SndBank, sendEffectCount, 56);
	assert_offsetof(SndBank, sendEffects, 64);
	assert_offsetof(SndBank, soundTable, 72);
	assert_offsetof(SndBank, duckCount, 320);
	assert_offsetof(SndBank, ducks, 328);
	assert_offsetof(SndBank, musicSetCount, 336);
	assert_offsetof(SndBank, musicSets, 344);

	struct SndBankResident
	{
		SndBank bank;
	}; assert_sizeof(SndBankResident, 0x160);

	struct SndBankTransient
	{
		const char* name;
		unsigned int elementCount;
		unsigned int* elements;
	}; assert_sizeof(SndBankTransient, 0x18);

	struct clipMap_t;

	enum GfxLightType : std::uint8_t
	{
		GFX_LIGHT_TYPE_NONE = 0x0,
		GFX_LIGHT_TYPE_DIR = 0x1,
		GFX_LIGHT_TYPE_SPOT = 0x2,
		GFX_LIGHT_TYPE_OMNI = 0x3,
		GFX_LIGHT_TYPE_COUNT = 0x4,
		GFX_LIGHT_TYPE_DIR_SHADOWMAP = 0x4,
		GFX_LIGHT_TYPE_SPOT_SHADOWMAP = 0x5,
		GFX_LIGHT_TYPE_OMNI_SHADOWMAP = 0x6,
		GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS = 0x7,
		GFX_LIGHT_TYPE_SPOT_SHADOWMAP_CUCOLORIS = 0x7,
		GFX_LIGHT_TYPE_COUNT_WITH_ALL_VERSIONS = 0x8,
	};

	struct ComPrimaryLight
	{
		GfxLightType type;
		unsigned char canUseShadowMap;
		unsigned char needsDynamicShadows;
		unsigned char isVolumetric;
		unsigned char exponent;
		char unused[3];
		unsigned int transientZoneList;
		unsigned int entityId;
		float uvIntensity;
		float heatIntensity;
		float color[3];
		float dir[3];
		float up[3];
		float origin[3];
		float radius;
		vec2_t fadeOffsetRt;
		float bulbRadius;
		vec3_t bulbLength;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float shadowSoftness;
		float shadowBias;
		float shadowArea;
		float distanceFalloff;
		float rotationLimit;
		float translationLimit;
		const char* defName;
	}; assert_sizeof(ComPrimaryLight, 144);

	struct ComPrimaryLightEnv
	{
		unsigned short primaryLightIndices[4];
		unsigned char numIndices;
	}; assert_sizeof(ComPrimaryLightEnv, 10);

	struct ComChangeListInfo
	{
		unsigned int changeListNumber;
		int time;
		const char* userName;
	}; assert_sizeof(ComChangeListInfo, 16);

	struct ComWorld
	{
		const char* name;
		int isInUse;
		int useForwardPlus;
		unsigned int bakeQuality;
		unsigned int primaryLightCount;
		ComPrimaryLight* primaryLights;
		unsigned int scriptablePrimaryLightCount;
		unsigned int firstScriptablePrimaryLight;
		unsigned int primaryLightEnvCount;
		ComPrimaryLightEnv* primaryLightEnvs;
		ComChangeListInfo changeListInfo;
		unsigned int numUmbraGates;
		const char** umbraGateNames;
		int umbraGateInitialStates[4];
	}; assert_sizeof(ComWorld, 0x68);

	struct G_GlassPiece
	{
		unsigned short damageTaken;
		unsigned short collapseTime;
		int lastStateChangeTime;
		char impactDir;
		char impactPos[2];
	};

	struct G_GlassName
	{
		char* nameStr;
		scr_string_t name;
		unsigned short pieceCount;
		unsigned short* pieceIndices;
	};

	struct G_GlassData
	{
		G_GlassPiece* glassPieces;
		unsigned int pieceCount;
		unsigned short damageToWeaken;
		unsigned short damageToDestroy;
		unsigned int glassNameCount;
		G_GlassName* glassNames;
	};

	struct GlassWorld
	{
		const char* name;
		G_GlassData* g_glassData;
	};

	struct pathnode_t;
	struct pathnode_tree_t;

	struct pathnode_yaworient_t
	{
		float fLocalAngle;
		vec2_t localForward;
	};

	union PathNodeOrientationUnion
	{
		pathnode_yaworient_t yaw_orient;
		vec3_t angles;
	};

	union PathNodeParentUnion
	{
		scr_string_t name;
		unsigned short index;
	};

	enum PathNodeErrorCode
	{
		PNERR_NONE = 0x0,
		PNERR_INSOLID = 0x1,
		PNERR_FLOATING = 0x2,
		PNERR_NOLINK = 0x3,
		PNERR_DUPLICATE = 0x4,
		PNERR_NOSTANCE = 0x5,
		PNERR_INVALIDDOOR = 0x6,
		PNERR_NOANGLES = 0x7,
		PNERR_NOPEEKOUT = 0x8,
		PNERR_NEARSOLID = 0x9,
		PNERR_NOCOVER = 0xA,
		NUM_PATH_NODE_ERRORS = 0xB,
	};

	union $96C58BBB7F3ED2CCC5F07A7E763FE6AE
	{
		float minUseDistSq;
		PathNodeErrorCode error;
	};

	struct pathlink_s
	{
		float fDist;
		unsigned short nodeNum;
		char disconnectCount;
		char negotiationLink;
		char flags;
		char ubBadPlaceCount[3];
	};

	struct pathnode_constant_t
	{
		unsigned short type;
		unsigned int spawnflags;
		scr_string_t targetname;
		scr_string_t script_linkName;
		scr_string_t script_noteworthy;
		scr_string_t target;
		scr_string_t animscript;
		int animscriptfunc;
		vec3_t vLocalOrigin;
		PathNodeOrientationUnion orientation;
		PathNodeParentUnion parent;
		$96C58BBB7F3ED2CCC5F07A7E763FE6AE ___u11;
		unsigned short wOverlapNode[2];
		unsigned short totalLinkCount;
		pathlink_s* Links;
	}; assert_sizeof(pathnode_constant_t, 80);
	assert_offsetof(pathnode_constant_t, parent, 56);
	assert_offsetof(pathnode_constant_t, animscript, 24);
	assert_offsetof(pathnode_constant_t, Links, 72);

	struct SentientHandle
	{
		unsigned short number;
		unsigned short infoIndex;
	};

	struct pathnode_dynamic_t
	{
		SentientHandle pOwner;
		int iFreeTime;
		int iValidTime[3];
		short wLinkCount;
		short wOverlapCount;
		short turretEntNumber;
		unsigned char userCount;
		unsigned char hasBadPlaceLink;
		int spreadUsedTime[2];
		short flags;
		short dangerousCount;
		int recentUseProxTime;
	};

	union $73F238679C0419BE2C31C6559E8604FC
	{
		float nodeCost;
		int linkIndex;
	};

	struct pathnode_transient_t
	{
		int iSearchFrame;
		pathnode_t* pNextOpen;
		pathnode_t* pPrevOpen;
		pathnode_t* pParent;
		float fCost;
		float fHeuristic;
		$73F238679C0419BE2C31C6559E8604FC ___u6;
	};

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
		pathnode_transient_t transient;
	}; assert_sizeof(pathnode_t, 176);

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

	struct PathDynamicNodeGroup
	{
		unsigned short parentIndex;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
	};

	struct PathData
	{
		const char* name;
		unsigned int nodeCount;
		pathnode_t* nodes;
		bool parentIndexResolved;
		unsigned __int16 version;
		int visBytes;
		char* pathVis;
		int nodeTreeCount;
		pathnode_tree_t* nodeTree;
		int dynamicNodeGroupCount;
		PathDynamicNodeGroup* dynamicNodeGroups;
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
	}; assert_sizeof(PathData, 0x90);

	struct NavMeshData;

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

	struct TriggerWinding
	{
		vec3_t dir;
		unsigned short pointCount;
		unsigned short firstPoint;
		unsigned int flags;
	};

	struct TriggerWindingPoint
	{
		vec3_t loc;
	};

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

	struct CTAudRvbPanInfo
	{
		bool hasCustomPosition;
		vec3_t position;
		float maxWallDistance;
		float minReverbVolume;
	};

	struct ClientEntityLinkToDef
	{
		int anchorIndex;
		vec3_t originOffset;
		vec3_t angleOffset;
	};

	struct ClientTriggers
	{
		MapTriggers trigger;
		unsigned int triggerStringLength;
		char* triggerString;
		short* visionSetTriggers;
		unsigned char* triggerType;
		vec3_t* origins;
		float* scriptDelay;
		short* audioTriggers;
		short* blendLookup;
		short* npcTriggers;
		short* audioStateIds;
		CTAudRvbPanInfo* audioRvbPanInfo;
		short* transientIndex; // guess
		ClientEntityLinkToDef** linkTo;
	}; assert_sizeof(ClientTriggers, 0xB8);

	struct ClientTriggerBlendNode
	{
		vec3_t pointA;
		vec3_t pointB;
		unsigned short triggerA;
		unsigned short triggerB;
	};

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
		vec3_t origin;
		vec3_t angles;
	};

	struct SpawnPointRecordList
	{
		unsigned short spawnsCount;
		SpawnPointEntityRecord* spawns;
	};

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
	};

	struct SplinePointRecordList
	{
		unsigned short splinePointCount;
		float splineLength;
		SplinePointEntityRecord* splinePoints;
	};

	struct SplineRecordList
	{
		unsigned short splineCount;
		SplinePointRecordList* splines;
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
		unsigned int havokEntsShapeDataSize;
		char* havokEntsShapeData;
		//unsigned int numSubModels;
		//cmodel_t* cmodels;
		char __pad0[480]; // todo
	}; assert_sizeof(MapEnts, 0x340);
	assert_offsetof(MapEnts, havokEntsShapeDataSize, 336);

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
		const char* damagedSound;
		const char* destroyedSound;
		const char* destroyedQuietSound;
		float highMipRadiusInvSq;
		float shatteredHighMipRadiusInvSq;
		int numCrackRings;
		bool isOpaque;
	};

	struct FxSpatialFrame
	{
		vec4_t quat;
		vec3_t origin;
	};

	struct $03A8A7B39FA20F64B5AB79125E07CD62
	{
		FxSpatialFrame frame;
		float radius;
	};

	union FxGlassPiecePlace
	{
		$03A8A7B39FA20F64B5AB79125E07CD62 __s0;
		unsigned int nextFree;
	};

	struct FxGlassPieceState
	{
		vec2_t texCoordOrigin;
		unsigned int supportMask;
		unsigned int geoDataStart;
		unsigned __int16 initIndex;
		char defIndex;
		char pad[1];
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
		unsigned int physicsInstance;
		unsigned int physicsDetailInstance;
		vec3_t vel;
		vec3_t avel;
		bool pendingCreation;
	};

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

	struct FxGlassSystem
	{
		int time;
		int prevTime;
		unsigned int defCount;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int initPieceCount;
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
		FxGlassInitPieceState* initPieceStates;
		FxGlassGeometryData* initGeoData;
		bool needToCompactData;
		unsigned char initCount;
		float effectChanceAccum;
		int lastPieceDeletionTime;
	};

	struct FxWorld
	{
		const char* name;
		FxGlassSystem glassSys;
	};

	struct GfxWorld;

	struct GfxWorldTransientZone;

	struct GfxLightDef
	{
		const char* name;
		const char* iesProfile;
		char __pad0[24];
	}; assert_sizeof(GfxLightDef, 0x28);

	struct AnimationEntry
	{
		scr_string_t animName;
		int aimSetIndex;
	};

	struct AnimAliasInfo
	{
		char animIndex;
		float animWeight;
	};

	struct AnimAlias
	{
		scr_string_t aliasName;
		char animCount;
		AnimAliasInfo* aliasInfo;
	};

	struct AnimationState
	{
		scr_string_t name;
		scr_string_t notify;
		float blendTime;
		float blendOutTime;
		char flags;
		char entryCount;
		char aliasCount;
		int aimSetIndex;
		AnimationEntry* animEntries;
		unsigned __int64* animIndices;
		AnimAlias* aliasList;
	};

	struct AnimationAimSet
	{
		scr_string_t name;
		scr_string_t rootName;
		int animCount;
		scr_string_t* animName;
		unsigned __int64 rootIndex;
		unsigned __int64* animIndices;
		unsigned __int64* aimNodeIndices;
	};

	struct AnimationStateMachine
	{
		scr_string_t name;
		unsigned __int16 stateCount;
		unsigned __int16 aimSetCount;
		AnimationState* states;
		AnimationAimSet* aimSets;
	};

	enum AnimationController
	{
		ANIMCTRL_NONE = 0x0,
		ANIMCTRL_PLAYER = 0x1,
		ANIMCTRL_DOG = 0x2,
		ANIMCTRL_NUM = 0x3,
	};

	struct AnimationClass
	{
		const char* className;
		AnimationStateMachine* stateMachine;
		AnimationController animCtrl;
		scr_string_t animTree;
		ScriptableDef* scriptable;
		unsigned __int16 soundCount;
		unsigned __int16 effectCount;
		scr_string_t* soundNotes;
		scr_string_t* soundNames;
		scr_string_t* soundOptions;
		scr_string_t* effectNotes;
		FxCombinedDef* effectDefs;
		scr_string_t* effectTags;
	}; assert_sizeof(AnimationClass, 0x58);

	struct PlayerAnimEntry
	{
		scr_string_t entryName;
		scr_string_t animName;
		scr_string_t shadowAnimName;
		char __pad0[68];
	}; assert_sizeof(PlayerAnimEntry, 80);

	struct PlayerAnimScriptEntry
	{
		unsigned int flags;
		unsigned int itemCount;
		unsigned int transitionCount;
		unsigned int* items;
		unsigned int* transitions;
	};

	struct PlayerAnimScriptCondition
	{
		unsigned int index;
		unsigned int value[4];
	}; assert_sizeof(PlayerAnimScriptCondition, 20);

	struct PlayerAnimScriptCommand
	{
		unsigned __int16 bodyPart;
		unsigned __int16 animIndex;
		unsigned __int16 animDuration;
	};

	struct PlayerAnimScriptItem
	{
		unsigned int conditionCount;
		unsigned int commandCount;
		PlayerAnimScriptCondition* conditions;
		PlayerAnimScriptCommand* commands;
	};

	enum PlayerAnimScriptTransitionTriggerType
	{
		ANIM_TRANSITIONTRIGGERTYPE_MOVETYPE = 0x0,
		ANIM_TRANSITIONTRIGGERTYPE_MOVETYPE_NOT_STRAFING = 0x1,
		ANIM_TRANSITIONTRIGGERTYPE_ANIMCOMPLETE = 0x2,
		ANIM_TRANSITIONTRIGGERTYPE_ANIMCHANGE = 0x3,
		NUM_ANIM_TRANSITION_TRIGGER_TYPES = 0x4,
	};

	enum PlayerAnimScriptMoveType
	{
		ANIM_MT_UNUSED = 0x0,
		ANIM_MT_IDLE = 0x1,
		ANIM_MT_IDLECR = 0x2,
		ANIM_MT_IDLEPRONE = 0x3,
		ANIM_MT_WALK = 0x4,
		ANIM_MT_WALKBK = 0x5,
		ANIM_MT_WALKCR = 0x6,
		ANIM_MT_WALKCRBK = 0x7,
		ANIM_MT_WALKPRONE = 0x8,
		ANIM_MT_WALKPRONEBK = 0x9,
		ANIM_MT_RUN = 0xA,
		ANIM_MT_RUNBK = 0xB,
		ANIM_MT_RUNCR = 0xC,
		ANIM_MT_RUNCRBK = 0xD,
		ANIM_MT_CLIMBUP = 0xE,
		ANIM_MT_LADDERSLIDE = 0xF,
		ANIM_MT_CLIMBDOWN = 0x10,
		ANIM_MT_JOG = 0x11,
		ANIM_MT_SPRINT = 0x12,
		ANIM_MT_SUPERSPRINT = 0x13,
		ANIM_MT_MANTLE = 0x14,
		ANIM_MT_IDLELASTSTAND = 0x15,
		ANIM_MT_CRAWLLASTSTAND = 0x16,
		ANIM_MT_CRAWLLASTSTANDBK = 0x17,
		ANIM_MT_SLIDE = 0x18,
		ANIM_MT_SLIDEBK = 0x19,
		ANIM_MT_MOUNTED_TOP = 0x1A,
		ANIM_MT_MOUNTED_LEFT = 0x1B,
		ANIM_MT_MOUNTED_RIGHT = 0x1C,
		ANIM_MT_BOOSTAIRDODGE = 0x1D,
		ANIM_MT_BOOSTAIRDODGELEFT = 0x1E,
		ANIM_MT_BOOSTAIRDODGERIGHT = 0x1F,
		ANIM_MT_BOOSTAIRDODGEBACK = 0x20,
		ANIM_MT_WALLRUN = 0x21,
		ANIM_MT_AIR = 0x22,
		ANIM_MT_KNOCKBACK = 0x23,
		ANIM_MT_KNOCKBACKBK = 0x24,
		ANIM_MT_PARACHUTE = 0x25,
		NUM_ANIM_CODEMOVETYPES = 0x26,
		FIRST_TRANSITION_MOVETYPE = 0x26,
		NUM_ANIM_MOVETYPES = 0x80,
	};

	struct PlayerAnimTriggerParam
	{
		PlayerAnimScriptMoveType moveType;
	};

	struct PlayerAnimScriptTransition
	{
		PlayerAnimScriptTransitionTriggerType triggerType;
		PlayerAnimTriggerParam triggerParam;
		PlayerAnimScriptMoveType targetMoveType;
	};

	struct PlayerAnimScriptIdleTurn
	{
		unsigned __int16 turnleft;
		unsigned __int16 turnright;
	};

	struct PlayerAnimScriptIdleTwitch
	{
		unsigned int twitchCount;
		float cycleLength;
		unsigned __int16* twitches;
	};

	struct PlayerAnimScriptAimSet
	{
		unsigned __int16 aim_1;
		unsigned __int16 aim_2;
		unsigned __int16 aim_3;
		unsigned __int16 aim_4;
		unsigned __int16 aim_6;
		unsigned __int16 aim_7;
		unsigned __int16 aim_8;
		unsigned __int16 aim_9;
	};

	struct PlayerAnimScriptLeanSet
	{
		unsigned __int16 lean_left;
		unsigned __int16 lean_right;
	};

	struct unk_1453E3788
	{
		char __pad0[48];
	};

	struct PlayerAnimScript
	{
		const char* scriptName;
		unsigned int animationCount;
		unsigned int scriptItemCount;
		unsigned int scriptTransitionCount;
		unsigned int scriptIdleTurnCount;
		unsigned int scriptIdleTwitchCount;
		unsigned int scriptAimSetCount;
		unsigned int scriptLeanSetCount;
		unsigned int unkCount;
		unsigned int torsoAnimCount;
		unsigned int legsAnimCount;
		PlayerAnimEntry* animations;
		PlayerAnimScriptEntry* scriptAnims; // count: 121
		PlayerAnimScriptEntry* scriptEvents; // count: 55
		PlayerAnimScriptItem* scriptItems;
		PlayerAnimScriptTransition* scriptTransitions;
		PlayerAnimScriptIdleTurn* scriptIdleTurns;
		PlayerAnimScriptIdleTwitch* scriptIdleTwitches;
		PlayerAnimScriptAimSet* scriptAimSets;
		PlayerAnimScriptLeanSet* scriptLeanSets;
		unk_1453E3788* unk;
		unsigned __int16* torsoAnimPackMap;
		unsigned __int16* torsoAnimUnpackMap;
		unsigned __int16* legsAnimPackMap;
		unsigned __int16* legsAnimUnpackMap;
		unsigned int xAnimCount;
		XAnimParts** xAnims;
	}; assert_sizeof(PlayerAnimScript, 0xB0);

	struct Gesture;

	struct LocalizeEntry
	{
		const char* name;
		const char* value;
	};

	struct WeaponAttachment;

	struct WeaponCompleteDef;

	enum PARTICLE_FLAG
	{
		PARTICLE_FLAG_KILL = 0x1,
		PARTICLE_FLAG_PENDING_DEFERRED_PHYSICS = 0x2,
		PARTICLE_FLAG_IMPACT = 0x4,
		PARTICLE_FLAG_IMPACT_WITH_RAYCAST = 0x8,
		PARTICLE_FLAG_AT_REST = 0x10,
		PARTICLE_FLAG_SENT_FIRST_RAY_CAST = 0x20,
		PARTICLE_FLAG_PENDING_DEFERRED_PHYSICS_UPDATE = 0x40,
		PARTICLE_FLAG_HAS_SOUND = 0x80,
	};

	enum PARTICLE_MODULE_FLAG
	{
		PARTICLE_MODULE_FLAG_DISABLED = 0x1,
		PARTICLE_MODULE_FLAG_HAS_CURVES = 0x2,
		PARTICLE_MODULE_FLAG_USE_EMITTER_LIFE = 0x4,
		PARTICLE_MODULE_FLAG_HAS_ASSETS = 0x8,
		PARTICLE_MODULE_FLAG_RANDOMIZE_BETWEEN_CURVES = 0x10,
		PARTICLE_MODULE_FLAG_USE_NON_UNIFORM_INTERPOLATION = 0x20,
		PARTICLE_MODULE_FLAG_NO_LERP = 0x40,
		PARTICLE_MODULE_FLAG_USE_WORLD_SPACE = 0x80,
		PARTICLE_MODULE_FLAG_GRAPH_BY_LENGTH = 0x100,
		PARTICLE_MODULE_FLAG_UPDATE_EXTERNAL = 0x200,
		PARTICLE_MODULE_FLAG_GRAPH_ADDITIVE = 0x400,
		PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS = 0x800,
		PARTICLE_MODULE_FLAG_GRAPH_IS_LINEAR = 0x1000,
		PARTICLE_MODULE_FLAG_MODEL_IGNORE_EMITTER_ORIENTATION = 0x2000,
		PARTICLE_MODULE_FLAG_USE_OCCLUSION_QUERY_WORLD_SIZE = 0x4000,
		PARTICLE_MODULE_FLAG_EMISSION_CURVE_SCRUBBED_BY_GAME = 0x8000,
		PARTICLE_MODULE_FLAG_ON_IMPACT_BOLT_TO_ENTITY = 0x10000,
		PARTICLE_MODULE_FLAG_RAYCAST_COLLIDE_WITH_WZ_TRAIN = 0x20000,
		PARTICLE_MODULE_FLAGS_ALL = 0xFFFFFFFF,
	};

	enum PARTICLE_EMITTER_DEF_FLAG
	{
		PARTICLE_EMITTER_DEF_FLAG_DISABLED = 0x1,
		PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE = 0x2,
		PARTICLE_EMITTER_DEF_FLAG_DRAW_PAST_FOG = 0x4,
		PARTICLE_EMITTER_DEF_FLAG_INFINITE_PARTICLE_LIFE = 0x8,
		PARTICLE_EMITTER_DEF_FLAG_LOOP_INFINITE_PARTICLES = 0x10,
		PARTICLE_EMITTER_DEF_FLAG_HAS_PHYSICS_HEAVY = 0x20,
		PARTICLE_EMITTER_DEF_FLAG_HAS_SCRIPTED_INPUTS = 0x40,
		PARTICLE_EMITTER_DEF_FLAG_HAS_LIGHTS = 0x80,
		PARTICLE_EMITTER_DEF_FLAG_USE_SCRIPTED_VELOCITY = 0x100,
		PARTICLE_EMITTER_DEF_FLAG_EMIT_BY_DISTANCE = 0x200,
		PARTICLE_EMITTER_DEF_FLAG_USE_OCCLUSION_QUERY = 0x400,
		PARTICLE_EMITTER_DEF_FLAG_HAS_TRANS_SHADOWS = 0x800,
		PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_NVG_ON = 0x1000,
		PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_NVG_OFF = 0x2000,
		PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_THERMAL_ON = 0x4000,
		PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_THERMAL_OFF = 0x8000,
		PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_SP = 0x10000,
		PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_MP_LPC = 0x20000,
		PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_MP_HPC = 0x40000,
		PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_CP = 0x80000,
		PARTICLE_EMITTER_DEF_FLAG_HAS_INSTANCE_POOL = 0x100000,
		PARTICLE_EMITTER_DEF_FLAG_FORCE_HALF_RES = 0x200000,
		PARTICLE_EMITTER_DEF_FLAG_FORCE_FULL_RES = 0x400000,
		PARTICLE_EMITTER_DEF_FLAG_IGNORE_MODIFIERS = 0x800000,
		PARTICLE_EMITTER_DEF_FLAG_HAS_BARREL_HEAT_SCALE = 0x1000000,
		PARTICLE_EMITTER_DEF_FLAG_IGNORE_FOV_SCALE = 0x2000000,
	};

	enum PARTICLE_ELEMENT_TYPE
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
		PARTICLE_ELEMENT_TYPE_DISMEMBER = 0xD,
	};

	enum PARTICLE_STATE_DEF_FLAG
	{
		PARTICLE_STATE_DEF_FLAG_DISABLED = 0x1,
		PARTICLE_STATE_DEF_FLAG_HAS_CAMERA_OFFSET = 0x2,
		PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE = 0x4,
		PARTICLE_STATE_DEF_FLAG_HAS_POSITION_CURVE = 0x8,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_CURVE = 0x10,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_CURVE = 0x20,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_INIT = 0x40,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_INIT = 0x80,
		PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL = 0x100,
		PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD = 0x200,
		PARTICLE_STATE_DEF_FLAG_USE_PHYSICS = 0x400,
		PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY = 0x800,
		PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY_RANDOM = 0x1000,
		PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY = 0x2000,
		PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY_RANDOM = 0x4000,
		PARTICLE_STATE_DEF_FLAG_SORT_PARTICLES = 0x8000,
		PARTICLE_STATE_DEF_FLAG_HANDLE_ON_IMPACT = 0x10000,
		PARTICLE_STATE_DEF_FLAG_PLAYER_FACING = 0x20000,
		PARTICLE_STATE_DEF_FLAG_PLAYER_FACING_LOCK_UP_VECTOR = 0x40000,
		PARTICLE_STATE_DEF_FLAG_USE_OCCLUSION_QUERY = 0x80000,
		PARTICLE_STATE_DEF_FLAG_HAS_COLOR = 0x100000,
		PARTICLE_STATE_DEF_FLAG_HAS_RAY_CAST_PHYSICS = 0x200000,
		PARTICLE_STATE_DEF_FLAG_HAS_EMISSIVE_CURVE = 0x400000,
		PARTICLE_STATE_DEF_FLAG_HAS_INTENSITY_CURVE = 0x800000,
		PARTICLE_STATE_DEF_FLAG_USE_VECTOR_FIELDS = 0x1000000,
		PARTICLE_STATE_DEF_FLAG_INHERIT_PARENT_VELOCITY = 0x2000000,
		PARTICLE_STATE_DEF_FLAG_DRAW_WITH_VIEW_MODEL = 0x4000000,
		PARTICLE_STATE_DEF_FLAG_PLAY_SOUNDS = 0x8000000,
		PARTICLE_STATE_DEF_FLAG_HAS_CAMERA_OFFSET_POSITION_ONLY = 0x10000000,
		PARTICLE_STATE_DEF_FLAG_ON_IMPACT_USE_SURFACE_TYPE = 0x20000000,
		PARTICLE_STATE_DEF_FLAG_IS_SPRITE = 0x40000000,
		PARTICLE_STATE_DEF_FLAG_HAS_TRANS_SHADOWS = 0x80000000,
		PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS = 0x0,
		PARTICLE_STATE_DEF_FLAG_BLOCKS_SIGHT = 0x0,
		PARTICLE_STATE_DEF_FLAG_HANDLE_TIME_IN_STATE = 0x0,
		PARTICLE_STATE_DEF_FLAG_SCALE_BY_DISTANCE = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_VECTOR_FIELD_CURVE = 0x0,
		PARTICLE_STATE_DEF_FLAG_USE_LOCAL_VECTOR_FIELDS_ONLY = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_SHADER_CURVE = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_SIZE_CURVE = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_SIZE_LERP = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_TEMPERATURE_CURVE = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_LIGHTING_FRACTION_CURVE = 0x0,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_CURVE = 0x30,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D = 0x50,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D = 0xA0,
		PARTICLE_STATE_DEF_FLAG_HAS_ROTATION = 0xF0,
		PARTICLE_STATE_DEF_FLAG_HAS_MIRROR_TEXTURE = 0x7800,
		PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE = 0x300,
		PARTICLE_STATE_DEF_FLAG_REQUIRES_WORLD_COLLISION = 0x200400,
	};

	enum PARTICLE_SYSTEM_DEF_FLAG
	{
		PARTICLE_SYSTEM_DEF_FLAG_HAS_SPRITES = 0x1,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES = 0x2,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_LIGHTS = 0x4,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_PHYSICS_HEAVY = 0x8,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_PHYSICS_LIGHT = 0x10,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_SCRIPTED_INPUTS = 0x20,
		PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY = 0x40,
		PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY_OVERRIDE_POS = 0x80,
		PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_OWNER_ENTITY_DEATH = 0x100,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_TRANS_SHADOWS = 0x200,
		PARTICLE_SYSTEM_DEF_FLAG_ALIGN_TO_SUN = 0x400,
		PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_KILLCAM_TRANSITION = 0x800,
		PARTICLE_SYSTEM_DEF_FLAG_CANNOT_PRE_ROLL = 0x1000,
		PARTICLE_SYSTEM_DEF_FLAG_ALWAYS_UPDATE_BONE_INDEX = 0x2000,
		PARTICLE_SYSTEM_DEF_FLAG_AFFECTS_GAMEPLAY = 0x4000,
		PARTICLE_SYSTEM_DEF_FLAG_UPDATE_CHILDREN_AFTER_BOLTING = 0x8000,
		PARTICLE_SYSTEM_DEF_FLAG_PRE_ROLL_CHILD_EFFECTS = 0x10000,
		PARTICLE_SYSTEM_DEF_FLAG_KILL_ON_KILLCAM_ENTITY_TRANSITION = 0x20000,
		PARTICLE_SYSTEM_DEF_FLAG_KILL_STOPPED_INFINITE_EFFECTS = 0x40000,
		PARTICLE_SYSTEM_DEF_FLAG_PARENT_UPDATES_CHILD = 0x80000,
		PARTICLE_SYSTEM_DEF_FLAG_STOP_ON_DYNAMIC_BOLT_DEATH = 0x100000,
		PARTICLE_SYSTEM_DEF_FLAG_DISABLE_IN_SPLIT_SCREEN = 0x200000,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_EMITTER_GROUP_IDS = 0x400000,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_EMITTER_NVG_OR_THERMAL = 0x800000,
		PARTICLE_SYSTEM_DEF_FLAG_HAS_INSTANCE_POOL = 0x1000000,
		PARTICLE_SYSTEM_DEF_FLAG_HIDE_IF_BONE_IS_HIDDEN = 0x2000000,
		PARTICLE_SYSTEM_DEF_FLAG_FOV_CULL = 0x4000000,
		PARTICLE_SYSTEM_DEF_FLAG_USE_OCCLUSION_QUERY_OVERRIDE_LOC = 0x8000000,
	};

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
		PARTICLE_MODULE_INIT_LIGHT_OMNI = 0x8,
		PARTICLE_MODULE_INIT_LIGHT_SPOT = 0x9,
		PARTICLE_MODULE_INIT_MATERIAL = 0xA,
		PARTICLE_MODULE_INIT_MIRROR_TEXTURE = 0xB,
		PARTICLE_MODULE_INIT_MODEL = 0xC,
		PARTICLE_MODULE_INIT_OCCLUSION_QUERY = 0xD,
		PARTICLE_MODULE_INIT_ORIENTED_SPRITE = 0xE,
		PARTICLE_MODULE_INIT_PARTICLE_SIM = 0xF,
		PARTICLE_MODULE_INIT_PLAYER_FACING = 0x10,
		PARTICLE_MODULE_INIT_RELATIVE_VELOCITY = 0x11,
		PARTICLE_MODULE_INIT_ROTATION = 0x12,
		PARTICLE_MODULE_INIT_ROTATION_3D = 0x13,
		PARTICLE_MODULE_INIT_RUNNER = 0x14,
		PARTICLE_MODULE_INIT_SOUND = 0x15,
		PARTICLE_MODULE_INIT_SPAWN = 0x16,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX = 0x17,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER = 0x18,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID = 0x19,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH = 0x1A,
		PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE = 0x1B,
		PARTICLE_MODULE_INIT_TAIL = 0x1C,
		PARTICLE_MODULE_INIT_VECTOR_FIELD = 0x1D,
		PARTICLE_MODULE_INIT_VOLUMETRIC = 0x1E,
		PARTICLE_MODULE_ATTRACTOR = 0x1F,
		PARTICLE_MODULE_COLOR_GRAPH = 0x20,
		PARTICLE_MODULE_COLOR_LERP = 0x21,
		PARTICLE_MODULE_EMISSION_GRAPH = 0x22,
		PARTICLE_MODULE_EMISSIVE_GRAPH = 0x23,
		PARTICLE_MODULE_FORCE = 0x24,
		PARTICLE_MODULE_GRAVITY = 0x25,
		PARTICLE_MODULE_INTENSITY_GRAPH = 0x26,
		PARTICLE_MODULE_TEMPERATURE_GRAPH = 0x27,
		PARTICLE_MODULE_PHYSICS_LIGHT = 0x28,
		PARTICLE_MODULE_PHYSICS_RAY_CAST = 0x29,
		PARTICLE_MODULE_POSITION_GRAPH = 0x2A,
		PARTICLE_MODULE_ROTATION_GRAPH = 0x2B,
		PARTICLE_MODULE_ROTATION_GRAPH_3D = 0x2C,
		PARTICLE_MODULE_SIZE_GRAPH = 0x2D,
		PARTICLE_MODULE_SIZE_LERP = 0x2E,
		PARTICLE_MODULE_VELOCITY_GRAPH = 0x2F,
		PARTICLE_MODULE_TEST_BIRTH = 0x30,
		PARTICLE_MODULE_TEST_DEATH = 0x31,
		PARTICLE_MODULE_TEST_GRAVITY = 0x32,
		PARTICLE_MODULE_TEST_IMPACT = 0x33,
		PARTICLE_MODULE_TEST_POS = 0x34,
		PARTICLE_MODULE_TEST_TIME_IN_STATE = 0x35,
		PARTICLE_MODULE_COUNT = 0x36,
		PARTICLE_MODULE_INVALID = 0xFF,
	};

	struct float4
	{
		float v[4];
	};

	struct ParticleFloatRange
	{
		float min;
		float max;
	};

	struct ParticleIntRange
	{
		int min;
		int max;
	};

	struct ParticlePhysicsFXData
	{
		PhysicsFXPipeline* physicsFXPipeline;
		PhysicsFXShape* physicsFXShape;
	};

	struct ParticleMarkVisuals
	{
		Material* materials[3];
	};

	union ParticleLinkedAssetDef
	{
		Material* material;
		XModel* model;
		ParticleSystemDef* particleSystem;
		ParticlePhysicsFXData physicsFXData;
		FxParticleSimAnimation* particleSim;
		ParticleMarkVisuals decal;
		const char* sound;
		VectorField* vectorField;
		GfxLightDef* lightDef;
		char pad[32];
	}; assert_sizeof(ParticleLinkedAssetDef, 32);

	struct ParticleModule
	{
		ParticleModuleType type;
		unsigned int m_flags;
	};

	struct ParticleCurveControlPointDef
	{
		float time;
		float value;
		float invTimeDelta;
		unsigned int pad[1];
	};

	struct ParticleCurveDef
	{
		ParticleCurveControlPointDef* controlPoints;
		int numControlPoints;
		float scale;
	};

	struct ParticleModuleInitAtlas : ParticleModule
	{
		int m_startFrame;
		int m_loopCount;
		bool m_randomIndex;
		bool m_playOverLife;
	}; assert_sizeof(ParticleModuleInitAtlas, 20);

	struct ParticleModuleInitAttributes : ParticleModule
	{
		bool m_useNonUniformInterpolationForColor;
		bool m_useNonUniformInterpolationForSize;
		char m_pad[6];
		float4 m_sizeMin;
		float4 m_sizeMax;
		float4 m_colorMin;
		float4 m_colorMax;
		float4 m_velocityMin;
		float4 m_velocityMax;
	}; assert_sizeof(ParticleModuleInitAttributes, 112);

	enum ParticleModuleInitBeamFlags
	{
		PARTICLE_MODULE_INIT_BEAM_FLAG_TILE = 0x1,
		PARTICLE_MODULE_INIT_BEAM_FLAG_USE_CURVE_POINTS = 0x2,
		PARTICLE_MODULE_INIT_BEAM_FLAG_CAMERA_FACING = 0x4,
	};

	struct ParticleLinkedAssetListDef
	{
		ParticleLinkedAssetDef* assetList;
		int numAssets;
	};

	struct ParticleModuleInitBeam : ParticleModule
	{
		ParticleModuleInitBeamFlags m_beamFlags;
		float m_splitDistance;
		float4 m_startPos;
		float4 m_endPos;
		float4 m_offset;
		float4 m_curvePoint1;
		float4 m_curvePoint2;
		ParticleLinkedAssetListDef m_linkedAssetList;
		float m_scrollRateUV;
		float m_scrollRateSpiralGraph;
		float m_spiralRepeatDistance;
	}; assert_sizeof(ParticleModuleInitBeam, 128);

	struct ParticleModuleInitCameraOffset : ParticleModule
	{
		unsigned int m_pad[2];
	}; assert_sizeof(ParticleModuleInitCameraOffset, 16);

	struct ParticleModuleInitCloud : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleCurveDef curves[2];
	}; assert_sizeof(ParticleModuleInitCloud, 48);

	struct ParticleModuleInitDecal : ParticleModule
	{
		unsigned __int16 m_fadeInTime;
		unsigned __int16 m_fadeOutTime;
		unsigned __int16 m_stoppableFadeOutTime;
		unsigned __int16 m_lerpWaitTime;
		float4 m_lerpColor;
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitDecal, 48);
	assert_offsetof(ParticleModuleInitDecal, m_linkedAssetList, 32);

	struct ParticleModuleInitFlare : ParticleModule
	{
		unsigned int m_pad[1];
		float m_position;
		float4 m_direction;
		int m_angularRotCount;
		unsigned int m_flareFlags;
		ParticleFloatRange m_depthScaleRange;
		ParticleFloatRange m_depthScaleValue;
		ParticleFloatRange m_radialRot;
		ParticleFloatRange m_radialScaleX;
		ParticleFloatRange m_radialScaleY;
		ParticleCurveDef m_curves[4];
	}; assert_sizeof(ParticleModuleInitFlare, 144);
	assert_offsetof(ParticleModuleInitFlare, m_curves, 80);

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
		char m_pad[3];
		unsigned int m_pad2[1];
	}; assert_sizeof(ParticleModuleInitGeoTrail, 64);

	struct ParticleModuleInitLightOmni : ParticleModule
	{
		ParticleLinkedAssetListDef m_linkedAssetList;
		float m_fovOuter;
		float m_fovInner;
		float m_bulbRadius;
		float m_bulbLength;
		bool m_disableVolumetric;
		bool m_disableShadowMap;
		bool m_disableDynamicShadows;
		bool m_scriptScale;
	}; assert_sizeof(ParticleModuleInitLightOmni, 48);

	struct ParticleModuleInitLightSpot : ParticleModule
	{
		float m_fovOuter;
		float m_fovInner;
		float m_bulbRadius;
		float m_bulbLength;
		float m_distanceFalloff;
		//float m_fovCollimation;
		float m_brightness;
		float m_intensityUV;
		float m_intensityIR;
		//float m_intensityHeat;
		float m_shadowSoftness;
		float m_shadowBias;
		float m_shadowArea;
		float m_shadowNearPlane;
		float m_toneMappingScaleFactor;
		bool m_disableVolumetric;
		bool m_disableShadowMap;
		bool m_disableDynamicShadows;
		bool m_scriptScale;
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitLightSpot, 80);
	assert_offsetof(ParticleModuleInitLightSpot, m_linkedAssetList, 64);

	struct ParticleModuleInitMaterial : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitMaterial, 32);

	enum ParticleMirrorTextureType
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
	}; assert_sizeof(ParticleModuleInitMirrorTexture, 16);

	struct ParticleModuleInitModel : ParticleModule
	{
		bool m_usePhysics;
		bool m_motionBlurHQ;
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitModel, 32);
	assert_offsetof(ParticleModuleInitModel, m_linkedAssetList, 16);

	struct ParticleModuleInitOcclusionQuery : ParticleModule
	{
		float m_fadeInTime;
		float m_fadeOutTime;
		ParticleFloatRange m_scale;
		vec2_t m_worldSize;
	}; assert_sizeof(ParticleModuleInitOcclusionQuery, 32);

	struct ParticleModuleInitOrientedSprite : ParticleModule
	{
		unsigned int m_pad[2];
		float4 m_orientationQuat;
	}; assert_sizeof(ParticleModuleInitOrientedSprite, 32);

	struct ParticleModuleInitParticleSim : ParticleModule
	{
		ParticleLinkedAssetListDef m_linkedAssetList;
		ParticleFloatRange m_scaleFactor;
		bool m_holdLastFrame;
		unsigned int m_pad[3];
	}; assert_sizeof(ParticleModuleInitParticleSim, 48);

	struct ParticleModuleInitPlayerFacing : ParticleModule
	{
		unsigned int m_pad[2];
	}; assert_sizeof(ParticleModuleInitPlayerFacing, 16);

	enum ParticleRelativeVelocityType
	{
		PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL = 0x0,
		PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD = 0x1,
		PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN = 0x2,
		PARTICLE_RELATIVE_VELOCITY_TYPE_COUNT = 0x3,
		PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO = 0x3,
		PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO = 0x4,
		PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN_WITH_BOLT_INFO = 0x5,
	};

	struct __declspec(align(4)) ParticleModuleInitRelativeVelocity : ParticleModule
	{
		ParticleRelativeVelocityType m_velocityType;
		bool m_useBoltInfo;
	}; assert_sizeof(ParticleModuleInitRelativeVelocity, 16);

	struct ParticleModuleInitRotation : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleFloatRange m_rotationAngle;
		ParticleFloatRange m_rotationRate;
	}; assert_sizeof(ParticleModuleInitRotation, 32);

	struct ParticleModuleInitRotation3D : ParticleModule
	{
		unsigned int m_pad[2];
		float4 m_rotationAngleMin;
		float4 m_rotationAngleMax;
		float4 m_rotationRateMin;
		float4 m_rotationRateMax;
	}; assert_sizeof(ParticleModuleInitRotation3D, 80);

	struct ParticleModuleInitRunner : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitRunner, 32);

	struct ParticleModuleInitSound : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitSound, 32);

	struct ParticleModuleInitSpawn : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleCurveDef m_curves[1];
	}; assert_sizeof(ParticleModuleInitSpawn, 32);

	struct ParticleModuleInitSpawnShape : ParticleModule
	{
		char m_axisFlags[1];
		char m_spawnFlags[1];
		char m_normalAxis[1];
		char m_spawnType[1];
		float m_volumeCubeRoot;
		//float4 m_calculationOffset;
		float4 m_offset;
	};

	struct ParticleModuleInitSpawnShapeBox : ParticleModuleInitSpawnShape
	{
		bool m_useBeamInfo;
		char m_pad[15];
		float4 m_dimensionsMin;
		float4 m_dimensionsMax;
	}; assert_sizeof(ParticleModuleInitSpawnShapeBox, 80);

	struct ParticleModuleInitSpawnShapeCylinder : ParticleModuleInitSpawnShape
	{
		bool m_hasRotation;
		bool m_rotateCalculatedOffset;
		float m_halfHeight;
		ParticleFloatRange m_radius;
		float4 m_directionQuat;
		unsigned int m_pad[4];
	}; assert_sizeof(ParticleModuleInitSpawnShapeCylinder, 80);

	struct ParticleModuleInitSpawnShapeEllipsoid : ParticleModuleInitSpawnShape
	{
		float4 m_radiusMin;
		float4 m_radiusMax;
		unsigned int m_pad[4];
	}; assert_sizeof(ParticleModuleInitSpawnShapeEllipsoid, 80);

	struct ParticleSpawnMeshAssetDef
	{
		unsigned int m_numVertsTotal;
		unsigned int m_numTrisTotal;
		unsigned int m_pad[2];
	};

	struct ParticleModuleInitSpawnShapeMesh : ParticleModuleInitSpawnShape
	{
		unsigned int m_numMeshAssets;
		ParticleLinkedAssetListDef m_linkedAssetList;
		ParticleSpawnMeshAssetDef* m_meshAssetData;
		unsigned int m_pad[3];
	}; assert_sizeof(ParticleModuleInitSpawnShapeMesh, 80);
	assert_offsetof(ParticleModuleInitSpawnShapeMesh, m_numMeshAssets, 32);

	struct ParticleModuleInitSpawnShapeSphere : ParticleModuleInitSpawnShape
	{
		unsigned int m_pad[2];
		ParticleFloatRange m_radius;
		unsigned int m_pad2[4];
	}; assert_sizeof(ParticleModuleInitSpawnShapeSphere, 64);

	struct ParticleModuleInitTail : ParticleModule
	{
		unsigned __int16 m_averagePastVelocities;
		unsigned __int16 m_maxParentSpeed;
		bool m_tailLeading;
		bool m_scaleWithVelocity;
		bool m_rotateAroundPivot;
	}; assert_sizeof(ParticleModuleInitTail, 16);

	struct ParticleModuleInitVectorField : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModuleInitVectorField, 32);

	struct ParticleModuleInitVolumetric : ParticleModule
	{
		float m_density;
		float m_falloff;
		float m_noiseAmplitude;
		unsigned int m_noiseMode;
		unsigned int m_pad[2];
	}; assert_sizeof(ParticleModuleInitVolumetric, 32);

	struct ParticleModuleAttractor : ParticleModule
	{
		unsigned int m_pad[3];
		bool m_killWhenNear;
		float m_forceMag;
		float m_nearDistanceSq;
		float4 m_attractPoint;
	}; assert_sizeof(ParticleModuleAttractor, 48);

	struct ParticleModuleColorGraph : ParticleModule
	{
		bool firstCurve;
		char m_pad[3];
		bool m_modulateColorByAlpha;
		ParticleCurveDef m_curves[8];
	}; assert_sizeof(ParticleModuleColorGraph, 144);

	struct ParticleModuleColorLerp : ParticleModule
	{
		unsigned int m_pad[2];
		float4 m_colorBegin;
		float4 m_colorEnd;
	}; assert_sizeof(ParticleModuleColorLerp, 48);

	struct ParticleModuleEmissionGraph : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleEmissionGraph, 48);

	struct ParticleModuleEmissiveGraph : ParticleModule
	{
		bool firstCurve;
		char m_pad[7];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleEmissiveGraph, 48);
	assert_offsetof(ParticleModuleEmissiveGraph, m_curves, 16);

	struct ParticleModuleForce : ParticleModule
	{
		unsigned int m_pad[2];
		float4 m_forceMin;
		float4 m_forceMax;
	}; assert_sizeof(ParticleModuleForce, 48);

	struct ParticleModuleGravity : ParticleModule
	{
		ParticleFloatRange m_gravityPercentage;
	}; assert_sizeof(ParticleModuleGravity, 16);

	struct ParticleModuleIntensityGraph : ParticleModule
	{
		bool firstCurve;
		char m_pad[7];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleIntensityGraph, 48);

	struct ParticleModuleTemperatureGraph : ParticleModule
	{
		bool firstCurve;
		char m_pad[7];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleTemperatureGraph, 48);

	struct ParticleModulePhysicsLight : ParticleModule
	{
		bool m_ignoreEmitterOrientation;
		bool m_useSurfaceType;
		char m_pad[6];
		ParticleLinkedAssetListDef m_linkedAssetList;
	}; assert_sizeof(ParticleModulePhysicsLight, 32);

	struct ParticleModulePhysicsRayCast : ParticleModule
	{
		ParticleFloatRange m_bounce;
		Bounds m_bounds;
		bool m_useItemClip;
		bool m_useSurfaceType;
		bool m_collideWithWater;
		bool m_ignoreContentItem;
		char m_pad[3];
	}; assert_sizeof(ParticleModulePhysicsRayCast, 48);

	struct ParticleModulePositionGraph : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModulePositionGraph, 112);

	struct ParticleModuleRotationGraph : ParticleModule
	{
		bool m_useRotationRate;
		unsigned int m_pad[1];
		ParticleCurveDef m_curves[2];
	}; assert_sizeof(ParticleModuleRotationGraph, 48);

	struct ParticleModuleRotationGraph3D : ParticleModule
	{
		bool m_useRotationRate;
		unsigned int m_pad[1];
		ParticleCurveDef m_curves[6];
	}; assert_sizeof(ParticleModuleRotationGraph3D, 112);

	struct ParticleModuleSizeGraph : ParticleModule
	{
		bool firstCurve;
		char m_pad[7];
		ParticleCurveDef m_curves[6];
		float4 m_sizeBegin;
		float4 m_sizeEnd;
	}; assert_sizeof(ParticleModuleSizeGraph, 144);

	struct ParticleModuleSizeLerp : ParticleModule
	{
		unsigned int m_pad[2];
		float4 m_sizeBegin;
		float4 m_sizeEnd;
	}; assert_sizeof(ParticleModuleSizeLerp, 48);

	struct ParticleModuleVelocityGraph : ParticleModule
	{
		unsigned int m_pad[2];
		ParticleCurveDef m_curves[6];
		float4 m_velocityBegin;
		float4 m_velocityEnd;
	}; assert_sizeof(ParticleModuleVelocityGraph, 144);

	struct ParticleModuleTestEventHandlerData
	{
		unsigned int m_nextState;
		ParticleLinkedAssetListDef m_linkedAssetList;
		bool m_kill;
		unsigned int m_pad[1];
	};

	struct ParticleModuleTest : ParticleModule
	{
		unsigned __int16 m_moduleIndex;
		char m_useOrientationOptions[1];
		char m_useScaleOptions[1];
		char m_useVelocityOptions[1];
		ParticleModuleTestEventHandlerData m_eventHandlerData;
	}; assert_sizeof(ParticleModuleTest, 48);
	assert_offsetof(ParticleModuleTest, m_eventHandlerData, 16);

	struct ParticleModuleTestBirth : ParticleModuleTest
	{
	};

	struct ParticleModuleTestDeath : ParticleModuleTest
	{
	};

	struct ParticleModuleTestGravity : ParticleModuleTest
	{
	};

	struct ParticleModuleTestImpact : ParticleModuleTest
	{
	};

	struct ParticleModuleTestPos : ParticleModuleTest
	{
	};

	struct ParticleModuleTestTimeInState : ParticleModuleTest
	{
	};

	union ParticleModuleTypeDef
	{
		ParticleModule moduleBase;
		ParticleModuleInitAtlas initAtlas;
		ParticleModuleInitAttributes initAttributes;
		ParticleModuleInitBeam initBeam;
		ParticleModuleInitCameraOffset initCameraOffset;
		ParticleModuleInitCloud initCloud; // maybe different
		ParticleModuleInitDecal initDecal; // maybe wrong fields
		ParticleModuleInitFlare initFlare;
		ParticleModuleInitGeoTrail initGeoTrail;
		ParticleModuleInitLightOmni initLightOmni; // maybe wrong fields
		ParticleModuleInitLightSpot initLightSpot; // maybe wrong fields
		ParticleModuleInitMaterial initMaterial;
		ParticleModuleInitMirrorTexture initMirrorTexture;
		ParticleModuleInitModel initModel;
		ParticleModuleInitOcclusionQuery initOcclusionQuery; // maybe wrong fields
		ParticleModuleInitOrientedSprite initOrientedSprite;
		ParticleModuleInitParticleSim initParticleSim;
		ParticleModuleInitPlayerFacing initPlayerFacing;
		ParticleModuleInitRelativeVelocity initRelativeVelocity;
		ParticleModuleInitRotation initRotation;
		ParticleModuleInitRotation3D initRotation3D;
		ParticleModuleInitRunner initRunner; // maybe wrong
		ParticleModuleInitSound initSound;
		ParticleModuleInitSpawn initSpawn;
		ParticleModuleInitSpawnShapeBox initSpawnShapeBox; // maybe wrong fields
		ParticleModuleInitSpawnShapeCylinder initSpawnShapeCylinder; // maybe wrong fields
		ParticleModuleInitSpawnShapeEllipsoid initSpawnShapeEllipsoid; // maybe wrong
		ParticleModuleInitSpawnShapeMesh initSpawnShapeMesh;  // maybe wrong
		ParticleModuleInitSpawnShapeSphere initSpawnShapeSphere;  // maybe wrong
		ParticleModuleInitTail initTail;
		ParticleModuleInitVectorField initVectorField; // maybe wrong
		ParticleModuleInitVolumetric initVolumetric;
		ParticleModuleAttractor attractor;
		ParticleModuleColorGraph colorGraph;
		ParticleModuleColorLerp colorLerp;
		ParticleModuleEmissionGraph emissionGraph;
		ParticleModuleEmissiveGraph emissiveGraph;
		ParticleModuleForce force;
		ParticleModuleGravity gravity;
		ParticleModuleIntensityGraph intensityGraph;
		ParticleModuleTemperatureGraph temperatureGraph;
		ParticleModulePhysicsLight physicsLight;
		ParticleModulePhysicsRayCast physicsRayCast;
		ParticleModulePositionGraph positionGraph;
		ParticleModuleRotationGraph rotationGraph;
		ParticleModuleRotationGraph3D rotationGraph3D;
		ParticleModuleSizeGraph sizeGraph;
		ParticleModuleSizeLerp sizeLerp;
		ParticleModuleVelocityGraph velocityGraph;
		ParticleModuleTestBirth testBirth;
		ParticleModuleTestDeath testDeath;
		ParticleModuleTestGravity testGravity;
		ParticleModuleTestImpact testImpact;
		ParticleModuleTestPos testPos;
		ParticleModuleTestTimeInState testTimeInState;
	};

	struct ParticleModuleDef
	{
		ParticleModuleType type;
		unsigned int m_pad[3];
		ParticleModuleTypeDef moduleData;
	}; assert_sizeof(ParticleModuleDef, 160);

	enum ParticleGravityOptions
	{
		PARTICLE_GRAVITY_OPTION_NONE = 0x0,
		PARTICLE_GRAVITY_OPTION_GRAVITY_ONLY = 0x1,
		PARTICLE_GRAVITY_OPTION_GRAVITY_NEVER = 0x2,
		PARTICLE_GRAVITY_OPTION_COUNT = 0x3,
	};

	enum ParticleInstancePoolAction
	{
		PARTICLE_INSTANCE_POOL_ACTION_NO_SPAWN = 0x0,
		PARTICLE_INSTANCE_POOL_ACTION_KILL_OLDEST_AND_SPAWN = 0x1,
		PARTICLE_INSTANCE_POOL_ACTION_COUNT = 0x2,
	};

	enum ParticleDataFlags
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

	struct ParticleModuleGroupDef
	{
		ParticleModuleDef* moduleDefs;
		int numModules;
		bool disabled;
	};

	struct ParticleStateDef
	{
		ParticleModuleGroupDef* moduleGroupDefs; // size: 3
		unsigned int elementType;
		unsigned __int64 flags;
		unsigned int pad[2];
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
		unsigned int flags;
		ParticleGravityOptions gravityOptions;
		//int groupIDs[4];
		ParticleFloatRange emitByDistanceDensity;
		unsigned int instancePool;
		unsigned int soloInstanceMax;
		ParticleInstancePoolAction instanceAction;
		ParticleDataFlags m_dataFlags;
		ParticleFloatRange particleSpawnShapeRange;
		unsigned int pad[1];
	}; assert_sizeof(ParticleEmitterDef, 144);

	enum ParticleScriptedInputNodeType
	{
		PARTICLE_SCRIPTED_INPUT_NODE_INVALID = 0xFFFFFFFF,
		PARTICLE_SCRIPTED_INPUT_NODE_KVP = 0x0,
		PARTICLE_SCRIPTED_INPUT_NODE_EMITTER_DISABLE = 0x1,
		PARTICLE_SCRIPTED_INPUT_NODE_COUNT = 0x2,
	};

	struct ParticleScriptedInputNode
	{
		ParticleScriptedInputNodeType m_type;
		unsigned int m_flags;
		unsigned int m_emitterIndex;
		unsigned int m_pad[5];
	};

	enum ParticleScriptedInputKey
	{
	};

	struct ParticleScriptedInputRecord
	{
		ParticleScriptedInputKey key;
		bool dirty;
		bool value;
		unsigned int pad[2];
	};

	struct ParticleScriptedInputNodeKVP : ParticleScriptedInputNode
	{
		ParticleScriptedInputRecord m_inputRecord;
	};

	struct ParticleScriptedInputNodeEmitterDisable : ParticleScriptedInputNode
	{
	};

	union ParticleScriptedInputNodeDef // maybe wrong
	{
		ParticleScriptedInputNode nodeBase;
		ParticleScriptedInputNodeKVP KVP;
		ParticleScriptedInputNodeEmitterDisable emitterDisable;
	}; assert_sizeof(ParticleScriptedInputNodeDef, 48);

	enum ParticlePhaseOptions
	{
		PARTICLE_PHASE_OPTION_PHASE_NEVER = 0x0,
		PARTICLE_PHASE_OPTION_PHASE_ONLY = 0x1,
		PARTICLE_PHASE_OPTION_ALWAYS = 0x2,
		PARTICLE_PHASE_OPTION_COUNT = 0x3,
	};

	struct ParticleSystemDef
	{
		const char* name;
		ParticleEmitterDef* emitterDefs;
		ParticleScriptedInputNodeDef* scriptedInputNodeDefs;
		int version;
		int numEmitters;
		int numScriptedInputNodes;
		unsigned int flags;
		int occlusionOverrideEmitterIndex;
		ParticlePhaseOptions phaseOptions;
		float drawFrustumCullRadius;
		float updateFrustumCullRadius;
		float sunDistance;
		int preRollMSec;
		float4 editorPosition;
		float4 editorRotation;
		float4 gameTweakPosition;
		float4 gameTweakRotation;
	}; assert_sizeof(ParticleSystemDef, 0x80);

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxElemVec3Range
	{
		float base[3];
		float amplitude[3];
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
		float color[4];
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

	struct FxElemMarkVisuals
	{
		Material* materials[3];
	};

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
		GfxLightDef* lightDef;
		FxParticleSimAnimation* particleSimAnimation;
	};

	union FxElemDefVisuals
	{
		FxElemMarkVisuals* markArray;
		FxElemVisuals* array;
		FxElemVisuals instance;
	};

	struct FxTrailVertex
	{
		vec2_t pos;
		vec2_t normal;
		vec2_t texCoord;
	};

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		float invSplitDist;
		float invSplitArcDist;
		float invSplitTime;
		int vertCount;
		FxTrailVertex* verts;
		int indCount;
		unsigned short* inds;
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
		vec3_t dir;
		int intensityXIntervalCount;
		int intensityYIntervalCount;
		int srcCosIntensityIntervalCount;
		int srcCosScaleIntervalCount;
		float* intensityX;
		float* intensityY;
		float* srcCosIntensity;
		float* srcCosScale;
	};

	union FxElemExtendedDefPtr
	{
		FxTrailDef* trailDef;
		FxSparkFountainDef* sparkFountainDef;
		FxSpotLightDef* spotLightDef;
		FxFlareDef* flareDef;
		void* unknownDef;
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
		FX_ELEM_TYPE_DECAL = 15,
		FX_ELEM_TYPE_RUNNER = 16,
		FX_ELEM_TYPE_VECTORFIELD = 17,
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
		FX_ELEM_EMIT_BOLT = 0x80000000,
		FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
		FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
		FX_ELEM_NODRAW_IN_THERMAL_VIEW = 0x20000,
		FX_ELEM_THERMAL_MASK = 0x22000,
		FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME = 0x40000,
		FX_ELEM_RECEIVE_DYNAMIC_LIGHT = 0x80000,
		FX_ELEM_VOLUMETRIC_TRAIL = 0x100000,
		FX_ELEM_USE_COLLISION = 0x200000,
		FX_ELEM_USE_VECTORFIELDS = 0x400000,
		FX_ELEM_NO_SURFACE_HDR_SCALAR = 0x800000,
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

	enum FxElemDefExtraFlags : std::uint32_t
	{

	};

	struct FxElemDef
	{
		int flags;
		int flags2;
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
		char unk[2];
		unsigned char elemType;
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
		int randomSeed;
		float litMaxColorChangePerSec;
		float litUnlitBlendFactor;
	}; assert_sizeof(FxElemDef, 304);
	assert_offsetof(FxElemDef, elemType, 182);
	assert_offsetof(FxElemDef, visualCount, 183);
	assert_offsetof(FxElemDef, visuals, 208);
	assert_offsetof(FxElemDef, extended, 280);

	struct FxEffectDef
	{
		const char* name;
		int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		float elemMaxRadius;
		float occlusionQueryDepthBias;
		int occlusionQueryFadeIn;
		int occlusionQueryFadeOut;
		FxFloatRange occlusionQueryScaleRange;
		FxElemDef* elemDefs;
	}; assert_sizeof(FxEffectDef, 0x40);

	struct FxImpactEntry
	{
		FxCombinedDef nonflesh[64];
		FxCombinedDef flesh[23];
	};

	struct FxImpactTable
	{
		const char* name;
		FxImpactEntry* table; // size: 49
	};

	struct SurfaceFxEntry
	{
		FxCombinedDef surfaceEffect[64];
	};

	struct SurfaceFxTable
	{
		const char* name;
		SurfaceFxEntry* table; // size: 6
	};

	struct RawFile
	{
		const char* name;
		int compressedLen;
		int len;
		const char* buffer;
	};

	struct ScriptFile
	{
		const char* name;
		int compressedLen;
		int len;
		int bytecodeLen;
		const char* buffer;
		char* bytecode;
	};

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

	enum LbColType
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

	enum LbAggType
	{
		LBAGG_TYPE_MIN = 0x0,
		LBAGG_TYPE_MAX = 0x1,
		LBAGG_TYPE_SUM = 0x2,
		LBAGG_TYPE_LAST = 0x3,
		LBAGG_TYPE_COUNT = 0x4,
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
	};

	enum LbUpdateType
	{
		LBUPDATE_TYPE_NORMAL = 0x0,
		LBUPDATE_TYPE_RANK = 0x1,
		LBUPDATE_TYPE_COMBINE = 0x2,
		LBUPDATE_TYPE_VIRTUAL = 0x3,
		LBUPDATE_TYPE_COUNT = 0x4,
	};

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
	};

	struct LbVrColumnDef
	{
		const char* name;
		int id;
		int uiCalColX;
		int uiCalColY;
	};

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
	};

	struct StructuredDataEnumEntry
	{
		scr_string_t string;
		unsigned short index;
	};

	struct StructuredDataEnum
	{
		int entryCount;
		int reservedEntryCount;
		StructuredDataEnumEntry* entries;
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

	union StructuredDataTypeUnion
	{
		unsigned int stringDataLength;
		int enumIndex;
		int structIndex;
		int indexedArrayIndex;
		int enumedArrayIndex;
		int index;
	};

	struct StructuredDataType
	{
		StructuredDataTypeCategory type;
		StructuredDataTypeUnion u;
	};

	enum StructuredDataValidationType
	{
		VALIDATION_NONE = 0x0,
	};

	struct StructuredDataStructProperty
	{
		scr_string_t name;
		StructuredDataType type;
		unsigned int offset;
		StructuredDataValidationType validation;
	};

	struct StructuredDataStruct
	{
		int propertyCount;
		StructuredDataStructProperty* properties;
		int size;
		unsigned int bitOffset;
	};

	struct StructuredDataIndexedArray
	{
		int arraySize;
		StructuredDataType elementType;
		unsigned int elementSize;
	};

	struct StructuredDataEnumedArray
	{
		int enumIndex;
		StructuredDataType elementType;
		unsigned int elementSize;
	};

	struct StructuredDataDef
	{
		int version;
		unsigned int formatChecksum;
		int enumCount;
		StructuredDataEnum* enums;
		int structCount;
		StructuredDataStruct* structs;
		int indexedArrayCount;
		StructuredDataIndexedArray* indexedArrays;
		int enumedArrayCount;
		StructuredDataEnumedArray* enumedArrays;
		StructuredDataType rootType;
		unsigned int size;
	};

	struct StructuredDataDefSet
	{
		const char* name;
		unsigned int defCount;
		StructuredDataDef* defs;
	};

	struct DDLMember
	{
		const char* name;
		int index;
		void* parent;
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
		int permission;
	};

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

	struct DDLEnum
	{
		const char* name;
		int memberCount;
		const char** members;
		DDLHashTable hashTable;
	};

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
		bool paddingUsed;
	};

	struct DDLFile
	{
		char* name;
		DDLDef* ddlDef;
	};

	struct TracerDef
	{
		const char* name;
		Material* material;
		unsigned int drawInterval;
		int unk1;
		float unk2;
		float speed;
		float beamLength;
		float beamWidth;
		float screwRadius;
		float screwDist;
		vec4_t colors[5];
	}; assert_sizeof(TracerDef, 0x80);

	struct VehicleDef;

	struct AddonMapEnts;

	enum NetConstStringType
	{
	};

	enum NetConstStringSource
	{
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

	struct LuaFile
	{
		const char* name;
		int len;
		char strippingType;
		const char* buffer;
	};

	struct ScriptableEventDef;
	struct ScriptablePartDef;

	struct ScriptableEventBaseDef
	{
		const char* name;
		int flags;
	}; assert_sizeof(ScriptableEventBaseDef, 16);

	enum Scriptable_EventType
	{
		Scriptable_EventType_StateChange = 0x0,
		Scriptable_EventType_Wait = 0x1,
		Scriptable_EventType_Random = 0x2,
		Scriptable_EventType_Script = 0x3,
		Scriptable_EventType_Model = 0x4,
		Scriptable_EventType_Collision = 0x5,
		Scriptable_EventType_Animation = 0x6,
		Scriptable_EventType_HideShowBone = 0x7,
		Scriptable_EventType_NoteTrack = 0x8,
		Scriptable_EventType_ChunkDynent = 0x9,
		Scriptable_EventType_SpawnDynent = 0xA,
		Scriptable_EventType_PFX = 0xB,
		Scriptable_EventType_Sound = 0xC,
		Scriptable_EventType_Explosion = 0xD,
		Scriptable_EventType_Light = 0xE,
		Scriptable_EventType_Sun = 0xF,
		Scriptable_EventType_Rumble = 0x10,
		Scriptable_EventType_Screenshake = 0x11,
		Scriptable_EventType_PartDamage = 0x12,
		Scriptable_EventType_SetMayhem = 0x13,
		Scriptable_EventType_PlayMayhem = 0x14,
		Scriptable_EventType_ViewmodelShaderParam = 0x15,
		Scriptable_EventType_ViewmodelChangeImage = 0x16,
		Scriptable_EventType_ClientViewSelector = 0x17,
		Scriptable_EventType_TeamSelector = 0x18,
		Scriptable_EventType_AddModel = 0x19,
		Scriptable_EventType_ApplyForce = 0x1A,
		Scriptable_EventType_CompassIcon = 0x1B,
		Scriptable_EventType_MaterialOverride = 0x1C,
		Scriptable_EventType_Count = 0x1D,
	};

	struct ScriptableEventAnonymousDef
	{
		ScriptableEventBaseDef* base;
		char buffer[144];
	};

	struct unk_1453E1B90
	{
		const char* name;
		char __pad0[8];
	}; assert_sizeof(unk_1453E1B90, 0x10);

	struct unk_1453E1BC0
	{
		const char* name;
	};

	struct unk_1453E1C00
	{
		const char* name;
	};

	struct unk_1453E1C20
	{
		const char* name;
		char __pad0[8];
	};

	struct unk_1453E1C70
	{
		const char* name;
		unsigned int count;
		unsigned int* val;
	};

	union unk_1453E1C80
	{
		unk_1453E1BC0 __1;
		unk_1453E1C00 __2;
		unk_1453E1C20 __3;
		unk_1453E1C70 __4;
	};

	enum ScriptablePartReferenceType
	{
		type_1453E1BC0,
		type_1453E1C00,
		type_1453E1C20,
		type_1453E1C70,
		unk_1453E1CC8_type_count,
	};

	struct ScriptablePartReference
	{
		unk_1453E1B90 unk01;
		int type;
		unk_1453E1C80 u;
	}; assert_sizeof(ScriptablePartReference, 0x30);

	struct ScriptableEventStateChangeDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventStateChangeDef, 64);

	struct ScriptableEventWaitDef
	{
		ScriptableEventBaseDef* base;
		float delayMin;
		float delayMax;
		unsigned __int16 eventStreamBufferOffsetServer;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventWaitDef, 24);

	struct ScriptableEventRandomDef
	{
		ScriptableEventBaseDef* base;
		float threshold;
		unsigned int eventACount;
		ScriptableEventDef* eventsA;
		unsigned int eventBCount;
		ScriptableEventDef* eventsB;
		unsigned __int16 eventStreamBufferOffsetServer;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventRandomDef, 48);

	struct ScriptableEventScriptDef
	{
		ScriptableEventBaseDef* base;
		const char* notification;
		scr_string_t scrNotification;
		int param;
		//bool doNotifyCallback;
	}; assert_sizeof(ScriptableEventScriptDef, 24);

	struct ScriptableEventModelDef
	{
		ScriptableEventBaseDef* base;
		XModel* model;
		char hudOutlineColor;
		bool hudOutlineActive;
		bool hudOutlineFill;
		bool neverMoves;
		bool dynamicSimulation;
		bool activatePhysics;
	}; assert_sizeof(ScriptableEventModelDef, 24);

	struct ScriptableEventCollisionDef
	{
		ScriptableEventBaseDef* base;
		const char* collmapName;
		bool aiObstacle;
		int clipmapCModelIndex;
		bool neverMoves;
		bool canPush;
		bool canTouch;
		bool destroyOnExit;
	}; assert_sizeof(ScriptableEventCollisionDef, 32);

	struct $6CB7272563F4458FB40A4A5E123C4ABA
	{
		unsigned __int16 index;
		unsigned __int16 tree;
	};

	union $754892263A8270AD7855E320C3BF2A51
	{
		$6CB7272563F4458FB40A4A5E123C4ABA __s0;
		unsigned __int64 linkPointer;
	};

	struct scr_anim_t
	{
		$754892263A8270AD7855E320C3BF2A51 ___u0;
	};

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
		unsigned __int16 eventStreamBufferOffsetClient;
		unsigned __int16 eventStreamBufferOffsetServer;
		scr_anim_t animationIndex[2];
	}; assert_sizeof(ScriptableEventAnimationDef, 80);

	struct ScriptableEventHideShowBoneDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		bool hide;
		bool allowMissingTag;
		bool hideShowChildren;
	}; assert_sizeof(ScriptableEventHideShowBoneDef, 24);

	struct ScriptableNoteTrackDef
	{
		int flags;
		const char* noteTrackName;
		scr_string_t scrNoteTrackName;
		unsigned int numEvents;
		ScriptableEventDef* events;
	}; assert_sizeof(ScriptableNoteTrackDef, 32);

	struct ScriptableEventNoteTrackDef
	{
		ScriptableEventBaseDef* base;
		unsigned int noteTrackCount;
		ScriptableNoteTrackDef* noteTracks;
	}; assert_sizeof(ScriptableEventNoteTrackDef, 24);

	struct ScriptableEventChunkDynentDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		vec3_t launchLinVel;
		vec3_t launchAngVel;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventChunkDynentDef, 88);

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
		unsigned __int16 eventStreamBufferOffsetClient;
		bool allowMissingTag;
		bool useRootOnMissingTag;
	}; assert_sizeof(ScriptableEventSpawnDynentDef, 72);

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
		unsigned __int16 eventStreamBufferOffsetClient;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		bool allowNearestVehicleVelocity;
		bool isViewmodel;
		bool killOnExit;
		bool useDynamicScriptedBeamLength;
		int beamBoneAxis;
		float beamLength;
		char __pad0[8];
	}; assert_sizeof(ScriptableEventPFXDef, 104);
	assert_offsetof(ScriptableEventPFXDef, effectDef, 56);

	struct ScriptableEventSoundDef
	{
		ScriptableEventBaseDef* base;
		char __pad0[8];
		const char* tagName;
		scr_string_t scrTagName;
		const char* soundAlias;
		const char* soundAliasCache;
		char __pad1[8];
	}; assert_sizeof(ScriptableEventSoundDef, 56);
	assert_offsetof(ScriptableEventSoundDef, tagName, 16);
	assert_offsetof(ScriptableEventSoundDef, soundAlias, 32);
	assert_offsetof(ScriptableEventSoundDef, soundAliasCache, 40);

	struct ScriptableEventExplosionDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		float radius;
		float dmgOuter;
		float dmgInner;
		bool stateful;
		bool allowMissingTag;
		bool useRootOnMissingTag;
		char __pad0[8];
	}; assert_sizeof(ScriptableEventExplosionDef, 48);

	struct ScriptableEventLightDef
	{
		ScriptableEventBaseDef* base;
		const char* name;
		scr_string_t scrName;
		float intensityScaleMin;
		float intensityScaleMax;
		float radiusMin;
		float radiusMax;
		bool useColor;
		vec4_t colorLinearSrgb;
		bool useStateTransitionTime;
		float transitionMin;
		float transitionMax;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventLightDef, 72);

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
	}; assert_sizeof(ScriptableEventSunDef, 72);

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
	}; assert_sizeof(ScriptableEventRumbleDef, 56);

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
	}; assert_sizeof(ScriptableEventScreenshakeDef, 80);

	struct ScriptableEventPartDamageDef
	{
		ScriptableEventBaseDef* base;
		ScriptablePartReference partReference;
		float amount;
		//bool destroy;
		float amountDoT;
		float intervalDoT;
		unsigned __int16 eventStreamBufferOffsetServer;
		unsigned __int16 eventStreamBufferOffsetClient;
		ScriptablePartDef* part;
	}; assert_sizeof(ScriptableEventPartDamageDef, 80);

	struct ScriptableEventSetMayhemDef
	{
		ScriptableEventBaseDef* base;
		MayhemData* mayhem;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventSetMayhemDef, 24);

	enum Scriptable_MayhemAction
	{
		Scriptable_MayhemAction_Play = 0x0,
		Scriptable_MayhemAction_Pause = 0x1,
		Scriptable_MayhemAction_Count = 0x2,
	};

	struct ScriptableEventPlayMayhemDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_MayhemAction action;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventPlayMayhemDef, 16);

	enum Scriptable_ShaderParamType
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
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventViewmodelShaderParamDef, 40);

	enum Scriptable_ImageId
	{
		Scriptable_ImageId_Unknown = 0x0,
		Scriptable_ImageId_Count = 0x1,
	};

	struct ScriptableEventViewmodelChangeImageDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_ImageId imageId;
		int imageValue;
	}; assert_sizeof(ScriptableEventViewmodelChangeImageDef, 16);

	struct ScriptableEventClientViewSelectorDef
	{
		ScriptableEventBaseDef* base;
		unsigned int event1pCount;
		ScriptableEventDef* events1p;
		unsigned int event3pCount;
		ScriptableEventDef* events3p;
		unsigned __int16 eventStreamBufferOffsetClient;
	}; assert_sizeof(ScriptableEventClientViewSelectorDef, 48);

	enum Scriptable_TeamFilter
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

	struct ScriptableEventTeamSelectorDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_TeamFilter teamFilter;
		bool testLocalPlayer;
		unsigned int eventPassCount;
		ScriptableEventDef* eventsPass;
		unsigned int eventFailCount;
		ScriptableEventDef* eventsFail;
		unsigned __int16 eventStreamBufferOffsetClient;
		unsigned __int16 eventStreamBufferOffsetServer;
	}; assert_sizeof(ScriptableEventTeamSelectorDef, 56);

	struct ScriptableEventAddModelDef
	{
		ScriptableEventBaseDef* base;
		const char* tagName;
		scr_string_t scrTagName;
		XModel* model;
	}; assert_sizeof(ScriptableEventAddModelDef, 32);

	struct ScriptableEventApplyForceDef
	{
		ScriptableEventBaseDef* base;
		vec3_t forcePos;
		vec3_t forceVector;
	}; assert_sizeof(ScriptableEventApplyForceDef, 32);

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
	}; assert_sizeof(ScriptableEventCompassIconDef, 152);

	enum Scriptable_MaterialOverrideType
	{
		Scriptable_MaterialOverrideType_Off = 0x0,
		Scriptable_MaterialOverrideType_Refraction = 0x1,
		Scriptable_MaterialOverrideType_Dissolve = 0x2,
		Scriptable_MaterialOverrideType_FlagAmplitudeScale = 0x3,
		Scriptable_MaterialOverrideType_Count = 0x4,
	};

	struct ScriptableEventMaterialOverrideDef
	{
		ScriptableEventBaseDef* base;
		Scriptable_MaterialOverrideType materialOverrideType;
		char __pad0[4];
		Material* material;
		int transitionTime;
		char finalValue;
		unsigned __int16 eventStreamBufferOffsetClient;
		char __pad1[8];
	}; assert_sizeof(ScriptableEventMaterialOverrideDef, 40);

	union ScriptableEventDefUnion
	{
		ScriptableEventAnonymousDef anonymous;
		ScriptableEventStateChangeDef stateChange; //
		ScriptableEventWaitDef wait;
		ScriptableEventRandomDef random;
		ScriptableEventScriptDef script; //
		ScriptableEventModelDef model;
		ScriptableEventCollisionDef collision;
		ScriptableEventAnimationDef animation;
		ScriptableEventHideShowBoneDef hideShowBone;
		ScriptableEventNoteTrackDef noteTrack;
		ScriptableEventChunkDynentDef chunkDynent;
		ScriptableEventSpawnDynentDef spawnDynent; //
		ScriptableEventPFXDef particleFX; //
		ScriptableEventSoundDef sound; //
		ScriptableEventExplosionDef explosion; //
		ScriptableEventLightDef light; //
		ScriptableEventSunDef sun;
		ScriptableEventRumbleDef rumble;
		ScriptableEventScreenshakeDef screenshake;
		ScriptableEventPartDamageDef partDamage; //
		ScriptableEventSetMayhemDef setMayhem;
		ScriptableEventPlayMayhemDef playMayhem;
		ScriptableEventViewmodelShaderParamDef viewmodelShaderParam;
		ScriptableEventViewmodelChangeImageDef viewmodelChangeImage;
		ScriptableEventClientViewSelectorDef clientViewSelector;
		ScriptableEventTeamSelectorDef teamSelector;
		ScriptableEventAddModelDef addModel;
		ScriptableEventApplyForceDef applyForce; //
		ScriptableEventCompassIconDef compassIcon;
		ScriptableEventMaterialOverrideDef materialOverride; //
	};

	struct ScriptableEventDef
	{
		ScriptableEventBaseDef base;
		Scriptable_EventType type;
		ScriptableEventDefUnion data;
	}; assert_sizeof(ScriptableEventDef, 176);

	struct ScriptableStateBaseDef
	{
		const char* name;
		int flags;
		unsigned int numEvents;
		ScriptableEventDef* events;
	}; assert_sizeof(ScriptableStateBaseDef, 24);

	enum Scriptable_StateType
	{
		Scriptable_StateType_Simple = 0x0,
		Scriptable_StateType_Health = 0x1,
		Scriptable_StateType_Scripted = 0x2,
		Scriptable_StateType_Count = 0x3,
	};

	struct ScriptableStateSimpleDef
	{
		ScriptableStateBaseDef* base;
	};

	struct ScriptableStateHealthDef
	{
		ScriptableStateBaseDef* base;
		int health;
		int validDamageCause;
		//int validAttackers;
		//int validAttackerTeams;
		int minimumDamage;
		float damagePropagationToParent;
		float damagePropagationToChild;
		float damagePropagationFromParent;
		float damagePropagationFromChild;
		const char* script_id;
		scr_string_t scrScript_id;
		bool localizeSplashDamage;
	}; assert_sizeof(ScriptableStateHealthDef, 56);
	assert_offsetof(ScriptableStateHealthDef, scrScript_id, 48);

	struct ScriptableStateScriptedDef
	{
		ScriptableStateBaseDef* base;
		const char* script_id;
		scr_string_t scrScript_id;
	};

	union ScriptableStateDefUnion
	{
		ScriptableStateSimpleDef simple;
		ScriptableStateHealthDef health; //
		ScriptableStateScriptedDef scripted;
	};

	struct ScriptableStateDef
	{
		ScriptableStateBaseDef base;
		Scriptable_StateType type;
		ScriptableStateDefUnion data;
	}; assert_sizeof(ScriptableStateDef, 88);
	assert_offsetof(ScriptableStateDef, data, 32);

	struct ScriptableDamageTagOverride
	{
		const char* tag;
		scr_string_t scrTag;
	};

	struct ScriptablePartDef // unchecked
	{
		const char* name;
		scr_string_t scrName;
		int flags;
		unsigned int flatId;
		unsigned int serverInstanceFlatId;
		unsigned int serverControlledFlatId;
		const char* tagName;
		scr_string_t scrTagName;
		unsigned int numStates;
		ScriptableStateDef* states;
		ScriptablePartDef* parentPart;
		unsigned int numChildParts;
		ScriptablePartDef* childParts;
		unsigned int numDamageTagOverrides;
		ScriptableDamageTagOverride* damageTagOverrides;
		unsigned __int16 eventStreamBufferOffsetServer;
		unsigned __int16 eventStreamBufferOffsetClient;
		unsigned __int16 eventStreamSize;
	}; assert_sizeof(ScriptablePartDef, 104);

	struct ScriptableDef // unchecked
	{
		const char* name;
		int flags;
		int type;
		ScriptableDef* nextScriptableDef;
		unsigned int numParts;
		ScriptablePartDef* parts;
		unsigned int maxNumDynEntsRequired;
		unsigned int partCount;
		unsigned int serverInstancedPartCount;
		unsigned int serverControlledPartCount;
		unsigned int maxNumDynEntPartsBase;
		unsigned int maxNumDynEntPartsForSpawning;
		unsigned __int16 eventStreamSizeRequiredServer;
		unsigned __int16 eventStreamSizeRequiredClient;
		unsigned __int16 eventStreamSize;
		unsigned int ffMemCost;
		scr_string_t animationTreeName;
		void* animationTreeDef[2];
		unsigned int numXModels;
		XModel** models;
	}; assert_sizeof(ScriptableDef, 0x70);

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
	};

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
	};

	struct EquipSoundSetMoveTypes
	{
		EquipmentSoundSet* soundSets;
	};

	struct EquipSoundSetMantleTypes
	{
		EquipmentSoundSet soundSets[12];
	}; assert_sizeof(EquipSoundSetMantleTypes, 192);

	struct EquipSoundSetStanceTypes
	{
		EquipmentSoundSet soundSets[4];
	};

	struct EquipSoundSetMeleeVM
	{
		EquipmentSoundSet soundSets[56];
	}; assert_sizeof(EquipSoundSetMeleeVM, 896);

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
		EquipSoundSetMoveTypes* mvmtClothFootstepCeilingSoundSets;
		EquipSoundSetMoveTypes* mvmtClothFoleySoundSets;
		EquipSoundSetMoveTypes* mvmtRattleSoundSets;
		EquipSoundSetMoveTypes mvmtAccentSoundSets;
		EquipSoundSetMantleTypes* mvmtMantleSoundSets;
		EquipSoundSetStanceTypes* mvmtStanceSoundSets;
		EquipSoundSetMeleeVM* meleeAttackVMSoundSets;
	}; assert_sizeof(EquipmentSoundTable, 0x70);
	assert_offsetof(EquipmentSoundTable, mvmtStanceSoundSets, 96);

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
		unsigned int pad[4];
	}; assert_sizeof(VectorSubField, 80);

	struct VectorField
	{
		const char* name;
		VectorSubField* subFields;
		ExtentBounds worldBounds;
		unsigned int numSubFields;
		unsigned int pad;
	}; assert_sizeof(VectorField, 48);

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
	};

	struct FxParticleSimAnimationParticleData
	{
		unsigned __int16 xNormalizedPos;
		unsigned __int16 yNormalizedPos;
		unsigned __int16 zNormalizedPos;
		unsigned __int16 xNormalizedWidth;
		unsigned __int16 yNormalizedHeight;
		unsigned __int16 orientation;
		unsigned __int16 lifetime;
		unsigned __int16 particleID;
		unsigned __int16 xNormalizedPosNextFrame;
		unsigned __int16 yNormalizedPosNextFrame;
		unsigned __int16 zNormalizedPosNextFrame;
		unsigned __int16 xNormalizedWidthNextFrame;
		unsigned __int16 yNormalizedHeightNextFrame;
		__int16 orientationDelta;
		unsigned __int16 colorTableIndex;
		unsigned __int16 nextColorTableIndex;
	};

	struct FxParticleSimAnimationParticleDataOrientation3D
	{
		unsigned __int16 yOrientation;
		unsigned __int16 zOrientation;
		__int16 yOrientationDelta;
		__int16 zOrientationDelta;
	};

	struct FxParticleSimAnimationParticleDataIndex
	{
		char index;
		char nextIndex;
	};

	struct FxParticleSimAnimationFrame
	{
		unsigned int particleDataOffset;
		unsigned int numActiveParticles;
	};

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
	};

	struct StreamingInfo;

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
	}; assert_sizeof(LaserDef, 0xB8);

	struct TTFDef
	{
		const char* name;
		int fileLen;
		const char* file;
		void* ftFace;
	}; assert_sizeof(TTFDef, 0x20);

	struct SuitDef;

	struct SuitAnimPackage;

	struct SpaceshipTargetDef
	{
		const char* name;
		scr_string_t targetTag;
		float annotationOffsetX;
		float annotationOffsetY;
		float annotationOffsetZ;
		bool disableLeading;
		float lock_minDistance;
		float lock_maxDistance;
		float lock_cutoffDistance;
		float lock_minRate;
		float lock_maxRate;
		float lock_maxZoomFOV;
		float lock_followDistance;
		float lockCooldown_rate;
		float aa_closeDistance;
		float aa_farDistance;
		float aa_closeMinAngle;
		float aa_closeMaxAngle;
		float aa_closeBlendMax;
		float aa_farMinAngle;
		float aa_farMaxAngle;
		float aa_farBlendMax;
		float aaLocked_closeDistance;
		float aaLocked_farDistance;
		float aaLocked_closeMinAngle;
		float aaLocked_closeMaxAngle;
		float aaLocked_closeBlendMax;
		float aaLocked_farMinAngle;
		float aaLocked_farMaxAngle;
		float aaLocked_farBlendMax;
		float pilotAssistScale;
		float callout_maxDistance;
		float callout_maxAngle;
	};

	enum RumbleGraphs
	{
		RUMBLEGRAPH_HIGH = 0x0,
		RUMBLEGRAPH_LOW = 0x1,
		RUMBLEGRAPH_COUNT = 0x2,
	};

	struct RumbleInfo
	{
		const char* name;
		int duration;
		float range;
		RumbleGraph* highRumbleGraph;
		RumbleGraph* lowRumbleGraph;
		bool fadeWithDistance;
		bool broadcast;
	};

	struct RumbleGraph
	{
		const char* name;
		vec2_t knots[16];
		unsigned short knotCount;
	};

	struct WeaponAnimPackage;

	struct WeaponSFXPackage;

	struct WeaponVFXPackage;

	struct BehaviorTree;

	struct XAnimArcheType;

	enum XAnimUpVectorType
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
		char curveType;
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
		char type;
		char localTargetBoneIndex;
		scr_string_t sourceBoneNames[2];
		XAnimConstraintUnion u;
	}; assert_sizeof(XAnimConstraint, 88);

	struct unk_1453E1E68
	{
		scr_string_t unk01;
		char __pad0[92];
		scr_string_t unk02;
		char __pad1[4];
		scr_string_t unk03;
		char __pad2[20];
	}; assert_sizeof(unk_1453E1E68, 0x80);

	struct unk_1453E1E30
	{
		scr_string_t unk01;
		char __pad0[8];
	}; assert_sizeof(unk_1453E1E30, 12);

	struct unk_1453E1EA8
	{
		char __pad0[32];
	}; assert_sizeof(unk_1453E1EA8, 32);

	struct XAnimProceduralBones
	{
		const char* name;
		XAnimConstraint* constraints;
		unsigned int numConstraints;
		unsigned int numTargetBones;
		scr_string_t* targetBoneNames;
		unsigned int unk01_count;
		unk_1453E1E68* unk01;
		unsigned int unk02_count;
		unk_1453E1E30* unk02;
		int unk_04;
		unsigned int unk03_count;
		unk_1453E1EA8* unk03;
	}; assert_sizeof(XAnimProceduralBones, 0x50);

	struct ReticleDef
	{
		const char* name;
		const char* widgetName;
		bool lockReticleToTag;
		bool primaryAffectedByEMP;
		bool secondaryAffectedByEMP;
		bool scaleWithFOV;
		float naturalDistance;
		float minReticleScale;
		float maxReticleScale;
	}; assert_sizeof(ReticleDef, 0x20);

	struct GfxLightMap
	{
		const char* name;
		GfxImage* textures[3];
	};

	union XAssetHeader
	{
		void* data;
		PhysicsLibrary* physicsLibrary;
		PhysicsSFXEventAsset* physicsSFXEventAsset;
		PhysicsVFXEventAsset* physicsVFXEventAsset;
		PhysicsAsset* physicsAsset;
		PhysicsFXPipeline* physicsFXPipeline;
		PhysicsFXShape* physicsFXShape;
		XAnimParts* parts;
		XModelSurfs* modelSurfs;
		XModel* model;
		MayhemData* mayhem;
		Material* material;
		ComputeShader* computeShader;
		MaterialVertexShader* vertexShader;
		MaterialHullShader* hullShader;
		MaterialDomainShader* domainShader;
		MaterialPixelShader* pixelShader;
		MaterialVertexDeclaration* vertexDecl;
		MaterialTechniqueSet* techniqueSet;
		GfxImage* image;
		SndGlobals* soundGlobals;
		SndBank* soundBank;
		SndBankTransient* soundBankTransient;
		//clipMap_t* clipMap;
		ComWorld* comWorld;
		GlassWorld* glassWorld;
		PathData* pathData;
		//NavMeshData* navMeshData;
		//MapEnts* mapEnts;
		FxWorld* fxWorld;
		//GfxWorld* gfxWorld;
		//GfxWorldTransientZone* gfxWorldTransientZone;
		//void* iesProfile;
		GfxLightDef* lightDef;
		//void* uiMap;
		AnimationClass* animClass;
		PlayerAnimScript* playerAnim;
		//Gesture* gesture;
		LocalizeEntry* localize;
		//WeaponAttachment* attachment;
		//WeaponCompleteDef* weapon;
		ParticleSystemDef* vfx;
		FxEffectDef* fx;
		FxImpactTable* impactFx;
		SurfaceFxTable* surfaceFx;
		//void* aiType;
		//void* mpType;
		//void* character;
		//void* modelAlias;
		RawFile* rawfile;
		ScriptFile* scriptfile;
		StringTable* stringTable;
		LeaderboardDef* leaderboardDef;
		VirtualLeaderboardDef* virtualLeaderboardDef;
		StructuredDataDefSet* structuredDataDefSet;
		DDLFile* ddlFile;
		TracerDef* tracerDef;
		//VehicleDef* vehDef;
		//AddonMapEnts* addonMapEnts;
		NetConstStrings* netConstStrings;
		LuaFile* luaFile;
		ScriptableDef* scriptable;
		EquipmentSoundTable* equipSndTable;
		VectorField* vectorField;
		FxParticleSimAnimation* particleSimAnimation;
		//StreamingInfo* streamingInfo;
		LaserDef* laserDef;
		TTFDef* ttfDef;
		//SuitDef* suitDef;
		//SuitAnimPackage* suitAnimPackage;
		//SpaceshipTargetDef* spaceshipTargetDef;
		RumbleInfo* rumble;
		RumbleGraph* rumbleGraph;
		//WeaponAnimPackage* weaponAnimPackage;
		//WeaponSFXPackage* weaponSFXPackage;
		//WeaponVFXPackage* weaponVFXPackage;
		//BehaviorTree* behaviorTree;
		//XAnimArcheType* archeType;
		XAnimProceduralBones *proceduralBones;
		ReticleDef* reticleDef;
		GfxLightMap* lightMap;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetEntry
	{
		XAssetHeader header;
		int nextHash;
		int nextOverride; // or nextPoolEntry
		char inuseMask;
		unsigned char type;
		unsigned short zoneIndex;
	}; assert_sizeof(XAssetEntry, 0x18);

	struct ScriptStringList
	{
		int count;
		const char** strings;
	};

	union GfxZoneTableEntry
	{
		char* dataPtr;
		void* voidPtr;
		ID3D11Buffer* buffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11BlendState* blendState;
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
		unsigned int stableConstantBufferCount;
		unsigned int* perPrimConstantBufferSizes;
		unsigned int* perObjConstantBufferSizes;
		unsigned int* stableConstantBufferSizes;
		GfxZoneTableEntry* perPrimConstantBuffers;
		GfxZoneTableEntry* perObjConstantBuffers;
		GfxZoneTableEntry* stableConstantBuffers;
	};

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

	enum DBSyncMode : std::int8_t
	{
		DB_LOAD_SYNC = 0x0,
		DB_LOAD_ASYNC = 0x1,
	};

	enum DBAllocFlags : std::int32_t
	{
		DB_ZONE_NONE = 0x0,
		DB_ZONE_COMMON = 0x1,
		DB_ZONE_GAME = 0x4, // maybe
		DB_ZONE_LOAD = 0x100,
		DB_ZONE_CUSTOM = 0x1000000 // added for custom zone loading
	};

	struct XZoneInfo
	{
		const char* name;
		int allocFlags;
	};

	struct DBFile
	{
		char name[64];
		void* handle;
		unsigned __int64 memoryBufferSize;
		unsigned char* memoryBuffer;
		void* handle_dcache;
		char __pad1[22];
		bool isSecure;
		char __pad2[49];
	}; assert_sizeof(DBFile, 0xA8);
	assert_offsetof(DBFile, isSecure, 118);

	struct db_internal_state
	{
		int dummy;
	};

	struct db_z_stream_s
	{
		unsigned char* next_in;
		unsigned int avail_in;
		unsigned int total_in;
		unsigned char* next_out;
		unsigned int avail_out;
		unsigned int total_out;
		char* msg;
		db_internal_state* state;
		unsigned __int8* (__fastcall* zalloc)(unsigned __int8*, unsigned int, unsigned int);
		void(__fastcall* zfree)(unsigned __int8*, unsigned __int8*);
		unsigned __int8* opaque;
		int data_type;
		unsigned long adler;
		unsigned long reserved;
	};

	enum DB_CompressorType : std::uint8_t
	{
		DB_COMPRESSOR_INVALID = 0x0,
		DB_COMPRESSOR_PASSTHROUGH = 0x1,
		DB_COMPRESSOR_BLOCK = 0x2,
	};

	struct DB_ReadStream
	{
		unsigned char* next_in;
		unsigned __int64 avail_in;
		unsigned __int64 total_in;
		unsigned char* next_out;
		unsigned __int64 avail_out;
		unsigned __int64 total_out;
		DB_CompressorType compressor;
	};

	struct DB_LoadData
	{
		DBFile* file;
		void* unk1;
		unsigned char* fileBuffer;
		unsigned __int64 readSize;
		unsigned __int64 completedReadSize;
		unsigned __int64 offset;
		unsigned char* start_in;
		unsigned int readError;
		int readingResident;
		DB_ReadStream stream;
		__int64 read_size_;
		__int64 offset_;
		int unk4;
		int unk5;
		int unk6;
		int unk7;
		int unk8;
		int unk9;
		int unk10;
		int unk11;
	}; assert_sizeof(DB_LoadData, 0xA8);

	enum XFileBlock
	{
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_UNK1 = 0x1,
		XFILE_BLOCK_UNK2 = 0x2,
		XFILE_BLOCK_IMAGE_STREAM = 0x3,
		XFILE_BLOCK_SHARED_STREAM = 0x4,
		XFILE_BLOCK_UNK5 = 0x5,
		XFILE_BLOCK_RUNTIME = 0x6,
		XFILE_BLOCK_UNK7 = 0x7,
		XFILE_BLOCK_VIRTUAL = 0x8,
		XFILE_BLOCK_SCRIPT = 0x9,
		MAX_XFILE_COUNT = 0xA,
	};

	struct XBlock
	{
		char* alloc;
		unsigned __int64 size;
	};

	struct XZoneMemory
	{
		XBlock blocks[MAX_XFILE_COUNT];
		char __pad0[112];
		void* shared_ff_data;
		unsigned int shared_ff_count;
		int padding1;
		void* unknown; // always 0
		void* image_ff_data;
		unsigned int image_ff_count;
		int padding2;
		char __pad1[0x100]; // unk size
	};

	struct DB_AuthHash
	{
		unsigned char bytes[32];
	};

	struct DB_AuthSignature
	{
		unsigned char bytes[256];
	};

	struct DB_MasterBlock
	{
		DB_AuthHash chunkHashes[512];
	}; assert_sizeof(DB_MasterBlock, 0x4000);

	struct DB_AuthSubHeader
	{
		char fastfileName[32];
		unsigned int reserved;
		DB_AuthHash masterBlockHashes[192];
	};

	struct DB_AuthHeader // sub_1409E6100
	{
		char magic[8]; // IWffs100
		unsigned int reserved;
		DB_AuthHash subheaderHash;
		DB_AuthSignature signedSubheaderHash;
		DB_AuthSubHeader subheader;
		char padding[9904]; // not used
	}; assert_sizeof(DB_AuthHeader, 0x4000);

	struct XPakHeader
	{
		char magic[8];
		std::uint32_t version;
		char unknown[20]; // unused
		DB_AuthHash hash;
		DB_AuthSignature signedhash;
	};

	struct XFileStreamData
	{
		std::uint64_t size;
		std::uint64_t unk1;
		std::uint64_t unk2;
		std::uint64_t block_size[MAX_XFILE_COUNT];
		std::uint64_t unk_arr[8];
	}; assert_sizeof(XFileStreamData, 168);

	struct XFileHeader
	{
		char magic[8];
		std::uint32_t version;
		std::uint8_t unused; // (unused)
		std::uint8_t has_no_image_fastfile;
		std::uint8_t has_no_shared_fastfile;
		std::uint8_t unk1;
		std::uint32_t fileTimeHigh; // (unused)
		std::uint32_t fileTimeLow; // (unused)
		XFileStreamData stream_data;
		std::uint32_t shared_ff_hash; // shared fastfile // (unused)
		std::uint32_t shared_ff_count; // shared fastfile // (unused)
		std::uint32_t image_ff_hash; // image fastfile // (unused)
		std::uint32_t image_ff_count; // image fastfile // (unused)
		// image streams 24 * sharedcount
		// image streams 24 * imagecount
		std::uint64_t fileLen;
		std::uint64_t fileLenUnk1;
		std::uint64_t fileLenUnk2;
		// if signed: DB_AuthHeader info;
		// XFileCompressorHeader
	};

	struct XFileCompressorHeader
	{
		DB_CompressorType compressor;
		char magic[3];
	};

#pragma pack(push, 1)
	struct XBlockCompressionBlockHeader
	{
		unsigned int compressedSize;
		std::uint64_t uncompressedSize;
	};

	struct XBlockCompressionBlockSizeAndCompressionType
	{
		unsigned __int32 blockSize : 24;
		unsigned __int32 compressionType : 8;
	};

	struct XBlockCompressionDataHeader
	{
		std::uint64_t uncompressedSize;
		XBlockCompressionBlockSizeAndCompressionType blockSizeAndType;
	};
#pragma pack(pop)

	enum XBlockCompressionType
	{
		XBLOCK_COMPRESSION_INVALID = 0x0,
		XBLOCK_COMPRESSION_ZLIB_SIZE = 0x1,
		XBLOCK_COMPRESSION_ZLIB_SPEED = 0x2,
		XBLOCK_COMPRESSION_LZ4HC = 0x3,
		XBLOCK_COMPRESSION_LZ4 = 0x4,
		XBLOCK_COMPRESSION_NONE = 0x5,
	};

	struct XFileReadData
	{
		bool header_parsed;
		std::uint8_t has_no_image_fastfile;
		std::uint8_t has_no_shared_fastfile;
		std::uint8_t unk1;
		char __pad0[4];
		XFileStreamData stream_data;
		std::uint32_t version;
		std::uint32_t shared_ff_hash;
		std::uint32_t shared_ff_count;
		std::uint64_t fileLenUnk1;
		std::uint32_t image_ff_hash;
		std::uint32_t image_ff_count;
		std::uint64_t fileLen;
		std::uint64_t fileLenUnk2;
	}; assert_sizeof(XFileReadData, 224);
}