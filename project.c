#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	unsigned z;
	switch(ALUControl) {
		case '0': // addition
			z = A + B;
			break;
		case '1': // subtraction
			z = A - B;
			break;
		case '2':
			if ((int)A < (int)B) // slt signed
				z = 1;
			else
				z = 0;
			break;
		case '3':
			if (A < B) // slt unsigned
				z = 1;
			else
				z = 0;
			break;
		case '4':
			z = A & B; // and
			break;
		case '5':
			z = A | B; // or
			break;
		case '6':
			z = (B << 16); // shift left 16 bits
			break;
		case '7':
			z = ~A; // not
			break;
		default :
			break;
	}

	*ALUresult = z;
	if (z == 0)
		*Zero = '1'; // sets zero to 1 if result is 0
	else
		*Zero = '0'; // zero is 0 if result is not 0
	return;

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if ((PC % 4) == 0) // check whether the PC is a word
	{
		*instruction = Mem[PC >> 2]; // if it is a word, shift PC right twice, use as index in Mem to find instruction
		return 0;
	}
	else
		// if PC is not a word, halt
		return 1; 
}

// Utility function to change strings of bits back to decimal
unsigned binToDec(char *binary, int size)
{
	int i, a;
	unsigned decimal = 0;
	for (i = 0; i < size; i++)
	{
		a = (int)binary[i];
		if (a == 1)
			decimal = decimal + (1 << i);
	}
	return decimal;
}

// Utility function to break number up into bits
char* getBits(unsigned number, int size)
{
	char *binary;
	binary = malloc(size*sizeof(char));
	int i;
	for (i = 0; i < size; i++)
	{
		if ((number%2) == 1)
			binary[i] = 1;
		else
			binary[i] = 0;

		number = number >> 1;
	}
	return binary;
}
/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	// Break instruction up into bits
	char *inst = getBits(instruction, 32);

	char op_code[6]; // op array of bits
	int i, j = 0;
	for (i = 26; i <= 31; i++)
	{
		op_code[j] = inst[i];
		j++;
	}

	*op = binToDec(op_code, 6); // set op to the decimal value of op_code

	// Complete the same exercise as we did for op and op_code with the 
	// other parts of the instruction
	char r_1[5], r_2[5], r_3[5], function[6], off_set[16], j_sec[26];
	j = 0;
	for (i = 21; i <= 25; i++)
	{
		r_1[j] = inst[i];
		j++;
	}
	*r1 = binToDec(r_1, 5);

	j = 0;
	for (i = 16; i <= 20; i++)
	{
		r_2[j] = inst[i];
		j++;
	}
	*r2 = binToDec(r_2, 5);

	j = 0;
	for (i = 11; i <= 15; i++)
	{
		r_3[j] = inst[i];
		j++;
	}
	*r3 = binToDec(r_3, 5);

	j = 0;
	for (i = 0; i <= 5; i++)
	{
		function[j] = inst[i];
		j++;
	}
	*funct = binToDec(function, 6);

	j = 0;
	for (i = 0; i <= 15; i++)
	{
		off_set[j] = inst[i];
		j++;
	}
	*offset = binToDec(off_set, 16);

	j = 0;
	for (i = 0; i <= 25; i++)
	{
		j_sec[j] = inst[i];
		j++;
	}
	*jsec = binToDec(j_sec, 26);

	free(inst); // Avoid seg faults--free space
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	if (op == 0) // R-types
	{
		controls->RegDst = '1';
		controls->RegWrite = '1';
		controls->ALUSrc = '0';
		controls->MemRead = '0';
		controls->MemtoReg = '0';
		controls->MemWrite = '0';
		controls->Branch = '0';
		controls->Jump = '0';
		controls->ALUOp = '7';
	}

	else if (op == 2 || op == 3) // J-types
	{
		controls->RegDst = '0';
		controls->RegWrite = '0';
		controls->ALUSrc = '0';
		controls->MemRead = '0';
		controls->MemtoReg = '0';
		controls->MemWrite = '0';
		controls->Branch = '0';
		controls->Jump = '1';
		controls->ALUOp = '0';
	}

	else // I-types
	{
		if (op == 35) // load word
		{
			controls->RegDst = '0';
			controls->RegWrite = '1';
			controls->ALUSrc = '1';
			controls->MemRead = '1';
			controls->MemtoReg = '1';
			controls->MemWrite = '0';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '0';
		}

		else if (op == 43) // store word
		{
			controls->RegDst = '0';
			controls->RegWrite = '0';
			controls->ALUSrc = '1';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '1';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '0';
		}

		else if (op == 15) // load upper immediate
		{
			controls->RegDst = '0';
			controls->RegWrite = '1';
			controls->ALUSrc = '1';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '0';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '6';
		}

		else if (op == 8) // add immediate
		{
			controls->RegDst = '0';
			controls->RegWrite = '1';
			controls->ALUSrc = '1';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '0';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '0';
		}

		else if (op == 10) // Set less than immediate
		{
			controls->RegDst = '0';
			controls->RegWrite = '1';
			controls->ALUSrc = '1';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '0';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '2';
		}

		else if (op == 11) // Set less than unsigned immediate
		{
			controls->RegDst = '0';
			controls->RegWrite = '1';
			controls->ALUSrc = '1';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '0';
			controls->Branch = '0';
			controls->Jump = '0';
			controls->ALUOp = '3';
		}

		else if (op == 4 || op == 5) // Branch if equal or not equal
		{
			controls->RegDst = '0';
			controls->RegWrite = '0';
			controls->ALUSrc = '0';
			controls->MemRead = '0';
			controls->MemtoReg = '0';
			controls->MemWrite = '0';
			controls->Branch = '1';
			controls->Jump = '0';
			controls->ALUOp = '1';
		}
		
		// if invalid code, halt
		else
			return 1;
	}

	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1]; //data 1 points to the information stored in Reg[r1]
	*data2 = Reg[r2]; //data 2 points to the information stored in Reg[r2]
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	char *bits;
	bits = getBits(offset, 16); // first get the individual bits of the offset
	char extend[32];
	
	// copy bits (formerly known as offset) to extend
	for (int i = 0; i < 16; i++)
			extend[i] = bits[i];

	int i;

	// extend the MSB from 16 to 32
	if (bits[15] == '1')
	{
		for (i = 16; i < 32; i++)
			extend[i] = 1;
	}

	else
		for (i = 16; i < 32; i++)
			extend[i] = 0;

	// extended_value now points to the decimal that extend represented
	*extended_value = binToDec(extend, 32);
	free(bits);
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	int halt = 0;
	switch(ALUOp) {
		case '0': // add or don't care
			if (ALUSrc == '1') // add
				ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '1': // subtract
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '2': // set less than
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '3': // set less than unsigned
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
		case '4': // and
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '5': // or
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '6': // shift extended_value << 16
			ALU(data1, extended_value, ALUOp, ALUresult, Zero);
			break;
		case '7': // R-Type
			if (funct == 32) // add
				ALU(data1, data2, '0', ALUresult, Zero);
			else if (funct == 34) //subtract
				ALU(data1, data2, '1', ALUresult, Zero);
			else if (funct == 36) //and
				ALU(data1, data2, '4', ALUresult, Zero);
			else if (funct == 37) //or
				ALU(data1, data2, '5', ALUresult, Zero);
			else if (funct == 42) //set on less than
				ALU(data1, data2, '2', ALUresult, Zero);
			else if (funct == 43) //set on less than unsigned
				ALU(data1, data2, '3', ALUresult, Zero);
			else // if code is invalid
				halt = 1;
			break;
		default : // ALUOp is goofy
			halt = 1;
			break;
    }

	return halt;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	if (MemWrite == '1' || MemRead == '1') //Only if either of these is asserted should we even verify that ALUresult is a word
	{
		if ((ALUresult % 4) != 0)
			return 1; // halt in the name of supercalifragilisticexpialadocious!
	}

	if (MemWrite == '1') //store data to correct location in memory
	{
		Mem[ALUresult >> 2] = data2;
	}

	else if (MemRead == '1') //load data from correct location in memory
	{
		*memdata = Mem[ALUresult >> 2];
	}

	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if (RegWrite == '1') // only continue if RegWrite is asserted
	{
		if (MemtoReg == '1') // I-Type, load word
		{
			Reg[r2] = memdata;
		}
		
		else
		{
			if (RegDst == '1') // R-type, r3
				Reg[r3] = ALUresult;

			else // I-Type
				Reg[r2] = ALUresult;
		}
	}

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	unsigned program_counter = *PC;
	program_counter += 4; // Always adjust PC in this way
	char *progCounter;
	char *jumpSec;

	if (Jump == '1')
	{
		char newAddress[32]; // will hold new jump address if Jump is asserted
		jsec = jsec << 2; // jsec moves from 26 to 28 bits
		jumpSec = getBits(jsec, 28);
		progCounter = getBits(program_counter, 32);

		// loops to concatenate the 28 bit jumpSec with the MSB 4 bits of progCounter
		// to make a 32 bit address
		int i;
		for (i = 0; i < 28; i++)
			newAddress[i] = jumpSec[i];

		for(i = 28; i < 32; i++)
			newAddress[i] = progCounter[i];

		program_counter = binToDec(newAddress, 32);

	}

	else if (Branch == '1')
	{
		if (Zero == '1') // only runs if the Branch & Zero == 1
		{
			extended_value = extended_value << 2; 
			*PC = extended_value;
			free(progCounter);
			free(jumpSec);
			return;
        }
	}

	*PC = program_counter;
}

