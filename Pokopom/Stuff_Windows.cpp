#include "General.h"
#include "PlayStation.h"
#include "Stuff.h"

#ifdef _WIN32

HWND hDisplay = NULL;
HINSTANCE hInstance = NULL;

void ShowDialog(const wchar_t* message, const wchar_t* title)
{
	MessageBox(NULL, message,title, MB_OK);
}

void MouseThread()
{
	while(true)
	{
		mouse_event( MOUSEEVENTF_MOVE, 0, 0, 0, NULL);
		Sleep(50000);
	}
}

void KeepAwake(u8 mode)
{
	static HANDLE hMouseThread = NULL;

	if(bKeepAwake)
	switch(mode)
	{
	case KEEPAWAKE_INIT:
	{
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

		if(hMouseThread == NULL)
			hMouseThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MouseThread, 0, 0, NULL);
	}
	break;

	case KEEPAWAKE_CLOSE:
	{
		SetThreadExecutionState(ES_CONTINUOUS);

		if(hMouseThread)
		{
			TerminateThread(hMouseThread,0);
			hMouseThread = NULL;
		}
	}
	break;
	}
}

void GetDisplay(void* pDisplay)
{
	emuStuff.hWnd = *(HWND*)pDisplay;
}

LRESULT CALLBACK KeyboardProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	keyEvent newEvent;

	switch(msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		newEvent.evt = 1;
		newEvent.key = wParam;
		keyEventList.push_back(newEvent);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		newEvent.evt = 2;
		newEvent.key = wParam;
		keyEventList.push_back(newEvent);
		break;

	case WM_DESTROY:
	case WM_QUIT:
		newEvent.evt = 1;
		newEvent.key = VK_ESCAPE;
		keyEventList.push_back(newEvent);
		break;
	}

	return CallWindowProcW(emuStuff.WndProc, hWnd, msg, wParam, lParam);
}

void KeyboardOpen()
{
	if(isPs2Emulator)
		emuStuff.WndProc = (WNDPROC)SetWindowLongPtr(emuStuff.hWnd, GWLP_WNDPROC, (LPARAM)KeyboardProc);
}

void KeyboardClose()
{
	if(isPs2Emulator)
		SetWindowLongPtr(emuStuff.hWnd, GWLP_WNDPROC, (LPARAM)emuStuff.WndProc);
}

void KeyboardCheck() {}

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
