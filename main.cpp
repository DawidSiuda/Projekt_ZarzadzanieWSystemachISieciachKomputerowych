#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <chrono> 
#include <conio.h>
#include <sstream>
#include <iomanip>
#include <map>

#include "Genetic.h"
#include "MyVector.h"
#include "GeneticAlgorithmIslandsSupervisor.h"

using namespace std;
using namespace std::chrono;

int readGrapgFromFile(std::string path, int**& array, int& width);
void printArray(int** array, int width);
void emptyArray(int**& array, int width);

int main()
{
	srand(time(NULL));
	
	int** array = nullptr;
	int width;
	int returnValue;
	int option;

	do
	{
		cout << "========================================\n" 
			<< "MENU\n"
			<< "1) Wczytaj graf\n"
			<< "2) Wyswietl graf\n"
			<< "3) Algorytm genetyczny\n"
			<< "0) Wyjscie\n";

		option = _getch();

		system("cls");

		switch (option)
		{
			case '0':
			{
				if (array != nullptr)
					emptyArray(array, width);
				exit(0);
				break;
			}
			case '1':
			{
				if (array != nullptr)
					emptyArray(array, width);

				string defaultName("file04-47-1776.txt");
				
				cout << "File name (" << defaultName <<"): ";

				std::string input;
				std::getline(std::cin, input);

				if (input.empty()) 
				{
					input = defaultName;
				}

				returnValue = readGrapgFromFile(input, array, width);

				if (returnValue == -1)
				{
					cout << "ERROR: Cannot read graph.\n";
				}

				break;
			}
			case '2':
			{
				if (array != nullptr)
					printArray(array, width);
				else
					cout << "ERROR: No load graph.\n";
				break;
			}
			case '3':
			{
				cout << "1) Manual test\n";
				cout << "2) Automatic test\n";

				option = _getch();
				switch (option)
				{
					case '1':
					{
						if (array != nullptr)
						{
							int populationMinSize = 20;
							int populationMaxSize = 40;
							float mutateFactor = 0.1;
							int limitOfWorstSolution = 1500;
							int numberOfThreads = 4;
							string input;

							cout << "population startup size (" << populationMinSize << "): ";

							getline(cin, input);

							if (input.empty() == false)
							{
								try
								{
									populationMinSize = stoi(input);
								}
								catch (...) {}
							}

							cout << "population final size (" << populationMaxSize << "): ";

							getline(cin, input);

							if (input.empty() == false)
							{
								try
								{
									populationMaxSize = stoi(input);
								}
								catch (...) {}
							}

							cout << "mutate factor (" << mutateFactor << "): ";

							getline(cin, input);

							if (input.empty() == false)
							{
								try
								{
									mutateFactor = stof(input);
								}
								catch (...) {}
							}

							cout << "number of worst solution (" << limitOfWorstSolution << "): ";

							getline(cin, input);

							if (input.empty() == false)
							{
								try
								{
									limitOfWorstSolution = stoi(input);
								}
								catch (...) {}
							}

							cout << "number of threads (" << numberOfThreads << "): ";

							getline(cin, input);

							if (input.empty() == false)
							{
								try
								{
									numberOfThreads = stoi(input);
								}
								catch (...) {}
							}

							GeneticAlgorithmIslandsSupervisor gen(array,
								width,
								false,
								populationMinSize,
								populationMaxSize,
								mutateFactor,
								numberOfThreads,
								limitOfWorstSolution);
							gen.StartAlgorithmLoop();
							break;
						}
						else
						{
							cout << "ERROR: No load graph.\n";
						}

						break;
					}
					case '2':
					{
						std::map<string, int> filemap;
						filemap["file01-6-80.txt"] = 80;
						filemap["file02-17-2085.txt"] = 2085;
						filemap["file03-29-2020.txt"] = 2020;
						filemap["file04-47-1776.txt"] = 1776;
						filemap["file05-64-1839.txt"] = 1839;
						filemap["file06-70-1950.txt"] = 1950;
						filemap["file07-170-2755.txt"] = 2755;
						
						int populationMinSize = 20;
						int populationMaxSize = 40;
						float mutateFactor = 0.2;
						int limitOfWorstSolution = 3000;
						int maxNumberOfthreads = 4;
						int numberOfTests = 10;

						for(auto it = filemap.begin(); it != filemap.end(); it++)
						{
							int returnValue = readGrapgFromFile(it->first, array, width);

							if (returnValue == -1)
							{
								cout << "ERROR: Cannot read graph.\n";
								return -1;
							}

							//for (mutateFactor = 0.1; mutateFactor <= 0.5; mutateFactor *= 2)
							//	for (limitOfWorstSolution = 750; limitOfWorstSolution <= 12000; limitOfWorstSolution *= 2)
									for (int numberOfThreads = 1; numberOfThreads <= maxNumberOfthreads; numberOfThreads++)
										for (int i = 0; i < numberOfTests; i++)
										{
											GeneticAlgorithmIslandsSupervisor gen(array,
												width,
												true,
												populationMinSize,
												populationMaxSize,
												mutateFactor,
												numberOfThreads,
												limitOfWorstSolution,
												it->second);
											gen.StartAlgorithmLoop();
										}
						}
						cout << endl;
						break;
					}
				}
				break;
			}
		}
	} while (1);
}

int readGrapgFromFile(std::string path, int**& array, int& width)
{
	array = nullptr;
	width = -1;

	std::string line;
	std::string temp;

	std::fstream file;

	file.open(path, std::ios::in);

	if (file.good() == false)
	{
		std::cout << "Cannot read file.\n";

		return -1;
	}

	int isFirstLine = true;

	file >> width;

	//std::cout << "Number of cities: " << width << "\n";

	array = new int* [width];

	for (int i = 0; i < width; ++i)
	{
		array[i] = new int[width];

		memset(array[i], 0, width * sizeof(int));
	}

	int row = 0;
	int collumn = 0;
	int value;

	int values = 0;
	while (file >> value)
	{
		array[row][collumn] = value;

		collumn++;

		if (collumn == width)
		{
			collumn = 0;
			row++;

			if (row == width)
			{
				break;
			}
		}
		values++;
	}

	file.close();

	return 0;
}

void printArray(int** array, int width)
{

	if (array == nullptr)
	{
		std::cout << "printArray(): ERROR: array is nulptr.";
		return;
	}

	std::cout << "printArray(): TABLE:\n";

	for (int i = 0; i < width; i++)
	{
		std::string line;

		for (int j = 0; j < width; j++)
		{
			std::cout.width(5);

			std::cout << array[i][j];
		}

		std::cout << std::endl;

	}
}

void emptyArray(int**& array, int width)
{
	for (int i = 0; i < width; ++i)
	{
		delete[] array[i];
	}

	delete[] array;

	array = nullptr;
}