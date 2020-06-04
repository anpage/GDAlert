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

	printf("Calling CNC_Init()\n");
	CNC_Init("", &cnc_event_callback);

	return 0;
}
