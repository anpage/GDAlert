#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <DLLInterface.h>
#include "CncDll.h"

void cnc_event_callback(const EventCallbackStruct& event)
{
	switch (event.EventType)
	{
		case (CALLBACK_EVENT_INVALID):
			printf("Unhandled event: CALLBACK_EVENT_INVALID\n");
			break;
		case (CALLBACK_EVENT_SOUND_EFFECT):
			printf("Unhandled event: CALLBACK_EVENT_SOUND_EFFECT\n");
			break;
		case (CALLBACK_EVENT_SPEECH):
			printf("Unhandled event: CALLBACK_EVENT_SPEECH\n");
			break;
		case (CALLBACK_EVENT_GAME_OVER):
			printf("Unhandled event: CALLBACK_EVENT_GAME_OVER\n");
			break;
		case (CALLBACK_EVENT_DEBUG_PRINT):
			printf("DEBUG PRINT: %s\n", event.DebugPrint.PrintString);
			break;
		case (CALLBACK_EVENT_MOVIE):
			printf("Unhandled event: CALLBACK_EVENT_MOVIE\n");
			break;
		case (CALLBACK_EVENT_MESSAGE):
			printf("Unhandled event: CALLBACK_EVENT_MESSAGE\n");
			break;
		case (CALLBACK_EVENT_UPDATE_MAP_CELL):
			printf("Unhandled event: CALLBACK_EVENT_UPDATE_MAP_CELL\n");
			break;
		case (CALLBACK_EVENT_ACHIEVEMENT):
			printf("Unhandled event: CALLBACK_EVENT_ACHIEVEMENT\n");
			break;
		case (CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS):
			printf("Unhandled event: CALLBACK_EVENT_STORE_CARRYOVER_OBJECTS\n");
			break;
		case (CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING):
			printf("Unhandled event: CALLBACK_EVENT_SPECIAL_WEAPON_TARGETTING\n");
			break;
		case (CALLBACK_EVENT_BRIEFING_SCREEN):
			printf("Unhandled event: CALLBACK_EVENT_BRIEFING_SCREEN\n");
			break;
		case (CALLBACK_EVENT_CENTER_CAMERA):
			printf("Unhandled event: CALLBACK_EVENT_CENTER_CAMERA\n");
			break;
		case (CALLBACK_EVENT_PING):
			printf("Unhandled event: CALLBACK_EVENT_PING\n");
			break;
		default:
			printf("Unknown event type: %i\n", event.EventType);
			break;
	}
}

int main(int argc, char* args[])
{
	printf("Loaded CNC DLL interface version %i\n", CNC_Version(0x100));

	printf("Calling CNC_Init\n");
	CNC_Init("", &cnc_event_callback);

	printf("Calling CNC_Start_Instance\n");
	CNC_Start_Instance(1, 10, "ALLY", "GAME_NORMAL", ".", NULL, NULL);

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* game_window = SDL_CreateWindow("Red Alert SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 864, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(game_window, -1, 0);

	SDL_Surface* game_surface = SDL_CreateRGBSurface(0, 720, 864, 8, 0, 0, 0, 0);
	game_surface = SDL_ConvertSurfaceFormat(game_surface, SDL_PIXELFORMAT_INDEX8, 0);

	SDL_Surface* helper_surface = SDL_CreateRGBSurface(0, 720, 864, 32, 0, 0, 0, 0);
	helper_surface = SDL_ConvertSurfaceFormat(helper_surface, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 720, 864);

	SDL_Event event;
	bool quit = false;
	while (CNC_Advance_Instance(0))
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		if (quit) break;

		unsigned int width = 0;
		unsigned int height = 0;
		if (CNC_Get_Visible_Page((unsigned char*)game_surface->pixels, width, height))
		{
			unsigned char palette[256][3];
			CNC_Get_Palette(palette);

			SDL_Color c[256];

			for (Uint8 i = 255; i--;) {
				c[i].r = palette[i][0]<<2;
				c[i].g = palette[i][1]<<2;
				c[i].b = palette[i][2]<<2;
				c[i].a = 255;
			}

			SDL_SetPaletteColors(game_surface->format->palette, c, 0, 256);

			SDL_BlitSurface(game_surface, NULL, helper_surface, NULL);

			SDL_UpdateTexture(texture, NULL, helper_surface->pixels, helper_surface->pitch);

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_window);
	SDL_Quit();

	return 0;
}
