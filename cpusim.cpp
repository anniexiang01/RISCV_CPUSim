#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and 
	load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either 
	dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). 
	Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an unsigned char and is 1 byte 
	(each four lines are one instruction). You need to read the file line by 
	line and store it into the memory. You may need a mechanism to convert these 
	values to bits so that you can read opcodes, operands, etc.
	*/

	bitset<8> instMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
			infile>>line;
			stringstream line2(line);
			int x; 
			line2>>x;
			instMem[i] = bitset<8>(x);
			i++;
		}
	int maxPC= i; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project 
	that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction,
	decoding, etc.).
	*/

	CPU myCPU;  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 
	/* OPTIONAL: Instantiate your Instruction object here. */

	// control signals
	bool RegWrite; 
	bool ImmGen;
	bool Branch;
	bool MemRead;
	bool MemWrite;
	bool MemToReg;

	int nextPCOffset;

	bitset<32> curr;
	Instruction instruction = Instruction(curr, myCPU);
	while (true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		//fetch
		curr = myCPU.Fetch(instMem); // fetching the instruction
		instruction = Instruction(curr, myCPU);
		myCPU.insCount++;

		// decode
		uint32_t instrBits = instruction.instr.to_ulong();
		myCPU.Decode(instrBits);
		// cout << myCPU.operation << endl;

		// set control flags
		if (myCPU.operation == "ADD" || myCPU.operation == "SUB" || myCPU.operation == "XOR" || myCPU.operation == "SRA") { // R-Type
			RegWrite = true;
			ImmGen = false;
			Branch = false;
			MemRead = false;
			MemWrite = false;
			MemToReg = false;
		}
		else if (myCPU.operation == "ADDI" || myCPU.operation == "ANDI"){ //I-Type
			RegWrite = true;
			ImmGen = true;
			Branch = false;
			MemRead = false;
			MemWrite = false;
			MemToReg = false;
		}
		else if (myCPU.operation == "LW"){
			RegWrite = true;
			ImmGen = true;
			Branch = false;
			MemRead = true;
			MemWrite = false;
			MemToReg = true;
		}
		else if (myCPU.operation == "SW"){
			RegWrite = false;
			ImmGen = true;
			Branch = false;
			MemRead = false;
			MemWrite = true;
			MemToReg = false;
		}
		else if (myCPU.operation == "BLT"){
			RegWrite = false;
			ImmGen = false;
			Branch = true;
			MemRead = false;
			MemWrite = false;
			MemToReg = false;
		}
		else if (myCPU.operation == "JALR"){
			RegWrite = true;
			ImmGen = true;
			Branch = true;
			MemRead = false;
			MemWrite = false;
			MemToReg = false;
		}
		else if (myCPU.operation == "NOP"){
			myCPU.UpdatePC(myCPU.readPC() + 4);
			myCPU.clockCycles++;
			myCPU.killCount++;
			if (myCPU.killCount >= 1) { // end of execution
				myCPU.ipc = (float) myCPU.insCount / myCPU.clockCycles;

				cout << "(" << myCPU.ReadRegFile(10) << "," << myCPU.ReadRegFile(11) << ")" << endl; //change indicies as neccessary

				//cout << "Clock Cycles: " << myCPU.clockCycles << endl;
				//cout << "R-Type Instruction Count: " << myCPU.rTypeCount << endl;
				//cout << "Instruction Count: " << myCPU.insCount << endl;
				//cout << "IPC: "<< myCPU.ipc << endl;

				break;
			}
			continue;
		}
		else {
			break;
		}

		// execute
		int aluOutput;
		int imm;

		if (!ImmGen){ //R-Type or BLT
			int rs1 = (instrBits >> 15) & 0b11111;
			int rs2 = (instrBits >> 20) & 0b11111;
			
			if (myCPU.operation =="ADD"){
				aluOutput = myCPU.ReadRegFile(rs1) + myCPU.ReadRegFile(rs2);
			}
			else if (myCPU.operation == "SUB"){
				aluOutput = myCPU.ReadRegFile(rs1) - myCPU.ReadRegFile(rs2);
			}
			else if (myCPU.operation == "XOR"){
				aluOutput = myCPU.ReadRegFile(rs1) ^ myCPU.ReadRegFile(rs2);
			}
			else if (myCPU.operation == "SRA"){
				aluOutput = myCPU.ReadRegFile(rs1) >> myCPU.ReadRegFile(rs2);
			}
			else if (myCPU.operation == "BLT"){
				if (myCPU.ReadRegFile(rs1) < myCPU.ReadRegFile(rs2)){

					imm = ((instrBits >> 31) << 12) |
						(((instrBits >> 7) & 0b1) << 11) |
						(((instrBits >> 25) & 0b1111111) << 5) |
						(((instrBits >> 8) & 0b1111) << 1);
					
					if (imm >> 11 == 1){ 
						imm = -((~imm & 0b111111111111) + 1);
					}
					aluOutput = imm;
				}
				else {
					Branch = false;
				}
			}
		}
		else {
			int rs1 = (instrBits >> 15) & 0b11111;

			if (myCPU.operation == "SW"){
				imm = (((instrBits >> 25) & 0b1111111) << 5) | ((instrBits >> 7) & 0b11111);
			}
			else
				imm = (instrBits >> 20) & 0b111111111111;

			if (imm >> 11 == 1){ 
				imm = -((~imm & 0b111111111111) + 1);
			}

			if (myCPU.operation == "ADDI"){
				aluOutput = myCPU.ReadRegFile(rs1) + imm;
			}
			else if (myCPU.operation == "ANDI"){
				aluOutput = myCPU.ReadRegFile(rs1) & imm;
			}
			else if (myCPU.operation == "LW"){
				aluOutput = myCPU.ReadRegFile(rs1) + imm;
			}
			else if (myCPU.operation == "SW"){
				aluOutput = myCPU.ReadRegFile(rs1) + imm;
			}
			else if (myCPU.operation == "JALR"){
				int rd = (instrBits >> 7) & 0b11111;
				aluOutput = myCPU.ReadRegFile(rs1) + imm;
				myCPU.WriteRegFile(rd, myCPU.readPC() + 4);
			}
		}

		int rd = (instrBits >> 7) & 0b11111;

		// memory + write back
		if (MemRead && MemToReg) { //LW
			myCPU.WriteRegFile(rd, myCPU.ReadDataMemory(aluOutput));
			// cout << "RegFile at " << rd << ": " << myCPU.ReadRegFile(rd) << endl;
		}

		else if (MemWrite) { //SW
			int rs2 = (instrBits >> 20) & 0b11111;
			myCPU.WriteDataMemory(aluOutput, myCPU.ReadRegFile(rs2));
			// cout << "DataMem at " << aluOutput << ": " << myCPU.ReadDataMemory(aluOutput) << endl; 
		}

		else if (RegWrite && myCPU.operation != "JALR") {
			myCPU.WriteRegFile(rd, aluOutput);
		}

		// update PC
		if (Branch){
			nextPCOffset = aluOutput;
		}
		else
			nextPCOffset = 4;

		if (myCPU.operation == "JALR")
			myCPU.UpdatePC(nextPCOffset);
		else
			myCPU.UpdatePC(myCPU.readPC() + nextPCOffset);
		
		myCPU.clockCycles++;


		if (myCPU.killCount >= 1) { // end of execution
			myCPU.ipc = myCPU.insCount / myCPU.clockCycles;

			cout << "(" << myCPU.ReadRegFile(10) << "," << myCPU.ReadRegFile(11) << ")" << endl; //change indicies as neccessary

			// cout << "Clock Cycles: " << myCPU.clockCycles << endl;
			// cout << "R-Type Instruction Count: " << myCPU.rTypeCount << endl;
			// cout << "IPC: "<< myCPU.ipc << endl;

			break;
		}

		// sanity check
		if (myCPU.readPC() > maxPC) {
			break;
		}

	}
	
	return 0;

}