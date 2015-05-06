#pragma once
#include "VoxelChunck.h"
#include "SphereChunck.h"
#include <algorithm>
class ChunckManager {
public:
	static const int NUM_VOXEL_CHUNCKS;

	ChunckManager();
	~ChunckManager();

	void createChunck();
	void addChunkThreaded();

	Mesh* getMeshAt(int i);
	Mesh* getMeshWithId(int id);
	VoxelChunck* getChunckAt(int i);

	int getSizeOfChuncks() const { return voxelChuncks.size(); }
	//Mesh* getSphereChunck() const { return sphere->getCube(); }

	bool getVoxelActive(Vector3 position) const;
	void restartChunckManager();

	float getHeightAtPosition(Vector3 position, float scaleTerrain);

	static Vector3 transformVoxelCoordinatesToWorldCoordinates(Vector3 position, float scale, int xOffset, int yOffset, int zOffset);
	static Vector3 transformWorldCoordinatesToVoxelCoordinates(Vector3 position, float scale, int xOffset, int yOffset, int zOffset);

	static Vector3 START_POSITION_PUDDLE;

	Mesh* getPendingChunckMesh(){ return pendingChunck->getCube(); };
	VoxelChunck* getPendingChunck(){ return pendingChunck; };
private:
	std::vector<VoxelChunck*> voxelChuncks;
	VoxelChunck* pendingChunck;

	Vector3 positionToPlaceAnotherChunck;

	CellGcmTexture texture;
	CellGcmTexture bumpTexture;
	CellGcmTexture puddleTexture;

	//SphereChunck* sphere;

	int xOffset;
	int zOffset;
//	module::Perlin perlinModule;

	void ModifyChunckThreaded(VoxelChunck* currentChunck);
};