#ifndef EXMEM_H
#define EXMEM_H

#include <cstdint>

//****************************************************************************************************
// EX/MEM pipeline register for control lines, ALU calculated values, and write back register number *
//****************************************************************************************************
class EXMEM
{
private:
	bool memRead, memToReg, memWrite, reg_write; // control lines
	int32_t aluResult, storeByteValue; // calculated values
	uint32_t writeRegNum; // write back register number
public:
	EXMEM(); // constructor - initializes values

	// mutators
	void SetMemRead(bool val) { memRead = val; }
	void SetMemToReg(bool val) { memToReg = val; }
	void SetMemWrite(bool val) { memWrite = val; }
	void SetRegWrite(bool val) { reg_write = val; }
	void SetAluResult(int32_t val) { aluResult = val; }
	void SetSendBackValue(int32_t val) { storeByteValue = val; }
	void SetWriteRegNum(uint32_t val) { writeRegNum = val; }

	// accessors
	bool GetMemRead() const { return memRead; }
	bool GetMemToReg() const { return memToReg; }
	bool GetMemWrite() const { return memWrite; }
	bool GetRegWrite() const { return reg_write; }
	int32_t GetAluResult() const { return aluResult; }
	int32_t GetSendBackValue() const { return storeByteValue; }
	uint32_t GetWriteRegNum() const { return writeRegNum; }
};

EXMEM::EXMEM() // ctor - initializes pipeline register values
{
	memRead = memToReg = memWrite = reg_write = 0x0;
	aluResult = storeByteValue = 0x0;
	writeRegNum = 0x0;
}

#endif