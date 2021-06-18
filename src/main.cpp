#include <iostream>
#include <time.h>
#include "chip8.h"
#include "display.h"

int main(int argc, char** argv)
{
	
	if (argc == 2)
	{
		char const* romFile = argv[1];
		CHIP8::Chip8Processor chip8;
		chip8.LoadROM(romFile);

		CHIP8::Chip8Display display("Chip 8 Emulator", 1000, 500, 64, 32);

		const float TIME_PER_CYCLE = 1.0f / 500.0f;

		bool running = true;
		clock_t start = clock();
		clock_t end;

		while (running)
		{
			running = !display.HandleInput(chip8.GetKeypadState());
			end = clock();
			float dt = (float)(end - start);

			if (dt > TIME_PER_CYCLE)
			{
				start = end;
				chip8.Cycle();
				display.UpdateDisplay(chip8.GetDisplayState(), 256);
			}
		}
	}
	else
	{
		std::cerr << "Error: Must provide Chip 8 ROM File as command line argument." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	return 0;
}
