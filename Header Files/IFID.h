#ifndef IFID_H
#define IFID_H

#include <cstdint>

//**************************************************
// IF/ID pipeline register for caching instruction *
//**************************************************
class IFID
{
	private:
		uint32_t instruction;
	public:
		IFID() { instruction = 0x0; }
		void SetInstruction(uint32_t val) { instruction = val; }
		uint32_t GetInstruction() const { return instruction; }
};

#endif