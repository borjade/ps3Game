#pragma once
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;
class Light
{
public:
	Light(Vector3 position = Vector3(0.0f, 0.0f, 0.0f), Vector4 colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 50)
	{
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}
	~Light(void){};

	Vector3 GetPosition()const{ return position; }
	void SetPosition(Vector3 val){ position = val; }

	float GetRadius()const{ return radius; }
	void SetRadius(float val){ radius = val; }

	Vector4 GetColour()const { return colour; }
	void SetColour(Vector4 val){ colour = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;
};