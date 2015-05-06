#pragma once
#include "SceneNode.h"
#include "CollisionVolume.h"
#include "Quaternion.h"
#include <vectormath/cpp/vectormath_aos.h>

#define LINEAR_VELOCITY_DAMP 0.998
#define ANGULAR_VELOCITY_DAMP 1
#define LINEAR_VELOCITY_MIN 0.00001

using namespace Vectormath::Aos;
class PhysicsNode
{
public:
	PhysicsNode(void);
	PhysicsNode(Quat orientation, Vector3 position);
	~PhysicsNode(void);

	Vector3		GetPosition() const			{ return m_position; }
	Vector3		GetLinearVelocity() const	{ return m_linearVelocity; }

	float		GetInverseMass() const		{ return m_invMass; }
	float		GetMass() const				{ return 1.0f / m_invMass; }

	Matrix4		GetInverseInertia() const	{ return m_invInertia; }

	
	Quat	GetOrientation() const		{ return m_orientation; }
	Vector3		GetAngularVelocity() const	{ return m_angularVelocity; }

	//Quaternion	GetOrientation() const		{ return m_orientation; }
	CollisionVolume* GetCollisionVolume() const { return vol; }

	Matrix4		BuildTransform(Vector3 pos);

	virtual void Update(float msec);

	void	SetPosition(Vector3 vec) { m_position = vec; }
	void	SetLinearVelocity(Vector3 vec)		{ m_linearVelocity = vec; }

	void	SetMass(float mass) { m_invMass = 1.0f / mass; }
	void	SetInverseMass(float invMass) { m_invMass = invMass; }

	void	SetInverseInertia(Matrix4 invInertia) { m_invInertia = invInertia; }

	/*void	SetOrientation(Quaternion q) { m_orientation = q; }*/
	void	SetOrientation(Quat q) { m_orientation = q; }
	void	SetAngularVelocity(Vector3 vec) { m_angularVelocity = vec; }

	void	SetTarget(SceneNode *s) { target = s; }
	void	SetUseGravity(bool value) { useGravity = value; }
	void	SetCollisionVolume(CollisionVolume* vol) { this->vol = vol; }

	void setsize(float s){ size = s; }
	Vector3	GetForce()	{ return m_force; }
	Vector3	GetTorque() { return m_torque; }

	void AddForce(Vector3 point, Vector3 force);

	static const Vector3 gravity;

	float GetZBegin()const { return zBegin; }
	float GetZEnd()const { return zEnd; };

	void setSkyBox(bool set){ skybox = set; }
	bool getSkyBox(){ return skybox; }

	void setLD(bool set){ linearDamp = set; }
	void SetUpdate(bool set){ update = set; }
	void SetToDespawn(bool set){ despawn = set; }
	bool isToDespawn() { return despawn; }

protected:
	bool useGravity;

	//<---------LINEAR-------------->
	Vector3		m_position;
	Vector3		m_linearVelocity;
	Vector3		m_force;
	float		m_invMass;

	//<----------ANGULAR--------------->
	//Quaternion  m_orientation;
	Quat m_orientation;
	Vector3		m_angularVelocity;
	Vector3		m_torque;
	Matrix4     m_invInertia;

	float zBegin;
	float zEnd;
	float size;

	SceneNode*	target;
	CollisionVolume* vol;

	bool linearDamp;
	bool skybox;
	bool update;
	bool despawn;
};



