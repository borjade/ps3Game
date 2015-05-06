#include "TerrainClass.h"

Vector3 TerrainClass::START_TERRAIN_POSITION = Vector3(0, 0, 0);
float TerrainClass::SCALE_TERRAIN = 50.0f;
bool TerrainClass::RESTART_TERRAIN = false;

TerrainClass::TerrainClass() {
	//create the terrain
	terrain = new ChunckManager();
	linearVelocityZ = -0.3f;
	treshold = 0 - ((SCALE_TERRAIN * VoxelChunck::CHUNCK_SIZE_Z) - START_TERRAIN_POSITION.getZ());
	countTerrainEntities = 0;
	debug = false;

	BuildFloorEntity();

	amountMovementZ = -terrainPhysicsNodes.at(0)->GetLinearVelocity().getZ();
	backupAmountMovementZ = 0.0f;
}

//restarts the terrain
void TerrainClass::restartTerrain() {
	amountMovementZ = -terrainPhysicsNodes.at(0)->GetLinearVelocity().getZ();
	backupAmountMovementZ = 0.0f;
	treshold = 0 - ((SCALE_TERRAIN * VoxelChunck::CHUNCK_SIZE_Z) - START_TERRAIN_POSITION.getZ());

	for (int i = 0; i < terrainPhysicsNodes.size(); i++) {
		terrainPhysicsNodes.at(i)->SetPosition(START_TERRAIN_POSITION);
	}
	terrain->restartChunckManager();
}

TerrainClass::~TerrainClass() {
	terrainChuncksId.clear();
	terrainEntities.clear();
	terrainPhysicsNodes.clear();
}

void TerrainClass::UpdateTerrain(float msec) {
	// Purpose: Update the track
	if (terrain && terrainEntities.size() > 0) {
		//if the player is past the end of the first chunck then delete the chunck he is at and add a new one at the end
		PhysicsNode* terrainPhysics_01 = terrainPhysicsNodes.at(1);
		Vector3 nr_01 = terrainPhysics_01->GetPosition();

		if ((nr_01.getZ() <= treshold)) {
			//create the chunck at the end
			terrain->createChunck();
			treshold -= (VoxelChunck::CHUNCK_SIZE_Z * SCALE_TERRAIN);
			float diff = abs(amountMovementZ) - abs(backupAmountMovementZ);// +29.1666641f;// -abs(terrainPhysicsNodes.at(0)->GetAmountMovement().z);
			amountMovementZ = -diff;

			//reset the puddle at the beginning of the chunck
			//get the height of the terraain an the start position where i want to place the puddle
			//the 26.0f is because a voxel has a scale of 50 and the origin of the voxel is at 0
			float height = terrain->getHeightAtPosition(ChunckManager::START_POSITION_PUDDLE, SCALE_TERRAIN) + 26.0f;
		}
	}
	backupAmountMovementZ = amountMovementZ;

	//if (Window::GetKeyboard()->KeyDown(TERRAIN_RESTART) && Window::GetKeyboard()->KeyHeld(TERRAIN_RESTART)) {
	//	restartTerrain();
	//}
}

void TerrainClass::ResetTerrainVelocityAfterCollision() {
	for (int i = 0; i < terrainChuncksId.size(); i++) {
		terrainPhysicsNodes.at(i)->SetLinearVelocity(Vector3(0, 0, linearVelocityZ));
	}
}

float TerrainClass::GetAmountMovementOnZAxis() {
	//recalculate the amount of movement made by the player on the terrain
	Vector3 amountMovement = terrainPhysicsNodes.at(0)->GetLinearVelocity();
	amountMovementZ = amountMovementZ + amountMovement.getZ();
	return amountMovementZ;
}

bool TerrainClass::GetActiveVoxel(Vector3 position) {
	return terrain->getVoxelActive(position);
}

void TerrainClass::SetMovementTerrain(Vector3 position) {
	for (int i = 0; i < terrainChuncksId.size(); i++) {
		terrainPhysicsNodes.at(i)->SetPosition(position);

		//disable applying linear velocity this frame
		terrainPhysicsNodes.at(i)->SetLinearVelocity(Vector3(0, 0, 0));
	}
}

Vector3 TerrainClass::GetTerrainPositionAt(int i) {
	return terrainPhysicsNodes.at(i)->GetPosition();
}

void TerrainClass::UpdatePhysicsTerrain(float msec) {
	//update the physics nodes
	for (int i = 0; i < terrainPhysicsNodes.size(); i++) {
		terrainPhysicsNodes.at(i)->Update(msec);
	}
}

void TerrainClass::BuildFloorEntity() {
	for (int i = 0; i < ChunckManager::NUM_VOXEL_CHUNCKS; i++) {
		SceneNode* sNode = new SceneNode();
		sNode->SetMesh(terrain->getMeshAt(i));
		sNode->SetModelScale(Vector3(SCALE_TERRAIN, SCALE_TERRAIN, SCALE_TERRAIN));

		PhysicsNode* pNode = new PhysicsNode();
		pNode->SetPosition(START_TERRAIN_POSITION);
		pNode->SetLinearVelocity(Vector3(0, 0, linearVelocityZ));
		pNode->setLD(false);
		pNode->setSkyBox(true);
		terrainPhysicsNodes.push_back(pNode);
		countTerrainEntities++;

		GameEntity* geFirst = new GameEntity(sNode, pNode);
		geFirst->ConnectToSystems();
		terrainEntities.push_back(geFirst);

		//add the id of the chunck just created
		//terrainChuncksId.push_back(geFirst->getId());
		//reset the chunck id to be the same as the entity added; for consistency purposes
		//terrain->getChunckAt(i)->setId(i);
	}
}

float TerrainClass::getHeightAtPosition(Vector3 position) {
	return terrain->getHeightAtPosition(position, SCALE_TERRAIN);
}
