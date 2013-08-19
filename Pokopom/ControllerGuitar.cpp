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

ControllerGuitar::ControllerGuitar(_Settings &config):
	Controller2(config) {}
	
void ControllerGuitar::ReadInputGuitar(const bool bConfig, u8 *buffer)
{
	if(bConfig) ReadInput(buffer);
	else ReadInputPressure(buffer);

	buffer[3] &= 0x7F; // Switch off LEFT DPAD for some reason.
	// 9 - A // A - F // E - 9 // F - E
	u8 bitswap[4] = {0};

	bitswap[0] = (buffer[4] & 0x2)? 0: 1 << 2; // 9 - A
	bitswap[1] = (buffer[4] & 0x4)? 0: 1 << 7; // A - F
	bitswap[2] = (buffer[4] & 0x40)? 0: 1 << 1; // E - 9 
	bitswap[3] = (buffer[4] & 0x80)? 0: 1 << 6; // F - E
	
	buffer[4] |= 0xC6;
	buffer[4] &= ~(bitswap[0] + bitswap[1] + bitswap[2] + bitswap[3]);		
}


void ControllerGuitar::Cmd1(u8 data)
{									
	switch(data)
	{	
	case 0x42: // Polls common input and handles vibration
		if(bConfig) ReadInputGuitar(true, dataBuffer);
		else ReadInputGuitar(false, dataBuffer);
		break;

	case 0x43: // Toggle config mode, poll input and pressures.		
		if(bConfig) memset(&dataBuffer[3], 0x00, 6);
		else ReadInputGuitar(false, dataBuffer);
		break;

	case 0x45: if(bConfig) { // Query model, 5th means LED status		
		memcpy(&dataBuffer[3], GUITARHERO_MODEL, 6);			
		dataBuffer[5] = padID == ID_ANALOG? 0x01 : 0x00; } 
		break;
	
	default: Controller2::Cmd1(data);
	}
}

void ControllerGuitar::Cmd4(u8 data)
{	
	switch(data)
	{
	case 0x46: if(bConfig) {// Unknown constant part 1 and 2			
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], GUITARHERO_ID[0], 5);
		else memcpy(&dataBuffer[4], GUITARHERO_ID[1], 5);} 
		break;
	
	case 0x47: if(bConfig) {//Unknown constant part 3
		memcpy(&dataBuffer[4], GUITARHERO_ID[2], 5); } 
		break;

	case 0x4C: if(bConfig) {// Unknown constant part 4 and 5			
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], GUITARHERO_ID[3], 5);
		else memcpy(&dataBuffer[4], GUITARHERO_ID[4], 5);}
		break;

	default: Controller2::Cmd4(data);
	}
}

