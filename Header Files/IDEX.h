#ifndef IDEX_H
#define IDEX_H

#include <cstdint>

//*************************************************************************************
// ID/EX pipeline register for control lines, parsed instruction, and register values *
//*************************************************************************************
class IDEX
{
	private:
		bool aluSrc, memRead, memToReg, memWrite, regDest, regWrite; // control lines
		uint32_t aluOp, seOffset, function, writeReg15_11, writeReg20_16; // parsed instruction parts
		int32_t readReg1Value, readReg2Value; // register values
	public:
		IDEX(); // constructor - initializes values 

		// mutators
		void SetAluSrc(bool val) { aluSrc = val; }
		void SetMemRead(bool val) { memRead = val; }
		void SetMemToReg(bool val) { memToReg = val; }
		void SetMemWrite(bool val) { memWrite = val; }
		void SetRegDest(bool val) { regDest = val; }
		void SetRegWrite(bool val) { regWrite = val; }
		void SetAluOp(uint32_t val) { aluOp = val; }
		void SetSignExtendedOffset(uint32_t val) { seOffset = val; }
		void SetFunction(uint32_t val) { function = val; }
		void SetWriteReg15_11(uint32_t val) { writeReg15_11 = val; }
		void SetWriteReg20_16(uint32_t val) { writeReg20_16 = val; }
		void SetReadReg1Value(int32_t val) { readReg1Value = val; }
		void SetReadReg2Value(int32_t val) { readReg2Value = val; }

		// accessors
		bool GetAluSrc() const { return aluSrc; }
		bool GetMemRead() const { return memRead; }
		bool GetMemToReg() const { return memToReg; }
		bool GetMemWrite() const { return memWrite; }
		bool GetRegDest() const { return regDest; }
		bool GetRegWrite() const { return regWrite; }
		uint32_t GetAluOp() const { return aluOp; }
		uint32_t GetSignExtendedOffset() const { return seOffset; }
		uint32_t GetFunction() const { return function; }
		uint32_t GetWriteReg15_11() const { return writeReg15_11; }
		uint32_t GetWriteReg20_16() const { return writeReg20_16; }
		int32_t GetReadReg1Value() const { return readReg1Value; }
		int32_t GetReadReg2Value() const { return readReg2Value; }
};

IDEX::IDEX() // constructor - initializes values
{
	aluSrc = memRead = memToReg = memWrite = regDest = regWrite = 0x0;
	aluOp = seOffset = function = writeReg15_11 = writeReg20_16 = 0x0;
	readReg1Value = readReg2Value = 0x0;
}

#endif