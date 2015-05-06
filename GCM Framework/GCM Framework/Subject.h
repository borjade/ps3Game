#pragma once
#include "Observer.h"
#include <vector>
#include <algorithm>

class PlayerClass;
using std::vector; 
class Subject
{
public:
	Subject(){};
	~Subject(){
		for (std::vector<Observer*>::iterator i = observers.begin(); i != observers.end(); ++i)
			{
				observers.pop_back();
			}
		}


	void addObserver(Observer* obs) {observers.push_back(obs);};
	void removeObserver(Observer* obs)
	{
		observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
	};
	void notify(PlayerClass * p)
	{
		for (vector<Observer*>::const_iterator iter = observers.begin(); iter != observers.end(); ++iter)
		{
			if (*iter != 0)
			{
				(*iter)->onNotify(p);//Update(price);
			}
		}
	};

	

private:
	int numObservers;
	vector<Observer*> observers;
};

