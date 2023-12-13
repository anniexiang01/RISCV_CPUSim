#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
using namespace std;

class Instruction;

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 
	int regFile[32];

public:
	CPU();
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	void Decode(uint32_t instr);

	string operation; 

	int killCount;
	int clockCycles;
	int rTypeCount;
	int insCount;
	float ipc;

	int ReadDataMemory(int address);
	void WriteDataMemory(int address, int data);
	int ReadRegFile(int reg);
	void WriteRegFile(int reg, int value);
	void UpdatePC(int imm);
};

class Instruction {
	public:
		bitset<32> instr;//instruction
		CPU myCPU;

		Instruction(bitset<32> fetch, CPU myCPU); // constructor
};


// add other functions and objects here