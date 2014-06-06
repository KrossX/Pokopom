/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */
#include <fstream>
#include <string>
#include <vector>

#include "General.h"
#include "FileIO.h"
#include "regini.h"

extern char settingsDirectory[1024];
extern u8 multitap;

extern bool bPriority;
extern void SetPriority();

#define PATH_LENGTH 1024
#define LINE_LENGTH 512

namespace FileIO
{
	std::string GetFilename()
	{
		const std::string pcsx2_directory(settingsDirectory);
		const std::string plugin_filename("padPokopom.ini");

		if (!pcsx2_directory.empty())
		{
			return pcsx2_directory + plugin_filename;
		}
		else
		{
#ifdef _WIN32
			const std::string plugin_directory("plugins/");
			return plugin_directory + plugin_filename;
#elif defined __linux__
			return plugin_filename;
#endif
		}
	}

	void INI_SaveSettings()
	{
		regini::regini_file inifile;
		inifile.open(GetFilename());

		inifile.write("General", "INIversion", INIversion);
		inifile.write("General", "ProcPriority", bPriority ? 1 : 0);
		inifile.write("General", "KeepAwake", bKeepAwake ? 1 : 0);
		inifile.write("General", "SwapPorts", SwapPortsEnabled ? 1 : 0);
		inifile.write("General", "Multitap", multitap);

		for(s32 port = 0; port < 4; port++)
		{
			std::string section = "Controller" + std::to_string(port);
			_Settings &set = settings[port];

			s32 AxisInverted = *(s32*)set.axisInverted;
			s32 AxisRemap = *(s32*)set.axisRemap;

			inifile.write(section, "AxisInverted", AxisInverted);
			inifile.write(section, "AxisRemap", AxisRemap);

			inifile.write(section, "TriggerDeadzone", set.triggerDeadzone);
			inifile.write(section, "Pressure", set.pressureRate);
			inifile.write(section, "Rumble", set.rumble);
			inifile.write(section, "SticksLocked", set.sticksLocked ? 1 : 0);

			inifile.write(section, "XInputPort", set.xinputPort);
			inifile.write(section, "Disabled", set.disabled ? 1 : 0);
			inifile.write(section, "DefautMode", set.defaultAnalog ? 1 : 0);
			inifile.write(section, "GreenAnalog", set.greenAnalog ? 1 : 0);
			inifile.write(section, "GuitarController", set.isGuitar ? 1 : 0);

			inifile.write(section, "SwapDCBumpers", set.SwapDCBumpers ? 1 : 0);
			inifile.write(section, "SwapSticks", set.SwapSticksEnabled ? 1 : 0);
			inifile.write(section, "SwapXO", set.SwapXO ? 1 : 0);

			// Left Stick Settings

			inifile.write(section, "LS_4wayDAC", set.stickL.b4wayDAC ? 1 : 0);
			inifile.write(section, "LS_EnableDAC", set.stickL.DACenabled ? 1 : 0);
			inifile.write(section, "LS_DACthreshold", set.stickL.DACthreshold);

			inifile.write(section, "LS_ExtentionThreshold", set.stickL.extThreshold);

			inifile.write(section, "LS_Linearity", set.stickL.linearity);
			inifile.write(section, "LS_AntiDeadzone", set.stickL.antiDeadzone);
			inifile.write(section, "LS_Deadzone", set.stickL.deadzone);

			// Right Stick Settings

			inifile.write(section, "RS_4wayDAC", set.stickR.b4wayDAC ? 1 : 0);
			inifile.write(section, "RS_EnableDAC", set.stickR.DACenabled ? 1 : 0);
			inifile.write(section, "RS_DACthreshold", set.stickR.DACthreshold);

			inifile.write(section, "RS_ExtentionThreshold", set.stickR.extThreshold);

			inifile.write(section, "RS_Linearity", set.stickR.linearity);
			inifile.write(section, "RS_AntiDeadzone", set.stickR.antiDeadzone);
			inifile.write(section, "RS_Deadzone", set.stickR.deadzone);
		}

		inifile.save();
	}

	void INI_LoadSettings()
	{
		settings[0].xinputPort = 0;
		settings[1].xinputPort = 1;
		settings[2].xinputPort = 2;
		settings[3].xinputPort = 3;

		regini::regini_file inifile;
		inifile.open(GetFilename());

		if (inifile.read("General", "INIversion", -1) != INIversion)
		{
			return;
		}

		bPriority = inifile.read("General", "ProcPriority", 0) == 1;
		SetPriority();
			
		SwapPortsEnabled = inifile.read("General", "SwapPorts", 0) == 1;
		bKeepAwake       = inifile.read("General", "KeepAwake", 0) == 1;
		multitap         = inifile.read("General", "Multitap", 0) & 0xF;
		multitap = multitap > 2 ? 0 : multitap;

		for (s32 port = 0; port < 4; port++)
		{
			std::string section = "Controller" + std::to_string(port);
			_Settings &set = settings[port];
			s32 result;

			result = inifile.read(section, "AxisInverted", -1);
			if (result != -1)  *(s32*)set.axisInverted = result;

			result = inifile.read(section, "AxisRemap", -1);
			if (result != -1) *(s32*)set.axisRemap = result;

			set.triggerDeadzone = inifile.read(section, "TriggerDeadzone", set.triggerDeadzone) & 0xFF;
			set.pressureRate    = inifile.read(section, "Pressure", set.pressureRate) & 0xFF;
			set.rumble          = inifile.read(section, "Rumble", set.rumble);
			set.xinputPort      = inifile.read(section, "XInputPort", set.xinputPort) & 0xF;

			set.disabled      = !!inifile.read(section, "Disabled", 0);
			set.sticksLocked  = !!inifile.read(section, "SticksLocked", 1);
			set.defaultAnalog = !!inifile.read(section, "DefautMode", 1);

			set.greenAnalog       = !!inifile.read(section, "GreenAnalog", 0);
			set.isGuitar          = !!inifile.read(section, "GuitarController", 0);
			set.SwapDCBumpers     = !!inifile.read(section, "SwapDCBumpers", 0);
			set.SwapSticksEnabled = !!inifile.read(section, "SwapSticks", 0);
			set.SwapXO            = !!inifile.read(section, "SwapXO", 0);

			// Left Stick Settings

			set.stickL.b4wayDAC   = !!inifile.read(section, "LS_4wayDAC", 0);
			set.stickL.DACenabled = !!inifile.read(section, "LS_EnableDAC", 0);

			set.stickL.DACthreshold = inifile.read(section, "LS_DACthreshold", set.stickL.DACthreshold);
			set.stickL.extThreshold = inifile.read(section, "LS_ExtentionThreshold", set.stickL.extThreshold);
			set.stickL.extMult = 46339.535798279205464084934426179 / set.stickL.extThreshold;

			set.stickL.linearity    = inifile.read(section, "LS_Linearity", set.stickL.linearity);
			set.stickL.antiDeadzone = inifile.read(section, "LS_AntiDeadzone", set.stickL.antiDeadzone);
			set.stickL.deadzone     = inifile.read(section, "LS_Deadzone", set.stickL.deadzone);

			// Right Stick Settings

			set.stickR.b4wayDAC   = !!inifile.read(section, "RS_4wayDAC", 0);
			set.stickR.DACenabled = !!inifile.read(section, "RS_EnableDAC", 0);

			set.stickR.DACthreshold = inifile.read(section, "RS_DACthreshold", set.stickR.DACthreshold);
			set.stickR.extThreshold = inifile.read(section, "RS_ExtentionThreshold", set.stickR.extThreshold);
			set.stickR.extMult = 46339.535798279205464084934426179 / set.stickR.extThreshold;

			set.stickR.linearity    = inifile.read(section, "RS_Linearity", set.stickR.linearity);
			set.stickR.antiDeadzone = inifile.read(section, "RS_AntiDeadzone", set.stickR.antiDeadzone);
			set.stickR.deadzone     = inifile.read(section, "RS_Deadzone", set.stickR.deadzone);
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

