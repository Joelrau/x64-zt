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
		unsigned char stableArgCount;
		unsigned char perViewArgCount;
		char __pad0[9];
		unsigned short zone;
		char __pad1[24];
		MaterialShaderArgument* args;
	};
	static_assert(sizeof(MaterialPass) == 0x58);
	static_assert(offsetof(MaterialPass, zone) == 54);

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

	struct PicMip
	{
		unsigned char platform[2];
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
		TS_RESIDENT = 0xC,
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
		PicMip picmip;
	};
	static_assert(sizeof(GfxImage) == 0x60);
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
		char __pad0[8];
		const char* name;
		const char* fileName;
		char __pad1[16];
	};
	static_assert(sizeof(LoadedSoundStreamInfo) == 0x28);

	struct LoadedSound
	{
		const char* name;
		LoadedSoundStreamInfo info;
		char __pad0[16];
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
		char __pad0[16];
	};
	static_assert(sizeof(StreamedSound) == 0x38);

	struct PrimedSound
	{
		LoadedSound* loadedPart;
		LoadedSoundStreamInfo streamedPart;
		char __pad0[8];
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
		char __pad0[7];
		SoundFileRef u;
	};
	static_assert(sizeof(SoundFile) == 0x40);

	struct SndAliasDuck
	{
		const char* name;
		char __pad0[0xC0];
	};
	static_assert(sizeof(SndAliasDuck) == 0xC8);

	struct snd_alias_t
	{
		const char* aliasName;
		const char* subtitle;
		const char* secondaryAliasName;
		const char* chainAliasName;
		const char* mixerGroup;
		SoundFile* soundFile;
		const char* squelchName;
		char __pad0[80];
		SndContext* sndContext;
		char __pad1[16];
		SndCurve* volumeFalloffCurve;
		DistCurve* distCurve;
		SndCurve* reverbSendCurve;
		SndAliasDuck* duck;
		char __pad2[56];
		SndAliasParameterModifier* parameterModifiers[8];
		char __pad3[16];
		SndAliasCombatCone* combatCone;
	};
	static_assert(sizeof(snd_alias_t) == 0x150);
	static_assert(offsetof(snd_alias_t, sndContext) == 136);
	static_assert(offsetof(snd_alias_t, duck) == 184);
	static_assert(offsetof(snd_alias_t, parameterModifiers) == 248);
	static_assert(offsetof(snd_alias_t, combatCone) == 328);

	struct snd_alias_list_t
	{
		const char* name;
		snd_alias_t* head;
		unsigned short* sortedIndices;
		unsigned char count;
		unsigned char sortedIndexCount;
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
		unsigned char depthStencilState[10];
		unsigned char blendState;
		unsigned char rasterizerState;
		char __pad0[2];
	};
	static_assert(sizeof(GfxStateBits) == 0x28);

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

	struct XSurface
	{
		unsigned short flags;
		unsigned short vertCount;
		unsigned short triCount;
		unsigned char rigidVertListCount;
		char __pad0[9];
		XSurfaceShared* shared;
		unsigned short* triIndices;
		XRigidVertList* rigidVertLists;
		vec4_t* verts;
		unsigned int partBits[8];
		char __pad1[24];
	};
	static_assert(sizeof(XSurface) == 0x68);
	static_assert(offsetof(XSurface, shared) == 16);

	struct XModelSurfs
	{
		const char* name;
		XSurface* surfs;
		unsigned short numsurfs;
		char __pad0[2];
		unsigned int partBits[8];
		char __pad1[28];
	};
	static_assert(sizeof(XModelSurfs) == 0x50);

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
		char __pad4[54];
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
		char __pad0[6];
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

	struct Mayhem;
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

	struct DLogHashTable
	{
		unsigned int* list;
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

	struct ClipMapTrZoneNode
	{
		unsigned int* data;
		char __pad0[16];
	};

	struct ClipMapTrZoneTree
	{
		std::uint64_t* nodeRefs;
		unsigned int nodeRefCount;
		unsigned int nodeCount;
		ClipMapTrZoneNode* nodes;
		char (*bounds)[24];
		std::uint64_t* nodeData;
	};
	static_assert(sizeof(ClipMapTrZoneTree) == 0x28);

	struct ClipMapTrZoneMesh
	{
		unsigned int vertCount;
		char __pad0[4];
		vec3_t* verts;
		unsigned int triCount;
		char __pad1[4];
		unsigned short* triIndices;
		unsigned char* triEdgeIsWalkable;
		unsigned int partitionCount;
		char __pad2[4];
		char (*partitions)[28];
		unsigned int aabbCount;
		char __pad3[4];
		char (*aabbs)[16];
	};
	static_assert(sizeof(ClipMapTrZoneMesh) == 0x48);

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
		char __pad0[8];
		ClipMapTrZoneTree tree;
		ClipMapTrZoneMesh mesh;
		ClipMapTrZoneStaticModels staticModels;
	};
	static_assert(sizeof(ClipMapTrZone) == 0x90);
	static_assert(offsetof(ClipMapTrZone, staticModels) == 128);

	struct TriggerModel
	{
		int contents;
		unsigned short hullCount;
		unsigned short firstHull;
		char __pad0[8];
	};
	static_assert(sizeof(TriggerModel) == 16);

	struct TriggerHull
	{
		char __pad0[40];
	};

	struct TriggerSlab
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

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
		void* partRuntime;
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

	struct PhysMeshData
	{
		char (*vertices)[32];
		char (*normals)[16];
		char (*triangles)[32];
		int vertexCount;
		int normalCount;
		int triangleCount;
		char __pad0[12];
	};
	static_assert(sizeof(PhysMeshData) == 0x30);

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
		std::uint64_t* models;
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
		char __pad0[6];
		AttachmentFieldValue value;
	};
	static_assert(sizeof(AttachmentField) == 0x10);

	struct WeaponAttachment
	{
		const char* szInternalName;
		const char* szDisplayName;
		char __pad0[24];
		XModel** worldModels;
		XModel** viewModels;
		XModel** worldModelsAdditional;
		XModel** viewModelsAdditional;
		XModel** reticleViewModels;
		char (*unkStruct80)[72];
		unsigned short* fieldOffsets;
		AttachmentField* fields;
		int fieldCount;
		char __pad1[12];
		scr_string_t knifeAttachTagOverride;
		char __pad2[20];
	};
	static_assert(sizeof(WeaponAttachment) == 0x90);
	static_assert(offsetof(WeaponAttachment, fieldCount) == 104);
	static_assert(offsetof(WeaponAttachment, knifeAttachTagOverride) == 120);

	struct WeaponCompleteDef
	{
		const char* szInternalName;
		const char* szDisplayName;
		char __pad0[0x1080];
	};
	static_assert(sizeof(WeaponCompleteDef) == 0x1090);

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
		char __pad0[0x10];
	};
	static_assert(sizeof(ClipMaterial) == 0x18);

	struct cbrushside_t
	{
		char __pad0[0x4];
		int indexCount;
		char __pad1[0x8];
		int* indices;
		char __pad2[0x8];
	};
	static_assert(sizeof(cbrushside_t) == 0x20);

	struct ClipBrushInfo
	{
		unsigned int sideCount;
		cbrushside_t* sides;
		unsigned int indexCount;
		int* indices;
	};
	static_assert(sizeof(ClipBrushInfo) == 0x20);

	struct ClipBrushBounds
	{
		int count;
		char __pad0[0x4];
		char (*data)[32];
	};
	static_assert(sizeof(ClipBrushBounds) == 0x10);

	struct ClipLeafBrushes
	{
		char __pad0[0x2];
		unsigned short count;
		char __pad1[0x4];
		char (*data)[32];
	};
	static_assert(sizeof(ClipLeafBrushes) == 0x10);

	struct ClipInfo
	{
		int planeCount;
		cplane_s* planes;
		unsigned int numMaterials;
		ClipMaterial* materials;
		ClipBrushInfo brushes;
		ClipBrushBounds brushBounds;
		ClipLeafBrushes leafBrushes;
		char __pad0[0x1C8];
		ClipMapTrZone* trZones[16];
	};
	static_assert(sizeof(ClipInfo) == 0x2A8);

	struct cNode_t
	{
		cplane_s* plane;
		char __pad0[0x8];
	};
	static_assert(sizeof(cNode_t) == 0x10);

	struct cmodel_t
	{
		char __pad0[0x20];
		ClipInfo* info;
		char __pad1[0x40];
	};
	static_assert(sizeof(cmodel_t) == 0x68);

	struct ClipStaticModelName
	{
		const char* name;
		char __pad0[0x18];
	};
	static_assert(sizeof(ClipStaticModelName) == 0x20);

	struct ClipStaticModelData
	{
		unsigned int nodeCount;
		char (*nodes)[16];
		unsigned int leafCount;
		char (*leafs)[40];
		unsigned int boundsCount;
		char (*bounds)[20];
	};
	static_assert(sizeof(ClipStaticModelData) == 0x30);

	struct DynEntityDef
	{
		char __pad0[0x20];
		XModel* xModel;
		char __pad1[0x8];
		FxEffectDef* destroyFx;
		snd_alias_list_t* sound;
		PhysPreset* physPreset;
		char (*hinge)[44];
		char (*linkTo)[28];
		char __pad2[0x20];
	};
	static_assert(sizeof(DynEntityDef) == 0x78);

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
		char (*leafs)[56];
		unsigned int numSubModels;
		cmodel_t* cmodels;
		MapEnts* mapEnts;
		ClipStaticModelName* staticModelNames;
		unsigned char staticModelNameCount;
		char __pad1[0x7];
		ClipStaticModelData staticModelData;
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
		char __pad3[0x50];
	};
	static_assert(sizeof(clipMap_t) == 0x400);

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
		char __pad0[0xA0];
		const char* defName;
	};
	static_assert(sizeof(ComPrimaryLight) == 0xA8);

	struct ComWorld
	{
		const char* name;
		char __pad0[0x8];
		unsigned int primaryLightCount;
		ComPrimaryLight* primaryLights;
		unsigned int primaryLightEnvCount;
		char (*primaryLightEnvs)[10];
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

	struct GfxWorld
	{
		const char* name;
		const char* baseName;
		char __pad0[0x10B8];
	};
	static_assert(sizeof(GfxWorld) == 0x10C8);

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

	struct VehicleDef
	{
		const char* name;
		char __pad0[0xBE0];
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

	struct EquipmentSndTable
	{
		const char* name;
		char __pad0[0x50];
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
