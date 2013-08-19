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

#include <Windows.h>
#include <stdio.h>

#include "Settings.h"

extern HINSTANCE hInstance;
extern _Settings settings[4];
extern wchar_t  settingsDirectory[1024];
extern bool bKeepAwake; 
extern int INIversion;

bool SaveEntry(wchar_t * section, int sectionNumber, wchar_t * key, int value, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);

	wchar_t valuestring[512] = {0};
	swprintf(valuestring, 512, L"%d", value);

	return WritePrivateProfileString(controller,  key, valuestring, filename) ? true : false;
}

int ReadEntry(wchar_t * section, int sectionNumber, wchar_t * key, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);
	
	int returnInteger = -1;
	wchar_t returnvalue[512] = {0};
	int nSize = GetPrivateProfileString(controller, key, L"-1", returnvalue, 512, filename);

	 if(nSize < 256) returnInteger = _wtoi(returnvalue);	 

	return returnInteger;
}

void INI_SaveSettings()
{
	wchar_t filename[1024] = {0};
	bool ready = false;
	
	if(settingsDirectory[0])
	{
		swprintf(filename, 1024, L"%spadPokopom.ini", settingsDirectory);
		ready = true;
	}
	else
	{		
		int length = GetModuleFileName(hInstance, filename, 1024);

		if(length)
		{
			filename[length -  3] = L'i';
			filename[length -  2] = L'n';
			filename[length -  1] = L'f';		
				
			ready = true;
		}
	}		

	if(ready)
	{		
		SaveEntry(L"General", -1, L"KeepAwake", bKeepAwake?1:0, filename);
		SaveEntry(L"General", -1, L"INIversion", INIversion, filename);
		
		for(int port = 0; port < 4; port++)
		{
			int AxisInverted =	((settings[port].axisInverted[GP_AXIS_LX]?1:0) << 12) | ((settings[port].axisInverted[GP_AXIS_LY]?1:0) << 8) |
											((settings[port].axisInverted[GP_AXIS_RX]?1:0) << 4) | (settings[port].axisInverted[GP_AXIS_RY]?1:0);
			
			int AxisRemap =	(settings[port].axisRemap[GP_AXIS_LX] << 12) | (settings[port].axisRemap[GP_AXIS_LY] << 8) |
										(settings[port].axisRemap[GP_AXIS_RX] << 4) | (settings[port].axisRemap[GP_AXIS_RY]);
			
			SaveEntry(L"Controller", port, L"AxisInverted", AxisInverted, filename);						
			SaveEntry(L"Controller", port, L"AxisRemap", AxisRemap, filename);

			SaveEntry(L"Controller", port, L"Pressure", settings[port].pressureRate, filename);
			SaveEntry(L"Controller", port, L"Linearity", (int)(settings[port].linearity * 10)+40, filename);

			SaveEntry(L"Controller", port, L"AntiDeadzone", (int)(settings[port].antiDeadzone * 100), filename);
			SaveEntry(L"Controller", port, L"Deadzone", (int)(settings[port].deadzone * 100), filename);
			SaveEntry(L"Controller", port, L"Rumble", (int)(settings[port].rumble * 100), filename);

			SaveEntry(L"Controller", port, L"ExtentionThreshold", (int)(settings[port].extThreshold), filename);

			SaveEntry(L"Controller", port, L"XInputPort", settings[port].xinputPort, filename);
			SaveEntry(L"Controller", port, L"DefautMode", settings[port].defaultAnalog ? 1 : 0, filename);
			SaveEntry(L"Controller", port, L"GuitarController", settings[port].isGuitar ? 1 : 0, filename);
						
		}		
	}
	
}

void INI_LoadSettings()
{
	settings[0].xinputPort = 0;
	settings[1].xinputPort = 1;
	settings[2].xinputPort = 2;
	settings[3].xinputPort = 3;
	
	wchar_t filename[1024] = {0};
	bool ready = false;
	
	if(settingsDirectory[0])
	{
		swprintf(filename, 1024, L"%spadPokopom.ini", settingsDirectory);
		ready = true;
	}
	else
	{		
		int length = GetModuleFileName(hInstance, filename, 1024);

		if(length)
		{
			filename[length -  3] = L'i';
			filename[length -  2] = L'n';
			filename[length -  1] = L'f';		
				
			ready = true;
		}
	}	

	if(ready)
	{
		bKeepAwake = ReadEntry(L"General", -1, L"KeepAwake", filename) == 1 ? true : false;
		if( ReadEntry(L"General", -1, L"INIversion", filename) != INIversion ) return;
		
		for(int port = 0; port < 4; port++)
		{
			int result;
						
			result = ReadEntry(L"Controller", port, L"AxisInverted", filename);
			if(result != -1)
			{
				settings[port].axisInverted[GP_AXIS_RY] = (result & 0xF) ? true : false;
				settings[port].axisInverted[GP_AXIS_RX] = ((result >> 4) & 0xF ) ? true : false;
				settings[port].axisInverted[GP_AXIS_LY] =  ((result >> 8) & 0xF ) ? true : false;
				settings[port].axisInverted[GP_AXIS_LX] = ((result >> 12) & 0xF ) ? true : false;
			}			

			result = ReadEntry(L"Controller", port, L"AxisRemap", filename);
			if(result != -1)
			{
				settings[port].axisRemap[GP_AXIS_RY] = result & 0xF;
				settings[port].axisRemap[GP_AXIS_RX] = (result >> 4) & 0xF;
				settings[port].axisRemap[GP_AXIS_LY] =  (result >> 8) & 0xF;
				settings[port].axisRemap[GP_AXIS_LX] = (result >> 12) & 0xF;
			}

			result = ReadEntry(L"Controller", port, L"ExtentionThreshold",  filename);
			if(result != -1)
			{
				settings[port].extThreshold = result;
				settings[port].extMult = 46339.535798279205464084934426179 / result;
			}

			result = ReadEntry(L"Controller", port, L"Pressure",  filename);
			if(result != -1) settings[port].pressureRate = result & 0xFF;

			result = ReadEntry(L"Controller", port, L"Linearity",  filename);
			if(result != -1) settings[port].linearity = (result-40) / 10.0;

			result = ReadEntry(L"Controller", port, L"AntiDeadzone",  filename);
			if(result != -1) settings[port].antiDeadzone = result / 100.0f;

			result = ReadEntry(L"Controller", port, L"Deadzone",  filename);
			if(result != -1) settings[port].deadzone = result / 100.0f;

			result = ReadEntry(L"Controller", port, L"Rumble", filename);
			if(result != -1) settings[port].rumble = result / 100.0f;

			result = ReadEntry(L"Controller", port, L"XInputPort", filename);
			if(result != -1) settings[port].xinputPort = result;

			result = ReadEntry(L"Controller", port, L"DefautMode", filename);
			if(result != -1) settings[port].defaultAnalog = result == 1? true : false;
			
			result = ReadEntry(L"Controller", port, L"GuitarController", filename);
			if(result != -1) settings[port].isGuitar = result == 1? true : false;
			
		}
	}
}