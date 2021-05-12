#include "chip8.h"
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <time.h>

namespace CHIP8
{
	#pragma region Chip8Processor
	Chip8Processor::Chip8Processor()
	{
		unsigned int i;

		/* Initialize the program counter */
		pc = START_ADDRESS;

		/* Load fonts into memory */
		for (i = 0; i < FONTSET_START_ADDRESS; i++)
		{
			memory[FONTSET_START_ADDRESS + i] = fontset[i];
		}

		opcode = 0;
		index = 0;
		sp = 0;

		delay_timer = 0;
		sound_timer = 0;

		/* Initialize random seed. Cast to unsigned int to suppress warnings.  */
		srand( (unsigned int)time(NULL));
	}
	#pragma endregion

	#pragma region LoadROM
	int Chip8Processor::LoadROM(const char *filename)
	{
		FILE *romFile;
		long file_size;
		char *buffer;
		size_t elements_read;
		int error;
		long i;

		try
		{
			if ((error = fopen_s(&romFile, filename, "rb")) != 0)
				throw error;

			// Get the size of the ROM file
			fseek(romFile, 0, SEEK_END);
			file_size = ftell(romFile);
			rewind(romFile);

			/* Dynamically allocate memory for buffer and read binary into buffer */
			buffer = new char[file_size];
			elements_read = fread(buffer, sizeof(uint8_t), file_size, romFile);
			error = elements_read == file_size;
			
			if (!error)
				throw error;

			/* Copy buffer into main memory at the specified starting address */
			for (i = 0; i < file_size; i++)
			{
				memory[START_ADDRESS + i] = buffer[i];
			}

			/* 
			 * Initialize random seed so that we can generate random 8-bit unsigned integers.
			 * Cast to unsigned int to suppress warnings.
			*/
			srand((unsigned int)time(NULL));

		}
		catch (int error)
		{
			std::cerr << "Unable to read ROM file. Received error code: " << error << std::endl;
		}

		return error;
	}
	#pragma endregion

	/* Calls machine code routine at address NNN. Not necessary for most ROMs. */
	void Chip8Processor::opcode_0NNN()
	{

	}

	/* Clears the screen. */
	void Chip8Processor::opcode_00E0()
	{
		memset(video, 0, sizeof(video));
	}

	/* Returns from a subroutine. */
	void Chip8Processor::opcode_00EE()
	{
		sp--;
		pc = stack[sp];
	}

	/* Jumps to address at NNN. */
	void Chip8Processor::opcode_1NNN()
	{
		pc = opcode & 0x0FFFU;
	}

	/* Calls subroutine at NNN. */
	void Chip8Processor::opcode_2NNN()
	{
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFFU;
	}

	/* Skips the next instruction if VX equals NN.*/
	void Chip8Processor::opcode_3XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		if (V[x] == nn)
			pc += 2;
	}

	/* Skips the next instruction if VX does not equal NN. */
	void Chip8Processor::opcode_4XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		if (V[x] != nn)
			pc += 2;
	}

	/* Skips the next instruction if VX equals VY. */
	void Chip8Processor::opcode_5XY0()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		if (V[x] == V[y])
			pc += 2;
	}

	/* Sets VX to NN. */
	void Chip8Processor::opcode_6XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		V[x] = nn;
	}

	/* Adds NN to VX (Carry flag is not changed) */
	void Chip8Processor::opcode_7XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		V[x] += nn;
	}

	/* Sets VX to the value of VY */
	void Chip8Processor::opcode_8XY0()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		V[x] = V[y];
	}

	/* Sets VX to VX bitwise-or VY */
	void Chip8Processor::opcode_8XY1()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		V[x] |= V[y];
	}

	/* Sets VX to VX bitwise-and VY */
	void Chip8Processor::opcode_8XY2()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		V[x] &= V[y];
	}

	/* Sets VX to VX bitwise-xor VY */
	void Chip8Processor::opcode_8XY3()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		V[x] ^= V[y];
	}

	/* Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not */
	void Chip8Processor::opcode_8XY4()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		uint16_t sum = V[x] + V[y];
		V[0xFU] = (sum > 255U) ? 1 : 0;
		V[x] = sum & 0xFFU;
	}

	/* VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not. */
	void Chip8Processor::opcode_8XY5()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		V[0xFU] = (V[x] > V[y]) ? 1 : 0;
		V[x] -= V[y];
	}

	/* Stores the least significant bit of VX in VF and then shifts VX to the right by 1. */
	void Chip8Processor::opcode_8XY6()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		V[0xFU] = V[x] & 0x1U;
		V[x] >>= 1U;
	}

	/* Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not. */
	void Chip8Processor::opcode_8XY7()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		uint16_t diff = V[y] - V[x];

		V[0xFU] = (V[y] > V[x]) ? 1 : 0;
		V[x] = diff & 0xFFU;
	}

	/* Stores the most significant bit of VX in VF and then shifts VX to the left by 1. */
	void Chip8Processor::opcode_8XYE()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		V[0xFU] = (V[x] & 0x80U) >> 7U;
		V[x] <<= 1;
	}

	/* Skips the next instruction if VX does not equal VY. */
	void Chip8Processor::opcode_9XY0()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		if (V[x] != V[y])
			pc += 2;
	}

	/* Sets the index register to the address NNN. */
	void Chip8Processor::opcode_ANNN()
	{
		uint16_t nnn = opcode & 0x0FFFU;
		index = nnn;
	}

	/* Jumps to the address NNN plus V0. */
	void Chip8Processor::opcode_BNNN()
	{
		uint16_t nnn = opcode & 0x0FFFU;
		pc = V[0] + nnn;
	}

	/* Sets VX to the result of a bitwise-and operation on a randomly generated number between 0 and 255 and NN. */
	void Chip8Processor::opcode_CXNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		uint8_t random = rand() % 256;
		V[x] = random & nn;
	}

	/*
	 * Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N + 1 pixels. Each row of 
	 * 8 pixels is read as bit-coded starting from memory location I. I value does not change after the execution of this instruction.
	 * As described above, VG is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen. 
	 */
	void Chip8Processor::opcode_DXYN()
	{

	}
}
