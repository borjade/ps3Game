#include "Spawner.h"


Spawner::Spawner(Vector3 initial, int freeList_space) : position(initial)
{
	freeList = new EntityFreeList(freeList_space);
	active = false;
	cube = Mesh::GenerateCube(1.0f);
	cube->SetDefaultTexture(*GCMRenderer::LoadGTF("/terrainTex.gtf"));
	cube2 = Mesh::GenerateCube(1.0f);
	cube2->SetDefaultTexture(*GCMRenderer::LoadGTF("/GRASS.gtf"));
	cube3 = Mesh::GenerateCube(1.0f);
	cube3->SetDefaultTexture(*GCMRenderer::LoadGTF("/objectTex.gtf"));
}


Spawner::~Spawner()
{
	delete freeList;
	delete cube;
	delete cube2;
	delete cube3;
}

/* Method used to first retrieve from freelist and then spawn into the game
* Return: Spawned objected connected to physics and renderer
* Parameters: void
* Author: Borja
*/
GameEntity* Spawner::spawn(int type)
{
	
	float displacement = rangeRandomAlg2(-250, 250);
	Vector3 spawnPoint = Vector3(position.getX() +  displacement, position.getY() + 100, position.getZ());


	GameEntity*g = freeList->GetNewObject();
	g->GetRenderNode().SetTransform(Matrix4::translation(spawnPoint));
	
	g->GetRenderNode().SetModelScale(Vector3(50, 50, 50));
	g->GetRenderNode().SetBoundingRadius(100);
	g->GetRenderNode().SetColour(0xff0000ff);
	



	g->GetPhysicsNode().SetPosition(spawnPoint);
	g->GetPhysicsNode().SetInverseMass(1.0f);
	g->GetPhysicsNode().SetInverseInertia(InertialMatrixHelper::createCuboidInvInertial(100, 25, 25, 25));
	g->GetPhysicsNode().SetUseGravity(true);
	g->GetPhysicsNode().SetUpdate(true);
	g->GetPhysicsNode().SetLinearVelocity(Vector3(0, 0, 5.0f));
	g->GetPhysicsNode().SetAngularVelocity(Vector3(0, 0.0f, 0.1f));
	g->GetPhysicsNode().setsize(25);
	g->GetPhysicsNode().setSkyBox(false);
	switch (type)
	{
	case 1:
		g->GetRenderNode().SetMesh(cube2);
		g->GetPhysicsNode().SetCollisionVolume(new CollisionOBB(Vector3(25, 25, 25), BUFF));
		break;
	case 2:
		g->GetRenderNode().SetMesh(cube3);
		g->GetPhysicsNode().SetCollisionVolume(new CollisionOBB(Vector3(25, 25, 25), DEBUFF));
		break;
	}
	
	g->GetPhysicsNode().setLD(false);


	g->ConnectToSystems();
	return g;
}

GameEntity* Spawner::spawnTerrain()
{
	float widthMod = rangeRandomAlg2(100, 300);

	float displacement = rangeRandomAlg2(-350, 350);
	Vector3 spawnPoint = Vector3(position.getX() + displacement, position.getY(), position.getZ());

	GameEntity*g = freeList->GetNewObject();
	g->GetRenderNode().SetTransform(Matrix4::translation(spawnPoint));
	g->GetRenderNode().SetMesh(cube);
	g->GetRenderNode().SetModelScale(Vector3(widthMod, 25, 800));
	g->GetRenderNode().SetBoundingRadius(100);
	g->GetRenderNode().SetColour(0xff0000ff);




	g->GetPhysicsNode().SetPosition(spawnPoint);
	g->GetPhysicsNode().SetInverseMass(1.0f);
	g->GetPhysicsNode().SetInverseInertia(InertialMatrixHelper::createCuboidInvInertial(100, widthMod, 25, 800));
	g->GetPhysicsNode().SetUseGravity(false);
	g->GetPhysicsNode().SetUpdate(true);
	g->GetPhysicsNode().SetLinearVelocity(Vector3(0, 0, 5.0f));
	g->GetPhysicsNode().SetAngularVelocity(Vector3(0, 0.0f, 0.1f));
	g->GetPhysicsNode().setsize(25);
	g->GetPhysicsNode().setSkyBox(false);
	g->GetPhysicsNode().SetCollisionVolume(new CollisionOBB(Vector3(widthMod, 50, 800), TERRAIN));
	g->GetPhysicsNode().setLD(false);

	g->ConnectToSystems();
	return g;
}

void Spawner::despawn(GameEntity* obj)
{
	obj->DisconnectFromSystems();
	freeList->DiscardObject(obj);
}