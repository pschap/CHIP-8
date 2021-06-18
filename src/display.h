#include "SDL.h"

namespace CHIP8 
{
	class Chip8Display
	{
		private:

			SDL_Window* window;
			SDL_Renderer* renderer;
			SDL_Texture* texture;

		public:

			Chip8Display(const char* title, int window_width, int window_height, int texture_width, int texture_height);
			~Chip8Display();

			void UpdateDisplay(const void* display_state, int pitch);
			bool HandleInput(uint8_t* keys_state);
	};
}
