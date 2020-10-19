#pragma once

#include "GeneticsAlgorithmIsland.h"

#include <vector>
#include <memory>

class GeneticAlgorithmIslandModel
{
public:
	GeneticAlgorithmIslandModel(int ** aArray, int aWidth, int aPopulationMinSize, int aPopulationMaxSize,
						float aMutateFactor, int aNumberOfIslands, bool aTest);
	//bool AddIsland();

private:
	int mWidth;
	int mPopulationMinSize;
	int mPopulationMaxSize;
	int** mArray;
	bool mTest;
	float mMutateFactor;

	std::vector<std::unique_ptr<GeneticsAlgorithmIsland>> mVestorOfIslands;
};

