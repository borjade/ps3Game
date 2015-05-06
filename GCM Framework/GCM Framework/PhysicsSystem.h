#pragma once

//#include "Constraint.h"

#include "PhysicsNode.h"
#include <algorithm>
#include <vector>
#include "PlayerClass.h"
#include <cell\sync\mutex.h>
class PhysicsSystem	{
public:
	friend class GameClass;

	void		Update(float msec);

	void		BroadPhaseCollisions(const bool& useCulling = true);
	void		NarrowPhaseCollisions(PhysicsNode& first, PhysicsNode& second);

	//Statics
	static void Initialise() {
		instance = new PhysicsSystem();
	}

	static void Destroy() {
		delete instance;
	}

	static PhysicsSystem& GetPhysicsSystem() {
		return *instance;
	}

	void	AddNode(PhysicsNode* n);
	void	RemoveNode(PhysicsNode* n);
	//void	AddConstraint(Constraint* c);
	//void	RemoveConstraint(Constraint* c);
	//void	AddDebugDraw(DebugDrawer* d);
	//void	RemoveDebugDraw(DebugDrawer* d);
	//void  DrawDebug();
	const float START_POINT = 6000;
	void addPlayerRef(PlayerClass* p){ player = p; }
protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);
	
	CellSyncMutex* mutex;

	PlayerClass* player;
	//Statics
	static PhysicsSystem* instance;
	std::vector<PhysicsNode*> copiedNodes;
	std::vector<PhysicsNode*> allNodes;
	//vector<Constraint*> allSprings;
	//vector<DebugDrawer*> allDebug;

};

