#include "PhysicsSystem.h"
#include "CollisionHelper.h"
struct myclass {
	bool operator() (PhysicsNode* i, PhysicsNode *j) { return (i->GetZBegin() < j->GetZBegin()); }
} myobject;


PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)	{
	mutex = new CellSyncMutex();
	if (cellSyncMutexInitialize(mutex) == CELL_OK)
	{
		std::cout << "ALL OK" << std::endl;
	}
	
}

PhysicsSystem::~PhysicsSystem(void)	{
	//for (int i = 0; i < allSprings.size(); i++) {
	//	delete allSprings[i];
	//}
}

void	PhysicsSystem::Update(float msec) {
	
		cellSyncMutexLock(mutex);
		copiedNodes.clear();
		copiedNodes = allNodes;
		cellSyncMutexUnlock(mutex);
		BroadPhaseCollisions(false);
		
		//for (std::vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
		//	(*i)->Update(msec);
		//}
		
		
		for (std::vector<PhysicsNode*>::iterator i = copiedNodes.begin(); i != copiedNodes.end(); ++i) {

			if (copiedNodes.size() != allNodes.size())
				break;
			(*i)->Update(msec);
			if ((*i)->GetPosition().getZ() >= START_POINT || (*i)->GetPosition().getY() <= -600)
			{
				(*i)->SetToDespawn(true);
			}
		}
		
	
}

void	PhysicsSystem::BroadPhaseCollisions(const bool& useCulling)
{
	if (useCulling)
	{
		//sort all the nodes on x axis with the bounding radius
		std::vector<PhysicsNode*> narrowPhaseNodes = std::vector<PhysicsNode*>(allNodes);
		std::sort(narrowPhaseNodes.begin(), narrowPhaseNodes.end(), myobject);

		for (std::vector<PhysicsNode*>::iterator i = narrowPhaseNodes.begin(); i != narrowPhaseNodes.end() - 1; ++i)
		{
			PhysicsNode& first = *(*i);
			CollisionVolume* fv = first.GetCollisionVolume();
			if (!fv) continue;
			for (std::vector<PhysicsNode*>::iterator k = i + 1; k != narrowPhaseNodes.end(); ++k)
			{
				PhysicsNode& second = *(*k);
				CollisionVolume* sv = second.GetCollisionVolume();
				if (!sv) continue;
				//if both node is not unknown type, then go on to sort and sweep.
				if (second.GetZBegin() > first.GetZEnd())
					break;
				NarrowPhaseCollisions(first, second);
			}
		}
	}
	else
	{
		for (int i = 0; i < copiedNodes.size() - 1; i++)
		{
			PhysicsNode& first = *copiedNodes[i];
			CollisionVolume* fv = first.GetCollisionVolume();
			if (!fv) continue;
			for (int j = i + 1; j < allNodes.size(); j++)
			{
				PhysicsNode& second = *copiedNodes[j];
				CollisionVolume* sv = second.GetCollisionVolume();
				if (!sv) continue;
				NarrowPhaseCollisions(first, second);
			}
		}
	}
}


void	PhysicsSystem::NarrowPhaseCollisions(PhysicsNode& first, PhysicsNode& second) {
	CollisionVolume* fv = first.GetCollisionVolume();
	CollisionVolume* sv = second.GetCollisionVolume();
	switch (fv->GetType()) {
	/*case COLLISION_VOL_SPHERE:
		switch (sv->GetType()) {
		case COLLISION_VOL_SPHERE:
			CollisionData data;
			if (CollisionHelper::SphereSphereCollision(first, second, &data)) {
				CollisionHelper::AddCollisionImpulse(first, second, &data);
			}
			return;
		case COLLISION_VOL_PLANE:
			if (CollisionHelper::PlaneSphereCollision(second, first, &data)) {
				CollisionHelper::AddCollisionImpulse(first, second, &data);
			}
			return;
		}*/
	case COLLISION_VOL_PLANE:
		switch (sv->GetType()) {
		case COLLISION_VOL_OBB:
			//CollisionData data;
			if (CollisionHelper::PlaneOBBCollision(second, first))
			{
				
				return;
			}
		}
		//<----- CHECK THE BLOODY CASE WHEN DONE ----->
	case COLLISION_VOL_OBB:
		switch (sv->GetType())
		{
		case COLLISION_VOL_OBB:
			//CollisionData data;
			if (CollisionHelper::AABBCollision(first, second))
			{

				if (first.GetCollisionVolume()->GetSubType() == PLAYER && second.GetCollisionVolume()->GetSubType() == BUFF )
				{
					player->UpdateHealth(5);
					player->getSubject()->notify(player);
					second.SetToDespawn(true);
					return;
				}

				if (first.GetCollisionVolume()->GetSubType() == PLAYER && second.GetCollisionVolume()->GetSubType() == DEBUFF)
				{
					player->UpdateHealth(-5);
					player->getSubject()->notify(player);
					second.SetToDespawn(true);
					return;
				}

				if (first.GetCollisionVolume()->GetSubType() == PLAYER || second.GetCollisionVolume()->GetSubType() == PLAYER)
				{
					if (first.GetCollisionVolume()->GetSubType() == PLAYER)
					{
						first.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, 0));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						first.SetPosition(Vector3(first.GetPosition().getX(), 50, first.GetPosition().getZ()));
					}
					else
					{
						second.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, 0));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						second.SetPosition(Vector3(second.GetPosition().getX(), 50, second.GetPosition().getZ()));
					}
					return;
				}

				if (first.GetCollisionVolume()->GetSubType() == BUFF || second.GetCollisionVolume()->GetSubType() == BUFF)
				{
					if (first.GetCollisionVolume()->GetSubType() == BUFF)
					{
						first.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, first.GetLinearVelocity().getZ()));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						first.SetPosition(Vector3(first.GetPosition().getX(), 50, first.GetPosition().getZ()));
					}
					else
					{
						second.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, first.GetLinearVelocity().getZ()));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						second.SetPosition(Vector3(second.GetPosition().getX(), 50, second.GetPosition().getZ()));
					}
					return;
				}

				if (first.GetCollisionVolume()->GetSubType() == DEBUFF || second.GetCollisionVolume()->GetSubType() == DEBUFF)
				{
					if (first.GetCollisionVolume()->GetSubType() == DEBUFF)
					{
						first.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, first.GetLinearVelocity().getZ()));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						first.SetPosition(Vector3(first.GetPosition().getX(), 50, first.GetPosition().getZ()));
					}
					else
					{
						second.SetLinearVelocity(Vector3(first.GetLinearVelocity().getX(), 0, first.GetLinearVelocity().getZ()));
						float diff = 50 - abs(first.GetPosition().getY() - second.GetPosition().getY());
						second.SetPosition(Vector3(second.GetPosition().getX(), 50, second.GetPosition().getZ()));
					}
					return;
				}

				

				/*if (Vectormath::Aos::dot(second.GetLinearVelocity(), second.GetLinearVelocity()) > 0)
				{
					
					Vector3 T = second.GetPosition() - first.GetPosition();
					T.setY(20.0f);
					second.SetPosition(Vector3(first.GetPosition() + T));
				}
				else if (Vectormath::Aos::dot(first.GetLinearVelocity(), first.GetLinearVelocity()) > 0)
				{
					
					Vector3 T = first.GetPosition() - second.GetPosition();
					T.setY(20.0f);
					first.SetPosition(Vector3(second.GetPosition() + T));
				}*/
				
				
			}
			//continue;
		case COLLISION_VOL_PLANE:
			if (CollisionHelper::PlaneOBBCollision(first, second))
			{
				/*first.SetUseGravity(false);
				second.SetUseGravity(false);*/
			}
			return;
		}
	}


}

void	PhysicsSystem::AddNode(PhysicsNode* n) {
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n) {
	for (std::vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		if ((*i) == n) {
			allNodes.erase(i);
			return;
		}
	}
}

//void	PhysicsSystem::AddConstraint(Constraint* s) {
//	allSprings.push_back(s);
//}
//
//void	PhysicsSystem::RemoveConstraint(Constraint* c) {
//	for (vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
//		if ((*i) == c) {
//			allSprings.erase(i);
//			return;
//		}
//	}
//}
//
//void	PhysicsSystem::AddDebugDraw(DebugDrawer* d) {
//	allDebug.push_back(d);
//}
//
//void	PhysicsSystem::RemoveDebugDraw(DebugDrawer* d) {
//	for (vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
//		if ((*i) == d) {
//			allDebug.erase(i);
//			return;
//		}
//	}
//}
//
//void    PhysicsSystem::DrawDebug() {
//	for (vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
//		(*i)->DebugDraw();
//	}
//}