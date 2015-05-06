/******************************************************************************
Class:CameraNode
Implements:SceneNode
Author:Rich Davison
Description:FPS-Style camera. Can be attached to a specific joypad, so multiple
joypads can all control multiple cameras (handy for split screen!)

This class is pretty much the Camera class from the Graphics for Games
Framework, but derived from a SceneNode

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once
#include "SceneNode.h"
#include "Input.h"
#include "common.h"
#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

class Camera	{
public:
	Camera(void);
	virtual ~Camera(void);

	Matrix4 BuildViewMatrix();

	virtual void UpdateCamera(float msec = 10.0f);

	////Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}


	void	SetControllingPad(JoyPadNum p) {pad = p;}

protected:
	JoyPadNum	pad;
	float		yaw;
	float		pitch;
	Vector3		position;
};
