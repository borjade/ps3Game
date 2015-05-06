#include "SceneNode.h"

SceneNode::SceneNode(void)	{
	transform		= Matrix4::identity();
	worldTransform	= Matrix4::identity();
	parent			= NULL;
	mesh			= NULL;
}

SceneNode::~SceneNode(void)	{
}

void	SceneNode::Update(float msec) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else{
		worldTransform = transform;
	}

	for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
	}
}

void	SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}
bool	SceneNode::CompareByCameraDistance(SceneNode*a, SceneNode*b)  {
	return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
}

bool	SceneNode::CompareByZ(SceneNode*a, SceneNode*b)  {
	return (a->GetWorldTransform().getTranslation().getZ() < b->GetWorldTransform().getTranslation().getZ()) ? true : false;
}

bool SceneNode::RemoveChild(SceneNode* s, bool recursive) {

	for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		if ((*i) == s) {
			i = children.erase(i);
			return true;
		}
	}

	if (recursive) {
		for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
			if ((*i)->RemoveChild(s, recursive)) {
				return true;
			}
		}
	}
	return false;
}