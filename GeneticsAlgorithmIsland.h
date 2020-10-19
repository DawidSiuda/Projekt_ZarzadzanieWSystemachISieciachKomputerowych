#pragma once
#include <time.h>       /* time */
#include <vector>
#include <mutex>
#include <iostream>

class GeneticsAlgorithmIsland
{
public:
	GeneticsAlgorithmIsland();

private:
	std::mutex mLastBestSolutionMutex;
	std::vector<int> mLastBestSolution;

	static void GenerateStartupPopulation(int** inArray, int vertexNumber, std::vector<std::vector<int>*>*& populationVector,
											int populationMinSize, int populationMaxSize);
	int CalcCost(std::vector<int>* solution, int** adjacencyMatrix);
	void QuickSort(int left, int right, int* const pathsSize, std::vector<std::vector<int>*>* populationVector);
	void SortPopulation(std::vector<std::vector<int>*>* populationVector, int** inArray);
	void ReducePopulation(std::vector<std::vector<int>*>* populationVector, int newSize);
	void MutatePopulation(std::vector<std::vector<int>*>* populationVector, float mutateFactor, int parrentNumber);
	void Mutate(std::vector<int>* instance);
	void CheckBestSolution(std::vector<std::vector<int>*>* populationVector, int** adjacencyMatrix, std::vector<int>& bestSolution, int& worseSolutionCounter);
	void PrintSolution(std::vector<int>* solution, int** adjacencyMatrix);
	void CrossPopulation(std::vector<std::vector<int>*>* populationVector, int populationMaxSize);
	int Genetic(int** aArray, int aWidth, int test, int populationMinSize, int populationMaxSize,
					float mutateFactor, int limitOfWorstSolution, int numberOfThreads, int* aPopulationCounter);

};

