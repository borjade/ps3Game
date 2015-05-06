#include "PlayerClass.h"
#include "Renderer.h"
#include "InverseInertiaHelper.h"
#include "Input.h"

PlayerClass::PlayerClass(JoyPadNum PadNum) : pad(PadNum)
{
	playerSubject = new Subject();
	health = 100;
	playerMesh = Mesh::GenerateCube(1.0f);
	playerMesh->SetDefaultTexture(*GCMRenderer::LoadGTF("/playerTex.gtf"));

	SceneNode*renderNode = new SceneNode();
	
	renderNode->SetMesh(playerMesh);
	renderNode->SetColour(0xff0000ff);
	renderNode->SetModelScale(Vector3(25, 25, 25));

	PhysicsNode* physicsNode = new PhysicsNode(Quat(0), Vector3(0, 0, 4500));
	physicsNode->SetPosition(Vector3(0, 700, 0));
	physicsNode->SetInverseMass(1.0f);
	physicsNode->SetInverseInertia(InertialMatrixHelper::createCuboidInvInertial(100, 25, 25, 25));
	physicsNode->SetUseGravity(false);
	physicsNode->SetUpdate(true);
	physicsNode->SetLinearVelocity(Vector3(0, 0, 0.0f));
	physicsNode->SetAngularVelocity(Vector3(0, 0.0f, 0.1f));
	physicsNode->setsize(25);
	physicsNode->setSkyBox(false);
	physicsNode->SetCollisionVolume(new CollisionOBB(Vector3(25, 25, 25), PLAYER));
	physicsNode->setLD(true);

	renderNode->SetTransform(Matrix4::identity());

	xMovement = 0.0f;
	Renderer::GetRenderer().getCamera()->SetPitch(-15.5);
	PlayerEntity = new GameEntity(renderNode, physicsNode);
	PlayerEntity->ConnectToSystems();
}


PlayerClass::~PlayerClass()
{

	delete PlayerEntity;
}

void PlayerClass::Update(float msec)
{	
	float x, p;

	Input::GetJoypadMovement(x, p, pad);
	xMovement += x;
	
	if (Input::ButtonDown(INPUT_CROSS, pad))
	{
		PlayerEntity->GetPhysicsNode().SetLinearVelocity(Vector3(PlayerEntity->GetPhysicsNode().GetLinearVelocity().getY(),  3, PlayerEntity->GetPhysicsNode().GetLinearVelocity().getZ()));
	}
	PlayerEntity->GetPhysicsNode().SetLinearVelocity(Vector3(xMovement/80000, PlayerEntity->GetPhysicsNode().GetLinearVelocity().getY(), PlayerEntity->GetPhysicsNode().GetLinearVelocity().getZ()));
	Vector3 pos = PlayerEntity->GetPhysicsNode().GetPosition();
	Renderer::GetRenderer().getCamera()->SetPosition(Vector3(pos.getX(), pos.getY() + 300.f, pos.getZ() + 600.f));

	if (PlayerEntity->GetPhysicsNode().GetPosition().getY() <= -1000)
	{
		PlayerEntity->GetPhysicsNode().SetLinearVelocity(Vector3(0, 0, 0));
		PlayerEntity->GetPhysicsNode().SetUseGravity(false);
	}

	if (PlayerEntity->GetPhysicsNode().GetPosition().getX() <= -750)
	{
		PlayerEntity->GetPhysicsNode().SetPosition(Vector3(-750, PlayerEntity->GetPhysicsNode().GetPosition().getY(), PlayerEntity->GetPhysicsNode().GetPosition().getZ()));
	}

	if (PlayerEntity->GetPhysicsNode().GetPosition().getX() >= 750)
	{
		PlayerEntity->GetPhysicsNode().SetPosition(Vector3(750, PlayerEntity->GetPhysicsNode().GetPosition().getY(), PlayerEntity->GetPhysicsNode().GetPosition().getZ()));
	}

}