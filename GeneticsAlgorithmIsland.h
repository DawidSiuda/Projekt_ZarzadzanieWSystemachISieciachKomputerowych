#pragma once
#include <time.h>       /* time */
#include <vector>
#include <mutex>
#include <iostream>

typedef std::shared_ptr<std::vector<std::shared_ptr<std::vector<int>>>> PopulationVector;
typedef std::shared_ptr<std::vector<int>> VectorSPtr;

class GeneticsAlgorithmIsland
{
public:
	GeneticsAlgorithmIsland();

	~GeneticsAlgorithmIsland();

	bool StartAlgorithm(int aWidth, int aPopulationMinSize, int aPopulationMaxSize,
		int mLimitOfWorstSolution, int** mArray, float mMutateFactor, bool aTest);

	bool StopAlgorithm();

	void SetNextNode(GeneticsAlgorithmIsland* aNextIsland);

	void SetPreviousNode(GeneticsAlgorithmIsland* aPreviousIsland);

	int GetGenerationNumber();

	int GetSolutionCost();

	bool GetBestSolution(VectorSPtr& aSolution);

	bool IsFinished();

	static void PrintSolution(VectorSPtr solution, int** adjacencyMatrix, int aPopulationCounter);

private:
	int mGenerationCounter;

	std::atomic<bool> mStopAlgorithmLoop;
	std::atomic<bool> mAlgorithmLoopIsWorking;
	std::unique_ptr<std::thread> mAlgorithmLoopThread;

	std::mutex mBestFoundSolutionMutex;
	VectorSPtr mBestFoundSolution;
	std::atomic<int> mBestFoundSolutionCost;

	std::mutex mUseOtherIslandsMutex;
	GeneticsAlgorithmIsland* mNextIsland;
	GeneticsAlgorithmIsland* mPreviousIsland;

	VectorSPtr mNextNodeBestSolutionBackup;
	VectorSPtr mPreviousNodeBestSolutionBackup;

	void GenerateStartupPopulation(int vertexNumber, PopulationVector& populationVector,
											int populationMinSize, int populationMaxSize);

	int CalcCost(VectorSPtr solution, int** adjacencyMatrix);

	void QuickSort(int left, int right, VectorSPtr pathsSize, PopulationVector populationVector);

	void SortPopulation(PopulationVector populationVector, int** inArray);

	void ReducePopulation(PopulationVector populationVector, int newSize);

	void MutatePopulation(PopulationVector populationVector, float mutateFactor, int aPopulationMinSize);

	void Mutate(VectorSPtr instance);

	void CheckAndSetBestSolution(PopulationVector populationVector, int** adjacencyMatrix, int& worseSolutionCounter);

	void CrossPopulation(PopulationVector populationVector, int populationMaxSize);

	bool DownloadBestSolutionFromNextNode(VectorSPtr& aSolution);

	bool DownloadBestSolutionFromPreviousNode(VectorSPtr& aSolution);

	void DownloadBestSolutionFromNextAndPreviousNode(PopulationVector aPopulationVector);

	int Genetic(int** aArray, int aWidth, int populationMinSize, int populationMaxSize, float mutateFactor,
					int limitOfWorstSolution, int aSeed, int test);
};

