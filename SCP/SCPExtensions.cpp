/* Scarlet.Crush's
 * XInput Wrapper for DS3 and Play.com USB Dual DS2 Controller 
 * http://forums.pcsx2.net/Thread-XInput-Wrapper-for-DS3-and-Play-com-USB-Dual-DS2-Controller
 */
#ifdef _WIN32
#include "SCPExtensions.h"

DWORD WINAPI XInputGetExtended(DWORD dwUserIndex, SCP_EXTN* pPressure)
{
static BOOL    l_Loaded = false, l_Available = false;
static HMODULE l_hXInputDll = NULL;
static FARPROC l_hXInputFunc[] = { NULL }; 

	DWORD RetVal = ERROR_NOT_SUPPORTED;

	if (!l_Loaded)
	{
		if ((l_hXInputDll = LoadLibrary(L"XInput1_3.dll")) != NULL)
		{
			if ((l_hXInputFunc[0] = GetProcAddress(l_hXInputDll, "XInputGetExtended")) != NULL)
			{
				l_Available = true;
			}
		}

		l_Loaded = true;
	}
	
	if (l_Available)
	{
		RetVal = ((XInputGetExtendedFunction)(l_hXInputFunc[0]))(dwUserIndex, pPressure);
	}

	return RetVal;
}
#endif
