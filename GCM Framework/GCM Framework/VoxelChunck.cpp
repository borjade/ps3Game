#include "VoxelChunck.h"

//using namespace noise;

const int VoxelChunck::CHUNCK_SIZE_X = 50;
const int VoxelChunck::CHUNCK_SIZE_Y = 50;
const int VoxelChunck::CHUNCK_SIZE_Z = 50;

VoxelChunck::VoxelChunck(CellGcmTexture tex, CellGcmTexture btexture, Vector3 start, int id, float xOffset, float yOffset/*, module::Perlin perlinModule*/) {
	startOffset = start;
	blocks = new VoxelCube**[CHUNCK_SIZE_X];
	for (int i = 0; i < CHUNCK_SIZE_X; i++) {
		blocks[i] = new VoxelCube*[CHUNCK_SIZE_Y];
		for (int j = 0; j < CHUNCK_SIZE_Y; j++) {
			blocks[i][j] = new VoxelCube[CHUNCK_SIZE_Z];
		}
	}

	cube = Mesh::GenerateCube(CHUNCK_SIZE_X);
	setTextureForCube(tex, btexture);
	this->id = id;

	vertexData = new Vertex[36];


	//bind buffer data
	//cube->BindBuffers(CHUNCK_SIZE_X * CHUNCK_SIZE_Y * CHUNCK_SIZE_Z * 36);
	setupLandscape( xOffset, yOffset);
	createMesh();
	//cube->UpdateMeshData();
}

void VoxelChunck::setTextureForCube(CellGcmTexture texture, CellGcmTexture bumpTexture) {
	cube->SetDefaultTexture(texture);
	/*cube->SetBumpMap(bumpTexture);
	cube->SetTextureAndBumpmapRepeat(true);*/
}

VoxelChunck::~VoxelChunck() {
	for (int i = 0; i < CHUNCK_SIZE_X; i++) {
		for (int j = 0; j < CHUNCK_SIZE_Y; j++) {
			delete[] blocks[i][j];
		}
		delete[] blocks[i];
	}
	delete[] blocks;
	delete cube;

	delete[] vertexData;
}

void VoxelChunck::createMesh() {
	for (int i = 0; i < CHUNCK_SIZE_X; i++) {
		for (int j = 0; j < CHUNCK_SIZE_Y; j++) {
			for (int k = 0; k < CHUNCK_SIZE_Z; k++) {
				if (blocks[i][j][k].getIsActive()) {
					bool onLeft = false;
					bool onRight = false;
					bool onTop = false;
					bool onBottom = false;
					bool onFront = false;
					bool onBack = false;
					if (i > 0) {
						onLeft = blocks[i - 1][j][k].getIsActive();
					}
					if (i < CHUNCK_SIZE_X - 1) {
						onRight = blocks[i + 1][j][k].getIsActive();
					}
					if (j > 0) {
						onBottom = blocks[i][j - 1][k].getIsActive();
					}
					if (j < CHUNCK_SIZE_Y - 1) {
						onTop = blocks[i][j + 1][k].getIsActive();
					}
					if (k > 0) {
						onBack = blocks[i][j][k - 1].getIsActive();
					}
					if (k < CHUNCK_SIZE_Z - 1) {
						onFront = blocks[i][j][k + 1].getIsActive();
					}
					//create the cube at the position according to the offset
					createCube(i + startOffset.getX(), j, k + startOffset.getZ(), onLeft, onRight, onTop, onBottom, onFront, onBack);
				}
			}
		}
	}

	//set the ending offset so that the next block knows where to start
	endOffset.setX(startOffset.getX());
	endOffset.setY(startOffset.getY());
	endOffset.setZ(startOffset.getZ() + CHUNCK_SIZE_Z);
}

void VoxelChunck::createCube(int x, int y, int z, bool onLeft, bool onRight, bool onTop, bool onBottom, bool onFront, bool onBack) {
	int nrVertices = (!onLeft) * 6 + (!onRight) * 6 + (!onTop) * 6 + (!onBottom) * 6 + (!onFront) * 6 + (!onBack) * 6;

	Vector3 p1(x - VoxelCube::BLOCK_RENDER_SIZE, y - VoxelCube::BLOCK_RENDER_SIZE, z + VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p2(x + VoxelCube::BLOCK_RENDER_SIZE, y - VoxelCube::BLOCK_RENDER_SIZE, z + VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p3(x + VoxelCube::BLOCK_RENDER_SIZE, y + VoxelCube::BLOCK_RENDER_SIZE, z + VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p4(x - VoxelCube::BLOCK_RENDER_SIZE, y + VoxelCube::BLOCK_RENDER_SIZE, z + VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p5(x + VoxelCube::BLOCK_RENDER_SIZE, y - VoxelCube::BLOCK_RENDER_SIZE, z - VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p6(x - VoxelCube::BLOCK_RENDER_SIZE, y - VoxelCube::BLOCK_RENDER_SIZE, z - VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p7(x - VoxelCube::BLOCK_RENDER_SIZE, y + VoxelCube::BLOCK_RENDER_SIZE, z - VoxelCube::BLOCK_RENDER_SIZE);
	Vector3 p8(x + VoxelCube::BLOCK_RENDER_SIZE, y + VoxelCube::BLOCK_RENDER_SIZE, z - VoxelCube::BLOCK_RENDER_SIZE);

#pragma region AUTO
	//auto createNormalsAndTangents = [&](const int& currentIndex){

	//	const int indexA = currentIndex - 6;
	//	const int indexB = currentIndex - 5;
	//	const int indexC = currentIndex - 4;

	//	//create normals here
	//	Vector3 &a = arrayVertices[indexA];
	//	Vector3 &b = arrayVertices[indexB];
	//	Vector3 &c = arrayVertices[indexC];

	//	Vector3 normal = Vectormath::Aos::cross(b - a, c - a);
	//	normal = Vectormath::Aos::normalize(normal);

	//	for (int k = 1; k < 7; ++k)
	//	{
	//		arrayNormals[currentIndex - k] = normal;
	//	}

	//	//create tangents here
	//	Vector2 coord1 = arrayTexture[indexB] - arrayTexture[indexA];
	//	Vector2 coord2 = arrayTexture[indexC] - arrayTexture[indexA];

	//	Vector3 vertex1 = arrayVertices[indexB] - arrayVertices[indexA];
	//	Vector3 vertex2 = arrayVertices[indexC] - arrayVertices[indexA];

	//	Vector3 axis = Vector3(vertex1*coord2.y - vertex2*coord1.y);

	//	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);
	//	Vector3 tangent = Mesh::GenerateTangent(arrayVertices[indexA], arrayVertices[indexB], arrayVertices[indexC], arrayTexture[indexA], arrayTexture[indexB], arrayTexture[indexC]);

	//	for (int k = 1; k < 7; ++k)
	//	{
	//		arrayTangents[currentIndex - k] = tangent;
	//	}
	//};
#pragma endregion AUTO

	int index = 0;
	if (!onFront) {
		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p1.getX();
		vertexData[index++].y = p1.getY();
		vertexData[index++].z = p1.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p2.getX();
		vertexData[index++].y = p2.getY();
		vertexData[index++].z = p2.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p3.getX();
		vertexData[index++].y = p3.getY();
		vertexData[index++].z = p3.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p3.getX();
		vertexData[index++].y = p3.getY();
		vertexData[index++].z = p3.getZ();
		
		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p4.getX();
		vertexData[index++].y = p4.getY();
		vertexData[index++].z = p4.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p1.getX();
		vertexData[index++].y = p1.getY();
		vertexData[index++].z = p1.getZ();

		//createNormalsAndTangents(index);
	}
	if (!onBack) {

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p7.getX();
		vertexData[index++].y = p7.getY();
		vertexData[index++].z = p7.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p8.getX();
		vertexData[index++].y = p8.getY();
		vertexData[index++].z = p8.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p5.getX();
		vertexData[index++].y = p5.getY();
		vertexData[index++].z = p5.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p5.getX();
		vertexData[index++].y = p5.getY();
		vertexData[index++].z = p5.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p6.getX();
		vertexData[index++].y = p6.getY();
		vertexData[index++].z = p6.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p7.getX();
		vertexData[index++].y = p7.getY();
		vertexData[index++].z = p7.getZ();

		//createNormalsAndTangents(index);
	}
	if (!onTop) {

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p4.getX();
		vertexData[index++].y = p4.getY();
		vertexData[index++].z = p4.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p3.getX();
		vertexData[index++].y = p3.getY();
		vertexData[index++].z = p3.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p8.getX();
		vertexData[index++].y = p8.getY();
		vertexData[index++].z = p8.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p8.getX();
		vertexData[index++].y = p8.getY();
		vertexData[index++].z = p8.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p7.getX();
		vertexData[index++].y = p7.getY();
		vertexData[index++].z = p7.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p4.getX();
		vertexData[index++].y = p4.getY();
		vertexData[index++].z = p4.getZ();

		//createNormalsAndTangents(index);
	}
	if (!onBottom) {
		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p6.getX();
		vertexData[index++].y = p6.getY();
		vertexData[index++].z = p6.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p5.getX();
		vertexData[index++].y = p5.getY();
		vertexData[index++].z = p5.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p2.getX();
		vertexData[index++].y = p2.getY();
		vertexData[index++].z = p2.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p2.getX();
		vertexData[index++].y = p2.getY();
		vertexData[index++].z = p2.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p1.getX();
		vertexData[index++].y = p1.getY();
		vertexData[index++].z = p1.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p6.getX();
		vertexData[index++].y = p6.getY();
		vertexData[index++].z = p6.getZ();

		//createNormalsAndTangents(index);
	}
	if (!onLeft) {

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p6.getX();
		vertexData[index++].y = p6.getY();
		vertexData[index++].z = p6.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p1.getX();
		vertexData[index++].y = p1.getY();
		vertexData[index++].z = p1.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p4.getX();
		vertexData[index++].y = p4.getY();
		vertexData[index++].z = p4.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p4.getX();
		vertexData[index++].y = p4.getY();
		vertexData[index++].z = p4.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p7.getX();
		vertexData[index++].y = p7.getY();
		vertexData[index++].z = p7.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p6.getX();
		vertexData[index++].y = p6.getY();
		vertexData[index++].z = p6.getZ();

		//createNormalsAndTangents(index);
	}
	if (!onRight) {

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p2.getX();
		vertexData[index++].y = p2.getY();
		vertexData[index++].z = p2.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p5.getX();
		vertexData[index++].y = p5.getY();
		vertexData[index++].z = p5.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p8.getX();
		vertexData[index++].y = p8.getY();
		vertexData[index++].z = p8.getZ();

		vertexData[index].u = 1.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p8.getX();
		vertexData[index++].y = p8.getY();
		vertexData[index++].z = p8.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 1.0f;
		vertexData[index++].x = p3.getX();
		vertexData[index++].y = p3.getY();
		vertexData[index++].z = p3.getZ();

		vertexData[index].u = 0.0f;
		vertexData[index].v = 0.0f;
		vertexData[index++].x = p2.getX();
		vertexData[index++].y = p2.getY();
		vertexData[index++].z = p2.getZ();

		//createNormalsAndTangents(index);
	}

	//cube->AddCubeToMesh(vertexData, nrVertices);
}

void VoxelChunck::setupLandscape(/*module::Perlin perlinModule,*/ float xOffset, float zOffset) {
	for (int x = 0; x < CHUNCK_SIZE_X; x++) {
		for (int z = 0; z < 5; z++) {
			blocks[x][0][z].setIsActive(true);
		}
	}
	for (int x = CHUNCK_SIZE_X - 6; x > 0; x--) {
		for (int z = 3; z < 5; z++) {
			blocks[x][1][z].setIsActive(true);
		}
	}
	for (int x = CHUNCK_SIZE_X - 9; x > 0; x--) {
		blocks[x][2][4].setIsActive(true);
	}

	for (int x = 0; x < CHUNCK_SIZE_X; x++) {
		for (int z = 4; z < CHUNCK_SIZE_Z; z++) {
			float nrX = (x + xOffset) * 0.01f;
			float nrZ = (z + zOffset) * 0.01f;
			int height = 3;// round((perlinModule.GetValue(nrX, 0, nrZ)) * 20);

			if (z == (CHUNCK_SIZE_Z - 1) && height > 3) {
				height = 3;
			}

			if (z == (CHUNCK_SIZE_Z - 2) && height > 5) {
				height = 5;
			}

			if (z == (CHUNCK_SIZE_Z - 3) && height > 7) {
				height = 7;
			}

			if (z == (CHUNCK_SIZE_Z - 4) && height > 9) {
				height = 9;
			}

			blocks[x][0][z].setIsActive(true);
			for (int y = 1; y < height; y++) {
					blocks[x][y][z].setIsActive(true);
			}
		}
	}
}

float VoxelChunck::getHeightAtPosition(Vector3 position, /*module::Perlin module,*/ float scaleTerrain) {
	for (int i = 0; i < CHUNCK_SIZE_Y; i++) {
		if (!blocks[(int)position.getX()][i][(int)position.getZ()].getIsActive()) {
			return (i) * scaleTerrain;
		}
	}
}

void VoxelChunck::updateChunck(CellGcmTexture texture, CellGcmTexture bTexture, Vector3 position, float xOffset, float yOffset/*, module::Perlin perlinModule*/) {
	//i know the texture I want to set to the voxel
	this->texture = texture;
	this->bumpTexture = bTexture;
	//i know the position to set it at
	this->startOffset = position;

	////delete old data in mesh
	//cube->DeleteOldData();

	//based on that I want to create the landscape that this mesh will represent
	//this will set the blocks within the mesh as active or not
	//heightMapBuilder.SetBounds(rectangleBoundaries.x, rectangleBoundaries.y, rectangleBoundaries.z, rectangleBoundaries.w);
	setupLandscape(/*perlinModule,*/ xOffset, yOffset);

	//now let's create the mesh according to the new position
	createMesh();
	updateMesh();
}

void VoxelChunck::updateChunckThreaded(CellGcmTexture texture, CellGcmTexture bTexture, Vector3 position, float xOffset, float yOffset/* module::Perlin perlinModule*/) {
	//i know the texture I want to set to the voxel
	this->texture = texture;
	this->bumpTexture = bTexture;
	//i know the position to set it at
	this->startOffset = position;

	//delete old data in mesh
	/*cube->DeleteOldData();*/

	setupLandscape(/*perlinModule, */xOffset, yOffset);

	//now let's create the mesh according to the new position
	createMesh();
}

void VoxelChunck::updateMesh() {
	//after adding all the data to the mesh, calculate the normals, tangents and send the data to the renderer
	//this->cube->UpdateMeshData();
}


bool VoxelChunck::getIsVoxelActive(Vector3 position) const {
	return blocks[(int)position.getX()][(int)position.getY()][(int)position.getY()].getIsActive();
}