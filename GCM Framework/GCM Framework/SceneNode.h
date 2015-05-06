/******************************************************************************
Class:SceneNode
Implements:
Author:Rich Davison
Description: A subset of the SceneNode class you saw in the Graphics for Games
module. Extend this as you wish! Nothing here you haven't seen before...

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once
#include <vector>
#include <map>
#include <vectormath/cpp/vectormath_aos.h>

class Mesh;

using namespace Vectormath::Aos;

class SceneNode	{
public:
	SceneNode(void);
	virtual ~SceneNode(void);

	void			SetTransform(const Matrix4 &matrix) { transform = matrix; } 
	const Matrix4&	GetTransform() const				{ return transform; }
	Matrix4			GetWorldTransform() const			{ return worldTransform; }

	Vector3 getPosition() { return Vector3(worldTransform.getElem(0, 3), worldTransform.getElem(1, 3), worldTransform.getElem(2, 3)); }

	void			SetMesh(Mesh*m)						{ mesh = m; }
	Mesh*			GetMesh()							{ return mesh; }

	virtual void	Update(float msec);

	float			GetBoundingRadius() const	{ return boundingRadius; }
	void			SetBoundingRadius(float f)	{ boundingRadius = f; }


	float			GetCameraDistance() const	{ return distanceFromCamera; }
	void			SetCameraDistance(float f)	{ distanceFromCamera = f; }

	void			AddChild(SceneNode* s);
	bool			RemoveChild(SceneNode* s, bool recursive = true);

	bool	IsAwake()	{ return awake; }
	void	Wake()		{ awake = true; }
	void	Sleep()		{ awake = false; }
	static bool		CompareByCameraDistance(SceneNode*a, SceneNode*b);

	//void setPosition(Vector3 val) { position = val; }


	static bool		CompareByZ(SceneNode*a, SceneNode*b);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart()	{ return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd()	{return children.end();}

	uint32_t			GetColour()		const			{ return color; }
	void			SetColour(const uint32_t &c)		{ color = c; }

	Vector3			GetModelScale()		const			{ return modelScale; }
	void			SetModelScale(const Vector3 &s)		{ modelScale = s; }

protected:

	float		boundingRadius;
	float		distanceFromCamera;
	SceneNode*	parent;
	Matrix4		worldTransform;
	Matrix4		transform;
	Mesh*		mesh;
	bool		awake;
	/*Vector3 position;*/
	Vector3		modelScale;
	uint32_t	color;
	std::vector<SceneNode*>		children;
};
