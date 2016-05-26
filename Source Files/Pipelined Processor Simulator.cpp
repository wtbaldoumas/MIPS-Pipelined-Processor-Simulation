#include <cstdlib>
#include <string>
#include "Processor.h"

void InitializeMainMemory(int32_t *, const uint32_t);
size_t LoadInputFile(std::ifstream &);
void LoadOutputFile(std::ofstream &);
void CleanUp(std::ofstream &, uint32_t *);

int main()
{
	Processor processor;
	uint32_t * inputInstructions{ nullptr };
	int32_t mainMemory[0x400]{ 0x0 };
	std::ifstream inputFile;
	std::ofstream outputFile;

	InitializeMainMemory(mainMemory, 0x400);
	uint32_t count{ LoadInputFile(inputFile) };
	LoadOutputFile(outputFile);

	inputInstructions = new uint32_t[count];

	std::string input{ "" };
	for (size_t i{ 0 }; i < count; ++i)
	{
		getline(inputFile, input);
		inputInstructions[i] = strtoul(input.c_str(), NULL, 16);
	}

	inputFile.close();

	for (size_t i{ 0 }; i < count; ++i)
	{
		processor.InstructionFetchStage(inputInstructions[i]);
		processor.InstructionDecodeStage();
		processor.ExecuteStage();
		processor.MemoryStage(mainMemory);
		processor.WriteBackStage();
		processor.Print(outputFile);
		processor.Copy();
	}

	CleanUp(outputFile, inputInstructions);

	return EXIT_SUCCESS;
}