#pragma once

namespace zonetool::iw8
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	struct dummy
	{
	};

	struct GfxWrappedBuffer
	{
		void* buffer; // 0
		char view[48]; // 8
		void* data; // 56
	}; static_assert(sizeof(GfxWrappedBuffer) == 64);

	struct GfxFrustumLights
	{
		char __pad0[64]; // 0
		GfxWrappedBuffer indexBuffer;		// 64
		GfxWrappedBuffer vertexBuffer;		// 128
	}; static_assert(sizeof(GfxFrustumLights) == 192);

	struct GfxWorldDpvsPlane
	{
		vec3_t normal;
		float dist;
		char type;
		char pad[3];
	};

	struct GfxWorldDpvsPlanes
	{
		unsigned int cellCount;		// 0
		unsigned short planeCount;	// 4
		GfxWorldDpvsPlane* planes;	// 8
		unsigned short nodeCount;	// 16
		unsigned short* nodes;		// 24
		unsigned int* sceneEntCellBits; // 32
	}; static_assert(sizeof(GfxWorldDpvsPlanes) == 40);

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};

	struct GfxPortal;

	struct GfxPortalWritable
	{
		bool isQueued;
		bool isAncestor;
		char recursionDepth;
		char hullPointCount;
		vec2_t* hullPoints;
		GfxPortal* queuedParent;
	};

	struct DpvsPlane
	{
		vec4_t coeffs;
	};

	struct GfxPortal
	{
		GfxPortalWritable writable; // 0
		DpvsPlane plane;			// 24
		vec3_t* vertices;			// 40
		unsigned __int16 cellIndex; // 48
		unsigned __int16 closeDistance; // 50
		char vertexCount; // 52
		vec3_t hullAxis[2];
	};

	struct GfxCell
	{
		Bounds bounds; // 0
		unsigned short portalCount; // 24
		GfxPortal* portals; // 32
	}; static_assert(sizeof(GfxCell) == 40);

	struct GfxCellTransientInfo
	{
		unsigned __int16 aabbTreeIndex;
		unsigned __int16 transientZone;
	};

	struct srfTriangles_t
	{
		unsigned int posOffset;
		int displacementParmsOffset;
		unsigned __int16 vertexCount;
		unsigned __int16 triCount;
		unsigned int baseIndex;
	};

	struct __declspec(align(8)) GfxSurface
	{
		srfTriangles_t tris;		// 0
		void* material;				// 16
		unsigned int surfDataIndex;	// 24
		unsigned __int16 transientZone; // 28
		unsigned __int16 lightmapIndex; // 32
		char flags[1]; // 36
	}; // 40

	struct GfxSurfaceBounds
	{
		Bounds bounds;
	}; // 24 -> 16????? what

	struct GfxWorldSurfData
	{
		unsigned int transientZone;
		unsigned int layerCount;
		unsigned int xyzOffset;
		unsigned int tangentFrameOffset;
		unsigned int lmapCoordOffset;
		unsigned int colorOffset;
		unsigned int texCoordOffset;
		unsigned int normalTransformOffset[7];
		unsigned int displacementOffset[8];
	};

	struct GfxDrawSurfFields
	{
		unsigned __int64 indirectArgsOffset : 10;
		unsigned __int64 gpBatchIndex : 16;
		unsigned __int64 objectId : 24;
		unsigned __int64 hasGfxEntIndex : 1;
		unsigned __int64 lightmapIndex : 9;
		unsigned __int64 shadowcaster : 1;
		unsigned __int64 materialSortedIndex : 16;
		unsigned __int64 tessellation : 3;
		unsigned __int64 prepass : 2;
		unsigned __int64 viewModelRender : 1;
		unsigned __int64 lowResShading : 1;
		unsigned __int64 surfType : 4;
		unsigned __int64 primarySortKey : 6;
	};

	/* 19635 */
	struct Packed128
	{
		unsigned __int64 p0;
		unsigned __int64 p1;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		Packed128 packed;
	};

	struct GfxWorldSurfaces
	{
		unsigned int count;					// 0
		unsigned int surfDataCount;			// 4
		unsigned int litOpaqueSurfsBegin;	// 8
		unsigned int litOpaqueSurfsEnd;		// 12
		unsigned int litDecalSurfsBegin;	// 16
		unsigned int litDecalSurfsEnd;		// 20
		unsigned int litTransSurfsBegin;	// 24
		unsigned int litTransSurfsEnd;		// 28
		unsigned int emissiveSurfsBegin;	// 32
		unsigned int emissiveSurfsEnd;		// 36
		//unsigned int himipRadiusInvSqCompactedCount;
		//unsigned int himipRadiusInvSqBModelCompactedCount;
		//unsigned int himipRadiusInvSqBModelSurfaceCount;
		GfxSurface* surfaces;						// 40
		unsigned __int16* surfacesTransientZone;	// 48
		GfxSurfaceBounds* surfaceBounds;			// 56
		//unsigned int* himipRadiusInvSqIndirection;	
		//float* himipRadiusInvSqCompacted;
		//unsigned int* himipRadiusInvSqBmodelIndirection;
		//unsigned int* himipRadiusInvSqBmodelSurfaceIndirection;
		//float* himipRadiusInvSqBModelCompacted;
		GfxDrawSurf* surfaceMaterials; // 64
		//GfxWorldSurfData* surfData;
		GfxWrappedBuffer surfDataBuffer; // 72
	}; assert_sizeof(GfxWorldSurfaces, 136); // 200 -> 136?

	struct GfxStaticModelSurface
	{
		unsigned __int16 smodelIndex; // 0
		char lodIndex; // 2
		char surfIndex; // 3
	}; assert_sizeof(GfxStaticModelSurface, 4);

	struct GfxClutterInstancePool
	{
		unsigned int start; // 0
		unsigned int size; // 4
	}; assert_sizeof(GfxClutterInstancePool, 8);

	struct __declspec(align(8)) GfxStaticModel
	{
		void* model; // 0
		char flags[1]; // 8
		char firstMtlSkinIndex; // idek lmfao
		unsigned __int16 firstStaticModelSurfaceIndex; // 14
	}; assert_sizeof(GfxStaticModel, 16);

	struct GfxWorldStaticModels
	{
		unsigned int surfaceCount;				// 0
		unsigned int modelCount;				// 4
		unsigned int collectionCount;			// 8
		unsigned int clutterCollectionCount;	// 12
		unsigned int instanceCount;				// 16
		unsigned int clutterTintCount;			// 20
		unsigned int lmapTransformsCount;		// 24
		unsigned int materialCount;				// 28
		unsigned int collectionVisDataCount;	// 32
		unsigned int splinedModelInstanceCount;	// 36
		unsigned int splineSegmentCount;		// 40
		unsigned int unk_pad;					// 44 (could be wrong)

		GfxClutterInstancePool clutterInstancePool;	// 48
		GfxStaticModelSurface* surfaces;			// 56
		GfxStaticModel* models;						// 64

		char __pad0[1200]; // 72
		/*
		GfxClutterCollection* clutterCollections;	// 72
		GfxStaticModelCollection* collections;		// 80
		GfxStaticModelInstance* instances;			// 88
		char* instanceFlags;						// 96
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
		HLSL::GpuSplinedModelInstance* splinedModelInstanceData;
		GfxWrappedBuffer splinedModelInstanceBuffer;
		HLSL::GpuSplineSegment* splineSegmentData;
		GfxWrappedBuffer splineSegmentBuffer;
		Bounds* splinedModelInstanceBounds;
		GfxWrappedBuffer splinedModelInstanceBoundsBuffer;
		bitarray_dynamic collectionTransientZonePrevFrameVisibility;
		unsigned int* collectionVisData;
		char* collectionVisRefCount;
		*/
	}; assert_sizeof(GfxWorldStaticModels, 1272);

	struct GfxWorldTransientZone
	{

	};

	struct GfxWorldDrawVerts
	{
		char __pad0[200];
	}; assert_sizeof(GfxWorldDrawVerts, 200);

	struct GfxCellTreeCount
	{
		int aabbTreeCount;
	}; assert_sizeof(GfxCellTreeCount, 4);

	struct GfxAabbTree
	{
		Bounds bounds; // 0
		int childrenOffset; // 24
		unsigned short childCount; // 28
		char __pad0[8];	// 30
		unsigned short smodelIndexCount; // 38
		unsigned short* smodelIndexes; // 40
		//int startSurfIndex; // 48
		//unsigned short surfaceCount;
		//unsigned short pad;
	}; assert_sizeof(GfxAabbTree, 48);
	assert_offsetof(GfxAabbTree, smodelIndexCount, 38);
	assert_offsetof(GfxAabbTree, smodelIndexes, 40); // was 32

	struct GfxCellTree
	{
		GfxAabbTree* aabbTree;
	}; assert_sizeof(GfxCellTree, 8);

	struct GfxWorldDrawCells
	{
		unsigned int cellCount; // 0
		unsigned int unk; // 4
		GfxCellTreeCount* aabbTreeCounts; // 8
		GfxCellTree* aabbTrees; // 16
	}; assert_sizeof(GfxWorldDrawCells, 24);

	struct GfxWorldTransientZone
	{
		const char* name;					// 0
		unsigned int transientZoneIndex;	// 8
		GfxWorldDrawVerts drawVerts;		// 16
		GfxWorldDrawCells drawCells;		// 216
		void* tempLightmapData;				// 240 (GfxLightmapAtlasDataTemp)
		GfxGpuLightGridTransientData gpuLightGrid;			// 248
		GfxReflectionProbeTransientData* reflectionProbes;
		unsigned int collectionCount;
		unsigned __int16* collections;
		GfxDecalVolumeTransientData* decalVolumes;
		GfxCompressedSunShadowDataTemp* compressedSunShadow;
	};

	struct GfxWorldDraw
	{
		// 264 decalVolumeMaterialCount
		// 268 decalVolumeMaskCount
		// 272 decalVolumeCollectionCount
		// 280 decalVolumeMaterials
		// 288 decalVolumeMasks
		// 296 decalVolumeCollections
		// 304 materialAndMaskIndirection
		// 312 materialAndMaskIndirectionBuffer
		char __pad0[392]; // 0
		GfxWorldTransientZone* transientZones[1536]; // 392
		char __pad0[192]; // 12680
	}; assert_sizeof(GfxWorldDraw, 12872);

	struct GfxWorld
	{
		const char* name; // 0
		const char* baseName; // 8
		unsigned int bspVersion; // 16

		unsigned int lastSunPrimaryLightIndex;				// 20
		unsigned int primaryLightCount;						// 24
		unsigned int firstMutablePrimaryLight;				// 28
		unsigned int mutablePrimaryLightCount;				// 32
		unsigned int firstStaticScriptablesPrimaryLight;	// 36
		unsigned int staticScriptablesPrimaryLightCount;	// 40
		unsigned int firstStaticScriptablePrimaryLight;		// 44
		unsigned int staticScriptablePrimaryLightCount;		// 48
		unsigned int firstMovingScriptablePrimaryLight;		// 52
		unsigned int movingScriptablePrimaryLightCount;		// 56
		unsigned int sortKeyLitDecal;						// 60
		unsigned int sortKeyEffectDecal;					// 64
		unsigned int sortKeyTopDecal;						// 68
		unsigned int sortKeyEffectAuto;						// 72
		unsigned int sortKeyDistortion;						// 76
		unsigned int sortKeyEffectDistortion;				// 80
		unsigned int sortKey2D;								// 84
		unsigned int sortKeyOpaqueBegin;					// 88
		unsigned int sortKeyOpaqueEnd;						// 92
		unsigned int sortKeyDecalBegin;						// 96
		unsigned int sortKeyDecalEnd;						// 100
		unsigned int sortKeyTransBegin;						// 104
		unsigned int sortKeyTransEnd;						// 108
		unsigned int sortKeyEmissiveBegin;					// 112
		unsigned int sortKeyEmissiveEnd;					// 116

		Bounds bounds;								// 120
		GfxWorldDpvsPlanes dpvsPlanes;				// 144
		GfxCell* cells;								// 184
		GfxCellTransientInfo* cellTransientInfos;	// 192

		GfxWorldSurfaces surfaces;		// 200
		GfxWorldStaticModels smodels;	// 336
		GfxWorldDraw draw;				// 1608

		char dynamicLightset[928]; // 14480
		char mayhemSelfVis[112]; // 15408

		GfxFrustumLights frustumLights; // 15520 (160 on 1.19, 32 byte difference)

		// this can all be stored into the pad but im using it for debugging
		void* lightViewFrustums; // 15712
		void* primaryLights; // 15720
		__int64 voxelTreeCount; // 15728
		void* voxelTree; // 15736

		char __pad1[2064]; // 15744
	}; static_assert(sizeof(GfxWorld) == 17808);
}
