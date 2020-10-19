#include "GeneticsAlgorithmIsland.h"

void GeneticsAlgorithmIsland::GenerateStartupPopulation(int** inArray, int vertexNumber, std::vector<std::vector<int>*>*& populationVector,
	int populationMinSize, int populationMaxSize)
{
	populationVector = new std::vector<std::vector<int>*>[populationMaxSize];

	for (int i = 0; i < populationMaxSize - populationMinSize; ++i)
	{
		int* tab_i = new int[vertexNumber];
		for (int i = 0; i < vertexNumber; ++i)
			tab_i[i] = i;

		std::vector<int>* res = new std::vector<int>;
		for (int i = 0; i < vertexNumber; ++i)
		{
			int x = rand() % vertexNumber;
			if (tab_i[x] == -1)
			{
				--i;
				continue;
			}
			res->push_back(tab_i[x]);
			tab_i[x] = -1;
		}
		delete[] tab_i;
		populationVector->push_back(res);
	}
}

int GeneticsAlgorithmIsland::CalcCost(std::vector<int>* solution, int** adjacencyMatrix)
{
	if (solution->size() <= 1)
		return -1;

	int cost = 0;

	int limit = solution->size() - 1;
	for (int i = 0; i < limit; i++)
	{
		int a = solution->at(i);
		int b = solution->at(i + 1);
		cost += adjacencyMatrix[a][b];
	}

	cost += adjacencyMatrix[solution->at(solution->size() - 1)][solution->at(0)];

	return cost;
}

void GeneticsAlgorithmIsland::QuickSort(int left, int right, int* const pathsSize, std::vector<std::vector<int>*>* populationVector)
{
	int i, j, piwot;

	i = (left + right) / 2;

	piwot = pathsSize[i];
	pathsSize[i] = pathsSize[right];

	std::vector<int>* tmp = (*populationVector)[i];
	(*populationVector)[i] = (*populationVector)[right];

	for (j = i = left; i < right; i++)
	{
		if (pathsSize[i] < piwot)
		{
			if (j == 3 && i == 4)
			{
				int a;
			}
			// Swap values.
			swap(pathsSize[i], pathsSize[j]);

			populationVector->swap(i, j);
			j++;
		}
	}

	pathsSize[right] = pathsSize[j];
	pathsSize[j] = piwot;

	(*populationVector)[right] = (*populationVector)[j];
	(*populationVector)[j] = tmp;

	if (left < j - 1)
	{
		QuickSort(left, j - 1, pathsSize, populationVector);
	}

	if (j + 1 < right)
	{
		QuickSort(j + 1, right, pathsSize, populationVector);
	}
}

void GeneticsAlgorithmIsland::SortPopulation(std::vector<std::vector<int>*>* populationVector, int** inArray)
{
	//
	// Calculate paths.
	//

	int populationCurrentSize = populationVector->size();

	int* pathsSize = new int[populationCurrentSize];

	for (int i = 0; i < populationCurrentSize; i++)
	{
		pathsSize[i] = CalcCost((*populationVector)[i], inArray);
	}

	//
	// Sort population.
	//

	QuickSort(0, populationCurrentSize - 1, pathsSize, populationVector);

	//
	// Clear after sort.
	//

	delete[] pathsSize;
	pathsSize = nullptr;
}

void GeneticsAlgorithmIsland::ReducePopulation(std::vector<std::vector<int>*>* populationVector, int newSize)
{
	while (populationVector->size() > newSize)
	{
		std::vector<int>* tmp = populationVector->pop_back();
		delete tmp;
	}
}

void GeneticsAlgorithmIsland::CrossPopulation(std::vector<std::vector<int>*>* populationVector, int populationMaxSize)
{
	int populationStartupSize = populationVector->size();

	for (int i = 0; i < populationMaxSize - populationStartupSize; ++i)
	{
		//
		// Select 2 parents.
		//

		int rand1 = rand() % populationStartupSize;
		int rand2 = rand() % populationStartupSize;

		std::vector<int>* parent_a = populationVector->at(rand1);
		std::vector<int>* parent_b = populationVector->at(rand2);

		//
		// Cross parents.
		//

		std::vector<int>* child;

		int parentSize = parent_b->size();

		child = new std::vector<int>(parentSize);

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

		populationVector->push_back(child);
	}
}

void GeneticsAlgorithmIsland::MutatePopulation(std::vector<std::vector<int>*>* populationVector, float mutateFactor, int parrentNumber)
{
	//for (int i = parrentNumber; i < populationVector->size(); ++i)
	for (int i = 1; i < populationVector->size(); ++i)
	{
		if (rand() % 101 <= mutateFactor * 100)
		{
			Mutate(populationVector->at(i));
		}
	}
}

void GeneticsAlgorithmIsland::Mutate(std::vector<int>* instance)
{
	int vectorSize = instance->size();

	int begin = rand() % vectorSize;

	int end = begin + rand() % (vectorSize - begin);

	int numberOfNode = end - begin;

	if (numberOfNode < 2)
		return;

	//
	// Reverse.
	//

	std::vector<int> backupInstance(numberOfNode);

	for (int i = 0; i < numberOfNode; i++)
	{
		backupInstance.push_back(instance->at(i));
	}

	for (int i = 0; i < numberOfNode; i++)
	{
		(*instance)[i] = backupInstance.pop_back();
	}
}

void GeneticsAlgorithmIsland::CheckBestSolution(std::vector<std::vector<int>*>* populationVector, int** adjacencyMatrix,
	std::vector<int>& bestSolution, int& worseSolutionCounter)
{
	if (bestSolution.size() == 0)
	{
		bestSolution = *(*populationVector)[0];
		return;
	}

	int first = CalcCost((*populationVector)[0], adjacencyMatrix);
	int lassTheBest = CalcCost(&bestSolution, adjacencyMatrix);

	if (first < lassTheBest)
	{
		bestSolution = *(*populationVector)[0];
		worseSolutionCounter = 0;
	}
	else
	{
		worseSolutionCounter++;
	}
}

void GeneticsAlgorithmIsland::PrintSolution(std::vector<int>* solution, int** adjacencyMatrix)
{
	for (int i = 0; i != solution->size(); i++)
	{
		std::cout << solution->at(i) << " -> ";
	}
	std::cout << solution->at(0) << std::endl;

	std::cout << "\tSum: " << CalcCost(solution, adjacencyMatrix) << std::endl << std::endl;
}

int GeneticsAlgorithmIsland::Genetic(int** aArray, int aWidth, int test, int populationMinSize, int populationMaxSize,
	float mutateFactor, int limitOfWorstSolution, int numberOfThreads, int* aPopulationCounter)
{
	std::vector<std::vector<int>*>* populationVector = nullptr;
	std::vector<int> bestSolution;

	int worseSolutionCounter = 0;
	int populationCounter = 0;

	GenerateStartupPopulation(aArray, aWidth, populationVector, populationMinSize, populationMaxSize);

	CheckBestSolution(populationVector, aArray, bestSolution, worseSolutionCounter);

	SortPopulation(populationVector, aArray);

	while (worseSolutionCounter < limitOfWorstSolution)
	{
		ReducePopulation(populationVector, populationMinSize);
		CrossPopulation(populationVector, populationMaxSize);
		MutatePopulation(populationVector, mutateFactor, populationMinSize);
		SortPopulation(populationVector, aArray);
		CheckBestSolution(populationVector, aArray, bestSolution, worseSolutionCounter);

		populationCounter++;
	}

	if (test == false)
	{
		PrintSolution(&bestSolution, aArray);
	}

	if (aPopulationCounter != nullptr)
	{
		*aPopulationCounter = populationCounter;
	}

	while (!populationVector->empty())
	{
		delete populationVector->pop_back();
	}

	return CalcCost(&bestSolution, aArray);
}
