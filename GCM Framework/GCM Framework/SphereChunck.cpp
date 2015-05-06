#include "SphereChunck.h"

SphereChunck::SphereChunck(GLuint tex, GLuint btexture, Vector3 start, int id, float xOffset, float yOffset, module::Perlin perlinModule) {
	startOffset = start;
	blocks = new VoxelCube**[CHUNCK_SIZE_X];
	for (int i = 0; i < CHUNCK_SIZE_X; i++) {
		blocks[i] = new VoxelCube*[CHUNCK_SIZE_Y];
		for (int j = 0; j < CHUNCK_SIZE_Y; j++) {
			blocks[i][j] = new VoxelCube[CHUNCK_SIZE_Z];
		}
	}

	cube = Mesh::GenerateCube();
	setTextureForCube(tex, btexture);
	this->id = id;

	arrayVertices = new Vector3[36];
	arrayTexture = new Vector2[36];
	arrayNormals = new Vector3[36];
	arrayTangents = new Vector3[36];

	//bind buffer data
	cube->BindBuffers(CHUNCK_SIZE_X * CHUNCK_SIZE_Y * CHUNCK_SIZE_Z * 36);
	setupLandscape(perlinModule, xOffset, yOffset);
	createMesh();
	cube->UpdateMeshData();
}

SphereChunck::~SphereChunck() {

}

void SphereChunck::setupLandscape(module::Perlin perlinModule, float xOffset, float yOffset) {
	for (int z = 0; z < CHUNCK_SIZE_Z; z++)	{
		for (int y = 0; y < CHUNCK_SIZE_Y; y++)	{
			for (int x = 0; x < CHUNCK_SIZE_X; x++)	{
				if (sqrt((float)(x - CHUNCK_SIZE_X / 2)*(x - CHUNCK_SIZE_X / 2) + (y - CHUNCK_SIZE_Y / 2)*(y - CHUNCK_SIZE_Y / 2) + (z - CHUNCK_SIZE_Z / 2)*(z - CHUNCK_SIZE_Z / 2)) <= CHUNCK_SIZE_X / 2)	{
					blocks[x][y][z].setIsActive(true);
					blocks[x][y][z].setBlockType(BlockType_Grass);
				}
			}
		}
	}
}