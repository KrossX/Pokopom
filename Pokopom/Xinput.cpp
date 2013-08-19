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
#include "nullDC_Devices.h"

#include <XInput.h>
#pragma comment(lib, "Xinput.lib")

#include <math.h>

inline int ClampAnalog(double input)
{
	return (int)(input < -32768? -32768 : input > 32767 ? 32767 : input);
}

unsigned short ConvertAnalog(int X, int Y, _Settings &set)
{							
	// If input is dead, no need to check or do anything else
	if((X == 0) && (Y == 0)) return 0x7F7F;

	double radius = sqrt((double)X*X + (double)Y*Y);
	const double deadzone = set.extThreshold * set.deadzone;

	// Input should be effectively dead under deadzone, no need to do more
	if(radius <= deadzone) return 0x7F7F;

	double rX = X/radius, rY = Y/radius;

	if(set.linearity != 0) 
		radius = pow(radius / set.extThreshold, set.linearity) * set.extThreshold;

	if(deadzone > 0)
		radius =  (radius - deadzone) * set.extThreshold / (set.extThreshold - deadzone);

	//Antideadzone, inspired by x360ce's setting
	if(set.antiDeadzone > 0)
	{
		const double antiDeadzone = set.extThreshold * set.antiDeadzone;
		radius = radius * ((set.extThreshold - antiDeadzone) / set.extThreshold) + antiDeadzone;		
	}

	if(radius > set.extThreshold) radius *= set.extMult;
	
	X = ClampAnalog(rX * radius);
	Y = ClampAnalog(rY * radius);
		
	Y = 32767 - Y;
	X = X + 32767;

	X = X < 0 ? 0 : X > 0xFFFF ? 0xFFFF : X;
	Y = Y < 0 ? 0 : Y > 0xFFFF ? 0xFFFF : Y;

	X >>= 8;	Y >>= 8;		

	unsigned short result = (unsigned short)((Y << 8) | X);
	
	return  result;
}

void Controller::poll()
{	
	XINPUT_STATE state;
	DWORD result = XInputGetState(settings.xinputPort, &state);	
	
	buttons = 0xFFFF;
	analogL = analogR = 0x7F7F;

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
		const int threshold = 16384;

		settings.axisValue[GP_AXIS_LY] = state.Gamepad.sThumbLY * (settings.axisInverted[GP_AXIS_LY] ? -1 : 1);
		settings.axisValue[GP_AXIS_LX] = state.Gamepad.sThumbLX * (settings.axisInverted[GP_AXIS_LX] ? -1 : 1);
		settings.axisValue[GP_AXIS_RY] = state.Gamepad.sThumbRY * (settings.axisInverted[GP_AXIS_RY] ? -1 : 1);
		settings.axisValue[GP_AXIS_RX] = state.Gamepad.sThumbRX * (settings.axisInverted[GP_AXIS_RX] ? -1 : 1);		
				
		if(settings.axisValue[GP_AXIS_LY] > threshold) buttonsStick &= ~(1 << 0x4);
		if(settings.axisValue[GP_AXIS_LX] > threshold) buttonsStick &= ~(1 << 0x5);
		if(settings.axisValue[GP_AXIS_LY] < -threshold) buttonsStick &= ~(1 << 0x6);
		if(settings.axisValue[GP_AXIS_LX] < -threshold) buttonsStick &= ~(1 << 0x7);

		if(settings.axisValue[GP_AXIS_RY] > threshold) buttonsStick &= ~(1 << 0xC);
		if(settings.axisValue[GP_AXIS_RX] > threshold) buttonsStick &= ~(1 << 0xD);
		if(settings.axisValue[GP_AXIS_RY] < -threshold) buttonsStick &= ~(1 << 0xE);
		if(settings.axisValue[GP_AXIS_RX] < -threshold) buttonsStick &= ~(1 << 0xF);

		analogL = ConvertAnalog(settings.axisValue[settings.axisRemap[GP_AXIS_LX]], settings.axisValue[settings.axisRemap[GP_AXIS_LY]], settings);
		analogR = ConvertAnalog(settings.axisValue[settings.axisRemap[GP_AXIS_RX]], settings.axisValue[settings.axisRemap[GP_AXIS_RY]], settings);
		
		triggerL = state.Gamepad.bLeftTrigger;
		triggerR = state.Gamepad.bRightTrigger;
		
		//printf("Pokopom: %04X %04X\n", analogL, analogR);
	}
	else
		gamepadPlugged = false;

	//Return
}

void Controller::Recheck()
{
	XINPUT_STATE state;
	DWORD result = XInputGetState(settings.xinputPort, &state);	

	if(result == ERROR_SUCCESS) gamepadPlugged = true;
}

inline WORD Clamp(double input)
{
	unsigned int result = (unsigned int) input;
	result = result > 0xFFFF ? 0xFFFF : result;
	return (WORD)result;
}

void Controller::vibration(unsigned char smalldata, unsigned char bigdata)
{	
	XINPUT_STATE state;
	DWORD result = XInputGetState(settings.xinputPort, &state);		

	if(result == ERROR_SUCCESS)
	{			
		//printf("Vibrate! [%X] [%X]\n", smalldata, bigdata);
		
		static XINPUT_VIBRATION vib;
		static DWORD timerS = 0, timerB = 0;		
		
		if(smalldata)
		{			
			vib.wRightMotorSpeed = Clamp(0xFFFF * settings.rumble);
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
			double broom = 0.006474549734772402 * pow(bigdata, 3.0) -
				1.258165252213538 *  pow(bigdata, 2.0) + 
				156.82454281087692 * bigdata + 
				3.637978807091713e-11;


			/*
			unsigned int broom = bigdata;

			if(bigdata <= 0x2C) broom *= 0x72;
			else if(bigdata <= 0x53) broom = 0x13C7 + bigdata * 0x24;
			else broom *= 0x205;
			*/
			
			vib.wLeftMotorSpeed = Clamp(broom * settings.rumble);
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
		

		XInputSetState(settings.xinputPort, &vib);							
	}	
	else
		gamepadPlugged = false;
}

void XInputPaused(bool pewpew)
{	XInputEnable(!pewpew); }

////////////////////////////////////////////////////////////////////////
// nullDC stuff below...
////////////////////////////////////////////////////////////////////////

void DreamcastController::PollOut(unsigned int* buffer_out)
{
	XINPUT_STATE state;
	DWORD result = XInputGetState(port, &state);

	unsigned short* buffer = (unsigned short*) buffer_out;

	// Some magic number...
	buffer[0] = 0x0100;
	buffer[1] = 0x0000;

	unsigned short buttons = 0xFFFF;
	unsigned short analog = 0x8080;
	unsigned short triggers = 0x0000;

	if(result == ERROR_SUCCESS) 
	{	
		unsigned short buttons = 0;

		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 0:1)	<< 0x1; // B
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 0:1)	<< 0x2; // A
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 0:1)		<< 0x3; // Start
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 0:1)		<< 0x4; // Up
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 0:1)	<< 0x5; // Down
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 0:1)	<< 0x6; // Left
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 0:1)	<< 0x7; // Right
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 0:1)	<< 0x9; // X
		buttons |= (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 0:1)	<< 0xA; // Y

		triggers = ((state.Gamepad.bLeftTrigger&0xFF)<<8) | (state.Gamepad.bRightTrigger&0xFF);
		analog = ConvertAnalog(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, set);
	}

	// Buttons
	buffer[2] = buttons | 0xF901;
	
	// Triggers
	buffer[3] = triggers;
	
	// Left Stick
	buffer[4] = analog;
	
	// Right Stick... not present.
	buffer[5] = 0x8080;
}