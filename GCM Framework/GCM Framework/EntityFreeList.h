#pragma once
#include <queue>
#include "GameEntity.h"

using std::queue;
class EntityFreeList
{
public:
	EntityFreeList(unsigned int start);
	~EntityFreeList();

	GameEntity* GetNewObject();

	void DiscardObject(GameEntity* object);

	void ClearList();

protected:
	queue<GameEntity *> contents;
};

