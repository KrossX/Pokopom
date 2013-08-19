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

#include "General.h"
#include "Input.h"
#include "Input_Shared.h"

namespace Input
{
const u16 ANALOG_TRESHOLD = 12540;

const f64 PI = 3.14159265358979323846;
const f64 PI_HALF = PI / 2.0;
const f64 PI_2 = 2 * PI;

void FASTCALL GetRadius(_Stick& stick)
{
	f64 X = stick.X;
	f64 Y = stick.Y;
	stick.radius = sqrt(X*X + Y*Y);
}

u8 FASTCALL GetAnalogDigital(_Stick& stick)
{
	u8 data = 0;

	if(stick.radius > 28000)
	{
		f64 angle = atan2((f64)stick.Y, (f64)stick.X) / PI_2;
		stick.angle = angle = angle < 0 ? angle + 1 : angle;

		u8 angled = (u8)(angle * 16);
		
		switch(angled)
		{
		case 0: 
			data = ANALOGD_XP; 
			break;

		case 1: case 2: 
			data = ANALOGD_XP | ANALOGD_YP;
			break;

		case 3: case 4: 
			data = ANALOGD_YP;
			break;

		case 5: case 6: 
			data = ANALOGD_XN | ANALOGD_YP;
			break;

		case 7: case 8: 
			data = ANALOGD_XN;
			break;

		case 9: case 10: 
			data = ANALOGD_XN | ANALOGD_YN;
			break;

		case 11: case 12: 
			data = ANALOGD_YN;
			break;

		case 13: case 14: 
			data = ANALOGD_XP | ANALOGD_YN;
			break;

		case 15: 
			data = ANALOGD_XP;
			break;
		}
	}

	return data;
}

u16 FASTCALL ConvertAnalog(_Stick& stick, _Settings &set, u8 mode)
{
	s32 X = stick.X;
	s32 Y = stick.Y;
	u16 result = mode == 0? 0x7F7F : 0;
	
	// If input is dead, no need to check or do anything else
	if((X == 0) && (Y == 0)) return result;

	f64 radius = stick.radius;
	const f64 deadzone = set.extThreshold * set.deadzone;

	// Input should be effectively dead under deadzone, no need to do more
	if(radius <= deadzone) return result;

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

	X = ClampAnalog(rX * radius);
	Y = ClampAnalog(rY * radius);

	switch(mode)
	{
	case 0: // Dualshock, Dreamcast...
		{
			Y = 32767 - Y;
			X = X + 32767;

			X = X < 0 ? 0 : X > 0xFFFF ? 0xFFFF : X;
			Y = Y < 0 ? 0 : Y > 0xFFFF ? 0xFFFF : Y;

			X = (s32)(X / 256.5f);
			Y = (s32)(Y / 256.5f);

			result = (u16)((Y << 8) | X);
		} break;

	case 1: // N64
		{
			s8 res[2];
			res[0] = (s8)(X / 256.5f);
			res[1] = (s8)(Y / 256.5f);

			u16 * r16 = (u16*)&res[0];
			result = r16[0];
		} break;
	}

	return  result;
}

////////////////////////////////////////////////////////////////////////
// DualShock
////////////////////////////////////////////////////////////////////////


void FASTCALL DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged, const bool digital)
{
	_Pad pad;

	u16 buttons, buttonsStick, analogL, analogR;
	u8 triggerL, triggerR;

	buttons = buttonsStick = 0xFFFF;
	analogL = analogR = 0x7F7F;
	triggerL = triggerR = 0;

	if(InputGetState(pad, set))
	{
		buttons = 0;
		buttons |= (~pad.buttons[X360_BACK]  & 0x1) << DS_SELECT;
		buttons |= (~pad.buttons[X360_LS]    & 0x1) << DS_L3;
		buttons |= (~pad.buttons[X360_RS]    & 0x1) << DS_R3;
		buttons |= (~pad.buttons[X360_START] & 0x1) << DS_START;
		buttons |= (~pad.buttons[X360_DUP]   & 0x1) << DS_UP;
		buttons |= (~pad.buttons[X360_DRIGHT]& 0x1) << DS_RIGHT;
		buttons |= (~pad.buttons[X360_DDOWN] & 0x1) << DS_DOWN;
		buttons |= (~pad.buttons[X360_DLEFT] & 0x1) << DS_LEFT;

		buttons |= (pad.analog[X360_TRIGGERL] > 10 ? 0:1) << DS_L2;
		buttons |= (pad.analog[X360_TRIGGERR] > 10 ? 0:1) << DS_R2;

		buttons |= (~pad.buttons[X360_LB] & 0x1) << DS_L1;
		buttons |= (~pad.buttons[X360_RB] & 0x1) << DS_R1;

		buttons |= (~pad.buttons[X360_Y] & 0x1) << DS_TRIANGLE;
		buttons |= (~pad.buttons[X360_B] & 0x1) << DS_CIRCLE;
		buttons |= (~pad.buttons[X360_A] & 0x1) << DS_CROSS;
		buttons |= (~pad.buttons[X360_X] & 0x1) << DS_SQUARE;

		if(digital)
		{
			u8 stickLD = GetAnalogDigital(pad.stickL);
			u8 stickRD = GetAnalogDigital(pad.stickR);

			buttonsStick = buttons | 0x06;
			buttonsStick &= ~((stickLD & ANALOGD_XP) << DS_RIGHT);
			buttonsStick &= ~(((stickLD & ANALOGD_XN) >> 1) << DS_LEFT);
			buttonsStick &= ~(((stickLD & ANALOGD_YP) >> 2) << DS_UP);
			buttonsStick &= ~(((stickLD & ANALOGD_YN) >> 3) << DS_DOWN);
		
			buttonsStick &= ~((stickRD & ANALOGD_XP) << DS_CIRCLE);
			buttonsStick &= ~(((stickRD & ANALOGD_XN) >> 1) << DS_SQUARE);
			buttonsStick &= ~(((stickRD & ANALOGD_YP) >> 2) << DS_TRIANGLE);
			buttonsStick &= ~(((stickRD & ANALOGD_YN) >> 3) << DS_CROSS);
		}
		else
		{
			analogL = ConvertAnalog(pad.modL, set, 0);
			analogR = ConvertAnalog(pad.modR, set, 0);

			triggerL = pad.analog[X360_TRIGGERL] & 0xFF;
			triggerR = pad.analog[X360_TRIGGERR] & 0xFF;
		}

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

////////////////////////////////////////////////////////////////////////
// Dreamcast
////////////////////////////////////////////////////////////////////////

void FASTCALL DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged)
{
	_Pad pad;

	u16* buffer = (u16*) buffer_out;

	// Some magic number...
	buffer[0] = 0x0000;
	buffer[1] = 0x0100;

	u16 buttons = 0xFFFF;
	u16 analog = 0x8080;
	u16 triggers = 0x0000;

	static bool analogToggle = false;

	if(InputGetState(pad, set))
	{
		buttons = 0;

		buttons |= pad.buttons[X360_A] << DC_A;
		buttons |= pad.buttons[X360_B] << DC_B;
		buttons |= pad.buttons[X360_X] << DC_X;
		buttons |= pad.buttons[X360_Y] << DC_Y;
		
		buttons |= pad.buttons[X360_DUP]   << DC_UP;
		buttons |= pad.buttons[X360_DDOWN] << DC_DOWN;
		buttons |= pad.buttons[X360_DLEFT] << DC_LEFT;
		buttons |= pad.buttons[X360_DRIGHT]<< DC_RIGHT;
		
		buttons |= pad.buttons[X360_START] << DC_START;

		triggers = (pad.buttons[X360_LB] ? 0xFF : (pad.analog[X360_TRIGGERL]&0xFF))<<8;
		triggers |= pad.buttons[X360_RB] ? 0xFF : pad.analog[X360_TRIGGERR]&0xFF;

		if(pad.buttons[X360_LS]) analogToggle = false;
		else if (pad.buttons[X360_RS]) analogToggle = true;

		if(analogToggle)
		{
			analog = ConvertAnalog(pad.modR, set, 0);

			_Stick stickL = pad.modL;
			stickL.X *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
			stickL.Y *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;

			u8 stickD = GetAnalogDigital(stickL);

			// Inactive left stick to work as dpad
			buttons |= (stickD & ANALOGD_XP) << DC_RIGHT;
			buttons |= ((stickD & ANALOGD_XN) >> 1) << DC_LEFT;
			buttons |= ((stickD & ANALOGD_YP) >> 2) << DC_UP;
			buttons |= ((stickD & ANALOGD_YN) >> 3) << DC_DOWN;
		}
		else
		{
			analog = ConvertAnalog(pad.modL, set, 0);

			_Stick stickR = pad.modR;
			stickR.X *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;
			stickR.Y *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
			
			u8 stickD = GetAnalogDigital(stickR);

			// Inactive right stick to work as face buttons
			buttons |= (stickD & ANALOGD_XP) << DC_B;
			buttons |= ((stickD & ANALOGD_YP) >> 2) << DC_Y;
			buttons |= ((stickD & ANALOGD_XN) >> 1) << DC_X;
			buttons |= ((stickD & ANALOGD_YN) >> 3) << DC_A;
		}

		buttons = ~buttons;
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

////////////////////////////////////////////////////////////////////////
// NAOMI
////////////////////////////////////////////////////////////////////////

void FASTCALL NaomiPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged)
{
	_Pad pad[2];

	u8 * buffer = (u8*)buffer_out;
	buffer = &buffer[26];

	u8 service = 0;

	//DWord analog[2]; 
	Word player; 
	
	player.bits32 = 0;
	//analog[0].bits64 = 0;
	//analog[1].bits64 = 0;

	static Word coin;
	static bool coinPressed[2] = {false};
	
	for(u8 i = 0; i < 2; i++)
		if(InputGetState(pad[i], settings[i]))
	{
		player.bits16[i] |= pad[i].buttons[X360_DRIGHT]<< NAOMI_DRIGHT;
		player.bits16[i] |= pad[i].buttons[X360_DLEFT] << NAOMI_DLEFT;
		player.bits16[i] |= pad[i].buttons[X360_DDOWN] << NAOMI_DDOWN;
		player.bits16[i] |= pad[i].buttons[X360_DUP]   << NAOMI_DUP;

		player.bits16[i] |= pad[i].buttons[X360_A] << NAOMI_BUTTON1;
		player.bits16[i] |= pad[i].buttons[X360_B] << NAOMI_BUTTON2;

		player.bits16[i] |= pad[i].buttons[X360_X] << NAOMI_BUTTON3;
		player.bits16[i] |= pad[i].buttons[X360_Y] << NAOMI_BUTTON4;
		player.bits16[i] |= pad[i].buttons[X360_LB] << NAOMI_BUTTON5;
		player.bits16[i] |= pad[i].buttons[X360_RB] << NAOMI_BUTTON6;

		/*
		analog[i].bits16[0] = (pad[i].analog[X360_STICKLX] + 32768) & 0xFFFF;
		analog[i].bits16[1] = (pad[i].analog[X360_STICKLY] + 32768) & 0xFFFF;
		analog[i].bits16[2] = (pad[i].analog[X360_STICKRX] + 32768) & 0xFFFF;
		analog[i].bits16[3] = (pad[i].analog[X360_STICKRY] + 32768) & 0xFFFF;
		*/

		u8 stickD = GetAnalogDigital(pad[i].modL);

		player.bits16[i] |= (stickD & ANALOGD_XP) << NAOMI_DRIGHT;
		player.bits16[i] |= ((stickD & ANALOGD_XN) >> 1) << NAOMI_DLEFT;
		player.bits16[i] |= ((stickD & ANALOGD_YP) >> 2) << NAOMI_DUP;
		player.bits16[i] |= ((stickD & ANALOGD_YN) >> 3) << NAOMI_DDOWN;

		if(pad[i].analog[X360_TRIGGERL] > 100)
		{
			service |= pad[i].buttons[X360_START] << NAOMI_START;
			player.bits16[i] |= pad[i].buttons[X360_BACK] << NAOMI_TEST;
		}
		else
		{
			player.bits16[i] |= pad[i].buttons[X360_START] << NAOMI_START;

			if(pad[i].buttons[X360_BACK] && !coinPressed[i])
			{
				coinPressed[i] = true;
				coin.bits16[i]++;
			}
			else if(coinPressed[i] && !pad[i].buttons[X360_BACK])
				coinPressed[i] = false;
		}
	}

	buffer[ 0] = 1; // Enabled?
	buffer[ 1] = service;	
	buffer[ 2] = player.bits8[0];
	buffer[ 3] = player.bits8[1];
	buffer[ 4] = player.bits8[2];
	buffer[ 5] = player.bits8[3];
	buffer[ 6] = 1;
	buffer[ 7] = coin.bits8[1];
	buffer[ 8] = coin.bits8[0];
	buffer[ 9] = coin.bits8[3];
	buffer[10] = coin.bits8[2];
	buffer[11] = 0;

	/* Analogs screw things up... =S */
	/*
	buffer[12] = analog[0].bits8[1];
	buffer[13] = analog[0].bits8[0];
	buffer[14] = analog[0].bits8[3];
	buffer[15] = analog[0].bits8[2];
	buffer[16] = analog[0].bits8[5];
	buffer[17] = analog[0].bits8[4];
	buffer[18] = analog[0].bits8[7];
	buffer[19] = analog[0].bits8[6];
	buffer[20] = 1;//analog[0].bits8[1];
	buffer[21] = analog[0].bits8[0];
	buffer[22] = analog[0].bits8[3];
	buffer[23] = analog[0].bits8[2];
	buffer[24] = analog[0].bits8[5];
	buffer[25] = analog[0].bits8[4];
	buffer[26] = analog[0].bits8[7];
	buffer[27] = analog[0].bits8[6];
	*/
}


////////////////////////////////////////////////////////////////////////
// Zilmar
////////////////////////////////////////////////////////////////////////

void FASTCALL N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged)
{
	_Pad pad;

	u16 buttons = 0;
	u16 analog = 0x0000;

	static bool analogToggle = false;

	if(InputGetState(pad, set))
	{
		buttons = 0;

		buttons |= pad.buttons[X360_DRIGHT]<< N64_RIGHT;
		buttons |= pad.buttons[X360_DLEFT] << N64_LEFT;
		buttons |= pad.buttons[X360_DDOWN] << N64_DOWN;
		buttons |= pad.buttons[X360_DUP]   << N64_UP;
		buttons |= pad.buttons[X360_START] << N64_START;

		buttons |= (pad.analog[X360_TRIGGERL] > 10? 1:0) << N64_TRIGGERZ;

		if(pad.analog[X360_TRIGGERR] > 100)
		{
			buttons |= pad.buttons[X360_X] << N64_CLEFT;
			buttons |= pad.buttons[X360_Y] << N64_CUP;
			buttons |= pad.buttons[X360_A] << N64_CDOWN;
			buttons |= pad.buttons[X360_B] << N64_CRIGHT;
		}
		else
		{
			buttons |= pad.buttons[X360_A] << N64_A;
			buttons |= pad.buttons[X360_X] << N64_B;

			buttons |= pad.buttons[X360_B] << N64_CDOWN;
			buttons |= pad.buttons[X360_Y] << N64_CLEFT;
		}

		buttons |= pad.buttons[X360_RB] << N64_TRIGGERR;
		buttons |= pad.buttons[X360_LB] << N64_TRIGGERL;

		if(pad.buttons[X360_LS]) analogToggle = false;
		else if (pad.buttons[X360_RS]) analogToggle = true;

		if(analogToggle)
		{
			analog = ConvertAnalog(pad.modR, set, 1);

			_Stick stickL = pad.modL;
			stickL.X *= set.axisInverted[GP_AXIS_LX] ? -1 : 1;
			stickL.Y *= set.axisInverted[GP_AXIS_LY] ? -1 : 1;

			u8 stickD = GetAnalogDigital(stickL);

			buttons |= ((stickD & ANALOGD_XP) << N64_CRIGHT);
			buttons |= (((stickD & ANALOGD_XN) >> 1) << N64_CLEFT);
			buttons |= (((stickD & ANALOGD_YP) >> 2) << N64_CUP);
			buttons |= (((stickD & ANALOGD_YN) >> 3) << N64_CDOWN);
		}
		else
		{
			analog = ConvertAnalog(pad.modL, set, 1);

			_Stick stickR = pad.modR;
			stickR.X *= set.axisInverted[GP_AXIS_RX] ? -1 : 1;
			stickR.Y *= set.axisInverted[GP_AXIS_RY] ? -1 : 1;
			
			u8 stickD = GetAnalogDigital(stickR);

			buttons |= ((stickD & ANALOGD_XP) << N64_CRIGHT);
			buttons |= (((stickD & ANALOGD_XN) >> 1) << N64_CLEFT);
			buttons |= (((stickD & ANALOGD_YP) >> 2) << N64_CUP);
			buttons |= (((stickD & ANALOGD_YN) >> 3) << N64_CDOWN);
		}
	}
	else
		gamepadPlugged = false;

	u16 * outBig = (u16*)outBuffer;

	outBig[0] = buttons;
	outBig[1] = analog;
}

} // End namespace Input
