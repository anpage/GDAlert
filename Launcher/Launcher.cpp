#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include <DLLInterface.h>
#include "CncDll.h"

void cnc_event_callback(const EventCallbackStruct& event)
{
	printf("Callback!");
}

int main(int argc, char* args[])
{
	printf("Loaded CNC DLL interface version %i\n", CNC_Version(0x100));

	printf("Calling CNC_Init()\n");
	CNC_Init("", &cnc_event_callback);

	return 0;
}
