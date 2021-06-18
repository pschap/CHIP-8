#include "SDL.h"
#include "display.h"

namespace CHIP8
{
	Chip8Display::Chip8Display(const char* title, int window_width, int window_height, int texture_width, int texture_height)
	{
		SDL_Init(SDL_INIT_VIDEO);

		window = SDL_CreateWindow(title, 0, 0, window_width, window_height, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);
	}

	Chip8Display::~Chip8Display()
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void Chip8Display::UpdateDisplay(const void* display_state, int pitch)
	{
		SDL_UpdateTexture(texture, NULL, display_state, pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	bool Chip8Display::HandleInput(uint8_t* keys_state)
	{
		bool quit = false;

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			    case SDL_QUIT:
			    {
				    quit = true;
			    } break;

			    case SDL_KEYDOWN:
			    {
				    switch (event.key.keysym.sym)
				    {
				        case SDLK_ESCAPE:
				        {
					        quit = true;
				        } break;

				        case SDLK_x:
				        {
					        keys_state[0] = 1;
				        } break;

				        case SDLK_1:
				        {
					        keys_state[1] = 1;
				        } break;

				        case SDLK_2:
				        {
					        keys_state[2] = 1;
				        } break;

				        case SDLK_3:
				        {
					        keys_state[3] = 1;
				        } break;

				        case SDLK_q:
				        {
					        keys_state[4] = 1;
				        } break;

				        case SDLK_w:
				        {
					        keys_state[5] = 1;
				        } break;

				        case SDLK_e:
				        {
					        keys_state[6] = 1;
				        } break;

				        case SDLK_a:
				        {
					        keys_state[7] = 1;
				        } break;

				        case SDLK_s:
				        {
					        keys_state[8] = 1;
				        } break;

				        case SDLK_d:
				        {
					        keys_state[9] = 1;
				        } break;

				        case SDLK_z:
				        {
					        keys_state[0xA] = 1;
				        } break;

				        case SDLK_c:
				        {
					        keys_state[0xB] = 1;
				        } break;

				        case SDLK_4:
				        {
					        keys_state[0xC] = 1;
				        } break;

				        case SDLK_r:
				        {
					        keys_state[0xD] = 1;
				        } break;

				        case SDLK_f:
				        {
					        keys_state[0xE] = 1;
				        } break;

				        case SDLK_v:
				        {
					        keys_state[0xF] = 1;
				        } break;
				    }
			    } break;

			    case SDL_KEYUP:
			    {
				    switch (event.key.keysym.sym)
				    {
				        case SDLK_x:
				        {
					        keys_state[0] = 0;
				        } break;

				        case SDLK_1:
				        {
					        keys_state[1] = 0;
				        } break;

				        case SDLK_2:
				        {
					        keys_state[2] = 0;
				        } break;

				        case SDLK_3:
				        {
					        keys_state[3] = 0;
				        } break;

				        case SDLK_q:
				        {
					        keys_state[4] = 0;
				        } break;

				        case SDLK_w:
				        {
					        keys_state[5] = 0;
				        } break;

				        case SDLK_e:
				        {
					        keys_state[6] = 0;
				        } break;

				        case SDLK_a:
				        {
					        keys_state[7] = 0;
				        } break;

						case SDLK_s:
						{
							keys_state[8] = 0;
						} break;

						case SDLK_d:
						{
							keys_state[9] = 0;
						} break;

						case SDLK_z:
						{
							keys_state[0xA] = 0;
						} break;

						case SDLK_c:
						{
							keys_state[0xB] = 0;
						} break;

						case SDLK_4:
						{
							keys_state[0xC] = 0;
						} break;

						case SDLK_r:
						{
							keys_state[0xD] = 0;
						} break;

						case SDLK_f:
						{
							keys_state[0xE] = 0;
						} break;

						case SDLK_v:
						{
							keys_state[0xF] = 0;
						} break;
					}
				} break;
			}
		}

		return quit;
	}
}
