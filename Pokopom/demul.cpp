/* Copyright (c) 2015 KrossX <krossx@live.com>
* License: http://www.opensource.org/licenses/mit-license.html  MIT License
*/

#include "demul.h"
#include "input.h"
#include "FileIO.h"
#include "ConfigDialog.h"

#include <string>
#include <iostream>
#include <fstream>

const wchar_t demul_name[] = L"Pokopom Xinput Pad Plugin";

std::ofstream logfile;

void printfn(std::string fname, int a = 0, int b = 0, int c = 0, int d = 0)
{
	if (logfile.is_open())
	{
		std::string message = fname;
		message.append(" (" + std::to_string(a) + ")");
		message.append(" (" + std::to_string(b) + ")");
		message.append(" (" + std::to_string(c) + ")");
		message.append(" (" + std::to_string(d) + ")\n");
		logfile << message;
	}
}

void stoplog()
{
	if (logfile.is_open())
	{
		printfn(__FUNCTION__);
		logfile.close();
	}
}

void startlog()
{
	return;

	static bool started = false;

	if (!started)
	{
		started = true;
		std::atexit(stoplog);
		logfile.open("pokpomlog.txt", std::ios::trunc | std::ios::out);
		printfn(__FUNCTION__);
	}
}

DllExport int getType(int number)
{
	startlog();
	printfn(__FUNCTION__, number);
	return (!number) ? 3 : 0;
}

DllExport const wchar_t* getName()
{
	startlog();
	printfn(__FUNCTION__);
	return demul_name;
}

int func1() { printfn(__FUNCTION__); return 0xFFFFFFFF; }
int func2() { printfn(__FUNCTION__); return 0; }

enum DMA_RETURN
{
	RET_STATUS = 0x05,
	RET_STATUS_EX,
	RET_DEVICE_REPLY = 0x07,
	RET_DATA_TRANSFER,
	RET_TRAMSMIT_AGAIN = 0xFC,
	RET_UNKNOWN_COMMAND,
	RET_JAMMA_ID = 0x83
};

enum DMA_COMMAND
{
	GET_STATUS = 1,
	GET_STATUS_EX,
	GET_CONDITION = 9,
	GET_MEDIA_INFO,
	BLOCK_READ,
	BLOCK_WRITE,
	SET_CONDITION = 14,

	STATE_NAME = 0x10,
	STATE_CMD_VER = 0x11,
	STATE_JAMMA_VER = 0x12,
	STATE_COM_VER = 0x13,
	STATE_FEATURES = 0x14,

	JAMMA_EEPROM_READ = 0x03,
	JAMMA_EEPROM_WRITE = 0x0B,
	JAMMA_GET_CAPS = 0x15,
	JAMMA_SUBDEVICE = 0x17,
	JAMMA_TRANSFER_REPEAT = 0x21,
	JAMMA_TRANSFER = 0x27,
	JAMMA_GET_DATA = 0x33,
	JAMMA_ID = 0x82,
	JAMMA_CMD = 0x86
};

RumbleSettings rSettings;
RumbleConfig rConfig;

u8 AST = 0x13;
u16 AST_ms = 0x13 * 250 + 250;
u8 FreqM, FreqL, FreqH;

u16 Watchdog_ms = 150;
HANDLE hVibrationThread;

void StopVibration()
{
	Input::StopRumble(settings[0].xinputPort);
}

void UpdateVibration()
{
	bool plugged = true;
	s16 intensity = (s16)(rConfig.Mpow - rConfig.Ppow);

	if (intensity == 0)
		StopVibration();
	else
	{
		Input::DreamcastRumble(intensity, rConfig.FREQ > FreqH, rConfig.FREQ < FreqL,
			Watchdog_ms, settings[0], plugged, hVibrationThread);
	}
}

u32 RUMBLE(u32 command, u32* buffer, u32 buffer_len)
{
	switch (command)
	{
	case GET_CONDITION:
		buffer[0] = 0x00010000;
		buffer[1] = rSettings.RAW;
		return RET_DATA_TRANSFER;

	case GET_MEDIA_INFO:
		buffer[0] = 0x00010000;
		buffer[1] = rSettings.RAW;
		return RET_DATA_TRANSFER;

	case BLOCK_READ:
		buffer[0] = 0x00010000;
		buffer[1] = 0;
		buffer[2] = (0x0200 << 16) | (AST << 8);
		return RET_DATA_TRANSFER;

	case BLOCK_WRITE:
		AST = (buffer[2] >> 16) & 0xFF;
		AST_ms = AST * 250 + 250;
		return RET_DEVICE_REPLY;

	case SET_CONDITION:
		rConfig.RAW = buffer[0];

		if (rConfig.EXH & rConfig.INH)
			return RET_TRAMSMIT_AGAIN;

		UpdateVibration();
		return RET_DEVICE_REPLY;

	default:
		printf("Pokopom Rumble -> Unknown MAPLE command: %X\n", command);
		return RET_UNKNOWN_COMMAND;
	}
}

int __fastcall func3(int a, int b, int c, int d)// ... rumble?
{
	u32 *buffer = (u32*)b;

	printfn(__FUNCTION__, a, buffer[0], buffer[1], d);
	RUMBLE(a, buffer, c);
	
	return 0;
} 



void func4() { printfn(__FUNCTION__); }

int func5() { printfn(__FUNCTION__); return 0x7601; } // ????

int __fastcall func6(u8 a) { printfn(__FUNCTION__, a); return 0; }
void func7() { printfn(__FUNCTION__); }

void __fastcall func8(int a, int b)
{
	//printfn(__FUNCTION__, a, b);
	bool plugged = true;
	Input::DreamcastPoll((u32*)a, settings[0], plugged);
} // READ JOY


void __fastcall func8b(int a, int b) { printfn(__FUNCTION__, a, b); } // READ JOY

int __fastcall func9(int a) { printfn(__FUNCTION__, a); return 0xFFFFFFFF; } // JOYCAPS ?

int func10()
{
	printfn(__FUNCTION__);

	FileIO::INI_LoadSettings();
	CreateConfigDialog();

	return 0;
}

void func11() { printfn(__FUNCTION__); }
int func12() { printfn(__FUNCTION__); return 0; } // Close ?
u8 func13() { printfn(__FUNCTION__); return 1; }

DllExport char getInterface(u32* demul)
{
	startlog();
	printfn(__FUNCTION__, demul[0], demul[4]);
	FileIO::INI_LoadSettings();

	rSettings.RAW = 0x3B07E010;
	rConfig.RAW = 0;

	hVibrationThread = nullptr;
	StopVibration();

	FreqM = (rSettings.FM1 + rSettings.FM0) >> 1;
	FreqL = (u8)(FreqM * 2.0f / 3.0f);
	FreqH = (u8)(FreqM * 1.5f);

	u32 some_var = demul[4];

	demul_interface *di = (demul_interface*)&demul[138];
	di->func01 = &func1; // 600

	switch (some_var)
	{
	case 0:
		di->func02 = &func2; // 596
		di->func03 = &func3; // 592
		di->func04 = &func4; // 588
		di->func05 = &func5; // 584
		di->func08 = &func8; // 572
		di->func09 = &func9; // 568
		di->func10 = &func10; // 564
		di->func11 = &func11; // 560
		di->func12 = &func12; // 556
		di->func13 = &func13; // 552, CONFIG ?
		return 1;

	case 1:
		di->func03 = &func3; // 592
		di->func04 = &func4; // 588
		di->func05 = &func5; // 584
		di->func08 = &func8; // 572
		di->func09 = &func9; // 568

	case 2:
		di->func02 = &func2; // 596
	
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
		di->func06 = &func6; // 580

	case 12:
		di->func07 = &func7; // 576

	case 8:
		if(some_var == 8)
			di->func08 = &func8b; // 572

		di->func10 = &func10; // 564
		di->func11 = &func11; // 560
		di->func12 = &func12; // 556
		di->func13 = &func13; // 552
		return 1;
	}
		
	return 0;
}
