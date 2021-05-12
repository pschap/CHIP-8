#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <cstdint>
#include <random>

namespace CHIP8
{
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
			static const unsigned int FONTSET_START_ADDRESS = 0x50;

			/* Memory layout */
			uint8_t V[NUM_REGISTERS] { };
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

			/*
			 * Functions to execute each of the 35 Chip-8 opcodes. List of opcodes can be
			 * found here: https://en.wikipedia.org/wiki/CHIP-8
			 */
			void opcode_0NNN();
			void opcode_00E0();
			void opcode_00EE();
			void opcode_1NNN();
			void opcode_2NNN();
			void opcode_3XNN();
			void opcode_4XNN();
			void opcode_5XY0();
			void opcode_6XNN();
			void opcode_7XNN();
			void opcode_8XY0();
			void opcode_8XY1();
			void opcode_8XY2();
			void opcode_8XY3();
			void opcode_8XY4();
			void opcode_8XY5();
			void opcode_8XY6();
			void opcode_8XY7();
			void opcode_8XYE();
			void opcode_9XY0();
			void opcode_ANNN();
			void opcode_BNNN();
			void opcode_CXNN();
			void opcode_DXYN();
			void opcode_EX9E();
			void opcode_EXA1();
			void opcode_FX07();
			void opcode_FX0A();
			void opcode_FX15();
			void opcode_FX18();
			void opcode_FX1E();
			void opcode_FX29();
			void opcode_FX33();
			void opcode_FX55();
			void opcode_FX65();

		public:
			/* Constructor initializes memory */
			Chip8Processor();

			/* 
			 * Load a Chip-8 ROM into main memory. On success,
			 * returns a nonzero integer. Otherwise, returns zero.
			 */
			int LoadROM(const char *filename);

			/* Emulate one Chip-8 "Cycle" */
			void Cycle();
	};
}

#endif
