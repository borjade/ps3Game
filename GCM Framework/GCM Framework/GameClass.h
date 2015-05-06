#pragma once
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "GameEntity.h"
#include "CameraNode.h"
#include <vector>

#define RENDER_HZ	60
#define PHYSICS_HZ	120

#define PHYSICS_TIMESTEP (1000.0f / (float)PHYSICS_HZ)


class GameClass
{
public:
	GameClass();
	~GameClass(void);

	virtual void UpdatePhysics(float msec);
	virtual void UpdateRendering(float msec);

	virtual void UpdateGame(float msec) = 0;

	static GameClass& GetGameClass() { return *instance; }

protected:
	float renderCounter;
	float physicsCounter;
	static GameClass* instance;
	CameraNode* gameCamera;
	std::vector<GameEntity*> allEntities;




	
};

