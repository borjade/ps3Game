#pragma once
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;

enum CollisionVolumeType { COLLISION_VOL_SPHERE, COLLISION_VOL_PLANE, COLLISION_VOL_AABB, COLLISION_VOL_OBB };
enum SubType { PLAYER, BUFF, DEBUFF,TERRAIN, ELSE };

class CollisionVolume {
public:
	virtual SubType GetSubType() const = 0;
	virtual CollisionVolumeType GetType() const = 0;
protected:

};


class CollisionSphere : public CollisionVolume {
public:

	CollisionSphere(float radius) : radius(radius) {}

	CollisionVolumeType GetType() const {
		return COLLISION_VOL_SPHERE;
	}

	SubType GetSubType() const
	{
		return ELSE;
	}

	float GetRadius() const {
		return radius;
	}

private:
	float radius;
};


class CollisionPlane : public CollisionVolume {
public:

	CollisionPlane(Vector3 normal, float distance) : distance(distance), normal(normal) {}

	CollisionVolumeType GetType() const {
		return COLLISION_VOL_PLANE;
	}

	SubType GetSubType() const
	{
		return ELSE;
	}

	Vector3 GetNormal() const {
		return normal;
	}

	float GetDistance() const {
		return distance;
	}

private:
	float distance;
	Vector3 normal;
};

class CollisionAABB : public CollisionVolume {
public:
	CollisionAABB(Vector3 halfDim) : halfDim(halfDim) {}

	CollisionVolumeType GetType() const {
		return COLLISION_VOL_AABB;
	}

	Vector3 getHalfDimensions() const { return halfDim; }

private:
	Vector3 halfDim;
};

//Author: Chris Fraser
class CollisionOBB : public CollisionVolume {
public:
	CollisionOBB(Vector3 halfDim, SubType s) : halfDim(halfDim), subType(s){}

	CollisionVolumeType GetType() const {
		return COLLISION_VOL_OBB;
	}

	SubType GetSubType() const
	{
		return subType;
	}


	Vector3 getHalfDimensions() const { return halfDim; }


private:
	Vector3 halfDim;
	SubType subType;
};


class CollisionData {
public:
	Vector3 m_point;
	Vector3 m_normal;
	float m_penetration;
};