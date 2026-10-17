#pragma once
#include <d3d11.h>
#include <formats/ddl.hpp>

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

	struct MaterialStreamRouting
	{
		unsigned char source;
		unsigned char dest;
		unsigned char mask;
	};

	struct MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[32];
		ID3D11InputLayout* decl[280];
	};

	struct MaterialVertexDeclaration
	{
		const char* name;
		unsigned char streamCount;
		bool hasOptionalSource;
		MaterialVertexStreamRouting routing;
	};
	static_assert(sizeof(MaterialVertexDeclaration) == 0x930);

	enum MaterialShaderArgumentType : std::uint8_t
	{
		MTL_ARG_LITERAL_CONST = 0x6,
	};

	union MaterialArgumentDef
	{
		float* literalConst;
		unsigned int codeConst;
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
		char __pad1[20];
		MaterialShaderArgument* args;
	};
	static_assert(sizeof(MaterialPass) == 0x58);
	static_assert(offsetof(MaterialPass, zone) == 0x36);
	static_assert(offsetof(MaterialPass, perPrimConstantBuffer) == 0x38);

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

	constexpr auto technique_count = 114;

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

	enum XSurfaceSharedFlags : std::uint8_t
	{
		XSURFACE_SHARED_STREAMED = 0x1,
	};

	struct XSurfaceShared
	{
		unsigned char flags;
		unsigned char __pad0;
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

	struct NetConstStrings
	{
		const char* name;
		int stringType;
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
		char __pad0[40];
		const char* sndAliasPrefix;
		char __pad1[8];
		PhysPresetSound sounds[3];
		char __pad2[16];
		SndPhysPreset* sndPhysPreset;
	};
	static_assert(sizeof(PhysPreset) == 0x88);
	static_assert(offsetof(PhysPreset, sounds) == 64);

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
		SndParameterModifierOp parameterModifierOps[8]; // the op of slot 6 is not used
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
		char __pad0[12];
	};

	struct pathnode_constant_t
	{
		unsigned short type;
		char __pad0[6];
		scr_string_t targetname;
		scr_string_t script_linkName;
		scr_string_t script_noteworthy;
		scr_string_t target;
		scr_string_t animscript;
		char __pad1[28];
		scr_string_t parentName;
		char __pad2[8];
		unsigned short totalLinkCount;
		char __pad3[2];
		pathlink_s* Links;
	};
	static_assert(sizeof(pathnode_constant_t) == 0x50);
	static_assert(offsetof(pathnode_constant_t, parentName) == 56);

	struct pathnode_t
	{
		pathnode_constant_t constant;
		char __pad0[112];
	};
	static_assert(sizeof(pathnode_t) == 192);

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
		unsigned short* pathZones;
		int dynStatesBytes;
		unsigned char* pathDynStates;
	};
	static_assert(sizeof(PathData) == 0x88);
	static_assert(offsetof(PathData, visBytes) == 28);
	static_assert(offsetof(PathData, zoneCount) == 104);

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
		char __pad0;
		std::uint64_t drawSurf[2];
		std::uint64_t surfaceTypeBits;
		unsigned int hashIndex;
	};
	static_assert(sizeof(MaterialInfo) == 0x30);

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
		unsigned char materialType;
		unsigned char assetFlags;
		unsigned char __pad0[4];
		unsigned char constantBufferCount;
		unsigned char layerCount;
		unsigned char __pad1[9];
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
	static_assert(offsetof(Material, parentMaterial) == 216);
	static_assert(offsetof(Material, constantBufferTable) == 344);

	struct dmPolytopeData
	{
		vec4_t* vec4_array0;
		vec4_t* vec4_array1;
		unsigned int* uint_array;
		unsigned char* byte_array;
		char __pad0[12];
		int vec4_array0_count;
		int vec4_array1_count;
		int uint_array_count;
		char __pad1[40];
	};
	static_assert(sizeof(dmPolytopeData) == 0x60);
	static_assert(offsetof(dmPolytopeData, vec4_array0_count) == 44);

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
		char __pad0[4];
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
		char __pad0[36];
	};
	static_assert(sizeof(PhysCollmap) == 0x40);

	struct XSurfaceCollisionLeaf
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		unsigned int leafCount;
		vec4_t* nodes;
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
		unsigned int partBits[8];
		char __pad2[24];
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
		unsigned int partBits[8];
		char __pad1[20];
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
		char __pad0[4];
		unsigned short numClientBones;
		char __pad1[70];
		scr_string_t* boneNames;
		unsigned short* parentList;
		short* tagAngles;
		float* tagPositions;
		unsigned char* partClassification;
		char* baseMat;
		XModelLodInfo lodInfo[6];
		unsigned char numLods;
		char collLod;
		char __pad2[4];
		unsigned short collSurfCount;
		char* collSurfs;
		int contents;
		char __pad3[4];
		char* boneInfo;
		float radius;
		float boundsMidPoint[3];
		float boundsHalfSize[3];
		char __pad4[4];
		float lodDistScale;
		float lodDistSq[6];
		char __pad10[22];
		unsigned short extraBoneNameCount;
		unsigned short extraDataCount;
		char __pad5[2];
		scr_string_t* extraBoneNames;
		char* extraData;
		PhysPreset* physPreset;
		PhysCollmap* physCollmap;
		char __pad6[2];
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
		char __pad9[24];
	};
	static_assert(sizeof(XModelBase) == 0x378);
	static_assert(offsetof(XModelBase, numClientBones) == 16);
	static_assert(offsetof(XModelBase, boneNames) == 88);
	static_assert(offsetof(XModelBase, lodInfo) == 136);
	static_assert(offsetof(XModelBase, numLods) == 616);
	static_assert(offsetof(XModelBase, collSurfCount) == 622);
	static_assert(offsetof(XModelBase, radius) == 648);
	static_assert(offsetof(XModelBase, lodDistSq) == 684);
	static_assert(offsetof(XModelBase, boneInfo) == 640);
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

	struct FxTrailVertex
	{
		char __pad0[32];
	};

	struct FxTrailDef
	{
		char __pad0[28];
		int vertCount;
		FxTrailVertex* verts;
		int indCount;
		char __pad1[4];
		unsigned short* inds;
	};
	static_assert(sizeof(FxTrailDef) == 0x38);

	struct FxSparkFountainDef
	{
		char __pad0[0x34];
	};

	struct FxSpotLightDef
	{
		char __pad0[48];
		const char* name;
	};
	static_assert(sizeof(FxSpotLightDef) == 0x38);

	struct FxOmniLightDef
	{
		char __pad0[24];
		const char* name;
	};
	static_assert(sizeof(FxOmniLightDef) == 0x20);

	struct FxFlareDef
	{
		char __pad0[64];
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

	struct FxElemDef
	{
		char __pad0[188];
		unsigned char elemType;
		unsigned char __pad1;
		unsigned char visualCount;
		unsigned char velIntervalCount;
		unsigned char visStateIntervalCount;
		char __pad2[7];
		char* velSamples;
		char* visSamples;
		FxElemDefVisuals visuals;
		char __pad3[24];
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		char __pad4[16];
		FxElemExtendedDefPtr extended;
		char __pad5[56];
		const char* name;
	};
	static_assert(sizeof(FxElemDef) == 0x168);
	static_assert(offsetof(FxElemDef, velSamples) == 200);
	static_assert(offsetof(FxElemDef, visuals) == 216);
	static_assert(offsetof(FxElemDef, effectOnImpact) == 248);
	static_assert(offsetof(FxElemDef, extended) == 288);
	static_assert(offsetof(FxElemDef, name) == 352);

	struct FxEffectDef
	{
		const char* name;
		char __pad0[12];
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		char __pad1[32];
		FxElemDef* elemDefs;
	};
	static_assert(sizeof(FxEffectDef) == 0x48);

	struct LightDef
	{
		const char* name;
		const char* attenuationName;
	};
	static_assert(sizeof(LightDef) == 0x10);

	struct FxParticleSimAnimation
	{
		const char* name;
		Material* material;
		char __pad0[8];
		unsigned int sampleCount;
		char __pad1[32];
		unsigned int stateCount;
		unsigned int frameCount;
		char __pad2[4];
		char* frames;
		char* samples;
		char* states;
	};
	static_assert(sizeof(FxParticleSimAnimation) == 0x60);
	static_assert(offsetof(FxParticleSimAnimation, stateCount) == 60);

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
		char __pad0[20];
		unsigned short boneCount;
		unsigned char notifyCount;
		char __pad1[5];
		unsigned int randomDataByteCount;
		unsigned int randomDataShortCount;
		unsigned int randomDataIntCount;
		unsigned int indexCount;
		char __pad2[12];
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
	static_assert(offsetof(XAnimParts, boneCount) == 36);
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

	struct GfxMapTrZoneData
	{
		vec3_t* positions;
		unsigned int* data0;
		unsigned int* data1;
		std::uint64_t* data2;
		std::uint64_t* data3;
		unsigned int* data4;
		unsigned int* data5;
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

	struct GfxMapTrZoneIndexBuffer
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
		unsigned int indexDataSize;
		char __pad1[4];
		GfxMapTrZoneIndexBuffer indices;
	};
	static_assert(sizeof(GfxMapTrZone) == 0x98);
	static_assert(offsetof(GfxMapTrZone, indexDataSize) == 128);
	static_assert(offsetof(GfxMapTrZone, indices) == 136);

	struct Mayhem
	{
		const char* name;
		char __pad0[0x48];
	};
	static_assert(sizeof(Mayhem) == 0x50);

	struct Spline;

	struct ScriptableEventModelDef
	{
		XModel* model;
		const char* name;
	};

	struct ScriptableEventFxDef
	{
		FxEffectDef* effect;
		scr_string_t tag;
	};

	struct ScriptableEventSoundDef
	{
		snd_alias_list_t* alias;
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

	struct ScriptableEventNamedSplineDef
	{
		char __pad0[40];
		const char* name;
		Spline* spline;
	};

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
		ScriptableEventFxDef fx;
		ScriptableEventSoundDef sound;
		ScriptableEventStringDef string;
		ScriptableEventSplineDef spline;
		ScriptableEventNamedSplineDef namedSpline;
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
		char __pad0[7];
	};
	static_assert(sizeof(ScriptableStateDef) == 0x18);

	struct ScriptablePartDef
	{
		ScriptableStateDef* states;
		scr_string_t name;
		unsigned char stateCount;
		char __pad0[3];
		unsigned char flagCount;
		char __pad1[7];
		unsigned char* flags;
	};
	static_assert(sizeof(ScriptablePartDef) == 0x20);
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
		char __pad1[4];
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
		char __pad0[72];
		FxEffectDef* fx[7];
	};
	static_assert(sizeof(PhysWaterPreset) == 0x88);

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

	struct ScriptableInstancePartState
	{
		char __pad0[72];
		scr_string_t name;
	};
	static_assert(sizeof(ScriptableInstancePartState) == 76);

	struct ScriptableInstance
	{
		ScriptableDef* def;
		unsigned char* flags;
		ScriptableInstancePartState* partStates;
		char __pad0[48];
		scr_string_t name;
		char __pad1[6];
		unsigned char partStateCount;
		char __pad2[5];
		char (*partRuntime)[4];
		unsigned char* eventStreamBuffer;
		char __pad3[67];
		unsigned char linkedObjectCount;
		char __pad4[4];
		unsigned char* modelTagRuntime;
		unsigned short* partStreams;
		unsigned short* linkedObjects;
	};
	static_assert(sizeof(ScriptableInstance) == 200);
	static_assert(offsetof(ScriptableInstance, name) == 72);
	static_assert(offsetof(ScriptableInstance, partRuntime) == 88);
	static_assert(offsetof(ScriptableInstance, partStateCount) == 82);
	static_assert(offsetof(ScriptableInstance, linkedObjectCount) == 171);

	struct ScriptableReservedDef
	{
		const char* name;
		void* runtime;
	};

	struct ScriptableMapEnts
	{
		unsigned int __pad0;
		unsigned int instanceCount;
		char __pad1[8];
		ScriptableInstance* instances;
		unsigned int reservedDefCount;
		char __pad2[4];
		ScriptableReservedDef* reservedDefs;
		char __pad3[8];
	};
	static_assert(sizeof(ScriptableMapEnts) == 0x30);

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

	struct dmFloat4
	{
		float x;
		float y;
		float z;
		float w;
	};

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
		char __pad0[12];
		scr_string_t name;
		char __pad1[8];
	};
	static_assert(sizeof(PhysWaterVolumeDef) == 0x20);

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
		bool unknown124[20];
	};
	static_assert(sizeof(WeaponAttachment) == 0x90);
	static_assert(offsetof(WeaponAttachment, fieldCount) == 104);
	static_assert(offsetof(WeaponAttachment, knifeAttachTagOverride) == 120);

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
		int unknown0;
		int unknown4;
		float minVelocity;
		float unknown12;
		snd_alias_list_t* verticalSound;
		snd_alias_list_t* verticalStopSound;
		snd_alias_list_t* horizontalSound;
		snd_alias_list_t* horizontalStopSound;
		snd_alias_list_t* movementSounds[2];
		float unknown64;
		float unknown68;
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
		int unknown24;
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
		int unknown72[5];
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
		int unknown152[5];
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
		int unknown300[5];
	};
	static_assert(sizeof(StateTimers) == 320);
	static_assert(offsetof(StateTimers, bHoldFullPrime) == 268);
	static_assert(offsetof(StateTimers, slideInTime) == 288);

	struct WeaponCompleteDef
	{
		const char* szInternalName;
		const char* szDisplayName;
		XModel** gunModel;
		XModel** unknown24;
		XModel* handModel;
		const char* lobWorldModelName;
		XAnimParts** szXAnimsRightHanded;
		XAnimParts** szXAnimsLeftHanded;
		scr_string_t* hideTags;
		WeaponAttachment** attachments;
		unsigned char unknown80[8];
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
		snd_alias_list_t* unknownSound800;
		snd_alias_list_t* unknownSound808;
		snd_alias_list_t* unknownSound816;
		snd_alias_list_t** bounceSound;
		snd_alias_list_t** rollingSound;
		FxEffectDef* viewShellEjectEffect;
		FxEffectDef* worldShellEjectEffect;
		FxEffectDef* viewLastShotEjectEffect;
		FxEffectDef* worldLastShotEjectEffect;
		FxEffectDef* viewMagEjectEffect;
		Material* reticleCenter;
		Material* unknownMaterial888;
		Material* reticleSide;
		XModel** worldModel;
		XModel** unknown912;
		XModel* worldClipModel;
		XModel* rocketModel;
		Material* hudIcon;
		Material* pickupIcon;
		Material* unknownMaterial952;
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
		int unknown1308;
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
		int ammoCounterIconRatio;
		int ammoCounterClip;
		int unknown1488[9];
		int maxAmmo;
		int unknown1528;
		int minAmmoReq;
		int startAmmo;
		int unknown1540;
		int clipSize;
		int shotCount;
		int sharedAmmoCapIndex;
		int sharedAmmoCap;
		int unknown1560; // 95 in all stock weapons, the game reads the damage at 3572
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
		float unknown2252[2];
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
		float unknown2612[6];
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
		int unknown2760[2];
		float damageConeAngle;
		int unknown2772[3];
		int projectileSpeed;
		int projectileSpeedUp;
		int projectileSpeedForward;
		int unknown2796;
		int projectileActivateDist;
		float projLifetime;
		float timeToAccelerate;
		float projectileCurvature;
		float unknown2816[8];
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
		float unknown2996[9];
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
		int unknown3136;
		float unknown3140[16];
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
		int unknown3304;
		float unknown3308[19];
		int positionReloadTransTime;
		int unknown3388;
		float unknown3392[2];
		const char* accuracyGraphName[2];
		vec2_t* accuracyGraphKnots[2];
		vec2_t* originalAccuracyGraphKnots[2];
		unsigned short accuracyGraphKnotCount[2];
		unsigned short originalAccuracyGraphKnotCount[2];
		float unknown3456[26];
		const char* szScript;
		unsigned char unknown3568[4];
		int damage;
		float unknown3576;
		int unknown3580;
		float unknown3584;
		int unknown3588;
		float unknown3592;
		int unknown3596;
		float unknown3600;
		int unknown3604;
		int playerDamage;
		float maxDamageRange;
		int minDamage;
		int minPlayerDamage;
		float unknown3624;
		int unknown3628[2];
		int midDamage;
		int midPlayerDamage;
		float minDamageRange;
		float unknown3648;
		int unknown3652[9];
		int signatureDamage;
		int signatureMinDamage;
		int signatureMidDamage;
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
		float unknown3824[17];
		int unknown3892[9];
		float unknown3928[4];
		scr_string_t stowTag;
		scr_string_t unknownTag3948;
		unsigned char unknown3952;
		bool unknown3953[103];
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
		unsigned char type; // 0
		unsigned char canUseShadowMap; // 1
		char __pad0[10]; // 2
		float color[3]; // 12
		float intensity; // 24
		float dir[3]; // 28
		float up[3]; // 40
		float origin[3]; // 52
		float fadeOffset[2]; // 64
		float bulbRadius; // 72
		float bulbLength[3]; // 76
		float radius; // 88
		float cosHalfFovOuter; // 92
		float cosHalfFovInner; // 96
		float cosHalfFovExpanded; // 100
		float rotationLimit; // 104
		float translationLimit; // 108
		char __pad1[44]; // 112
		int unk_0x9C; // 156
		const char* defName; // 160
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

	struct GlassDef
	{
		const char* name;
		scr_string_t targetName;
		unsigned short indexCount;
		unsigned short* indices;
	};
	static_assert(sizeof(GlassDef) == 0x18);

	struct G_GlassData
	{
		char (*glassPieces)[12];
		unsigned int pieceCount;
		char __pad0[0x4];
		unsigned int glassNameCount;
		char __pad1[0x4];
		GlassDef* glassNames;
		char __pad2[0x70];
	};
	static_assert(sizeof(G_GlassData) == 0x90);

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
		char __pad0[0x28];
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
		char __pad1[0x10];
	};
	static_assert(sizeof(FxGlassDef) == 0x88);

	struct FxGlassSystem
	{
		char __pad0[0x8];
		unsigned int defCount;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int cellCount;
		char __pad1[0x8];
		unsigned int geoDataLimit;
		char __pad2[0x4];
		unsigned int initGeoDataCount;
		char __pad3[0x4];
		FxGlassDef* defs;
		char (*piecePlaces)[32];
		char (*pieceStates)[32];
		char (*pieceDynamics)[48];
		char (*geoData)[4];
		unsigned int* isInUse;
		unsigned int* cellBits;
		unsigned char* visData;
		char (*linkOrg)[12];
		float* halfThickness;
		unsigned short* lightingHandles;
		char (*initGeoData)[4];
		char __pad4[0xC];
		unsigned int initPieceCount;
		char (*initPieceStates)[56];
	};
	static_assert(sizeof(FxGlassSystem) == 0xA8);

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
		int skySurfCount; // 0
		int* skyStartSurfs; // 8
		GfxImage* skyImage; // 16
		unsigned char skySamplerState; // 24
		Bounds bounds; // 28
	};
	assert_sizeof(GfxSky, 56);
	assert_offsetof(GfxSky, skyStartSurfs, 8);
	assert_offsetof(GfxSky, skyImage, 16);

	struct GfxWorldDpvsPlanes
	{
		unsigned int cellCount; // 0
		cplane_s* planes; // 8
		unsigned int* nodes; // 16
		unsigned int* sceneEntCellBits; // 24
		unsigned int* unk_0x20; // 32
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
		Bounds bounds; // 0
		int childrenOffset; // 24
		unsigned short childCount; // 28
		unsigned short smodelIndexCount; // 30
		unsigned short* smodelIndexes; // 32
		int startSurfIndex; // 40
		unsigned short surfaceCount; // 44
		unsigned short pad; // 46
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
		unsigned short indexCount; // 0
		unsigned short* indices; // 8
		char unk_0x10[8]; // 16
	};
	assert_sizeof(GfxCellTransientInfo, 24);
	assert_offsetof(GfxCellTransientInfo, indices, 8);

	struct GfxPortal;

	struct GfxPortalWritable
	{
		bool isQueued; // 0
		bool isAncestor; // 1
		unsigned char recursionDepth; // 2
		unsigned char hullPointCount; // 3
		float (*hullPoints)[2]; // 8
		GfxPortal* queuedParent; // 16
	};
	assert_sizeof(GfxPortalWritable, 24);

	struct DpvsPlane
	{
		float coeffs[4];
	};

	struct GfxPortal
	{
		GfxPortalWritable writable; // 0
		DpvsPlane plane; // 24
		float (*vertices)[3]; // 40
		unsigned short cellIndex; // 48
		unsigned short closeDistance; // 50
		unsigned char vertexCount; // 52
		float hullAxis[2][3]; // 56
	};
	assert_sizeof(GfxPortal, 80);
	assert_offsetof(GfxPortal, vertices, 40);
	assert_offsetof(GfxPortal, vertexCount, 52);

	struct GfxCell
	{
		Bounds bounds; // 0
		unsigned short portalCount; // 24
		unsigned char reflectionProbeCount; // 26
		unsigned char reflectionProbeReferenceCount; // 27
		GfxPortal* portals; // 32
		unsigned char* reflectionProbes; // 40
		unsigned char* reflectionProbeReferences; // 48
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
		const char* targetName; // 0
		GfxPortalGroupInfo* gfxPortalArray; // 8
		scr_string_t targetNameIndex; // 16
		unsigned short numPortals; // 20
	};
	assert_sizeof(GfxPortalGroup, 24);
	assert_offsetof(GfxPortalGroup, gfxPortalArray, 8);
	assert_offsetof(GfxPortalGroup, numPortals, 20);

	struct GfxReflectionProbeVolume
	{
		unsigned short* probeVolumePlanes; // 0
		unsigned int probeVolumePlaneCount; // 8
	};
	assert_sizeof(GfxReflectionProbeVolume, 16);

	struct GfxReflectionProbe
	{
		float origin[3]; // 0
		GfxReflectionProbeVolume* probeVolumes; // 16
		unsigned int probeVolumeCount; // 24
		unsigned int lightingState; // 28
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
		unsigned int materialCount; // 0
		Material** materials; // 8
	};
	assert_sizeof(GfxTransientZoneMaterials, 16);

	struct GfxWorldDraw
	{
		unsigned int reflectionProbeCount; // 0
		GfxImage** reflectionProbes; // 8
		GfxReflectionProbe* reflectionProbeOrigins; // 16
		GfxTexture* reflectionProbeTextures; // 24
		unsigned int reflectionProbeReferenceCount; // 32
		GfxReflectionProbeReferenceOrigin* reflectionProbeReferenceOrigins; // 40
		GfxReflectionProbeReference* reflectionProbeReferences; // 48
		unsigned int fogSplineCount; // 56
		GfxImage** fogSplineImages; // 64
		char (*fogSplineInfo)[116]; // 72
		GfxImage* iesLookup; // 80
		char unk_0x58[68]; // 88
		unsigned int lightmapOverrideCount; // 156
		GfxLightmapArray* lightmapOverrides; // 160
		GfxTexture* lightmapPrimaryTextures; // 168
		GfxTexture* lightmapSecondaryTextures; // 176
		GfxImage* lightmapOverridePrimary; // 184
		GfxImage* lightmapOverrideSecondary; // 192
		unsigned int imageCount; // 200
		GfxImage** images; // 208
		GfxLightmapParameters lightmapParameters; // 216
		char unk_0xEC[396]; // 236
		unsigned int transientZoneCount; // 632
		GfxMapTrZone* transientZones[16]; // 640
		GfxTransientZoneMaterials* transientZoneMaterials; // 768
		unsigned int indexCount; // 776
		unsigned short* indices; // 784
		ID3D11Buffer* indexBuffer; // 792
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
	assert_offsetof(GfxWorldDraw, lightmapParameters, 216);
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
		unsigned char lightingState; // 0
		unsigned char maxDepth; // 1
		unsigned char pad[2]; // 2
		int nodeCount; // 4
		int leafCount; // 8
		int coordMinGridSpace[3]; // 12
		int coordMaxGridSpace[3]; // 24
		int coordHalfSizeGridSpace[3]; // 36
		int defaultColorIndexBitCount; // 48
		int defaultLightIndexBitCount; // 52
		unsigned int* p_nodeTable; // 56
		int leafTableSize; // 64
		unsigned char* p_leafTable; // 72
	};
	assert_sizeof(GfxLightGridTree, 80);
	assert_offsetof(GfxLightGridTree, nodeCount, 4);
	assert_offsetof(GfxLightGridTree, p_nodeTable, 56);
	assert_offsetof(GfxLightGridTree, leafTableSize, 64);
	assert_offsetof(GfxLightGridTree, p_leafTable, 72);

	struct GfxLightGrid
	{
		bool hasLightRegions; // 0
		bool useSkyForLowZ; // 1
		unsigned int lastSunPrimaryLightIndex; // 4
		unsigned short mins[3]; // 8
		unsigned short maxs[3]; // 14
		unsigned int rowAxis; // 20
		unsigned int colAxis; // 24
		unsigned short* rowDataStart; // 32
		unsigned int rawRowDataSize; // 40
		unsigned char* rawRowData; // 48
		unsigned int entryCount; // 56
		GfxLightGridEntry* entries; // 64
		unsigned int colorCount; // 72
		GfxLightGridColors* colors; // 80
		GfxLightGridEntry cachedSkyLightGridEntry; // 88
		float cachedSkyLightEntryAverageColor[3]; // 96
		unsigned int missingGridColorIndex; // 108
		int tableVersion; // 112
		int paletteVersion; // 116
		char rangeExponent8BitsEncoding; // 120
		char rangeExponent12BitsEncoding; // 121
		char rangeExponent16BitsEncoding; // 122
		unsigned char stageCount; // 123
		float* stageLightingContrastGain; // 128
		unsigned int paletteEntryCount; // 136
		int* paletteEntryAddress; // 144
		unsigned int paletteBitstreamSize; // 152
		unsigned char* paletteBitstream; // 160
		GfxLightGridColors skyLightGridColors; // 168
		GfxLightGridColors defaultLightGridColors; // 504
		GfxLightGridTree tree[3]; // 840
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
		unsigned int probeCount; // 0
		GfxSHProbeData* probes; // 8
		ID3D11Buffer* probesBuffer; // 16
		ID3D11ShaderResourceView* probesView; // 24
		ID3D11UnorderedAccessView* probesRWView; // 32
		GfxGpuLightGridProbePosition* probePositions; // 40
		ID3D11Buffer* probePositionsBuffer; // 48
		ID3D11ShaderResourceView* probePositionsView; // 56
		unsigned int zoneCount; // 64
		GfxGpuLightGridZone* zones; // 72
		unsigned int tetrahedronCount; // 80
		unsigned int tetrahedronCountVisible; // 84
		GfxGpuLightGridTetrahedron* tetrahedrons; // 88
		ID3D11Buffer* tetrahedronBuffer; // 96
		ID3D11ShaderResourceView* tetrahedronView; // 104
		GfxGpuLightGridTetrahedronNeighbors* tetrahedronNeighbors; // 112
		ID3D11Buffer* tetrahedronNeighborsBuffer; // 120
		ID3D11ShaderResourceView* tetrahedronNeighborsView; // 128
		GfxGpuLightGridTetrahedronVisibility* tetrahedronVisibility; // 136
		ID3D11Buffer* tetrahedronVisibilityBuffer; // 144
		ID3D11ShaderResourceView* tetrahedronVisibilityView; // 152
		unsigned int voxelStartTetrahedronCount; // 160
		GfxGpuLightGridVoxelStartTetrahedron* voxelStartTetrahedron; // 168
		ID3D11Buffer* voxelStartTetrahedronBuffer; // 176
		ID3D11ShaderResourceView* voxelStartTetrahedronView; // 184
		unsigned int smodelProbeGridCount; // 192
		GfxStaticModelProbeGrid* smodelProbeGrids; // 200
		unsigned int smodelProbeCount; // 208
		GfxSHProbeData* smodelProbes; // 216
		ID3D11Buffer* smodelProbesBuffer; // 224
		ID3D11ShaderResourceView* smodelProbesView; // 232
		ID3D11UnorderedAccessView* smodelProbesRWView; // 240
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
		unsigned int count; // 0
		char (*data)[Stride]; // 8
	};
	assert_sizeof(GfxWorldUnknownArray<32>, 16);

	struct GfxWorldUnknownBufferGroup
	{
		unsigned int unk_0x0; // 0
		unsigned int count; // 4
		char (*data)[64]; // 8
		ID3D11Buffer* buffer; // 16
		ID3D11ShaderResourceView* view; // 24
		unsigned int countA; // 32
		unsigned int countB; // 36
		char (*dataA)[64]; // 40
		ID3D11Buffer* bufferA; // 48
		ID3D11ShaderResourceView* viewA; // 56
		unsigned int* dataB; // 64
		ID3D11Buffer* bufferB; // 72
		ID3D11ShaderResourceView* viewB; // 80
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
		GfxWorldUnknownArray<32> array0; // 0
		GfxWorldUnknownArray<28> array1; // 16
		GfxWorldUnknownArray<16> array2; // 32
		GfxWorldUnknownArray<20> array3; // 48
		GfxWorldUnknownArray<16> array4; // 64
		GfxWorldUnknownArray<16> array5; // 80
		GfxWorldUnknownArray<16> array6; // 96
		GfxWorldUnknownArray<16> array7; // 112
		GfxWorldUnknownArray<16> array8; // 128
		GfxWorldUnknownArray<16> array9; // 144
		char unk_0xA0[8]; // 160
		GfxImage* image0; // 168
		unsigned int image0DataCount; // 176
		unsigned int* image0Data; // 184
		char unk_0xC0[8]; // 192
		GfxImage* image1; // 200
		unsigned int image1DataCount; // 208
		unsigned short* image1Data; // 216
		char unk_0xE0[8]; // 224
		GfxWorldUnknownBufferGroup groups[3]; // 232
	};
	assert_sizeof(GfxWorldUnknownData, 496);
	assert_offsetof(GfxWorldUnknownData, image0, 168);
	assert_offsetof(GfxWorldUnknownData, image0DataCount, 176);
	assert_offsetof(GfxWorldUnknownData, image0Data, 184);
	assert_offsetof(GfxWorldUnknownData, image1, 200);
	assert_offsetof(GfxWorldUnknownData, image1DataCount, 208);
	assert_offsetof(GfxWorldUnknownData, image1Data, 216);
	assert_offsetof(GfxWorldUnknownData, groups, 232);

	struct GfxFrustumLights
	{
		Bounds bounds; // 0
		unsigned int vertexCount; // 24
		unsigned int indexCount; // 28
		unsigned short* indices; // 32
		char (*vertices)[32]; // 40
		ID3D11Buffer* indexBuffer; // 48
		ID3D11Buffer* vertexBuffer; // 56
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
		Bounds zoneBound; // 0
		unsigned int voxelTopDownViewNodeCount; // 24
		unsigned int voxelInternalNodeCount; // 28
		unsigned int voxelLeafNodeCount; // 32
		unsigned int lightListArraySize; // 36
		GfxVoxelTreeHeader* voxelTreeHeader; // 40
		GfxVoxelTopDownViewNode* voxelTopDownViewNodeArray; // 48
		GfxVoxelInternalNode* voxelInternalNodeArray; // 56
		GfxVoxelLeafNode* voxelLeafNodeArray; // 64
		unsigned short* lightListArray; // 72
		unsigned int (*voxelInternalNodeDynamicLightList)[2]; // 80
		char unk_0x58[24]; // 88
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
		unsigned int surfaceListOffsetCount; // 0
		unsigned int* surfaceListOffsets; // 8
		unsigned int smodelListOffsetCount; // 16
		unsigned int* smodelListOffsets; // 24
		unsigned int listsSize; // 32
		unsigned short* lists; // 40
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
		unsigned short surfaceCount; // 0
		unsigned short smodelCount; // 2
		unsigned int* sortedSurfIndex; // 8
		unsigned short* smodelIndex; // 16
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
		float kdopMidPoint[9]; // 0
		float kdopHalfSize[9]; // 36
		unsigned int axisCount; // 72
		GfxLightRegionAxis* axis; // 80
	};
	assert_sizeof(GfxLightRegionHull, 88);
	assert_offsetof(GfxLightRegionHull, axisCount, 72);
	assert_offsetof(GfxLightRegionHull, axis, 80);

	struct GfxLightRegion
	{
		unsigned int hullCount; // 0
		GfxLightRegionHull* hulls; // 8
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
		GfxPackedPlacement placement; // 0
		float unk_0x34[3]; // 52
		char unk_0x40[8]; // 64
		XModel* model; // 72
		char unk_0x50[2]; // 80
		unsigned short cullDist; // 82
		unsigned short flags; // 84
		char unk_0x56[2]; // 86
		unsigned short staticModelId; // 88
		char unk_0x5A[2]; // 90
		unsigned int unk_0x5C; // 92
		char unk_0x60[10]; // 96
		unsigned short lightListOffset; // 106
		float radius; // 108
		unsigned char reflectionProbeIndex; // 112
		char unk_0x71[1]; // 113
		unsigned char unk_0x72; // 114
		char unk_0x73[1]; // 115
		float unk_0x74; // 116
		char unk_0x78[8]; // 120
	};
	assert_sizeof(GfxStaticModelDrawInst, 128);
	assert_offsetof(GfxStaticModelDrawInst, unk_0x34, 52);
	assert_offsetof(GfxStaticModelDrawInst, model, 72);
	assert_offsetof(GfxStaticModelDrawInst, cullDist, 82);
	assert_offsetof(GfxStaticModelDrawInst, flags, 84);
	assert_offsetof(GfxStaticModelDrawInst, staticModelId, 88);
	assert_offsetof(GfxStaticModelDrawInst, unk_0x5C, 92);
	assert_offsetof(GfxStaticModelDrawInst, lightListOffset, 106);
	assert_offsetof(GfxStaticModelDrawInst, radius, 108);
	assert_offsetof(GfxStaticModelDrawInst, reflectionProbeIndex, 112);
	assert_offsetof(GfxStaticModelDrawInst, unk_0x72, 114);
	assert_offsetof(GfxStaticModelDrawInst, unk_0x74, 116);

	struct GfxStaticModelVertexLighting
	{
		unsigned char visibility[4];
		unsigned short ambientColorFloat16[4];
		unsigned short highlightColorFloat16[4];
	};
	assert_sizeof(GfxStaticModelVertexLighting, 20);

	struct GfxStaticModelVertexLightingInfo
	{
		GfxStaticModelVertexLighting* lightingValues; // 0
		ID3D11Buffer* lightingValuesVb; // 8
		unsigned int numLightingValues; // 16
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
		char unk_0x0[16]; // 0
		unsigned int dataSize; // 16
		char unk_0x14[44]; // 20
		char* data; // 64
		char unk_0x48[184]; // 72
	};
	assert_sizeof(GfxSurfaceBuffer, 0x100);
	assert_offsetof(GfxSurfaceBuffer, dataSize, 16);
	assert_offsetof(GfxSurfaceBuffer, data, 64);

	enum GfxSurfaceFlag : std::uint8_t
	{
		GFX_SURFACE_FLAG_DEFAULT = 0x01,
		GFX_SURFACE_FLAG_CASTS_SUN_SHADOW = 0x10,
	};

	struct GfxSurface
	{
		unsigned int vertexLayerData; // 0
		unsigned int firstVertex; // 4
		unsigned short vertexCount; // 8
		unsigned short triCount; // 10
		char unk_0xC[4]; // 12
		GfxSurfaceBuffer* buffer; // 16
		unsigned int baseIndex; // 24
		char unk_0x1C[4]; // 28
		Material* material; // 32
		unsigned char lightmapIndex; // 40
		unsigned char reflectionProbeIndex; // 41
		unsigned char flags; // 42
		char unk_0x2B[1]; // 43
		unsigned short lightListOffset; // 44
		char unk_0x2E[2]; // 46
		unsigned int transientZone; // 48
		char unk_0x34[4]; // 52
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
		Bounds bounds; // 0
		float midPoint[3]; // 24
		char unk_0x24[12]; // 36
		float unk_0x30; // 48
		unsigned char unk_0x34[5]; // 52
		char unk_0x39[7]; // 57
	};
	assert_sizeof(GfxSurfaceBounds, 64);
	assert_offsetof(GfxSurfaceBounds, midPoint, 24);
	assert_offsetof(GfxSurfaceBounds, unk_0x30, 48);
	assert_offsetof(GfxSurfaceBounds, unk_0x34, 52);

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount; // 0
		unsigned int staticSurfaceCount; // 4
		unsigned int surfaceRangeBegins[8]; // 8
		unsigned int surfaceRangeEnds[8]; // 40
		unsigned int smodelVisDataCount; // 72
		unsigned int surfaceVisDataCount; // 76
		unsigned int primaryLightVisDataCount; // 80
		char unk_0x54[4]; // 84
		unsigned int* smodelVisData[4]; // 88
		unsigned int* smodelVisDataUnused[8]; // 120
		unsigned int* smodelUnknownVisData[27]; // 184
		unsigned int* surfaceVisData[4]; // 400
		unsigned int* surfaceVisDataUnused[8]; // 432
		unsigned int* surfaceUnknownVisData[27]; // 496
		unsigned int* unk_0x2C8; // 712
		unsigned int* primaryLightVisData; // 720
		unsigned int* unk_0x2D8; // 728
		unsigned int* unk_0x2E0; // 736
		unsigned int* sortedSurfIndex; // 744
		GfxSurface* surfaces; // 752
		GfxSurfaceBounds* surfacesBounds; // 760
		unsigned long long* unk_0x300; // 768
		GfxStaticModelInst* smodelInsts; // 776
		GfxStaticModelDrawInst* smodelDrawInsts; // 784
		unsigned int* unk_0x318; // 792
		GfxStaticModelLighting* smodelLightingInsts; // 800
		unsigned int* unk_0x328; // 808
		unsigned short* sortedSmodelIndices; // 816
		unsigned int sunShadowOptCount; // 824
		unsigned int sunSurfVisDataCount; // 828
		unsigned int* surfaceCastsSunShadowOpt; // 832
		unsigned long long* surfaceMaterials; // 840
		unsigned long long* unk_0x350; // 848
		char unk_0x358[8]; // 856
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
	assert_offsetof(GfxWorldDpvsStatic, unk_0x300, 768);
	assert_offsetof(GfxWorldDpvsStatic, smodelInsts, 776);
	assert_offsetof(GfxWorldDpvsStatic, smodelDrawInsts, 784);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x318, 792);
	assert_offsetof(GfxWorldDpvsStatic, smodelLightingInsts, 800);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x328, 808);
	assert_offsetof(GfxWorldDpvsStatic, sortedSmodelIndices, 816);
	assert_offsetof(GfxWorldDpvsStatic, sunShadowOptCount, 824);
	assert_offsetof(GfxWorldDpvsStatic, sunSurfVisDataCount, 828);
	assert_offsetof(GfxWorldDpvsStatic, surfaceCastsSunShadowOpt, 832);
	assert_offsetof(GfxWorldDpvsStatic, surfaceMaterials, 840);
	assert_offsetof(GfxWorldDpvsStatic, unk_0x350, 848);

	struct GfxWorldDpvsDynamic
	{
		unsigned int dynEntClientWordCount[2]; // 0
		unsigned int dynEntClientCount[2]; // 8
		unsigned int* dynEntCellBits[2]; // 16
		unsigned char* dynEntVisData[2][4]; // 32
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
		ExtentBounds bounds; // 0
		unsigned short cellCount[3]; // 24
		unsigned short parentBoneIndex; // 30
		GfxImage* volumeData; // 32
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

	struct GfxWorld
	{
		const char* name; // 0
		const char* baseName; // 8
		unsigned int bspVersion; // 16
		int planeCount; // 20
		int nodeCount; // 24
		unsigned int surfaceCount; // 28
		int skyCount; // 32
		GfxSky* skies; // 40
		unsigned int portalGroupCount; // 48
		unsigned int lastSunPrimaryLightIndex; // 52
		unsigned int primaryLightCount; // 56
		unsigned int primaryLightEnvCount; // 60
		unsigned int sortKeyLitDecal; // 64
		unsigned int sortKeyEffectDecal; // 68
		unsigned int sortKeyTopDecal; // 72
		unsigned int sortKeyEffectAuto; // 76
		unsigned int sortKeyDistortion; // 80
		unsigned int sortKeyHair; // 84
		GfxWorldDpvsPlanes dpvsPlanes; // 88
		GfxCellTreeCount* aabbTreeCounts; // 128
		GfxCellTree* aabbTrees; // 136
		GfxCellTransientInfo* cellTransientInfos; // 144
		GfxCell* cells; // 152
		GfxPortalGroup* portalGroup; // 160
		unsigned int portalDistanceAnchorCount; // 168
		vec4_t* portalDistanceAnchorsAndCloseDistSquared; // 176
		char unk_0xB8[8]; // 184
		GfxWorldDraw draw; // 192
		GfxLightGrid lightGrid; // 992
		GfxLightGridProbeData probeData; // 2072
		GfxWorldUnknownData unknownData; // 2320
		GfxFrustumLights* frustumLights; // 2816
		int voxelTreeCount; // 2824
		GfxVoxelTree* voxelTree; // 2832
		GfxWorldLightLists lightLists; // 2840
		int modelCount; // 2888
		GfxBrushModel* models; // 2896
		Bounds bounds; // 2904
		Bounds shadowBounds; // 2928
		unsigned int checksum; // 2952
		int materialMemoryCount; // 2956
		MaterialMemory* materialMemory; // 2960
		sunflare_t sun; // 2968
		float outdoorLookupMatrix[4][4]; // 3080
		GfxImage* outdoorImage; // 3144
		unsigned int* cellCasterBits; // 3152
		unsigned int* cellHasSunLitSurfsBits; // 3160
		char (*sceneDynModel)[12]; // 3168
		char (*sceneDynBrush)[8]; // 3176
		unsigned int* primaryLightEntityShadowVis; // 3184
		unsigned int* primaryLightDynEntShadowVis[2]; // 3192
		unsigned short* nonSunPrimaryLightForModelDynEnt; // 3208
		GfxShadowGeometry* shadowGeom; // 3216
		GfxShadowGeometry* shadowGeomOptimized; // 3224
		GfxLightRegion* lightRegion; // 3232
		GfxWorldDpvsStatic dpvs; // 3240
		GfxWorldDpvsDynamic dpvsDyn; // 4104
		unsigned int mapVtxChecksum; // 4200
		unsigned int heroOnlyLightCount; // 4204
		GfxHeroOnlyLight* heroOnlyLights; // 4208
		unsigned char fogTypesAllowed; // 4216
		unsigned int mdaoVolumesCount; // 4220
		MdaoVolume* mdaoVolumes; // 4224
		bool useLightGridDefaultModelLightingLookup; // 4232
		bool useLightGridDefaultFXLightingLookup; // 4233
		float lightGridDefaultModelLightingLookup[3]; // 4236
		float lightGridDefaultFXLightingLookup[3]; // 4248
		GfxBuildInfo buildInfo; // 4264
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
	assert_offsetof(GfxWorld, primaryLightEnvCount, 60);
	assert_offsetof(GfxWorld, sortKeyLitDecal, 64);
	assert_offsetof(GfxWorld, sortKeyHair, 84);
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
	assert_offsetof(GfxWorld, useLightGridDefaultModelLightingLookup, 4232);
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
