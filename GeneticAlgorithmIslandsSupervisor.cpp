#include "GeneticAlgorithmIslandsSupervisor.h"

#include <chrono>
#include <iomanip>
#include <iostream>

GeneticAlgorithmIslandsSupervisor::GeneticAlgorithmIslandsSupervisor(
	int** aArray,
	int aWidth,
	bool aTest, 
	int aPopulationMinSize,
	int aPopulationMaxSize,
	float aMutateFactor,
	int aNumberOfIslands,
	int aLimitOfWorstSolution,
	int aOptimalSolution)
{
	mArray = aArray;
	mWidth = aWidth;
	mPopulationMinSize = aPopulationMinSize;
	mPopulationMaxSize = aPopulationMaxSize;
	mMutateFactor = aMutateFactor;
	mLimitOfWorstSolution = aLimitOfWorstSolution;
	mTest = aTest;
	mOptimalSolution = aOptimalSolution;

	for (int i = 0; i < aNumberOfIslands; i++)
		mVestorOfIslands.push_back(std::make_shared<GeneticsAlgorithmIsland>());

	// Set island in circle topology.
	for (int i = 0; i < aNumberOfIslands; i++)
	{
		if (i == 0)
		{
			mVestorOfIslands.at(0)->SetPreviousNode(mVestorOfIslands.at(aNumberOfIslands-1).get());
			mVestorOfIslands.at(aNumberOfIslands-1)->SetNextNode(mVestorOfIslands.at(0).get());
			continue;
		}

		mVestorOfIslands.at(i)->SetPreviousNode(mVestorOfIslands.at(i-1).get());
		mVestorOfIslands.at(i - 1)->SetNextNode(mVestorOfIslands.at(i).get());
	}
}

void GeneticAlgorithmIslandsSupervisor::StartAlgorithmLoop()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (auto it = mVestorOfIslands.begin(); it != mVestorOfIslands.end(); ++it)
	{
		(*it)->StartAlgorithm(mWidth, mPopulationMinSize, mPopulationMaxSize,
			mLimitOfWorstSolution, mArray, mMutateFactor, mTest);
	}

	
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		bool allThreadHasBeenFinished = true;
		for (auto it = mVestorOfIslands.begin(); it != mVestorOfIslands.end(); ++it)
		{
			if ((*it)->IsFinished() == false)
			{
				allThreadHasBeenFinished = false;
				break;
			}
		}

		if (allThreadHasBeenFinished == true)
			break;
	}

	{
		// Measure time.
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		// Colect data.

		int numberOfAllGeneration = 0;
		int averageNumberOfAllGeneration = 0;
		int bestSolutionCost = 0;
		std::shared_ptr<std::vector<int>> bestSolution;

		for (auto it = mVestorOfIslands.begin(); it != mVestorOfIslands.end(); ++it)
		{
			numberOfAllGeneration += (*it)->GetGenerationNumber();
			if (bestSolutionCost == 0 || (*it)->GetSolutionCost() < bestSolutionCost)
			{
				bestSolutionCost = (*it)->GetSolutionCost();
				(*it)->GetBestSolution(bestSolution);
			}
		}
		averageNumberOfAllGeneration = numberOfAllGeneration / mVestorOfIslands.size();

		int error = bestSolutionCost - mOptimalSolution;
		float temp = (float)error / (float)mOptimalSolution;
		int errorInProcent = (int)(temp * 100);
		
		// Print solution.
		if (mTest == false)
		{
			std::cout << std::endl;
			GeneticsAlgorithmIsland::PrintSolution(bestSolution, mArray, 0);
			std::cout << std::endl;
		}

		std::cout << std::setfill(' ') << std::setw(6) << mWidth << "; ";						// Size
		std::cout << std::setfill(' ') << std::setw(6) << mPopulationMinSize << "; ";			// PopulationMinSize
		std::cout << std::setfill(' ') << std::setw(6) << mPopulationMaxSize << "; ";			// PopulationMaxSize
		std::cout << std::setfill(' ') << std::setw(6) << mMutateFactor << "; ";				// MutateFactor
		std::cout << std::setfill(' ') << std::setw(6) << mLimitOfWorstSolution << "; ";		// LimitOfWorstSolution
		std::cout << std::setfill(' ') << std::setw(6) << numberOfAllGeneration << "; ";		// numberOfAllGeneration
		std::cout << std::setfill(' ') << std::setw(6) << averageNumberOfAllGeneration << "; "; // averageNumberOfAllGeneration
		std::cout << std::setfill(' ') << std::setw(6) << bestSolutionCost << "; ";				// bestSolutionCost
		std::cout << std::setfill(' ') << std::setw(6) << mOptimalSolution << "; ";				// mOptimalSolution
		std::cout << std::setfill(' ') << std::setw(6) << duration.count() << "; ";				// time (millisecondsd)
		std::cout << std::setfill(' ') << std::setw(6) << error << "; ";						// blad
		std::cout << std::setfill(' ') << std::setw(6) << errorInProcent << "; ";						// blad in %
		std::cout << std::setfill(' ') << std::setw(6) << mVestorOfIslands.size() << "; ";		// NumberOfThreads.
		std::cout << std::endl;

		std::cout << std::setfill(' ') << std::setw(8) << " Size   ";
		std::cout << std::setfill(' ') << std::setw(8) << " MinSiz ";
		std::cout << std::setfill(' ') << std::setw(8) << " MaxSiz ";
		std::cout << std::setfill(' ') << std::setw(8) << " MutFac ";
		std::cout << std::setfill(' ') << std::setw(8) << " WorstL ";
		std::cout << std::setfill(' ') << std::setw(8) << " Genera ";
		std::cout << std::setfill(' ') << std::setw(8) << " Gen/th ";
		std::cout << std::setfill(' ') << std::setw(8) << " Solu   ";
		std::cout << std::setfill(' ') << std::setw(8) << " OptSol ";
		std::cout << std::setfill(' ') << std::setw(8) << " Time   ";
		std::cout << std::setfill(' ') << std::setw(8) << " Error  ";
		std::cout << std::setfill(' ') << std::setw(8) << " Error% ";
		std::cout << std::setfill(' ') << std::setw(8) << " Thread ";
		mTest ? std::cout << "\r": std::cout << std::endl;
	}

}
