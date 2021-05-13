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

		for (i = 0; i < 0xF + 1; i++)
			table[i] = &Chip8Processor::opcode_NULL;

		for (i = 0; i < 0xE + 1)
		{
			table0[i] = &Chip8Processor::opcode_NULL;
			table8[i] = &Chip8Processor::opcode_NULL;
			tableE[i] = &Chip8Processor::opcode_NULL;
		}

		for (i = 0; i < 0x65 + 1; i++)
			tableF[i] = &Chip8Processor::opcode_NULL;

		// Set up function pointer table
		table[0x0] = &Chip8Processor::Table0;
		table[0x1] = &Chip8Processor::opcode_1NNN;
		table[0x2] = &Chip8Processor::opcode_2NNN;
		table[0x3] = &Chip8Processor::opcode_3XNN;
		table[0x4] = &Chip8Processor::opcode_4XNN;
		table[0x5] = &Chip8Processor::opcode_5XY0;
		table[0x6] = &Chip8Processor::opcode_6XNN;
		table[0x7] = &Chip8Processor::opcode_7XNN;
		table[0x8] = &Chip8Processor::Table8;
		table[0x9] = &Chip8Processor::opcode_9XY0;
		table[0xA] = &Chip8Processor::opcode_ANNN;
		table[0xB] = &Chip8Processor::opcode_BNNN;
		table[0xC] = &Chip8Processor::opcode_CXNN;
		table[0xD] = &Chip8Processor::opcode_DXYN;
		table[0xE] = &Chip8Processor::TableE;
		table[0xF] = &Chip8Processor::TableF;

		table0[0x0] = &Chip8Processor::opcode_00E0;
		table0[0xE] = &Chip8Processor::opcode_00EE;

		table8[0x0] = &Chip8Processor::opcode_8XY0;
		table8[0x1] = &Chip8Processor::opcode_8XY1;
		table8[0x2] = &Chip8Processor::opcode_8XY2;
		table8[0x3] = &Chip8Processor::opcode_8XY3;
		table8[0x4] = &Chip8Processor::opcode_8XY4;
		table8[0x5] = &Chip8Processor::opcode_8XY5;
		table8[0x6] = &Chip8Processor::opcode_8XY6;
		table8[0x7] = &Chip8Processor::opcode_8XY7;
		table8[0xE] = &Chip8Processor::opcode_8XYE;

		tableE[0x1] = &Chip8Processor::opcode_EXA1;
		tableE[0xE] = &Chip8Processor::opcode_EX9E;

		tableF[0x07] = &Chip8Processor::opcode_FX07;
		tableF[0x0A] = &Chip8Processor::opcode_FX0A;
		tableF[0x15] = &Chip8Processor::opcode_FX15;
		tableF[0x18] = &Chip8Processor::opcode_FX18;
		tableF[0x1E] = &Chip8Processor::opcode_FX1E;
		tableF[0x29] = &Chip8Processor::opcode_FX29;
		tableF[0x33] = &Chip8Processor::opcode_FX33;
		tableF[0x55] = &Chip8Processor::opcode_FX55;
		tableF[0x65] = &Chip8Processor::opcode_FX65;
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

	#pragma region opcodes

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
	 * As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen. 
	 */
	void Chip8Processor::opcode_DXYN()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t y = (opcode & 0x00F0U) >> 4U;
		uint8_t height = opcode & 0x000FU;

		uint8_t xPosition = V[x] % DISPLAY_WIDTH;
		uint8_t yPosition = V[y] % DISPLAY_HEIGHT;

		V[0xFU] = 0;

		unsigned int row, column;
		for (row = 0; row < height; row++)
		{
			uint8_t spriteByte = memory[index + row];

			for (column = 0; column < 8; column++)
			{
				uint8_t spritePixel = spriteByte & (0x80 >> column);
				uint32_t* screenPixel = &video[(yPosition + row) * DISPLAY_WIDTH + (xPosition + column)];

				if (spritePixel)
				{
					if (*screenPixel == 0xFFFFFFFF)
						V[0xFU] = 1;

					*screenPixel ^= 0xFFFFFFFFF;
				}
			}
		}
	}

	/* Skips the next instruction if the key stored in VX is pressed. */
	void Chip8Processor::opcode_EX9E()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t key = V[x];

		if (keypad[key])
			pc += 2;
	}

	/* Skips the next instruction if the key stored in VX is not pressed. */
	void Chip8Processor::opcode_EXA1()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t key = V[x];

		if (!keypad[key])
			pc += 2;
	}

	/* Sets VX to the value of the delay timer. */
	void Chip8Processor::opcode_FX07()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		V[x] = delay_timer;
	}

	/* A key press is awaited, and then stored in VX. */
	void Chip8Processor::opcode_FX0A()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		unsigned int i = 0;
		bool found = false;

		while (i < INPUT_KEYS && !found)
		{
			if (keypad[i])
			{
				V[x] = i;
				found = true;
			}

			i++;
		}

		/* Run same instruction again if no key press is detected */
		if (!found)
			pc -= 2;
	}

	/* Sets the delay timer to VX */
	void Chip8Processor::opcode_FX15()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		delay_timer = V[x];
	}

	/* Sets the sound timer to VX */
	void Chip8Processor::opcode_FX18()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		sound_timer = V[x];
	}

	/* Adds VX to I. VF is not affected */
	void Chip8Processor::opcode_FX1E()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		index += V[x];
	}

	/* Sets I to the location of the sprite for the character in VX. */
	void Chip8Processor::opcode_FX29()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		index = FONTSET_START_ADDRESS + (5 * V[x]);
	}

	/*
	 * Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I + 1, and the least 
	 * significant digit at I + 2. 
	 */
	void Chip8Processor::opcode_FX33()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t value = V[x];

		/* Ones-place */
		memory[index + 2] = value % 10;
		value /= 10;

		/* Tens-place */
		memory[index + 1] = value % 10;
		value /= 10;

		/* Hundreds place */
		memory[index] = value % 10;
	}

	/* Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified */
	void Chip8Processor::opcode_FX55()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t i;

		for (i = 0; i <= x; i++)
		{
			memory[index + i] = V[i];
		}
	}

	/* Fillx V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified */
	void Chip8Processor::opcode_FX65()
	{
		uint8_t x = (opcode & 0x0F00U) >> 8U;
		uint8_t i;

		for (i = 0; i <= x; i++)
		{
			V[i] = memory[index + i];
		}
	}

	#pragma endregion

	#pragma region Jump Table Helpers

	void Chip8Processor::Table0()
	{
		((*this).*(table0[opcode & 0x000FU]))();
	}

	#pragma endregion

}
