#pragma once

#include "ChunckManager.h"


class TerrainClass {
public: 
	TerrainClass();
	~TerrainClass();

	void UpdateTerrain(float msec);
	void UpdatePhysicsTerrain(float msec);
	//build the floor entities
	void BuildFloorEntity();

	int GetTerrainEntitiesSize() const { return terrainEntities.size(); }

	GameEntity* GetGameEntityAt(int i) { return terrainEntities.at(i); }
	PhysicsNode* GetTerrainPhysicsNodeAt(int i) { return terrainPhysicsNodes.at(i); }

	// ----- Collision detection methods ----- //
	void ResetTerrainVelocityAfterCollision();
	float GetAmountMovementOnZAxis();
	bool GetActiveVoxel(Vector3 position);
	void SetMovementTerrain(Vector3 position);
	Vector3 GetTerrainPositionAt(int i);
	void SetAmountMovementz(float val) { amountMovementZ = val; }
	/// ------------------------------------- ///

	void restartTerrain();
	float getHeightAtPosition(Vector3 position);

	static Vector3 START_TERRAIN_POSITION;
	static float SCALE_TERRAIN;
	static bool RESTART_TERRAIN;
private:
	ChunckManager* terrain;

	std::vector<int> terrainChuncksId;
	std::vector<GameEntity*> terrainEntities;
	std::vector<PhysicsNode*> terrainPhysicsNodes;

	float treshold;
	float amountMovementZ;
	float linearVelocityZ;
	float backupAmountMovementZ;

	int puddleId;
	int countTerrainEntities;

	bool debug;
};