#include "General.h"
#include "Settings.h"
#include "SharedStuff.h"

bool isPs2Emulator = false;
bool isPSemulator = false;
bool bKeepAwake = false; // Screensaver and stuff
s32 INIversion = 2; // INI version
_Settings settings[4];

#ifdef _WIN32

HINSTANCE hInstance;

void ScrollLockStuff(bool init)
{
	static bool scrollLock = false;
	static bool scrollLockSaved = false;


	if(!scrollLockSaved && init)
	{
		scrollLock = GetKeyState(VK_SCROLL)&0x1;
		scrollLockSaved = true;
	}
	else if(scrollLockSaved && !init)
	{
		if(scrollLock != (GetKeyState(VK_SCROLL)&0x1))
		{
			keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
			keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hInst, DWORD dwReason, LPVOID lpReserved)
{
	hInstance = hInst;

	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		ScrollLockStuff(true);
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		ScrollLockStuff(false);
		break;
	}

	return TRUE;
}

#endif //WIN32