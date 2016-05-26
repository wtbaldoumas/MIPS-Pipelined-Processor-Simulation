#ifndef MEMWB_H
#define MEMWB_H

#include <cstdint>

//***********************************************************************************************
// MEM/WB pipeline register for control lines, ALU calculated values, and write register number *
//***********************************************************************************************
class MEMWB
{
private:
	bool memToReg, regWrite; // control lines
	int32_t loadByteValue, aluResult; // ALU calculated values
	uint32_t writeRegNum; // write register number
public:
	MEMWB(); // constructor - initializes values

	// mutators
	void SetMemToReg(bool val) { memToReg = val; }
	void SetRegWrite(bool val) { regWrite = val; }
	void SetLoadByteValue(int32_t val) { loadByteValue = val; }
	void SetAluResult(int32_t val) { aluResult = val; }
	void SetWriteRegNum(uint32_t val) { writeRegNum = val; }

	// accessors
	bool GetMemToReg() const { return memToReg; }
	bool GetRegWrite() const { return regWrite; }
	int32_t GetLoadByteValue() const { return loadByteValue; }
	int32_t GetAluResult() const { return aluResult; }
	uint32_t GetWriteRegNum() const { return writeRegNum; }
};

MEMWB::MEMWB() // ctor - initializes values
{
	memToReg = regWrite = 0x0;
	loadByteValue = aluResult = 0x0;
	writeRegNum = 0x0;
}

#endif