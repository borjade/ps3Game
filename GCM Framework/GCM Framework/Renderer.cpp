#include "Renderer.h"

Renderer* Renderer::instance = NULL;
Renderer::Renderer(void)	{
	/*
	You're provided with a very basic vertex / fragment shader, to get you started
	with Cg, and drawing textured objects. 
	*/
	VertexShader* basicVert		= new VertexShader("/vertex.vpo");
	FragmentShader* basicFrag	= new FragmentShader("/fragment.fpo");
	instance = this;
	this->SetCurrentShader(*basicVert,*basicFrag);

	/*
	Projection matrix...0.7853982 is 45 degrees in radians.
	*/
	projMatrix	= Matrix4::perspective(0.7853982, screenRatio, 1.0f, 20000.0f);	//CHANGED TO THIS!!

	//m = Mesh::GenerateCube(1.0f);
	//m->SetDefaultTexture(*GCMRenderer::LoadGTF("/GRASS.gtf"));
	//Create a new scenenode
	root = new SceneNode();
	SetRootNode(root);


	camera = new Camera();
	//camera->SetControllingPad(JOYPAD_A);	//Controlled via joypad A
	//camera->SetPosition(Vector3(0, 0, 600));
	//SetCamera(camera);
}

Renderer::~Renderer(void)
{

	delete root;
}

void Renderer::UpdateScene(float msec)
{
	if (camera)
		camera->UpdateCamera(msec);

	root->Update(msec);
}

/*
Main rendering function. Note how it's essentially the same as the
ones you were writing in OpenGL! We start by clearing the buffer,
render some stuff, then swap the buffers. All that's different is
some slightly different matrix access.

*/
void Renderer::RenderScene() {
	//float msec = (float)gameTime.GetTimedMS();
	//camera->UpdateCamera(msec);
	//root->Update(msec);

	//std::cout << "RenderScene!" << std::endl;
	SetViewport();
	ClearBuffer();
	this->SetCurrentShader(*currentVert,*currentFrag);

	cellGcmSetDepthTestEnable(CELL_GCM_TRUE);
	cellGcmSetDepthFunc(CELL_GCM_LESS);

	modelMatrix = Matrix4::identity(); 

	if(camera) {
		viewMatrix = camera->BuildViewMatrix();
	}
	else{
		viewMatrix = Matrix4::identity();
	}

	//currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);
	currentVert->UpdateShaderMatrices(modelMatrix, viewMatrix, projMatrix);
	if(root) {
		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}
	SwapBuffers();
}

// Here is the code that takes care of the scenenode lists etc
#pragma region ScenNodes
void	Renderer::BuildNodeLists(SceneNode* from)	{
	
	Vector3 direction = from->GetWorldTransform().getCol3().getXYZ() - camera->GetPosition(); 
	from->SetCameraDistance(Vectormath::Aos::dot(direction, direction));

	//if(frameFrustum.InsideFrustum(*from)) {
	//if (from->GetColour().w < 1.0f) {
		//transparentNodeList.push_back(from);
	//}
	//else{
		nodeList.push_back(from);
	//}
	//}

	for (std::vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()	 {
	for (std::vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	/*for (std::vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}*/
}

void	Renderer::AddNode(SceneNode* n) {
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n) {
	root->RemoveChild(n);
}


void	Renderer::SortNodeLists()	{
	//std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	{
	//transparentNodeList.clear();
	nodeList.clear();
}
#pragma endregion ScenNodes

//void Renderer::Drawtext()