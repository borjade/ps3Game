#include "VoxelCube.h"

const float VoxelCube::BLOCK_RENDER_SIZE = 0.5f;

bool VoxelCube::getIsActive() const {
	return isActive;
}