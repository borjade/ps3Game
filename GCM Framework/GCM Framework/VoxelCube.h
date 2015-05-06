#pragma once

#include "Mesh.h"

enum BlockType {
	BlockType_Default = 0,
	BlockType_Grass,
	BlockType_Stone,
	BlockType_Water,
	BlockType_NumTypes
};

class VoxelCube {
public:
	VoxelCube() : isActive(false), blockType(BlockType_Stone) {}
	~VoxelCube() {}

	bool getIsActive() const;
	void setIsActive(bool val) { isActive = val; }

	BlockType getBlockType() const { return blockType; }
	void setBlockType(BlockType bType) { blockType = bType; }

	static const float BLOCK_RENDER_SIZE;
private:
	bool isActive;
	BlockType blockType;
};