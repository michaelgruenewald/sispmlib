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

// rundll32 entry points (according to KB164787)

void CALLBACK Toggle(HWND hwnd, HINSTANCE hinst, LPTSTR lpszCmdLine, int nCmdShow) {
RUNDLL32EXPORT

    DWORD number = _ttoi(lpszCmdLine);
    SisPmSocket socket = SisPmDevice::findDevices().front().socket(number);
    socket.turn(!socket.isTurnedOn());
}

void CALLBACK TurnOn(HWND hwnd, HINSTANCE hinst, LPTSTR lpszCmdLine, int nCmdShow) {
RUNDLL32EXPORT

    DWORD number = _ttoi(lpszCmdLine);
    SisPmDevice::findDevices().front().socket(number).turn(TRUE);
}

void CALLBACK TurnOff(HWND hwnd, HINSTANCE hinst, LPTSTR lpszCmdLine, int nCmdShow) {
RUNDLL32EXPORT

    DWORD number = _ttoi(lpszCmdLine);
    SisPmDevice::findDevices().front().socket(number).turn(FALSE);
}
