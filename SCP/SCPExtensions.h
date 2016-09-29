/* Scarlet.Crush's
 * XInput Wrapper for DS3 and Play.com USB Dual DS2 Controller 
 * http://forums.pcsx2.net/Thread-XInput-Wrapper-for-DS3-and-Play-com-USB-Dual-DS2-Controller
 */

#pragma once

typedef struct
{
	float SCP_UP;
	float SCP_RIGHT;
	float SCP_DOWN;
	float SCP_LEFT;

	float SCP_LX;
	float SCP_LY;

	float SCP_L1;
	float SCP_L2;
	float SCP_L3;

	float SCP_RX;
	float SCP_RY;

	float SCP_R1;
	float SCP_R2;
	float SCP_R3;

	float SCP_T;
	float SCP_C;
	float SCP_X;
	float SCP_S;

	float SCP_SELECT;
	float SCP_START;

} SCP_EXTN;

typedef DWORD (WINAPI *XInputGetExtendedFunction)(DWORD dwUserIndex, SCP_EXTN* pPressure);

extern DWORD WINAPI XInputGetExtended(DWORD dwUserIndex, SCP_EXTN* pPressure);

