#include "chip8.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	int status;
	CHIP8::Chip8Processor chip8;
	status = chip8.LoadROM("chip8-roms/games/Airplane.ch8");

	cout << status << endl;

	return 0;
}
