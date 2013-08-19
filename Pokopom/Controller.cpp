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

void Controller::Reset()
{			
	memset(dataBuffer, 0xFF, sizeof(dataBuffer));
	memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
	
	dataBuffer[2] = 0x5A;
	padID = settings.defaultAnalog ? (unsigned char)ID_ANALOG : (unsigned char)ID_DIGITAL;	

	buttons = buttonsStick = 0xFFFF;
	analogL = analogR = 0x7F7F;

	motorMapS = 0xFF;
	motorMapL = 0xFF;

	gamepadPlugged = false;
	bConfig = bModeLock = false;
}

Controller::Controller(_Settings &config, unsigned short bsize):
settings(config), sizeBuffer(bsize)
{  	
	cmdBuffer = new unsigned char[sizeBuffer];
	dataBuffer = new unsigned char[sizeBuffer];
	
	Reset();
}

unsigned char Controller::command(const unsigned int counter, const unsigned char data)
{						
	if(counter >= sizeBuffer) 
	{
		printf("Pokopom: Out of Bound Buffer ERROR! [%02d:%02d]\n", sizeBuffer, counter); 
		return 0x00;
	}
	
	cmdBuffer[counter] = data;

	if(!gamepadPlugged) 
	{				
		if(counter == 0) Recheck();
		if(!gamepadPlugged) return 0x00;
	}

	switch(counter)
	{
	case 0x00: Cmd0(); break;		
	
	case 0x01:
		dataBuffer[1] = bConfig? ID_CONFIG : padID;		
		if(cmdBuffer[0]==0x01) Cmd1(cmdBuffer[1]);		
		break;
		
	case 0x04: if(cmdBuffer[0]==0x01) Cmd4(cmdBuffer[1]); break;	
	case 0x08: if(cmdBuffer[0]==0x01) Cmd8(cmdBuffer[1]); break;
	}

	return dataBuffer[counter];
}

void Controller::ReadInput(unsigned char *buffer)
{	
	poll(); 
	
	if(padID == ID_DIGITAL)
	{
		buffer[3] = buttonsStick & 0xFF;
		buffer[4] = buttonsStick >> 8; 
		memset(&buffer[5], 0xFF, 4);	
	}
	else
	{
		buffer[3] = buttons & 0xFF;
		buffer[4] = buttons >> 8; 

		buffer[5] = analogR & 0xFF;
		buffer[6] = analogR >> 8; 
		buffer[7] = analogL & 0xFF;
		buffer[8] = analogL >> 8;
	}
}

void Controller::SetVibration()
{
	motorMapS = motorMapL = 0xFF;	
	
	for(unsigned char i = 3; i<9; i++)
	{
		if(cmdBuffer[i] == 0x00) motorMapS = i - 3;
		if(cmdBuffer[i] == 0x01) motorMapL = i - 3;
	}
}

void Controller::Cmd0()
{	
	static bool bPressed = false;

	if(!bModeLock) 
	{				
		if(!bPressed && !!(GetAsyncKeyState(VK_SCROLL) >> 1))
		{			
			if(padID == ID_DIGITAL)
			{
				padID = ID_ANALOG;
				//printf("Pokopom -> [%d] Switched to analog mode.\n", Settings.padPort);
			}
			else if(padID == ID_ANALOG)
			{
				padID = ID_DIGITAL;
				//printf("Pokopom -> [%d] Switched to digital mode.\n", Settings.padPort);
			}

			bPressed = true;
		} 
		else if (bPressed && !(GetAsyncKeyState(VK_SCROLL) >> 1)) 
		{
			bPressed = false; 
		}
	}
	
	if(gamepadPlugged && !bPressed)		
	{
		bool ledScrollLock = GetKeyState(VK_SCROLL)&0x1;

		if((padID != ID_DIGITAL && !ledScrollLock) || (padID == ID_DIGITAL && ledScrollLock))
		{
			keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY, 0 );
			keybd_event( VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );	
		}
	}
}

void Controller::Cmd1(const unsigned char data)
{						
	switch(data)
	{
	case 0x40: if(bConfig) memcpy(&dataBuffer[3], WTF, 6); // WTF ???!!!

	case 0x41: break;

	case 0x42: // Read_Data
		if(bConfig) memset(&dataBuffer[3], 0xFF, 6);
		else ReadInput(dataBuffer);
		break;

	case 0x43: // Config mode
		if(bConfig) memset(&dataBuffer[3], 0x00, 6);
		else ReadInput(dataBuffer);
		break;

	case 0x44: // Set mode and lock		
		memset(&dataBuffer[3], 0x00, 6);
		break;

	case 0x45: if(bConfig) { // Query model, 5th means LED status		
		memcpy(&dataBuffer[3], DUALSHOCK2_MODEL, 6); // Using DS2 Model for now
		dataBuffer[5] = padID == ID_DIGITAL? 0x00 : 0x01; } 
		break;

	case 0x46: 	
	case 0x47: 
		if(bConfig) dataBuffer[3] =0x00;
		break;

	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4B: break;

	case 0x4C: 
		if(bConfig) dataBuffer[3] = 0x00;
		break;

	case 0x4D: if(bConfig) { // Sets which bytes will be for vibration on cmd42	
		dataBuffer[3] = motorMapS;
		dataBuffer[4] = motorMapL;
		memset(&dataBuffer[5], 0xFF, 4);	}
		break;

	case 0x4E: break;
	case 0x4F: break;
	}
}

void Controller::Cmd4(const unsigned char data)
{			
	switch(data)
	{
	case 0x40: break;
	case 0x41: break;
	case 0x42: break;

	case 0x43: // Config mode
		bConfig = cmdBuffer[3] == 1;
		break;

	case 0x44: if(bConfig) {// Set mode and lock
		padID = cmdBuffer[3] == 0x01 ? (unsigned char)ID_ANALOG : (unsigned char)ID_DIGITAL;		
		bModeLock = cmdBuffer[4] == 0x03; } // Disgaea sends 0x01 here
		break;

	case 0x45: break;
	
	// Using DS2 ID code
	case 0x46: if(bConfig) {// Unknown constant part 1 and 2			
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], DUALSHOCK2_ID[0], 5);
		else memcpy(&dataBuffer[4], DUALSHOCK2_ID[1], 5);} 
		break;
	
	case 0x47: if(bConfig) {//Unknown constant part 3
		memcpy(&dataBuffer[4], DUALSHOCK2_ID[2], 5); } 
		break;

	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4B: break;

	case 0x4C: if(bConfig) {// Unknown constant part 4 and 5			
		if(cmdBuffer[3] == 0x00) memcpy(&dataBuffer[4], DUALSHOCK2_ID[3], 5);
		else memcpy(&dataBuffer[4], DUALSHOCK2_ID[4], 5);}
		break;

	case 0x4D: break;
	case 0x4E: break;
	case 0x4F: break;
	}
}

void Controller::Cmd8(const unsigned char data)
{			
	switch(data)
	{
	case 0x40: break;
	case 0x41: break;

	case 0x42: 
		vibration(	motorMapS == 0xFF? 0: cmdBuffer[motorMapS+3],
					motorMapL == 0xFF? 0: cmdBuffer[motorMapL+3]);
		break;

	case 0x43: break;
	case 0x44: break;
	case 0x45: break;	
	case 0x46: break;	
	case 0x47: break;
	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4B: break;
	case 0x4C: break;
	
	case 0x4D: 
		SetVibration();
		break;

	case 0x4E: break;
	case 0x4F: break;
	}
}