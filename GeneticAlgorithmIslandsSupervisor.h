#pragma once

#include "GeneticsAlgorithmIsland.h"

#include <vector>
#include <memory>

class GeneticAlgorithmIslandsSupervisor
{
public:
	GeneticAlgorithmIslandsSupervisor(int** aArray, int aWidth, bool aTest = false,
		int aPopulationMinSize = 20,
		int aPopulationMaxSize = 40,
		float aMutateFactor = 0.2,
		int aNumberOfIsland = 1,
		int aLimitOfWorstSolution = 1500,
		int aOptimalSolution = -1);

	void StartAlgorithmLoop();

private:
	int mWidth;
	int mPopulationMinSize;
	int mPopulationMaxSize;
	int mLimitOfWorstSolution;
	int** mArray;
	bool mTest;
	float mMutateFactor;
	int mOptimalSolution;

	std::vector<std::shared_ptr<GeneticsAlgorithmIsland>> mVestorOfIslands;
};

