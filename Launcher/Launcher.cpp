#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <WinUser.h>
#include <DLLInterface.h>
#include "CncDll.h"

unsigned char game_buffer[0xFFFFFFF];

float pixel_scale = 2;

SDL_Rect viewport;

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
			printf("Incoming message: %s\n", event.Message.Message);
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

void mouse_to_map_coords(int in_x, int in_y, int &out_x, int &out_y)
{
	out_x = viewport.x + (in_x / pixel_scale);
	out_y = viewport.y + (in_y / pixel_scale);
}

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char* args[])
{
	printf("Loaded CNC DLL interface version %i\n", CNC_Version(0x100));

	printf("Calling CNC_Init\n");
	CNC_Init("", &cnc_event_callback);

	CNCRulesDataStruct rules;
	// Rules taken from original game
	// Easy
	rules.Difficulties[0].FirepowerBias   = 1.000f;
	rules.Difficulties[0].GroundspeedBias = 1.200f;
	rules.Difficulties[0].AirspeedBias    = 1.200f;
	rules.Difficulties[0].ArmorBias       = 1.200f;
	rules.Difficulties[0].ROFBias         = 0.800f;
	rules.Difficulties[0].CostBias        = 0.800f;
	rules.Difficulties[0].BuildSpeedBias  = 0.800f;
	rules.Difficulties[0].RepairDelay     = 0.001f;
	rules.Difficulties[0].BuildDelay      = 0.001f;
	rules.Difficulties[0].IsBuildSlowdown = false;
	rules.Difficulties[0].IsWallDestroyer = true;
	rules.Difficulties[0].IsContentScan   = true;
	// Medium
	rules.Difficulties[1].FirepowerBias   = 1.000f;
	rules.Difficulties[1].GroundspeedBias = 1.000f;
	rules.Difficulties[1].AirspeedBias    = 1.000f;
	rules.Difficulties[1].ArmorBias       = 1.000f;
	rules.Difficulties[1].ROFBias         = 1.000f;
	rules.Difficulties[1].CostBias        = 1.000f;
	rules.Difficulties[1].BuildSpeedBias  = 1.000f;
	rules.Difficulties[1].RepairDelay     = 0.020f;
	rules.Difficulties[1].BuildDelay      = 0.030f;
	rules.Difficulties[1].IsBuildSlowdown = true;
	rules.Difficulties[1].IsWallDestroyer = true;
	rules.Difficulties[1].IsContentScan   = true;
	//Hard
	rules.Difficulties[2].FirepowerBias   = 1.000f;
	rules.Difficulties[2].GroundspeedBias = 0.800f;
	rules.Difficulties[2].AirspeedBias    = 0.800f;
	rules.Difficulties[2].ArmorBias       = 0.800f;
	rules.Difficulties[2].ROFBias         = 1.200f;
	rules.Difficulties[2].CostBias        = 1.000f;
	rules.Difficulties[2].BuildSpeedBias  = 1.000f;
	rules.Difficulties[2].RepairDelay     = 0.050f;
	rules.Difficulties[2].BuildDelay      = 0.100f;
	rules.Difficulties[2].IsBuildSlowdown = true;
	rules.Difficulties[2].IsWallDestroyer = false;
	rules.Difficulties[2].IsContentScan   = false;
	CNC_Config(rules);

	printf("Calling CNC_Start_Instance\n");
	CNC_Start_Instance(1, 10, "ALLY", "GAME_NORMAL", ".", NULL, NULL);

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* game_window = SDL_CreateWindow("Red Alert SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
	ShowCursor(TRUE);

	SDL_Renderer* renderer = SDL_CreateRenderer(game_window, -1, 0);
	SDL_Surface* game_surface;
	SDL_Surface* intermediate_surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
	SDL_SetSurfaceBlendMode(intermediate_surface, SDL_BLENDMODE_NONE);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	viewport.x = 0;
	viewport.y = 0;
	viewport.w = WIDTH / pixel_scale;
	viewport.h = HEIGHT / pixel_scale;

	unsigned char palette[256][3];
	CNC_Get_Palette(palette);
	SDL_Color c[256];
	for (Uint8 i = 255; i--;) {
		c[i].r = (palette[i][0] << 2) | (palette[i][0] >> 6);
		c[i].g = (palette[i][1] << 2) | (palette[i][1] >> 6);
		c[i].b = (palette[i][2] << 2) | (palette[i][2] >> 6);
		c[i].a = 255;
	}

	Uint32 lastUpdate = SDL_GetTicks();

	SDL_Event event;
	bool quit = false;
	bool dragging = false;
	int drag_mousepos[2];
	int drag_viewportpos[2];
	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 2)
			{
				dragging = true;
				SDL_GetMouseState(&drag_mousepos[0], &drag_mousepos[1]);
				drag_viewportpos[0] = viewport.x;
				drag_viewportpos[1] = viewport.y;
			}
			if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 2)
			{
				dragging = false;
			}
			if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 1)
			{
				int mousex;
				int mousey;
				SDL_GetMouseState(&mousex, &mousey);
				int mapx;
				int mapy;
				mouse_to_map_coords(mousex, mousey, mapx, mapy);
				CNC_Handle_Input(INPUT_REQUEST_MOUSE_LEFT_CLICK, NULL, 0, mapx, mapy, NULL, NULL);
			}
			if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 3)
			{
				int mousex;
				int mousey;
				SDL_GetMouseState(&mousex, &mousey);
				int mapx;
				int mapy;
				mouse_to_map_coords(mousex, mousey, mapx, mapy);
				CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_CLICK, NULL, 0, mapx, mapy, NULL, NULL);
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 3)
			{
				int mousex;
				int mousey;
				SDL_GetMouseState(&mousex, &mousey);
				int mapx;
				int mapy;
				mouse_to_map_coords(mousex, mousey, mapx, mapy);
				CNC_Handle_Input(INPUT_REQUEST_MOUSE_RIGHT_DOWN, NULL, 0, mapx, mapy, NULL, NULL);
			}
			if (event.type == SDL_MOUSEWHEEL)
			{
				if (event.wheel.y > 0) // Scroll up
				{
					// Zoom in
					pixel_scale *= 2;
					viewport.x += viewport.w / 4;
					viewport.y += viewport.h / 4;
					viewport.w /= 2;
					viewport.h /= 2;
				}
				else if (event.wheel.y < 0) // Scroll down
				{
					// Zoom out
					pixel_scale /= 2;
					viewport.w *= 2;
					viewport.h *= 2;
					viewport.x -= viewport.w / 4;
					viewport.y -= viewport.h / 4;
				}

			}
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		if (dragging)
		{
			int mousex;
			int mousey;
			SDL_GetMouseState(&mousex, &mousey);
			int deltax = (mousex - drag_mousepos[0]) / pixel_scale;
			int deltay = (mousey - drag_mousepos[1]) / pixel_scale;
			viewport.x = drag_viewportpos[0] - deltax;
			viewport.y = drag_viewportpos[1] - deltay;
		}

		Uint32 currTime = SDL_GetTicks();
		if (currTime - lastUpdate > 1000/30)
		{
			if (!CNC_Advance_Instance(0)) quit = true;
			lastUpdate = currTime;
		}

		unsigned int width = 0;
		unsigned int height = 0;
		if (CNC_Get_Visible_Page(game_buffer, width, height))
		{
			game_surface = SDL_CreateRGBSurfaceFrom(game_buffer, width, height, 8, width, NULL, NULL, NULL, NULL);
			SDL_SetSurfaceBlendMode(game_surface, SDL_BLENDMODE_NONE);
			SDL_SetPaletteColors(game_surface->format->palette, c, 0, 256);
			SDL_Surface* optimized_surface = SDL_ConvertSurface(game_surface, intermediate_surface->format, 0);
			SDL_FillRect(intermediate_surface, NULL, SDL_MapRGB(intermediate_surface->format, 0, 0, 0));
			SDL_BlitScaled(optimized_surface, &viewport, intermediate_surface, NULL);
			SDL_FreeSurface(game_surface);
			SDL_FreeSurface(optimized_surface);

			SDL_UpdateTexture(texture, NULL, intermediate_surface->pixels, intermediate_surface->pitch);

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
