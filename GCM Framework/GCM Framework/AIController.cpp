#pragma once
#include "AIController.h"

using namespace std;

AIController::AIController()
{
	currentGameTimer = 0.0f;
	gooodTickRate = 3000.0f;
	maliciousTickRate = 2000.0f;
	difficulty = 1;
}

AIController::~AIController()
{

}


void AIController::Update(float gameTimer)
{
	currentGameTimer = gameTimer;
}

void AIController::onNotify(PlayerClass* entity)
{
	/*
		if the entity player has high health, AND is doing well AND difficulty is not at the highest
			++ increase the difficulty
				-> decrease spawn cooldown of malicious objects
				-> increase spawn cooldown of good objects
		if the player has low health, AND is not doing well, AND difficulty is not at the lowest,
			-- decrease the difficulty
				-> increase spawn cooldown of malicious objects
				-> decrease spawn cooldown of good objects
	*/
	if (difficulty < 3&& entity->getHealth() > 50 )
	{
		++difficulty;
		gooodTickRate += 1500.0f;
		maliciousTickRate -= 500.0f;
	}
	else if (difficulty > 1 &&  entity->getHealth() < 40)
	{
		--difficulty;
		gooodTickRate -= 1500.0f;
		maliciousTickRate += 500.0f;
	}
}

/*  
	State Definition:
		- Easy
			*	Highest spawn rate of good items = every 3.0s  
			*	Lowest spawn rate of malicious objects = every 2.5s
					-> Highest % chance of simple blocking objects = 85 %
					-> Low % chance of slowing objects = 15 %

		- Medium
			*	Normal spawn rate of good items = every 4.5s  
			*	Normal spawn rate of malicious objects = every 2.0s
					-> Normal % chance of  simple blocking objects = 75 %
					-> Normal % chance of slowing objects = 25 %

		- Hard
			*	Lowest spawn rate of good items = every 6.0s  
			*	Highest spawn rate of malicious objects = every 1.5s
					-> Normal % chance of  simple blocking objects = 65 %
					-> Highest % chance of slowing objects = 35 %
*/

void AIController::Reset()
{
	currentGameTimer = 0.0f;
	gooodTickRate = 3000.0f;
	maliciousTickRate = 2000.0f;
	difficulty = 1;
}