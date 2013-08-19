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
#include "Codes_IDs.h"
#include "General.h"

Controller2::Controller2(_Settings &config):
	Controller(config, 21)
{ 				
	memset(pressureButton, 0x00, sizeof(pressureButton));
	memcpy(pollMask, POLL_MASK_FULL, 6);	
}

void Controller2::ReadInputPressure(unsigned char *buffer)
{
	unsigned int mask = ((pollMask[2] << 16) | (pollMask[1] << 8) | (pollMask[0])) >> 6;	
	
	ReadInput(buffer);		

	if(mask)
	{
		//Right, left, up, down		
		pressureButton[0x00] = (mask & 0x01) && (buttons & 0x20) ? 0x00 : pressureButton[0x00] + 10;
		pressureButton[0x01] = (mask & 0x02) && (buttons & 0x80) ? 0x00 : pressureButton[0x01] + 10;
		pressureButton[0x02] = (mask & 0x03) && (buttons & 0x10) ? 0x00 : pressureButton[0x02] + 10;
		pressureButton[0x03] = (mask & 0x04) && (buttons & 0x40) ? 0x00 : pressureButton[0x03] + 10;
	
		//triangle, circle, cross, square
		pressureButton[0x04] = (mask & 0x05) && (buttons & 0x1000) ? 0x00 : pressureButton[0x04] + 10;
		pressureButton[0x05] = (mask & 0x06) && (buttons & 0x2000) ? 0x00 : pressureButton[0x05] + 10;
		pressureButton[0x06] = (mask & 0x07) && (buttons & 0x4000) ? 0x00 : pressureButton[0x06] + 10;
		pressureButton[0x07] = (mask & 0x08) && (buttons & 0x8000) ? 0x00 : pressureButton[0x07] + 10;

		//l1, r1, l2, r2
		pressureButton[0x08] = (mask & 0x09) && (buttons & 0x400) ? 0x00 : pressureButton[0x08] + 10;
		pressureButton[0x09] = (mask & 0x0A) && (buttons & 0x800) ? 0x00 : pressureButton[0x09] + 10;
		pressureButton[0x0A] = (mask & 0x0B) && (buttons & 0x100) ? 0x00 : triggerL;
		pressureButton[0x0B] = (mask & 0x0C) && (buttons & 0x200) ? 0x00 : triggerR;

		for(int i = 0; i < 0x0C; i++) 					
			pressureButton[i] = buffer[i+9] = pressureButton[i] > 0xFF ? 0xFF : pressureButton[i] & 0xFF;
	}	
	else
		memset(&buffer[9], 0x00, 12);
}

void Controller2::Cmd1(const unsigned char data)
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

	case 0x4F: if(bConfig) { // Enables/disables what do poll analog stuff, including digital.		
		memset(&dataBuffer[3], 0x00, 5);		
		dataBuffer[8] = 0x5A;}
		break;	

	default: Controller::Cmd1(data);
	}
}

void Controller2::Cmd4(const unsigned char data)
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
	
	default: Controller::Cmd4(data);
	}
}

void Controller2::Cmd8(const unsigned char data)
{		
	switch(data)
	{
	case 0x4F: if(bConfig) {	
		padID = ID_ANALOGP;		
		pollMask[0] = cmdBuffer[3];
		pollMask[1] = cmdBuffer[4];
		pollMask[2] = cmdBuffer[5]; }
		break;

	default: Controller::Cmd8(data);
	}
}