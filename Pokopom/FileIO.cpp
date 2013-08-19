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
#include "FileIO.h"

#include <fstream>
#include <stdlib.h>

#ifdef _WIN32
extern HINSTANCE hInstance;
#endif

extern char settingsDirectory[1024];
extern u8 multitap;

extern bool bPriority;
extern void SetPriority();

#define PATH_LENGTH 1024
#define LINE_LENGTH 512

namespace FileIO
{
	void GetFilename(char *filename)
	{

		if(settingsDirectory[0])
			sprintf(filename, "%spadPokopom.ini", settingsDirectory);

#ifdef _WIN32
		else
		{
			s32 length = GetModuleFileNameA(hInstance, filename, 1024);

			if(length)
			{
				filename[length -  3] = L'i';
				filename[length -  2] = L'n';
				filename[length -  1] = L'i';
			}
			else
				sprintf(filename, "padPokopom.ini");
		}
#elif defined __linux__
		else
			sprintf(filename, "padPokopom.ini");
#endif

	}

	bool SaveEntry(const char *sec, s32 sectionNumber, const char *key, s32 value, FILE *iniFile)
	{
		char newsec[LINE_LENGTH+1] = {0};
		char line[LINE_LENGTH+1] = {0};

		if(sectionNumber >= 0)
			sprintf(newsec, "[%s%d]", sec, sectionNumber);
		else
			sprintf(newsec, "[%s]", sec);

		static char section[LINE_LENGTH+1] = {0};

		if(strcmp(newsec, section) != 0)
		{
			memcpy(section, newsec, LINE_LENGTH);
			sprintf(line, "%s\n", newsec);
			fputs(line, iniFile);
		}

		sprintf(line, "%s=%d\n", key, value);
		fputs(line, iniFile);

		return true;
	}

	s32 ReadEntry(const char *sec, s32 sectionNumber, const char *key, FILE *iniFile)
	{
		char section[LINE_LENGTH+1] = {0};
		char line[LINE_LENGTH+1] = {0};

		s32 value = -1, length;
		bool sectionFound = false;

		fseek(iniFile, 0, SEEK_SET);

		if(sectionNumber >= 0)
			sprintf(section, "[%s%d]", sec, sectionNumber);
		else
			sprintf(section, "[%s]", sec);

		while(fgets(line, LINE_LENGTH, iniFile) != NULL)
		{
			if(memcmp(line, section, strlen(section)) == 0)
			{
				sectionFound = true;
			}
			else if(sectionFound)
			{
				length = strlen(key);

				if(memcmp(line, key, length) == 0)
				{
					if(line[length] == '=')
					{
						value = atoi(&line[strlen(key)+1]);
						return value;
					}
				}
			}
		}

		return value;
	}

	void INI_SaveSettings()
	{
		char filename[PATH_LENGTH] = {0};
		FILE* iniFile = NULL;
		bool ready = false;

		GetFilename(filename);

		iniFile = fopen(filename, "w");
		ready = iniFile == NULL? false : true;

		if(ready)
		{	
			SaveEntry("General", -1, "ProcPriority", bPriority? 1 : 0, iniFile);
			SaveEntry("General", -1, "KeepAwake", bKeepAwake? 1 : 0, iniFile);
			SaveEntry("General", -1, "INIversion", INIversion, iniFile);
			SaveEntry("General", -1, "Multitap", multitap, iniFile);
			SaveEntry("General", -1, "SwapPorts", SwapPortsEnabled, iniFile); 

			for(s32 port = 0; port < 4; port++)
			{
				s32 AxisInverted =	((settings[port].axisInverted[GP_AXIS_LX]?1:0) << 12) | ((settings[port].axisInverted[GP_AXIS_LY]?1:0) << 8) |
												((settings[port].axisInverted[GP_AXIS_RX]?1:0) << 4) | (settings[port].axisInverted[GP_AXIS_RY]?1:0);

				s32 AxisRemap =	(settings[port].axisRemap[GP_AXIS_LX] << 12) | (settings[port].axisRemap[GP_AXIS_LY] << 8) |
											(settings[port].axisRemap[GP_AXIS_RX] << 4) | (settings[port].axisRemap[GP_AXIS_RY]);

				SaveEntry("Controller", port, "AxisInverted", AxisInverted, iniFile);
				SaveEntry("Controller", port, "AxisRemap", AxisRemap, iniFile);

				SaveEntry("Controller", port, "Pressure", settings[port].pressureRate, iniFile);
				SaveEntry("Controller", port, "Rumble", (s32)(settings[port].rumble * 100.1), iniFile);
				SaveEntry("Controller", port, "SticksLocked", settings[port].sticksLocked ? 1 : 0, iniFile);

				SaveEntry("Controller", port, "Linearity", (s32)(settings[port].stickL.linearity * 10.1)+40, iniFile);
				SaveEntry("Controller", port, "AntiDeadzone", (s32)(settings[port].stickL.antiDeadzone * 100.1), iniFile);
				SaveEntry("Controller", port, "Deadzone", (s32)(settings[port].stickL.deadzone * 100.1), iniFile);
				
				SaveEntry("Controller", port, "Linearity2", (s32)(settings[port].stickR.linearity * 10.1)+40, iniFile);
				SaveEntry("Controller", port, "AntiDeadzone2", (s32)(settings[port].stickR.antiDeadzone * 100.1), iniFile);
				SaveEntry("Controller", port, "Deadzone2", (s32)(settings[port].stickR.deadzone * 100.1), iniFile);

				SaveEntry("Controller", port, "ExtentionThreshold", (s32)(settings[port].extThreshold), iniFile);

				SaveEntry("Controller", port, "XInputPort", settings[port].xinputPort, iniFile);
				SaveEntry("Controller", port, "Disabled", settings[port].disabled ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "DefautMode", settings[port].defaultAnalog ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "GreenAnalog", settings[port].greenAnalog ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "GuitarController", settings[port].isGuitar ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "4wayStick", settings[port].b4wayStick ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "SwapSticks", settings[port].SwapSticksEnabled ? 1 : 0, iniFile);
				SaveEntry("Controller", port, "SwapXO", settings[port].SwapXO ? 1 : 0, iniFile);

			}

			fclose(iniFile);
		}
	}

	void INI_LoadSettings()
	{
		settings[0].xinputPort = 0;
		settings[1].xinputPort = 1;
		settings[2].xinputPort = 2;
		settings[3].xinputPort = 3;

		char filename[PATH_LENGTH] = {0};
		FILE* iniFile = NULL;
		bool ready = false;

		GetFilename(filename);

		iniFile = fopen(filename, "r");
		ready = iniFile == NULL? false : true;

		if(ready)
		{
			if(ReadEntry("General", -1, "INIversion", iniFile) != INIversion)
			{
				fclose(iniFile);
				return;
			}

			bPriority = ReadEntry("General", -1, "ProcPriority", iniFile) == 1;
				SetPriority();
			
			SwapPortsEnabled = ReadEntry("General", -1, "SwapPorts", iniFile) == 1;
			bKeepAwake = ReadEntry("General", -1, "KeepAwake", iniFile) == 1;
			multitap = ReadEntry("General", -1, "Multitap", iniFile) & 0xFF;
			multitap = multitap > 2 ? 0 : multitap;

			for(s32 port = 0; port < 4; port++)
			{
				s32 result;

				result = ReadEntry("Controller", port, "AxisInverted", iniFile);
				if(result != -1)
				{
					settings[port].axisInverted[GP_AXIS_RY] = (result & 0xF) ? true : false;
					settings[port].axisInverted[GP_AXIS_RX] = ((result >> 4) & 0xF ) ? true : false;
					settings[port].axisInverted[GP_AXIS_LY] =  ((result >> 8) & 0xF ) ? true : false;
					settings[port].axisInverted[GP_AXIS_LX] = ((result >> 12) & 0xF ) ? true : false;
				}

				result = ReadEntry("Controller", port, "AxisRemap", iniFile);
				if(result != -1)
				{
					settings[port].axisRemap[GP_AXIS_RY] = result & 0xF;
					settings[port].axisRemap[GP_AXIS_RX] = (result >> 4) & 0xF;
					settings[port].axisRemap[GP_AXIS_LY] =  (result >> 8) & 0xF;
					settings[port].axisRemap[GP_AXIS_LX] = (result >> 12) & 0xF;
				}

				result = ReadEntry("Controller", port, "ExtentionThreshold",  iniFile);
				if(result != -1)
				{
					settings[port].extThreshold = result;
					settings[port].extMult = 46339.535798279205464084934426179 / result;
				}

				result = ReadEntry("Controller", port, "Pressure",  iniFile);
				if(result != -1) settings[port].pressureRate = result & 0xFF;

				result = ReadEntry("Controller", port, "Linearity",  iniFile);
				if(result != -1) settings[port].stickL.linearity = (result-40) / 10.0;

				result = ReadEntry("Controller", port, "AntiDeadzone",  iniFile);
				if(result != -1) settings[port].stickL.antiDeadzone = result / 100.0f;

				result = ReadEntry("Controller", port, "Deadzone",  iniFile);
				if(result != -1) settings[port].stickL.deadzone = result / 100.0f;

				result = ReadEntry("Controller", port, "Linearity2",  iniFile);
				if(result != -1) settings[port].stickR.linearity = (result-40) / 10.0;

				result = ReadEntry("Controller", port, "AntiDeadzone2",  iniFile);
				if(result != -1) settings[port].stickR.antiDeadzone = result / 100.0f;

				result = ReadEntry("Controller", port, "Deadzone2",  iniFile);
				if(result != -1) settings[port].stickR.deadzone = result / 100.0f;

				result = ReadEntry("Controller", port, "Rumble", iniFile);
				if(result != -1) settings[port].rumble = result / 100.0f;

				result = ReadEntry("Controller", port, "XInputPort", iniFile);
				if(result != -1) settings[port].xinputPort = result & 0xF;

				result = ReadEntry("Controller", port, "Disabled", iniFile);
				if(result != -1) settings[port].disabled = result == 1;

				result = ReadEntry("Controller", port, "SticksLocked", iniFile);
				if(result != -1) settings[port].sticksLocked = result == 1;

				result = ReadEntry("Controller", port, "DefautMode", iniFile);
				if(result != -1) settings[port].defaultAnalog = result == 1;

				result = ReadEntry("Controller", port, "GreenAnalog", iniFile);
				if(result != -1) settings[port].greenAnalog = result == 1;

				result = ReadEntry("Controller", port, "GuitarController", iniFile);
				if(result != -1) settings[port].isGuitar = result == 1;

				result = ReadEntry("Controller", port, "4wayStick", iniFile);
				if(result != -1) settings[port].b4wayStick = result == 1;

				result = ReadEntry("Controller", port, "SwapSticks", iniFile);
				if(result != -1) settings[port].SwapSticksEnabled = result == 1;

				result = ReadEntry("Controller", port, "SwapXO", iniFile);
				if(result != -1) settings[port].SwapXO = result == 1;
			}

			fclose(iniFile);
		}
	}

	bool FASTCALL LoadMempak(u8 *data, u8 port)
	{
#ifdef  _WIN32
		char filename[256];

		sprintf(filename, "MemPaks\\Pokopom%d.mempak", port+1);

		std::fstream file;
		file.open(filename, std::ios::binary | std::ios::in);

		if(!file.is_open()) return false;

		file.seekg(EOF, std::ios_base::end);
		s64 size = file.tellg();

		if(size != 32767) { file.close(); return false; }

		file.seekg(0);
		file.read((char*)data, 0x8000);
		file.close();

		return true;
#else
		return false;
#endif
	}

	void FASTCALL SaveMempak(u8* data, u8 port)
	{
#ifdef _WIN32
		CreateDirectoryA("MemPaks", NULL);

		char filename[1024] = {0};
		sprintf(filename, "MemPaks\\Pokopom%d.mempak", port+1);

		std::fstream file;
		file.open(filename, std::ios::binary | std::ios::out);

		if(!file.is_open()) return;

		file.write((char*)data, 0x8000);
		file.close();
#endif
	}



	bool FASTCALL LoadEEPROM(u8 *data)
	{
#ifdef _WIN32
		FILE *file = NULL;
		file = fopen("Pokopom.eeprom", "r");

		if(file != NULL)
		{
			fread(data, 1, 0x80, file);
			fclose(file);
			return true;
		}
		else
			return false;
#else
		return false;
#endif
	}

	void FASTCALL SaveEEPROM(u8 *data)
	{
#ifdef _WIN32
		FILE *file = NULL;
		file = fopen("Pokopom.eeprom", "w");

		if(file != NULL)
		{
			fwrite(data, 1, 0x80, file);
			fclose(file);
		}
#endif
	}

} // End namespace FileIO

