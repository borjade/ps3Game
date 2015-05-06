#pragma once
#include "PhysicsNode.h"
#include "VectorAid.h"
class CollisionHelper {
public:

	static bool SphereSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static bool PlaneSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static bool AABBCollision(PhysicsNode& p0, PhysicsNode& p1);

	static bool OBBCollision(PhysicsNode& OBBA, PhysicsNode& OBBB, float test = 0.1f);

	static bool PlaneOBBCollision(PhysicsNode& OBB, PhysicsNode& Plane);

	static void AddCollisionImpulse(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data);
};

inline float LengthSq(Vector3 v) {
	return  Vectormath::Aos::dot(v, v);
}



