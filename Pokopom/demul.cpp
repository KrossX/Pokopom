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

std::ofstream demul_logfile;

void printfn(std::string fname, int a = 0, int b = 0, int c = 0, int d = 0)
{
	if (demul_logfile.is_open())
	{
		std::string message = fname;
		message.append(" (" + std::to_string(a) + ")");
		message.append(" (" + std::to_string(b) + ")");
		message.append(" (" + std::to_string(c) + ")");
		message.append(" (" + std::to_string(d) + ")\n");
		demul_logfile << message;
	}
}

void stoplog()
{
	if (demul_logfile.is_open())
	{
		printfn(__FUNCTION__);
		demul_logfile.close();
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
		demul_logfile.open("pokpomlog.txt", std::ios::trunc | std::ios::out);
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

int __fastcall func3(int a, int b, int c, int d)// ... rumble?
{
	u32 *buffer = (u32*)b;

	if (a == 0x0E)
	{
		rConfig.RAW = buffer[0];
		UpdateVibration();
	}

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

	//+2 (8 bytes) diff 230915 -> 221215
	demul_interface *di = (demul_interface*)&demul[138 + 2];
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
