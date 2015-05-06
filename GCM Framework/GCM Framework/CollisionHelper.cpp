#include "CollisionHelper.h"

bool CollisionHelper::SphereSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data) {
	CollisionSphere& s0 = *(CollisionSphere*)p0.GetCollisionVolume();
	CollisionSphere& s1 = *(CollisionSphere*)p1.GetCollisionVolume();

	Vector3 normal = p0.GetPosition() - p1.GetPosition();
	const float distSq = LengthSq(normal);
	const float sumRadius = s0.GetRadius() + s1.GetRadius();

	if (distSq < sumRadius*sumRadius) {
		if (data) {
			data->m_penetration = sumRadius - sqrtf(distSq);
			Vectormath::Aos::normalize(normal);
			data->m_normal = normal;
			data->m_point = p0.GetPosition() - normal*(s0.GetRadius() - data->m_penetration*0.5f);
		}
		return true;
	}
	return false;
}

bool CollisionHelper::PlaneSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data) {
	CollisionPlane& plane = *(CollisionPlane*)p0.GetCollisionVolume();
	CollisionSphere& sphere = *(CollisionSphere*)p1.GetCollisionVolume();

	float separation = Vectormath::Aos::dot(p1.GetPosition(), plane.GetNormal()) - plane.GetDistance();

	if (separation > sphere.GetRadius()) {
		return false;
	}
	if (data) {
		data->m_penetration = sphere.GetRadius() - separation;
		data->m_normal = -plane.GetNormal();
		data->m_point = p1.GetPosition() - plane.GetNormal()*separation;
	}

	return true;
}


bool CollisionHelper::AABBCollision(PhysicsNode& p0, PhysicsNode& p1) {
	CollisionAABB& aabb0 = *(CollisionAABB*)p0.GetCollisionVolume();
	CollisionAABB& aabb1 = *(CollisionAABB*)p1.GetCollisionVolume();

	float dist = abs(p0.GetPosition().getX() - p1.GetPosition().getX());
	float sum = aabb0.getHalfDimensions().getX() + aabb1.getHalfDimensions().getX();
	if (dist <= sum) {
		dist = abs(p0.GetPosition().getY() - p1.GetPosition().getY());
		sum = aabb0.getHalfDimensions().getY() + aabb1.getHalfDimensions().getY();
		if (dist <= sum) {
			dist = abs(p0.GetPosition().getZ() - p1.GetPosition().getZ());
			sum = aabb0.getHalfDimensions().getZ() + aabb1.getHalfDimensions().getZ();
			if (dist <= sum) {
				return true;
			}
		}
	}
	return false;
}


/*Author: Chris Fraser
Modified by: -------
Created 14/02/2015
Using the Separating Axis Theorem, we use this to check for Oriented Bounding Box Collisions*/
bool CollisionHelper::OBBCollision(PhysicsNode& OBBA, PhysicsNode& OBBB, float test) 
{
	CollisionOBB& obb1 = *(CollisionOBB*)OBBA.GetCollisionVolume();
	CollisionOBB& obb2 = *(CollisionOBB*)OBBB.GetCollisionVolume();
	//all of our collisions are cubes, so what we need is the bounding radius for each cube as that will be width height and depth
	//if we choose to change that, then we need to adjust the code accordingly

	//<----- CUBE A PROPERTIES ----->
	Vector3 centreA = OBBA.GetPosition(); //Centre of our cube
	Vector3 AX = (Matrix4::rotation(OBBA.GetOrientation())*Vector4(1.0f, 0.0f, 0.0f, 1.0f)).getXYZ(); //Initial local x-axis of cubeA
	Vector3 AY = (Matrix4::rotation(OBBA.GetOrientation())*Vector4(0.0f, 1.0f, 0.0f, 1.0f)).getXYZ(); //Initial local y-axis of cubeA
	Vector3 AZ = (Matrix4::rotation(OBBA.GetOrientation())*Vector4(0.0f, 0.0f, 1.0f, 1.0f)).getXYZ(); //Initial local z-axis of cubeA

	//Vector4 DimCalcsA = Matrix4::rotation(OBBA.GetOrientation())*Vector4(obb1.getHalfDimensions().x, obb1.getHalfDimensions().y, obb1.getHalfDimensions().z, 1.0f);
	float Wa = obb1.getHalfDimensions().getX();// abs(DimCalcsA.x); //half width in local x-axis of A (DOUBLE CHECK THESE LATER, THEY COULD BE VERY WRONG!!!)
	float Ha = obb1.getHalfDimensions().getY();// abs(DimCalcsA.y); //half width in local y-axis of A
	float Da = obb1.getHalfDimensions().getZ();// abs(DimCalcsA.z); //half width in local z-axis of A

	//<----- CUBE B PROPERTIES ----->
	Vector3 centreB = OBBB.GetPosition(); //Centre of our cube
	Vector3 BX = (Matrix4::rotation(OBBB.GetOrientation())*Vector4(1.0f, 0.0f, 0.0f, 1.0f)).getXYZ(); //Initial local x-axis of cubeB
	Vector3 BY = (Matrix4::rotation(OBBB.GetOrientation())*Vector4(0.0f, 1.0f, 0.0f, 1.0f)).getXYZ(); //Initial local y-axis of cubeB
	Vector3 BZ = (Matrix4::rotation(OBBB.GetOrientation())*Vector4(0.0f, 0.0f, 1.0f, 1.0f)).getXYZ(); //Initial local z-axis of cubeB

	//Vector4 DimCalcsB = Matrix4::rotation(OBBB.GetOrientation())*Vector4(obb2.getHalfDimensions().x, obb2.getHalfDimensions().y, obb2.getHalfDimensions().z, 1.0f);
	float Wb = obb2.getHalfDimensions().getX();//abs(DimCalcsB.x); //half width in local x-axis of B
	float Hb = obb2.getHalfDimensions().getY();//abs(DimCalcsB.y); //half width in local y-axis of B
	float Db = obb2.getHalfDimensions().getZ();//abs(DimCalcsB.z); //half width in local z-axis of B

	//Here begins the mother of all checks, a nested if statement containing all 15 cases
	//for more info on the cases refer to the following paper: http://www.jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf
	//page 28 begins all the cases, with one important caveat, for the purpose of the checks the inequality sign is flipped
	//This is because it is testing to see if there is a separating axis, whereas here we are checking to see if they aren't there

	//<----- NEATENING VARIABLES -----> This will tidy up our checks making them more readable
	Vector3 T = centreB - centreA; //Difference between cube centres
	Vectormath::Aos::normalize(AX); Vectormath::Aos::normalize(AY);  Vectormath::Aos::normalize(AZ); //Normalising our local axis coordinates before we do anything else otherwise they don't work
	Vectormath::Aos::normalize(BX);  Vectormath::Aos::normalize(BY);  Vectormath::Aos::normalize(BZ);
	//T.Normalise();

	//Case 1: L=Ax
	if (abs(Vectormath::Aos::dot(T, AX)) <= (Wa + Wb*abs(Vectormath::Aos::dot(AX, BX)) + Hb*abs(Vectormath::Aos::dot(AX, BY)) + Db*abs(Vectormath::Aos::dot(AX, BZ)))) //Double checked to be correct
	{
		//Case 2: L=Ay
		if (abs(Vectormath::Aos::dot(T, AY)) <= (Ha + Wb*abs(Vectormath::Aos::dot(AY, BX)) + Hb*abs(Vectormath::Aos::dot(AY, BY)) + Db*abs(Vectormath::Aos::dot(AY, BZ))))
		{
			//Case 3: L=Az
			if (abs(Vectormath::Aos::dot(T, AZ)) <= (Da + Wb*abs(Vectormath::Aos::dot(AZ, BX)) + Hb*abs(Vectormath::Aos::dot(AZ, BY)) + Db*abs(Vectormath::Aos::dot(AZ, BZ))))
			{
				//Case 4: L=Bx
				if (abs(Vectormath::Aos::dot(T, BX)) <= (Wb + Wa*abs(Vectormath::Aos::dot(AX, BX)) + Ha*abs(Vectormath::Aos::dot(AY, BX)) + Da*abs(Vectormath::Aos::dot(AZ, BX))))
				{
					//Case 5: L=By
					if (abs(Vectormath::Aos::dot(T, BY)) <= (Hb + Wa*abs(Vectormath::Aos::dot(AX, BY)) + Ha*abs(Vectormath::Aos::dot(AY, BY)) + Da*abs(Vectormath::Aos::dot(AZ, BY))))
					{
						//Case 6: L=Bz
						if (abs(Vectormath::Aos::dot(T, BZ)) <= (Db + Wa*abs(Vectormath::Aos::dot(AX, BZ)) + Ha*abs(Vectormath::Aos::dot(AY, BZ)) + Da*abs(Vectormath::Aos::dot(AZ, BZ))))
						{
							//Case 7: L=Ax X Bx
							if (abs((Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AY, BX)) - (Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AZ, BX))) <= (Ha*abs(Vectormath::Aos::dot(AZ, BX)) + Da*abs(Vectormath::Aos::dot(AY, BX)) + Hb*abs(Vectormath::Aos::dot(AX, BZ)) + Db*abs( Vectormath::Aos::dot(AX, BY))))
							{
								//Case 8: L=Ax X By
								if (abs(Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AY, BY) - Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AZ, BY)) <= (abs(Ha* Vectormath::Aos::dot(AZ, BY)) + abs(Da* Vectormath::Aos::dot(AY, BY)) + abs(Wb* Vectormath::Aos::dot(AX, BZ)) + abs(Db* Vectormath::Aos::dot(AX, BX))))
								{
									//Case 9: L=Ax X Bz
									if (abs(Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AY, BZ) - Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AZ, BZ)) <= (abs(Ha* Vectormath::Aos::dot(AZ, BZ)) + abs(Da* Vectormath::Aos::dot(AY, BZ)) + abs(Wb* Vectormath::Aos::dot(AX, BY)) + abs(Hb* Vectormath::Aos::dot(AX, BX))))
									{
										//Case 10: L=Ay X Bx
										if (abs( Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AZ, BX) -  Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AX, BX)) <= (abs(Wa* Vectormath::Aos::dot(AZ, BX)) + abs(Da* Vectormath::Aos::dot(AX, BX)) + abs(Hb* Vectormath::Aos::dot(AY, BZ)) + abs(Db* Vectormath::Aos::dot(AY, BY))))
										{
											//Case 11: L=Ay X By
											if (abs( Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AZ, BY) -  Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AX, BY)) <= (abs(Wa* Vectormath::Aos::dot(AZ, BY)) + abs(Da* Vectormath::Aos::dot(AX, BY)) + abs(Wb* Vectormath::Aos::dot(AY, BZ)) + abs(Db* Vectormath::Aos::dot(AY, BX))))
											{
												//Case 12: L=Ay X Bz
												if (abs( Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AZ, BZ) -  Vectormath::Aos::dot(T, AZ)* Vectormath::Aos::dot(AX, BZ)) <= (abs(Wa* Vectormath::Aos::dot(AZ, BZ)) + abs(Da* Vectormath::Aos::dot(AX, BZ)) + abs(Wb* Vectormath::Aos::dot(AY, BY)) + abs(Hb* Vectormath::Aos::dot(AY, BX))))
												{
													//Case 13: L=Az X Bx
													if (abs( Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AX, BX) -  Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AY, BX)) <= (abs(Wa* Vectormath::Aos::dot(AY, BX)) + abs(Ha* Vectormath::Aos::dot(AX, BX)) + abs(Hb* Vectormath::Aos::dot(AZ, BZ)) + abs(Db* Vectormath::Aos::dot(AZ, BY))))
													{
														//Case 14: L=Az X By
														if (abs( Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AX, BY) -  Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AY, BY)) <= (abs(Wa* Vectormath::Aos::dot(AY, BY)) + abs(Ha* Vectormath::Aos::dot(AX, BY)) + abs(Wb* Vectormath::Aos::dot(AZ, BZ)) + abs(Db* Vectormath::Aos::dot(AZ, BX))))
														{
															//Case 15: L=Az X Bz
															if (abs( Vectormath::Aos::dot(T, AY)* Vectormath::Aos::dot(AX, BZ) -  Vectormath::Aos::dot(T, AX)* Vectormath::Aos::dot(AY, BZ)) <= (abs(Wa* Vectormath::Aos::dot(AY, BZ)) + abs(Ha* Vectormath::Aos::dot(AX, BZ)) + abs(Wb* Vectormath::Aos::dot(AZ, BY)) + abs(Hb* Vectormath::Aos::dot(AZ, BX))))
															{
																return true;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

/*Author: Chris Fraser
Modified by: -------
Created 18/02/2015
Using the Separating Axis Theorem, we use this to check for Oriented Bounding Box Collisions with a plane*/
bool CollisionHelper::PlaneOBBCollision(PhysicsNode& OBB, PhysicsNode& Plane)
{
	CollisionOBB& obb = *(CollisionOBB*)OBB.GetCollisionVolume();
	CollisionPlane& plane = *(CollisionPlane*)Plane.GetCollisionVolume();
	//all of our collisions are cubes, so what we need is the bounding radius for each cube as that will be width height and depth
	//if we choose to change that, then we need to adjust the code accordingly

	//<----- CUBE PROPERTIES ----->
	Vector3 centreA = OBB.GetPosition(); //Centre of our cube
	Vector3 AX = (Matrix4::rotation(OBB.GetOrientation())*Vector4(1.0f, 0.0f, 0.0f, 1.0f)).getXYZ(); //Initial local x-axis of cubeA
	Vector3 AY = (Matrix4::rotation(OBB.GetOrientation())*Vector4(0.0f, 1.0f, 0.0f, 1.0f)).getXYZ(); //Initial local y-axis of cubeA
	Vector3 AZ = (Matrix4::rotation(OBB.GetOrientation())*Vector4(0.0f, 0.0f, 1.0f, 1.0f)).getXYZ(); //Initial local z-axis of cubeA

	//Vector4 DimCalcsA = Matrix4::rotation(OBBA.GetOrientation())*Vector4(obb1.getHalfDimensions().x, obb1.getHalfDimensions().y, obb1.getHalfDimensions().z, 1.0f);
	float Wa = obb.getHalfDimensions().getX();// abs(DimCalcsA.x); //half width in local x-axis of A (DOUBLE CHECK THESE LATER, THEY COULD BE VERY WRONG!!!)
	float Ha = obb.getHalfDimensions().getY();// abs(DimCalcsA.y); //half width in local y-axis of A
	float Da = obb.getHalfDimensions().getZ();// abs(DimCalcsA.z); //half width in local z-axis of A

	//<----- PLANE PROPERTIES ----->
	//Vector3 planeCentre = Plane.GetPosition(); //Centre of the plane
	Vector3 planeNormal = plane.GetNormal(); //Normal to the plane

	float separation =  Vectormath::Aos::dot(centreA, planeNormal) - plane.GetDistance();

	if (abs(separation) <= (Wa*abs( Vectormath::Aos::dot(planeNormal, AX)) + Ha*abs( Vectormath::Aos::dot(planeNormal, AY)) + Da*abs( Vectormath::Aos::dot(planeNormal, AZ))))
	{
		return true;
	}
	return false;
}


void CollisionHelper::AddCollisionImpulse(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data) {

	if (p0.GetInverseMass() + p1.GetInverseMass() == 0.0f) return;

	Vector3 r0 = data->m_point -p0.GetPosition();
	Vector3 r1 = data->m_point -p1.GetPosition();
	

	Vector3 v0 = p0.GetLinearVelocity() + Vectormath::Aos::cross(p0.GetAngularVelocity(), r0);
	Vector3 v1 = p1.GetLinearVelocity() + Vectormath::Aos::cross(p1.GetAngularVelocity(), r1);

	Vector3 dv = v0 - v1;

	float relMov = -Vectormath::Aos::dot(dv, data->m_normal);
	if (relMov < -0.01f) return;
	;
	{
		float e = 0.1f;
		float normDiv = (p0.GetInverseMass() + p1.GetInverseMass()) +
			Vectormath::Aos::dot(data->m_normal,
			Vectormath::Aos::cross((p0.GetInverseInertia()*Vectormath::Aos::cross(r0, data->m_normal)).getXYZ(), r0) +
			Vectormath::Aos::cross((p1.GetInverseInertia()*Vectormath::Aos::cross(r1, data->m_normal)).getXYZ(), r1));
		float jn = -1 * (1 + e)* Vectormath::Aos::dot(dv, data->m_normal) / normDiv;

		jn = jn + (data->m_penetration*0.01f);

		Vector3 l0 = p0.GetLinearVelocity() + data->m_normal*(jn*p0.GetInverseMass());
		p0.SetLinearVelocity(l0 / 10);
		Vector3 a0 = p0.GetAngularVelocity() + (p0.GetInverseInertia()* Vectormath::Aos::cross(r0, data->m_normal * jn)).getXYZ();
		p0.SetAngularVelocity(a0);

		Vector3 l1 = p1.GetLinearVelocity() - data->m_normal*(jn*p1.GetInverseMass());
		p1.SetLinearVelocity(l1 / 10);
		Vector3 a1 = p1.GetAngularVelocity() - (p1.GetInverseInertia()*  Vectormath::Aos::cross(r1, data->m_normal * jn)).getXYZ();
		p1.SetAngularVelocity(a1);

	}

	{
		Vector3 tangent = dv - data->m_normal* Vectormath::Aos::dot(dv, data->m_normal);
		Vectormath::Aos::normalize(tangent);
		float tangDiv = (p0.GetInverseMass() + p1.GetInverseMass()) +
			 Vectormath::Aos::dot(tangent,
			 Vectormath::Aos::cross((p0.GetInverseInertia()*  Vectormath::Aos::cross(r0, tangent)).getXYZ(), r0) +
			 Vectormath::Aos::cross((p1.GetInverseInertia()*  Vectormath::Aos::cross(r1, tangent)).getXYZ(), r1));

		float jt = -1 * Vectormath::Aos::dot(dv, tangent) / tangDiv;

		Vector3 l0 = p0.GetLinearVelocity() + tangent*(jt*p0.GetInverseMass());
		p0.SetLinearVelocity(l0 / 10);
		Vector3 a0 = p0.GetAngularVelocity() + (p0.GetInverseInertia()* Vectormath::Aos::cross(r0, tangent * jt)).getXYZ();
		p0.SetAngularVelocity(a0);

		Vector3 l1 = p1.GetLinearVelocity() - tangent*(jt*p1.GetInverseMass());
		p1.SetLinearVelocity(l1/10);
		Vector3 a1 = p1.GetAngularVelocity() - (p1.GetInverseInertia()*  Vectormath::Aos::cross(r1, tangent * jt)).getXYZ();
		p1.SetAngularVelocity(a1);
	}

}