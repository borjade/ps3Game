/******************************************************************************
Class:Renderer
Implements:
Author:Rich Davison
Description: For consistency with the Graphics for Games module, we're deriving
a 'renderer' from the actual API specific parent class. This is the one you 
should be fiddling with! Add whatever you like, here.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "GCMRenderer.h"
#include "Mesh.h"
#include "Timer.h"
#include "Light.h"
#include "VectorAid.h"
#include <sys\synchronization.h>
#include <algorithm>
#include <TextMesh.h>
class Renderer : public GCMRenderer	{
public:
	Renderer(void);
	~Renderer(void);

	virtual void RenderScene();

	void	AddNode(SceneNode* n);

	void	RemoveNode(SceneNode* n);

	void	UpdateScene(float msec);

	static bool Initialise() {
		instance = new Renderer();
		return instance->HasInitialised();
	}

	static void Destroy() {
		delete instance;
	}

	//void start_button()		{
	//	done = true;
	//	std::cout << "Pressed start button!" << std::endl;
	//}

	//void select_button()		{
	//	camera->SetPosition(Vector3(0, 0, 10));
	//	camera->SetPitch(0.0f);
	//	camera->SetYaw(0.0f);
	//}
	static Renderer&GetRenderer() { return *instance; }
	Camera* getCamera(){ return camera; }

	//Added by Chris Fraser
	Matrix4 GetModelMatrix(){ return modelMatrix; }
	Matrix4 GetViewMatrix(){ return viewMatrix; }
	Matrix4 GetProjectionMatrix(){ return projMatrix; }
	Matrix4 GetMVPMatrix() { return mvpMatrix; }
	void SetMVPMatrix(Matrix4 setter){ mvpMatrix = setter; }

	void Drawtext(const std::string &text, const Vector3 &position, const float size, const bool perspective);

protected:
	static Renderer*	instance;

	Mesh* m;
	SceneNode* root;
	SceneNode* logo;
	Timer gameTime;
	Camera* camera;

	Font* basicFont;

	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();

	std::vector<SceneNode*> nodeList;
};
