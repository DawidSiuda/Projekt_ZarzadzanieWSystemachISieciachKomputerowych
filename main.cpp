#include <string>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <chrono> 
#include <conio.h>
#include <sstream>
#include <iomanip>

#include "Genetic.h"
#include "MyVector.h"

#define NUMBER_OF_TEST 1

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
			<< "6) Algorytm genetyczny\n"
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

				//string defaultName("graf.txt");
				//string defaultName("tsp_10b.txt");
				string defaultName("17-2085.txt");
				
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
			case '6':
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

							auto start = high_resolution_clock::now();
							Genetic(array, width, false, populationMinSize, populationMaxSize, mutateFactor, limitOfWorstSolution, numberOfThreads);
							auto stop = high_resolution_clock::now();
							auto duration = duration_cast<milliseconds>(stop - start);
							cout << "Time: " << duration.count() << " milliseconds" << endl;
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
						const int numberOfFiles = 7;
						string files[numberOfFiles] = { "6-80.txt",
														"17-2085.txt",
														"29-2020.txt",
														"47-1776.txt",
														"64-1839.txt",
														"70-1950.txt",
														"170-2755.txt" };

						int optimalSolution[numberOfFiles] = { 80, 2085, 2020, 1776, 1839, 1950, 2755 };

						cout << std::setw(PRINT_WIDTH + DAW) << "width"
							<< std::setw(PRINT_WIDTH + DAW) << "minSiz"
							<< std::setw(PRINT_WIDTH + DAW) << "maxSiz"
							<< std::setw(PRINT_WIDTH + DAW) << "mutFac"
							<< std::setw(PRINT_WIDTH + DAW) << "worstL"
							<< std::setw(PRINT_WIDTH + DAW) << "popCon"
							<< std::setw(PRINT_WIDTH + DAW) << "cost"
							<< std::setw(PRINT_WIDTH + DAW) << "optSolu"
							<< std::setw(PRINT_WIDTH + DAW) << "time"
							<< std::setw(PRINT_WIDTH + DAW) << "blad" << endl;

						for (int i = 0; i < numberOfFiles; i++) // Graph size.
						{
							int returnValue = readGrapgFromFile(files[i], array, width);

							if (returnValue == -1)
							{
								cout << "ERROR: Cannot read graph.\n";
								return -1;
							}

							int populationMinSize = 20;
							int populationMaxSize = 40;
							float mutateFactor = 0.1;
							int limitOfWorstSolution = 1500;

							for (mutateFactor = 0.1; mutateFactor <= 0.5; mutateFactor *= 2)
							{
								for (limitOfWorstSolution = 750; limitOfWorstSolution <= 12000; limitOfWorstSolution *= 2)
								{
									int averageTime = 0;
									int averagePopulationCounter = 0;
									int averageSolution = 0;

									for (int i = 0; i < NUMBER_OF_TEST; i++)
									{
										int popCounter;
										auto start = high_resolution_clock::now();
										averageSolution += genetic(array, width, true, populationMinSize, populationMinSize * 2,
											mutateFactor, limitOfWorstSolution, &popCounter);
										auto stop = high_resolution_clock::now();
										auto duration = duration_cast<milliseconds>(stop - start);

										averageTime += duration.count();
										averagePopulationCounter += popCounter;
									}


									averageTime /= NUMBER_OF_TEST;
									averagePopulationCounter /= NUMBER_OF_TEST;
									averageSolution /= NUMBER_OF_TEST;

									int other = 100 * ((float)averageSolution - (float)optimalSolution[i]) / (float)optimalSolution[i];

									std::cout << std::setw(PRINT_WIDTH) << width << "; ";

									std::cout << std::setw(PRINT_WIDTH) << populationMinSize << "; ";

									std::cout << std::setw(PRINT_WIDTH) << populationMaxSize << "; ";

									std::cout << std::setw(PRINT_WIDTH) << std::setprecision(1) << std::fixed << mutateFactor << "; ";

									std::cout << std::setw(PRINT_WIDTH) << limitOfWorstSolution << "; ";

									std::cout << std::setw(PRINT_WIDTH) << averagePopulationCounter << "; ";

									std::cout << std::setw(PRINT_WIDTH) << averageSolution << "; ";

									std::cout << std::setw(PRINT_WIDTH) << optimalSolution[i] << "; ";

									std::cout << std::setw(PRINT_WIDTH) << averageTime << "; ";

									std::cout << std::setw(PRINT_WIDTH) << other << "; ";

									std::cout << std::endl;
								}
							}

						}
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