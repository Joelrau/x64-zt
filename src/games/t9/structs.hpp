#pragma once

namespace zonetool::t9
{
	enum XFileBlock : std::uint8_t
	{
		XFILE_BLOCK_TEMP = 0,
		XFILE_BLOCK_TEMP_PRELOAD = 1,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 2,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 3,
		XFILE_BLOCK_VIRTUAL = 4,
		XFILE_BLOCK_CPU_GPU_SHARE = 5,
		XFILE_BLOCK_PHYSICAL = 6,
		XFILE_BLOCK_STREAMER = 7,
		XFILE_BLOCK_STREAMER_CPU = 8,
		XFILE_BLOCK_MEMMAPPED = 9,
		XFILE_BLOCK_LOAD_TIME = 10,
		XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR = 11,
		XFILE_BLOCK_MESH = 12,
		MAX_XFILE_COUNT = 13,
	};

	enum XAssetType : std::uint8_t
	{
		ASSET_TYPE_ZONE = 0,
		ASSET_TYPE_ASSETLIST = 1,
		ASSET_TYPE_PHYSPRESET = 2,
		ASSET_TYPE_PHYSCONSTRAINTS = 3,
		ASSET_TYPE_DESTRUCTIBLEDEF = 4,
		ASSET_TYPE_XANIM = 5,
		ASSET_TYPE_XMODEL = 6,
		ASSET_TYPE_XCOLLISION = 7,
		ASSET_TYPE_XSKELETON = 8,
		ASSET_TYPE_XMODELMESH = 9,
		ASSET_TYPE_MATERIAL = 10,
		ASSET_TYPE_CSDEF = 11,
		ASSET_TYPE_COMPUTESHADERSET = 12,
		ASSET_TYPE_RTSDEF = 13,
		ASSET_TYPE_RAYTRACESHADERSET = 14,
		ASSET_TYPE_TECHSET = 15,
		ASSET_TYPE_IMAGE = 16,
		ASSET_TYPE_SOUND = 17,
		ASSET_TYPE_SOUND_BANK = 18,
		ASSET_TYPE_SOUND_ASSET = 19,
		ASSET_TYPE_SOUND_DUCK = 20,
		ASSET_TYPE_SOUND_ALIAS_MODIFIER = 21,
		ASSET_TYPE_SOUND_ACOUSTICS = 22,
		ASSET_TYPE_COL_MAP = 23,
		ASSET_TYPE_CLIP_MAP = 24,
		ASSET_TYPE_COM_MAP = 25,
		ASSET_TYPE_GAME_MAP = 26,
		ASSET_TYPE_GFX_MAP = 27,
		ASSET_TYPE_FONTICON = 28,
		ASSET_TYPE_LOCALIZEENTRY = 29,
		ASSET_TYPE_GESTURE = 30,
		ASSET_TYPE_GESTURETABLE = 31,
		ASSET_TYPE_CINEMATICMOTION = 32,
		ASSET_TYPE_WEAPON = 33,
		ASSET_TYPE_WEAPONFULL = 34,
		ASSET_TYPE_WEAPONFRONTEND = 35,
		ASSET_TYPE_WEAPONBLUEPRINT = 36,
		ASSET_TYPE_WEAPONSTYLESETTINGS = 37,
		ASSET_TYPE_WEAPONSECONDARYMOVEMENT = 38,
		ASSET_TYPE_WEAPONTUNABLES = 39,
		ASSET_TYPE_CGMEDIATABLE = 40,
		ASSET_TYPE_PLAYERSOUNDSTABLE = 41,
		ASSET_TYPE_PLAYERFXTABLE = 42,
		ASSET_TYPE_SHAREDWEAPONSOUNDS = 43,
		ASSET_TYPE_ATTACHMENT = 44,
		ASSET_TYPE_ATTACHMENTUNIQUE = 45,
		ASSET_TYPE_WEAPONCAMO = 46,
		ASSET_TYPE_WEAPONCAMOBINDING = 47,
		ASSET_TYPE_CUSTOMIZATIONTABLE = 48,
		ASSET_TYPE_CUSTOMIZATIONTABLEFRONTEND = 49,
		ASSET_TYPE_SNDDRIVERGLOBALS = 50,
		ASSET_TYPE_FX = 51,
		ASSET_TYPE_TAGFX = 52,
		ASSET_TYPE_KLF = 53,
		ASSET_TYPE_IMPACTSFXTABLE = 54,
		ASSET_TYPE_IMPACTSOUNDSTABLE = 55,
		ASSET_TYPE_AITYPE = 56,
		ASSET_TYPE_CHARACTER = 57,
		ASSET_TYPE_XMODELALIAS = 58,
		ASSET_TYPE_RAWFILE = 59,
		ASSET_TYPE_RAWFILEPREPROC = 60,
		ASSET_TYPE_RAWTEXTFILE = 61,
		ASSET_TYPE_ANIMTREE = 62,
		ASSET_TYPE_STRINGTABLE = 63,
		ASSET_TYPE_STRUCTUREDTABLE = 64,
		ASSET_TYPE_LEADERBOARDDEF = 65,
		ASSET_TYPE_DDL = 66,
		ASSET_TYPE_GLASSES = 67,
		ASSET_TYPE_SCRIPTPARSETREE = 68,
		ASSET_TYPE_SCRIPTPARSETREEDBG = 69,
		ASSET_TYPE_SCRIPT_USING = 70,
		ASSET_TYPE_SCRIPT_USING_CP = 71,
		ASSET_TYPE_SCRIPT_USING_MP = 72,
		ASSET_TYPE_SCRIPT_USING_WZ = 73,
		ASSET_TYPE_SCRIPT_USING_ZM = 74,
		ASSET_TYPE_KEYVALUEPAIRS = 75,
		ASSET_TYPE_VEHICLE = 76,
		ASSET_TYPE_TRACER = 77,
		ASSET_TYPE_SURFACEFXTABLE = 78,
		ASSET_TYPE_SURFACESOUNDDEF = 79,
		ASSET_TYPE_FOOTSTEPTABLE = 80,
		ASSET_TYPE_ENTITYFXIMPACTS = 81,
		ASSET_TYPE_ENTITYSOUNDIMPACTS = 82,
		ASSET_TYPE_ZBARRIER = 83,
		ASSET_TYPE_VEHICLEFXDEF = 84,
		ASSET_TYPE_VEHICLESOUNDDEF = 85,
		ASSET_TYPE_TYPEINFO = 86,
		ASSET_TYPE_SCRIPTBUNDLE = 87,
		ASSET_TYPE_SCRIPTBUNDLELIST = 88,
		ASSET_TYPE_RUMBLE = 89,
		ASSET_TYPE_BULLETPENETRATION = 90,
		ASSET_TYPE_LOCDMGTABLE = 91,
		ASSET_TYPE_AIMTABLE = 92,
		ASSET_TYPE_SHOOTTABLE = 93,
		ASSET_TYPE_PLAYERGLOBALTUNABLES = 94,
		ASSET_TYPE_OVERHEADCAMERATUNABLES = 95,
		ASSET_TYPE_ANIMSELECTORTABLE = 96,
		ASSET_TYPE_ANIMMAPPINGTABLE = 97,
		ASSET_TYPE_ANIMSTATEMACHINE = 98,
		ASSET_TYPE_BEHAVIORTREE = 99,
		ASSET_TYPE_BEHAVIORSTATEMACHINE = 100,
		ASSET_TYPE_TTF = 101,
		ASSET_TYPE_SANIM = 102,
		ASSET_TYPE_SHELLSHOCK = 103,
		ASSET_TYPE_STATUSEFFECT = 104,
		ASSET_TYPE_CINEMATIC_CAMERA = 105,
		ASSET_TYPE_CINEMATIC_SEQUENCE = 106,
		ASSET_TYPE_SPECTATE_CAMERA = 107,
		ASSET_TYPE_XCAM = 108,
		ASSET_TYPE_BGCACHE = 109,
		ASSET_TYPE_FLAMETABLE = 110,
		ASSET_TYPE_BITFIELD = 111,
		ASSET_TYPE_MAPTABLE = 112,
		ASSET_TYPE_MAPTABLEENTRY = 113,
		ASSET_TYPE_MAPTABLELIST = 114,
		ASSET_TYPE_OBJECTIVE = 115,
		ASSET_TYPE_OBJECTIVELIST = 116,
		ASSET_TYPE_NAVMESH = 117,
		ASSET_TYPE_NAVVOLUME = 118,
		ASSET_TYPE_LASER = 119,
		ASSET_TYPE_BEAM = 120,
		ASSET_TYPE_STREAMERHINT = 121,
		ASSET_TYPE_FLOWGRAPH = 122,
		ASSET_TYPE_POSTFXBUNDLE = 123,
		ASSET_TYPE_LUAFILE = 124,
		ASSET_TYPE_LUAFILEDEBUG = 125,
		ASSET_TYPE_RENDEROVERRIDEBUNDLE = 126,
		ASSET_TYPE_STATICLEVELFXLIST = 127,
		ASSET_TYPE_TRIGGERLIST = 128,
		ASSET_TYPE_PLAYERROLETEMPLATE = 129,
		ASSET_TYPE_PLAYERROLETEMPLATEFRONTEND = 130,
		ASSET_TYPE_PLAYERROLECATEGORYTABLE = 131,
		ASSET_TYPE_PLAYERROLECATEGORY = 132,
		ASSET_TYPE_CHARACTERBODYTYPE = 133,
		ASSET_TYPE_CHARACTERBODYTYPEFRONTEND = 134,
		ASSET_TYPE_PLAYEROUTFIT = 135,
		ASSET_TYPE_GAMETYPETABLE = 136,
		ASSET_TYPE_GAMETYPETABLEENTRY = 137,
		ASSET_TYPE_FEATURE = 138,
		ASSET_TYPE_FEATURETABLE = 139,
		ASSET_TYPE_UNLOCKABLEITEM = 140,
		ASSET_TYPE_UNLOCKABLEITEMTABLE = 141,
		ASSET_TYPE_ENTITYLIST = 142,
		ASSET_TYPE_PLAYLISTS = 143,
		ASSET_TYPE_PLAYLISTGLOBALSETTINGS = 144,
		ASSET_TYPE_PLAYLISTSCHEDULE = 145,
		ASSET_TYPE_MOTIONMATCHINGINPUT = 146,
		ASSET_TYPE_BLACKBOARD = 147,
		ASSET_TYPE_TACTICALQUERY = 148,
		ASSET_TYPE_PLAYERMOVEMENTTUNABLES = 149,
		ASSET_TYPE_HIERARCHICALTASKNETWORK = 150,
		ASSET_TYPE_RAGDOLL = 151,
		ASSET_TYPE_STORAGEFILE = 152,
		ASSET_TYPE_STORAGEFILELIST = 153,
		ASSET_TYPE_CHARMIXER = 154,
		ASSET_TYPE_STOREPRODUCT = 155,
		ASSET_TYPE_STORECATEGORY = 156,
		ASSET_TYPE_STORECATEGORYLIST = 157,
		ASSET_TYPE_RANK = 158,
		ASSET_TYPE_RANKTABLE = 159,
		ASSET_TYPE_PRESTIGE = 160,
		ASSET_TYPE_PRESTIGETABLE = 161,
		ASSET_TYPE_FIRSTPARTYENTITLEMENT = 162,
		ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST = 163,
		ASSET_TYPE_ENTITLEMENT = 164,
		ASSET_TYPE_ENTITLEMENTLIST = 165,
		ASSET_TYPE_SKU = 166,
		ASSET_TYPE_LABELSTORE = 167,
		ASSET_TYPE_LABELSTORELIST = 168,
		ASSET_TYPE_CPU_OCCLUSION_DATA = 169,
		ASSET_TYPE_LIGHTING = 170,
		ASSET_TYPE_DISTRICTS = 171,
		ASSET_TYPE_STREAMERWORLD = 172,
		ASSET_TYPE_TALENT = 173,
		ASSET_TYPE_PLAYERTALENTTEMPLATE = 174,
		ASSET_TYPE_PLAYERANIMATION = 175,
		ASSET_TYPE_UNUSED = 176,
		ASSET_TYPE_TERRAINGFX = 177,
		ASSET_TYPE_HIGHLIGHTREELINFODEFINES = 178,
		ASSET_TYPE_HIGHLIGHTREELPROFILEWEIGHTING = 179,
		ASSET_TYPE_HIGHLIGHTREELSTARLEVELS = 180,
		ASSET_TYPE_DLOGEVENT = 181,
		ASSET_TYPE_RAWSTRING = 182,
		ASSET_TYPE_BALLISTICDESC = 183,
		ASSET_TYPE_STREAMKEY = 184,
		ASSET_TYPE_RENDERTARGETS = 185,
		ASSET_TYPE_DRAWNODES = 186,
		ASSET_TYPE_GROUPLODMODEL = 187,
		ASSET_TYPE_FXLIBRARYVOLUME = 188,
		ASSET_TYPE_ARENASEASONS = 189,
		ASSET_TYPE_SPRAYORGESTUREITEM = 190,
		ASSET_TYPE_SPRAYORGESTURELIST = 191,
		ASSET_TYPE_HWPLATFORM = 192,
		ASSET_TYPE_ATTACHMENTTABLE = 193,
		ASSET_TYPE_NAVINPUT = 194,
		ASSET_TYPE_UIMODELDATASTRUCT = 195,
		ASSET_TYPE_CRAFTICON = 196,
		ASSET_TYPE_CRAFTICONLIST = 197,
		ASSET_TYPE_CRAFTWEAPONSTICKER = 198,
		ASSET_TYPE_CRAFTWEAPONSTICKERLIST = 199,
		ASSET_TYPE_CRAFTBACKGROUND = 200,
		ASSET_TYPE_CRAFTBACKGROUNDLIST = 201,
		ASSET_TYPE_CRAFTMATERIAL = 202,
		ASSET_TYPE_CRAFTMATERIALLIST = 203,
		ASSET_TYPE_CRAFTCATEGORY = 204,
		ASSET_TYPE_CRAFTCATEGORYLIST = 205,
		ASSET_TYPE_CRAFTWEAPONICONTRANSFORM = 206,
		ASSET_TYPE_CRAFTWEAPONICONTRANSFORMLIST = 207,
		ASSET_TYPE_XANIMCURVE = 208,
		ASSET_TYPE_DYNMODEL = 209,
		ASSET_TYPE_VECTORFIELD = 210,
		ASSET_TYPE_WINDDEF = 211,
		ASSET_TYPE_VEHICLEASSEMBLY = 212,
		ASSET_TYPE_MILESTONE = 213,
		ASSET_TYPE_MILESTONETABLE = 214,
		ASSET_TYPE_TRIGGEREFFECTDESC = 215,
		ASSET_TYPE_TRIGGERACTIONS = 216,
		ASSET_TYPE_PLAYERSETTINGS = 217,
		ASSET_TYPE_COMPASSTUNABLES = 218,
		ASSET_TYPE_EXECUTION = 219,
		ASSET_TYPE_SCENARIO = 220,
		ASSET_TYPE_COUNT = 221,
	};

	union XAssetHeader
	{
		void* data;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetList
	{
		std::int32_t stringCount;
		const char** strings;
		std::int32_t* unknown;
		std::int32_t assetCount;
		XAsset* assets;
	};

	static_assert(sizeof(XAsset) == 0x10);
	static_assert(sizeof(XAssetList) == 0x28);

	struct XHash
	{
		std::uint64_t value;
	};

	struct KeyValuePair
	{
		XHash key;
		const char* value;
	};

	struct KeyValuePairs
	{
		XHash name;
		std::int32_t numVariables;
		KeyValuePair* keyValuePairs;
	};

	static_assert(sizeof(KeyValuePair) == 0x10);
	static_assert(sizeof(KeyValuePairs) == 0x18);

	enum StreamKeyFlags : std::uint8_t
	{
		STREAM_KEY_FLAG_CPU = 0x1,
		STREAM_KEY_FLAG_RESIDENT = 0x2,
		STREAM_KEY_FLAG_PAGE_ALIGNED = 0x20,
	};

	struct StreamKey
	{
		XHash name;
		std::uint64_t xpakKey;
		char __pad0[0x10];
		std::uint8_t* data;
		char __pad1[0x8];
		std::uint32_t size;
		char __pad2[0x3];
		std::uint8_t flags;
	};

	struct SndValueArray
	{
		std::uint64_t* values;
		std::int32_t count;
	};

	struct SndAsset
	{
		XHash name;
		char __pad0[0x10];
		std::uint32_t extraSize;
		std::uint8_t* extra;
		std::uint32_t dataSize;
		std::uint8_t* data;
		char __pad1[0x8];
		StreamKey* streamKey;
		char __pad2[0x10];
	};

	struct SndDuck
	{
		char __pad0[0x660];
	};

	struct SndAliasModifier
	{
		XHash name;
		char __pad0[0x8];
		SndValueArray curves[7];
		char __pad1[0x18];
	};

	struct SndAliasInfo
	{
		SndAliasModifier* modifier;
		char __pad0[0x48];
		std::uint32_t runtimeIndex;
		char __pad1[0x7C];
	};

	struct SndAlias
	{
		XHash name;
		char __pad0[0x18];
		SndAsset* assets[3];
		char __pad1[0x18];
		SndAliasInfo* info;
		std::uint8_t* extra;
	};

	struct SndAliasList
	{
		XHash name;
		SndAlias* aliases;
		std::int32_t count;
		char __pad0[0xC];
	};

	struct SndIndexEntry
	{
		std::uint16_t value;
		std::uint16_t next;
	};

	struct SndBankEntry
	{
		char __pad0[0x88];
	};

	struct SndBankGroupValue
	{
		char __pad0[0x80];
	};

	struct SndBankGroupItem
	{
		char __pad0[0x108];
		std::uint32_t valueCount;
		SndBankGroupValue* values;
		char __pad1[0x8];
	};

	struct SndBankGroup
	{
		char __pad0[0x48];
		std::uint32_t itemCount;
		SndBankGroupItem* items;
	};

	struct SndAcousticsProbe
	{
		char __pad0[0x28];
		StreamKey* streamKey;
		char __pad1[0x10];
	};

	struct SndAcousticsData
	{
		std::int32_t size;
		std::uint8_t* data;
		std::int32_t probeCount;
		char __pad0[0x8];
		SndAcousticsProbe* probes;
		StreamKey* streamKey;
		char __pad1[0x8];
	};

	struct SndAcousticsRecord
	{
		char __pad0[0x54];
	};

	struct SndAcoustics
	{
		XHash name;
		const char* nameString;
		char __pad0[0x8];
		StreamKey* streamKey;
		char __pad1[0x8];
		SndAcousticsData acousticsData;
		char __pad2[0x20];
		SndValueArray values0[3];
		char __pad3[0x8];
		SndValueArray values1;
		char __pad4[0x18];
		SndValueArray values2[3];
		char __pad5[0x8];
		SndValueArray values3;
		char __pad6[0x8];
		SndValueArray values4;
		char __pad7[0x10];
		SndAcousticsRecord* records;
		std::int32_t recordCount;
	};

	struct SndDriverGlobalsList
	{
		char __pad0[0x8];
		std::uint64_t* values;
		std::int32_t valueCount;
		char __pad1[0x4];
	};

	struct SndDriverGlobals
	{
		XHash name;
		std::uint32_t records0Count;
		std::uint8_t (*records0)[0x30];
		std::uint32_t listCount;
		SndDriverGlobalsList* lists;
		std::uint32_t records1Count;
		std::uint8_t (*records1)[0x30];
		std::uint32_t records2Count;
		std::uint64_t* records2;
		std::uint32_t records3Count;
		std::uint8_t (*records3)[0x98];
		std::uint32_t records4Count;
		std::uint8_t (*records4)[0xD8];
		std::uint32_t records5Count;
		std::uint8_t (*records5)[0x58];
		std::uint32_t records6Count;
		std::uint8_t (*records6)[0x188];
		std::uint32_t records7Count;
		std::uint8_t (*records7)[0x30];
		std::uint32_t records8Count;
		std::uint8_t (*records8)[0x20];
		std::uint32_t records9Count;
		std::uint8_t (*records9)[0x20];
		std::uint32_t modifierCount;
		SndAliasModifier** modifiers;
	};

	static_assert(sizeof(SndDriverGlobalsList) == 0x18);
	static_assert(sizeof(SndDriverGlobals) == 0xC8);
	static_assert(offsetof(SndDriverGlobals, modifiers) == 0xC0);

	struct SndBank
	{
		XHash name;
		char __pad0[0x18];
		std::uint32_t aliasCount;
		SndAliasList* aliases;
		SndIndexEntry* aliasIndex;
		std::uint32_t duckCount;
		SndDuck** ducks;
		std::uint32_t entryCount;
		SndBankEntry* entries;
		std::uint32_t groupCount;
		SndBankGroup* groups;
		SndAcoustics* acoustics;
	};

	enum GfxImageFlags : std::uint32_t
	{
		GFX_IMAGE_FLAG_STREAMED = 0x10,
	};

	enum GfxImageMapType : std::uint8_t
	{
		MAPTYPE_NONE = 0,
		MAPTYPE_2D = 1,
		MAPTYPE_2D_ARRAY = 2,
		MAPTYPE_3D = 3,
		MAPTYPE_CUBE = 4,
		MAPTYPE_CUBE_ARRAY = 5,
	};

	struct GfxImageStreamPart
	{
		std::uint64_t key;
		char __pad0[0x10];
		std::uint64_t packedEnd;
	};

	enum XPakMode : std::uint32_t
	{
		XPAK_MODE_XSUB = 2,
	};

	struct XPakSection
	{
		std::uint64_t count;
		std::uint64_t offset;
		std::uint64_t size;
	};

	struct XPakHeader
	{
		std::uint32_t magic;
		std::uint16_t unknown;
		std::uint16_t version;
		char __pad0[0x8];
		XPakMode mode;
		std::uint32_t xsubBits;
		char __pad1[0x10];
		std::uint32_t xsubCount;
		char __pad2[0x774];
		XPakSection index;
		char __pad3[0x48];
	};

#pragma pack(push, 1)
	struct XPakXSub
	{
		std::uint64_t key;
		std::uint16_t number;
		std::uint8_t flags;
	};
#pragma pack(pop)

	enum XPakEntryFlags : std::uint64_t
	{
		XPAK_ENTRY_COMPRESSED = 1,
	};

	struct XPakIndexEntry
	{
		std::uint64_t key;
		std::uint64_t packed;
	};

	enum XSubBlockType : std::uint32_t
	{
		XSUB_BLOCK_UNCOMPRESSED = 0,
		XSUB_BLOCK_SKIP = 1,
		XSUB_BLOCK_OODLE = 8,
		XSUB_BLOCK_OODLE_ALT = 9,
	};

	struct XSubBlock
	{
		std::uint32_t size : 24;
		XSubBlockType type : 8;
	};

	struct XSubChunkHeader
	{
		std::uint32_t blockCount;
		std::uint32_t outputOffset;
		XSubBlock blocks[30];
	};

	struct GfxImage
	{
		XHash name;
		std::uint8_t* firstPartData;
		char __pad0[0x18];
		std::uint8_t* pixels;
		GfxImageStreamPart* streamParts;
		char __pad1[0x58];
		std::uint32_t flags;
		char __pad2[0x4];
		std::uint32_t pixelSize;
		std::uint32_t format;
		std::uint16_t width;
		std::uint16_t height;
		std::uint16_t depth;
		char __pad3[0xF];
		GfxImageMapType mapType;
		std::uint8_t levelCount;
		char __pad4[0x1];
		std::uint8_t streamPartCount;
		char __pad5[0x17];
	};

	static_assert(sizeof(GfxImageStreamPart) == 0x20);
	static_assert(offsetof(XPakHeader, xsubCount) == 0x28);
	static_assert(offsetof(XPakHeader, index) == 0x7A0);
	static_assert(sizeof(XPakHeader) == 0x800);
	static_assert(sizeof(XPakXSub) == 0xB);
	static_assert(sizeof(XSubBlock) == 4);
	static_assert(sizeof(XSubChunkHeader) == 0x80);
	static_assert(sizeof(GfxImage) == 0xD0);
	static_assert(offsetof(GfxImage, flags) == 0x90);
	static_assert(offsetof(GfxImage, width) == 0xA0);
	static_assert(offsetof(GfxImage, mapType) == 0xB5);
	static_assert(offsetof(GfxImage, streamPartCount) == 0xB8);

	using scr_string_t = std::uint32_t;

	struct XSkeletonExtraBone
	{
		scr_string_t name;
		std::uint32_t data;
	};

	struct XSkeletonExtraBoneInfo
	{
		char __pad0[0x28];
	};

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};

	struct XSkeleton
	{
		XHash name;
		scr_string_t* boneNames;
		XSkeletonExtraBone* extraBones;
		std::uint16_t* parentList;
		std::int16_t (*quats)[4];
		float (*trans)[4];
		std::uint8_t* partClassification;
		DObjAnimMat* baseMat;
		XSkeletonExtraBoneInfo* extraBoneInfo;
		std::uint16_t numBones;
		std::uint16_t numExtraBones;
		std::uint16_t numRootBones;
		char __pad0[0xA];
	};

	static_assert(sizeof(XSkeletonExtraBone) == 0x8);
	static_assert(sizeof(DObjAnimMat) == 0x20);
	static_assert(sizeof(XSkeleton) == 0x58);
	static_assert(offsetof(XSkeleton, numBones) == 0x48);

	struct LocalizeEntry
	{
		const char* value;
		XHash name;
	};

	static_assert(sizeof(LocalizeEntry) == 0x10);

	struct RawFile
	{
		XHash name;
		std::int32_t len;
		char __pad0[0x4];
		const char* buffer;
	};

	struct ScriptParseTree
	{
		XHash name;
		std::uint8_t* buffer;
		std::int32_t len;
		char __pad0[0x4];
	};

	struct ScriptUsing
	{
		XHash name;
		char __pad0[0x10];
	};

	static_assert(sizeof(RawFile) == 0x18);
	static_assert(sizeof(ScriptParseTree) == 0x18);
	static_assert(sizeof(ScriptUsing) == 0x18);

	struct SurfaceSoundDef
	{
		XHash name;
		char __pad0[0x140];
	};

	struct EntitySoundImpacts
	{
		XHash name;
		char __pad0[0x240];
	};

	struct ImpactSoundsTable
	{
		XHash name;
		SurfaceSoundDef* surfaceSoundDef;
		EntitySoundImpacts* entitySoundImpacts[4];
		char __pad0[0x8];
	};

	static_assert(sizeof(SurfaceSoundDef) == 0x148);
	static_assert(sizeof(EntitySoundImpacts) == 0x248);
	static_assert(sizeof(ImpactSoundsTable) == 0x38);

	struct MaterialShaderProgram
	{
		char __pad0[0x8];
		std::uint8_t* bytecode;
		std::uint32_t bytecodeSize;
		char __pad1[0x4];
	};

	struct MaterialShaderSetLayout
	{
		char __pad0[0x38];
	};

	struct MaterialShaderSetInfo
	{
		char __pad0[0x20];
	};

	struct MaterialShaderSet
	{
		MaterialShaderSetLayout* layout;
		MaterialShaderSetInfo* info;
		MaterialShaderProgram* programs[6];
		char __pad0[0x10];
	};

	struct MaterialShaderArgument
	{
		char __pad0[0xC];
	};

	struct MaterialPassTable
	{
		char __pad0[0x4];
		std::int32_t entryCount;
		char __pad1[0x8];
		std::uint64_t* entries;
		std::uint8_t* table;
	};

	struct MaterialPass
	{
		char __pad0[0x1];
		std::uint8_t argCounts[2];
		char __pad1[0xB];
		std::uint8_t valueCount;
		char __pad2[0x1];
		MaterialShaderArgument* args;
		std::uint64_t* values;
		MaterialShaderSet* shaderSet;
		MaterialPassTable* table;
	};

	struct MaterialTechnique
	{
		XHash name;
		MaterialPass passes[2];
		char __pad0[0x6];
		std::uint8_t hashCount;
		char __pad1[0x1];
		std::uint64_t* hashes;
	};

	struct MaterialTechniqueSetInfo
	{
		const char* strings[3];
		char __pad0[0x8];
		std::uint64_t nameCount;
		const char** names;
	};

	enum MaterialTechniqueSetFlags : std::uint32_t
	{
		TECHSET_FLAG_ALPHA_TEST = 0x10,
		TECHSET_FLAG_OPAQUE_DEPTH = 0x10000,
		TECHSET_FLAG_TWO_SIDED = 0x800000,
	};

	struct MaterialTechniqueSet
	{
		XHash name;
		char __pad0[0x8];
		std::uint32_t flags;
		char __pad1[0x4];
		MaterialTechnique* techniques[17];
		MaterialTechniqueSetInfo* info;
	};

	static_assert(sizeof(MaterialShaderProgram) == 0x18);
	static_assert(sizeof(MaterialShaderSet) == 0x50);
	static_assert(sizeof(MaterialShaderArgument) == 0xC);
	static_assert(sizeof(MaterialPassTable) == 0x20);
	static_assert(sizeof(MaterialPass) == 0x30);
	static_assert(sizeof(MaterialTechnique) == 0x78);
	static_assert(sizeof(MaterialTechniqueSetInfo) == 0x30);
	static_assert(sizeof(MaterialTechniqueSet) == 0xA8);
	static_assert(offsetof(MaterialPass, args) == 0x10);
	static_assert(offsetof(MaterialTechnique, hashCount) == 0x6E);

	enum MaterialTextureSlot : std::uint32_t
	{
		TEXTURE_SLOT_COLOR_MAP = 0xA0AB1041,
		TEXTURE_SLOT_NORMAL_MAP = 0x59D30D0F,
		TEXTURE_SLOT_ROUGHNESS = 0xE9817F0D,
		TEXTURE_SLOT_AO_MAP = 0x07176BF2,
		TEXTURE_SLOT_SPEC_COLOR_MAP = 0xEC443804,
		TEXTURE_SLOT_EMISSIVE_MAP = 0x34614347,
		TEXTURE_SLOT_REVEAL_MAP = 0x34D849D5,
		TEXTURE_SLOT_THICKNESS_MAP = 0x2335512A,
		TEXTURE_SLOT_MIX_MAP = 0x04429A80,
		TEXTURE_SLOT_TINT_MASK = 0x199A03D3,
		TEXTURE_SLOT_DETAIL_MAP = 0xEB529B4D,
		TEXTURE_SLOT_COLOR_MAP_DETAIL2 = 0x80951342,
		TEXTURE_SLOT_TINT_MASK1 = 0x4CDA7E02,
	};

	struct MaterialTextureDef
	{
		GfxImage* image;
		MaterialTextureSlot slot;
		float uvScale[2];
		std::uint32_t flags;
	};

	struct MaterialImageSetEntry
	{
		GfxImage* image;
		std::uint32_t instanceDataIndex;
		char __pad0[0x4];
	};

	struct MaterialImageSet
	{
		std::uint32_t* instanceData;
		MaterialImageSetEntry* entries;
		std::uint32_t instanceDataCount;
		std::uint32_t entryCount;
	};

	struct MaterialDataEntry
	{
		std::uint32_t offset;
		std::uint32_t size;
		char __pad0[0x4];
	};

	struct MaterialDataEntries
	{
		MaterialDataEntry* entries[8];
	};

	struct MaterialPhysicalData
	{
		char __pad0[0x28];
		MaterialDataEntries* entrySets[13];
		std::uint64_t dataSize;
		std::uint8_t* data;
	};

	struct MaterialRecord
	{
		char __pad0[0x18];
	};

	struct MaterialRecordTable
	{
		char __pad0[0x8];
		MaterialRecord* records;
		std::uint32_t recordCount;
		char __pad1[0x4];
		std::uint64_t* values;
		std::uint32_t valueCount;
		char __pad2[0x4];
	};

	struct MaterialEmbeddedRecord
	{
		char __pad0[0x20];
	};

	struct MaterialEmbeddedTechniqueSet
	{
		MaterialEmbeddedRecord* records;
		std::uint8_t recordCount;
		char __pad0[0x7];
		MaterialTechniqueSet* techniqueSet;
	};

	enum MaterialDrawList : std::uint8_t
	{
		DRAW_LIST_GBUFFER_DEPTH_HACK = 0x0,
		DRAW_LIST_GBUFFER_DEPTH_HACK_DECAL = 0x1,
		DRAW_LIST_GBUFFER = 0x2,
		DRAW_LIST_GBUFFER_DECAL = 0x6,
		DRAW_LIST_GBUFFER_NO_VOLUME_DECAL = 0x8,
		DRAW_LIST_LIT_OPAQUE = 0x9,
		DRAW_LIST_LIT_TRANS = 0xA,
		DRAW_LIST_LIT_TRANS_POST_RESOLVE = 0xB,
		DRAW_LIST_LIT_POST_RESOLVE = 0xC,
		DRAW_LIST_WATER = 0xE,
		DRAW_LIST_EMISSIVE_FX = 0xF,
		DRAW_LIST_DEPTH_HACK_SSS = 0x10,
		DRAW_LIST_FORWARD_SSS = 0x11,
		DRAW_LIST_HAIR = 0x12,
		DRAW_LIST_DEPTH_HACK = 0x13,
		DRAW_LIST_DEPTH_HACK_TRANS = 0x14,
		DRAW_LIST_DEPTH_HACK_POST_BLUR = 0x15,
		DRAW_LIST_SONAR = 0x16,
		DRAW_LIST_SONAR_UINT = 0x17,
		DRAW_LIST_SONAR_DEPTH_HACK = 0x18,
		DRAW_LIST_SONAR_UINT_DEPTH_HACK = 0x19,
		DRAW_LIST_OIT = 0x1A,
		DRAW_LIST_DEPTH_PRIME = 0x1B,
		DRAW_LIST_UNDERWATER = 0x1C,
		DRAW_LIST_NONE = 0x1D,
	};

	enum SurfaceType : std::uint32_t
	{
		SURFACE_TYPE_NONE = 0,
		SURFACE_TYPE_BARK = 1,
		SURFACE_TYPE_BRICK = 2,
		SURFACE_TYPE_CARPET = 3,
		SURFACE_TYPE_CLOTH = 4,
		SURFACE_TYPE_CONCRETE = 5,
		SURFACE_TYPE_DIRT = 6,
		SURFACE_TYPE_FLESH = 7,
		SURFACE_TYPE_FOLIAGE = 8,
		SURFACE_TYPE_GLASS = 9,
		SURFACE_TYPE_GRASS = 10,
		SURFACE_TYPE_GRAVEL = 11,
		SURFACE_TYPE_ICE = 12,
		SURFACE_TYPE_METAL = 13,
		SURFACE_TYPE_MUD = 14,
		SURFACE_TYPE_PAPER = 15,
		SURFACE_TYPE_PLASTER = 16,
		SURFACE_TYPE_ROCK = 17,
		SURFACE_TYPE_SAND = 18,
		SURFACE_TYPE_SNOW = 19,
		SURFACE_TYPE_WATER = 20,
		SURFACE_TYPE_WOOD = 21,
		SURFACE_TYPE_ASPHALT = 22,
		SURFACE_TYPE_CERAMIC = 23,
		SURFACE_TYPE_PLASTIC = 24,
		SURFACE_TYPE_RUBBER = 25,
		SURFACE_TYPE_CUSHION = 26,
		SURFACE_TYPE_FRUIT = 27,
		SURFACE_TYPE_PAINTED_METAL = 28,
		SURFACE_TYPE_PLAYER = 29,
		SURFACE_TYPE_TALL_GRASS = 30,
		SURFACE_TYPE_RIOT_SHIELD = 31,
		SURFACE_TYPE_METAL_THIN = 32,
		SURFACE_TYPE_METAL_HOLLOW = 33,
		SURFACE_TYPE_METAL_CATWALK = 34,
		SURFACE_TYPE_METAL_CAR = 35,
		SURFACE_TYPE_GLASS_CAR = 36,
		SURFACE_TYPE_GLASS_BULLETPROOF = 37,
		SURFACE_TYPE_WATER_SHALLOW = 38,
		SURFACE_TYPE_BODY_ARMOR = 39,
	};

	struct Material
	{
		XHash name;
		char __pad0[0x18];
		std::uint32_t surfaceTypeBits;
		std::uint32_t contents;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;
		std::uint64_t* samplerTable;
		MaterialImageSet* imageSets[8];
		MaterialPhysicalData physicalData;
		MaterialRecordTable recordTable;
		std::uint8_t textureCount;
		std::uint8_t samplerCount;
		MaterialDrawList drawList;
		char __pad1[0x5];
		MaterialEmbeddedTechniqueSet* embeddedTechniqueSet;
	};

	static_assert(sizeof(MaterialTextureDef) == 0x18);
	static_assert(sizeof(MaterialImageSetEntry) == 0x10);
	static_assert(sizeof(MaterialImageSet) == 0x18);
	static_assert(sizeof(MaterialDataEntries) == 0x40);
	static_assert(sizeof(MaterialPhysicalData) == 0xA0);
	static_assert(sizeof(MaterialRecordTable) == 0x28);
	static_assert(sizeof(MaterialEmbeddedTechniqueSet) == 0x18);
	static_assert(sizeof(Material) == 0x158);
	static_assert(offsetof(Material, surfaceTypeBits) == 0x20);
	static_assert(offsetof(Material, physicalData) == 0x80);
	static_assert(offsetof(Material, textureCount) == 0x148);
	static_assert(offsetof(Material, drawList) == 0x14A);

	enum XSurfaceFlags : std::uint32_t
	{
		XSURFACE_FLAG_STREAMED = 0x1,
		XSURFACE_FLAG_RIGID = 0x8,
	};

	struct XSurfaceBoneRef
	{
		scr_string_t name;
		std::uint32_t value;
	};

	struct XSurfaceVertex
	{
		std::uint32_t color;
		std::uint16_t texCoord[2];
		std::uint32_t normal;
		std::uint32_t tangent;
	};

	struct XSurfaceWeights
	{
		std::uint8_t weights[4];
		std::uint16_t bones[4];
	};

	struct XSurface
	{
		std::uint32_t flags;
		std::uint32_t vertCount;
		char __pad0[0x4];
		std::uint32_t indexCount;
		char __pad1[0x10];
		std::uint8_t* meshData;
		std::uint32_t meshDataSize;
		std::uint32_t positionOffset;
		std::uint32_t vertexOffset;
		char __pad2[0x4];
		std::uint32_t weightsOffset;
		char __pad3[0x170];
		std::uint32_t boneCounts[2];
		scr_string_t name;
		scr_string_t* boneNames;
		XSurfaceBoneRef* boneRefs;
		std::uint64_t* boneData;
	};

	struct XModelExtents
	{
		char __pad0[0x60];
	};

	struct XModelMeshPart
	{
		char __pad0[0x20];
	};

	struct XModelMeshSection
	{
		char __pad0[0x18];
	};

	struct XModelMeshInfo
	{
		const char* name;
		std::uint16_t tripleCount;
		std::uint16_t partCount;
		std::uint16_t sectionCount;
		char __pad0[0x2];
		XModelMeshPart* parts;
		std::uint8_t (*triples)[3];
		XModelMeshSection* sections;
	};

	struct XModelMeshEntry
	{
		char __pad0[0x2];
		std::uint16_t vertCount;
		std::uint32_t triCount;
		std::uint32_t vertexStart;
		std::uint32_t indexStart;
		XSurface* surface;
		XModelMeshInfo* info;
		XModelExtents* extents;
		char __pad1[0x8];
	};

	struct XModelMesh
	{
		XHash name;
		XModelMeshEntry* entries;
		XSurface* surface;
		XModelExtents* extents;
		std::uint64_t streamKey;
		char __pad0[0x14];
		std::uint16_t entryCount;
		char __pad1[0x2];
	};

	static_assert(sizeof(XSurfaceVertex) == 0x10);
	static_assert(sizeof(XSurfaceWeights) == 0xC);
	static_assert(sizeof(XSurface) == 0x1D0);
	static_assert(offsetof(XSurface, meshDataSize) == 0x28);
	static_assert(offsetof(XSurface, weightsOffset) == 0x38);
	static_assert(offsetof(XModelMesh, streamKey) == 0x20);
	static_assert(offsetof(XSurface, boneCounts) == 0x1AC);
	static_assert(offsetof(XSurface, boneNames) == 0x1B8);
	static_assert(sizeof(XModelMeshInfo) == 0x28);
	static_assert(sizeof(XModelMeshEntry) == 0x30);
	static_assert(sizeof(XModelMesh) == 0x40);
	static_assert(offsetof(XModelMesh, entryCount) == 0x3C);

	struct PhysPreset;
	struct PhysConstraints;
	struct TriggerActions;

	struct XCollisionMesh
	{
		std::uint16_t (*indices)[3];
		std::uint8_t (*triangles)[3];
		std::uint32_t* values;
		std::uint8_t* data;
		std::uint32_t dataSize;
		char __pad0[0x2];
		std::uint16_t valueCount;
		std::uint16_t triangleCount;
		std::uint16_t indexCount;
		char __pad1[0x24];
	};

	struct XCollisionHullEdge
	{
		char __pad0[0x1C];
	};

	struct XCollisionHullFace
	{
		char __pad0[0xC];
	};

	struct XCollisionHullPoly
	{
		char __pad0[0x20];
	};

	struct XCollisionHullRecord
	{
		char __pad0[0x2C];
	};

	struct XCollisionHull
	{
		char __pad0[0x18];
		std::uint32_t planeCount;
		std::uint32_t vertexCount;
		std::uint32_t edgeCount;
		char __pad1[0x4];
		std::uint32_t faceCount;
		std::uint32_t faceIndexCount;
		std::uint32_t polyCount;
		char __pad2[0x4];
		std::uint32_t recordCount;
		char __pad3[0x4];
		float (*planes)[4];
		float (*vertices)[3];
		std::uint64_t* packedEdges;
		std::uint8_t* extraData;
		std::uint32_t extraDataSize;
		char __pad4[0x4];
		std::uint32_t* edgeValues1;
		std::uint32_t* edgeValues2;
		std::uint32_t* edgeValues3;
		XCollisionHullEdge* edges;
		XCollisionHullFace* faces;
		std::uint8_t* faceIndices;
		std::uint8_t* polyIndices;
		std::uint32_t polyIndexCount;
		char __pad5[0x4];
		XCollisionHullPoly* polys;
		XCollisionHullRecord* records;
	};

	struct XCollisionBvh
	{
		std::uint64_t* nodes;
		std::uint32_t* leaves;
		std::uint8_t* data;
		std::uint32_t nodeCount;
		std::uint32_t dataSize;
	};

	struct XCollisionShape
	{
		XCollisionHull* hull;
		XCollisionBvh* bvh;
		char __pad0[0x8];
	};

	struct XCollisionData
	{
		std::uint8_t* bytes;
		XCollisionMesh* meshes;
		XCollisionShape* shapes;
		char __pad0[0x8];
		std::uint16_t byteCount;
		std::uint16_t meshCount;
		std::uint8_t shapeCount;
		char __pad1[0x3];
	};

	struct XCollision
	{
		XHash name;
		XSkeleton* skeleton;
		StreamKey* streamKey;
		XCollisionData* data;
		char __pad0[0x8];
		PhysPreset* physPreset;
		TriggerActions* triggerActions;
		PhysConstraints* physConstraints;
		char __pad1[0x20];
	};

	static_assert(sizeof(XCollisionMesh) == 0x50);
	static_assert(offsetof(XCollisionMesh, dataSize) == 0x20);
	static_assert(offsetof(XCollisionMesh, valueCount) == 0x26);
	static_assert(sizeof(XCollisionHull) == 0xB8);
	static_assert(offsetof(XCollisionHull, planes) == 0x40);
	static_assert(offsetof(XCollisionHull, extraDataSize) == 0x60);
	static_assert(offsetof(XCollisionHull, polyIndexCount) == 0xA0);
	static_assert(sizeof(XCollisionBvh) == 0x20);
	static_assert(sizeof(XCollisionShape) == 0x18);
	static_assert(sizeof(XCollisionData) == 0x28);
	static_assert(offsetof(XCollisionData, byteCount) == 0x20);
	static_assert(sizeof(XCollision) == 0x60);
	static_assert(offsetof(XCollision, physPreset) == 0x28);

	struct XModelLodMaterials
	{
		std::uint16_t count;
		char __pad0[0x6];
		Material** materials;
		Material** materials2;
		std::uint32_t* values;
	};

	struct XModel;

	struct XModelAttachment
	{
		XModel* model;
		scr_string_t tag;
		char __pad0[0x1C];
	};

	struct XModel
	{
		XHash name;
		XSkeleton* skeleton;
		XCollision* collision;
		XModelAttachment* attachments;
		XModelMesh* meshes[8];
		XModelLodMaterials* lodMaterials;
		XModelExtents* extents;
		std::uint16_t numLods;
		std::uint8_t lodScaleIndex;
		char __pad0;
		float lodThresholds[8];
		char __pad1[0x4A];
		std::int8_t attachmentCount;
		char __pad2[0x9];
	};

	static_assert(sizeof(XModelLodMaterials) == 0x20);
	static_assert(sizeof(XModelAttachment) == 0x28);
	static_assert(sizeof(XModel) == 0xE8);
	static_assert(offsetof(XModel, lodMaterials) == 0x60);
	static_assert(offsetof(XModel, attachmentCount) == 0xDE);

	struct FxEffectDef;
	struct FxImpactTable;

	struct PhysPreset
	{
		XHash name;
		char __pad0[0x50];
		FxEffectDef* fx;
		FxImpactTable* impactFxTable;
		ImpactSoundsTable* impactSoundsTable;
	};

	struct Ragdoll
	{
		XHash name;
		char __pad0[0xB90];
		ImpactSoundsTable* impactSoundsTable;
		char __pad1[0x10];
	};

	struct XAnimCurve
	{
		XHash name;
		float (*keys)[3];
		std::uint32_t keyCount;
		char __pad0[0xC];
	};

	struct TriggerActions
	{
		char __pad0[0x58];
	};

	struct PhysConstraintMaterialRecord
	{
		scr_string_t name;
		char __pad0[0x14];
		std::uint64_t valueCount;
		std::uint64_t* values;
	};

	struct PhysConstraintMaterial
	{
		char __pad0[0xA0];
		Material* material;
		char __pad1[0x40];
		PhysConstraintMaterialRecord records[6];
		char __pad2[0x8];
	};

	struct PhysConstraintPoint
	{
		char __pad0[0x24];
	};

	struct PhysConstraintCurveRef
	{
		XAnimCurve* curve;
		char __pad0[0x10];
	};

	struct PhysConstraintCurves
	{
		char __pad0[0x78];
		std::int32_t pointCount;
		char __pad1[0x4];
		PhysConstraintPoint* points;
		char __pad2[0x10];
		PhysConstraintCurveRef curves1[3];
		char __pad3[0x8];
		PhysConstraintCurveRef curves2[3];
	};

	struct PhysConstraintPoints
	{
		scr_string_t name;
		char __pad0[0x4];
		std::uint32_t pointCount;
		char __pad1[0x4];
		PhysConstraintPoint* points;
		char __pad2[0x1C8];
	};

	enum PhysConstraintType : std::uint32_t
	{
		PHYS_CONSTRAINT_CURVES = 16,
		PHYS_CONSTRAINT_NAMES = 17,
		PHYS_CONSTRAINT_POINTS = 18,
	};

	struct PhysConstraint
	{
		PhysConstraintType type;
		char __pad0[0x4];
		union
		{
			PhysConstraintMaterial material;
			PhysConstraintCurves curves;
			PhysConstraintPoints points;
		};
	};

	struct PhysConstraintBodyName
	{
		scr_string_t name;
		std::uint32_t value;
	};

	struct PhysConstraintBodyInfo
	{
		char __pad0[0x50];
	};

	struct PhysConstraints
	{
		XHash name;
		std::uint32_t constraintCount;
		std::uint32_t bodyNameCount;
		Ragdoll* ragdoll;
		PhysConstraintBodyInfo* bodyInfos;
		PhysConstraint* constraints;
		PhysConstraintBodyName* bodyNames;
	};

	static_assert(sizeof(PhysPreset) == 0x70);
	static_assert(offsetof(PhysPreset, fx) == 0x58);
	static_assert(sizeof(Ragdoll) == 0xBB0);
	static_assert(offsetof(Ragdoll, impactSoundsTable) == 0xB98);
	static_assert(sizeof(XAnimCurve) == 0x20);
	static_assert(sizeof(TriggerActions) == 0x58);
	static_assert(sizeof(PhysConstraintMaterialRecord) == 0x28);
	static_assert(sizeof(PhysConstraintMaterial) == 0x1E0);
	static_assert(offsetof(PhysConstraintMaterial, records) == 0xE8);
	static_assert(sizeof(PhysConstraintCurves) == 0x130);
	static_assert(offsetof(PhysConstraintCurves, points) == 0x80);
	static_assert(offsetof(PhysConstraintCurves, curves1) == 0x98);
	static_assert(offsetof(PhysConstraintCurves, curves2) == 0xE8);
	static_assert(sizeof(PhysConstraintPoints) == 0x1E0);
	static_assert(offsetof(PhysConstraintPoints, points) == 0x10);
	static_assert(sizeof(PhysConstraint) == 0x1E8);
	static_assert(sizeof(PhysConstraints) == 0x30);
	static_assert(offsetof(PhysConstraints, bodyNames) == 0x28);

	struct GfxBuffer
	{
		std::uint8_t* data;
		std::uint32_t count;
		std::uint32_t stride;
		char __pad0[0x28];
	};

	struct KlfEntry
	{
		const char* name;
		char __pad0[0x280];
		GfxBuffer* buffer;
		GfxImage* image;
		char __pad1[0x30];
	};

	struct KlfRecord
	{
		char __pad0[0x1A8];
	};

	struct Klf
	{
		const char* name;
		char __pad0[0x10];
		KlfEntry* entries;
		KlfRecord* records;
		std::uint64_t* values;
		GfxBuffer* buffer1;
		GfxBuffer* buffer2;
		std::uint16_t entryCount;
		std::uint16_t valueCount;
		char __pad1[0x4];
		GfxImage* image;
		char __pad2[0x8];
	};

	struct VectorField
	{
		XHash name;
		std::uint8_t* data;
		char __pad0[0x3C];
		std::uint32_t dataSize;
		char __pad1[0x8];
	};

	struct FxLightDef
	{
		char __pad0[0x2A8];
		GfxImage* image;
		char __pad1[0x8];
	};

	enum FxElemType : std::uint8_t
	{
		FX_ELEM_TYPE_TRAIL = 5,
		FX_ELEM_TYPE_MODEL = 7,
		FX_ELEM_TYPE_LIGHT = 8,
		FX_ELEM_TYPE_UNKNOWN_9 = 9,
		FX_ELEM_TYPE_SOUND = 10,
		FX_ELEM_TYPE_KLF = 11,
		FX_ELEM_TYPE_DECAL = 13,
		FX_ELEM_TYPE_RUNNER = 14,
		FX_ELEM_TYPE_UNKNOWN_15 = 15,
		FX_ELEM_TYPE_UNKNOWN_16 = 16,
		FX_ELEM_TYPE_VECTOR_FIELD = 17,
	};

	union FxElemVisual
	{
		const void* anonymous;
		Material* material;
		XModel* model;
		FxEffectDef* effectDef;
		FxLightDef* lightDef;
		VectorField* vectorField;
	};

	struct FxElemVisualEntry
	{
		FxElemVisual visual;
		char __pad0[0x38];
	};

	struct FxElemDecalVisuals
	{
		Material* materials[2];
	};

	union FxElemVisuals
	{
		FxElemVisual instance;
		FxElemVisualEntry* array;
		FxElemDecalVisuals* decalArray;
	};

	struct FxTrailVertex
	{
		char __pad0[0x14];
	};

	struct FxTrailDef
	{
		char __pad0[0x14];
		std::int32_t vertCount;
		FxTrailVertex* verts;
		std::int32_t indCount;
		char __pad1[0x4];
		std::uint16_t* inds;
	};

	union FxElemExtendedDef
	{
		FxTrailDef* trailDef;
		FxLightDef* lightDef;
		Klf* klf;
	};

	struct FxElemVelSample
	{
		char __pad0[0x60];
	};

	struct FxElemVisSample
	{
		char __pad0[0x30];
	};

	struct FxElemRecord
	{
		char __pad0[0x78];
	};

	struct SurfaceFxTable;

	struct FxElemDef
	{
		FxElemVisuals visuals;
		char __pad0[0x38];
		FxElemVisuals secondaryVisuals;
		char __pad1[0xC8];
		FxElemExtendedDef extended;
		FxElemVelSample* velSamples;
		std::uint64_t* samples2;
		std::uint64_t* samples3;
		FxElemVisSample* visSamples;
		std::uint32_t* values;
		FxElemRecord* records;
		char __pad2[0x18];
		FxEffectDef* effects[4];
		SurfaceFxTable* surfaceFxTable;
		char __pad3[0x164];
		std::uint32_t valueCount;
		char __pad4[0x38];
		FxElemType elemType;
		char __pad5[0x9];
		std::uint8_t visualCount;
		std::uint8_t velSampleCount;
		std::uint8_t sample2Count;
		std::uint8_t sample3Count;
		std::uint8_t visSampleCount;
		std::uint8_t recordCount;
		char __pad6[0x8];
	};

	struct FxEffectDef
	{
		XHash name;
		char __pad0[0x14];
		std::uint16_t elemDefCounts[3];
		char __pad1[0x6];
		FxElemDef* elemDefs;
		char __pad2[0x60];
	};

	struct SurfaceFxTable
	{
		XHash name;
		FxEffectDef* effects[40];
	};

	struct EntityFxImpactGroup
	{
		FxEffectDef* effects[6];
	};

	struct EntityFxImpacts
	{
		XHash name;
		EntityFxImpactGroup groups[12];
	};

	struct FxImpactTable
	{
		XHash name;
		SurfaceFxTable* surfaceFxTable;
		EntityFxImpacts* entityFxImpacts;
	};

	struct FxLibraryVolumeRecord4
	{
		char __pad0[0x30];
	};

	struct FxLibraryVolumeRecord5
	{
		char __pad0[0x20];
	};

	struct FxLibraryVolume
	{
		XHash name;
		std::uint64_t count1;
		std::uint64_t* values1;
		std::uint64_t count2;
		std::uint64_t* values2;
		std::uint64_t count3;
		std::uint64_t* values3;
		std::uint64_t count4;
		FxLibraryVolumeRecord4* values4;
		std::uint64_t count5;
		FxLibraryVolumeRecord5* values5;
		std::uint64_t count6;
		float (*values6)[3];
		std::uint64_t count7;
		std::uint32_t* values7;
		std::uint64_t count8;
		std::uint32_t* values8;
		std::uint64_t materialCount;
		Material** materials;
		std::uint64_t imageCount;
		GfxImage** images;
		std::uint64_t count11;
		float (*values11)[4];
		std::uint64_t count12;
		std::uint64_t* values12;
		char __pad0[0x128];
		std::uint32_t bufferSizes[4];
		std::uint8_t* buffers[4];
		char __pad1[0xC00];
	};

	struct GlassDef
	{
		char __pad0[0x28];
		Material* materials[3];
		char __pad1[0x18];
		FxEffectDef* effects[2];
	};

	struct GlassEntry
	{
		GlassDef* def;
		float origin[3];
		float angles[3];
		float mins[3];
		float maxs[3];
		float radius;
		char __pad0[0x9];
		std::uint8_t valueCount;
		char __pad1[0x2];
		std::uint32_t (*values)[2];
		char __pad2[0x40];
	};

	struct Glasses
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		GlassEntry* entries;
		std::uint8_t* data;
		std::uint32_t dataSize;
		char __pad1[0x2C];
	};

	enum TriggerEntryType : std::uint16_t
	{
		TRIGGER_ENTRY_TYPE_STRING = 2,
		TRIGGER_ENTRY_TYPE_VECTOR = 3,
		TRIGGER_ENTRY_TYPE_HASH = 4,
		TRIGGER_ENTRY_TYPE_FLOAT = 5,
		TRIGGER_ENTRY_TYPE_INT = 6,
	};

	union TriggerEntryValue
	{
		const char* string;
		float vector[3];
		XHash hash;
		float number;
		std::uint32_t integer;
	};

	struct TriggerEntry
	{
		const char* key;
		TriggerEntryValue value;
		TriggerEntryType type;
		char __pad0[0x6];
	};

	struct TriggerGroup
	{
		std::uint32_t entryCount;
		char __pad0[0x4];
		TriggerEntry* entries;
		char __pad1[0x20];
	};

	struct TriggerModel
	{
		std::uint32_t contents;
		std::uint16_t hullCount;
		std::uint16_t firstHull;
	};

	struct TriggerHull
	{
		float midPoint[3];
		float halfSize[3];
		std::uint32_t contents;
		std::uint16_t slabCount;
		std::uint16_t firstSlab;
	};

	struct TriggerSlab
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct TriggerList
	{
		XHash name;
		std::uint32_t modelCount;
		char __pad0[0x4];
		TriggerModel* models;
		std::uint32_t hullCount;
		char __pad1[0x4];
		TriggerHull* hulls;
		std::uint32_t slabCount;
		char __pad2[0x4];
		TriggerSlab* slabs;
		std::uint64_t groupCount;
		TriggerGroup* groups;
	};

	struct XAnimPartTransFrames
	{
		float mins[3];
		float size[3];
		union
		{
			std::uint8_t (*byteFrames)[3];
			std::uint16_t (*shortFrames)[3];
		} frames;
	};

	struct XAnimPartTrans
	{
		std::uint16_t size;
		std::uint8_t smallTrans;
		char __pad0[0x5];
		union
		{
			XAnimPartTransFrames frames;
			float frame0[3];
		} u;
	};

	struct XAnimDeltaPartQuat2
	{
		std::uint16_t size;
		char __pad0[0x6];
		union
		{
			std::int16_t (*frames)[2];
			std::int16_t frame0[2];
		} u;
	};

	struct XAnimDeltaPartQuat
	{
		std::uint16_t size;
		char __pad0[0x6];
		union
		{
			std::int16_t (*frames)[4];
			std::int16_t frame0[4];
		} u;
	};

	struct XAnimDeltaPart
	{
		XAnimPartTrans* trans;
		XAnimDeltaPartQuat2* quat2;
		XAnimDeltaPartQuat* quat;
	};

	struct XAnimRecord32
	{
		char __pad0[0x20];
	};

	struct XAnimRecord24
	{
		char __pad0[0x18];
	};

	struct XAnimBoneTrack
	{
		std::uint16_t count;
		char __pad0[0x6];
		std::uint16_t* indices;
		float (*values)[3];
	};

	struct XAnimExtraRecord
	{
		char __pad0[0x16];
	};

	struct XAnimExtra
	{
		XAnimExtraRecord* records;
		std::int32_t recordCount;
		char __pad0[0x5C];
	};

	struct XAnim
	{
		std::uint64_t* names;
		std::uint32_t* boneNames;
		XAnimBoneTrack* boneTracks;
		XAnimDeltaPart* deltaPart;
		XAnimRecord32* records1;
		std::uint64_t* values2;
		XAnimRecord24* records3;
		XAnimRecord32* records4;
		XAnimRecord32* records5;
		XAnimRecord32* records6;
		XAnimExtra* extra;
		std::uint8_t* dataByte;
		std::uint8_t* dataByte2;
		std::uint8_t* dataByte3;
		XHash name;
		std::uint32_t* dataInt;
		std::uint32_t* dataInt2;
		std::uint16_t* dataShort;
		std::uint16_t* dataShort2;
		std::uint8_t* indices;
		char __pad0[0x8];
		StreamKey* streamKey;
		char __pad1[0x10];
		std::uint32_t indexCount;
		std::uint32_t records1Count;
		std::uint32_t values2Count;
		std::uint32_t dataShort2Count;
		char __pad2[0x4];
		std::uint32_t dataByteCount;
		std::uint32_t dataIntCount;
		std::uint32_t dataShortCount;
		std::uint32_t dataByte2Count;
		std::uint32_t dataByte3Count;
		std::uint32_t dataInt2Count;
		char __pad3[0x12];
		std::uint16_t boneNameCount;
		std::uint16_t records4Count;
		char __pad4[0x2];
		std::uint16_t numframes;
		std::uint16_t nameCount;
		char __pad5[0xD];
		std::uint8_t records3Count;
		std::uint8_t records5Count;
		std::uint8_t records6Count;
		char __pad6[0x8];
	};

	struct DestructibleStageRecord
	{
		char __pad0[0x8];
		scr_string_t names[4];
	};

	struct DestructibleStage
	{
		char __pad0[0x8];
		PhysPreset* physPreset;
		DestructibleStageRecord* records;
		FxEffectDef* effect;
		scr_string_t names[6];
		char __pad1[0xC];
		std::uint32_t recordCount;
	};

	struct DestructiblePiece
	{
		DestructibleStage stages[7];
		char __pad0[0x28];
		scr_string_t name;
		char __pad1[0x4];
		PhysConstraints* physConstraints;
		char __pad2[0x10];
		FxEffectDef* effect;
		char __pad3[0x8];
		scr_string_t name2;
		char __pad4[0x4];
	};

	struct DestructibleDef
	{
		XHash name;
		XModel* models[3];
		std::int32_t pieceCount;
		char __pad0[0x4];
		DestructiblePiece* pieces;
		char __pad1[0x8];
	};

	struct ScriptBundleKeyValue
	{
		char __pad0[0x14];
		scr_string_t stringValue;
		char __pad1[0x8];
	};

	struct ScriptBundleObject;

	struct ScriptBundleObjectList
	{
		char __pad0[0x10];
		std::uint64_t objectCount;
		ScriptBundleObject* objects;
	};

	struct ScriptBundleObject
	{
		std::uint64_t keyValueCount;
		ScriptBundleKeyValue* keyValues;
		std::uint64_t listCount;
		ScriptBundleObjectList* lists;
	};

	struct ScriptBundle
	{
		XHash name;
		char __pad0[0x8];
		ScriptBundleObject object;
	};

	struct EntityList
	{
		XHash name;
		std::uint64_t groupCount;
		TriggerGroup* groups;
	};

	struct CpuOcclusionEntry
	{
		char __pad0[0x1C];
		scr_string_t name;
		std::uint64_t count1;
		std::uint32_t (*data1)[3];
		std::uint64_t count2;
		std::uint16_t* data2;
		std::uint64_t count3;
		std::uint16_t* data3;
	};

	struct CpuOcclusionRecord
	{
		char __pad0[0x28];
	};

	struct CpuOcclusionBuffer
	{
		std::uint64_t dataSize;
		std::uint8_t* data;
		std::uint64_t count1;
		std::uint32_t* data1;
		std::uint64_t count2;
		std::uint32_t* data2;
	};

	struct CpuOcclusionData
	{
		XHash name;
		std::uint64_t entryCount;
		CpuOcclusionEntry* entries;
		std::uint64_t records1Count;
		CpuOcclusionRecord* records1;
		std::uint64_t records2Count;
		CpuOcclusionRecord* records2;
		std::uint64_t nameCount;
		scr_string_t* names;
		CpuOcclusionBuffer buffer1;
		CpuOcclusionBuffer buffer2;
		std::uint8_t (*runtimeData1)[16];
		std::uint32_t* runtimeData2;
	};

	enum ComPrimaryLightType : std::uint32_t
	{
		COM_PRIMARY_LIGHT_TYPE_LOCAL = 2,
		COM_PRIMARY_LIGHT_TYPE_AREA = 4,
	};

	struct ComPrimaryLight
	{
		float projection[16];
		ComPrimaryLightType type;
		char __pad0[0x24];
		float origin[3];
		float dir[3];
		float widthAxis[3];
		float heightAxis[3];
		char __pad1[0x30];
		float color[3];
		char __pad2[0x8];
		float cosHalfFovOuter;
		float feather;
		float reflectorEfficiency;
		float radius;
		char __pad3[0x4];
		float apexOffset[2];
		float emitterSize[2];
		char __pad4[0x100];
		float lodFadeRange;
		float lodFadeEnd;
		char __pad5[0x98];
		GfxImage* image;
		char __pad6[0x8];
	};

	struct ComRecord20
	{
		char __pad0[0x14];
	};

	struct ComRecord28
	{
		char __pad0[0x1C];
	};

	template <typename T>
	struct ComRecordGroup
	{
		char __pad0[0x8];
		std::uint32_t recordCount;
		char __pad1[0x4];
		T* records;
	};

	struct ComWorld
	{
		XHash name;
		char __pad0[0x4];
		std::uint32_t primaryLightCount;
		ComPrimaryLight* primaryLights;
		std::uint32_t groups20Count;
		char __pad1[0x4];
		ComRecordGroup<ComRecord20>* groups20;
		std::uint32_t values1Count;
		char __pad2[0x4];
		std::uint32_t* values1;
		std::uint32_t values2Count;
		char __pad3[0x4];
		std::uint32_t* values2;
		std::uint32_t groups28Count;
		char __pad4[0x4];
		ComRecordGroup<ComRecord28>* groups28;
	};

	struct WindDef
	{
		XHash name;
		char __pad0[0x78];
	};

	struct ZBarrierBoard
	{
		XModel* models[3];
		char __pad0[0x20];
		FxEffectDef* effects[2];
		char __pad1[0x40];
	};

	struct ZBarrier
	{
		XHash name;
		char __pad0[0x70];
		XModel* model;
		ZBarrierBoard boards[6];
	};

	static_assert(sizeof(ZBarrierBoard) == 0x88);
	static_assert(sizeof(ZBarrier) == 0x3B0);
	static_assert(offsetof(ZBarrier, model) == 0x78);

	struct StreamerHintGroup
	{
		char __pad0[0x14];
		std::int32_t entryCount;
		std::uint32_t (*entries)[2];
		std::int32_t indexCount;
		char __pad1[0x4];
		std::uint16_t* indices;
		std::int32_t valueCount;
		char __pad2[0x4];
		std::uint32_t (*values)[2];
	};

	struct StreamerHint
	{
		XHash name;
		std::int32_t recordCount;
		char __pad0[0x4];
		std::uint8_t (*records)[0x18];
		std::int32_t groupCount;
		char __pad1[0x4];
		StreamerHintGroup* groups;
	};

	static_assert(sizeof(StreamerHintGroup) == 0x40);
	static_assert(sizeof(StreamerHint) == 0x28);

	struct FeatureTable
	{
		XHash name;
		char __pad0[0x8];
	};

	struct WeaponFrontend
	{
		XHash name;
		ScriptBundleObject bundle;
	};

	struct PlaylistInfo
	{
		char __pad0[0x18];
	};

	struct PlaylistSettingString
	{
		char __pad0[0x8];
		const char* string;
	};

	struct PlaylistSettingValue
	{
		char __pad0[0x10];
	};

	struct PlaylistSetting
	{
		char __pad0[0x10];
		const char* name;
		char __pad1[0x8];
		std::int32_t stringCount;
		PlaylistSettingString* strings;
		std::uint32_t valueCount;
		PlaylistSettingValue* values;
		char __pad2[0x8];
	};

	struct PlaylistSettingGroup
	{
		char __pad0[0x28];
		PlaylistSetting* settings;
		std::int32_t settingCount;
	};

	struct PlaylistString
	{
		char __pad0[0x10];
		const char* string;
	};

	struct PlaylistVariant
	{
		PlaylistInfo* info;
		PlaylistSettingGroup* settingGroup;
		char __pad0[0x8];
	};

	struct Playlist
	{
		char __pad0[0x28];
		const char* string0;
		GfxImage* images[6];
		char __pad1[0x58];
		const char* string1;
		char __pad2[0x38];
		PlaylistString* strings;
		std::int32_t stringCount;
		PlaylistSetting* settings;
		std::int32_t settingCount;
		PlaylistVariant* variants;
		std::int32_t variantCount;
		char __pad3[0x8];
		ScriptBundleObject bundle;
	};

	struct PlaylistSet
	{
		char __pad0[0x28];
		Playlist* playlists;
		std::int32_t playlistCount;
	};

	struct Playlists
	{
		XHash name;
		char __pad0[0x10];
		PlaylistInfo** infos;
		std::int32_t infoCount;
		PlaylistSettingGroup** settingGroups;
		std::int32_t settingGroupCount;
		PlaylistSet* sets;
		std::int32_t setCount;
	};

	static_assert(sizeof(WeaponFrontend) == 0x28);
	static_assert(sizeof(PlaylistSetting) == 0x48);
	static_assert(sizeof(PlaylistSettingGroup) == 0x38);
	static_assert(sizeof(PlaylistString) == 0x18);
	static_assert(sizeof(PlaylistVariant) == 0x18);
	static_assert(sizeof(Playlist) == 0x150);
	static_assert(offsetof(Playlist, string1) == 0xB8);
	static_assert(offsetof(Playlist, strings) == 0xF8);
	static_assert(offsetof(Playlist, bundle) == 0x130);
	static_assert(sizeof(PlaylistSet) == 0x38);
	static_assert(sizeof(Playlists) == 0x48);
	static_assert(offsetof(Playlists, sets) == 0x38);

	struct PlaylistScheduleEntry
	{
		char __pad0[0x14];
		std::int32_t valueCount;
		std::uint64_t* values;
	};

	struct PlaylistSchedule
	{
		XHash name;
		char __pad0[0x18];
		std::int32_t entryCount;
		PlaylistScheduleEntry* entries;
	};

	struct ArenaSeasonName
	{
		char text[0x32];
	};

	struct ArenaSeason
	{
		char __pad0[0x8];
		ArenaSeasonName* seasonName;
		std::uint64_t* values;
		std::int32_t valueCount;
	};

	struct ArenaSeasonRange
	{
		char __pad0[0x10];
	};

	struct ArenaSeasons
	{
		XHash name;
		char __pad0[0x18];
		ArenaSeason* seasons;
		std::int32_t seasonCount;
		ArenaSeasonRange* ranges;
		std::int32_t rangeCount;
	};

	struct ScenarioItem
	{
		char __pad0[0xD0];
	};

	struct ScenarioGroup
	{
		char __pad0[0x18];
		std::uint64_t valueCount;
		std::uint64_t* values;
		char __pad1[0x8];
	};

	struct Scenario
	{
		XHash name;
		std::uint64_t itemCount;
		ScenarioItem* items;
		std::uint64_t groupCount;
		ScenarioGroup* groups;
	};

	static_assert(sizeof(PlaylistScheduleEntry) == 0x20);
	static_assert(sizeof(PlaylistSchedule) == 0x30);
	static_assert(sizeof(ArenaSeason) == 0x20);
	static_assert(sizeof(ArenaSeasons) == 0x40);
	static_assert(offsetof(ArenaSeasons, ranges) == 0x30);
	static_assert(sizeof(ScenarioGroup) == 0x30);
	static_assert(sizeof(Scenario) == 0x28);

	struct RenderTargetNode
	{
		char __pad0[0x8];
		RenderTargetNode* next;
		char __pad1[0x20];
	};

	struct RenderTargetDef
	{
		char __pad0[0x18];
		RenderTargetDef* child;
		RenderTargetNode* nodes;
		char __pad1[0x28];
	};

	struct RenderTargetEntry
	{
		char __pad0[0x10];
		RenderTargetDef* def;
		std::uint64_t defCount;
		RenderTargetDef** defs;
	};

	struct RenderTargets
	{
		XHash name;
		std::uint64_t entryCount;
		RenderTargetEntry* entries;
	};

	static_assert(sizeof(RenderTargetNode) == 0x30);
	static_assert(sizeof(RenderTargetDef) == 0x50);
	static_assert(sizeof(RenderTargetEntry) == 0x28);
	static_assert(sizeof(RenderTargets) == 0x18);

	struct HwPlatformList
	{
		std::uint64_t count;
		std::uint8_t (*values)[0x10];
	};

	struct HwPlatform
	{
		XHash name;
		HwPlatformList lists[5];
		char __pad0[0x10];
	};

	static_assert(sizeof(HwPlatform) == 0x68);

	struct RumbleGraph
	{
		char __pad0[0x90];
	};

	struct RumbleInfo
	{
		XHash name;
		char __pad0[0x8];
		RumbleGraph* highRumbleGraph;
		RumbleGraph* lowRumbleGraph;
		char __pad1[0x20];
		scr_string_t tagName;
		char __pad2[0x5C];
	};

	static_assert(sizeof(RumbleInfo) == 0xA0);
	static_assert(offsetof(RumbleInfo, tagName) == 0x40);

	struct BlackboardEntry
	{
		char __pad0[0x8];
		std::int32_t type;
		char __pad1[0x4];
		std::uint8_t* value;
		char __pad2[0x18];
		std::uint64_t* values;
		std::int32_t valueCount;
		char __pad3[0xC];
	};

	static_assert(sizeof(BlackboardEntry) == 0x48);

	struct Blackboard
	{
		XHash name;
		BlackboardEntry* entries;
		std::int32_t entryCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(Blackboard) == 0x18);

	struct DDLMember
	{
		char __pad0[0x20];
	};

	struct DDLRecord
	{
		char __pad0[0x10];
	};

	struct DDLStruct
	{
		char __pad0[0xC];
		std::int32_t memberCount;
		DDLMember* members;
		DDLRecord* tableA;
		std::int32_t tableACount;
		char __pad1[0x4];
		DDLRecord* tableB;
		std::int32_t tableBCount;
		char __pad2[0x4];
	};

	static_assert(sizeof(DDLStruct) == 0x38);

	struct DDLEnum
	{
		char __pad0[0x8];
		DDLRecord* members;
		std::int32_t memberCount;
		char __pad1[0x4];
		std::uint64_t* values;
		std::int16_t valueCount;
		char __pad2[0x6];
	};

	static_assert(sizeof(DDLEnum) == 0x28);

	struct DDLDef
	{
		char __pad0[0x20];
		DDLStruct* structs;
		DDLEnum* enums;
		DDLDef* next;
		char __pad1[0x8];
		std::int32_t structCount;
		std::int32_t enumCount;
		char __pad2[0x10];
	};

	static_assert(sizeof(DDLDef) == 0x58);

	struct DDLFile
	{
		XHash name;
		char __pad0[0x8];
		DDLDef* ddlDef;
	};

	static_assert(sizeof(DDLFile) == 0x18);

	struct LightingImageRef
	{
		char __pad0[0x18];
		GfxImage* image;
		char __pad1[0x8];
	};

	struct LightingProbe
	{
		StreamKey* streamKey;
		std::uint32_t* values1;
		std::uint32_t* values2;
		char __pad0[0x4];
		std::uint32_t valueCount;
		char __pad1[0x30];
		LightingImageRef* imageRef1;
		GfxBuffer* buffer;
		LightingImageRef* imageRef2;
	};

	struct LightingProbeSlot
	{
		char __pad0[0x60];
		LightingProbe* probe;
		char __pad1[0x8];
	};

	struct LightingProbeSet
	{
		char __pad0[0x10];
		LightingProbeSlot slots[4];
	};

	struct LightingVolumeProbeSlot
	{
		char __pad0[0x70];
		LightingProbe* probe;
		char __pad1[0x8];
	};

	struct LightingVolumeGroup
	{
		std::uint64_t imageCount;
		GfxImage** images;
		std::uint8_t (*records1)[0x178];
		std::uint8_t (*records2)[0x25C];
		CpuOcclusionBuffer buffer;
		std::uint32_t records1Count;
		std::uint32_t records2Count;
	};

	struct LightingImageTriple
	{
		GfxImage* images[3];
		char __pad0[0x8];
	};

	struct LightingImageTripleList
	{
		std::uint64_t tripleCount;
		LightingImageTriple* triples;
	};

	struct LightingModelRef
	{
		XModel* model;
		char __pad0[0x10];
		GfxImage* image;
		Klf* klf;
	};

	struct LightingGridCell
	{
		char __pad0[0x3C];
		std::uint32_t valueCount;
		std::uint8_t (*values)[16];
		char __pad1[0x8];
	};

	struct LightingGridPoint
	{
		std::uint16_t sh[4];
		std::uint32_t color;
	};

	struct LightingGrid
	{
		float origin[3];
		float halfSize[3];
		float angles[3];
		char __pad0[0x8];
		std::uint16_t sizeX;
		std::uint16_t sizeY;
		std::uint16_t sizeZ;
		std::uint16_t cellCount;
		char __pad1[0x4];
		LightingGridPoint* points;
		LightingGridCell* cells;
		char __pad2[0x8];
	};

	struct LightingGridTableEntry
	{
		std::uint32_t first;
		std::uint32_t count;
	};

	struct LightingGridSet
	{
		std::uint32_t gridCount;
		char __pad0[0x4];
		LightingGrid* grids;
		float mins[3];
		float maxs[3];
		std::uint32_t tableSize;
		std::uint32_t valueCount;
		LightingGridTableEntry* table;
		std::uint32_t* values;
	};

	struct LightingVolumeState
	{
		char __pad0[0x3C];
		float sunYaw;
		float sunPitch;
		char __pad1[0x8];
		float sunColor[3];
		float sunIntensity;
		char __pad2[0x7C8];
	};

	struct LightingVolume
	{
		char __pad0[0x10];
		LightingVolumeState states[4];
		char __pad1[0x40];
		LightingVolumeProbeSlot probeSlots[4];
		GfxImage* images[4];
		std::uint32_t valueCount;
		char __pad2[0x4];
		std::uint16_t* values;
		CpuOcclusionBuffer occlusionBuffer;
		LightingVolumeGroup groups[4];
		LightingImageTripleList imageTripleLists[4];
		LightingModelRef modelRefs[4];
		LightingGridSet gridSets[4];
		char __pad3[0x20];
	};

	struct LightingImageSet
	{
		char __pad0[0x50];
		GfxImage* images[6];
	};

	struct LightingWindRef
	{
		char __pad0[0x20];
		WindDef* windDef;
	};

	struct LightingImageRecord
	{
		GfxImage* image;
		char __pad0[0x30];
	};

	struct LightingImagePairRecord
	{
		GfxImage* images[2];
		char __pad0[0x18];
		std::uint32_t valueCount;
		char __pad1[0x4];
		std::uint32_t* values;
	};

	struct LightingImageValuesRecord
	{
		GfxImage* image;
		char __pad0[0x8];
		std::uint32_t valueCount;
		char __pad1[0x4];
		std::uint32_t* values1;
		std::uint32_t* values2;
		std::uint32_t* values3;
	};

	struct LightingImageRecordLarge
	{
		GfxImage* image;
		char __pad0[0x78];
	};

	struct Lighting
	{
		XHash name;
		std::uint32_t primaryLightCount;
		char __pad0[0x4];
		ComPrimaryLight* primaryLights;
		std::uint32_t groups20Count;
		char __pad1[0x4];
		ComRecordGroup<ComRecord20>* groups20;
		std::uint32_t groups28Count;
		char __pad2[0x4];
		ComRecordGroup<ComRecord28>* groups28;
		std::uint32_t volumeCount;
		char __pad3[0x4];
		LightingVolume* volumes;
		std::uint32_t values1Count;
		char __pad4[0x4];
		std::uint8_t (*values1)[16];
		std::uint32_t* volumeValues;
		std::uint32_t values2Count;
		char __pad5[0x4];
		std::uint32_t* values2;
		GfxImage* images[4];
		std::uint32_t probeSetCount;
		char __pad6[0x4];
		LightingProbeSet* probeSets;
		std::uint32_t values3Count;
		char __pad7[0x4];
		std::uint8_t (*values3)[16];
		std::uint64_t records1Count;
		std::uint8_t (*records1)[0x270];
		std::uint64_t values4Count;
		std::uint8_t (*values4)[16];
		std::uint64_t imageSetCount;
		LightingImageSet* imageSets;
		std::uint64_t values5Count;
		std::uint8_t (*values5)[16];
		std::uint64_t records2Count;
		std::uint8_t (*records2)[0x28];
		std::uint64_t values6Count;
		std::uint8_t (*values6)[16];
		std::uint32_t records3Count;
		char __pad8[0x4];
		std::uint8_t (*records3)[0x90];
		std::uint64_t records4Count;
		std::uint8_t (*records4)[0xE0];
		char __pad9[0x18];
		std::uint64_t records5Count;
		std::uint8_t (*records5)[0x168];
		std::uint64_t values7Count;
		std::uint8_t (*values7)[16];
		std::uint32_t values8Count;
		char __pad10[0x4];
		std::uint8_t (*values8)[16];
		std::uint32_t imageRecordCount;
		char __pad11[0x4];
		LightingImageRecord* imageRecords;
		std::uint32_t imagePairRecordCount;
		char __pad12[0x4];
		LightingImagePairRecord* imagePairRecords;
		std::uint32_t imageValuesRecordCount;
		char __pad13[0x4];
		LightingImageValuesRecord* imageValuesRecords;
		std::uint32_t imageRecordLargeCount;
		char __pad14[0x4];
		LightingImageRecordLarge* imageRecordsLarge;
		std::uint64_t windRefCount;
		LightingWindRef* windRefs;
		std::uint64_t values9Count;
		std::uint8_t (*values9)[16];
	};

	struct TerrainMesh
	{
		std::uint8_t* data1;
		char __pad0[0x40];
		std::uint8_t* data2;
		char __pad1[0x1C];
		std::uint32_t data1Size;
		std::uint32_t data2Size;
		std::uint32_t valueCount;
		std::uint16_t* values;
		char __pad2[0x18];
		StreamKey* streamKey;
		char __pad3[0x38];
	};

	struct TerrainMeshMaterial
	{
		char __pad0[0x48];
		Material* material;
		char __pad1[0x10];
	};

	struct TerrainCellMaterial
	{
		char __pad0[0x48];
		Material* material;
		char __pad1[0x40];
	};

	struct TerrainCell
	{
		TerrainCellMaterial* cellMaterial;
		TerrainMesh mesh;
		char __pad0[0x80];
	};

	struct TerrainCellGrid
	{
		std::uint32_t sizeX;
		std::uint32_t sizeY;
		TerrainCell* cells;
	};

	struct TerrainCellGridList
	{
		std::uint32_t gridCount;
		char __pad0[0x4];
		TerrainCellGrid* grids;
	};

	struct TerrainLayer
	{
		Material* material;
		std::uint8_t (*data)[0x16];
		char __pad0[0x120];
	};

	struct TerrainTile
	{
		std::uint32_t meshCount;
		char __pad0[0x4];
		TerrainMesh* meshes;
		TerrainMeshMaterial* meshMaterials;
		std::uint32_t indexCount;
		char __pad1[0x4];
		std::uint16_t* indices;
		char __pad2[0x18];
		TerrainCellGridList cellGrids;
		char __pad3[0x78];
		std::uint32_t values1Count;
		char __pad4[0x4];
		std::uint32_t* values1;
		char __pad5[0x30];
		std::uint64_t layerCount;
		TerrainLayer* layers;
		char __pad6[0x40];
		std::uint64_t values2Count;
		std::uint8_t (*values2)[8];
		GfxImage* image1;
		GfxImage* image2;
		GfxImage* image3;
		std::uint64_t imageCount;
		GfxImage** images;
		GfxImage* image4;
		GfxImage* image5;
		char __pad7[0x10];
		std::uint64_t values3Count;
		std::uint16_t* values3;
		GfxImage* image6;
		GfxImage* image7;
		char __pad8[0x8];
	};

	struct TerrainRegionPart
	{
		char __pad0[0x20];
		std::uint16_t bitCount;
		char __pad1[0x6];
		std::uint16_t* values;
		std::uint8_t* bits;
		std::uint8_t* data;
		char __pad2[0x8];
		std::uint32_t dataSize;
		char __pad3[0x4];
	};

	struct TerrainRegion
	{
		std::uint32_t partCount;
		char __pad0[0x14];
		TerrainRegionPart* parts;
		char __pad1[0x78];
		std::uint8_t (**entries)[0x18];
		std::uint32_t entryCount;
		char __pad2[0xC];
	};

	struct TerrainRegionList
	{
		char __pad0[0x3C];
		std::uint32_t regionCount;
		TerrainRegion* regions;
	};

	struct TerrainBlend
	{
		char __pad0[0x68];
		std::uint64_t values1Count;
		std::uint32_t* values1;
		std::uint64_t values2Count;
		std::uint32_t* values2;
		char __pad1[0x48];
		GfxImage* image;
		char __pad2[0x28];
	};

	struct TerrainPatch
	{
		char __pad0[0x10];
		std::uint64_t valueCount;
		std::uint8_t (*values)[0x3C];
		char __pad1[0x58];
	};

	struct GfxImageSet6
	{
		GfxImage* images[6];
	};

	struct TerrainImageBlock
	{
		char __pad0[0x18];
		GfxImage* image1;
		GfxImage* image2;
		GfxBuffer* buffer;
		std::uint64_t imageSetCount;
		GfxImageSet6* imageSets;
	};

	struct TerrainGfx
	{
		XHash name;
		std::uint32_t tileCount;
		char __pad0[0x4];
		TerrainTile* tiles;
		char __pad1[0x18];
		std::uint64_t imageCount;
		GfxImage** images;
		std::uint64_t values1Count;
		std::uint32_t* values1;
		char __pad2[0x48];
		TerrainRegionList* regionList;
		std::uint64_t values2Count;
		std::uint8_t (*values2)[64];
		std::uint64_t blendCount;
		TerrainBlend* blends;
		std::uint64_t patchCount;
		TerrainPatch* patches;
		std::uint64_t values3Count;
		std::uint8_t (*values3)[64];
		TerrainImageBlock imageBlock;
		char __pad3[0x8];
	};

	struct StreamerKeyRef
	{
		char __pad0[0x8];
		StreamKey* streamKey;
		char __pad1[0x20];
	};

	struct StreamerBitGrid
	{
		std::uint64_t cellCount;
		std::uint8_t (*cells)[32];
		std::uint8_t* bits;
		char __pad0[0x4];
		std::uint32_t bitRows;
		std::uint32_t bitStride;
		char __pad1[0x4];
		std::uint64_t pairCount;
		std::uint32_t (*pairs)[2];
		std::uint64_t tripleCount;
		std::uint32_t (*triples)[3];
	};

	struct StreamerValueList
	{
		char __pad0[0x8];
		std::uint64_t valueCount;
		std::uint8_t (*values)[36];
	};

	struct StreamerValueLists
	{
		std::uint64_t listCount;
		StreamerValueList* lists;
	};

	struct StreamerSection
	{
		std::uint64_t keyCount;
		StreamerKeyRef* keys;
		StreamerBitGrid bitGrid;
		StreamerValueLists* valueLists;
		std::uint64_t materialCount;
		Material** materials;
		std::uint64_t imageCount;
		GfxImage** images;
		char __pad0[0x28];
	};

	struct StreamerModelEntry
	{
		XModel* model;
		char __pad0[0x8];
	};

	struct StreamerModelLodGroup
	{
		char __pad0[0x38];
		std::uint32_t valueCount;
		char __pad1[0x4];
		std::uint8_t (*values)[0x34];
	};

	struct StreamerModelLod
	{
		std::uint32_t groupCount;
		char __pad0[0x4];
		StreamerModelLodGroup* groups;
	};

	struct StreamerModelSet
	{
		std::uint32_t modelCount;
		char __pad0[0x4];
		StreamerModelEntry* models;
		StreamerModelLod* lods;
		std::uint8_t (*modelData)[32];
		char __pad1[0x8];
		StreamKey* streamKey;
	};

	struct StreamerCell
	{
		char __pad0[0x8];
		StreamerSection section;
		StreamerModelSet modelSet;
		char __pad1[0x20];
	};

	struct StreamerRangeList
	{
		char __pad0[0x18];
		std::uint64_t rangeCount;
		std::uint32_t (*ranges)[2];
	};

	struct StreamerRangeGroup
	{
		char __pad0[0x8];
		std::uint64_t listCount;
		StreamerRangeList* lists;
	};

	struct StreamerPairList
	{
		char __pad0[0x8];
		std::uint64_t pairCount;
		std::uint32_t (*pairs)[2];
	};

	struct StreamerWorld
	{
		XHash name;
		char __pad0[0x8];
		std::uint64_t groupCount;
		StreamerRangeGroup* groups;
		std::uint64_t pairListCount;
		StreamerPairList* pairLists;
		std::uint64_t values1Count;
		std::uint64_t* values1;
		std::uint64_t values2Count;
		std::uint8_t (*values2)[0x28];
		std::uint64_t cellCount;
		StreamerCell* cells;
		std::uint64_t values3Count;
		std::uint8_t (*values3)[32];
		std::uint64_t values4Count;
		std::uint64_t* values4;
		char __pad1[0x30];
		std::uint64_t modelCount;
		XModel** models;
		std::uint64_t dataSize;
		std::uint8_t* data;
		TerrainGfx* terrainGfx;
		Lighting* lighting;
	};

	struct DynModelStateEffect
	{
		FxEffectDef* effect;
		char __pad0[0x10];
	};

	struct DynModel
	{
		DynModelStateEffect stateEffects[5];
		FxEffectDef* effects1[3];
		FxEffectDef* effects2[3];
		ScriptBundleObject bundle;
		XModel* model1;
		XModel* model2;
		PhysPreset* physPreset;
		XHash name;
		char __pad0[0x38];
	};

	struct ClipMapPartitionEntry
	{
		XCollision* collision;
		char __pad0[0x48];
	};

	struct ClipMapPartition
	{
		char __pad0[0x18];
		std::uint8_t* data;
		std::uint32_t dataSize;
		char __pad1[0x4];
		ClipMapPartitionEntry* entries;
		std::uint32_t entryCount;
		char __pad2[0xC];
	};

	struct ClipMapModelEntryList
	{
		std::uint8_t (*entries)[0x108];
		char __pad0[0x3C];
		std::uint16_t entryCount;
		char __pad1[0x2];
	};

	struct ClipMapModelData
	{
		std::uint8_t (*values1)[32];
		std::uint8_t (*values2)[64];
		std::uint8_t (*values3)[32];
		char __pad0[0x200];
	};

	struct ClipMapModel
	{
		char __pad0[0x18];
		std::uint8_t (*names)[0x390];
		ClipMapModelEntryList* entryList;
		char __pad1[0x18];
		XModel** models;
		std::uint16_t* modelValues;
		std::uint8_t (*values)[32];
		char __pad2[0x8];
		ClipMapModelData data;
		char __pad3[0x130];
		std::uint32_t* indices;
		std::uint32_t indexCount;
		char __pad4[0x1C];
		std::uint16_t dataCount2;
		char __pad5[0x6];
		std::int16_t dataCount1;
		std::uint8_t modelCount;
		std::uint8_t dataCount3;
		char __pad6[0x1];
		std::uint8_t valueCount;
		char __pad7[0x2];
	};

	struct ClipMapModelList
	{
		ClipMapModel** models;
		std::uint8_t (*values)[16];
		std::uint32_t modelCount;
		char __pad0[0x4];
	};

	struct ClipMapBrush
	{
		float mins[3];
		float maxs[3];
		std::uint32_t flags;
	};

	struct ClipMapBrushVertices
	{
		std::uint32_t firstVertex : 24;
		std::uint32_t vertexCount : 8;
	};

	struct ClipMapMeshLeaf
	{
		float mins[3];
		float maxs[3];
		std::uint32_t firstVertex : 23;
		std::uint32_t firstTriangleLow : 9;
		std::uint32_t firstTriangleHigh : 16;
		std::uint32_t unknown : 1;
		std::uint32_t triangleCount : 5;
		std::uint32_t surface : 10;
	};

	struct ClipMapMesh
	{
		float mins[3];
		float maxs[3];
		std::uint32_t planeCount;
		std::uint32_t brushVertexCount;
		std::uint32_t brushCount;
		char __pad1[0x4];
		std::uint32_t meshVertexCount;
		std::uint32_t meshIndexCount;
		std::uint32_t meshLeafCount;
		char __pad2[0x4];
		std::uint32_t count6;
		char __pad3[0x4];
		float (*planes)[4];
		float (*brushVertices)[3];
		std::uint64_t* bits;
		std::uint8_t* data1;
		std::uint32_t data1Size;
		char __pad4[0x4];
		std::uint32_t* brushFirstPlane;
		ClipMapBrushVertices* brushVertexRanges;
		std::uint32_t* values5;
		ClipMapBrush* brushes;
		float (*meshVertices)[3];
		std::uint8_t* meshIndices;
		std::uint8_t* data3;
		std::uint32_t data3Size;
		char __pad5[0x4];
		ClipMapMeshLeaf* meshLeaves;
		std::uint8_t (*values9)[0x2C];
	};

	struct ClipMapTable
	{
		std::uint64_t* values1;
		std::uint32_t* values2;
		std::uint8_t* data;
		std::uint32_t valueCount;
		std::uint32_t dataSize;
	};

	struct ClipMapDynModel
	{
		DynModel* dynModel;
		float quat[4];
		float origin[3];
		char __pad0[0x8];
		float scale;
		char __pad1[0x30];
	};

	struct ClipMap
	{
		XHash name;
		ClipMapPartition* partitions;
		std::uint32_t* partitionValues;
		ClipMapModel** models1;
		ClipMapModel** models2;
		ClipMapModelList* modelList;
		ClipMapMesh* meshes;
		ClipMapTable* table;
		XCollision** collisions;
		std::uint32_t* values1;
		std::uint16_t* values2;
		std::uint16_t dynModelCount;
		std::uint16_t runtimeCount;
		char __pad0[0x4];
		ClipMapDynModel* dynModels;
		std::uint8_t (*runtime1)[0xF8];
		std::uint8_t (*runtime2)[0x118];
		std::uint8_t (*runtime3)[0x1B8];
		std::uint8_t (*runtime4)[0x2C];
		std::uint8_t (*runtime5)[0x2C];
		PhysConstraint* constraints;
		std::uint8_t (*runtime6)[0x11A0];
		TerrainRegionList* regionList;
		std::uint32_t scriptStrings[0x40];
		std::uint32_t collisionCount;
		std::uint32_t models1Count;
		std::uint32_t models2Count;
		std::uint32_t meshCount;
		std::uint32_t values1Count;
		std::uint32_t values2Count;
		std::uint32_t constraintCount;
		std::uint32_t runtime6Count;
		char __pad1[0x6];
		std::uint16_t partitionCount;
		char __pad2[0x8];
	};

	struct NavMeshEntry
	{
		char __pad0[0x28];
		StreamKey* streamKey;
		char __pad1[0x10];
	};

	struct NavMesh
	{
		XHash name;
		StreamKey* streamKey1;
		char __pad0[0x10];
		std::uint32_t entryCount;
		char __pad1[0x4];
		NavMeshEntry* entries;
		char __pad2[0x18];
		StreamKey* streamKey2;
		char __pad3[0x18];
	};

	struct NavVolumeEntry
	{
		char __pad0[0x8];
		StreamKey* streamKey1;
		char __pad1[0x10];
		StreamKey* streamKey2;
		char __pad2[0x10];
	};

	struct NavVolume
	{
		XHash name;
		std::int32_t entryCount;
		char __pad0[0x4];
		NavVolumeEntry* entries;
	};

	struct GameMapEntity
	{
		std::uint8_t (*values)[12];
		char __pad0[0x6C];
		std::uint16_t valueCount;
		char __pad1[0x3A];
	};

	struct GameMapLinkValues
	{
		std::int32_t count;
		char __pad0[0x4];
		std::uint16_t* values;
	};

	union GameMapLinkData
	{
		void* pointers[2];
		GameMapLinkValues values;
	};

	struct GameMapLink
	{
		std::int32_t type;
		char __pad0[0x4];
		GameMapLinkData data;
	};

	struct GameMapData
	{
		char __pad0[0x4];
		std::uint32_t entityCount;
		GameMapEntity* entities;
		std::uint8_t (*runtimeData)[16];
		std::uint32_t dataSize;
		char __pad1[0x4];
		std::uint8_t* data;
		std::int32_t linkCount;
		char __pad2[0x4];
		GameMapLink* links;
	};

	struct GameMap
	{
		XHash name;
		GameMapData data;
		std::uint8_t (*grid1)[0x1A0008];
		std::uint8_t (*grid2)[0x1A0008];
	};

	struct SAnimData
	{
		std::uint8_t* data;
		char __pad0[0x68];
		std::uint32_t dataSize;
		char __pad1[0xC];
	};

	struct SAnim
	{
		const char* name;
		char __pad0[0x8];
		SAnimData* data;
		std::uint8_t (*values1)[12];
		std::uint8_t (*values2)[0x1C];
		char __pad1[0x3A];
		std::uint16_t values1Count;
		char __pad2[0x4];
		std::uint16_t values2Count;
		char __pad3[0x6];
	};

	struct XCamNotes
	{
		scr_string_t name;
		std::int32_t count;
		std::uint8_t (*values)[0x1C];
	};

	struct XCamTrack
	{
		char __pad0[0x8];
		scr_string_t name;
		char __pad1[0x4];
		std::int32_t keyCount;
		char __pad2[0x4];
		std::uint8_t (*keys)[48];
		char __pad3[0x8];
	};

	struct XCamTarget
	{
		char __pad0[0x8];
		scr_string_t names[4];
		char __pad1[0x8];
	};

	struct XCam
	{
		XHash name;
		char __pad0[0x14];
		std::int32_t trackCount;
		std::int32_t values1Count;
		char __pad1[0x4];
		std::int32_t nameCount;
		char __pad2[0x1C];
		XCamNotes* notes;
		XCamTrack* tracks;
		std::uint8_t (*values1)[16];
		scr_string_t (*names)[2];
		XCamTarget* targets1;
		std::uint8_t target1Count;
		char __pad3[0x7];
		XCamTarget* targets2;
		std::uint8_t target2Count;
		char __pad4[0x7];
		XCamTarget* targets3;
		std::uint8_t target3Count;
		char __pad5[0x1F];
	};

	struct BGCacheInfo
	{
		std::uint64_t type;
		XHash name;
		std::uint64_t asset;
	};

	struct BGCache
	{
		XHash name;
		BGCacheInfo* infos;
		std::uint32_t infoCount;
		char __pad0[0x4];
	};

	template <typename T>
	struct GfxArray
	{
		std::uint32_t count;
		char __pad0[0x4];
		T* values;
	};

	template <typename T>
	struct GfxList
	{
		std::uint64_t count;
		T* values;
	};

	struct GroupLodModel
	{
		XHash name;
		XModel* model;
		GfxList<XModelMesh*> meshes;
		char __pad0[0x8];
	};

	struct GfxWorldGpuBuffer
	{
		void* data;
		std::uint32_t size;
		char __pad0[0x4];
		std::uint64_t resource;
	};

	struct GfxWorldVertex
	{
		std::uint32_t color;
		float texCoord[2];
		std::uint32_t normal;
		std::uint32_t tangent;
	};

	struct GfxWorldVertexData
	{
		char __pad0[0x8];
		GfxImage* image;
		std::uint32_t vertexCount;
		char __pad1[0x4];
		float (*positions)[3];
		GfxWorldGpuBuffer positionBuffer;
		GfxWorldVertex* vertices;
		GfxWorldGpuBuffer vertexBuffer;
		std::uint32_t indexCount;
		char __pad2[0x4];
		std::uint16_t* indices;
		GfxWorldGpuBuffer indexBuffer;
		char __pad3[0x28];
	};

	struct GfxWorldSurface
	{
		char __pad0[0xC];
		std::uint32_t baseVertex;
		char __pad1[0x16];
		std::uint16_t triCount;
		std::uint32_t baseIndex;
		char __pad2[0x1C];
		Material* material;
		char __pad3[0x10];
	};

	struct GfxWorldValueTable
	{
		char __pad0[0x8];
		GfxList<std::uint32_t> values;
	};

	struct GfxWorldDataTable
	{
		GfxList<std::uint8_t> data;
		GfxList<std::uint32_t> values1;
		GfxList<std::uint32_t> values2;
	};

	struct GfxWorldModelGroup
	{
		char __pad0[0x8];
		GfxList<std::uint32_t> values1;
		GfxList<std::uint32_t> values2;
		GfxList<ClipMapModel*> models;
		GfxList<XModel*> xmodels;
		char __pad1[0x8];
	};

	struct GfxWorldLodGroup
	{
		GroupLodModel* model;
		char __pad0[0x20];
		GfxList<std::uint8_t> data;
		char __pad1[0x10];
	};

	struct GfxWorldNamedValues
	{
		scr_string_t name;
		char __pad0[0x4];
		GfxList<std::uint32_t> values;
	};

	struct GfxWorldMaterialLists
	{
		GfxList<Material*> materials1;
		GfxList<Material*> materials2;
		char __pad0[0x8];
	};

	struct GfxWorldModelInstance
	{
		char __pad0[0x20];
		XModel* model1;
		XModel* model2;
		char __pad1[0x8];
		GfxList<GfxWorldMaterialLists> materialLists;
		GfxList<std::uint8_t[48]> values;
		GfxList<std::uint8_t> runtimeData1;
		GfxList<std::uint8_t> runtimeData2;
		char __pad2[0x10];
	};

	struct GfxWorldImageEntry
	{
		GfxImage* image;
		char __pad0[0x88];
		GfxList<std::uint32_t> values;
	};

	struct GfxWorldEffectEntry
	{
		char __pad0[0x8];
		GfxList<std::uint8_t[24]> values1;
		FxEffectDef* effect;
		char __pad1[0x8];
		GfxList<std::uint8_t[12]> values2;
		char __pad2[0x68];
	};

	struct GfxWorldModelData
	{
		char __pad0[0x18];
		GfxList<GfxWorldModelInstance> instances;
		GfxList<GfxWorldImageEntry> images;
		GfxList<GfxWorldEffectEntry> effects;
		GfxBuffer* buffers[6];
		GfxList<std::uint16_t> indices;
	};

	struct GfxWorldDrawData
	{
		char __pad0[0xC];
		std::uint32_t cellCount;
		char __pad1[0x30];
		GfxArray<std::uint8_t> runtimeBuffers[3][4];
		GfxWorldValueTable valueTables[3];
		GfxWorldDataTable dataTables[3];
		std::uint16_t* cellValues;
		std::uint32_t values1Count;
		std::uint32_t values2Count;
		std::uint32_t* values1;
		std::uint32_t* values2;
		std::uint32_t values3Count;
		char __pad2[0x4];
		GfxWorldSurface* surfaces;
		GfxList<std::uint8_t[12]> values4;
		GfxWorldModelGroup modelGroup;
		std::uint8_t (*values3)[0x1C];
		std::uint64_t* runtimeCells;
		GfxArray<std::uint8_t> data1;
		GfxArray<std::uint8_t> data2;
		GfxBuffer* buffers[3];
		GfxWorldModelData* modelData;
		GfxList<GfxWorldLodGroup> lodGroups;
		GfxList<GroupLodModel*> lodModels;
		char __pad3[0x18];
		GfxList<std::uint8_t> data3;
		GfxList<GfxWorldNamedValues> namedValues;
	};

	struct GfxWorldRuntimeBuffers
	{
		std::uint32_t count;
		char __pad0[0x4];
		GfxArray<std::uint8_t> buffers[4];
		GfxArray<std::uint8_t> buffer;
	};

	struct GfxWorldAnimation
	{
		char __pad0[0x28];
		SAnim* anim;
		char __pad1[0x8];
		scr_string_t name1;
		scr_string_t name2;
		char __pad2[0x8];
	};

	struct GfxWorldLight
	{
		char __pad0[0xB8];
		Material* material1;
		Material* material2;
		char __pad1[0x4];
		scr_string_t name;
		char __pad2[0x8];
	};

	struct GfxWorldDecal
	{
		char __pad0[0x48];
		Material* material;
		char __pad1[0x8];
	};

	struct GfxWorldMaterialRef
	{
		char __pad0[0x20];
		Material* material;
		char __pad1[0x8];
	};

	struct GfxWorldProbeList
	{
		std::uint64_t* values;
		std::uint32_t count;
		char __pad0[0x14];
	};

	struct GfxWorldProbeIndexList
	{
		char __pad0[0x8];
		std::uint64_t* values;
		std::uint32_t count;
		char __pad1[0x14];
	};

	struct GfxWorldVolume
	{
		char __pad0[0x1C];
		std::uint32_t values1Count;
		std::uint8_t (*values1)[40];
		std::uint32_t probeListCount;
		char __pad1[0x4];
		GfxWorldProbeList* probeLists;
		std::uint32_t probeIndexListCount;
		char __pad2[0x4];
		GfxWorldProbeIndexList* probeIndexLists;
		Material* material;
		char __pad3[0x50];
	};

	struct GfxWorldVolumeEntry
	{
		GfxWorldVolume volume;
		std::uint32_t valueCount;
		char __pad0[0x4];
		std::uint8_t (*values)[0x48];
		char __pad1[0x28];
	};

	struct GfxWorldVolumeList
	{
		char __pad0[0x4];
		std::uint32_t entryCount;
		GfxWorldVolumeEntry* entries;
		GfxArray<std::uint8_t> runtimeData;
	};

	struct GfxWorldRuntimeGrid
	{
		std::uint32_t valueCount;
		std::uint32_t cellCount;
		std::uint16_t* values;
		std::uint32_t* cells;
	};

	struct GfxWorldCellData
	{
		char __pad0[0x528];
		std::uint32_t values1Count;
		std::uint32_t values2Count;
		std::uint32_t values3Count;
		std::uint32_t values4Count;
		std::uint32_t values5Count;
		char __pad1[0x4];
		std::uint16_t* values1;
		std::uint8_t (*values2)[0x8C];
		std::uint8_t (*values3)[0x78];
		std::uint8_t (*values4)[0xD8];
		std::uint8_t (*values5)[0xD8];
		std::uint32_t blockCounts[4];
		std::uint8_t (*blocks[4])[128];
	};

	struct GfxWorldModelRef
	{
		char __pad0[0x8];
		std::uint8_t* data1;
		XModel* model1;
		char __pad1[0x18];
		std::uint8_t* data2;
		char __pad2[0x8];
		XModel* model2;
		char __pad3[0x8];
	};

	struct GfxWorld
	{
		XHash name;
		const char* baseName;
		std::int32_t surfaceCount;
		char __pad0[0x9C];
		GfxList<std::uint8_t[12]> values1;
		GfxList<std::uint8_t[16]> values2;
		GfxWorldVertexData vertexData;
		GfxArray<std::uint8_t[0x50]> values3;
		char __pad1[0x20];
		std::uint64_t* runtimeValues;
		GfxWorldDrawData draw;
		GfxWorldRuntimeBuffers runtimeBuffers;
		Material* materials[2];
		GfxImage* images1[6];
		GfxList<GfxWorldAnimation*> animations;
		GfxArray<std::uint8_t[0x54]> values4;
		GfxArray<GfxWorldLight> lights;
		std::uint64_t* runtimeLights;
		GfxImage* image2;
		char __pad2[0x18];
		GfxArray<GfxWorldDecal> decals;
		GfxArray<GfxWorldMaterialRef> materialRefs;
		std::uint8_t* data1;
		std::uint32_t data1Size;
		char __pad3[0x4];
		std::uint8_t* data2;
		std::uint32_t data2Size;
		char __pad4[0x54];
		GfxWorldVolumeList volumes;
		char __pad5[0xC8];
		GfxWorldRuntimeGrid runtimeGrid;
		GfxWorldCellData cellData;
		char __pad6[0x1010];
		GfxArray<std::uint8_t[64]> values5;
		GfxArray<std::uint8_t[0xB0]> values6;
		GfxArray<std::uint8_t[0xB0]> values7;
		Lighting* lighting;
		StreamerWorld* streamerWorld;
		GfxList<GfxWorldModelRef> modelRefs;
		GfxArray<std::uint8_t[0x1BA]> values8;
		GfxArray<std::uint8_t[0xFF4]> values9;
		char __pad7[0x8];
		GfxImage* image3;
		GfxArray<std::uint8_t[0x344]> values10;
		GfxArray<std::uint8_t[0xFD0]> values11;
		GfxArray<std::uint8_t[0x2B0]> values12;
		GfxArray<std::uint8_t[512]> values13;
		GfxArray<std::uint8_t[0x43C]> values14;
		GfxArray<std::uint8_t[0x94]> values15;
		GfxArray<std::uint8_t[0x60]> values16;
		GfxArray<std::uint8_t[0x14]> values17;
		GfxArray<std::uint8_t[64]> blocks[15];
		GfxImage* images2[10];
	};

	struct StaticLevelFxTarget
	{
		const char* name;
		char __pad0[0x18];
	};

	struct StaticLevelFx
	{
		FxEffectDef* effect;
		char __pad0[0x30];
		std::uint32_t targetCount;
		char __pad1[0x4];
		const char* name;
		StaticLevelFxTarget* targets;
	};

	struct StaticLevelFxList
	{
		XHash name;
		std::uint32_t effectCount;
		char __pad0[0x4];
		StaticLevelFx* effects;
		std::uint32_t stringCount;
		char __pad1[0x4];
		const char** strings;
	};

	struct DistrictData
	{
		std::uint64_t* values1;
		std::uint32_t* values2;
		std::uint32_t* values3;
		std::uint32_t* values4;
		std::uint32_t count1;
		std::uint32_t count2;
	};

	enum DistrictStreamKey
	{
		DISTRICT_STREAM_KEY_STATIC_MODELS = 6,
	};

	struct District
	{
		char __pad0[0x38];
		StreamKey* streamKeys[10];
	};

	struct StaticModelStreamHeader
	{
		std::uint32_t dataSize;
		char __pad0[0x104];
		std::uint32_t firstInstance;
		std::uint32_t instanceCount;
		char __pad1[0x20];
	};

	struct StaticModelInstance
	{
		char __pad0[0x10];
		XHash model;
		std::uint32_t id;
		char __pad1[0xC];
		std::uint32_t deformationIndex;
		char __pad2[0x14];
	};

	struct StaticModelPlacement
	{
		float quat[4];
		float origin[3];
		float scale;
		float mins[3];
		float maxs[3];
		std::uint32_t index;
		float radius;
	};

	struct Districts
	{
		XHash name;
		GfxWorld* world;
		DistrictData data;
		char __pad0[0x8];
		std::uint64_t districtCount;
		District* districts;
		std::uint64_t refCount;
		void** refs;
	};

	static_assert(sizeof(GfxBuffer) == 0x38);
	static_assert(sizeof(KlfEntry) == 0x2C8);
	static_assert(offsetof(KlfEntry, buffer) == 0x288);
	static_assert(sizeof(Klf) == 0x58);
	static_assert(offsetof(Klf, entryCount) == 0x40);
	static_assert(offsetof(Klf, image) == 0x48);
	static_assert(sizeof(VectorField) == 0x58);
	static_assert(offsetof(VectorField, dataSize) == 0x4C);
	static_assert(sizeof(FxLightDef) == 0x2B8);
	static_assert(sizeof(FxElemVisualEntry) == 0x40);
	static_assert(sizeof(FxTrailDef) == 0x30);
	static_assert(sizeof(FxElemDef) == 0x340);
	static_assert(offsetof(FxElemDef, secondaryVisuals) == 0x40);
	static_assert(offsetof(FxElemDef, extended) == 0x110);
	static_assert(offsetof(FxElemDef, effects) == 0x160);
	static_assert(offsetof(FxElemDef, surfaceFxTable) == 0x180);
	static_assert(offsetof(FxElemDef, valueCount) == 0x2EC);
	static_assert(offsetof(FxElemDef, elemType) == 0x328);
	static_assert(offsetof(FxElemDef, visualCount) == 0x332);
	static_assert(offsetof(FxElemDef, recordCount) == 0x337);
	static_assert(sizeof(FxEffectDef) == 0x90);
	static_assert(offsetof(FxEffectDef, elemDefs) == 0x28);
	static_assert(sizeof(SurfaceFxTable) == 0x148);
	static_assert(sizeof(EntityFxImpacts) == 0x248);
	static_assert(sizeof(FxImpactTable) == 0x18);
	static_assert(sizeof(FxLibraryVolume) == 0xE20);
	static_assert(offsetof(FxLibraryVolume, materials) == 0x90);
	static_assert(offsetof(FxLibraryVolume, bufferSizes) == 0x1F0);
	static_assert(offsetof(FxLibraryVolume, buffers) == 0x200);
	static_assert(sizeof(GlassDef) == 0x68);
	static_assert(sizeof(GlassEntry) == 0x90);
	static_assert(offsetof(GlassEntry, valueCount) == 0x45);
	static_assert(sizeof(Glasses) == 0x50);
	static_assert(offsetof(Glasses, dataSize) == 0x20);
	static_assert(sizeof(TriggerEntry) == 0x20);
	static_assert(sizeof(TriggerGroup) == 0x30);
	static_assert(sizeof(TriggerModel) == 0x8);
	static_assert(sizeof(TriggerHull) == 0x20);
	static_assert(sizeof(TriggerSlab) == 0x14);
	static_assert(sizeof(TriggerList) == 0x48);
	static_assert(offsetof(TriggerList, groups) == 0x40);
	static_assert(sizeof(XAnimPartTransFrames) == 0x20);
	static_assert(offsetof(XAnimPartTrans, u) == 0x8);
	static_assert(offsetof(XAnimDeltaPartQuat2, u) == 0x8);
	static_assert(offsetof(XAnimDeltaPartQuat, u) == 0x8);
	static_assert(sizeof(XAnimExtraRecord) == 0x16);
	static_assert(sizeof(XAnimBoneTrack) == 0x18);
	static_assert(sizeof(XAnimExtra) == 0x68);
	static_assert(sizeof(XAnim) == 0x120);
	static_assert(offsetof(XAnim, name) == 0x70);
	static_assert(offsetof(XAnim, streamKey) == 0xA8);
	static_assert(offsetof(XAnim, indexCount) == 0xC0);
	static_assert(offsetof(XAnim, dataShort2Count) == 0xCC);
	static_assert(offsetof(XAnim, dataInt2Count) == 0xE8);
	static_assert(offsetof(XAnim, boneNameCount) == 0xFE);
	static_assert(offsetof(XAnim, numframes) == 0x104);
	static_assert(offsetof(XAnim, nameCount) == 0x106);
	static_assert(offsetof(XAnim, records3Count) == 0x115);
	static_assert(offsetof(XAnim, records6Count) == 0x117);
	static_assert(sizeof(DestructibleStageRecord) == 0x18);
	static_assert(sizeof(DestructibleStage) == 0x48);
	static_assert(offsetof(DestructibleStage, recordCount) == 0x44);
	static_assert(sizeof(DestructiblePiece) == 0x258);
	static_assert(offsetof(DestructiblePiece, physConstraints) == 0x228);
	static_assert(offsetof(DestructiblePiece, effect) == 0x240);
	static_assert(offsetof(DestructiblePiece, name2) == 0x250);
	static_assert(sizeof(DestructibleDef) == 0x38);
	static_assert(offsetof(DestructibleDef, pieces) == 0x28);
	static_assert(sizeof(ScriptBundleKeyValue) == 0x20);
	static_assert(sizeof(ScriptBundleObjectList) == 0x20);
	static_assert(sizeof(ScriptBundleObject) == 0x20);
	static_assert(sizeof(ScriptBundle) == 0x30);
	static_assert(sizeof(EntityList) == 0x18);
	static_assert(sizeof(CpuOcclusionEntry) == 0x50);
	static_assert(sizeof(CpuOcclusionBuffer) == 0x30);
	static_assert(sizeof(CpuOcclusionData) == 0xB8);
	static_assert(offsetof(CpuOcclusionData, buffer1) == 0x48);
	static_assert(offsetof(CpuOcclusionData, runtimeData1) == 0xA8);
	static_assert(sizeof(ComPrimaryLight) == 0x2B0);
	static_assert(offsetof(ComPrimaryLight, origin) == 0x68);
	static_assert(offsetof(ComPrimaryLight, color) == 0xC8);
	static_assert(offsetof(ComPrimaryLight, radius) == 0xE8);
	static_assert(offsetof(ComPrimaryLight, emitterSize) == 0xF8);
	static_assert(offsetof(ComPrimaryLight, lodFadeRange) == 0x200);
	static_assert(offsetof(ComPrimaryLight, image) == 0x2A0);
	static_assert(sizeof(ComRecordGroup<ComRecord20>) == 0x18);
	static_assert(sizeof(ComWorld) == 0x58);
	static_assert(offsetof(ComWorld, groups28) == 0x50);
	static_assert(sizeof(WindDef) == 0x80);
	static_assert(sizeof(LightingImageRef) == 0x28);
	static_assert(sizeof(LightingProbe) == 0x68);
	static_assert(offsetof(LightingProbe, valueCount) == 0x1C);
	static_assert(offsetof(LightingProbe, imageRef1) == 0x50);
	static_assert(sizeof(LightingProbeSlot) == 0x70);
	static_assert(sizeof(LightingProbeSet) == 0x1D0);
	static_assert(sizeof(LightingVolumeProbeSlot) == 0x80);
	static_assert(sizeof(LightingVolumeGroup) == 0x58);
	static_assert(offsetof(LightingVolumeGroup, records1Count) == 0x50);
	static_assert(sizeof(LightingImageTriple) == 0x20);
	static_assert(sizeof(LightingModelRef) == 0x28);
	static_assert(sizeof(LightingGridCell) == 0x50);
	static_assert(offsetof(LightingGridCell, values) == 0x40);
	static_assert(sizeof(LightingGridPoint) == 0xC);
	static_assert(sizeof(LightingGridTableEntry) == 0x8);
	static_assert(sizeof(LightingGrid) == 0x50);
	static_assert(offsetof(LightingGrid, sizeX) == 0x2C);
	static_assert(offsetof(LightingGrid, points) == 0x38);
	static_assert(sizeof(LightingGridSet) == 0x40);
	static_assert(offsetof(LightingGridSet, tableSize) == 0x28);
	static_assert(sizeof(LightingVolumeState) == 0x824);
	static_assert(offsetof(LightingVolumeState, sunColor) == 0x4C);
	static_assert(sizeof(LightingVolume) == 0x26A0);
	static_assert(offsetof(LightingVolume, states) == 0x10);
	static_assert(offsetof(LightingVolume, probeSlots) == 0x20E0);
	static_assert(offsetof(LightingVolume, images) == 0x22E0);
	static_assert(offsetof(LightingVolume, values) == 0x2308);
	static_assert(offsetof(LightingVolume, occlusionBuffer) == 0x2310);
	static_assert(offsetof(LightingVolume, groups) == 0x2340);
	static_assert(offsetof(LightingVolume, imageTripleLists) == 0x24A0);
	static_assert(offsetof(LightingVolume, modelRefs) == 0x24E0);
	static_assert(offsetof(LightingVolume, gridSets) == 0x2580);
	static_assert(sizeof(LightingImageSet) == 0x80);
	static_assert(sizeof(LightingWindRef) == 0x28);
	static_assert(sizeof(LightingImageRecord) == 0x38);
	static_assert(sizeof(LightingImagePairRecord) == 0x38);
	static_assert(sizeof(LightingImageValuesRecord) == 0x30);
	static_assert(sizeof(LightingImageRecordLarge) == 0x80);
	static_assert(sizeof(Lighting) == 0x1D8);
	static_assert(offsetof(Lighting, probeSets) == 0x98);
	static_assert(offsetof(Lighting, imageSets) == 0xD8);
	static_assert(offsetof(Lighting, records4) == 0x128);
	static_assert(offsetof(Lighting, records5) == 0x150);
	static_assert(offsetof(Lighting, imageRecords) == 0x180);
	static_assert(offsetof(Lighting, windRefs) == 0x1C0);
	static_assert(offsetof(Lighting, values9) == 0x1D0);
	static_assert(sizeof(TerrainMesh) == 0xD8);
	static_assert(offsetof(TerrainMesh, data2) == 0x48);
	static_assert(offsetof(TerrainMesh, data1Size) == 0x6C);
	static_assert(offsetof(TerrainMesh, values) == 0x78);
	static_assert(offsetof(TerrainMesh, streamKey) == 0x98);
	static_assert(sizeof(TerrainMeshMaterial) == 0x60);
	static_assert(sizeof(TerrainCellMaterial) == 0x90);
	static_assert(sizeof(TerrainCell) == 0x160);
	static_assert(sizeof(TerrainCellGrid) == 0x10);
	static_assert(sizeof(TerrainCellGridList) == 0x10);
	static_assert(sizeof(TerrainLayer) == 0x130);
	static_assert(sizeof(TerrainTile) == 0x1D8);
	static_assert(offsetof(TerrainTile, cellGrids) == 0x40);
	static_assert(offsetof(TerrainTile, values1) == 0xD0);
	static_assert(offsetof(TerrainTile, layers) == 0x110);
	static_assert(offsetof(TerrainTile, values2) == 0x160);
	static_assert(offsetof(TerrainTile, images) == 0x188);
	static_assert(offsetof(TerrainTile, values3) == 0x1B8);
	static_assert(offsetof(TerrainTile, image7) == 0x1C8);
	static_assert(sizeof(TerrainRegionPart) == 0x50);
	static_assert(offsetof(TerrainRegionPart, values) == 0x28);
	static_assert(offsetof(TerrainRegionPart, dataSize) == 0x48);
	static_assert(sizeof(TerrainRegion) == 0xB0);
	static_assert(offsetof(TerrainRegion, parts) == 0x18);
	static_assert(offsetof(TerrainRegion, entries) == 0x98);
	static_assert(sizeof(TerrainRegionList) == 0x48);
	static_assert(sizeof(TerrainBlend) == 0x100);
	static_assert(offsetof(TerrainBlend, image) == 0xD0);
	static_assert(sizeof(TerrainPatch) == 0x78);
	static_assert(sizeof(TerrainImageBlock) == 0x40);
	static_assert(sizeof(TerrainGfx) == 0x128);
	static_assert(offsetof(TerrainGfx, images) == 0x38);
	static_assert(offsetof(TerrainGfx, regionList) == 0x98);
	static_assert(offsetof(TerrainGfx, values3) == 0xD8);
	static_assert(offsetof(TerrainGfx, imageBlock) == 0xE0);
	static_assert(sizeof(StreamerKeyRef) == 0x30);
	static_assert(sizeof(StreamerBitGrid) == 0x48);
	static_assert(offsetof(StreamerBitGrid, bitRows) == 0x1C);
	static_assert(offsetof(StreamerBitGrid, pairCount) == 0x28);
	static_assert(sizeof(StreamerValueList) == 0x18);
	static_assert(sizeof(StreamerSection) == 0xA8);
	static_assert(offsetof(StreamerSection, valueLists) == 0x58);
	static_assert(offsetof(StreamerSection, images) == 0x78);
	static_assert(sizeof(StreamerModelLodGroup) == 0x48);
	static_assert(sizeof(StreamerModelLod) == 0x10);
	static_assert(sizeof(StreamerModelSet) == 0x30);
	static_assert(sizeof(StreamerCell) == 0x100);
	static_assert(offsetof(StreamerCell, modelSet) == 0xB0);
	static_assert(sizeof(StreamerRangeList) == 0x28);
	static_assert(sizeof(StreamerRangeGroup) == 0x18);
	static_assert(sizeof(StreamerPairList) == 0x18);
	static_assert(sizeof(StreamerWorld) == 0xE0);
	static_assert(offsetof(StreamerWorld, cells) == 0x58);
	static_assert(offsetof(StreamerWorld, models) == 0xB8);
	static_assert(offsetof(StreamerWorld, lighting) == 0xD8);
	static_assert(sizeof(DynModel) == 0x120);
	static_assert(offsetof(DynModel, effects1) == 0x78);
	static_assert(offsetof(DynModel, bundle) == 0xA8);
	static_assert(offsetof(DynModel, physPreset) == 0xD8);
	static_assert(offsetof(DynModel, name) == 0xE0);
	static_assert(sizeof(ClipMapPartitionEntry) == 0x50);
	static_assert(sizeof(ClipMapPartition) == 0x40);
	static_assert(sizeof(ClipMapModelEntryList) == 0x48);
	static_assert(sizeof(ClipMapModelData) == 0x218);
	static_assert(sizeof(ClipMapModel) == 0x3E0);
	static_assert(offsetof(ClipMapModel, data) == 0x60);
	static_assert(offsetof(ClipMapModel, indices) == 0x3A8);
	static_assert(offsetof(ClipMapModel, dataCount2) == 0x3D0);
	static_assert(offsetof(ClipMapModel, dataCount1) == 0x3D8);
	static_assert(offsetof(ClipMapModel, valueCount) == 0x3DD);
	static_assert(sizeof(ClipMapModelList) == 0x18);
	static_assert(sizeof(ClipMapMesh) == 0xB8);
	static_assert(offsetof(ClipMapMesh, planes) == 0x40);
	static_assert(offsetof(ClipMapMesh, data3Size) == 0xA0);
	static_assert(sizeof(ClipMapBrush) == 0x1C);
	static_assert(sizeof(ClipMapBrushVertices) == 0x4);
	static_assert(sizeof(ClipMapMeshLeaf) == 0x20);
	static_assert(sizeof(ClipMapTable) == 0x20);
	static_assert(sizeof(ClipMapDynModel) == 0x60);
	static_assert(offsetof(ClipMapDynModel, scale) == 0x2C);
	static_assert(sizeof(ClipMap) == 0x1D8);
	static_assert(offsetof(ClipMap, dynModels) == 0x60);
	static_assert(offsetof(ClipMap, regionList) == 0xA0);
	static_assert(offsetof(ClipMap, collisionCount) == 0x1A8);
	static_assert(offsetof(ClipMap, partitionCount) == 0x1CE);
	static_assert(sizeof(NavMeshEntry) == 0x40);
	static_assert(sizeof(NavMesh) == 0x68);
	static_assert(offsetof(NavMesh, entries) == 0x28);
	static_assert(offsetof(NavMesh, streamKey2) == 0x48);
	static_assert(sizeof(NavVolumeEntry) == 0x38);
	static_assert(sizeof(NavVolume) == 0x18);
	static_assert(sizeof(GameMapEntity) == 0xB0);
	static_assert(offsetof(GameMapEntity, valueCount) == 0x74);
	static_assert(sizeof(GameMapLink) == 0x18);
	static_assert(sizeof(GameMapData) == 0x38);
	static_assert(sizeof(GameMap) == 0x50);
	static_assert(offsetof(GameMap, grid1) == 0x40);
	static_assert(sizeof(SAnimData) == 0x80);
	static_assert(offsetof(SAnimData, dataSize) == 0x70);
	static_assert(sizeof(SAnim) == 0x70);
	static_assert(offsetof(SAnim, values1Count) == 0x62);
	static_assert(offsetof(SAnim, values2Count) == 0x68);
	static_assert(sizeof(XCamNotes) == 0x10);
	static_assert(sizeof(XCamTrack) == 0x28);
	static_assert(offsetof(XCamTrack, keys) == 0x18);
	static_assert(sizeof(XCamTarget) == 0x20);
	static_assert(sizeof(XCam) == 0xB0);
	static_assert(offsetof(XCam, trackCount) == 0x1C);
	static_assert(offsetof(XCam, nameCount) == 0x28);
	static_assert(offsetof(XCam, notes) == 0x48);
	static_assert(offsetof(XCam, target1Count) == 0x70);
	static_assert(offsetof(XCam, target3Count) == 0x90);
	static_assert(sizeof(BGCacheInfo) == 0x18);
	static_assert(sizeof(BGCache) == 0x18);
	static_assert(sizeof(GroupLodModel) == 0x28);
	static_assert(sizeof(GfxWorldGpuBuffer) == 0x18);
	static_assert(sizeof(GfxWorldVertex) == 0x14);
	static_assert(sizeof(GfxWorldVertexData) == 0xA8);
	static_assert(offsetof(GfxWorldVertexData, vertices) == 0x38);
	static_assert(offsetof(GfxWorldVertexData, indexCount) == 0x58);
	static_assert(offsetof(GfxWorldVertexData, indexBuffer) == 0x68);
	static_assert(sizeof(GfxWorldSurface) == 0x60);
	static_assert(offsetof(GfxWorldSurface, triCount) == 0x26);
	static_assert(offsetof(GfxWorldSurface, baseIndex) == 0x28);
	static_assert(sizeof(GfxWorldValueTable) == 0x18);
	static_assert(sizeof(GfxWorldDataTable) == 0x30);
	static_assert(sizeof(GfxWorldModelGroup) == 0x50);
	static_assert(sizeof(GfxWorldLodGroup) == 0x48);
	static_assert(sizeof(GfxWorldNamedValues) == 0x18);
	static_assert(sizeof(GfxWorldMaterialLists) == 0x28);
	static_assert(sizeof(GfxWorldModelInstance) == 0x88);
	static_assert(offsetof(GfxWorldModelInstance, materialLists) == 0x38);
	static_assert(sizeof(GfxWorldImageEntry) == 0xA0);
	static_assert(sizeof(GfxWorldEffectEntry) == 0xA0);
	static_assert(offsetof(GfxWorldEffectEntry, values2) == 0x28);
	static_assert(sizeof(GfxWorldModelData) == 0x88);
	static_assert(offsetof(GfxWorldModelData, buffers) == 0x48);
	static_assert(sizeof(GfxWorldDrawData) == 0x310);
	static_assert(offsetof(GfxWorldDrawData, valueTables) == 0x100);
	static_assert(offsetof(GfxWorldDrawData, cellValues) == 0x1D8);
	static_assert(offsetof(GfxWorldDrawData, surfaces) == 0x200);
	static_assert(offsetof(GfxWorldDrawData, modelGroup) == 0x218);
	static_assert(offsetof(GfxWorldDrawData, values3) == 0x268);
	static_assert(offsetof(GfxWorldDrawData, buffers) == 0x298);
	static_assert(offsetof(GfxWorldDrawData, lodGroups) == 0x2B8);
	static_assert(offsetof(GfxWorldDrawData, data3) == 0x2F0);
	static_assert(sizeof(GfxWorldRuntimeBuffers) == 0x58);
	static_assert(sizeof(GfxWorldAnimation) == 0x48);
	static_assert(offsetof(GfxWorldAnimation, name1) == 0x38);
	static_assert(sizeof(GfxWorldLight) == 0xD8);
	static_assert(offsetof(GfxWorldLight, name) == 0xCC);
	static_assert(sizeof(GfxWorldDecal) == 0x58);
	static_assert(sizeof(GfxWorldMaterialRef) == 0x30);
	static_assert(sizeof(GfxWorldProbeList) == 0x20);
	static_assert(sizeof(GfxWorldProbeIndexList) == 0x28);
	static_assert(sizeof(GfxWorldVolume) == 0xA0);
	static_assert(offsetof(GfxWorldVolume, material) == 0x48);
	static_assert(sizeof(GfxWorldVolumeEntry) == 0xD8);
	static_assert(sizeof(GfxWorldVolumeList) == 0x20);
	static_assert(sizeof(GfxWorldRuntimeGrid) == 0x18);
	static_assert(sizeof(GfxWorldCellData) == 0x598);
	static_assert(offsetof(GfxWorldCellData, values1) == 0x540);
	static_assert(offsetof(GfxWorldCellData, blocks) == 0x578);
	static_assert(sizeof(GfxWorldModelRef) == 0x50);
	static_assert(offsetof(GfxWorldModelRef, data2) == 0x30);
	static_assert(sizeof(GfxWorld) == 0x1F28);
	static_assert(offsetof(GfxWorld, values1) == 0xB0);
	static_assert(offsetof(GfxWorld, vertexData) == 0xD0);
	static_assert(offsetof(GfxWorld, runtimeValues) == 0x1A8);
	static_assert(offsetof(GfxWorld, draw) == 0x1B0);
	static_assert(offsetof(GfxWorld, runtimeBuffers) == 0x4C0);
	static_assert(offsetof(GfxWorld, animations) == 0x558);
	static_assert(offsetof(GfxWorld, image2) == 0x590);
	static_assert(offsetof(GfxWorld, decals) == 0x5B0);
	static_assert(offsetof(GfxWorld, data2Size) == 0x5E8);
	static_assert(offsetof(GfxWorld, volumes) == 0x640);
	static_assert(offsetof(GfxWorld, runtimeGrid) == 0x728);
	static_assert(offsetof(GfxWorld, cellData) == 0x740);
	static_assert(offsetof(GfxWorld, values5) == 0x1CE8);
	static_assert(offsetof(GfxWorld, lighting) == 0x1D18);
	static_assert(offsetof(GfxWorld, modelRefs) == 0x1D28);
	static_assert(offsetof(GfxWorld, image3) == 0x1D60);
	static_assert(offsetof(GfxWorld, blocks) == 0x1DE8);
	static_assert(offsetof(GfxWorld, images2) == 0x1ED8);
	static_assert(sizeof(StaticLevelFxTarget) == 0x20);
	static_assert(sizeof(StaticLevelFx) == 0x50);
	static_assert(offsetof(StaticLevelFx, targetCount) == 0x38);
	static_assert(offsetof(StaticLevelFx, targets) == 0x48);
	static_assert(sizeof(StaticLevelFxList) == 0x28);
	static_assert(offsetof(StaticLevelFxList, strings) == 0x20);
	static_assert(sizeof(DistrictData) == 0x28);
	static_assert(sizeof(District) == 0x88);
	static_assert(sizeof(Districts) == 0x60);
	static_assert(offsetof(Districts, data) == 0x10);
	static_assert(offsetof(Districts, districtCount) == 0x40);
	static_assert(offsetof(Districts, refs) == 0x58);
	static_assert(sizeof(StaticModelStreamHeader) == 0x130);
	static_assert(offsetof(StaticModelStreamHeader, instanceCount) == 0x10C);
	static_assert(sizeof(StaticModelInstance) == 0x40);
	static_assert(offsetof(StaticModelInstance, model) == 0x10);
	static_assert(sizeof(StaticModelPlacement) == 0x40);
	static_assert(offsetof(StaticModelPlacement, index) == 0x38);

	static_assert(sizeof(StreamKey) == 0x38);
	static_assert(sizeof(SndValueArray) == 0x10);
	static_assert(sizeof(SndAsset) == 0x58);
	static_assert(sizeof(SndDuck) == 0x660);
	static_assert(sizeof(SndAliasModifier) == 0x98);
	static_assert(sizeof(SndAliasInfo) == 0xD0);
	static_assert(sizeof(SndAlias) == 0x60);
	static_assert(sizeof(SndAliasList) == 0x20);
	static_assert(sizeof(SndBankGroupItem) == 0x120);
	static_assert(sizeof(SndBankGroup) == 0x58);
	static_assert(sizeof(SndAcousticsProbe) == 0x40);
	static_assert(sizeof(SndAcousticsData) == 0x38);
	static_assert(sizeof(SndAcoustics) == 0x160);
	static_assert(sizeof(SndBank) == 0x70);
	static_assert(offsetof(StreamKey, data) == 0x20);
	static_assert(offsetof(StreamKey, flags) == 0x37);
	static_assert(offsetof(SndAsset, streamKey) == 0x40);
	static_assert(offsetof(SndAlias, info) == 0x50);
	static_assert(offsetof(SndAcoustics, acousticsData) == 0x28);
	static_assert(offsetof(SndAcoustics, values1) == 0xB8);
	static_assert(offsetof(SndAcoustics, values2) == 0xE0);
	static_assert(offsetof(SndAcoustics, values3) == 0x118);
	static_assert(offsetof(SndAcoustics, values4) == 0x130);
	static_assert(offsetof(SndAcoustics, records) == 0x150);
	static_assert(offsetof(SndBank, acoustics) == 0x68);

	struct ValueArrayEntry
	{
		char __pad0[0x8];
	};

	static_assert(sizeof(ValueArrayEntry) == 0x8);

	struct ValueArray
	{
		ValueArrayEntry* values;
		std::int32_t count;
		char __pad0[0x4];
	};

	static_assert(sizeof(ValueArray) == 0x10);
	static_assert(offsetof(ValueArray, count) == 0x8);

	struct PostFxPassParam
	{
		char __pad0[0x18];
	};

	static_assert(sizeof(PostFxPassParam) == 0x18);

	struct PostFxPass
	{
		char __pad0[0x8];
		Material* materials[3];
		char __pad1[0x6];
		std::uint8_t paramCount;
		char __pad2[0x9];
		PostFxPassParam* params;
	};

	static_assert(sizeof(PostFxPass) == 0x38);
	static_assert(offsetof(PostFxPass, params) == 0x30);

	struct PostFxGroupValue
	{
		char __pad0[0x18];
	};

	static_assert(sizeof(PostFxGroupValue) == 0x18);

	struct PostFxGroup
	{
		char __pad0[0x1];
		std::uint8_t valueCount;
		char __pad1[0x6];
		PostFxGroupValue* values;
	};

	static_assert(sizeof(PostFxGroup) == 0x10);
	static_assert(offsetof(PostFxGroup, values) == 0x8);

	struct PostFxData
	{
		char __pad0[0xE];
		std::uint8_t passCount;
		std::uint8_t groupCount;
		PostFxPass* passes;
		PostFxGroup* groups;
		GfxImage* image;
	};

	static_assert(sizeof(PostFxData) == 0x28);
	static_assert(offsetof(PostFxData, image) == 0x20);

	struct PostFxBundle
	{
		XHash name;
		PostFxData data;
	};

	static_assert(sizeof(PostFxBundle) == 0x30);
	static_assert(offsetof(PostFxBundle, data) == 0x8);

	struct RenderOverrideBundle
	{
		XHash name;
		char __pad0[0x8];
		PostFxData entries[3];
	};

	static_assert(sizeof(RenderOverrideBundle) == 0x88);
	static_assert(offsetof(RenderOverrideBundle, entries) == 0x10);

	struct Gesture
	{
		XHash name;
		char __pad0[0x10];
		XAnim* anims[38];
		char __pad1[0xB8];
	};

	static_assert(sizeof(Gesture) == 0x200);
	static_assert(offsetof(Gesture, anims) == 0x18);

	struct GestureTable
	{
		XHash name;
		Gesture* gestures[12];
		char __pad0[0x8];
	};

	static_assert(sizeof(GestureTable) == 0x70);
	static_assert(offsetof(GestureTable, gestures) == 0x8);

	struct WeaponStyleData
	{
		char __pad0[0x54];
	};

	static_assert(sizeof(WeaponStyleData) == 0x54);

	struct WeaponStyleSettings
	{
		XHash name;
		char __pad0[0xA8];
		WeaponStyleData* data;
		char __pad1[0x118];
	};

	static_assert(sizeof(WeaponStyleSettings) == 0x1D0);
	static_assert(offsetof(WeaponStyleSettings, data) == 0xB0);

	struct WeaponTunablesData
	{
		char __pad0[0xA0];
	};

	static_assert(sizeof(WeaponTunablesData) == 0xA0);

	struct WeaponTunablesEntry
	{
		char __pad0[0x10];
	};

	static_assert(sizeof(WeaponTunablesEntry) == 0x10);

	struct WeaponValueTables
	{
		char __pad0[0x58];
		ValueArray values0;
		char __pad1[0x30];
		ValueArray values1;
		char __pad2[0x58];
	};

	static_assert(sizeof(WeaponValueTables) == 0x100);
	static_assert(offsetof(WeaponValueTables, values1) == 0x98);

	struct WeaponTunables
	{
		WeaponTunablesData* data0;
		WeaponTunablesData* data1;
		char __pad0[0x8];
		char* string0;
		char* string1;
		char __pad1[0x470];
		std::uint64_t entryCount;
		WeaponTunablesEntry* entries;
		char __pad2[0x118];
		WeaponValueTables valueTables;
		char __pad3[0xD58];
	};

	static_assert(sizeof(WeaponTunables) == 0x1418);
	static_assert(offsetof(WeaponTunables, valueTables) == 0x5C0);

	struct WeaponSecondaryMovementState
	{
		char __pad0[0x50];
	};

	static_assert(sizeof(WeaponSecondaryMovementState) == 0x50);

	struct WeaponSecondaryMovementEntry
	{
		char __pad0[0x20];
	};

	static_assert(sizeof(WeaponSecondaryMovementEntry) == 0x20);

	struct WeaponSecondaryMovement
	{
		XHash name;
		char __pad0[0xC0];
		std::int32_t stateCount0;
		char __pad1[0x4];
		WeaponSecondaryMovementState** states0;
		std::int32_t stateCount1;
		char __pad2[0x4];
		WeaponSecondaryMovementState** states1;
		std::int32_t entryCount0;
		char __pad3[0x4];
		WeaponSecondaryMovementEntry* entries0;
		std::int32_t entryCount1;
		char __pad4[0x4];
		WeaponSecondaryMovementEntry* entries1;
	};

	static_assert(sizeof(WeaponSecondaryMovement) == 0x108);
	static_assert(offsetof(WeaponSecondaryMovement, entries1) == 0x100);

	struct CinematicMotion
	{
		XHash name;
		char __pad0[0x100];
		ValueArray values0;
		char __pad1[0x38];
		ValueArray values1;
		char __pad2[0x68];
		ValueArray values2;
		char __pad3[0x38];
		ValueArray values3;
		char __pad4[0x50];
	};

	static_assert(sizeof(CinematicMotion) == 0x270);
	static_assert(offsetof(CinematicMotion, values3) == 0x210);

	struct TagFxEntry
	{
		FxEffectDef* effect;
		FxEffectDef* tagEffects[12];
		char __pad0[0x10];
	};

	static_assert(sizeof(TagFxEntry) == 0x78);
	static_assert(offsetof(TagFxEntry, tagEffects) == 0x8);

	struct TagFx
	{
		XHash name;
		std::int32_t entryCount;
		char __pad0[0x4];
		TagFxEntry* entries;
	};

	static_assert(sizeof(TagFx) == 0x18);
	static_assert(offsetof(TagFx, entries) == 0x10);

	struct Beam
	{
		XHash name;
		Material* material0;
		Material* material1;
		char __pad0[0x48];
		ValueArray values0;
		char __pad1[0x8];
		ValueArray values1;
		char __pad2[0x30];
		ValueArray values2;
		char __pad3[0x8];
		ValueArray values3;
		char __pad4[0x38];
		ValueArray values4;
		char __pad5[0x8];
		ValueArray values5;
		char __pad6[0x8];
		ValueArray values6;
		char __pad7[0x20];
		ValueArray values7;
		char __pad8[0x8];
		ValueArray values8;
		char __pad9[0x8];
		ValueArray values9;
		char __pad10[0x8];
		ValueArray values10;
		char __pad11[0x50];
		ValueArray values11;
		ValueArray values12;
		char __pad12[0x1D8];
		FxEffectDef* effects[6];
		char __pad13[0x38];
		ValueArray values13;
		char __pad14[0x8];
		ValueArray values14;
		char __pad15[0x8];
		ValueArray values15;
		char __pad16[0x8];
		ValueArray values16;
		char __pad17[0x10];
	};

	static_assert(sizeof(Beam) == 0x4E8);
	static_assert(offsetof(Beam, values16) == 0x4C8);

	struct Tracer
	{
		XHash name;
		char __pad0[0x8];
		Material* material;
		char __pad1[0xC0];
		Beam* beam;
		char __pad2[0x10];
		ValueArray values;
	};

	static_assert(sizeof(Tracer) == 0x100);
	static_assert(offsetof(Tracer, values) == 0xF0);

	struct Laser
	{
		XHash name;
		Material* material;
		char __pad0[0x28];
		FxEffectDef* effect;
		char __pad1[0x30];
	};

	static_assert(sizeof(Laser) == 0x70);
	static_assert(offsetof(Laser, effect) == 0x38);

	struct FlameTable
	{
		XHash name;
		char __pad0[0x1B0];
		Material* materials[8];
		char __pad1[0x20];
	};

	static_assert(sizeof(FlameTable) == 0x218);
	static_assert(offsetof(FlameTable, materials) == 0x1B8);

	struct TriggerEffectDesc
	{
		XHash name;
		char __pad0[0x10];
	};

	static_assert(sizeof(TriggerEffectDesc) == 0x18);

	struct BallisticDesc
	{
		XHash name;
		char __pad0[0x18];
	};

	static_assert(sizeof(BallisticDesc) == 0x20);

	struct ShellShock
	{
		XHash name;
		char __pad0[0x70];
		char* string;
		char __pad1[0x18];
	};

	static_assert(sizeof(ShellShock) == 0x98);
	static_assert(offsetof(ShellShock, string) == 0x78);

	struct StatusEffectData
	{
		char __pad0[0x28];
		PostFxBundle* postFxBundle0;
		PostFxBundle* postFxBundle1;
		TagFx* tagFx0;
		TagFx* tagFx1;
		TagFx* tagFx2;
		char __pad1[0x8];
		ScriptBundle* scriptBundle;
		char __pad2[0x18];
		RumbleInfo* rumble;
		char __pad3[0x30];
		Gesture* gesture;
		GestureTable* gestureTable;
		char __pad4[0x8];
	};

	static_assert(sizeof(StatusEffectData) == 0xC8);
	static_assert(offsetof(StatusEffectData, gestureTable) == 0xB8);

	struct StatusEffectFx
	{
		FxEffectDef* effect0;
		FxEffectDef* effect1;
		char __pad0[0x10];
	};

	static_assert(sizeof(StatusEffectFx) == 0x20);
	static_assert(offsetof(StatusEffectFx, effect1) == 0x8);

	struct StatusEffect
	{
		XHash name;
		char __pad0[0x10];
		ShellShock* shellShock;
		StatusEffectData data;
		char __pad1[0x80];
		GfxImage* image;
		char __pad2[0x30];
		StatusEffectFx fx;
		ValueArray values;
		char __pad3[0x40];
	};

	static_assert(sizeof(StatusEffect) == 0x210);
	static_assert(offsetof(StatusEffect, values) == 0x1C0);

	struct WeaponCamoLayer
	{
		Material* material;
		char __pad0[0x20];
		GfxImage* image;
		char __pad1[0x10];
	};

	static_assert(sizeof(WeaponCamoLayer) == 0x40);
	static_assert(offsetof(WeaponCamoLayer, image) == 0x28);

	struct WeaponCamoEntry
	{
		Material** materials;
		GfxImage** images;
		WeaponCamoLayer* layers;
		std::uint16_t count;
		char __pad0[0x6];
	};

	static_assert(sizeof(WeaponCamoEntry) == 0x20);
	static_assert(offsetof(WeaponCamoEntry, count) == 0x18);

	struct WeaponCamo
	{
		XHash name;
		std::uint64_t entryCount;
		WeaponCamoEntry* entries;
		char __pad0[0x8];
	};

	static_assert(sizeof(WeaponCamo) == 0x20);
	static_assert(offsetof(WeaponCamo, entries) == 0x10);

	struct WeaponCamoBindingEntry
	{
		std::uint64_t materialCount;
		Material** materials;
		std::uint64_t indexCount;
		std::uint16_t* indices;
	};

	static_assert(sizeof(WeaponCamoBindingEntry) == 0x20);
	static_assert(offsetof(WeaponCamoBindingEntry, indices) == 0x18);

	struct WeaponCamoBinding
	{
		XHash name;
		std::uint64_t entryCount;
		WeaponCamoBindingEntry* entries;
		WeaponCamo* camo;
	};

	static_assert(sizeof(WeaponCamoBinding) == 0x20);
	static_assert(offsetof(WeaponCamoBinding, camo) == 0x18);

	struct SharedWeaponSounds
	{
		XHash name;
		SurfaceSoundDef* surfaceSoundDef0;
		SurfaceSoundDef* surfaceSoundDef1;
		EntitySoundImpacts* entitySoundImpacts[9];
		char __pad0[0x120];
	};

	static_assert(sizeof(SharedWeaponSounds) == 0x180);
	static_assert(offsetof(SharedWeaponSounds, entitySoundImpacts) == 0x18);

	struct Objective
	{
		XHash name;
		char __pad0[0x38];
		char* string;
		char __pad1[0x78];
		GfxImage* image0;
		char __pad2[0x8];
		GfxImage* image1;
		char __pad3[0x20];
		GfxImage* image2;
		char __pad4[0x18];
		ScriptBundleObject bundle;
		char __pad5[0x8];
		GfxImage* image3;
		char __pad6[0x18];
	};

	static_assert(sizeof(Objective) == 0x160);
	static_assert(offsetof(Objective, image3) == 0x140);

	struct WeaponBlueprintEntry
	{
		char __pad0[0x8];
		GfxImage* image;
		XModel* models[12];
		WeaponCamoBinding* camoBinding;
	};

	static_assert(sizeof(WeaponBlueprintEntry) == 0x78);
	static_assert(offsetof(WeaponBlueprintEntry, camoBinding) == 0x70);

	struct WeaponBlueprint
	{
		XHash name;
		char __pad0[0x10];
		XModel* models[12];
		std::int32_t entryCount;
		char __pad1[0x4];
		WeaponBlueprintEntry* entries;
		char __pad2[0x8];
		XAnim** anims0;
		XAnim** anims1;
		FxEffectDef* effects0[5];
		TagFx* tagFx;
		FxImpactTable* impactTables[2];
		FxEffectDef* effects1[4];
		Tracer** tracers0;
		Tracer** tracers1;
		char __pad3[0x8];
		WeaponCamoBinding* camoBinding;
		ScriptBundle* scriptBundle;
		ScriptBundleObject bundle;
	};

	static_assert(sizeof(WeaponBlueprint) == 0x148);
	static_assert(offsetof(WeaponBlueprint, bundle) == 0x128);

	struct AttachmentEntry
	{
		char __pad0[0x20];
	};

	static_assert(sizeof(AttachmentEntry) == 0x20);

	struct Attachment
	{
		XHash name;
		char __pad0[0x40];
		GfxImage* image;
		char __pad1[0x4B0];
		RumbleInfo* rumble;
		StatusEffect* statusEffects[4];
		std::int32_t entryCount0;
		char __pad2[0x4];
		AttachmentEntry* entries0;
		std::int32_t entryCount1;
		char __pad3[0x4];
		AttachmentEntry* entries1;
		std::int32_t entryCount2;
		char __pad4[0x4];
		AttachmentEntry* entries2;
		char __pad5[0x60];
		ValueArray values0;
		char __pad6[0x30];
		ValueArray values1;
		char __pad7[0x218];
	};

	static_assert(sizeof(Attachment) == 0x820);
	static_assert(offsetof(Attachment, values1) == 0x5F8);

	struct AttachmentModelSet
	{
		XModel* models[3];
	};

	static_assert(sizeof(AttachmentModelSet) == 0x18);

	struct AttachmentModelPair
	{
		XModel* models[2];
	};

	static_assert(sizeof(AttachmentModelPair) == 0x10);

	struct AttachmentUniqueView
	{
		char __pad0[0x10];
		AttachmentModelSet modelSets0[2];
		char __pad1[0x38];
		AttachmentModelSet modelSets1[2];
		char __pad2[0x50];
		GfxImage* image;
		char __pad3[0x10];
	};

	static_assert(sizeof(AttachmentUniqueView) == 0x110);
	static_assert(offsetof(AttachmentUniqueView, image) == 0xF8);

	struct AttachmentUnique
	{
		XHash name;
		char __pad0[0x18];
		GfxImage* images0[4];
		char __pad1[0x30];
		scr_string_t* strings0;
		scr_string_t* strings1;
		AttachmentUniqueView views[2];
		char __pad2[0x8];
		AttachmentModelPair modelPairs0[3];
		char __pad3[0x58];
		AttachmentModelPair modelPairs1[3];
		char __pad4[0x58];
		XModel* models[2];
		FxEffectDef* effect;
		WeaponCamo* camo;
		WeaponCamoBinding* camoBinding;
		char __pad5[0xA8];
		GfxImage* images1[2];
		char __pad6[0x70];
		XAnim** anims0;
		XAnim** anims1;
		char __pad7[0x40];
		Gesture** gestures;
		char __pad8[0x70];
		SharedWeaponSounds* sounds[2];
		char __pad9[0x170];
		FxEffectDef* effects0[9];
		FxEffectDef* effect0;
		TagFx* tagFx0;
		TagFx* tagFx1;
		char __pad10[0x40];
		FxImpactTable* impactTables[4];
		FxEffectDef* effects1[4];
		char __pad11[0x38];
		Tracer** tracers0;
		Tracer** tracers1;
		Laser* lasers[3];
		char __pad12[0x150];
		WeaponValueTables valueTables;
		char __pad13[0x310];
	};

	static_assert(sizeof(AttachmentUnique) == 0xDF0);
	static_assert(offsetof(AttachmentUnique, valueTables) == 0x9E0);

	struct WeaponStyleEntry
	{
		char __pad0[0x8];
		WeaponStyleSettings* settings;
	};

	static_assert(sizeof(WeaponStyleEntry) == 0x10);
	static_assert(offsetof(WeaponStyleEntry, settings) == 0x8);

	struct WeaponModelSet
	{
		char __pad0[0x8];
		XModel* model0;
		XModel* model1;
		FxEffectDef* effect;
		char __pad1[0x18];
	};

	static_assert(sizeof(WeaponModelSet) == 0x38);
	static_assert(offsetof(WeaponModelSet, effect) == 0x18);

	struct WeaponStatusEffect
	{
		StatusEffect* statusEffect;
		char __pad0[0x18];
	};

	static_assert(sizeof(WeaponStatusEffect) == 0x20);

	struct WeaponValueEntry
	{
		char __pad0[0x8];
	};

	static_assert(sizeof(WeaponValueEntry) == 0x8);

	struct WeaponData
	{
		char __pad0[0x5DC];
	};

	static_assert(sizeof(WeaponData) == 0x5DC);

	struct Weapon
	{
		XHash name;
		char __pad0[0x4C8];
		XAnim** anims0;
		XAnim** anims1;
		scr_string_t* strings0;
		scr_string_t* strings1;
		scr_string_t* strings2;
		XCam* cams[3];
		TagFx* tagFx0;
		WeaponValueEntry* values0;
		WeaponValueEntry* values1;
		WeaponValueEntry* values2;
		WeaponValueEntry* values3;
		WeaponValueEntry* values4;
		WeaponValueEntry* values5;
		Gesture** gestures;
		char __pad1[0x8];
		GestureTable** gestureTables;
		WeaponStatusEffect statusEffects[5];
		Tracer** tracers0;
		Tracer** tracers1;
		WeaponCamo* camo;
		WeaponCamoBinding* camoBinding;
		Attachment** attachments;
		AttachmentUnique** attachmentUniques;
		char __pad2[0x400];
		WeaponData* data;
		WeaponTunables* tunables;
		WeaponStyleEntry styles[9];
		WeaponBlueprint** blueprints;
		WeaponSecondaryMovement* secondaryMovement;
		CinematicMotion* cinematicMotion;
		XAnim* anims2[2];
		XModel* models0[2];
		XModel* model0;
		XModel* models1[3];
		char __pad3[0x58];
		XModel* models2[3];
		char __pad4[0x58];
		FxEffectDef* effects0[14];
		FxEffectDef* effect0;
		TagFx* tagFx1[3];
		FxEffectDef* effect1;
		SharedWeaponSounds* sounds[2];
		TagFx* tagFx2[4];
		TagFx* tagFx3[4];
		GfxImage* images0[5];
		char* string;
		FxEffectDef* effects2[6];
		GfxImage* images1[3];
		XModel* models3[6];
		GfxImage* images2[3];
		GfxImage* images3[2];
		Material* materials[5];
		GfxImage* images4[7];
		RumbleInfo* rumbles[15];
		Laser* lasers[3];
		FlameTable* flameTables[2];
		Beam* beams[7];
		GfxImage* images5[3];
		BallisticDesc* ballisticDesc;
		GfxImage* image0;
		Objective* objectives[2];
		XModel* models4[2];
		FxEffectDef* effects3[5];
		XModel* model5;
		FxEffectDef* effects4[2];
		FxEffectDef* effects5[13];
		TagFx* tagFx4[2];
		FxEffectDef* effect2;
		FxEffectDef* effects6[3];
		char __pad5[0x10];
		FxImpactTable* impactTables[5];
		ImpactSoundsTable* impactSoundsTables[5];
		ScriptBundle* scriptBundles0[6];
		GfxImage* images6[3];
		ScriptBundle* scriptBundle1;
		TriggerEffectDesc* triggerEffects[2];
		char __pad6[0x35C];
		std::int32_t blueprintCount;
		char __pad7[0x34];
		std::int32_t valueCount0;
		std::int32_t valueCount1;
		std::int32_t valueCount2;
		char __pad8[0xB8];
		WeaponModelSet modelSets[4];
		char __pad9[0x320];
	};

	static_assert(sizeof(Weapon) == 0x1918);
	static_assert(offsetof(Weapon, modelSets) == 0x1518);

	struct FontIconEntry
	{
		char __pad0[0x8];
		GfxImage* image;
		char __pad1[0x10];
	};

	static_assert(sizeof(FontIconEntry) == 0x20);
	static_assert(offsetof(FontIconEntry, image) == 0x8);

	struct FontIconAlias
	{
		char __pad0[0xC];
	};

	static_assert(sizeof(FontIconAlias) == 0xC);

	struct FontIcon
	{
		XHash name;
		std::int32_t entryCount;
		std::int32_t aliasCount;
		FontIconEntry* entries;
		FontIconAlias* aliases;
	};

	static_assert(sizeof(FontIcon) == 0x20);
	static_assert(offsetof(FontIcon, aliases) == 0x18);

	struct RawTextFile
	{
		XHash name;
		std::int32_t length;
		char __pad0[0x4];
		char* buffer;
	};

	static_assert(sizeof(RawTextFile) == 0x18);
	static_assert(offsetof(RawTextFile, buffer) == 0x10);

	struct AnimTree
	{
		XHash name;
		char __pad0[0x8];
		std::int32_t size;
		char __pad1[0x4];
		char* data;
	};

	static_assert(sizeof(AnimTree) == 0x20);
	static_assert(offsetof(AnimTree, data) == 0x18);

	struct FootstepEntry
	{
		std::int32_t surfaceSoundDefCount;
		char __pad0[0x4];
		SurfaceSoundDef** surfaceSoundDefs;
		std::int32_t hashCount;
		char __pad1[0x4];
		XHash* hashes;
		SndAliasModifier* modifier;
	};

	static_assert(sizeof(FootstepEntry) == 0x28);
	static_assert(offsetof(FootstepEntry, modifier) == 0x20);

	struct FootstepTable
	{
		XHash name;
		FootstepEntry entries[9];
	};

	static_assert(sizeof(FootstepTable) == 0x170);
	static_assert(offsetof(FootstepTable, entries) == 0x8);

	struct PlayerGlobalTunables;

	struct AimTable
	{
		XHash name;
		char __pad0[0x258];
	};

	static_assert(sizeof(AimTable) == 0x260);

	struct ShootTable
	{
		XHash name;
		char __pad0[0x3E8];
	};

	static_assert(sizeof(ShootTable) == 0x3F0);

	struct AnimSelectorColumn
	{
		char __pad0[0x18];
	};

	static_assert(sizeof(AnimSelectorColumn) == 0x18);

	struct AnimSelectorCell
	{
		char __pad0[0x10];
	};

	static_assert(sizeof(AnimSelectorCell) == 0x10);

	struct AnimSelectorRowData0
	{
		char __pad0[0x20];
	};

	static_assert(sizeof(AnimSelectorRowData0) == 0x20);

	struct AnimSelectorRowData1
	{
		char __pad0[0x30];
	};

	static_assert(sizeof(AnimSelectorRowData1) == 0x30);

	struct AnimSelectorRow
	{
		AnimSelectorCell* cells;
		std::int32_t cellCount;
		char __pad0[0x4];
		AnimSelectorRowData0* data0;
		AnimSelectorRowData1* data1;
	};

	static_assert(sizeof(AnimSelectorRow) == 0x20);
	static_assert(offsetof(AnimSelectorRow, data1) == 0x18);

	struct AnimSelectorDefault
	{
		char __pad0[0x10];
	};

	static_assert(sizeof(AnimSelectorDefault) == 0x10);

	struct AnimSelector
	{
		char __pad0[0x8];
		AnimSelectorColumn* columns;
		std::int32_t columnCount;
		char __pad1[0x4];
		AnimSelectorRow* rows;
		std::int32_t rowCount;
		char __pad2[0x4];
		AnimSelectorDefault* defaultRow;
		char __pad3[0x38];
	};

	static_assert(sizeof(AnimSelector) == 0x68);
	static_assert(offsetof(AnimSelector, defaultRow) == 0x28);

	struct AnimSelectorTable
	{
		XHash name;
		AnimSelector* selectors;
		std::int32_t selectorCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(AnimSelectorTable) == 0x18);
	static_assert(offsetof(AnimSelectorTable, selectorCount) == 0x10);

	struct AnimMappingEntry
	{
		std::uint64_t animCount;
		XAnim** anims;
	};

	static_assert(sizeof(AnimMappingEntry) == 0x10);
	static_assert(offsetof(AnimMappingEntry, anims) == 0x8);

	struct AnimMappingTable
	{
		XHash name;
		XHash* hashes;
		AnimMappingEntry* entries;
		std::int32_t entryCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(AnimMappingTable) == 0x20);
	static_assert(offsetof(AnimMappingTable, entryCount) == 0x18);

	struct AnimState
	{
		char __pad0[0x10];
		std::int32_t* indices;
		std::int32_t indexCount;
		char __pad1[0x4];
	};

	static_assert(sizeof(AnimState) == 0x20);
	static_assert(offsetof(AnimState, indexCount) == 0x18);

	struct AnimStateTransition
	{
		char __pad0[0x88];
		std::int32_t* indices;
		std::int32_t indexCount;
		char __pad1[0x4];
	};

	static_assert(sizeof(AnimStateTransition) == 0x98);
	static_assert(offsetof(AnimStateTransition, indexCount) == 0x90);

	struct AnimStateParameter
	{
		char __pad0[0x80];
	};

	static_assert(sizeof(AnimStateParameter) == 0x80);

	struct AnimStateMachine
	{
		XHash name;
		AnimState* states;
		std::int32_t stateCount;
		char __pad0[0x4];
		AnimStateTransition* transitions;
		std::int32_t transitionCount;
		char __pad1[0x4];
		AnimStateParameter* parameters;
		std::int32_t parameterCount;
		char __pad2[0x4];
	};

	static_assert(sizeof(AnimStateMachine) == 0x38);
	static_assert(offsetof(AnimStateMachine, parameterCount) == 0x30);

	struct BehaviorTransition
	{
		char __pad0[0x20];
	};

	static_assert(sizeof(BehaviorTransition) == 0x20);

	struct BehaviorState
	{
		char __pad0[0x38];
		BehaviorTransition* transitions;
		std::uint32_t transitionCount;
		char __pad1[0x4];
	};

	static_assert(sizeof(BehaviorState) == 0x48);
	static_assert(offsetof(BehaviorState, transitionCount) == 0x40);

	struct BehaviorStateMachine
	{
		XHash name;
		BehaviorState* states;
		std::uint32_t stateCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(BehaviorStateMachine) == 0x18);
	static_assert(offsetof(BehaviorStateMachine, stateCount) == 0x10);

	struct BehaviorTreeNode
	{
		char __pad0[0x18];
		std::int32_t* children;
		std::uint32_t childCount;
		char __pad1[0x4];
		BehaviorStateMachine* stateMachine;
		char __pad2[0x50];
	};

	static_assert(sizeof(BehaviorTreeNode) == 0x80);
	static_assert(offsetof(BehaviorTreeNode, stateMachine) == 0x28);

	struct BehaviorTree
	{
		XHash name;
		BehaviorTreeNode* nodes;
		std::int32_t nodeCount;
		char __pad0[0x4];
		std::int32_t* roots;
		std::int32_t rootCount;
		char __pad1[0x4];
	};

	static_assert(sizeof(BehaviorTree) == 0x28);
	static_assert(offsetof(BehaviorTree, rootCount) == 0x20);

	struct StringTableCell
	{
		char* string;
		char __pad0[0x4];
		std::uint8_t type;
		char __pad1[0x3];
	};

	static_assert(sizeof(StringTableCell) == 0x10);
	static_assert(offsetof(StringTableCell, type) == 0xC);

	struct StringTable
	{
		XHash name;
		std::int32_t columnCount;
		std::int32_t rowCount;
		char __pad0[0x4];
		std::int32_t hashCount;
		StringTableCell* values;
		std::int32_t* cellIndices;
		std::int16_t* cellOrder;
		std::uint64_t* hashes;
	};

	static_assert(sizeof(StringTable) == 0x38);
	static_assert(offsetof(StringTable, hashes) == 0x30);

	struct StructuredTableCell
	{
		char* string;
		std::int32_t type;
		char __pad0[0x4];
	};

	static_assert(sizeof(StructuredTableCell) == 0x10);
	static_assert(offsetof(StructuredTableCell, type) == 0x8);

	struct StructuredTableProperty
	{
		char* name;
		char __pad0[0x8];
	};

	static_assert(sizeof(StructuredTableProperty) == 0x10);

	struct StructuredTable
	{
		XHash name;
		char __pad0[0x4];
		std::int32_t propertyCount;
		std::int32_t entryCount;
		char __pad1[0x4];
		StructuredTableCell* cells;
		std::int32_t* cellIndices;
		StructuredTableProperty* properties;
		std::int32_t* propertyIndices;
	};

	static_assert(sizeof(StructuredTable) == 0x38);
	static_assert(offsetof(StructuredTable, propertyIndices) == 0x30);

	struct ScriptBundleList
	{
		XHash name;
		char __pad0[0x4];
		std::uint32_t bundleCount;
		ScriptBundle** bundles;
	};

	static_assert(sizeof(ScriptBundleList) == 0x18);
	static_assert(offsetof(ScriptBundleList, bundles) == 0x10);

	struct XModelAlias
	{
		XHash name;
		XModel* models[50];
	};

	static_assert(sizeof(XModelAlias) == 0x198);
	static_assert(offsetof(XModelAlias, models) == 0x8);

	struct CharacterModelPair
	{
		XModelAlias* alias;
		XModel* model;
	};

	static_assert(sizeof(CharacterModelPair) == 0x10);
	static_assert(offsetof(CharacterModelPair, model) == 0x8);

	struct CharacterModelEntry
	{
		XModel* model;
		char __pad0[0x8];
	};

	static_assert(sizeof(CharacterModelEntry) == 0x10);

	struct Character
	{
		XHash name;
		ScriptBundle* scriptBundles0[2];
		ScriptBundleList* scriptBundleList;
		ScriptBundle* scriptBundles1[8];
		XModel* model0;
		XModelAlias* alias0;
		XModel* model1;
		XModelAlias* aliases0[2];
		XModel* model2;
		XModelAlias* alias1;
		XModel* model3;
		XModelAlias* alias2;
		XModel* models0[3];
		CharacterModelPair modelPairs[8];
		XModelAlias* alias3;
		CharacterModelEntry modelEntries[17];
		Ragdoll* ragdoll;
		char* string0;
		char* string1;
		char __pad0[0x8];
		FxEffectDef* effects[2];
	};

	static_assert(sizeof(Character) == 0x288);
	static_assert(offsetof(Character, effects) == 0x278);

	struct AIType
	{
		XHash name;
		char* string;
		char __pad0[0x20];
		Weapon* weapons0[4];
		char __pad1[0x448];
		Weapon* weapons1[4];
		char __pad2[0x8];
		Weapon* weapon;
		char __pad3[0xE8];
		FootstepTable* footstepTable;
		char __pad4[0x8];
		SurfaceFxTable* surfaceFxTables[2];
		char __pad5[0x88];
		ScriptBundle* scriptBundle0;
		char __pad6[0x138];
		Character* characters[8];
		char __pad7[0x8];
		BehaviorTree* behaviorTree;
		AnimStateMachine* animStateMachine;
		AnimSelectorTable* animSelectorTable;
		char __pad8[0x8];
		AnimMappingTable* animMappingTables[3];
		AimTable* aimTable;
		ShootTable* shootTable;
		ScriptBundle* scriptBundle1;
		ScriptBundle* scriptBundles0[5];
		ScriptBundle* scriptBundles1[3];
		char __pad9[0x18];
		ScriptBundle* scriptBundles2[2];
		char __pad10[0x928];
		GfxImage* image;
		char __pad11[0x18];
		ScriptBundle* scriptBundle2;
		char __pad12[0x110];
	};

	static_assert(sizeof(AIType) == 0x12F8);
	static_assert(offsetof(AIType, scriptBundle2) == 0x11E0);

	struct VehicleFxDef
	{
		XHash name;
		char __pad0[0x8];
		FxEffectDef* effects0[40];
		FxEffectDef* effects1[40];
		FxEffectDef* effects2[40];
	};

	static_assert(sizeof(VehicleFxDef) == 0x3D0);
	static_assert(offsetof(VehicleFxDef, effects2) == 0x290);

	struct OverheadCameraKey
	{
		char __pad0[0x8];
	};

	static_assert(sizeof(OverheadCameraKey) == 0x8);

	struct OverheadCameraValues
	{
		char __pad0[0x8];
		OverheadCameraKey* values0;
		std::int32_t valueCount0;
		char __pad1[0x4];
		OverheadCameraKey* values1;
		std::int32_t valueCount1;
		char __pad2[0x4];
		OverheadCameraKey* values2;
		std::int32_t valueCount2;
		char __pad3[0x4];
		OverheadCameraKey* values3;
		std::int32_t valueCount3;
		char __pad4[0x4];
	};

	static_assert(sizeof(OverheadCameraValues) == 0x48);
	static_assert(offsetof(OverheadCameraValues, valueCount3) == 0x40);

	struct OverheadCameraTunables
	{
		XHash name;
		char __pad0[0x20];
		OverheadCameraValues values0;
		OverheadCameraValues values1;
		char __pad1[0x38];
		CinematicMotion* cinematicMotion;
	};

	static_assert(sizeof(OverheadCameraTunables) == 0xF8);
	static_assert(offsetof(OverheadCameraTunables, cinematicMotion) == 0xF0);

	struct TTFKerningEntry
	{
		char __pad0[0xA];
	};

	static_assert(sizeof(TTFKerningEntry) == 0xA);

	struct TTFGlyphEntry
	{
		char __pad0[0x6];
	};

	static_assert(sizeof(TTFGlyphEntry) == 0x6);

	struct TTFDef
	{
		XHash name;
		char __pad0[0xC];
		std::int32_t fileSize;
		char* file;
		char __pad1[0x8];
		TTFDef* fallback0;
		TTFDef* fallback1;
		std::int32_t glyphCount;
		char __pad2[0x4];
		std::uint32_t* glyphs;
		GfxImage* image;
		TTFKerningEntry* kerningEntries;
		std::uint32_t kerningCount;
		char __pad3[0x4];
		TTFGlyphEntry* glyphEntries;
		std::uint32_t glyphEntryCount;
		char __pad4[0x4];
	};

	static_assert(sizeof(TTFDef) == 0x70);
	static_assert(offsetof(TTFDef, glyphEntryCount) == 0x68);

	struct CompassIconSet
	{
		GfxImage* images[3];
		char __pad0[0x18];
	};

	static_assert(sizeof(CompassIconSet) == 0x30);

	struct CompassEntry
	{
		GfxImage* image;
		CompassIconSet iconSets[2];
	};

	static_assert(sizeof(CompassEntry) == 0x68);
	static_assert(offsetof(CompassEntry, iconSets) == 0x8);

	struct CompassMapData
	{
		char __pad0[0x10];
		TTFDef* font;
		char __pad1[0x28];
		GfxImage* image0;
		char __pad2[0x18];
		GfxImage* image1;
		GfxImage* image2;
	};

	static_assert(sizeof(CompassMapData) == 0x70);
	static_assert(offsetof(CompassMapData, image2) == 0x68);

	struct CompassTunables
	{
		XHash name;
		CompassMapData mapData;
		char __pad0[0x8];
		char* label0;
		char __pad1[0x8];
		char* label1;
		std::uint32_t entryCount;
		char __pad2[0x4];
		CompassEntry* entries;
	};

	static_assert(sizeof(CompassTunables) == 0xA8);
	static_assert(offsetof(CompassTunables, entries) == 0xA0);

	struct ObjectiveList
	{
		XHash name;
		std::uint32_t objectiveCount;
		char __pad0[0x4];
		Objective** objectives;
	};

	static_assert(sizeof(ObjectiveList) == 0x18);
	static_assert(offsetof(ObjectiveList, objectives) == 0x10);

	struct MapTableCompassSet
	{
		GfxImage* images[3];
		char __pad0[0x28];
	};

	static_assert(sizeof(MapTableCompassSet) == 0x40);

	struct MapTableEntry
	{
		char* name;
		char __pad0[0x20];
		char* string0;
		char __pad1[0x10];
		char* string1;
		char __pad2[0x20];
		GfxImage* previewImages[2];
		char __pad3[0x8];
		std::uint32_t compassSetCount;
		char __pad4[0x4];
		MapTableCompassSet* compassSets;
		char* string2;
		char* string3;
		ObjectiveList* objectiveList;
		char* string4;
		ScriptBundleList* scriptBundleList;
		char __pad5[0x18];
		char* string5;
		GfxImage* image;
		std::uint32_t stringCount0;
		char __pad6[0x4];
		char** strings0;
		char** strings1;
		std::uint32_t stringCount1;
		char __pad7[0x4];
		char** strings2;
		std::uint32_t stringCount2;
		char __pad8[0x4];
		char** strings3;
		char** strings4;
		char __pad9[0x10];
		PlayerGlobalTunables* playerGlobalTunables;
		OverheadCameraTunables* overheadCameraTunables[5];
		char __pad10[0x10];
		CompassTunables* compassTunables[2];
		char __pad11[0x10];
		ScriptBundleObject bundle;
		char __pad12[0x18];
	};

	static_assert(sizeof(MapTableEntry) == 0x1C8);
	static_assert(offsetof(MapTableEntry, bundle) == 0x190);

	struct MapTable
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		MapTableEntry** entries;
		std::uint32_t* entryIndices;
		char __pad1[0x8];
		XHash* hashes0;
		XHash* hashes1;
		char __pad2[0x4];
		std::uint32_t hashCount0;
		std::uint32_t hashCount1;
		char __pad3[0x4];
	};

	static_assert(sizeof(MapTable) == 0x48);
	static_assert(offsetof(MapTable, hashCount1) == 0x40);

	struct MapTableListSpan
	{
		std::uint64_t count;
		XHash* hashes;
	};

	static_assert(sizeof(MapTableListSpan) == 0x10);
	static_assert(offsetof(MapTableListSpan, hashes) == 0x8);

	struct MapTableList
	{
		XHash name;
		MapTableListSpan spans[8];
	};

	static_assert(sizeof(MapTableList) == 0x88);
	static_assert(offsetof(MapTableList, spans) == 0x8);

	struct PlayerTunableGroup
	{
		ValueArray values[7];
		char __pad0[0x10];
	};

	static_assert(sizeof(PlayerTunableGroup) == 0x80);

	struct PlayerTunableValues0
	{
		ValueArray values[35];
		char __pad0[0x8];
	};

	static_assert(sizeof(PlayerTunableValues0) == 0x238);

	struct PlayerTunableValues1
	{
		char __pad0[0x8];
		PlayerTunableGroup groups[2];
		ValueArray values[33];
	};

	static_assert(sizeof(PlayerTunableValues1) == 0x318);
	static_assert(offsetof(PlayerTunableValues1, values) == 0x108);

	struct PlayerTunableSet
	{
		PlayerTunableValues0 values0;
		PlayerTunableValues1 values1;
	};

	static_assert(sizeof(PlayerTunableSet) == 0x550);
	static_assert(offsetof(PlayerTunableSet, values1) == 0x238);

	struct PlayerGlobalTunables
	{
		XHash name;
		PlayerTunableSet sets[3];
		ValueArray values[5];
		CinematicMotion* cinematicMotions[3];
	};

	static_assert(sizeof(PlayerGlobalTunables) == 0x1060);
	static_assert(offsetof(PlayerGlobalTunables, cinematicMotions) == 0x1048);

	struct VehicleSoundParams
	{
		char* name;
		char __pad0[0x20];
	};

	static_assert(sizeof(VehicleSoundParams) == 0x28);

	struct VehicleSoundAlias
	{
		char* name;
		char __pad0[0x20];
	};

	static_assert(sizeof(VehicleSoundAlias) == 0x28);

	struct VehicleSoundEvent
	{
		char* name;
		VehicleSoundParams* params;
		char __pad0[0x8];
		scr_string_t tag0;
		scr_string_t tag1;
	};

	static_assert(sizeof(VehicleSoundEvent) == 0x20);
	static_assert(offsetof(VehicleSoundEvent, tag1) == 0x1C);

	struct VehicleSoundDef
	{
		XHash name;
		char __pad0[0x310];
		char* string;
		char __pad1[0x290];
		VehicleSoundAlias aliases[20];
		VehicleSoundEvent events[20];
		char __pad2[0x140];
	};

	static_assert(sizeof(VehicleSoundDef) == 0xC90);
	static_assert(offsetof(VehicleSoundDef, events) == 0x8D0);

	struct VehicleAssemblyImagePair
	{
		char __pad0[0x28];
		GfxImage* image0;
		GfxImage* image1;
		char __pad1[0x10];
	};

	static_assert(sizeof(VehicleAssemblyImagePair) == 0x48);
	static_assert(offsetof(VehicleAssemblyImagePair, image1) == 0x30);

	struct VehicleAssemblyPart
	{
		char __pad0[0x18];
		XModel* model;
		DestructibleDef* destructibleDef;
		std::uint64_t valueCount;
		std::uint64_t* values;
		char __pad1[0x8];
	};

	static_assert(sizeof(VehicleAssemblyPart) == 0x40);
	static_assert(offsetof(VehicleAssemblyPart, values) == 0x30);

	struct VehicleAssemblyMaterialSet
	{
		std::uint64_t indexCount;
		std::uint16_t* indices;
		std::uint64_t materialCount;
		Material** materials;
	};

	static_assert(sizeof(VehicleAssemblyMaterialSet) == 0x20);
	static_assert(offsetof(VehicleAssemblyMaterialSet, materials) == 0x18);

	struct VehicleAssemblyTag
	{
		char __pad0[0x18];
		scr_string_t name;
	};

	static_assert(sizeof(VehicleAssemblyTag) == 0x1C);
	static_assert(offsetof(VehicleAssemblyTag, name) == 0x18);

	struct VehicleAssemblyModelList
	{
		std::uint64_t modelCount;
		XModel** models;
	};

	static_assert(sizeof(VehicleAssemblyModelList) == 0x10);
	static_assert(offsetof(VehicleAssemblyModelList, models) == 0x8);

	struct VehicleAssembly
	{
		XHash name;
		char __pad0[0x10];
		GfxImage* image;
		char __pad1[0x8];
		ScriptBundle* scriptBundle;
		std::uint64_t imagePairCount;
		VehicleAssemblyImagePair* imagePairs;
		std::uint64_t partCount0;
		VehicleAssemblyPart* parts0;
		std::uint64_t indexCount0;
		std::uint16_t* indices0;
		std::uint64_t partCount1;
		VehicleAssemblyPart* parts1;
		std::uint64_t indexCount1;
		std::uint16_t* indices1;
		std::uint64_t partCount2;
		VehicleAssemblyPart* parts2;
		std::uint64_t materialSetCount;
		VehicleAssemblyMaterialSet* materialSets;
		std::uint64_t materialCount;
		Material** materials;
		char __pad2[0x8];
		std::uint64_t tagCount;
		VehicleAssemblyTag* tags;
		std::uint64_t modelListCount;
		VehicleAssemblyModelList* modelLists;
	};

	static_assert(sizeof(VehicleAssembly) == 0xD8);
	static_assert(offsetof(VehicleAssembly, modelLists) == 0xD0);

	struct VehicleTireFx
	{
		char __pad0[0x20];
		FxEffectDef* effect;
	};

	static_assert(sizeof(VehicleTireFx) == 0x28);
	static_assert(offsetof(VehicleTireFx, effect) == 0x20);

	struct VehicleSurfaceFx
	{
		char __pad0[0x28];
		FxEffectDef* effect;
		SurfaceFxTable* surfaceFxTable;
		char __pad1[0x8];
	};

	static_assert(sizeof(VehicleSurfaceFx) == 0x40);
	static_assert(offsetof(VehicleSurfaceFx, surfaceFxTable) == 0x30);

	struct VehicleBundleEntry
	{
		char __pad0[0x20];
		ScriptBundle* scriptBundle;
		char __pad1[0x30];
	};

	static_assert(sizeof(VehicleBundleEntry) == 0x58);
	static_assert(offsetof(VehicleBundleEntry, scriptBundle) == 0x20);

	struct VehicleSeat
	{
		char __pad0[0xC];
	};

	static_assert(sizeof(VehicleSeat) == 0xC);

	struct VehicleCollision
	{
		ClipMapMesh* mesh;
		ClipMapTable* table;
		char __pad0[0x8];
	};

	static_assert(sizeof(VehicleCollision) == 0x18);
	static_assert(offsetof(VehicleCollision, table) == 0x8);

	struct VehicleNamedString
	{
		char* name;
		char __pad0[0x8];
	};

	static_assert(sizeof(VehicleNamedString) == 0x10);

	struct Vehicle
	{
		XHash name;
		char __pad0[0x158];
		ValueArray values0[5];
		char __pad1[0x10];
		ValueArray values1;
		char __pad2[0x4E0];
		VehicleTireFx tireFx[16];
		char __pad3[0x10];
		VehicleSurfaceFx surfaceFx[16];
		char __pad4[0x258];
		VehicleBundleEntry bundleEntries[5];
		CinematicMotion* cinematicMotion;
		char __pad5[0xA8];
		VehicleSoundDef* soundDefs[2];
		char __pad6[0x30];
		char* string0;
		char __pad7[0x40];
		VehicleSeat* seats;
		std::int32_t seatCount;
		char __pad8[0xA4];
		XModel* models[4];
		VehicleCollision* collisions[2];
		char __pad9[0x8];
		FxEffectDef* effect0;
		char __pad10[0x8];
		FxEffectDef* effect1;
		char __pad11[0x8];
		RumbleInfo* rumble0;
		FxEffectDef* effect2;
		char __pad12[0x8];
		VehicleFxDef* fxDef;
		VehicleFxDef* fxDefs[6];
		char __pad13[0x8];
		FxEffectDef* effect3;
		char __pad14[0x10];
		FxEffectDef* effect4;
		char __pad15[0x18];
		FxEffectDef* effect5;
		char __pad16[0x10];
		FxEffectDef* effects0[4];
		char __pad17[0x10];
		FxEffectDef* effects1[4];
		char __pad18[0x10];
		FxEffectDef* effects2[4];
		char __pad19[0x10];
		FxEffectDef* effects3[4];
		char __pad20[0x20];
		char* string1;
		char __pad21[0x10];
		RumbleInfo* rumble1;
		char* string2;
		char __pad22[0x48];
		GfxImage* image0;
		char __pad23[0x18];
		VehicleNamedString strings[19];
		char __pad24[0x8];
		char* string3;
		char __pad25[0x38];
		RumbleInfo* rumble2;
		char __pad26[0x8];
		RumbleInfo* rumbles[4];
		char __pad27[0x48];
		ValueArray values2;
		char __pad28[0x300];
		ScriptBundle* scriptBundles[6];
		AnimStateMachine* animStateMachine;
		AnimSelectorTable* animSelectorTable;
		AnimMappingTable* animMappingTable;
		char __pad29[0x8];
		SurfaceSoundDef* surfaceSoundDef;
		SurfaceFxTable* surfaceFxTable;
		DestructibleDef* destructibleDef;
		GfxImage* image1;
		GfxImage* image2;
		char __pad30[0x18];
		ValueArray values3[5];
		char __pad31[0x8];
		ValueArray values4[7];
		ScriptBundleObject bundle;
		VehicleAssembly* assembly;
	};

	static_assert(sizeof(Vehicle) == 0x1C18);
	static_assert(offsetof(Vehicle, assembly) == 0x1C10);

	struct UnlockableItem
	{
		char* string;
		XHash name;
		char __pad0[0x28];
		GfxImage* images[6];
		char __pad1[0x70];
		std::uint32_t valueCount;
		char __pad2[0x4];
		std::uint32_t* values;
		std::uint32_t tagCount0;
		char __pad3[0x4];
		scr_string_t* tags0;
		std::uint32_t tagCount1;
		char __pad4[0x4];
		scr_string_t* tags1;
		char __pad5[0x10];
		ScriptBundleObject bundle;
	};

	static_assert(sizeof(UnlockableItem) == 0x138);
	static_assert(offsetof(UnlockableItem, bundle) == 0x118);

	struct UnlockableItemTable
	{
		XHash name;
		char __pad0[0x4];
		std::uint32_t itemCount0;
		UnlockableItem** items0;
		char __pad1[0x20];
		std::uint32_t itemCount1;
		char __pad2[0x4];
		UnlockableItem** items1;
	};

	static_assert(sizeof(UnlockableItemTable) == 0x48);
	static_assert(offsetof(UnlockableItemTable, items1) == 0x40);

	struct MotionMatchingInput
	{
		XHash name;
		char __pad0[0x1A0];
	};

	static_assert(sizeof(MotionMatchingInput) == 0x1A8);

	struct StorageFile
	{
		char* string;
		XHash name;
		char __pad0[0x8];
		char** fileNames;
		RawFile** files;
		char __pad1[0x4];
		std::uint32_t fileCount;
		char __pad2[0x20];
	};

	static_assert(sizeof(StorageFile) == 0x50);
	static_assert(offsetof(StorageFile, fileCount) == 0x2C);

	struct StorageFileList
	{
		XHash name;
		std::uint32_t fileCount;
		char __pad0[0x4];
		StorageFile** files;
	};

	static_assert(sizeof(StorageFileList) == 0x18);
	static_assert(offsetof(StorageFileList, files) == 0x10);

	struct StoreProduct
	{
		XHash name;
		GfxImage* images[3];
		char* strings0[3];
		char __pad0[0x18];
		char* strings1[2];
		char __pad1[0x18];
		char* strings2[4];
	};

	static_assert(sizeof(StoreProduct) == 0x98);
	static_assert(offsetof(StoreProduct, strings2) == 0x78);

	struct StoreCategory
	{
		char* string;
		XHash name;
		GfxImage* image;
		char __pad0[0x8];
		std::uint64_t productCount;
		StoreProduct** products;
		char __pad1[0x8];
	};

	static_assert(sizeof(StoreCategory) == 0x38);
	static_assert(offsetof(StoreCategory, products) == 0x28);

	struct StoreCategoryList
	{
		XHash name;
		std::uint64_t categoryCount;
		StoreCategory** categories;
	};

	static_assert(sizeof(StoreCategoryList) == 0x18);
	static_assert(offsetof(StoreCategoryList, categories) == 0x10);

	struct RankLevel
	{
		char __pad0[0x10];
	};

	static_assert(sizeof(RankLevel) == 0x10);

	struct Rank
	{
		XHash name;
		char __pad0[0x28];
		GfxImage* images[2];
		std::uint64_t levelCount;
		RankLevel* levels;
	};

	static_assert(sizeof(Rank) == 0x50);
	static_assert(offsetof(Rank, levels) == 0x48);

	struct RankTable
	{
		XHash name;
		std::uint64_t rankCount;
		Rank** ranks;
	};

	static_assert(sizeof(RankTable) == 0x18);
	static_assert(offsetof(RankTable, ranks) == 0x10);

	struct Prestige
	{
		XHash name;
		char __pad0[0x20];
		GfxImage* images[2];
	};

	static_assert(sizeof(Prestige) == 0x38);
	static_assert(offsetof(Prestige, images) == 0x28);

	struct PrestigeTable
	{
		XHash name;
		std::uint64_t prestigeCount;
		Prestige** prestiges;
	};

	static_assert(sizeof(PrestigeTable) == 0x18);
	static_assert(offsetof(PrestigeTable, prestiges) == 0x10);

	struct FirstPartyEntitlement
	{
		XHash name;
		char* string;
		std::uint64_t idCount;
		char** ids;
	};

	static_assert(sizeof(FirstPartyEntitlement) == 0x20);
	static_assert(offsetof(FirstPartyEntitlement, ids) == 0x18);

	struct FirstPartyEntitlementList
	{
		XHash name;
		std::uint64_t entitlementCount;
		FirstPartyEntitlement** entitlements;
	};

	static_assert(sizeof(FirstPartyEntitlementList) == 0x18);
	static_assert(offsetof(FirstPartyEntitlementList, entitlements) == 0x10);

	struct Entitlement
	{
		XHash name;
		char __pad0[0x8];
		FirstPartyEntitlement* firstPartyEntitlement;
		char __pad1[0x18];
		char* string;
		std::uint64_t valueCount;
		std::uint32_t* values;
	};

	static_assert(sizeof(Entitlement) == 0x48);
	static_assert(offsetof(Entitlement, values) == 0x40);

	struct EntitlementList
	{
		XHash name;
		std::uint64_t entitlementCount;
		Entitlement** entitlements;
	};

	static_assert(sizeof(EntitlementList) == 0x18);
	static_assert(offsetof(EntitlementList, entitlements) == 0x10);

	struct Sku
	{
		XHash name;
		char __pad0[0x8];
	};

	static_assert(sizeof(Sku) == 0x10);

	struct LabelStore
	{
		XHash name;
		std::uint64_t skuCount;
		Sku** skus;
	};

	static_assert(sizeof(LabelStore) == 0x18);
	static_assert(offsetof(LabelStore, skus) == 0x10);

	struct LabelStoreList
	{
		XHash name;
		std::uint64_t storeCount;
		LabelStore** stores;
	};

	static_assert(sizeof(LabelStoreList) == 0x18);
	static_assert(offsetof(LabelStoreList, stores) == 0x10);

	struct MilestoneTier
	{
		char __pad0[0x10];
	};

	static_assert(sizeof(MilestoneTier) == 0x10);

	struct Milestone
	{
		XHash name;
		char __pad0[0x8];
		std::uint64_t tierCount;
		MilestoneTier* tiers;
	};

	static_assert(sizeof(Milestone) == 0x20);
	static_assert(offsetof(Milestone, tiers) == 0x18);

	struct MilestoneTable
	{
		XHash name;
		std::uint64_t milestoneCount;
		Milestone** milestones;
	};

	static_assert(sizeof(MilestoneTable) == 0x18);
	static_assert(offsetof(MilestoneTable, milestones) == 0x10);

	struct PlayerSettings
	{
		XHash name;
		char __pad0[0x8];
		std::uint32_t dataSize;
		char __pad1[0x4];
		std::uint8_t* data;
	};

	static_assert(sizeof(PlayerSettings) == 0x20);
	static_assert(offsetof(PlayerSettings, data) == 0x18);

	struct BitField
	{
		XHash name;
		char __pad0[0x4];
		std::uint32_t rowCount;
		std::uint32_t columnCount;
		char __pad1[0x4];
		std::uint64_t* bits;
	};

	static_assert(sizeof(BitField) == 0x20);
	static_assert(offsetof(BitField, bits) == 0x18);

	struct TacticalQuery
	{
		XHash name;
		char __pad0[0x10];
		char* strings[9];
		char __pad1[0x548];
	};

	static_assert(sizeof(TacticalQuery) == 0x5A8);
	static_assert(offsetof(TacticalQuery, strings) == 0x18);

	struct RawString
	{
		XHash name;
		char* string;
	};

	static_assert(sizeof(RawString) == 0x10);
	static_assert(offsetof(RawString, string) == 0x8);

	struct LeaderboardColumn
	{
		char* name;
		char __pad0[0x38];
	};

	static_assert(sizeof(LeaderboardColumn) == 0x40);

	struct LeaderboardDef
	{
		XHash name;
		char __pad0[0x4];
		std::int32_t columnCount;
		char __pad1[0x10];
		LeaderboardColumn* columns;
		char __pad2[0x8];
	};

	static_assert(sizeof(LeaderboardDef) == 0x30);
	static_assert(offsetof(LeaderboardDef, columnCount) == 0xC);
	static_assert(offsetof(LeaderboardDef, columns) == 0x20);

	struct CgMediaTable
	{
		XHash name;
		FootstepTable* footstepTables[10][4];
		SurfaceSoundDef* surfaceSoundDefs[2];
		Material* materials0[31];
		GfxImage* images[76];
		Material* materials1[7];
		SurfaceFxTable* surfaceFxTable;
		PhysPreset* physPreset;
		Beam* beam;
		FxEffectDef* effects[23];
		char __pad0[0xF0];
	};

	static_assert(sizeof(CgMediaTable) == 0x6A8);
	static_assert(offsetof(CgMediaTable, surfaceSoundDefs) == 0x148);
	static_assert(offsetof(CgMediaTable, materials0) == 0x158);
	static_assert(offsetof(CgMediaTable, images) == 0x250);
	static_assert(offsetof(CgMediaTable, materials1) == 0x4B0);
	static_assert(offsetof(CgMediaTable, surfaceFxTable) == 0x4E8);
	static_assert(offsetof(CgMediaTable, beam) == 0x4F8);
	static_assert(offsetof(CgMediaTable, effects) == 0x500);

	struct BulletPenetration
	{
		XHash name;
		char __pad0[0x1E0];
	};

	static_assert(sizeof(BulletPenetration) == 0x1E8);

	struct LocDmgTable
	{
		XHash name;
		char __pad0[0x58];
	};

	static_assert(sizeof(LocDmgTable) == 0x60);

	struct CinematicCamera
	{
		XHash name;
		char __pad0[0x68];
	};

	static_assert(sizeof(CinematicCamera) == 0x70);

	struct CinematicSequenceShot
	{
		CinematicCamera* camera;
		char __pad0[0x28];
	};

	static_assert(sizeof(CinematicSequenceShot) == 0x30);

	struct CinematicSequence
	{
		XHash name;
		CinematicSequenceShot* shots;
		std::int32_t shotCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(CinematicSequence) == 0x18);
	static_assert(offsetof(CinematicSequence, shotCount) == 0x10);

	struct SpectateCameraShot
	{
		CinematicCamera* camera;
		char __pad0[0x8];
	};

	static_assert(sizeof(SpectateCameraShot) == 0x10);

	struct SpectateCameraEntry
	{
		char __pad0[0x20];
		std::int32_t shotCount;
		std::int32_t pointCount;
		char __pad1[0x8];
		SpectateCameraShot* shots;
		float (*points)[3];
	};

	static_assert(sizeof(SpectateCameraEntry) == 0x40);
	static_assert(offsetof(SpectateCameraEntry, shotCount) == 0x20);
	static_assert(offsetof(SpectateCameraEntry, shots) == 0x30);
	static_assert(offsetof(SpectateCameraEntry, points) == 0x38);

	struct SpectateCamera
	{
		XHash name;
		std::int32_t entryCount;
		char __pad0[0x4];
		SpectateCameraEntry* entries;
	};

	static_assert(sizeof(SpectateCamera) == 0x18);
	static_assert(offsetof(SpectateCamera, entries) == 0x10);

	struct HighlightReelSequence
	{
		CinematicSequence* sequence;
		char __pad0[0x8];
	};

	static_assert(sizeof(HighlightReelSequence) == 0x10);

	struct HighlightReelSegment
	{
		char __pad0[0x28];
	};

	static_assert(sizeof(HighlightReelSegment) == 0x28);

	struct HighlightReelInfo
	{
		char __pad0[0x60];
		std::int32_t sequenceCount;
		char __pad1[0x4];
		HighlightReelSequence* sequences;
		std::int32_t segmentCount;
		char __pad2[0x4];
		HighlightReelSegment* segments;
	};

	static_assert(sizeof(HighlightReelInfo) == 0x80);
	static_assert(offsetof(HighlightReelInfo, sequences) == 0x68);
	static_assert(offsetof(HighlightReelInfo, segments) == 0x78);

	struct HighlightReelInfoDefines
	{
		XHash name;
		std::int32_t infoCount;
		char __pad0[0x4];
		HighlightReelInfo* infos;
		HighlightReelInfo** infoPointers;
	};

	static_assert(sizeof(HighlightReelInfoDefines) == 0x20);
	static_assert(offsetof(HighlightReelInfoDefines, infos) == 0x10);
	static_assert(offsetof(HighlightReelInfoDefines, infoPointers) == 0x18);

	struct HighlightReelValue
	{
		char __pad0[0x8];
	};

	static_assert(sizeof(HighlightReelValue) == 0x8);

	struct HighlightReelValueTable
	{
		XHash name;
		std::uint32_t valueCount;
		char __pad0[0x4];
		HighlightReelValue* values;
	};

	static_assert(sizeof(HighlightReelValueTable) == 0x18);
	static_assert(offsetof(HighlightReelValueTable, values) == 0x10);

	struct DLogEventField
	{
		char __pad0[0x28];
	};

	struct DLogEventFieldList
	{
		char __pad0[0x8];
		char* name;
		DLogEventField* fields;
		std::int32_t fieldCount;
		char __pad1[0x4];
	};

	struct DLogEventFieldListEntry
	{
		char __pad0[0x10];
		DLogEventFieldList* list;
		char __pad1[0x8];
	};

	struct DLogEventIntList
	{
		char __pad0[0x20];
		std::int32_t* values;
		std::int32_t valueCount;
		char __pad1[0x4];
	};

	struct DLogEventIntListEntry
	{
		char __pad0[0x8];
		DLogEventIntList* list;
		char __pad1[0x10];
	};

	struct DLogEvent
	{
		char* eventName;
		XHash name;
		char __pad0[0x8];
		char* description;
		char __pad1[0x8];
		DDLFile* ddl;
		DLogEventFieldListEntry* fieldLists;
		std::int32_t fieldListCount;
		char __pad2[0x4];
		DLogEventField* fields;
		std::int32_t fieldCount;
		char __pad3[0x4];
		DLogEventIntListEntry* intLists;
		std::int32_t intListCount;
		char __pad4[0xC];
	};

	static_assert(sizeof(DLogEventField) == 0x28);
	static_assert(sizeof(DLogEventFieldList) == 0x20);
	static_assert(sizeof(DLogEventFieldListEntry) == 0x20);
	static_assert(sizeof(DLogEventIntList) == 0x30);
	static_assert(offsetof(DLogEventIntList, values) == 0x20);
	static_assert(sizeof(DLogEventIntListEntry) == 0x20);
	static_assert(sizeof(DLogEvent) == 0x68);
	static_assert(offsetof(DLogEvent, ddl) == 0x28);
	static_assert(offsetof(DLogEvent, intLists) == 0x50);

	struct ShaderProgramArg
	{
		char __pad0[0xC];
	};

	struct ShaderProgramImage
	{
		GfxImage* image;
		char __pad0[0x10];
	};

	struct ShaderProgram
	{
		char __pad0[0x8];
		std::uint8_t* bytecode;
		std::uint32_t bytecodeSize;
		char __pad1[0x18];
		std::uint32_t argCount;
		ShaderProgramArg* args;
		char __pad2[0x14];
		std::uint8_t imageCount;
		std::uint8_t valueCount;
		char __pad3[0x2];
		ShaderProgramImage* images;
		std::uint64_t* values;
	};

	struct ComputeShader
	{
		char __pad0[0x8];
		ShaderProgram programs[2];
	};

	struct ComputeShaderSet
	{
		XHash name;
		std::uint32_t shaderCount;
		char __pad0[0x4];
		ComputeShader** shaders;
	};

	struct RaytraceShaderLibrary
	{
		char __pad0[0x8];
		std::uint8_t* bytecode;
		std::uint32_t bytecodeSize;
		char __pad1[0x4];
	};

	struct RaytraceShaderHitGroup
	{
		char __pad0[0x8];
		char* name;
		char* entryName;
		char __pad1[0x128];
		ShaderProgram* program;
	};

	struct RaytraceShaderExport
	{
		char __pad0[0x8];
		char* name;
		char* entryName;
		char __pad1[0x128];
	};

	struct RaytraceShaderExportList
	{
		std::uint64_t count;
		RaytraceShaderExport* exports;
	};

	struct RaytraceShaderSymbol
	{
		char __pad0[0x8];
		char* name;
		char __pad1[0xB0];
	};

	struct RaytraceShaderStage
	{
		char __pad0[0x8];
		RaytraceShaderLibrary* library;
		char __pad1[0x18];
		std::uint64_t hitGroupCount;
		RaytraceShaderHitGroup* hitGroups;
		RaytraceShaderExportList exportLists[5];
		std::uint64_t symbolCount;
		RaytraceShaderSymbol* symbols;
	};

	struct RaytraceShader
	{
		char __pad0[0x8];
		RaytraceShaderStage stages[2];
	};

	struct RaytraceShaderSet
	{
		XHash name;
		std::uint32_t shaderCount;
		char __pad0[0x4];
		RaytraceShader** shaders;
	};

	static_assert(sizeof(ShaderProgramArg) == 0xC);
	static_assert(sizeof(ShaderProgramImage) == 0x18);
	static_assert(sizeof(ShaderProgram) == 0x60);
	static_assert(offsetof(ShaderProgram, args) == 0x30);
	static_assert(offsetof(ShaderProgram, imageCount) == 0x4C);
	static_assert(offsetof(ShaderProgram, images) == 0x50);
	static_assert(sizeof(ComputeShader) == 0xC8);
	static_assert(sizeof(ComputeShaderSet) == 0x18);
	static_assert(sizeof(RaytraceShaderLibrary) == 0x18);
	static_assert(sizeof(RaytraceShaderHitGroup) == 0x148);
	static_assert(offsetof(RaytraceShaderHitGroup, program) == 0x140);
	static_assert(sizeof(RaytraceShaderExport) == 0x140);
	static_assert(sizeof(RaytraceShaderSymbol) == 0xC0);
	static_assert(sizeof(RaytraceShaderStage) == 0x98);
	static_assert(offsetof(RaytraceShaderStage, hitGroups) == 0x30);
	static_assert(offsetof(RaytraceShaderStage, symbols) == 0x90);
	static_assert(sizeof(RaytraceShader) == 0x138);
	static_assert(sizeof(RaytraceShaderSet) == 0x18);

	struct GameTypeTableEntryValue
	{
		char __pad0[0x38];
	};

	static_assert(sizeof(GameTypeTableEntryValue) == 0x38);

	struct GameTypeTableEntry
	{
		char* strings0[3];
		char __pad0[0x8];
		XHash name;
		char __pad1[0x58];
		char* string0;
		GfxImage* image;
		char __pad2[0x10];
		char* strings1[3];
		ScriptBundleObject bundle;
		char __pad3[0x8];
		std::int32_t dataSize;
		char __pad4[0x4];
		std::uint8_t* data;
		char __pad5[0x18];
		char* string1;
		char __pad6[0x50];
		std::uint32_t valueCount;
		char __pad7[0x4];
		GameTypeTableEntryValue* values;
		char __pad8[0x8];
	};

	static_assert(sizeof(GameTypeTableEntry) == 0x178);
	static_assert(offsetof(GameTypeTableEntry, values) == 0x168);

	struct GameTypeTable
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		GameTypeTableEntry** entries;
		std::uint32_t* values;
		char __pad1[0x8];
	};

	static_assert(sizeof(GameTypeTable) == 0x28);
	static_assert(offsetof(GameTypeTable, values) == 0x18);

	struct CraftIcon
	{
		char __pad0[0x8];
		GfxImage* image;
		XHash name;
		char __pad1[0x20];
	};

	static_assert(sizeof(CraftIcon) == 0x38);
	static_assert(offsetof(CraftIcon, name) == 0x10);

	struct CraftIconList
	{
		XHash name;
		char __pad0[0x8];
		std::uint64_t iconCount;
		CraftIcon** icons;
	};

	static_assert(sizeof(CraftIconList) == 0x20);
	static_assert(offsetof(CraftIconList, icons) == 0x18);

	struct CraftWeaponSticker
	{
		char __pad0[0x8];
		GfxImage* image;
		XHash name;
		char __pad1[0x18];
		Material* material;
		char __pad2[0x8];
	};

	static_assert(sizeof(CraftWeaponSticker) == 0x40);
	static_assert(offsetof(CraftWeaponSticker, material) == 0x30);

	struct CraftWeaponStickerList
	{
		XHash name;
		char __pad0[0x8];
		std::uint64_t stickerCount;
		CraftWeaponSticker** stickers;
	};

	static_assert(sizeof(CraftWeaponStickerList) == 0x20);
	static_assert(offsetof(CraftWeaponStickerList, stickers) == 0x18);

	struct CraftBackground
	{
		char __pad0[0x8];
		XHash name;
		GfxImage* image0;
		GfxImage* image1;
		char __pad1[0x40];
	};

	static_assert(sizeof(CraftBackground) == 0x60);
	static_assert(offsetof(CraftBackground, image1) == 0x18);

	struct CraftBackgroundList
	{
		XHash name;
		char __pad0[0xE18];
		std::uint64_t backgroundCount;
		CraftBackground** backgrounds;
	};

	static_assert(sizeof(CraftBackgroundList) == 0xE30);
	static_assert(offsetof(CraftBackgroundList, backgrounds) == 0xE28);

	struct CraftMaterial
	{
		char __pad0[0x10];
		XHash name;
		char __pad1[0x8];
		GfxImage* image0;
		GfxImage* image1;
		Material* material;
	};

	static_assert(sizeof(CraftMaterial) == 0x38);
	static_assert(offsetof(CraftMaterial, material) == 0x30);

	struct CraftMaterialList
	{
		XHash name;
		char __pad0[0x108];
		std::uint64_t materialCount;
		CraftMaterial** materials;
	};

	static_assert(sizeof(CraftMaterialList) == 0x120);
	static_assert(offsetof(CraftMaterialList, materials) == 0x118);

	struct CraftCategory
	{
		char __pad0[0x8];
		XHash name;
		char __pad1[0x10];
	};

	static_assert(sizeof(CraftCategory) == 0x20);
	static_assert(offsetof(CraftCategory, name) == 0x8);

	struct CraftCategoryList
	{
		XHash name;
		char __pad0[0x10];
		std::uint64_t categoryCount0;
		CraftCategory** categories0;
		std::uint64_t categoryCount1;
		CraftCategory** categories1;
		std::uint64_t categoryCount2;
		CraftCategory** categories2;
	};

	static_assert(sizeof(CraftCategoryList) == 0x48);
	static_assert(offsetof(CraftCategoryList, categories2) == 0x40);

	struct CraftWeaponIconTransform
	{
		XHash name;
		char __pad0[0x48];
	};

	static_assert(sizeof(CraftWeaponIconTransform) == 0x50);

	struct CraftWeaponIconTransformList
	{
		XHash name;
		char __pad0[0x8];
		CraftWeaponIconTransform* transforms[1024];
	};

	static_assert(sizeof(CraftWeaponIconTransformList) == 0x2010);
	static_assert(offsetof(CraftWeaponIconTransformList, transforms) == 0x10);

	struct PlayerSoundsTable
	{
		XHash name;
		SurfaceSoundDef* soundDefs0[3];
		char __pad0[0x2F8];
		SurfaceSoundDef* soundDefs1[8];
		SurfaceSoundDef* soundDefs2[3];
		char __pad1[0x2E8];
		SurfaceSoundDef* soundDefs3[8];
	};

	static_assert(sizeof(PlayerSoundsTable) == 0x698);
	static_assert(offsetof(PlayerSoundsTable, soundDefs3) == 0x658);

	struct PlayerFxTable
	{
		XHash name;
		SurfaceFxTable* surfaceFxTables[9];
		FxEffectDef* effects[7];
	};

	static_assert(sizeof(PlayerFxTable) == 0x88);
	static_assert(offsetof(PlayerFxTable, effects) == 0x50);

	struct PlayerRoleCategory
	{
		XHash name;
		char __pad0[0x10];
		GfxImage* image;
		char __pad1[0x8];
		ScriptBundleObject bundle;
	};

	static_assert(sizeof(PlayerRoleCategory) == 0x48);
	static_assert(offsetof(PlayerRoleCategory, bundle) == 0x28);

	struct PlayerRoleCategoryTable
	{
		XHash name;
		std::uint32_t categoryCount;
		char __pad0[0x4];
		PlayerRoleCategory** categories;
	};

	static_assert(sizeof(PlayerRoleCategoryTable) == 0x18);
	static_assert(offsetof(PlayerRoleCategoryTable, categories) == 0x10);

	struct PlayerOutfitMaterialSlot
	{
		Material* material0;
		Material* material1;
		GfxImage* image;
	};

	static_assert(sizeof(PlayerOutfitMaterialSlot) == 0x18);
	static_assert(offsetof(PlayerOutfitMaterialSlot, image) == 0x10);

	struct PlayerOutfitMaterialImage
	{
		Material* material;
		char __pad0[0x20];
		GfxImage* image;
		char __pad1[0x10];
	};

	static_assert(sizeof(PlayerOutfitMaterialImage) == 0x40);
	static_assert(offsetof(PlayerOutfitMaterialImage, image) == 0x28);

	struct PlayerOutfitMaterialImageSlot
	{
		PlayerOutfitMaterialImage* materialImage;
		std::uint64_t* values;
		std::uint8_t valueCount;
		char __pad0[0x7];
	};

	static_assert(sizeof(PlayerOutfitMaterialImageSlot) == 0x18);
	static_assert(offsetof(PlayerOutfitMaterialImageSlot, valueCount) == 0x10);

	struct PlayerOutfitMaterialGroup
	{
		char __pad0[0x20];
		PlayerOutfitMaterialSlot* materialSlots;
		std::uint16_t* materialIndices;
		PlayerOutfitMaterialImageSlot* materialImageSlots;
		std::uint16_t materialImageSlotCount;
		std::uint16_t materialSlotCount;
		char __pad1[0x4];
	};

	static_assert(sizeof(PlayerOutfitMaterialGroup) == 0x40);
	static_assert(offsetof(PlayerOutfitMaterialGroup, materialSlotCount) == 0x3A);

	struct PlayerOutfitMaterialSet
	{
		char __pad0[0x20];
		GfxImage* image;
		std::uint32_t indexCount;
		char __pad1[0x4];
		std::uint16_t* indices;
		std::uint32_t groupCount;
		char __pad2[0x4];
		PlayerOutfitMaterialGroup* groups;
		char __pad3[0x8];
	};

	static_assert(sizeof(PlayerOutfitMaterialSet) == 0x50);
	static_assert(offsetof(PlayerOutfitMaterialSet, groups) == 0x40);

	struct PlayerOutfitPart
	{
		char __pad0[0x8];
		ScriptBundleList* bundleList;
		ScriptBundle* bundles[17];
		XModel* models[17];
		XModel* model;
		std::uint32_t materialCount;
		char __pad1[0x4];
		Material** materials;
		char __pad2[0x8];
	};

	static_assert(sizeof(PlayerOutfitPart) == 0x140);
	static_assert(offsetof(PlayerOutfitPart, materials) == 0x130);

	struct PlayerOutfitModel
	{
		char __pad0[0x8];
		XModel* model;
		std::uint32_t materialCount0;
		char __pad1[0x4];
		Material** materials0;
		char __pad2[0x4];
		std::uint32_t materialCount1;
		Material** materials1;
		char __pad3[0x8];
	};

	static_assert(sizeof(PlayerOutfitModel) == 0x38);
	static_assert(offsetof(PlayerOutfitModel, materials1) == 0x28);

	struct PlayerOutfitBundleSet0
	{
		char __pad0[0x8];
		ScriptBundleList* bundleList;
		ScriptBundle* bundles[17];
		XModel* models0[17];
		XModel* models1[17];
		XModel* models2[6];
		char __pad1[0x8];
	};

	static_assert(sizeof(PlayerOutfitBundleSet0) == 0x1E0);
	static_assert(offsetof(PlayerOutfitBundleSet0, models2) == 0x1A8);

	struct PlayerOutfitModelList
	{
		char __pad0[0x8];
		XModel* models[14];
		char __pad1[0x8];
	};

	static_assert(sizeof(PlayerOutfitModelList) == 0x80);
	static_assert(offsetof(PlayerOutfitModelList, models) == 0x8);

	struct PlayerOutfitBundleSet1
	{
		char __pad0[0x8];
		ScriptBundleList* bundleList;
		ScriptBundle* bundles[17];
		XModel* models0[17];
		XModel* models1[17];
		XModel* models2[7];
		char __pad1[0x8];
	};

	static_assert(sizeof(PlayerOutfitBundleSet1) == 0x1E8);
	static_assert(offsetof(PlayerOutfitBundleSet1, models2) == 0x1A8);

	struct PlayerOutfitIndexSet
	{
		std::uint32_t* values;
		std::uint16_t* indices;
		std::uint16_t valueCount;
		std::uint16_t indexCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(PlayerOutfitIndexSet) == 0x18);
	static_assert(offsetof(PlayerOutfitIndexSet, indexCount) == 0x12);

	struct PlayerOutfitImageSet
	{
		char __pad0[0x30];
		GfxImage* image0;
		GfxImage* image1;
		char __pad1[0x20];
	};

	static_assert(sizeof(PlayerOutfitImageSet) == 0x60);
	static_assert(offsetof(PlayerOutfitImageSet, image1) == 0x38);

	struct PlayerOutfit
	{
		XHash name;
		char __pad0[0x10];
		GfxImage* image;
		char __pad1[0x8];
		PlayerFxTable* fxTable;
		TagFx* tagFx;
		Ragdoll* ragdoll;
		FxEffectDef* effect;
		scr_string_t tag;
		char __pad2[0x8];
		std::uint32_t partCount;
		PlayerOutfitPart* parts;
		std::uint32_t modelCount;
		char __pad3[0x4];
		PlayerOutfitModel* models;
		std::uint32_t bundleSetCount0;
		char __pad4[0x4];
		PlayerOutfitBundleSet0* bundleSets0;
		std::uint32_t modelListCount;
		char __pad5[0x4];
		PlayerOutfitModelList* modelLists;
		std::uint32_t bundleSetCount1;
		char __pad6[0x4];
		PlayerOutfitBundleSet1* bundleSets1;
		std::uint32_t materialGroupCount;
		char __pad7[0x4];
		PlayerOutfitMaterialGroup* materialGroups;
		std::uint32_t materialSetCount0;
		char __pad8[0x4];
		PlayerOutfitMaterialSet* materialSets0;
		std::uint32_t materialSetCount1;
		char __pad9[0x4];
		PlayerOutfitMaterialSet* materialSets1;
		std::uint32_t indexSetCount;
		char __pad10[0x4];
		PlayerOutfitIndexSet* indexSets;
		std::uint64_t imageSetCount;
		PlayerOutfitImageSet* imageSets;
	};

	static_assert(sizeof(PlayerOutfit) == 0xF0);
	static_assert(offsetof(PlayerOutfit, imageSets) == 0xE8);

	struct CharacterBodyTypeIndexSet
	{
		std::uint32_t* values;
		std::uint32_t valueCount;
		char __pad0[0x4];
	};

	static_assert(sizeof(CharacterBodyTypeIndexSet) == 0x10);
	static_assert(offsetof(CharacterBodyTypeIndexSet, valueCount) == 0x8);

	struct CharacterBodyType
	{
		XHash name;
		char __pad0[0x20];
		ScriptBundle* scriptBundle;
		char* string;
		GfxImage* image;
		std::uint32_t outfitCount;
		char __pad1[0x4];
		PlayerOutfit** outfits;
		char __pad2[0x10];
		ScriptBundleObject bundle;
		PlayerSoundsTable* soundsTable;
		PlayerFxTable* fxTable;
		TagFx* tagFx;
		FootstepTable* footstepTables[5];
		std::uint32_t* values;
		std::uint16_t* indices;
		std::uint16_t valueCount;
		std::uint16_t indexCount;
		char __pad3[0x4];
		CharacterBodyTypeIndexSet* indexSets;
	};

	static_assert(sizeof(CharacterBodyType) == 0xE0);
	static_assert(offsetof(CharacterBodyType, indexSets) == 0xD8);

	struct PlayerMovementValue
	{
		ValueArray values;
		char __pad0[0x8];
	};

	static_assert(sizeof(PlayerMovementValue) == 0x18);

	struct PlayerMovementTunables
	{
		XHash name;
		char __pad0[0x78];
		CinematicMotion* cinematicMotion0;
		char __pad1[0x50];
		PlayerMovementValue values0[7];
		char __pad2[0x8];
		PlayerMovementValue values1[2];
		char __pad3[0x60];
		CinematicMotion* cinematicMotion1;
		char __pad4[0x50];
		PlayerMovementValue values2[7];
		char __pad5[0x8];
		PlayerMovementValue values3[2];
		char __pad6[0x68];
		ValueArray values4[2];
		char __pad7[0x8];
		ValueArray values5[2];
		char __pad8[0x8];
		ValueArray values6[5];
		char __pad9[0x28];
		RumbleInfo* rumbles[4];
	};

	static_assert(sizeof(PlayerMovementTunables) == 0x4A0);
	static_assert(offsetof(PlayerMovementTunables, rumbles) == 0x480);

	struct Talent
	{
		XHash name;
		char __pad0[0x120];
		ScriptBundleObject bundle;
	};

	static_assert(sizeof(Talent) == 0x148);
	static_assert(offsetof(Talent, bundle) == 0x128);

	struct PlayerTalentTemplate
	{
		XHash name;
		std::uint32_t talentCount0;
		char __pad0[0x4];
		Talent** talents0;
		std::uint32_t talentCount1;
		char __pad1[0x4];
		Talent** talents1;
	};

	static_assert(sizeof(PlayerTalentTemplate) == 0x28);
	static_assert(offsetof(PlayerTalentTemplate, talents1) == 0x20);

	struct PlayerRoleTemplate
	{
		XHash name;
		char __pad0[0x10];
		CharacterBodyType* bodyType;
		PlayerRoleCategory* category;
		PlayerMovementTunables* movementTunables;
		PlayerTalentTemplate* talentTemplate;
		char __pad1[0x18];
		ScriptBundleObject bundle;
		char __pad2[0xD8];
		ScriptBundle** scriptBundles;
	};

	static_assert(sizeof(PlayerRoleTemplate) == 0x150);
	static_assert(offsetof(PlayerRoleTemplate, scriptBundles) == 0x148);

	struct CustomizationTableEntry
	{
		char __pad0[0x8];
		PlayerRoleTemplate* roleTemplate;
	};

	static_assert(sizeof(CustomizationTableEntry) == 0x10);
	static_assert(offsetof(CustomizationTableEntry, roleTemplate) == 0x8);

	struct CustomizationTable
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		CustomizationTableEntry** entries;
	};

	static_assert(sizeof(CustomizationTable) == 0x18);
	static_assert(offsetof(CustomizationTable, entries) == 0x10);

	struct CharacterBodyTypeFrontend
	{
		XHash name;
		char __pad0[0x8];
		ScriptBundleObject bundle;
	};

	struct PlayerRoleTemplateFrontend
	{
		XHash name;
		CharacterBodyTypeFrontend* bodyType;
		ScriptBundleObject bundle;
	};

	struct CustomizationTableFrontendEntry
	{
		char __pad0[0x8];
		PlayerRoleTemplateFrontend* roleTemplate;
	};

	struct CustomizationTableFrontend
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		CustomizationTableFrontendEntry** entries;
	};

	struct CharMixer
	{
		XHash name;
		XModel* model;
	};

	static_assert(sizeof(CharacterBodyTypeFrontend) == 0x30);
	static_assert(sizeof(PlayerRoleTemplateFrontend) == 0x30);
	static_assert(sizeof(CustomizationTableFrontend) == 0x18);
	static_assert(sizeof(CharMixer) == 0x10);

	struct Execution
	{
		XHash name;
		char __pad0[0x8];
		scr_string_t tag;
		char __pad1[0xC];
		XCam* xcam0;
		char __pad2[0x8];
		XCam* xcam1;
		char __pad3[0x8];
		XCam* xcam2;
		XAnim* anims[12];
		char __pad4[0x30];
		ScriptBundleObject bundle;
	};

	static_assert(sizeof(Execution) == 0xF8);
	static_assert(offsetof(Execution, bundle) == 0xD8);

	struct PlayerAnimationKey
	{
		std::uint32_t valueCount;
		char __pad0[0x4];
		std::uint64_t* values;
		std::uint32_t* weights0;
		std::uint32_t* weights1;
		std::uint16_t weightCount;
		char __pad1[0x6];
	};

	static_assert(sizeof(PlayerAnimationKey) == 0x28);
	static_assert(offsetof(PlayerAnimationKey, weightCount) == 0x20);

	struct PlayerAnimationSet
	{
		char* name;
		std::uint32_t keyCount;
		char __pad0[0x4];
		PlayerAnimationKey* keys;
	};

	static_assert(sizeof(PlayerAnimationSet) == 0x18);
	static_assert(offsetof(PlayerAnimationSet, keys) == 0x10);

	struct PlayerAnimation
	{
		XHash name;
		std::uint32_t entryCount;
		char __pad0[0x4];
		std::uint32_t* values0;
		std::uint64_t* values1;
		std::uint32_t setCount0;
		char __pad1[0x4];
		PlayerAnimationSet* sets0;
		std::uint32_t setCount1;
		char __pad2[0x4];
		PlayerAnimationSet* sets1;
		char __pad3[0x18];
	};

	static_assert(sizeof(PlayerAnimation) == 0x58);
	static_assert(offsetof(PlayerAnimation, sets1) == 0x38);

	struct UIModelDataStruct;

	struct UIModelDataField
	{
		char __pad0[0x18];
		UIModelDataStruct* dataStruct;
	};

	static_assert(sizeof(UIModelDataField) == 0x20);
	static_assert(offsetof(UIModelDataField, dataStruct) == 0x18);

	struct UIModelDataStruct
	{
		XHash name;
		char __pad0[0x8];
		UIModelDataStruct* parent0;
		char __pad1[0x8];
		UIModelDataStruct* parent1;
		char __pad2[0x18];
		std::uint32_t fieldCount;
		char __pad3[0x4];
		UIModelDataField* fields;
	};

	static_assert(sizeof(UIModelDataStruct) == 0x50);
	static_assert(offsetof(UIModelDataStruct, fields) == 0x48);

	enum SprayOrGestureType : std::uint64_t
	{
		SPRAY_OR_GESTURE_TYPE_GESTURE = 0x43391A0572202ED4,
		SPRAY_OR_GESTURE_TYPE_MATERIAL = 0x56D7AB194448A4F3,
		SPRAY_OR_GESTURE_TYPE_GESTURE_TABLE = 0x2339D24B2F9E718B,
	};

	struct SprayOrGestureEntry
	{
		Gesture* gesture0;
		Gesture* gesture1;
		std::uint32_t valueCount;
		std::uint64_t* values;
	};

	struct SprayOrGestureGestures
	{
		Gesture* gesture0;
		Gesture* gesture1;
		std::uint32_t entryCount;
		SprayOrGestureEntry* entries;
	};

	struct SprayOrGestureItem
	{
		XHash name;
		XHash type;
		union
		{
			SprayOrGestureGestures gestures;
			Material* material;
			GestureTable* gestureTable;
		};
		ScriptBundleObject bundle;
	};

	struct SprayOrGestureList
	{
		XHash name;
		std::uint32_t itemCount;
		SprayOrGestureItem** items;
	};

	static_assert(sizeof(SprayOrGestureEntry) == 0x20);
	static_assert(sizeof(SprayOrGestureGestures) == 0x20);
	static_assert(sizeof(SprayOrGestureItem) == 0x50);
	static_assert(offsetof(SprayOrGestureItem, bundle) == 0x30);
	static_assert(sizeof(SprayOrGestureList) == 0x18);
}
