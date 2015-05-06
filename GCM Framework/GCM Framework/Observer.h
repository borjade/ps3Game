#pragma once
class PlayerClass;
class Observer
{
public:
	Observer(){};
	virtual ~Observer() {};
	virtual void onNotify(PlayerClass* p){};
};

