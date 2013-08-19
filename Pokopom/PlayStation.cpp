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

#include "PlayStation.h"
#include "ConfigDialog.h"
#include "Input.h"

_emuStuff emuStuff;
PlayStationDevice * controller[2] = {NULL, NULL};

char settingsDirectory[1024] = {0}; // for PCSX2

u32 bufferCount = 0, curPort = 0;
u8 multitap = 0;

////////////////////////////////////////////////////////////////////////
// PPDK developer must change libraryName field and can change revision and build
////////////////////////////////////////////////////////////////////////

const u32 revision = 2;
const u32 build    = 0;

const u32 versionPS1 = (emupro::PLUGIN_VERSION << 16) | (revision << 8) | build;
const u32 versionPS2 = (0x02 << 16) | (revision << 8) | build;

char libraryName[]      = "Pokopom XInput Pad Plugin"; // rewrite your plug-in name
char PluginAuthor[]     = "KrossX"; // rewrite your name

////////////////////////////////////////////////////////////////////////
// stuff to make this a true PDK module
////////////////////////////////////////////////////////////////////////

DllExport char* CALLBACK PSEgetLibName()
{
	isPSemulator = true;
	return libraryName;
}

DllExport u32 CALLBACK PSEgetLibType()
{
	isPSemulator = true;
	return emupro::LT_PAD;
}

DllExport u32 CALLBACK PSEgetLibVersion()
{
	isPSemulator = true;
	return versionPS1;
}

DllExport char* CALLBACK PS2EgetLibName()
{
	isPs2Emulator = true;
	return PSEgetLibName();
}

DllExport u32 CALLBACK PS2EgetLibType()
{
	isPs2Emulator = true;
	return 0x02;
}

DllExport u32 CALLBACK PS2EgetLibVersion2(u32 type)
{
	isPs2Emulator = true;
	if (type == 0x02)  return versionPS2;
	else return 0;
}

////////////////////////////////////////////////////////////////////////
// Init/shutdown, will be called just once on emu start/close
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADinit(s32 flags) // PAD INIT
{
	FileIO::INI_LoadSettings();
	//printf("Pokopom -> PADinit [%X]\n", flags);

	if (flags & emupro::pad::USE_PORT1)
	{
		if(isPs2Emulator)
		{
			if(settings[0].isGuitar)	controller[0] = new PS2_Guitar(settings[0]);
			else						controller[0] = new DualShock2(settings[0]);
		}
		else
		{
			if(multitap == 1)	controller[0] = new MultiTap(settings);
			else				controller[0] = new DualShock(settings[0]);
		}

		if(controller[0]) controller[0]->SetPort(0);
		else return emupro::ERR_FATAL;
	}

	if (flags & emupro::pad::USE_PORT2)
	{
		if(isPs2Emulator)
		{
			if(settings[0].isGuitar)	controller[1] = new PS2_Guitar(settings[1]);
			else						controller[1] = new DualShock2(settings[1]);
		}
		else
		{
			if(multitap == 2)	controller[1] = new MultiTap(settings);
			else				controller[1] = new DualShock(settings[1]);

			if(multitap == 1) controller[1]->Disable();
		}

		if(controller[1]) controller[1]->SetPort(1);
		else return emupro::ERR_FATAL;
	}

	return emupro::INIT_ERR_SUCCESS;
}

DllExport void CALLBACK PADshutdown() // PAD SHUTDOWN
{
	//printf("Pokopom -> PADshutdown\n");

	delete controller[0];
	delete controller[1];

	controller[0] = controller[1] = NULL;
}

////////////////////////////////////////////////////////////////////////
// Open/close will be called when a games starts/stops
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADopen(void* pDisplay) // PAD OPEN
{
	//printf("Pokopom -> PADopen\n");
	Input::Pause(false);

	GetDisplay(pDisplay);
	KeyboardOpen();
	KeepAwake(KEEPAWAKE_INIT);

	return emupro::pad::ERR_SUCCESS;
}

DllExport s32 CALLBACK PADclose() // PAD CLOSE
{
	//printf("Pokopom -> PADclose\n");
	Input::Pause(true);
	KeyboardClose();
	KeepAwake(KEEPAWAKE_CLOSE);

	return emupro::pad::ERR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// call config dialog
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADconfigure()
{
	isPSemulator = true;

	FileIO::INI_LoadSettings();
	CreateConfigDialog();

	return emupro::pad::ERR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// show about dialog
////////////////////////////////////////////////////////////////////////

DllExport void CALLBACK PADabout()
{
	ShowDialog(L"Pokopom XInput pad plugin - KrossX © 2012", L"About...");
}

////////////////////////////////////////////////////////////////////////
// test... well, we are ever fine ;)
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADtest()
{
	return emupro::pad::ERR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// tell the controller's port which can be used
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADquery()
{
	//printf("Pokopom -> PADquery\n");
	return emupro::pad::USE_PORT1 | emupro::pad::USE_PORT2;
}

////////////////////////////////////////////////////////////////////////
// tell the input of pad
// this function should be replaced with PADstartPoll and PADpoll
////////////////////////////////////////////////////////////////////////

s32 FASTCALL PADreadPort(s32 port, emupro::pad::DataS* ppds)
{
	//printf("Pokopom -> PADreadPort [%X]\n", port);

	controller[port]->command(0, 0x01);
	u8 cType = controller[port]->command(1, 0x42);
	ppds->controllerType = cType >> 4;

	controller[port]->command(2, 0x00);
	ppds->buttonStatus = controller[port]->command(3, 0x00) | (controller[port]->command(4, 0x00) << 8);

	cType = cType & 0xF;

	if(cType > 2)
	{
		ppds->rightJoyX = ppds->moveX = controller[port]->command(5, 0x00);
		ppds->rightJoyY = ppds->moveY = controller[port]->command(6, 0x00);

		if (cType >= 0x03)
		{
			ppds->leftJoyX = controller[port]->command(7, 0x00);
			ppds->leftJoyY = controller[port]->command(8, 0x00);
		}
	}

	return emupro::pad::ERR_SUCCESS;
}

DllExport s32 CALLBACK PADreadPort1(emupro::pad::DataS* ppds)
{
	return PADreadPort(0, ppds);
}

DllExport s32 CALLBACK PADreadPort2(emupro::pad::DataS* ppds)
{
	return PADreadPort(1, ppds);
}

////////////////////////////////////////////////////////////////////////
// input and output of pad
////////////////////////////////////////////////////////////////////////

DllExport u8 CALLBACK PADstartPoll(s32 port)
{
	curPort = port - 1;
	bufferCount = 0;

	u8 data = controller[curPort]->command(bufferCount, 0x01);

	//if(curPort == 0) printf("\n[%02d] [%02X|%02X]\n", bufferCount, 0x01, data);
	//printf("\n[%02d|%02d] [%02X|%02X]\n", bufferCount, curPort, 0x01, data);

	KeepAwake(KEEPAWAKE_KEEP);

	return data;
}

DllExport u8 CALLBACK PADpoll(u8 data)
{
	bufferCount++;

	u8 doto = controller[curPort]->command(bufferCount, data);

	//if(curPort == 0) printf("[%02d] [%02X|%02X]\n", bufferCount, data, doto);
	//printf("[%02d|%02d] [%02X|%02X]\n", bufferCount, curPort, data, doto);

	return doto;
}

////////////////////////////////////////////////////////////////////////
// other stuff
////////////////////////////////////////////////////////////////////////


DllExport u32 CALLBACK PADfreeze(s32 mode, freezeData *data)
{
	//printf("Pokopom -> PADfreeze [%X]\n", mode);
	if(!data) return (u32)emupro::ERR_FATAL;

	switch(mode)
	{
	case emupro::Savestate::LOAD:
		{
			PlayStationDeviceState *state = (PlayStationDeviceState *)data->data;

			if(memcmp(state[0].libraryName, libraryName, 25) == 0 &&
				state[0].version == ((revision << 8) | build))
			{
				controller[0]->LoadState(state[0]);
				controller[1]->LoadState(state[1]);
			}
			else
			{
				printf("Pokopom -> Wrong savestate data to load.");
			}

		} break;

	case emupro::Savestate::SAVE:
		{
			PlayStationDeviceState state[2];

			memset(state, 0, sizeof(state));

			memcpy(state[0].libraryName, libraryName, 25);
			memcpy(state[1].libraryName, libraryName, 25);

			state[0].version = state[1].version = (revision << 8) | build;

			controller[0]->SaveState(state[0]);
			controller[1]->SaveState(state[1]);
			memcpy(data->data, state, sizeof(state));
		} break;

	case emupro::Savestate::QUERY_SIZE:
		{
			data->size = sizeof(PlayStationDeviceState) * 2;
		} break;
	}

	return emupro::ERR_SUCCESS;
}


DllExport keyEvent* CALLBACK PADkeyEvent()
{
	//printf("Pokopom -> PADkeyEvent\n");

	static keyEvent pochy;

	if(!keyEventList.empty())
	{
		pochy = keyEventList.front();
		keyEventList.pop_back();
		return &pochy;
	}

	return NULL;
}

DllExport s32 PADkeypressed()
{
	//printf("Pokopom -> PADkeypressed\n");

	static keyEvent pochy;

	KeyboardCheck();

	if(!keyEventList.empty())
	{
		pochy = keyEventList.front();
		keyEventList.pop_back();
		return pochy.key;
	}

	return 0;
}

DllExport u32 CALLBACK PADqueryMtap(u8 port)
{
	//printf("Pokopom -> PADqueryMtap [%X]\n", port);
	return 0;
}

DllExport void CALLBACK PADsetSettingsDir(const char *dir)
{
	memcpy(settingsDirectory, dir, strlen(dir)+1);
}

DllExport u32 CALLBACK PADsetSlot(u8 port, u8 slot)
{
	//printf("Pokopom -> PADsetSlot [%X|%X]\n", port, slot);
	return 0;
}

DllExport void CALLBACK PADupdate(s32 port)
{
	//printf("Pokopom -> PADupdate [%X]\n", port);
}


