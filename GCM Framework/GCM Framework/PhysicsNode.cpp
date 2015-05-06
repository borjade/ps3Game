#include "PhysicsNode.h"
#include "VectorAid.h"
const Vector3 PhysicsNode::gravity = Vector3(0, -0.01, 0);

PhysicsNode::PhysicsNode(void) : vol(NULL) {
	target = NULL;
	m_orientation = Quat(0,0,0,1);
	m_position =  Vector3(0);
	useGravity = true;
	m_force = Vector3(0, 0, 0);
	m_torque = Vector3(0, 0, 0);
	despawn = false;
}

PhysicsNode::PhysicsNode(Quat orientation, Vector3 position) : vol(NULL) {
	m_orientation = orientation;
	m_position =  Vector3(position);
	useGravity = true;
	m_force = Vector3(0, 0, 0);
	m_torque = Vector3(0, 0, 0);
	despawn = false;
}


PhysicsNode::~PhysicsNode()
{
}


void	PhysicsNode::Update(float msec) {
	

	if (update) {
		Vector3 acc = m_force*m_invMass + (useGravity ? gravity : Vector3(0, 0, 0));
		m_linearVelocity = m_linearVelocity + acc*msec;

		m_linearVelocity = m_linearVelocity * (linearDamp ? LINEAR_VELOCITY_DAMP : 1);

	

		m_position = m_position +m_linearVelocity*msec;
		
	/*	if (m_position.getY() < 0)
		{
			m_position.setY(300);
		}*/

		/*Vector3 angAcc = (m_invInertia*m_torque).getXYZ();*/
		Vector3 angAcc = m_invInertia.getUpper3x3()*m_torque;
		m_angularVelocity = m_angularVelocity + angAcc*msec;
		m_angularVelocity = m_angularVelocity*ANGULAR_VELOCITY_DAMP;
		
		m_orientation = m_orientation + m_orientation * Quat(m_angularVelocity*(msec / 2.0f),1.0f);
	 //m_orientation = Vectormath::Aos::normalize(m_orientation);

		

		m_force = Vector3(0, 0, 0);
		m_torque = Vector3(0, 0, 0);
	}
	
	Matrix4 temp = this->BuildTransform(m_position);
	if (target) {
		target->SetTransform(temp); 
	}

	//update the the value for projection
	zBegin = m_position.getZ() - target->GetBoundingRadius();
	zEnd = m_position.getZ() + target->GetBoundingRadius();
}

void PhysicsNode::AddForce(Vector3 point, Vector3 force) {
	m_force = m_force + force;
	m_torque = m_torque + Vectormath::Aos::cross(point - m_position, force);
}

Matrix4		PhysicsNode::BuildTransform(Vector3 pos) { 
	Matrix4 m = Matrix4::identity();
	m = Matrix4::rotation(m_orientation);
	m.setElem(3, 0, pos.getX());
	m.setElem(3, 1, pos.getY());
	m.setElem(3, 2, pos.getZ());
	return m;
}