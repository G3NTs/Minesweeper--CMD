#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <string>
#include <stdexcept>

class MineField
{
private:
	int seed, sizeWidth, sizeHeight, mineNumber;

public:
	vector<vector<int>> map;

	MineField(int c_seed, int c_sizeWidth, int c_sizeHeight, int c_mineNumber)
	{
		GenerateSeed(c_seed, c_sizeWidth, c_sizeHeight, c_mineNumber);
	}

	void GenerateSeed(int c_seed, int c_sizeWidth, int c_sizeHeight, int c_mineNumber)
	{
		if (c_sizeWidth <= 100 && c_sizeHeight <= 100 && c_seed <= 99999999 && c_sizeWidth >= 13 && c_sizeHeight >= 13)
		{
			seed = c_seed;
			sizeWidth = c_sizeWidth;
			sizeHeight = c_sizeHeight;
			mineNumber = c_mineNumber;
			MakeMap(seed, sizeWidth, sizeHeight, mineNumber);
			MakeNeighborList();
		}
		else
		{
			cerr << "seed or size does not compute!" << endl;
			exit(-1);
		}

	};
	void MakeMap(int seed, int sizeWidth, int sizeHeight, int mineNumber)
	{
		vector<vector<float>> map_f;
		map.resize(sizeWidth, vector<int>(sizeHeight, 0));
		map_f.resize(sizeWidth, vector<float>(sizeHeight, 0));

		srand(seed);

		for (int i = 0; i < sizeWidth; i++)
		{
			for (int j = 0; j < sizeHeight; j++)
			{
				map_f[i][j] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			}
		}

		vector<float> maxElement(mineNumber,0);
		vector<int> maxI(mineNumber, 0);
		vector<int> maxJ(mineNumber, 0);
		vector<float>::iterator it = min_element(maxElement.begin(), maxElement.end());

		int index = 0;

		for (int i = 0; i < sizeWidth; i++)
		{
			for (int j = 0; j < sizeHeight; j++)			
			{
				if (map_f[i][j] > *min_element(maxElement.begin(), maxElement.end()) && ((i != (sizeWidth/2)) && (j != (sizeHeight/2))))
				{
					index = distance(maxElement.begin(), it);
					maxElement[index] = map_f[i][j];
					maxI[index] = i;
					maxJ[index] = j;
					it = min_element(maxElement.begin(), maxElement.end());
				}
			}
		}

		for (int k = 0; k < maxElement.size(); k++)
		{
			map[maxI[k]][maxJ[k]] = 9; // 9 == bom
		}
	};

	void MakeNeighborList()
	{
		

		for (int encounters, i = 0; i < sizeWidth; i++)
		{
			for (int j = 0; j < sizeHeight; j++)
			{
				if (map[i][j] == 9)
				{
					for (int ii = i - 1; ii < i + 2; ii++)
					{
						for (int jj = j - 1; jj < j + 2; jj++)
						{
							if (ii < 0 || ii >= sizeWidth || jj < 0 || jj >= sizeHeight)
							{
								// outside of area
							}
							else if (map[ii][jj] != 9)
							{
								map[ii][jj]++;
							}
						}
					}
				}
				
				encounters = 0;
			}
		}
	}
};

