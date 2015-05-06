#pragma once
#include "SceneNode.h"
#include "Camera.h"
class CameraNode : public SceneNode, public Camera {
public:
	CameraNode(void){};
	~CameraNode(void){};

	virtual void	Update(float msec) {
		UpdateCamera(msec);	//Camera class update
		position.setX(parent->getPosition().getX());
		transform = Matrix4::translation(position)* Matrix4::rotation(yaw, Vector3(0, 1, 0)) * Matrix4::rotation(pitch, Vector3(1, 0, 0));
		SceneNode::Update(msec);	//SceneNode class update
	}
};