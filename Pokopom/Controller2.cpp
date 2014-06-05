/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "Codes_IDs.h"
#include "Controller.h"
#include "Input.h"

DualShock2::DualShock2(_Settings &config) : DualShock(config, 21)
{
	memset(pressureButton, 0x00, sizeof(pressureButton));
	memcpy(pollMask, POLL_MASK_FULL, 6);
}

void DualShock2::ReadInputPressure(u8 *buffer)
{
	u8 pressure[12] = {0};
	u32 mask = ((pollMask[2] << 16) | (pollMask[1] << 8) | (pollMask[0])) >> 6;

	ReadInput(buffer);
	
	if(mask)
	{
		static bool hasPressure = true;
		hasPressure = hasPressure? Input::DualshockPressure(pressure, mask, settings, gamepadPlugged) : false;

		if(!hasPressure)
		{
			//Right, left, up, down
			pressureButton[0x00] = (mask & 0x01) && (buttons & 0x20) ? 0x00 : pressureButton[0x00] + settings.pressureRate;
			pressureButton[0x01] = (mask & 0x02) && (buttons & 0x80) ? 0x00 : pressureButton[0x01] + settings.pressureRate;
			pressureButton[0x02] = (mask & 0x04) && (buttons & 0x10) ? 0x00 : pressureButton[0x02] + settings.pressureRate;
			pressureButton[0x03] = (mask & 0x08) && (buttons & 0x40) ? 0x00 : pressureButton[0x03] + settings.pressureRate;

			//triangle, circle, cross, square
			pressureButton[0x04] = (mask & 0x10) && (buttons & 0x1000) ? 0x00 : pressureButton[0x04] + settings.pressureRate;
			pressureButton[0x05] = (mask & 0x20) && (buttons & 0x2000) ? 0x00 : pressureButton[0x05] + settings.pressureRate;
			pressureButton[0x06] = (mask & 0x40) && (buttons & 0x4000) ? 0x00 : pressureButton[0x06] + settings.pressureRate;
			pressureButton[0x07] = (mask & 0x80) && (buttons & 0x8000) ? 0x00 : pressureButton[0x07] + settings.pressureRate;

			//l1, r1, l2, r2
			pressureButton[0x08] = (mask & 0x100) && (buttons & 0x400) ? 0x00 : pressureButton[0x08] + settings.pressureRate;
			pressureButton[0x09] = (mask & 0x200) && (buttons & 0x800) ? 0x00 : pressureButton[0x09] + settings.pressureRate;
			pressureButton[0x0A] = (mask & 0x400) && (buttons & 0x100) ? 0x00 : triggerL;
			pressureButton[0x0B] = (mask & 0x800) && (buttons & 0x200) ? 0x00 : triggerR;

			for(s32 i = 0; i < 0x0C; i++)
				pressureButton[i] = buffer[i+9] = pressureButton[i] > 0xFF ? 0xFF : pressureButton[i] & 0xFF;
		}
		else
		{
			for(s32 i = 0; i < 0x0C; i++)
				pressureButton[i] = buffer[i+9] = pressure[i];
		}
	}
	else
		memset(&buffer[9], 0x00, 12);
}

void DualShock2::Cmd1(const u8 data)
{
	switch(data)
	{
	case 0x41: if(bConfig) {
		if(padID == ID_DIGITAL) memset(&dataBuffer[3], 0x00, 6);
		else memcpy(&dataBuffer[3], pollMask, 6);}
		break;

	case 0x42: // Polls common input and handles vibration
		if(bConfig) ReadInput(dataBuffer);
		else ReadInputPressure(dataBuffer);
		break;

	case 0x43: // Toggle config mode, poll input and pressures.
		if(bConfig) memset(&dataBuffer[3], 0x00, 6);
		else ReadInputPressure(dataBuffer);
		break;

	case 0x45: if(bConfig) { // Query model, 5th means LED status
		memcpy(&dataBuffer[3], DUALSHOCK2_MODEL, 6);
		dataBuffer[5] = padID == ID_DIGITAL? 0x00 : 0x01; }
		break;

	case 0x4F: if(bConfig) { // Poll mask
		memset(&dataBuffer[3], 0x00, 5);
		dataBuffer[8] = 0x5A;}
		break;

	default: DualShock::Cmd1(data);
	}
}

void DualShock2::Cmd4(const u8 data)
{
	switch(data)
	{
	case 0x46: if(bConfig) {// Unknown constant part 1 and 2
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], DUALSHOCK2_ID[0], 5);
		else memcpy(&dataBuffer[4], DUALSHOCK2_ID[1], 5);}
		break;

	case 0x47: if(bConfig) {//Unknown constant part 3
		memcpy(&dataBuffer[4], DUALSHOCK2_ID[2], 5); }
		break;

	case 0x4C: if(bConfig) {// Unknown constant part 4 and 5
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], DUALSHOCK2_ID[3], 5);
		else memcpy(&dataBuffer[4], DUALSHOCK2_ID[4], 5);}
		break;

	default: DualShock::Cmd4(data);
	}
}

void DualShock2::Cmd8(const u8 data)
{
	switch(data)
	{
	case 0x4F: if(bConfig) {
		padID = ID_ANALOG_REDP;
		pollMask[0] = cmdBuffer[3];
		pollMask[1] = cmdBuffer[4];
		pollMask[2] = cmdBuffer[5]; }
		break;

	default: DualShock::Cmd8(data);
	}
}

void DualShock2::SaveState(PlayStationDeviceState &state)
{
	DualShock::SaveState(state);

	memcpy(state.pollMask, pollMask, 6);
	memcpy(state.pressureButton, pressureButton, sizeof(pressureButton));
}

void DualShock2::LoadState(PlayStationDeviceState state)
{
	DualShock::LoadState(state);

	memcpy(pollMask, state.pollMask, 6);
	memcpy(pressureButton, state.pressureButton, sizeof(pressureButton));
}