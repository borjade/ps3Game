#include "GameClass.h"


GameClass* GameClass::instance = NULL;

GameClass::GameClass()	{
	renderCounter = 0.0f;
	physicsCounter = 0.0f;

	instance = this;
}

GameClass::~GameClass(void)	{
	for (std::vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
		delete (*i);
	}
	//delete gameCamera;
}

void GameClass::UpdatePhysics(float msec) {
	physicsCounter += msec;


	while (physicsCounter >= 0.0f) {

		physicsCounter -= PHYSICS_TIMESTEP;
		PhysicsSystem::GetPhysicsSystem().Update(PHYSICS_TIMESTEP);
	}
}

void GameClass::UpdateRendering(float msec) {
	renderCounter -= msec;


	if (renderCounter <= 0.0f) {	//Update our rendering logic
		Renderer::GetRenderer().UpdateScene(1000.0f / (float)RENDER_HZ);
		Renderer::GetRenderer().RenderScene();
		renderCounter += (1000.0f / (float)RENDER_HZ);
	}
}