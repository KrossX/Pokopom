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

u32 bufferCount = 0;
u8 curPort = 0, curSlot = 1;
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
	Debug("Pokopom -> PADinit [%X]\n", flags);

	for(int pad = 0; pad < 2; pad++)
	{
		if(isPs2Emulator)
		{
			switch(multitap)
			{
			case 0: 
				if(settings[pad].isGuitar)	controller[pad] = new PS2_Guitar(settings[pad]);
				else						controller[pad] = new DualShock2(settings[pad]);
				break;

			case 1: 
				if(pad == 0) controller[pad] = new MultiTap2(settings);
				else controller[pad] = new DualShock(settings[pad]);
				break;

			case 2: 
				if(pad == 0) controller[pad] = new DualShock(settings[pad]);
				else controller[pad] = new MultiTap2(settings);
				break;
			}

		}
		else
		{
			switch(multitap)
			{
			case 0: 
				controller[pad] = new DualShock(settings[pad]); 
				break;

			case 1: 
				if(pad == 0) controller[pad] = new MultiTap(settings);
				else controller[pad] = new DualShock(settings[pad]);
				break;

			case 2: 
				if(pad == 0) controller[pad] = new DualShock(settings[pad]);
				else controller[pad] = new MultiTap(settings);
				break;
			}
		}

		if(controller[pad])
			controller[pad]->SetPort((u8)pad);
		else 
			return emupro::ERR_FATAL;
	}

	return emupro::INIT_ERR_SUCCESS;
}

DllExport void CALLBACK PADshutdown() // PAD SHUTDOWN
{
	DebugFunc();

	delete controller[0];
	delete controller[1];

	controller[0] = controller[1] = NULL;
}

////////////////////////////////////////////////////////////////////////
// Open/close will be called when a games starts/stops
////////////////////////////////////////////////////////////////////////

DllExport s32 CALLBACK PADopen(void* pDisplay) // PAD OPEN
{
	DebugFunc();
	Input::Pause(false);

	GetDisplay(pDisplay);
	KeyboardOpen();
	KeepAwake(KEEPAWAKE_INIT);

	return emupro::pad::ERR_SUCCESS;
}

DllExport s32 CALLBACK PADclose() // PAD CLOSE
{
	DebugFunc();
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
	DebugFunc();
	return emupro::pad::USE_PORT1 | emupro::pad::USE_PORT2;
}

////////////////////////////////////////////////////////////////////////
// tell the input of pad
// this function should be replaced with PADstartPoll and PADpoll
////////////////////////////////////////////////////////////////////////

s32 FASTCALL PADreadPort(s32 port, emupro::pad::DataS* ppds)
{
	Debug("Pokopom -> PADreadPort [%X]\n", port);

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
	curPort = (u8)(port - 1) ^ SwapPorts();
	bufferCount = 0;
	
	u8 data = controller[curPort]->command(bufferCount, curSlot);

	//if(curPort == 0) printf("\n[%02d] [%02X|%02X]\n", bufferCount, curSlot, data);
	Debug("\n[%02d|%02d] [%02X|%02X]\n", bufferCount, curPort, curSlot, data);

	return data;
}

DllExport u8 CALLBACK PADpoll(u8 data)
{
	bufferCount++;

	u8 doto = controller[curPort]->command(bufferCount, data);

	//if(curPort == 0) printf("[%02d] [%02X|%02X]\n", bufferCount, data, doto);
	Debug("[%02d|%02d] [%02X|%02X]\n", bufferCount, curPort, data, doto);

	return doto;
}

////////////////////////////////////////////////////////////////////////
// other stuff
////////////////////////////////////////////////////////////////////////


DllExport u32 CALLBACK PADfreeze(s32 mode, freezeData *data)
{
	Debug("Pokopom -> PADfreeze [%X]\n", mode);
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
	DebugFunc();
	static keyEvent pochy;

	if(!isPs2Emulator)
		KeyboardCheck();

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
	keyEvent *pochy = PADkeyEvent();
	return pochy? pochy->key : 0;
}

DllExport u32 CALLBACK PADqueryMtap(u8 port)
{
	printf("Pokopom -> PADqueryMtap [%X]\n", port);
	
	if(multitap == 1 && port == 1) return 1;
	else if(multitap == 2) return 1;
	else return 0;
}

DllExport void CALLBACK PADsetSettingsDir(const char *dir)
{
	Debug("Pokopom -> PadsetSettingsDir: %s\n", dir);
	if(dir)
		memcpy(settingsDirectory, dir, strlen(dir)+1);
}

DllExport void  PADWriteEvent(keyEvent &evt)
{
	DebugFunc();
	switch(evt.evt)
	{
	case 0x02:
		evt.evt -= 1;
		keyEventList.push_back(evt);
		break;

	case 0x03:
		evt.evt -= 1;
		evt.key |= 0x40000000;
		keyEventList.push_back(evt);
		break;
	}
}

DllExport u32 CALLBACK PADsetSlot(u8 port, u8 slot)
{
	Debug("Pokopom -> PADsetSlot [%X|%X] ", port, slot);
	curPort = port - 1;
	curSlot = slot;

	switch(multitap)
	{
	case 0:
		if(slot == 1) return 1;

	case 1:
		if(port == 1) return 1;

	case 2:
		if(port == 1 && slot == 1) return 1;
		else if(port == 2 && slot < 4) return 1;
	}

	return 0;
}

DllExport void CALLBACK PADupdate(s32 port)
{
	Debug("Pokopom -> PADupdate [%X]\n", port);
}




