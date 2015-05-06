#pragma once
#include <cstdlib>
#include "EntityFreeList.h"
#include "Mesh.h"
#include "InverseInertiaHelper.h"

class Spawner
{
public:
	Spawner(Vector3 initial, int freeList_space);
	~Spawner();

#pragma region set&get

	void setPosition(Vector3& pos)
	{
		position = pos;
	}

	Vector3 getPosition()
	{
		return position;
	}

#pragma endregion set&get


	GameEntity* spawn(int type);
	GameEntity* spawnTerrain();

	void despawn(GameEntity* obj);

	void flipAcitve(){ active = !active; }
	bool isActive(){ return active; }

	float rangeRandomAlg2(float min, float max){
		float random = ((float)rand()) / (float)RAND_MAX;

		// generate (in your case) a float between 0 and (4.5-.78)
		// then add .78, giving you a float between .78 and 4.5
		float range = max - min;
		return (random*range) + min;
	}

protected:
	Vector3 position;
	EntityFreeList* freeList;
	bool active;

	Mesh* cube;
	Mesh* cube2;
	Mesh* cube3;

};

