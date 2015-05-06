#pragma once
#include "GameClass.h"
#include "Spawner.h"
#include "PlayerClass.h"
#include "AIController.h"
#include <sys\synchronization.h>
class Game :
	public GameClass
{
public:
	Game();
	~Game(void);
	virtual void UpdateGame(float msec);

protected:

	sys_mutex_t* mutex;
	sys_mutex_attribute_t*attr;
	//put all stuff needed for this game here
	GameEntity* BuildCubeEntity(Vector3 Pos, Mesh* mesh, bool grav, float size, bool skybox);
	GameEntity* BuildFloorEntity(Vector3 Pos);
	Mesh* cube;
	Mesh* cube2;
	Mesh* cube3;
	Mesh* quad;

	Spawner* spawnPoint;

	AIController* controller;
	
	PlayerClass * player1;
	PlayerClass * player2;
	float timer;
	float timer2;
	float buffTimer;
	float debuffTimer;
	float deathTimer;
	bool cooldownOn;
};

