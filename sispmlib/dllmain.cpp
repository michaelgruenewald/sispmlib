#include "stdafx.h"
#include "sispmlib.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

// rundll32 entry points

void SISPMLIB_API CALLBACK Toggle(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    DWORD number = atoi(lpszCmdLine);
    SisPmSocket socket = SisPmDevice::findDevices().front().socket(number);
    socket.turn(!socket.isOn());
}


void SISPMLIB_API CALLBACK TurnOn(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    DWORD number = atoi(lpszCmdLine);
    SisPmDevice::findDevices().front().socket(number).turn(TRUE);
}

void SISPMLIB_API CALLBACK TurnOff(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    DWORD number = atoi(lpszCmdLine);
    SisPmDevice::findDevices().front().socket(number).turn(FALSE);
}
