#include <algorithm>    // std::random_shuffle
#include <iomanip>

#include "GeneticsAlgorithmIsland.h"


GeneticsAlgorithmIsland::GeneticsAlgorithmIsland()
{
	mGenerationCounter = 0;
	mBestFoundSolutionCost.store(-1);

	mNextIsland = nullptr;
	mPreviousIsland = nullptr;
}

GeneticsAlgorithmIsland::~GeneticsAlgorithmIsland()
{
	StopAlgorithm();
}

bool GeneticsAlgorithmIsland::StartAlgorithm(int aWidth, int aPopulationMinSize,
												int aPopulationMaxSize, int aLimitOfWorstSolution,
													int** aArray, float aMutateFactor, bool aTest)
{
	mAlgorithmLoopIsWorking.store(true);

	try
	{
		mStopAlgorithmLoop.store(false);
		mAlgorithmLoopThread = std::make_unique<std::thread>(&GeneticsAlgorithmIsland::Genetic,
			this,
			aArray,
			aWidth,
			aPopulationMinSize,
			aPopulationMaxSize,
			aMutateFactor,
			aLimitOfWorstSolution,
			rand(),
			aTest);
	}
	catch (const std::exception&)
	{
		mAlgorithmLoopIsWorking.store(false);
		return false;
	}

	return true;
}

bool GeneticsAlgorithmIsland::StopAlgorithm()
{
	mStopAlgorithmLoop.store(true);

	if (mAlgorithmLoopThread)
	{
		mAlgorithmLoopThread->join();
		mAlgorithmLoopThread.reset();
	}
	return true;
}

void GeneticsAlgorithmIsland::SetNextNode(GeneticsAlgorithmIsland* aNextIsland)
{
	std::lock_guard<std::mutex> lck(mUseOtherIslandsMutex);
	mNextIsland = aNextIsland;
}

void GeneticsAlgorithmIsland::SetPreviousNode(GeneticsAlgorithmIsland* aPreviousIsland)
{
	std::lock_guard<std::mutex> lck(mUseOtherIslandsMutex);
	mPreviousIsland = aPreviousIsland;
}

int GeneticsAlgorithmIsland::GetGenerationNumber()
{
	return mGenerationCounter;
}

int GeneticsAlgorithmIsland::GetSolutionCost()
{
	return mBestFoundSolutionCost.load();
}

bool GeneticsAlgorithmIsland::IsFinished()
{
	return mAlgorithmLoopIsWorking.load() == false;
}

void GeneticsAlgorithmIsland::GenerateStartupPopulation(int aVertexNumber, PopulationVector& aPopulationVector,
															int aPopulationMinSize, int aPopulationMaxSize)
{
	aPopulationVector = std::make_shared<std::vector<std::shared_ptr<std::vector<int>>>>();

	for (int i = 0; i < aPopulationMaxSize - aPopulationMinSize; ++i)
	{
		auto solution = std::make_shared<std::vector<int>>();
		for (int i = 0; i < aVertexNumber; ++i)
			solution->push_back(i);

		std::random_shuffle(solution->begin(), solution->end());

		aPopulationVector->push_back(solution);
	}
}

int GeneticsAlgorithmIsland::CalcCost(VectorSPtr aSolution, int** aAdjacencyMatrix)
{
	if (aSolution->size() <= 1)
		return -1;

	int cost = 0;

	for (auto it = aSolution->begin(); it != aSolution->end()-1; ++it)
	{
		cost += aAdjacencyMatrix[*it][*(it+1)];
	}

	cost += aAdjacencyMatrix[aSolution->back()][aSolution->front()];

	return cost;
}

void GeneticsAlgorithmIsland::QuickSort(int aLeft, int aRight, VectorSPtr aPathsSize, PopulationVector aPopulationVector)
{
	int i, j, piwot;

	i = (aLeft + aRight) / 2;

	piwot = aPathsSize->at(i);
	aPathsSize->at(i) = aPathsSize->at(aRight);

	VectorSPtr tmp = aPopulationVector->at(i);
	(*aPopulationVector)[i] = (*aPopulationVector)[aRight];

	for (j = i = aLeft; i < aRight; i++)
	{
		if (aPathsSize->at(i) < piwot)
		{
			std::iter_swap(aPathsSize->begin() + i, aPathsSize->begin() + j);

			std::iter_swap(aPopulationVector->begin() + i, aPopulationVector->begin() + j);
			j++;
		}
	}

	aPathsSize->at(aRight) = aPathsSize->at(j);
	aPathsSize->at(j) = piwot;

	aPopulationVector->at(aRight) = (*aPopulationVector)[j];
	aPopulationVector->at(j) = tmp;

	if (aLeft < j - 1)
	{
		QuickSort(aLeft, j - 1, aPathsSize, aPopulationVector);
	}

	if (j + 1 < aRight)
	{
		QuickSort(j + 1, aRight, aPathsSize, aPopulationVector);
	}
}

void GeneticsAlgorithmIsland::SortPopulation(PopulationVector aPopulationVector, int** aInArray)
{
	//
	// Calculate paths.
	//

	size_t populationVectorSize = aPopulationVector->size();

	VectorSPtr lengthOfRoadsInSolutions = std::make_shared<std::vector<int>>(populationVectorSize);

	for (int i = 0; i < populationVectorSize; i++)
	{
		lengthOfRoadsInSolutions->at(i) = CalcCost(aPopulationVector->at(i), aInArray);
	}

	//
	// Sort population.
	//

	QuickSort(0, populationVectorSize - 1, lengthOfRoadsInSolutions, aPopulationVector);
}

void GeneticsAlgorithmIsland::ReducePopulation(PopulationVector aPopulationVector, int aNewSize)
{
	while (aPopulationVector->size() > aNewSize)
	{
		aPopulationVector->pop_back();
	}
}

void GeneticsAlgorithmIsland::CrossPopulation(PopulationVector aPopulationVector, int aPopulationMaxSize)
{
	size_t populationStartupSize = aPopulationVector->size();

	for (int i = 0; i < aPopulationMaxSize - populationStartupSize; ++i)
	{
		//
		// Select 2 parents.
		//

		int rand1 = rand() % populationStartupSize;
		int rand2 = rand() % populationStartupSize;

		VectorSPtr parent_a = aPopulationVector->at(rand1);
		VectorSPtr parent_b = aPopulationVector->at(rand2);

		//
		// Cross parents.
		//
		size_t parentSize = parent_b->size();

		VectorSPtr child = std::make_shared<std::vector<int>>();

		int beginIndex = rand() % (parentSize / 2);

		for (int i = 0; i < parentSize / 2; ++i)
		{
			child->push_back(parent_a->at(beginIndex + i));
		}

		for (int i = 0; i < parentSize; ++i)
		{
			bool exist = false;
			for (int j = 0; j < child->size(); ++j)
			{
				if (parent_b->at(i) == child->at(j))
				{
					exist = true;
					break;
				}
			}

			if (!exist)
			{
				child->push_back(parent_b->at(i));
			}
		}

		aPopulationVector->push_back(child);
	}
}

void GeneticsAlgorithmIsland::MutatePopulation(PopulationVector aPopulationVector, float aMutateFactor, int aPopulationMinSize)
{

	for (int i = 0; i < aPopulationVector->size(); ++i)
	{
		if (rand() % 101 <= aMutateFactor * 100)
		{
			aPopulationVector->push_back(std::make_shared<std::vector<int>>(*aPopulationVector->at(i)));
			Mutate(aPopulationVector->back());
		}
	}
}

void GeneticsAlgorithmIsland::Mutate(VectorSPtr aInstance)
{
	size_t vectorSize = aInstance->size();

	int begin = rand() % vectorSize;

	int end = begin + rand() % (vectorSize - begin);

	int numberOfNodesToReverse = end - begin;

	if (numberOfNodesToReverse < 2)
		return;

	//
	// Reverse.
	//

	std::reverse(aInstance->begin() + begin, aInstance->begin() + end);
}

void GeneticsAlgorithmIsland::CheckAndSetBestSolution(PopulationVector aPopulationVector, int** aAdjacencyMatrix,
														int& aWorseSolutionCounter)
{
	std::lock_guard<std::mutex> lck(mBestFoundSolutionMutex);

	if (mBestFoundSolution == nullptr)
	{
		mBestFoundSolution = std::make_shared<std::vector<int>>(*aPopulationVector->at(0));
		return;
	}

	int first = CalcCost(aPopulationVector->at(0), aAdjacencyMatrix);
	int lastTheBest = CalcCost(mBestFoundSolution, aAdjacencyMatrix);

	if (first < 1776 || lastTheBest < 1776)
	{
		int a = 0;
	}

	if (first < lastTheBest)
	{
		//std::cout << "aWorseSolutionCounter: " << std::setfill(' ') << std::setw(6) << aWorseSolutionCounter << " "
		//	<< std::setfill(' ') << std::setw(6) << first << " "
		//	<< std::setfill(' ') << std::setw(6) << lastTheBest << std::endl;

		*mBestFoundSolution = *aPopulationVector->at(0);
		aWorseSolutionCounter = 0;
	}
	else
	{
		aWorseSolutionCounter++;
	}
}

void GeneticsAlgorithmIsland::PrintSolution(VectorSPtr aSolution, int** aAdjacencyMatrix, int aPopulationCounter)
{
	//std::cout << "Instance ID: " << mInstanceID << std::endl;
	//std::cout << "Population counter: " << aPopulationCounter << std::endl;
	for (int i : *aSolution)
	{
		std::cout << i << " -> ";
	}
	std::cout << aSolution->at(0) << std::endl;

	//std::cout << "\tSum: " << CalcCost(aSolution, aAdjacencyMatrix) << std::endl << std::endl;
}

bool GeneticsAlgorithmIsland::DownloadBestSolutionFromNextNode(VectorSPtr& aSolution)
{
	std::lock_guard<std::mutex> lck(mUseOtherIslandsMutex);

	if (mNextIsland == nullptr)
	{
		return false;
	}

	if (mNextIsland->GetBestSolution(aSolution) == true)
	{
		if (mNextNodeBestSolutionBackup == nullptr)
		{
			mNextNodeBestSolutionBackup = std::make_shared<std::vector<int>>(*aSolution);
			return true;
		}

		if (*aSolution == *mNextNodeBestSolutionBackup)
		{
			return false;
		}
		else
		{
			*mNextNodeBestSolutionBackup = *aSolution;
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool GeneticsAlgorithmIsland::DownloadBestSolutionFromPreviousNode(VectorSPtr& aSolution)
{
	std::lock_guard<std::mutex> lck(mUseOtherIslandsMutex);

	if (mPreviousIsland == nullptr)
	{
		return false;
	}

	if (mPreviousIsland->GetBestSolution(aSolution) == true)
	{
		if (mPreviousNodeBestSolutionBackup == nullptr)
		{
			mPreviousNodeBestSolutionBackup = std::make_shared<std::vector<int>>(*aSolution);
			return true;
		}

		if (*aSolution == *mPreviousNodeBestSolutionBackup)
		{
			return false;
		}
		else
		{
			*mPreviousNodeBestSolutionBackup = *aSolution;
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool GeneticsAlgorithmIsland::GetBestSolution(VectorSPtr& aSolution)
{
	std::lock_guard<std::mutex> lck(mBestFoundSolutionMutex);

	if (mBestFoundSolution == nullptr)
	{
		return false;
	}
	else
	{
		//aSolution = std::make_shared<std::vector<int>>(*mBestFoundSolution);
		aSolution = mBestFoundSolution;
		return true;
	}
}

void GeneticsAlgorithmIsland::DownloadBestSolutionFromNextAndPreviousNode(PopulationVector aPopulationVector)
{
	VectorSPtr bestSolution;
	if (DownloadBestSolutionFromNextNode(bestSolution) == true)
	{
		aPopulationVector->push_back(std::make_shared<std::vector<int>>(*bestSolution));
	}
	
	if (DownloadBestSolutionFromPreviousNode(bestSolution) == true)
	{
		aPopulationVector->push_back(std::make_shared<std::vector<int>>(*bestSolution));
	}
}

int GeneticsAlgorithmIsland::Genetic(int** aArray, int aWidth, int aPopulationMinSize, int aPopulationMaxSize,
										float aMutateFactor, int aLimitOfWorstSolution, int aSeed, int aTest)
{
	srand(aSeed);
	PopulationVector populationVector;

	int worseSolutionCounter = 0;
	mGenerationCounter = 0;

	GenerateStartupPopulation(aWidth, populationVector, aPopulationMinSize, aPopulationMaxSize);

	CheckAndSetBestSolution(populationVector, aArray, worseSolutionCounter);

	SortPopulation(populationVector, aArray);

	while (worseSolutionCounter < aLimitOfWorstSolution)
	{
		ReducePopulation(populationVector, aPopulationMinSize);
		CrossPopulation(populationVector, aPopulationMaxSize);
		MutatePopulation(populationVector, aMutateFactor, aPopulationMinSize);
		if (mGenerationCounter % 8 == 0)
			DownloadBestSolutionFromNextAndPreviousNode(populationVector);
		SortPopulation(populationVector, aArray);
		CheckAndSetBestSolution(populationVector, aArray, worseSolutionCounter);

		mGenerationCounter++;
	}

	mBestFoundSolutionMutex.lock();
	VectorSPtr bestFoundSolutionCopy = std::make_shared<std::vector<int>>(*mBestFoundSolution);
	mBestFoundSolutionMutex.unlock();

	if (false)
	{
		PrintSolution(bestFoundSolutionCopy, aArray, mGenerationCounter);
	}

	mBestFoundSolutionCost.store(CalcCost(bestFoundSolutionCopy, aArray));
	mAlgorithmLoopIsWorking.store(false);
	return mBestFoundSolutionCost.load();
}
