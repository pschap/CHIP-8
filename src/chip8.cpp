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

	void Chip8Processor::opcode_0NNN()
	{

	}

	void Chip8Processor::opcode_00E0()
	{
		memset(video, 0, sizeof(video));
	}

	void Chip8Processor::opcode_00EE()
	{
		sp--;
		pc = stack[sp];
	}

	void Chip8Processor::opcode_1NNN()
	{
		pc = opcode & 0x0FFFU;
	}

	void Chip8Processor::opcode_2NNN()
	{
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFFU;
	}

	void Chip8Processor::opcode_3XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		if (V[x] == nn)
			pc += 2;
	}

	void Chip8Processor::opcode_4XNN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		if (V[x] != nn)
			pc += 2;
	}

	void Chip8Processor::opcode_5XY0()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;

		if (V[x] == V[y])
			pc += 2;
	}

	void Chip8Processor::opcode_6XNN()
	{
		uint8_t x = (opcode & 0xF00U) >> 8U;
		uint8_t nn = opcode & 0x00FFU;

		V[x] = nn;
	}
}
