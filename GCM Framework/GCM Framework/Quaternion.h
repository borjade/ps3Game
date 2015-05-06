#pragma once
#pragma once

#include <iostream>
#include "common.h"


#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;
class Quaternion	{
public:
	Quaternion(void);
	Quaternion(float x, float y, float z, float w);

	~Quaternion(void);

	float x;
	float y;
	float z;
	float w;

	void	Normalise();
	Matrix4 ToMatrix() const;


	Quaternion	Conjugate() const;
	void		GenerateW();	//builds 4th component when loading in shortened, 3 component quaternions

	static Quaternion EulerAnglesToQuaternion(float pitch, float yaw, float roll);
	static Quaternion AxisAngleToQuaterion(const Vector3& vector, float degrees);

	static void RotatePointByQuaternion(const Quaternion &q, Vector3 &point);

	static Quaternion FromMatrix(const Matrix4 &m);

	static float Dot(const Quaternion &a, const Quaternion &b);

	Quaternion operator *(const Quaternion &a) const;
	Quaternion operator *(const Vector3 &a) const;

	Quaternion operator+(const Quaternion &a) const {
		return Quaternion(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	inline friend std::ostream& operator<<(std::ostream& o, const Quaternion& q){
		o << "Quat(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")" << std::endl;
		return o;
	}



};

