#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

void InitializeMainMemory(int32_t * mainMemory, const uint32_t count)
{
	uint32_t initializer{ 0x0 };

	for (size_t i{ 0x0 }; i < 0x400; ++i)
	{
		if (initializer == 0x100)
		{
			initializer = 0x0;
		}

		mainMemory[i] = initializer;
		++initializer;
	}
}

size_t LoadInputFile(std::ifstream & inputFile)
{
	std::string filename{ "" };
	std::cout << "Enter the input file path and filename: ";
	getline(std::cin, filename);

	inputFile.open(filename);
	while (inputFile.fail())
	{
		std::cout << std::endl << "Error: invalid input file." << std::endl;
		std::cout << "Enter the path and name of the file: ";
		getline(std::cin, filename);
		inputFile.open(filename);
	}

	// get the size of the array of instructions to be created
	inputFile.unsetf(std::ios_base::skipws);
	size_t count = (std::count(std::istream_iterator<char>(inputFile), std::istream_iterator<char>(), '\n') + 1);

	// return to the top of the input file for reading
	inputFile.clear();
	inputFile.seekg(0, std::ios::beg);

	return count;
}

void LoadOutputFile(std::ofstream & outputFile)
{
	std::string filename{ "" };
	std::cout << "Enter the output file path and filename: ";
	getline(std::cin, filename);

	outputFile.open(filename);
	while (outputFile.fail())
	{
		std::cout << std::endl << "Error: invalid output file." << std::endl;
		std::cout << "Enter the path and name of the file: ";
		getline(std::cin, filename);
		outputFile.open(filename);
	}
}

void CleanUp(std::ofstream & outputFile, uint32_t * inputInstructions)
{
	outputFile.close();
	delete [] inputInstructions;
	inputInstructions = nullptr;
}