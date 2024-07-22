#include <iostream>
#include <chrono>

using namespace std;

#include "GameEngine2.h"
#include "MineSweeper.h"

class MineSweeper : public GameEngine
{
private:
	vector<vector<int>> map;		// Matrix that holds data of the minesweeper map
	vector<vector<int>> ConnectMap; // Matrix used to determine connected empty spaces
	
	bool GameEnding = false;

	int Bombs;
	int seed;
	int iterationSize = 0;
	int BombCount = 0, BombCount100 = 0;

	std::chrono::steady_clock::time_point end;
	std::chrono::steady_clock::time_point begin;
	int timeCount;
	int timeCount10;

public:
	MineSweeper(int BomNumber, int x, int y, int s)
	{
		Bombs = BomNumber;
		nFieldWidth = x;
		nFieldHeight = y;
		seed = s;
	}
protected:
	virtual bool OnUserCreate()
	{
		cout << "===========================================" << endl;
		cout << "||      Welcome to MineSweeper C++       ||" << endl;
		cout << "===========================================" << endl;
		cout << endl;
		cout << "Please enter a seed to generate playfield (integer): ";
		cin >> seed;
		if (!cin)
		{
			cerr << "please enter a integer!";
			exit(-1);
		}
		cout << "Please enter a size for the playfield, x: ";
		cin >> nFieldWidth;
		if (nFieldWidth < 13 || nFieldWidth > 35)
		{
			cerr << "Size is too small or too large! Range: 13 to 35";
			exit(-1);
		}
		cout << "Please enter a size for the playfield, y: ";
		cin >> nFieldHeight;
		if (nFieldHeight < 13 || nFieldHeight > 35)
		{
			cerr << "Size is too small or too large! Range: 13 to 35";
			exit(-1);
		}
		cout << "How many bombs would you like to play with: ";
		cin >> Bombs;
		if (Bombs > (nFieldWidth * nFieldHeight) / 2)
		{
			cerr << "Too many Bombs for fieldsize!";
			exit(-1);
		}

		cout << "\n\nGenerating Map..." << endl;

		// Generate Game Seed
		ConstructConsole(nFieldWidth, nFieldHeight);
		MineField mField(seed, nFieldWidth, nFieldHeight, Bombs);
		map = mField.map;
		ConnectMap.resize(nFieldWidth, vector<int>(nFieldHeight, 0));

		// Create Standard pField
		for (int x = 0; x < nScreenWidth; x++)
		{
			for (int y = 0; y < nScreenHeight; y++)
			{
				pField[y * (nScreenWidth) + x] = (x == 0 || y == 2 || y == 0 || x == nScreenWidth - 1 || y == nScreenHeight - 1) ? 11 : 13;
				if (x > 2 && x < nScreenWidth - 1 && y == 1)
				{
					pField[y * (nScreenWidth)+x] = 0;
				}
			}
		}
		pField[1 * (nScreenWidth)+1] = 27;
		pField[1 * (nScreenWidth)+2] = 28;
		pField[1 * (nScreenWidth)+6] = 11;
		pField[1 * (nScreenWidth)+7] = 29;
		pField[1 * (nScreenWidth)+8] = 28;
		pField[1 * (nScreenWidth)+11] = 28;

		begin = std::chrono::steady_clock::now();

		return true;
	}

	auto above(int threshold)
	{
		return [=](int value)
		{
			return value >= threshold;
		};
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (mouse[0].bHeld && GameEnding == false)
		{
			OpenSpot(mousePosX, mousePosY);
		}
		if (mouse[1].bPressed && GameEnding == false)
		{
			FlagSpot(mousePosX, mousePosY);
		}
		if (count_if(pField.begin() + (nScreenWidth*2), pField.end(), above(12)) == Bombs)
		{
			EndGame(0);
		}
		if (GameEnding == true)
		{
			UpdateDisplay();
			Sleep(9999);
			//HWND myConsole = GetConsoleWindow();
			//ShowWindow(myConsole, 1);
		}
		if (GameEnding == false)
		{
			BombCount = Bombs - count(pField.begin(), pField.end(), 14);
			BombCount100 = (BombCount) % 10;
			pField[1 * (nScreenWidth)+5] = abs(BombCount100) + 1;
			BombCount100 = (BombCount / 10) % 10;
			pField[1 * (nScreenWidth)+4] = abs(BombCount100) + 1;
			if (BombCount < 0)
			{
				pField[1 * (nScreenWidth)+3] = 31;
			}
			else
			{
				BombCount100 = (BombCount / 100) % 100;
				pField[1 * (nScreenWidth)+3] = abs(BombCount100) + 1;
			}
			end = std::chrono::steady_clock::now();
			timeCount = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
			timeCount10 = (timeCount / 600) % 6;
			pField[1 * (nScreenWidth)+9] = abs(timeCount10) + 1;
			timeCount10 = (timeCount / 60) % 6;
			pField[1 * (nScreenWidth)+10] = abs(timeCount10) + 1;
			timeCount10 = (timeCount / 10) % 6;
			pField[1 * (nScreenWidth)+12] = abs(timeCount10) + 1;
			timeCount10 = timeCount % 10;
			pField[1 * (nScreenWidth)+13] = abs(timeCount10) + 1;
		}
		return true;
	}

	vector<vector<int>> CheckConnected(vector<vector<int>> ConnectMap, int x, int y)
	{
		if (x < (nScreenWidth - 2) && map[x][y - 3] <= 8 && ConnectMap[x][y - 3] == 0)
		{
			ConnectMap[x][y - 3] = 1;
			pField[(y) * (nScreenWidth)+(x + 1)] = map[x][y - 3] + 1;
			if (map[x][y - 3] == 0)
			{
				ConnectMap = CheckConnected(ConnectMap, x + 1, y);
			}
		}
		if (y < (nScreenHeight - 2) && map[x - 1][y - 2] <= 8 && ConnectMap[x - 1][y - 2] == 0)
		{
			ConnectMap[x - 1][y - 2] = 1;
			pField[(y + 1) * (nScreenWidth) + (x)] = map[x - 1][y - 2] + 1;
			if (map[x - 1][y - 2] == 0)
			{
				ConnectMap = CheckConnected(ConnectMap, x, y + 1);
			}
		}
		if (y > 3 && map[x - 1][y - 4] <= 8 && ConnectMap[x - 1][y - 4] == 0)
		{
			ConnectMap[x - 1][y - 4] = 1;
			pField[(y - 1) * (nScreenWidth) + (x)] = map[x - 1][y - 4] + 1;
			if (map[x - 1][y - 4] == 0)
			{
				ConnectMap = CheckConnected(ConnectMap, x, y - 1);
			}
		}
		if (x > 1 && map[x - 2][y - 3] <= 8 && ConnectMap[x - 2][y - 3] == 0)
		{
			ConnectMap[x - 2][y - 3] = 1;
			pField[(y) * (nScreenWidth) + (x - 1)] = map[x - 2][y - 3] + 1;
			if (map[x - 2][y - 3] == 0)
			{
				ConnectMap = CheckConnected(ConnectMap, x - 1, y);
			}
		}
		return ConnectMap;
	}

	void EndingAnimation(int x, int y)
	{
		for (int xx = -iterationSize; xx <= iterationSize; xx++)
		{
			for (int yy = -iterationSize; yy <= iterationSize; yy++)
			{
				if (x - xx >= 1 && y - yy >= 3 && x - xx <= nScreenWidth-2 && y - yy <= nScreenHeight-2)
				{
					if (map[x - xx - 1][y - yy - 3] <= 8)
					{
						pField[(y - yy) * nScreenWidth + (x - xx)] = 22;
					}
					else
					{
						pField[(y - yy) * nScreenWidth + (x - xx)] = 30;
					}
				}
			}
		}
		iterationSize++;
		UpdateDisplay();
		Sleep(500);
		if (iterationSize <= max(nFieldHeight, nFieldWidth))
		{
			EndingAnimation(x, y);
		}
		EndGame(1);
	}

	void OpenSpot(int x, int y)
	{
		if (x >= 1 && x < nScreenWidth-1 && y >= 3 && y < nScreenHeight-1 && pField[y*nScreenWidth+x] >= 12)
		{
			if (map[x-1][y-3] <= 8)
			{
				ConnectMap[x - 1][y - 3] = 1;
				pField[(y) * (nScreenWidth) + (x)] = map[x - 1][y - 3] + 1;
				if (map[x - 1][y - 3] == 0)
				{
					ConnectMap = CheckConnected(ConnectMap, x, y);
				}
			}
			if (map[x - 1][y - 3] == 9)
			{
				GameEnding = true;
				EndingAnimation(x,y);
			}
		}
	}

	void FlagSpot(int x, int y)
	{
		if (x >= 1 && x < nScreenWidth - 1 && y >= 3 && y < nScreenHeight - 1 && pField[y * nScreenWidth + x] == 13)
		{
			pField[y * nScreenWidth + x] = 14;
		}
		else if (x >= 1 && x < nScreenWidth - 1 && y >= 3 && y < nScreenHeight - 1 && pField[y * nScreenWidth + x] == 14)
		{
			pField[y * nScreenWidth + x] = 12;
		}
		else if (x >= 1 && x < nScreenWidth - 1 && y >= 3 && y < nScreenHeight - 1 && pField[y * nScreenWidth + x] == 12)
		{
			pField[y * nScreenWidth + x] = 13;
		}
	}

	void EndGame(int case_i)
	{
		fill(pField.begin(), pField.end(), 0);
		const char* word[2] = { "You Win","You Lose" };
		int letter = 15;
		for (int x = 0; x < nScreenWidth; x++)
		{
			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y == nScreenHeight / 2 && x >= (nScreenWidth - strlen(word[case_i])) / 2 && x <= (nScreenWidth + strlen(word[case_i])) / 2 - 1)
				{
					pField[y * nScreenWidth + x] = letter;
					letter++;
					if (case_i == 1 && letter == 19) 
					{
						letter = 23;
					}
				}
			}
		}
		GameEnding = true;
	}
};

int main()
{
	MineSweeper game(15, 13, 13, 7); // int values: bom number, size x, size y, seed
	game.Start(); // starts game engine

	return 0;
}