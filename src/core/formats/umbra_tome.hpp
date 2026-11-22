#pragma once

namespace umbra_tome
{
	struct UmbraTomeHeader
	{
		unsigned int version;
		unsigned int checksum;
		unsigned int size;
		float unitsPerMeter;
		unsigned int unk0x10;
		float boundsMin[3];
		float boundsMax[3];
		unsigned int treeDepth;
		unsigned int topTree;
		unsigned int topMap;
		unsigned int unk0x38[2];
		unsigned int objectCount;
		unsigned int objectBounds;
		unsigned int objectDistances;
		unsigned int unk0x4C;
		unsigned int userIDs;
		unsigned int indexWidth;
		unsigned int objectLists;
		unsigned int objectCount2;
		unsigned int unk0x60[10];
		unsigned int cellStarts;
		unsigned int cellCount;
		unsigned int tileCount;
		unsigned int unk0x94[2];
		unsigned int tileLodLevels;
		unsigned int tileOffsets;
		unsigned int unk0xA4[42];
		unsigned int tilePortalExpands;
		float rootBoundsMin[3];
		float rootBoundsMax[3];
		float tileScale;
		unsigned int unk0x16C;
	}; assert_sizeof(UmbraTomeHeader, 0x170);
	assert_offsetof(UmbraTomeHeader, unitsPerMeter, 0x0C);
	assert_offsetof(UmbraTomeHeader, boundsMin, 0x14);
	assert_offsetof(UmbraTomeHeader, treeDepth, 0x2C);
	assert_offsetof(UmbraTomeHeader, objectCount, 0x40);
	assert_offsetof(UmbraTomeHeader, userIDs, 0x50);
	assert_offsetof(UmbraTomeHeader, objectLists, 0x58);
	assert_offsetof(UmbraTomeHeader, cellStarts, 0x88);
	assert_offsetof(UmbraTomeHeader, tileLodLevels, 0x9C);
	assert_offsetof(UmbraTomeHeader, tileOffsets, 0xA0);
	assert_offsetof(UmbraTomeHeader, tilePortalExpands, 0x14C);
	assert_offsetof(UmbraTomeHeader, rootBoundsMin, 0x150);
	assert_offsetof(UmbraTomeHeader, tileScale, 0x168);

	struct UmbraTomeTile
	{
		float boundsMin[3];
		float boundsMax[3];
		unsigned int depth;
		unsigned int tree;
		unsigned int map;
		unsigned int unk0x24[2];
		unsigned int sizeAndFlags;
		float unk0x30;
		unsigned int unk0x34;
		unsigned int cell;
		unsigned int unk0x3C;
		unsigned int cellIndices;
		float scale;
		float inverseScale;
		unsigned int unk0x4C[5];
	}; assert_sizeof(UmbraTomeTile, 0x60);
	assert_offsetof(UmbraTomeTile, depth, 0x18);
	assert_offsetof(UmbraTomeTile, sizeAndFlags, 0x2C);
	assert_offsetof(UmbraTomeTile, cell, 0x38);
	assert_offsetof(UmbraTomeTile, cellIndices, 0x40);
	assert_offsetof(UmbraTomeTile, scale, 0x44);

	struct UmbraTomeCell
	{
		unsigned int unk0x00[2];
		unsigned int unk0x08;
		unsigned int objectCount;
		unsigned int unk0x10;
		unsigned int unk0x14;
		unsigned int unk0x18;
		unsigned int unk0x1C;
		unsigned int unk0x20;
	}; assert_sizeof(UmbraTomeCell, 0x24);
	assert_offsetof(UmbraTomeCell, objectCount, 0x0C);

	struct object
	{
		unsigned int user_id;
		float minimum[3];
		float maximum[3];
	};

	std::vector<unsigned char> build_single_cell(const float minimum[3], const float maximum[3], const std::vector<object>& objects);
}
