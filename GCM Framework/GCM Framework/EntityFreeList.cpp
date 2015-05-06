#include "EntityFreeList.h"


EntityFreeList::EntityFreeList(unsigned int startCount)
{
	for (unsigned int i = 0; i < startCount; ++i)
	{
		contents.push(new GameEntity(new SceneNode(), new PhysicsNode(Quat(0), Vector3(0))));
	}
}


EntityFreeList::~EntityFreeList()
{
	ClearList();
}

void EntityFreeList::DiscardObject(GameEntity* object)
{
	contents.push(object); //add the returned entity to the back of the queue
}

GameEntity* EntityFreeList::GetNewObject()
{
	if (contents.empty())
	{
		return new GameEntity(new SceneNode(), new PhysicsNode(Quat(0), Vector3(0)));
	}//this is kinda the snag, we don't really want our list to be empty or else it'll have to create the entities on the fly -> kinda expensive
		

	GameEntity* object = contents.front(); // we'll be returning the front of the queue
	contents.pop();//pop it from the queue
	return object; //return the object
}


void EntityFreeList::ClearList()
{
	while (!contents.empty())
	{
		GameEntity* object = contents.front();
		contents.pop();
		//delete object;
	}
}