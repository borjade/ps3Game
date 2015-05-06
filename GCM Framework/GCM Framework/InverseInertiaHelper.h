#pragma once
/**
* Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
*/
#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

class InertialMatrixHelper {
public:

	static Matrix4 createSphereInertial(float mass, float radius) {
		float I = 0.4f*mass*radius*radius;
		//float elements[] = { I, 0, 0, 0, 0, I, 0, 0, 0, 0, I, 0, 0, 0, 0, 1 };
		Matrix4 m = Matrix4::identity();
		m.setElem(0, 0, I);
		m.setElem(1, 1, I);
		m.setElem(2, 2, I);
		return m;
	}

	static Matrix4 createSphereInvInertial(float mass, float radius) {
		float I = 2.5f / (mass*radius*radius);
		Matrix4 m = Matrix4::identity();
		m.setElem(0, 0, I);
		m.setElem(1, 1, I);
		m.setElem(2, 2, I);
		return m;
	}

	static Matrix4 createCuboidInertial(float mass, float width, float height, float depth) {
		float Ixx = 0.0833f*mass*(depth*depth + height*height);
		float Iyy = 0.0833f*mass*(width*width + depth*depth);
		float Izz = 0.0833f*mass*(width*width + height*height);
		Matrix4 m = Matrix4::identity();
		m.setElem(0, 0, Ixx);
		m.setElem(1, 1, Iyy);
		m.setElem(2, 2, Izz);
		return m;
	}

	static Matrix4 createCuboidInvInertial(float mass, float width, float height, float depth) {
		float Ixx = 12.0f / (mass*(depth*depth + height*height));
		float Iyy = 12.0f / (mass*(width*width + depth*depth));
		float Izz = 12.0f / (mass*(width*width + height*height));
		Matrix4 m = Matrix4::identity();
		m.setElem(0, 0, Ixx);
		m.setElem(1, 1, Iyy);
		m.setElem(2, 2, Izz);
		return m;
	}

	static Matrix4 createImmovableInvInertial() {
		Matrix4 m = Matrix4::identity();
		m.setElem(3, 3, 1);
		return m;
	}

};