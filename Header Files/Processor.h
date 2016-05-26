#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <fstream>
#include <iomanip>
#include "IFID.h"
#include "IDEX.h"
#include "EXMEM.h"
#include "MEMWB.h"

// bitmasks
const uint32_t
	OPCODE_BM{ 0xFC000000 },
	READ_REG1_BM{ 0x03E00000 },
	READ_REG2_BM{ 0x001F0000 },
	WRITE_REG_15_11_BM{ 0x0000F800 },
	WRITE_REG_20_16_BM{ 0x001F0000 },
	FUNCTION_BM{ 0x0000003F },
	OFFSET_BM{ 0x0000FFFF },
	SIGN_BM{ 0x00008000 };

// bitwise shifts
const uint32_t
	OPCODE_SHIFT{ 26 },
	READ_REG1_SHIFT{ 21 },
	READ_REG2_SHIFT{ 16 },
	WRITE_REG_15_11_SHIFT{ 11 },
	WRITE_REG_20_16_SHIFT{ 16 };

class Processor
{
private:
	IFID IFID_Write, IFID_Read;
	IDEX IDEX_Write, IDEX_Read;
	EXMEM EXMEM_Write, EXMEM_Read;
	MEMWB MEMWB_Write, MEMWB_Read;
	int32_t Regs[0x20];
public:
	Processor();
	void InstructionFetchStage(uint32_t);
	void InstructionDecodeStage();
	void ExecuteStage();
	void MemoryStage(int32_t *);
	void WriteBackStage();
	void Print(std::ofstream &) const;
	void Copy();
};

//***********************************************
// Constructor - initialize processor registers *
//***********************************************
Processor::Processor()
{
	Regs[0x0] = 0x0;

	for (size_t i{ 0x1 }; i < 0x20; ++i) { Regs[i] = 0x100 + i; }
}

//*************************************************
// Instruction fetch stage - take the instruction *
// and put it in the IF/ID pipeline register      *
//*************************************************

void Processor::InstructionFetchStage(uint32_t instruction) { IFID_Write.SetInstruction(instruction); }

//***********************************************************
// Instruction decode stage - take the instruction from the *
// IF/ID pipeline register, decode it and set control lines *
//***********************************************************
void Processor::InstructionDecodeStage()
{
	// fetch register information
	IDEX_Write.SetReadReg1Value(Regs[(IFID_Read.GetInstruction() & READ_REG1_BM) >> READ_REG1_SHIFT]);
	IDEX_Write.SetReadReg2Value(Regs[(IFID_Read.GetInstruction() & READ_REG2_BM) >> READ_REG2_SHIFT]);
	IDEX_Write.SetWriteReg15_11((IFID_Read.GetInstruction() & WRITE_REG_15_11_BM) >> WRITE_REG_15_11_SHIFT);
	IDEX_Write.SetWriteReg20_16((IFID_Read.GetInstruction() & WRITE_REG_20_16_BM) >> WRITE_REG_20_16_SHIFT);

	// decode function
	IDEX_Write.SetFunction(IFID_Read.GetInstruction() & FUNCTION_BM);

	// calculate sign extended offset
	uint32_t offset{ IFID_Read.GetInstruction() & OFFSET_BM };
	if (SIGN_BM & IFID_Read.GetInstruction()) { offset += 0xFFFF0000; } // if bit 16 is a 1, extend with 0xFFFF
	IDEX_Write.SetSignExtendedOffset(offset); // set the offset

	// set control lines based on the opcode
	if (IFID_Read.GetInstruction() == 0x0) // no-op
	{
		IDEX_Write.SetAluOp(0);
		IDEX_Write.SetAluSrc(0);
		IDEX_Write.SetMemRead(0);
		IDEX_Write.SetMemToReg(0);
		IDEX_Write.SetMemWrite(0);
		IDEX_Write.SetRegDest(0);
		IDEX_Write.SetRegWrite(0);
	}
	else if (((IFID_Read.GetInstruction() & OPCODE_BM) >> OPCODE_SHIFT) == 0x0) // r-format
	{
		IDEX_Write.SetAluOp(10);
		IDEX_Write.SetAluSrc(0);
		IDEX_Write.SetMemRead(0);
		IDEX_Write.SetMemToReg(0);
		IDEX_Write.SetMemWrite(0);
		IDEX_Write.SetRegDest(1);
		IDEX_Write.SetRegWrite(1);
	}
	else if (((IFID_Read.GetInstruction() & OPCODE_BM) >> OPCODE_SHIFT) == 0x20) // load byte
	{
		IDEX_Write.SetAluOp(00);
		IDEX_Write.SetAluSrc(1);
		IDEX_Write.SetMemRead(1);
		IDEX_Write.SetMemToReg(1);
		IDEX_Write.SetMemWrite(0);
		IDEX_Write.SetRegDest(0);
		IDEX_Write.SetRegWrite(1);
	}
	else if (((IFID_Read.GetInstruction() & OPCODE_BM) >> OPCODE_SHIFT) == 0x28) // store byte
	{
		IDEX_Write.SetAluOp(00);
		IDEX_Write.SetAluSrc(1);
		IDEX_Write.SetMemRead(0);
		IDEX_Write.SetMemToReg(NULL); // using NULL here to denote that this value doesn't matter - still treated as 0
		IDEX_Write.SetMemWrite(1);
		IDEX_Write.SetRegDest(NULL); // using NULL here to denote that this value doesn't matter - still treated as 0
		IDEX_Write.SetRegWrite(0);
	}
}

void Processor::ExecuteStage()
{
	// pass over control signals
	EXMEM_Write.SetMemRead(IDEX_Read.GetMemRead());
	EXMEM_Write.SetMemToReg(IDEX_Read.GetMemToReg());
	EXMEM_Write.SetMemWrite(IDEX_Read.GetMemWrite());
	EXMEM_Write.SetRegWrite(IDEX_Read.GetRegWrite());

	if (IDEX_Read.GetAluOp() == 10 && IDEX_Read.GetAluSrc() == 0) // r-format
	{
		EXMEM_Write.SetWriteRegNum(IDEX_Read.GetWriteReg15_11());

		if (IDEX_Read.GetFunction() == 0x20) // add instruction
		{
			EXMEM_Write.SetAluResult((IDEX_Read.GetReadReg1Value() + IDEX_Read.GetReadReg2Value()));
		}
		else if (IDEX_Read.GetFunction() == 0x22) // subtract instruction
		{
			EXMEM_Write.SetAluResult((IDEX_Read.GetReadReg1Value() - IDEX_Read.GetReadReg2Value()));
		}
	}
	else
	{
		EXMEM_Write.SetWriteRegNum(IDEX_Read.GetWriteReg20_16());
		EXMEM_Write.SetAluResult((IDEX_Read.GetReadReg1Value() + IDEX_Read.GetSignExtendedOffset())); // alu_src == 1
	}

	EXMEM_Write.SetSendBackValue(IDEX_Read.GetReadReg2Value()); // this would be passed over in either case
}

void Processor::MemoryStage(int32_t * mainMem)
{
	// pass values over
	MEMWB_Write.SetMemToReg(EXMEM_Read.GetMemToReg());
	MEMWB_Write.SetRegWrite(EXMEM_Read.GetRegWrite());
	MEMWB_Write.SetWriteRegNum(EXMEM_Read.GetWriteRegNum());
	MEMWB_Write.SetAluResult(EXMEM_Read.GetAluResult());

	if (EXMEM_Read.GetMemRead() == 1) // load byte
	{
		MEMWB_Write.SetLoadByteValue(mainMem[EXMEM_Read.GetAluResult()]); // load the value from requested address
	}
	else if (EXMEM_Read.GetMemWrite() == 1) // store byte
	{
		mainMem[EXMEM_Read.GetAluResult()] = EXMEM_Read.GetSendBackValue();
		MEMWB_Write.SetLoadByteValue(NULL); // I use NULL here to denote that this value doesn't matter
	}
}

void Processor::WriteBackStage()
{
	if (MEMWB_Read.GetRegWrite() == 1)
	{
		if (MEMWB_Read.GetMemToReg() == 1) // load byte
		{
			Regs[MEMWB_Read.GetWriteRegNum()] = MEMWB_Read.GetLoadByteValue();
		}
		else // r-format
		{
			Regs[MEMWB_Read.GetWriteRegNum()] = MEMWB_Read.GetAluResult();
		}
	}
}

//******************************************************
// Print stage - print out everything... this is messy *
//******************************************************
void Processor::Print(std::ofstream & outputFile) const
{
	outputFile << std::hex << std::uppercase;
	outputFile << std::setw(79) << std::setfill('*') << '*';
	outputFile << std::endl << std::endl << std::endl;

	outputFile << "IF/ID Write: " << std::endl << std::endl;
	outputFile << "\t Instruction: 0x" << std::setw(8) << std::setfill('0') << IFID_Write.GetInstruction(); 
	outputFile << std::endl << std::endl << std::endl;

	outputFile << "IF/ID Read: " << std::endl << std::endl;
	outputFile << "\t Instruction: 0x" << std::setw(8) << std::setfill('0') << IFID_Read.GetInstruction();
	outputFile << std::endl << std::endl << std::endl << std::endl;

	outputFile << "ID/EX Write: " << std::endl << std::endl;
	outputFile << "\t signExtendedOffset: 0x" << std::setw(8) << std::setfill('0') << IDEX_Write.GetSignExtendedOffset() << std::setfill(' ') << std::endl;
	outputFile << "\t function: 0x" << IDEX_Write.GetFunction() << std::endl;
	outputFile << "\t readReg1Value: 0x" << IDEX_Write.GetReadReg1Value();
	outputFile << "\t readReg2Value: 0x" << IDEX_Write.GetReadReg2Value() << std::endl;
	outputFile << "\t writeReg15_11: " << std::dec << IDEX_Write.GetWriteReg15_11();
	outputFile << "\t writeReg20_16: " << IDEX_Write.GetWriteReg20_16() << std::endl;
	outputFile << "\n\t control: aluOp: " << IDEX_Write.GetAluOp() << ", aluSrc: " << IDEX_Write.GetAluSrc();
	outputFile << ", memRead: " << IDEX_Write.GetMemRead() << ", memToReg: " << IDEX_Write.GetMemToReg() << std::endl;
	outputFile << "\t\t  memWrite: " << IDEX_Write.GetMemWrite() << ", regDst: " << IDEX_Write.GetRegDest();
	outputFile << ", regWrite: " << IDEX_Write.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "ID/EX Read: " << std::endl << std::endl;
	outputFile << "\t signExtendedOffset: " << "0x" << std::setw(8) << std::setfill('0') << IDEX_Read.GetSignExtendedOffset() << std::setfill(' ') << std::endl;
	outputFile << "\t function: 0x" << IDEX_Read.GetFunction() << std::endl;
	outputFile << "\t readReg1Value: 0x" << IDEX_Read.GetReadReg1Value();
	outputFile << "\t readReg2Value: 0x" << IDEX_Read.GetReadReg2Value() << std::endl;
	outputFile << "\t writeReg15_11: " << std::dec << IDEX_Read.GetWriteReg15_11();
	outputFile << "\t writeReg20_16: " << IDEX_Read.GetWriteReg20_16() << std::endl;
	outputFile << "\n\t control: aluOp: " << IDEX_Read.GetAluOp() << ", aluSrc: " << IDEX_Read.GetAluSrc();
	outputFile << ", memRead: " << IDEX_Read.GetMemRead() << ", memToReg: " << IDEX_Read.GetMemToReg() << std::endl;
	outputFile << "\t\t  memWrite: " << IDEX_Read.GetMemWrite() << ", regDst: " << IDEX_Read.GetRegDest();
	outputFile << ", regWrite: " << IDEX_Read.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "EX/MEM Write: " << std::endl << std::endl;
	outputFile << "\t aluResult: " << EXMEM_Write.GetAluResult() << ", storeByteValue: " << EXMEM_Write.GetSendBackValue() << std::dec;
	outputFile << ", writeRegNum: " << EXMEM_Write.GetWriteRegNum() << std::endl;
	outputFile << "\t control: memRead: " << EXMEM_Write.GetMemRead() << ", memToReg: " << EXMEM_Write.GetMemToReg();
	outputFile << ", memWrite: " << EXMEM_Write.GetMemWrite() << ", regWrite: " << EXMEM_Write.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "EX/MEM Read: " << std::endl << std::endl;
	outputFile << "\t aluResult: " << EXMEM_Read.GetAluResult() << ", storeByteValue: " << EXMEM_Read.GetSendBackValue() << std::dec;
	outputFile << ", writeRegNum: " << EXMEM_Read.GetWriteRegNum() << std::endl;
	outputFile << "\t control: memRead: " << EXMEM_Read.GetMemRead() << ", memToReg: " << EXMEM_Read.GetMemToReg();
	outputFile << ", memWrite: " << EXMEM_Read.GetMemWrite() << ", regWrite: " << EXMEM_Read.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "MEM/WB Write: " << std::endl << std::endl;
	outputFile << "\t aluResult: " << MEMWB_Write.GetAluResult() << ", loadByteValue: " << MEMWB_Write.GetLoadByteValue();
	outputFile << std::dec << ", writeRegNum: " << MEMWB_Write.GetWriteRegNum() << std::endl << std::endl;
	outputFile << "\t control: memToReg: " << MEMWB_Write.GetMemToReg() << ", regWrite: " << MEMWB_Write.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "MEM/WB Read: " << std::endl << std::endl;
	outputFile << "\t aluResult: " << MEMWB_Read.GetAluResult() << ", loadByteValue: " << MEMWB_Read.GetLoadByteValue();
	outputFile << std::dec << ", writeRegNum: " << MEMWB_Read.GetWriteRegNum() << std::endl << std::endl;
	outputFile << "\t control: memToReg: " << MEMWB_Read.GetMemToReg() << ", regWrite: " << MEMWB_Read.GetRegWrite();
	outputFile << std::endl << std::endl << std::endl << std::hex;

	outputFile << "Registers: " << std::endl << std::endl;
	for (size_t i{ 0x0 }; i < 0x20; ++i)
	{
		if (i % 4 == 0) { outputFile << std::endl; } // formatting

		if (Regs[i] < 0x10) // formatting
		{
			outputFile << std::setw(6) << std::dec << i << ": 0x" << std::hex << Regs[i] << "  ";
		}
		else if (Regs[i] < 0x100)
		{
			outputFile << std::setw(6) << std::dec << i << ": 0x" << std::hex << Regs[i] << " ";
		}
		else
		{
			outputFile << std::setw(6) << std::dec << i << ": 0x" << std::hex << Regs[i];
		}
	}

	outputFile << std::endl << std::endl << std::endl;
}

//********************************************
// Copy stage - copy write to read pipelines *
//********************************************

void Processor::Copy()
{
	IFID_Read = IFID_Write;
	IDEX_Read = IDEX_Write;
	EXMEM_Read = EXMEM_Write;
	MEMWB_Read = MEMWB_Write;
}

#endif