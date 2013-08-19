/*  Pokopom - Input Plugin for PSX/PS2 Emulators
 *  Copyright (C) 2012  KrossX
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Controller.h"
#include "General.h"
//#include "nullDC_Devices.h"
#include "Chankast.h"
#include "Zilmar_Devices.h"

#include <XInput.h>
#pragma comment(lib, "Xinput.lib")

#include "XInput_Backend.h"

#include <math.h>

namespace XInput
{
	////////////////////////////////////////////////////////////////////////
	// General
	////////////////////////////////////////////////////////////////////////

	bool __fastcall Recheck(u8 port)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(port, &state);	

		return (result == ERROR_SUCCESS);
	}
		
	void __fastcall Pause(bool pewpew) { XInputEnable(!pewpew); }

	void __fastcall StopRumble(u8 port, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(port, &state);

		if(result != ERROR_SUCCESS)
		{
			gamepadPlugged = false;
			return;
		}

		XINPUT_VIBRATION vib;

		vib.wLeftMotorSpeed = 0;
		vib.wRightMotorSpeed = 0;

		XInputSetState(port, &vib);
	}

	////////////////////////////////////////////////////////////////////////

	inline WORD Clamp(f64 input)
	{
		return input > 0xFFFF ? 0xFFFF : (WORD)input;
	}
	
	inline s32 ClampAnalog(f64 input)
	{
		return (s32)(input < -32768? -32768 : input > 32767 ? 32767 : input);
	}
	
	template<u8 mode>
	u16 __fastcall ConvertAnalog(s32 X, s32 Y, _Settings &set)
	{							
		// If input is dead, no need to check or do anything else
		if((X == 0) && (Y == 0)) return mode == 0 ? 0x7F7F : 0;

		f64 radius = sqrt((f64)X*X + (f64)Y*Y);
		const f64 deadzone = set.extThreshold * set.deadzone;

		// Input should be effectively dead under deadzone, no need to do more
		if(radius <= deadzone) return mode == 0? 0x7F7F : 0;

		f64 rX = X/radius, rY = Y/radius;

		if(set.linearity != 0) 
			radius = pow(radius / set.extThreshold, set.linearity) * set.extThreshold;

		if(deadzone > 0)
			radius =  (radius - deadzone) * set.extThreshold / (set.extThreshold - deadzone);

		//Antideadzone, inspired by x360ce's setting
		if(set.antiDeadzone > 0)
		{
			const f64 antiDeadzone = set.extThreshold * set.antiDeadzone;
			radius = radius * ((set.extThreshold - antiDeadzone) / set.extThreshold) + antiDeadzone;
		}

		if(radius > set.extThreshold) radius *= set.extMult;

		u16 result = mode == 0? 0x7F7F : 0;

		switch(mode)
		{
		case 0: // Dualshock, Dreamcast...
			{
				X = ClampAnalog(rX * radius);
				Y = ClampAnalog(rY * radius);
				
				Y = 32767 - Y;
				X = X + 32767;

				X = X < 0 ? 0 : X > 0xFFFF ? 0xFFFF : X;
				Y = Y < 0 ? 0 : Y > 0xFFFF ? 0xFFFF : Y;

				X >>= 8;	Y >>= 8;		

				result = (u16)((Y << 8) | X);
			} break;

		case 1: // N64
			{	
				//radius modifier should go here...

				X = ClampAnalog(rX * radius);
				Y = ClampAnalog(rY * radius);
				
				s8 res[2];
				res[0] = (s8)(X>>8);
				res[1] = (s8)(Y>>8);
				
				result = *(u16*)&res;
			} break;
		}
	
		return  result;
	}

	////////////////////////////////////////////////////////////////////////
	// DualShock
	////////////////////////////////////////////////////////////////////////

	void __fastcall DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged)
	{	
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);	
	
		u16 buttons, buttonsStick, analogL, analogR;
		u8 triggerL, triggerR;
		
		buttons = buttonsStick = 0xFFFF;
		analogL = analogR = 0x7F7F;
		triggerL = triggerR = 0;

		if(result == ERROR_SUCCESS) 
		{	
			buttons = 0;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 0:1) << 0x0; // Select
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 0:1) << 0x1; // L3
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 0:1) << 0x2; // R3
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 0:1) << 0x3; // Start
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 0:1) << 0x4; // Up
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 0:1) << 0x5; // Right
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 0:1) << 0x6; // Down		
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 0:1) << 0x7; // Left

			buttons |= (state.Gamepad.bLeftTrigger > 10 ? 0:1) << 0x8; // L2
			buttons |= (state.Gamepad.bRightTrigger > 10 ? 0:1) << 0x9; // R2

			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 0:1) << 0xA; // L1
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 0:1) << 0xB; // R1
	
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 0:1) << 0xC; // Triangle
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 0:1) << 0xD; // Circle
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 0:1) << 0xE; // Cross
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 0:1) << 0xF; // Square

			buttonsStick = buttons | 0x06;
			const s32 threshold = 16384;

			set.axisValue[GP_AXIS_LY] = state.Gamepad.sThumbLY * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
			set.axisValue[GP_AXIS_LX] = state.Gamepad.sThumbLX * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
			set.axisValue[GP_AXIS_RY] = state.Gamepad.sThumbRY * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
			set.axisValue[GP_AXIS_RX] = state.Gamepad.sThumbRX * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);		
				
			if(state.Gamepad.sThumbLY > threshold) buttonsStick &= ~(1 << 0x4);
			if(state.Gamepad.sThumbLX > threshold) buttonsStick &= ~(1 << 0x5);
			if(state.Gamepad.sThumbLY < -threshold) buttonsStick &= ~(1 << 0x6);
			if(state.Gamepad.sThumbLX < -threshold) buttonsStick &= ~(1 << 0x7);

			if(state.Gamepad.sThumbRY > threshold) buttonsStick &= ~(1 << 0xC);
			if(state.Gamepad.sThumbRX > threshold) buttonsStick &= ~(1 << 0xD);
			if(state.Gamepad.sThumbRY < -threshold) buttonsStick &= ~(1 << 0xE);
			if(state.Gamepad.sThumbRX < -threshold) buttonsStick &= ~(1 << 0xF);

			analogL = ConvertAnalog<0>(set.axisValue[set.axisRemap[GP_AXIS_LX]], 
									set.axisValue[set.axisRemap[GP_AXIS_LY]], set);

			analogR = ConvertAnalog<0>(set.axisValue[set.axisRemap[GP_AXIS_RX]], 
									set.axisValue[set.axisRemap[GP_AXIS_RY]], set);
		
			triggerL = state.Gamepad.bLeftTrigger;
			triggerR = state.Gamepad.bRightTrigger;
		
			//printf("Pokopom: %04X %04X\n", analogL, analogR);
		}
		else
			gamepadPlugged = false;

		bufferOut[0] = buttons;
		bufferOut[1] = buttonsStick;
		bufferOut[2] = analogL;
		bufferOut[3] = analogR;
		bufferOut[4] = (triggerL << 8) | triggerR;				
	}

	void __fastcall DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged)
	{	
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);		

		if(result == ERROR_SUCCESS)
		{			
			//printf("Vibrate! [%X] [%X]\n", smalldata, bigdata);
		
			static XINPUT_VIBRATION vib;
			static DWORD timerS = 0, timerB = 0;		
		
			if(smalldata)
			{			
				vib.wRightMotorSpeed = Clamp(0xFFFF * set.rumble);
				timerS = GetTickCount();			
			}
			else if (vib.wRightMotorSpeed && GetTickCount() - timerS > 150)
			{						
				vib.wRightMotorSpeed = 0; 
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
			
				vib.wLeftMotorSpeed = Clamp(broom * set.rumble);
				timerB = GetTickCount();					
			}
			else if (vib.wLeftMotorSpeed && GetTickCount() - timerB > 150)
			{						
				vib.wLeftMotorSpeed = 0; 
			}

			/*
		
			vib.wRightMotorSpeed = smalldata == 0? 0 : 0xFFFF;
			vib.wLeftMotorSpeed = bigdata * 0x101;	

			vib.wRightMotorSpeed = Clamp(vib.wRightMotorSpeed * settings.rumble);
			vib.wLeftMotorSpeed = Clamp(vib.wLeftMotorSpeed * settings.rumble);
			*/

			//printf("Vibrate! [%X] [%X]\n", vib.wLeftMotorSpeed, vib.wRightMotorSpeed);
		

			XInputSetState(set.xinputPort, &vib);
		}	
		else
			gamepadPlugged = false;
	}

	////////////////////////////////////////////////////////////////////////
	// Dreamcast
	////////////////////////////////////////////////////////////////////////

	void __fastcall DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);

		u16* buffer = (u16*) buffer_out;

		// Some magic number...
		buffer[0] = 0x0000;
		buffer[1] = 0x0100;

		u16 buttons = 0xFFFF;
		u16 analog = 0x8080;
		u16 triggers = 0x0000;

		static bool analogToggle = false;

		if(result == ERROR_SUCCESS) 
		{	
			buttons = 0;

			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 0:1)	<< 0x1; // B
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 0:1)	<< 0x2; // A
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 0:1)		<< 0x3; // Start
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 0:1)		<< 0x4; // Up
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 0:1)	<< 0x5; // Down
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 0:1)	<< 0x6; // Left
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 0:1)	<< 0x7; // Right
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 0:1)	<< 0x9; // Y
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 0:1)	<< 0xA; // X

			triggers = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 0xFF : (state.Gamepad.bLeftTrigger&0xFF))<<8;
			triggers |= state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 0xFF : state.Gamepad.bRightTrigger&0xFF;

			if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) analogToggle = false;
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) analogToggle = true;

			set.axisValue[GP_AXIS_LY] = state.Gamepad.sThumbLY * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
			set.axisValue[GP_AXIS_LX] = state.Gamepad.sThumbLX * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
			set.axisValue[GP_AXIS_RY] = state.Gamepad.sThumbRY * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
			set.axisValue[GP_AXIS_RX] = state.Gamepad.sThumbRX * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);

			const s32 threshold = 16384;

			if(analogToggle)
			{
				analog = ConvertAnalog<0>(set.axisValue[set.axisRemap[GP_AXIS_RX]],
									   set.axisValue[set.axisRemap[GP_AXIS_RY]], set);

				set.axisValue[GP_AXIS_LY] *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;
				set.axisValue[GP_AXIS_LX] *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
				set.axisValue[GP_AXIS_RY] *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
				set.axisValue[GP_AXIS_RX] *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;

				// Inactive left stick to work as dpad
				if(set.axisValue[set.axisRemap[GP_AXIS_LY]] > threshold) buttons &= ~(1 << 0x4);
				if(set.axisValue[set.axisRemap[GP_AXIS_LX]] > threshold) buttons &= ~(1 << 0x7);
				if(set.axisValue[set.axisRemap[GP_AXIS_LY]] < -threshold) buttons &= ~(1 << 0x5);
				if(set.axisValue[set.axisRemap[GP_AXIS_LX]] < -threshold) buttons &= ~(1 << 0x6);
			}
			else
			{
				analog = ConvertAnalog<0>(set.axisValue[set.axisRemap[GP_AXIS_LX]], 
									   set.axisValue[set.axisRemap[GP_AXIS_LY]], set);

				set.axisValue[GP_AXIS_LY] *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;
				set.axisValue[GP_AXIS_LX] *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
				set.axisValue[GP_AXIS_RY] *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
				set.axisValue[GP_AXIS_RX] *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;

				// Inactive right stick to work as face buttons
				if(set.axisValue[set.axisRemap[GP_AXIS_RY]] > threshold) buttons &= ~(1 << 0x9);
				if(set.axisValue[set.axisRemap[GP_AXIS_RX]] > threshold) buttons &= ~(1 << 0x1);
				if(set.axisValue[set.axisRemap[GP_AXIS_RY]] < -threshold) buttons &= ~(1 << 0x2);
				if(set.axisValue[set.axisRemap[GP_AXIS_RX]] < -threshold) buttons &= ~(1 << 0xA);
			}

		}
		else 
			gamepadPlugged = false;

		// Buttons
		buffer[2] = buttons | 0xF901;
	
		// Triggers
		buffer[3] = triggers;
	
		// Left Stick
		buffer[4] = analog;
	
		// Right Stick... not present.
		buffer[5] = 0x8080;
	}
		
	void __fastcall VibrationWatchdog(LPVOID param)
	{
		PuruPuruPack::_thread *pochy = (PuruPuruPack::_thread*)param;
		Sleep(pochy->wait);
		StopRumble(pochy->port, pochy->gamepadPlugged);
	}

	void __fastcall DreamcastRumble(s16 intensity, bool freqH, bool freqL, LPVOID thread, 
		_Settings &set, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);

		if(result != ERROR_SUCCESS)
		{
			gamepadPlugged = false;
			return;
		}

		PuruPuruPack::_thread *th = (PuruPuruPack::_thread*)thread;
		XINPUT_VIBRATION vib;
			
		u16 uIntensity = intensity < 0 ? -intensity : intensity;
		
		vib.wLeftMotorSpeed = freqH ? 0 : (WORD)((uIntensity * 9362) * set.rumble);
		vib.wRightMotorSpeed = freqL ? 0 : (WORD)((uIntensity * 8192 + 8190) * set.rumble);

		if(th->hThread) 
		{
			TerminateThread(th->hThread, 0);
			CloseHandle(th->hThread);
			th->hThread = NULL;
		}
				
		th->hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VibrationWatchdog, thread, 0, NULL);
		
		XInputSetState(set.xinputPort, &vib);
	}

	////////////////////////////////////////////////////////////////////////
	// Zilmar
	////////////////////////////////////////////////////////////////////////

	enum
	{
		N64_RIGHT = 0,
		N64_LEFT,
		N64_DOWN,
		N64_UP,
		N64_START,
		N64_TRIGGERZ,
		N64_B,
		N64_A,
		N64_CRIGHT,
		N64_CLEFT,		
		N64_CDOWN,		
		N64_CUP,
		N64_TRIGGERR,
		N64_TRIGGERL
	};

	void __fastcall N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(port, &state);

		static bool pressed[4] = {false};

		if(result == ERROR_SUCCESS)
		{
			if(!pressed[port] && (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
			{
				pressed[port] = true;
				rumble = !rumble;
			}
			else if(pressed[port] && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
			{
				pressed[port] = false;
			}
		}
		else
			gamepadPlugged = false;

		if(gamepadPlugged && !pressed[port])		
		{
			bool ledScrollLock = GetKeyState(VK_SCROLL)&0x1;

			if((!rumble && !ledScrollLock) || (rumble && ledScrollLock))
			{
				keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
				keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );	
			}
		}
	}

	void __fastcall N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);

		u16 buttons = 0;
		u16 analog = 0x0000;

		static bool analogToggle = false;

		if(result == ERROR_SUCCESS) 
		{	
			buttons = 0;

			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1:0) << N64_RIGHT;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1:0)  << N64_LEFT;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1:0)  << N64_DOWN;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1:0)	  << N64_UP;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1:0)	  << N64_START;

			buttons |= (state.Gamepad.bRightTrigger > 10? 1:0)	<< N64_TRIGGERZ;
			
			if(state.Gamepad.bLeftTrigger > 100)
			{
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1:0) << N64_CLEFT;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1:0) << N64_CUP;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1:0) << N64_CDOWN;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1:0) << N64_CRIGHT;
			}
			else
			{
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1:0) << N64_B;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1:0) << N64_B;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1:0) << N64_A;
				buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1:0) << N64_A;
			}

			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1:0) << N64_TRIGGERR;
			buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1:0)  << N64_TRIGGERL;
		
			if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) analogToggle = false;
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) analogToggle = true;

			set.axisValue[GP_AXIS_LY] = state.Gamepad.sThumbLY * (set.axisInverted[GP_AXIS_LY] ? -1 : 1);
			set.axisValue[GP_AXIS_LX] = state.Gamepad.sThumbLX * (set.axisInverted[GP_AXIS_LX] ? -1 : 1);
			set.axisValue[GP_AXIS_RY] = state.Gamepad.sThumbRY * (set.axisInverted[GP_AXIS_RY] ? -1 : 1);
			set.axisValue[GP_AXIS_RX] = state.Gamepad.sThumbRX * (set.axisInverted[GP_AXIS_RX] ? -1 : 1);

			const s32 threshold = 16384;

			if(analogToggle)
			{
				analog = ConvertAnalog<1>(set.axisValue[set.axisRemap[GP_AXIS_RX]], 
									   set.axisValue[set.axisRemap[GP_AXIS_RY]], set);

				set.axisValue[GP_AXIS_LY] *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;
				set.axisValue[GP_AXIS_LX] *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
				set.axisValue[GP_AXIS_RY] *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
				set.axisValue[GP_AXIS_RX] *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;

				if(set.axisValue[set.axisRemap[GP_AXIS_LX]] >  threshold) buttons |= (1 << N64_CRIGHT);
				if(set.axisValue[set.axisRemap[GP_AXIS_LX]] < -threshold) buttons |= (1 << N64_CLEFT);
				if(set.axisValue[set.axisRemap[GP_AXIS_LY]] < -threshold) buttons |= (1 << N64_CDOWN);
				if(set.axisValue[set.axisRemap[GP_AXIS_LY]] >  threshold) buttons |= (1 << N64_CUP);
			}
			else
			{
				analog = ConvertAnalog<1>(set.axisValue[set.axisRemap[GP_AXIS_LX]], 
									   set.axisValue[set.axisRemap[GP_AXIS_LY]], set);

				set.axisValue[GP_AXIS_LY] *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;
				set.axisValue[GP_AXIS_LX] *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
				set.axisValue[GP_AXIS_RY] *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
				set.axisValue[GP_AXIS_RX] *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;

				if(set.axisValue[set.axisRemap[GP_AXIS_RX]] >  threshold) buttons |= (1 << N64_CRIGHT);
				if(set.axisValue[set.axisRemap[GP_AXIS_RX]] < -threshold) buttons |= (1 << N64_CLEFT);
				if(set.axisValue[set.axisRemap[GP_AXIS_RY]] < -threshold) buttons |= (1 << N64_CDOWN);
				if(set.axisValue[set.axisRemap[GP_AXIS_RY]] >  threshold) buttons |= (1 << N64_CUP);
			}

		}
		else 
			gamepadPlugged = false;

		u16 * outBig = (u16*)outBuffer;
	
		outBig[0] = buttons;
		outBig[1] = analog;
	}

	void __fastcall N64rumble(bool on, _Settings &set, bool &gamepadPlugged)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(set.xinputPort, &state);		

		if(result == ERROR_SUCCESS)
		{			
			static XINPUT_VIBRATION vib;

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

			XInputSetState(set.xinputPort, &vib);
		}	
		else
			gamepadPlugged = false;
	}

} // End namespace