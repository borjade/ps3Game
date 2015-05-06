#pragma once
#include "GameEntity.h"
#include "Input.h"
#include "CameraNode.h"
#include "Subject.h"

class PlayerClass 
{
public:
	PlayerClass(JoyPadNum controllingPad);
	~PlayerClass();

	float getHealth() { return health; }
	GameEntity* getEntity(){ return PlayerEntity; }
	virtual void	Update(float msec);
	void UpdateHealth(float i){ health += i; }
	Subject* getSubject() { return playerSubject; }
protected:
	JoyPadNum	pad;
	GameEntity* PlayerEntity;
	float health;
	Mesh* playerMesh;
	float xMovement;
	Subject * playerSubject;
	
};

