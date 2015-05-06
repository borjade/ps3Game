#include "Game.h"
#include "InverseInertiaHelper.h"
int count = 0;

Game::Game()
{
	//create our game assets here
	cube = Mesh::GenerateCube(1.0f);
	cube2 = Mesh::GenerateCube(1.0f);
	//cube3 = Mesh::GenerateCube(30.0f);
	cube->SetDefaultTexture(*GCMRenderer::LoadGTF("/GRASS.gtf"));
	cube2->SetDefaultTexture(*GCMRenderer::LoadGTF("/Stars.gtf"));
	
	controller = new AIController();
	/*allEntities.push_back(BuildCubeEntity(Vector3(200, 20, 0), cube2,false,20.0f));*/
	allEntities.push_back(BuildCubeEntity(Vector3(0, 0, 0), cube2, false, 7500, true));
	deathTimer = 0.0f;
	timer = 0.0f;
	timer2 = 0.0f;
	buffTimer = 0.0f;
	debuffTimer = 0.0f;
	player1 = new PlayerClass(JOYPAD_A);
	allEntities.push_back(player1->getEntity());
	spawnPoint = new Spawner(Vector3(0, 0, -3500), 200);
	player1->getSubject()->addObserver(controller);
	PhysicsSystem::GetPhysicsSystem().addPlayerRef(player1);
	/*mutex = new sys_mutex_t();
	attr = new sys_mutex_attribute_t();
	sys_mutex_create(mutex, attr);*/
}


Game::~Game()
{
	//destroy our game assets here
	delete spawnPoint;
	delete player1;
	delete controller;
}

void Game::UpdateGame(float msec)
{
	//for (std::vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
	//	(*i)->Update(msec);
	//}
	
	for (int i = 0; i < allEntities.size(); i) {
		allEntities.at(i)->Update(msec);
		if (allEntities.at(i)->GetPhysicsNode().isToDespawn())
		{
			spawnPoint->despawn(allEntities.at(i));
			allEntities.erase(allEntities.begin() + i);
		}
		else
		{
			++i;
		}
	}
	
	timer += msec;
	buffTimer += msec;
	debuffTimer += msec;
	if (buffTimer >= controller->getCurrentGoodTickRate())
	{
		allEntities.push_back(spawnPoint->spawn(1));
		buffTimer = 0.0f;
	}

	if (debuffTimer >= controller->getCurrentMaliciousTickRate())
	{
		allEntities.push_back(spawnPoint->spawn(2));
		debuffTimer = 0.0f;
	}

	if (deathTimer >= 1000)
	{
		deathTimer = 0.0f;
		player1->UpdateHealth(-1);
	}

	if (timer2 <= 2000)
	{
		timer2 += msec;
	}
	else
	{
		player1->getEntity()->GetPhysicsNode().SetUseGravity(true);
	}
	
	player1->Update(msec);
	if (timer >= 800 ) 
	{
		timer = 0.0f;
		allEntities.push_back(spawnPoint->spawnTerrain());
		count++;
		std::cout << "Count: " << count<< std::endl;

	}
	
	player1->getSubject()->notify(player1);
	
}

GameEntity* Game::BuildCubeEntity(Vector3 Pos, Mesh* mesh, bool grav, float size, bool skybox)
{
	PhysicsNode* physNode = new PhysicsNode(Quat(0), Pos);
	/*Quat(0), Pos*/
	SceneNode* sceneNode = new SceneNode();
	sceneNode->SetTransform(Matrix4::identity());
	sceneNode->SetModelScale(Vector3(size, size, size));
	sceneNode->SetColour(0xff0000ff);
	physNode->SetUseGravity(grav);
	physNode->SetCollisionVolume(new CollisionOBB(Vector3(size, size, size), ELSE));
	physNode->SetLinearVelocity(Vector3(0.1f,0,0.0f));
	physNode->SetAngularVelocity(Vector3(0.0f, 0.1f, 0.0f));
	physNode->SetPosition(Pos);
	physNode->setsize(size);
	physNode->SetInverseMass(1.0f);
	physNode->SetInverseInertia(InertialMatrixHelper::createCuboidInvInertial(1.0f, size, size, size));
	physNode->setSkyBox(skybox); 
	sceneNode->SetMesh(mesh);
	sceneNode->SetBoundingRadius(size);
	
	GameEntity* g = new GameEntity(sceneNode,physNode);
	g->ConnectToSystems();
	return g;
}


GameEntity* Game::BuildFloorEntity(Vector3 Pos)
{
	SceneNode* s = new SceneNode();

	s->SetMesh(quad);
	s->SetBoundingRadius(1000);
	s->SetModelScale(Vector3(1,1,1));
	s->SetColour(0xff0000ff);
	PhysicsNode*p = new PhysicsNode(Quat::rotation(1.57079633, Vector3(1, 0, 0)), Vector3(0, 0, 0));
	p->SetUseGravity(false);
	p->SetAngularVelocity(Vector3(0, 0.0f, 0.1f));
	p->SetInverseMass(0.0f);
	p->SetInverseInertia(InertialMatrixHelper::createImmovableInvInertial());
	p->SetCollisionVolume(new CollisionPlane(Vector3(0, 1, 0), 0));
	p->setSkyBox(false);
	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}
