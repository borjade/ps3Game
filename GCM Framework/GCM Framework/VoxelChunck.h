#pragma once

#include "VoxelCube.h"

#include "SceneNode.h"
#include "GameEntity.h"
#include "PhysicsNode.h"
#include "noiseutils.h"

class VoxelChunck {
public:
	static const int CHUNCK_SIZE_X;
	static const int CHUNCK_SIZE_Y;
	static const int CHUNCK_SIZE_Z;

	VoxelChunck() {}
	VoxelChunck(CellGcmTexture tex, CellGcmTexture btexture, Vector3 start, int id, float xOffset, float yOffset/*, module::Perlin perlinModule*/);
	~VoxelChunck();

	Mesh* getCube() { return cube; }

	Vector3 getEndOffset() const { return endOffset; }
	void setEndOffset(Vector3 vect) { endOffset = vect; }

	Vector3 getStartOffset() const { return startOffset; }
	void setStartOffset(Vector3 st) { startOffset = st; }

	int getId() const { return id; }
	void setId(int val) { id = val; }

	void createMesh();
	void updateMesh();
	void createCube(int x, int y, int z, bool onLeft, bool onRight, bool onTop, bool onBottom, bool onFront, bool onBack);

	void updateChunck(CellGcmTexture texture, CellGcmTexture bTexture, Vector3 position, float xOffset, float yOffset/*, module::Perlin perlinModule*/);
	void updateChunckThreaded(CellGcmTexture texture, CellGcmTexture bTexture, Vector3 position, float xOffset, float yOffset);/*, module::Perlin perlinModule*/

	void setupLandscape(/*module::Perlin perlinModule*/ float xOffset, float yOffset);

	bool getIsVoxelActive(Vector3 position) const;

	void setTextureForCube(CellGcmTexture texture, CellGcmTexture bumpTexture);

	float getHeightAtPosition(Vector3 position, /*module::Perlin module*/ float scaleTerrain);
protected:
	VoxelCube*** blocks;
	Mesh* cube;
	CellGcmTexture texture;
	CellGcmTexture bumpTexture;

	Vector4 rectangleBoundaries;
	int id;

	Vector3 startOffset;
	Vector3 endOffset;

	Vertex*	vertexData;

	//Vector3* arrayVertices;
	//Vector2* arrayTexture;
	//Vector3* arrayNormals;
	//Vector3* arrayTangents;
};