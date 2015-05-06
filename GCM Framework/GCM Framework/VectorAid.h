#pragma once
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;
class VectorAid
{
public:
	VectorAid();
	~VectorAid();
	static Vector3	Cross(const Vector3 &a, const Vector3 &b) {
		return Vector3((a.getY()*b.getZ()) - (a.getZ()*b.getY()), (a.getZ()*b.getX()) - (a.getX()*b.getZ()), (a.getX()*b.getY()) - (a.getY()*b.getX()));
	};

	static float	Dot(const Vector3 &a, const Vector3 &b) {

		
		return (a.getX()*b.getX()) + (a.getY()*b.getY()) + (a.getZ()*b.getZ());
	};

	static void Normalise(Vector3 &a) {
		float length = Length(a);

		if (length != 0.0f)	{
			length = 1.0f / length;
			a.setX(a.getX() * length);
			a.setY(a.getY() * length);
			a.setZ(a.getZ() * length);
		}
	}

	static float Length(const Vector3 &a)  {
		return sqrt((a.getX()*a.getX()) + (a.getY() *a.getY()) + (a.getZ()*a.getZ()));
	}

};


