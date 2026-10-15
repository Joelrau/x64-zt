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

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};
	assert_sizeof(Bounds, 24);

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
		unsigned int unkCount_0xC0; // 192
		char (*unk_0xC8)[64]; // 200
		unsigned int unkCount_0xD0; // 208
		char (*unk_0xD8)[64]; // 216
		ID3D11Buffer* unk_0xE0Buffer; // 224
		ID3D11ShaderResourceView* unk_0xE8View; // 232
		ID3D11UnorderedAccessView* unk_0xF0RWView; // 240
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
	assert_offsetof(GfxLightGridProbeData, unkCount_0xC0, 192);
	assert_offsetof(GfxLightGridProbeData, unk_0xC8, 200);
	assert_offsetof(GfxLightGridProbeData, unkCount_0xD0, 208);
	assert_offsetof(GfxLightGridProbeData, unk_0xD8, 216);
	assert_offsetof(GfxLightGridProbeData, unk_0xF0RWView, 240);

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
		char unk_0x0[24]; // 0
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

	struct GfxLightViewFrustum
	{
		char unk_0x0[24]; // 0
		unsigned int vertexCount; // 24
		unsigned int planeCount; // 28
		unsigned int indexCount; // 32
		unsigned int unkIndexCount; // 36
		float (*matrix)[16]; // 40
		vec3_t* vertices; // 48
		vec4_t* planes; // 56
		unsigned short* indices; // 64
		unsigned short* unkIndices; // 72
		char (*unkRuntime)[8]; // 80
		char unk_0x58[24]; // 88
	};
	assert_sizeof(GfxLightViewFrustum, 112);
	assert_offsetof(GfxLightViewFrustum, vertexCount, 24);
	assert_offsetof(GfxLightViewFrustum, planeCount, 28);
	assert_offsetof(GfxLightViewFrustum, indexCount, 32);
	assert_offsetof(GfxLightViewFrustum, unkIndexCount, 36);
	assert_offsetof(GfxLightViewFrustum, matrix, 40);
	assert_offsetof(GfxLightViewFrustum, vertices, 48);
	assert_offsetof(GfxLightViewFrustum, planes, 56);
	assert_offsetof(GfxLightViewFrustum, indices, 64);
	assert_offsetof(GfxLightViewFrustum, unkIndices, 72);
	assert_offsetof(GfxLightViewFrustum, unkRuntime, 80);

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
		float mins[3];
		float maxs[3];
		float lightingOrigin[3];
	};
	assert_sizeof(GfxStaticModelInst, 36);

	enum StaticModelFlag : std::uint16_t
	{
		STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING = 0x80,
		STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING = 0x100,
	};

	struct GfxStaticModelDrawInst
	{
		char unk_0x0[72]; // 0
		XModel* model; // 72
		unsigned int unk_0x50; // 80
		unsigned short flags; // 84
		char unk_0x56[42]; // 86
	};
	assert_sizeof(GfxStaticModelDrawInst, 128);
	assert_offsetof(GfxStaticModelDrawInst, model, 72);
	assert_offsetof(GfxStaticModelDrawInst, flags, 84);

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

	union GfxStaticModelLighting
	{
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

	struct GfxSurface
	{
		char unk_0x0[16]; // 0
		GfxSurfaceBuffer* buffer; // 16
		char unk_0x18[8]; // 24
		Material* material; // 32
		char unk_0x28[16]; // 40
	};
	assert_sizeof(GfxSurface, 56);
	assert_offsetof(GfxSurface, buffer, 16);
	assert_offsetof(GfxSurface, material, 32);

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
		char (*surfacesBounds)[64]; // 760
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
		int lightViewFrustumCount; // 2824
		GfxLightViewFrustum* lightViewFrustums; // 2832
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
	assert_offsetof(GfxWorld, lightViewFrustumCount, 2824);
	assert_offsetof(GfxWorld, lightViewFrustums, 2832);
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
