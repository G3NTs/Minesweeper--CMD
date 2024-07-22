#include <iostream>

using namespace std;

#include "GameEngine2.h"

class MineSweeper : public GameEngine
{
public:
	MineSweeper()
	{

	}
protected:
	virtual bool OnUserCreate()
	{
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		return true;
	}
};

int main()
{
    MineSweeper game; // int values: map size
	game.ConstructConsole(39,39);

	return 0;
}