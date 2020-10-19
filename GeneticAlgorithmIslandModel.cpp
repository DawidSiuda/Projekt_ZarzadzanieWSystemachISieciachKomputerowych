#include "GeneticAlgorithmIslandModel.h"

GeneticAlgorithmIslandModel::GeneticAlgorithmIslandModel(int** aArray, int aWidth, int aPopulationMinSize, int aPopulationMaxSize, float aMutateFactor, int aNumberOfIslands, bool aTest)
{
	mArray = aArray;
	mWidth = aWidth;
	mPopulationMinSize = aPopulationMinSize;
	mPopulationMaxSize = aPopulationMaxSize;
	mMutateFactor = aMutateFactor;
	mTest = aTest;
	
	for (int i = 0; i < aNumberOfIslands; i++)
		mVestorOfIslands.push_back(std::make_unique<GeneticsAlgorithmIsland>());
}
