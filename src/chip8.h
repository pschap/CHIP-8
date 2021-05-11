#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <cstdint>

class Chip8Processor
{
	private:
		static const unsigned int NUM_REGISTERS = 16;
		static const unsigned int MEMORY_LOCATIONS = 4096;
		static const unsigned int STACK_LEVELS = 16;
		static const unsigned int INPUT_KEYS = 16;
		static const unsigned int DISPLAY_WIDTH = 64;
		static const unsigned int DISPLAY_HEIGHT = 32;
		static const unsigned int START_ADDRESS = 0X200;
		static const unsigned int FONTSET_SIZE = 80;

		/* Memory layout */
		uint8_t V[NUM_REGISTERS]{ };
		uint8_t memory[MEMORY_LOCATIONS]{ };
		uint16_t index;
		uint16_t pc;
		uint16_t stack[STACK_LEVELS]{ };
		uint8_t sp;

		/* Timers */
		uint8_t delay_timer;
		uint8_t sound_timer;

		/* Keypad Inputs */
		uint8_t keypad[INPUT_KEYS]{ };

		/* Graphics Display */
		uint32_t video[DISPLAY_WIDTH * DISPLAY_HEIGHT]{ };

		/* Pointer to current Opcode to be executed*/
		uint16_t opcode;

		uint8_t fontset[FONTSET_SIZE] =
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

	public:
		/* Constructor initializes memory */
		Chip8Processor();

		/* 
		 * Load a Chip-8 ROM into main memory. Takes a null terminated string
		 * as an argument. This argument should contain the name of the ROM file.
		*/
		void LoadROM(char const *filename);

		/* Emulate one Chip-8 "Cycle" */
		void Cycle();
};

#endif
