/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "Input.h"
#include "Input_Shared.h"

#ifdef _WIN32

#include "../SCP/SCPExtensions.cpp"
//#include "Controller.h"
//#include "General.h"
#include "nullDC_Devices.h"
//#include "Chankast.h"
//#include "Zilmar_Devices.h"

#include <XInput.h>
#pragma comment(lib, "Xinput.lib")

typedef DWORD (WINAPI* XInputGetStateEx_t)(DWORD dwUserIndex, XINPUT_STATE *pState);
XInputGetStateEx_t XInputGetStateEx = NULL;

#define XINPUT_GAMEPAD_GUIDE 0x400

XINPUT_STATE state[4];

namespace Input
{

////////////////////////////////////////////////////////////////////////
// General
////////////////////////////////////////////////////////////////////////

bool FASTCALL Recheck(u8 port)
{
	if(settings[port].disabled) return false;

	DWORD result = XInputGetState(port, &state[port]);
	
	return (result == ERROR_SUCCESS);
}

void FASTCALL Pause(bool pewpew) 
{ 
	if(pewpew) StopRumbleAll();
	//XInputEnable(!pewpew); 
}

void StopRumbleAll()
{
	StopRumble(0); StopRumble(1);
	StopRumble(2); StopRumble(3);
}

void FASTCALL StopRumble(u8 port)
{
	XINPUT_VIBRATION vib;

	vib.wLeftMotorSpeed = 0;
	vib.wRightMotorSpeed = 0;

	XInputSetState(port, &vib);
}

bool FASTCALL CheckAnalogToggle(u8 port)
{
	const bool key = !!(GetAsyncKeyState(0x31 + port) >> 1);
	const bool pad = !!(state[port].Gamepad.wButtons & XINPUT_GAMEPAD_GUIDE);

	return pad || key;
}

void FASTCALL SetAnalogLed(u8 port, bool digital)
{
	bool ledScrollLock = GetKeyState(VK_SCROLL)&0x1;

	if((!digital && !ledScrollLock) || (digital && ledScrollLock))
	{
		keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
	}
}

bool FASTCALL InputGetState(_Pad& pad, _Settings &set)
{
	if(!XInputGetStateEx)
	{
		HINSTANCE hXInput = LoadLibrary(XINPUT_DLL);
		XInputGetStateEx = (XInputGetStateEx_t) GetProcAddress(hXInput, (LPCSTR) 100);

		if(!XInputGetStateEx) // Might help with wrappers compatibility
			XInputGetStateEx = (XInputGetStateEx_t) GetProcAddress(hXInput, "XInputGetState");
	} 

	const int xport = set.xinputPort;
	DWORD result = XInputGetStateEx(xport, &state[xport]);

	if(result == ERROR_SUCCESS)
	{
		pad.buttons[X360_DUP] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
		pad.buttons[X360_DDOWN] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) >> 1;
		pad.buttons[X360_DLEFT] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) >> 2;
		pad.buttons[X360_DRIGHT] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) >> 3;

		pad.buttons[X360_START] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_START) >> 4;
		pad.buttons[X360_BACK] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_BACK) >> 5;

		pad.buttons[X360_LS] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) >> 6;
		pad.buttons[X360_RS] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) >> 7;
		pad.buttons[X360_LB] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) >> 8;
		pad.buttons[X360_RB] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) >> 9;

		pad.buttons[X360_BIGX] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_GUIDE) >> 10;

		pad.buttons[X360_A] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_A) >> 12;
		pad.buttons[X360_B] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_B) >> 13;
		pad.buttons[X360_X] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_X) >> 14;
		pad.buttons[X360_Y] = (state[xport].Gamepad.wButtons & XINPUT_GAMEPAD_Y) >> 15;

		pad.analog[X360_STICKLX] = state[xport].Gamepad.sThumbLX;
		pad.analog[X360_STICKLY] = state[xport].Gamepad.sThumbLY;
		pad.analog[X360_STICKRX] = state[xport].Gamepad.sThumbRX;
		pad.analog[X360_STICKRY] = state[xport].Gamepad.sThumbRY;

		pad.analog[X360_TRIGGERL] = state[xport].Gamepad.bLeftTrigger;
		pad.analog[X360_TRIGGERR] = state[xport].Gamepad.bRightTrigger;

		TriggerDeadzone(pad, set);

		pad.stickL.X = pad.analog[X360_STICKLX];
		pad.stickL.Y = pad.analog[X360_STICKLY];
		pad.stickR.X = pad.analog[X360_STICKRX];
		pad.stickR.Y = pad.analog[X360_STICKRY];

		set.axisValue[GP_AXIS_LY] = pad.analog[X360_STICKLY] * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
		set.axisValue[GP_AXIS_LX] = pad.analog[X360_STICKLX] * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
		set.axisValue[GP_AXIS_RY] = pad.analog[X360_STICKRY] * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
		set.axisValue[GP_AXIS_RX] = pad.analog[X360_STICKRX] * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);

		pad.modL.X = set.axisValue[set.axisRemap[GP_AXIS_LX]];
		pad.modL.Y = set.axisValue[set.axisRemap[GP_AXIS_LY]];
		pad.modR.X = set.axisValue[set.axisRemap[GP_AXIS_RX]];
		pad.modR.Y = set.axisValue[set.axisRemap[GP_AXIS_RY]];

		GetRadius(pad.stickL); GetRadius(pad.stickR);
		GetRadius(pad.modL); GetRadius(pad.modR);
	}
	
	return result == ERROR_SUCCESS;
};

////////////////////////////////////////////////////////////////////////
// DualShock
////////////////////////////////////////////////////////////////////////

void FASTCALL DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged)
{
	if(!gamepadPlugged) return;
	
	//Debug("Vibrate! [%X] [%X]\n", smalldata, bigdata);
	const u8 xport = set.xinputPort;

	static XINPUT_VIBRATION vib[4];
	static DWORD timerS[4], timerB[4];

	if(smalldata)
	{
		vib[xport].wRightMotorSpeed = Clamp(0xFFFF * set.rumble);
		timerS[xport] = GetTickCount();
	}
	else if (vib[xport].wRightMotorSpeed && GetTickCount() - timerS[xport] > 150)
	{
		vib[xport].wRightMotorSpeed = 0;
	}

	/*
	3.637978807091713*^-11 +
	156.82454281087692 * x + -1.258165252213538 *  x^2 +
	0.006474549734772402 * x^3;
	*/

	if(bigdata)
	{
		f64 broom = 0.006474549734772402 * pow(bigdata, 3.0) -
			1.258165252213538 *  pow(bigdata, 2.0) +
			156.82454281087692 * bigdata +
			3.637978807091713e-11;


		/*
		u32 broom = bigdata;

		if(bigdata <= 0x2C) broom *= 0x72;
		else if(bigdata <= 0x53) broom = 0x13C7 + bigdata * 0x24;
		else broom *= 0x205;
		*/

		vib[xport].wLeftMotorSpeed = Clamp(broom * set.rumble);
		timerB[xport] = GetTickCount();
	}
	else if (vib[xport].wLeftMotorSpeed && GetTickCount() - timerB[xport] > 150)
	{
		vib[xport].wLeftMotorSpeed = 0;
	}

	/*

	vib.wRightMotorSpeed = smalldata == 0? 0 : 0xFFFF;
	vib.wLeftMotorSpeed = bigdata * 0x101;

	vib.wRightMotorSpeed = Clamp(vib.wRightMotorSpeed * settings.rumble);
	vib.wLeftMotorSpeed = Clamp(vib.wLeftMotorSpeed * settings.rumble);
	*/

	if( XInputSetState(xport, &vib[xport]) != ERROR_SUCCESS )
		gamepadPlugged = false;
}

bool FASTCALL DualshockPressure(u8 * bufferOut, u32 mask, _Settings &set, bool &gamepadPlugged)
{
	SCP_EXTN pressure;

	if(XInputGetExtended(set.xinputPort, &pressure) == ERROR_SUCCESS)
	{
		//Right, left, up, down
		bufferOut[0x00] = (mask & 0x01) ? (u8)(pressure.SCP_RIGHT* 255) : 0x00;
		bufferOut[0x01] = (mask & 0x02) ? (u8)(pressure.SCP_LEFT * 255) : 0x00;
		bufferOut[0x02] = (mask & 0x04) ? (u8)(pressure.SCP_UP   * 255) : 0x00;
		bufferOut[0x03] = (mask & 0x08) ? (u8)(pressure.SCP_DOWN * 255) : 0x00;

		//triangle, circle, cross, square
		bufferOut[0x04] = (mask & 0x10) ? (u8)(pressure.SCP_T * 255) : 0x00;
		bufferOut[0x05] = (mask & 0x20) ? (u8)(pressure.SCP_C * 255) : 0x00;
		bufferOut[0x06] = (mask & 0x40) ? (u8)(pressure.SCP_X * 255) : 0x00;
		bufferOut[0x07] = (mask & 0x80) ? (u8)(pressure.SCP_S * 255) : 0x00;

		//l1, r1, l2, r2
		bufferOut[0x08] = (mask & 0x100) ? (u8)(pressure.SCP_L1 * 255) : 0x00;
		bufferOut[0x09] = (mask & 0x200) ? (u8)(pressure.SCP_R1 * 255) : 0x00;
		bufferOut[0x0A] = (mask & 0x400) ? (u8)(pressure.SCP_L2 * 255) : 0x00;
		bufferOut[0x0B] = (mask & 0x800) ? (u8)(pressure.SCP_R2 * 255) : 0x00;

		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
// Dreamcast
////////////////////////////////////////////////////////////////////////

struct watchdogThread
{
	u16 wait;
	u8 port;
};

void VibrationWatchdog(LPVOID param)
{
	if(param == NULL) return;
	watchdogThread *pochy = (watchdogThread*)param;
	Sleep(pochy->wait);
	StopRumble(pochy->port);
}

void FASTCALL DreamcastRumble(s16 intensity, bool freqH, bool freqL, u16 wait,
	_Settings &set, bool &gamepadPlugged, HANDLE &thread)
{
	if(!gamepadPlugged) return;

	XINPUT_VIBRATION vib;

	u16 uIntensity = intensity < 0 ? -intensity : intensity;

	vib.wLeftMotorSpeed = freqH ? 0 : (WORD)((uIntensity * 9362) * set.rumble);
	vib.wRightMotorSpeed = freqL ? 0 : (WORD)((uIntensity * 8192 + 8190) * set.rumble);

	if(thread)
	{
		TerminateThread(thread, 0);
		CloseHandle(thread);
		thread = NULL;
	}

	watchdogThread * th = new watchdogThread();
		th->wait = wait;
		th->port = set.xinputPort;
	
	thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VibrationWatchdog, th, 0, NULL);
	
	if(XInputSetState(set.xinputPort, &vib) != ERROR_SUCCESS)
		gamepadPlugged = false;
}

////////////////////////////////////////////////////////////////////////
// Zilmar
////////////////////////////////////////////////////////////////////////

void FASTCALL N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged)
{
	if(!gamepadPlugged) return;

	static bool pressed[4] = {false};

	if(!pressed[port] && (state[port].Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		pressed[port] = true;
		rumble = !rumble;
	}
	else if(pressed[port] && !(state[port].Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		pressed[port] = false;
	}

	bool ledScrollLock = GetKeyState(VK_SCROLL)&0x1;

	if((!rumble && !ledScrollLock) || (rumble && ledScrollLock))
	{
		keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
		keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
	}
}

void FASTCALL N64rumble(bool on, _Settings &set, bool &gamepadPlugged)
{
	if(!gamepadPlugged) return;

	XINPUT_VIBRATION vib;

	if(on)
	{
		vib.wRightMotorSpeed = Clamp(0xFFFF * set.rumble);
		vib.wLeftMotorSpeed = Clamp(0xFFFF * set.rumble);
	}
	else
	{
		vib.wRightMotorSpeed = 0;
		vib.wLeftMotorSpeed = 0;
	}

	if(XInputSetState(set.xinputPort, &vib) == ERROR_SUCCESS)
		gamepadPlugged = false;
}

} // End namespace Input
#endif // WIN32
