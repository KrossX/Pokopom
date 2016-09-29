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

extern bool disableLED;
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

		inifile.write("General", "INIversion", std::to_string(INIversion));
		inifile.write("General", "ProcPriority", std::to_string(bPriority ? 1 : 0));
		inifile.write("General", "KeepAwake", std::to_string(bKeepAwake ? 1 : 0));
		inifile.write("General", "SwapPorts", std::to_string(SwapPortsEnabled ? 1 : 0));
		inifile.write("General", "Multitap", std::to_string(multitap));
		inifile.write("General", "DisableLED", std::to_string(disableLED ? 1 : 0));

		for(s32 port = 0; port < 4; port++)
		{
			std::string section = "Controller" + std::to_string(port);
			_Settings &set = settings[port];

			u32 AxisInverted, AxisRemap;
			
			{
				u8 *ai = set.axisInverted;
				u8 *ar = set.axisRemap;
				
				AxisInverted = ai[0] | (ai[1] << 8) | (ai[2] << 16) | (ai[3] << 24);
				AxisRemap = ar[0] | (ar[1] << 8) | (ar[2] << 16) | (ar[3] << 24);
			}
			
			inifile.write(section, "AxisInverted", std::to_string(AxisInverted));
			inifile.write(section, "AxisRemap", std::to_string(AxisRemap));

			inifile.write(section, "TriggerDeadzone", std::to_string(set.triggerDeadzone));
			inifile.write(section, "Pressure", std::to_string(set.pressureRate));
			inifile.write(section, "Rumble", std::to_string(set.rumble));
			inifile.write(section, "SticksLocked", std::to_string(set.sticksLocked ? 1 : 0));

			inifile.write(section, "XInputPort", std::to_string(set.xinputPort));
			inifile.write(section, "Disabled", std::to_string(set.disabled ? 1 : 0));
			inifile.write(section, "DefautMode", std::to_string(set.defaultAnalog ? 1 : 0));
			inifile.write(section, "GreenAnalog", std::to_string(set.greenAnalog ? 1 : 0));
			inifile.write(section, "GuitarController", std::to_string(set.isGuitar ? 1 : 0));

			inifile.write(section, "SwapDCBumpers", std::to_string(set.SwapDCBumpers ? 1 : 0));
			inifile.write(section, "SwapSticks", std::to_string(set.SwapSticksEnabled ? 1 : 0));
			inifile.write(section, "SwapXO", std::to_string(set.SwapXO ? 1 : 0));

			// Left Stick Settings

			inifile.write(section, "LS_4wayDAC", std::to_string(set.stickL.b4wayDAC ? 1 : 0));
			inifile.write(section, "LS_EnableDAC", std::to_string(set.stickL.DACenabled ? 1 : 0));
			inifile.write(section, "LS_DACthreshold", std::to_string(set.stickL.DACthreshold));

			inifile.write(section, "LS_ExtentionThreshold", std::to_string(set.stickL.extThreshold));

			inifile.write(section, "LS_Linearity", std::to_string(set.stickL.linearity));
			inifile.write(section, "LS_AntiDeadzone", std::to_string(set.stickL.antiDeadzone));
			inifile.write(section, "LS_Deadzone", std::to_string(set.stickL.deadzone));

			// Right Stick Settings

			inifile.write(section, "RS_4wayDAC", std::to_string(set.stickR.b4wayDAC ? 1 : 0));
			inifile.write(section, "RS_EnableDAC", std::to_string(set.stickR.DACenabled ? 1 : 0));
			inifile.write(section, "RS_DACthreshold", std::to_string(set.stickR.DACthreshold));

			inifile.write(section, "RS_ExtentionThreshold", std::to_string(set.stickR.extThreshold));

			inifile.write(section, "RS_Linearity", std::to_string(set.stickR.linearity));
			inifile.write(section, "RS_AntiDeadzone", std::to_string(set.stickR.antiDeadzone));
			inifile.write(section, "RS_Deadzone", std::to_string(set.stickR.deadzone));
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

		if (inifile.readi("General", "INIversion", -1) != INIversion)
		{
			return;
		}

		bPriority = inifile.readi("General", "ProcPriority", 0) == 1;
		SetPriority();

		disableLED       = inifile.readi("General", "DisableLED", 0) == 1;
		SwapPortsEnabled = inifile.readi("General", "SwapPorts", 0) == 1;
		bKeepAwake       = inifile.readi("General", "KeepAwake", 0) == 1;
		multitap         = inifile.readi("General", "Multitap", 0) & 0xF;
		multitap = multitap > 2 ? 0 : multitap;

		for (s32 port = 0; port < 4; port++)
		{
			std::string section = "Controller" + std::to_string(port);
			_Settings &set = settings[port];
			s32 result;
			
			result = inifile.readi(section, "AxisInverted", -1);
			if (result != -1)
			{
				u8 *ai = set.axisInverted;
				u32 res = result;
				
				ai[0] = res & 0xFF;
				ai[1] = (res >> 8 ) & 0xFF;
				ai[2] = (res >> 16) & 0xFF;
				ai[3] = (res >> 24) & 0xFF;
			}

			result = inifile.readi(section, "AxisRemap", -1);
			if (result != -1)
			{
				u8 *ar = set.axisRemap;
				u32 res = result;
				
				ar[0] = res & 0xFF;
				ar[1] = (res >> 8 ) & 0xFF;
				ar[2] = (res >> 16) & 0xFF;
				ar[3] = (res >> 24) & 0xFF;
			}

			set.triggerDeadzone = inifile.readi(section, "TriggerDeadzone", set.triggerDeadzone) & 0xFF;
			set.pressureRate    = inifile.readi(section, "Pressure", set.pressureRate) & 0xFF;
			set.rumble          = inifile.readf(section, "Rumble", set.rumble);
			set.xinputPort      = inifile.readi(section, "XInputPort", set.xinputPort) & 0xF;

			DebugPrint("[%d] -> XInputPort [%d]", port, set.xinputPort);

			set.disabled      = !!inifile.readi(section, "Disabled", 0);
			set.sticksLocked  = !!inifile.readi(section, "SticksLocked", 1);
			set.defaultAnalog = !!inifile.readi(section, "DefautMode", 1);

			set.greenAnalog       = !!inifile.readi(section, "GreenAnalog", 0);
			set.isGuitar          = !!inifile.readi(section, "GuitarController", 0);
			set.SwapDCBumpers     = !!inifile.readi(section, "SwapDCBumpers", 0);
			set.SwapSticksEnabled = !!inifile.readi(section, "SwapSticks", 0);
			set.SwapXO            = !!inifile.readi(section, "SwapXO", 0);

			// Left Stick Settings

			set.stickL.b4wayDAC   = !!inifile.readi(section, "LS_4wayDAC", 0);
			set.stickL.DACenabled = !!inifile.readi(section, "LS_EnableDAC", 0);

			set.stickL.DACthreshold = inifile.readd(section, "LS_DACthreshold", set.stickL.DACthreshold);
			set.stickL.extThreshold = inifile.readd(section, "LS_ExtentionThreshold", set.stickL.extThreshold);
			set.stickL.extMult = 46339.535798279205464084934426179 / set.stickL.extThreshold;

			set.stickL.linearity    = inifile.readd(section, "LS_Linearity", set.stickL.linearity);
			set.stickL.antiDeadzone = inifile.readf(section, "LS_AntiDeadzone", set.stickL.antiDeadzone);
			set.stickL.deadzone     = inifile.readf(section, "LS_Deadzone", set.stickL.deadzone);

			// Right Stick Settings

			set.stickR.b4wayDAC   = !!inifile.readi(section, "RS_4wayDAC", 0);
			set.stickR.DACenabled = !!inifile.readi(section, "RS_EnableDAC", 0);

			set.stickR.DACthreshold = inifile.readd(section, "RS_DACthreshold", set.stickR.DACthreshold);
			set.stickR.extThreshold = inifile.readd(section, "RS_ExtentionThreshold", set.stickR.extThreshold);
			set.stickR.extMult = 46339.535798279205464084934426179 / set.stickR.extThreshold;

			set.stickR.linearity    = inifile.readd(section, "RS_Linearity", set.stickR.linearity);
			set.stickR.antiDeadzone = inifile.readf(section, "RS_AntiDeadzone", set.stickR.antiDeadzone);
			set.stickR.deadzone     = inifile.readf(section, "RS_Deadzone", set.stickR.deadzone);
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

