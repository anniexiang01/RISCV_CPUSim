#include "CPU.h"

Instruction::Instruction(bitset<32> fetch, CPU myCPU)
{
	instr = fetch;
	myCPU = CPU();
}

CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
	for (int i = 0; i < 32; i++){
		regFile[i] = 0;
	}
	killCount = 0;
	operation = "NOP";
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	return instr;
}


void CPU::Decode(uint32_t instr)
{
	uint32_t opcode = instr & 0b1111111;

	int funct3 = (instr >> 12) & 0b111;
	int funct7 = (instr >> 25) & 0b1111111;

	switch (opcode) {
		case 0b0110011: //R-Type: ADD, SUB, XOR, SRA
			rTypeCount++;
			if (funct3 == 0b000 && funct7 == 0b0000000) 
				operation = "ADD";
			else if (funct3 == 0b000 && funct7 == 0b0100000)
				operation = "SUB";
			else if (funct3 == 0b100)
				operation = "XOR";
			else if (funct3 == 0b101)
				operation = "SRA";
			else
				operation = "ERR";
			break;

		case 0b0010011: //I-Type: ADDI, ANDI
			if (funct3 == 0b000) //ADDI
				operation = "ADDI";
			else if (funct3 == 0b111) //ANDI
				operation = "ANDI";
			else
				operation = "ERR";
			break;

		case 0b0000011: //LW
			operation = "LW";
			break;

		case 0b0100011: //SW
			operation = "SW";
			break;
		
		case 0b1100011: //BLT
			operation = "BLT";
			break;
		
		case 0b1100111: //JALR
			operation = "JALR";
			break;

		case 0b0000000: //NOP
			operation = "NOP";
			break;

		default:
			operation = "ERR";
	}
}

unsigned long CPU::readPC()
{
	return PC;
}

void CPU::UpdatePC(int imm)
{
	PC = imm;
}

int CPU::ReadRegFile(int reg){
	return regFile[reg];
}

void CPU::WriteRegFile(int reg, int value){
	regFile[reg] = value;
}

int CPU::ReadDataMemory(int address) { //TODO: fix if necessary
    int data = 0;
    for (int i = 0; i < 4; i++) {
        data |= (dmemory[address + i] & 0xFF) << (i * 8);
    }
    return data;
}

void CPU::WriteDataMemory(int address, int data) { //TODO: fix if necessary
    for (int i = 0; i < 4; i++) {
        dmemory[address + i] = (data >> (i * 8)) & 0xFF;
    }
}


// Add other functions here ... 