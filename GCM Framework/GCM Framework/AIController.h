#pragma once
/*	Initial Author: Chris Fraser
 *	Refactored By: ||Borja De La Viuda||
 *	Description: Simple Controller object to regulate spawn times of objects according to player performance
 */

#include "Observer.h"
#include "PlayerClass.h"

class AIController : public Observer
{
public:
	AIController();
	~AIController();
	
	virtual void onNotify(PlayerClass* entity);

	void Update(float gameTimer);

	inline float getCurrentMaliciousTickRate(){ return maliciousTickRate; };
	inline float getCurrentGoodTickRate(){ return gooodTickRate; };

	void Reset();
	
protected:

	int difficulty;
	float maliciousTickRate;
	float gooodTickRate;
	float currentGameTimer;

	
};