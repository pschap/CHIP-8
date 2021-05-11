#include "chip8.h"
#include <cstdint>
#include <stdio.h>

namespace CHIP8
{
	int Chip8Processor::LoadROM(const char *filename)
	{
		FILE *romFile;
		long file_size;
		char *buffer;
		size_t elements_read;
		int error;

		romFile = fopen(filename, "rb");

		if (romFile != NULL)
		{
			// Get the size of the ROM file
			fseek(romFile, 0, SEEK_END);
			file_size = ftell(romFile);
			rewind(romFile);

			/* Dynamically allocate memory for buffer and read binary into buffer */
			buffer = new char[file_size];
			elements_read = fread(buffer, sizeof(uint8_t), file_size, romFile);
			error = elements_read == file_size;

			fclose(romFile);
			delete[] buffer;
		}
		else
		{
			fputs("Error reading CHIP-8 ROM File", stderr);
			error = 0;
		}

		return error;
	}
}
