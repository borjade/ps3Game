#include "ChunckManager.h"

const int ChunckManager::NUM_VOXEL_CHUNCKS = 4;
Vector3 ChunckManager::START_POSITION_PUDDLE = Vector3(0, 0, 0);

ChunckManager::ChunckManager() {
	xOffset = 0.0f;
	zOffset = 0.0f;

	//createHeightMapForLandscape();
	texture = *GCMRenderer::LoadGTF("/GRASS.gtf");
	bumpTexture = *GCMRenderer::LoadGTF("/GRASS.gtf");

	Vector3 start(0, 0, 0);

	int i;
	for (i = 0; i < NUM_VOXEL_CHUNCKS; i++) {
		VoxelChunck* vChunck = new VoxelChunck(texture, bumpTexture, start, i, xOffset, zOffset/*, perlinModule*/);
		//set the starting point for the next chunck of terrain
		start = vChunck->getEndOffset();
		voxelChuncks.push_back(vChunck);

		//update the coordinates for the heightmap
		xOffset += VoxelChunck::CHUNCK_SIZE_X;
		zOffset += VoxelChunck::CHUNCK_SIZE_Z;

	}
	positionToPlaceAnotherChunck = start;
	pendingChunck = new VoxelChunck(texture, bumpTexture, start, NUM_VOXEL_CHUNCKS, xOffset, zOffset/*, perlinModule*/);
	start = pendingChunck->getEndOffset();
	positionToPlaceAnotherChunck = start;
	//sphere = new SphereChunck(puddleTexture, bumpTexture, START_POSITION_PUDDLE, i, xOffset, zOffset, perlinModule);
}

ChunckManager::~ChunckManager() {
	for (int i = 0; i < voxelChuncks.size(); i++) {
		delete voxelChuncks.at(i);
	}
	voxelChuncks.clear();
}

void ChunckManager::createChunck() {
	////the next chunck of terrain will be created at the end of the existing piece of terrain
	std::rotate(voxelChuncks.begin(), voxelChuncks.begin() + 1, voxelChuncks.end());

	//update the last voxel in the terrain to reflect the new terrain for the player to go on
	int index = voxelChuncks.size() - 1;
	voxelChuncks.at(index)->updateChunck(texture, bumpTexture, positionToPlaceAnotherChunck, xOffset, zOffset/*, perlinModule*/);

	//in case the offset gets too big
	if ((xOffset / VoxelChunck::CHUNCK_SIZE_X) > 100) {
		xOffset = 0.0f;
	}
	if ((zOffset / VoxelChunck::CHUNCK_SIZE_Z) > 100) {
		zOffset = 0.0f;
	}

	xOffset += VoxelChunck::CHUNCK_SIZE_X;
	zOffset += VoxelChunck::CHUNCK_SIZE_Z;

	//set the starting point for the next chunck of terrain
	positionToPlaceAnotherChunck = voxelChuncks.at(voxelChuncks.size() - 1)->getEndOffset();
}

void ChunckManager::addChunkThreaded()
{
	//current chunck that need to be poped out.
	VoxelChunck* currentChunck = voxelChuncks[0];
	voxelChuncks.erase(voxelChuncks.begin());
	//after poping the chunck out, push the chunck at the front of the pending list.
	//pendingChunkMutex.lock();
	pendingChunck->updateMesh();
	voxelChuncks.push_back(pendingChunck);
	pendingChunck = currentChunck;
	//pendingChunkMutex.unlock();
	//then modify the poped chunck
	ModifyChunckThreaded(pendingChunck);

}

void ChunckManager::ModifyChunckThreaded(VoxelChunck* currentChunck)
{
	positionToPlaceAnotherChunck = voxelChuncks.at(voxelChuncks.size() - 1)->getEndOffset();
	currentChunck->updateChunckThreaded(texture, bumpTexture, positionToPlaceAnotherChunck, xOffset, zOffset/*, perlinModule*/);
}

void ChunckManager::restartChunckManager() {
	xOffset = 0.0f;
	zOffset = 0.0f;
	Vector3 start(0, 0, 0);

	for (int i = 0; i < voxelChuncks.size(); i++) {
		voxelChuncks.at(i)->updateChunck(texture, bumpTexture, start, xOffset, zOffset/*, perlinModule*/);

		start = voxelChuncks.at(i)->getEndOffset();

		//update the coordinates for the heightmap
		xOffset += VoxelChunck::CHUNCK_SIZE_X;
		zOffset += VoxelChunck::CHUNCK_SIZE_Z;
	}

	//set the starting point for the next chunck of terrain
	positionToPlaceAnotherChunck = start;
}

Mesh* ChunckManager::getMeshAt(int i) {
	return voxelChuncks.at(i)->getCube();
}

VoxelChunck* ChunckManager::getChunckAt(int i) {
	return voxelChuncks.at(i);
}

Mesh* ChunckManager::getMeshWithId(int id) {
	for (int i = 0; i < voxelChuncks.size(); i++) {
		if (voxelChuncks.at(i)->getId() == id) {
			return voxelChuncks.at(i)->getCube();
		}
	}
}

bool ChunckManager::getVoxelActive(Vector3 position) const {
	return voxelChuncks.at(0)->getIsVoxelActive(position);
}

float ChunckManager::getHeightAtPosition(Vector3 position, float scaleTerrain) {
	//transform the start position of the entity into voxel coordinates
	Vector3 voxels = transformWorldCoordinatesToVoxelCoordinates(position, scaleTerrain, 0, 0, 0);
	return voxelChuncks.at(0)->getHeightAtPosition(voxels/*, perlinModule*/, scaleTerrain);
}

Vector3 ChunckManager::transformVoxelCoordinatesToWorldCoordinates(Vector3 position, float scale, int xOffset = 0, int yOffset = 0, int zOffset = 0) {
	float x = position.getX() * scale + xOffset;
	float y = (position.getY() * scale) + yOffset;
	float z = (position.getZ() * scale) + zOffset;

	return Vector3(x, y, z);
}

Vector3 ChunckManager::transformWorldCoordinatesToVoxelCoordinates(Vector3 position, float scale, int xOffset = 0, int yOffset = 0, int zOffset = 0) {
	int x = round((position.getX() + xOffset) / scale);
	int y = round((position.getY() + yOffset) / scale);
	int z = round(abs(position.getZ() + zOffset) / scale);

	return Vector3(x, y, z);
}