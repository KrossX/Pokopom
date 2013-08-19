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

#include "Controller.h"
#include "PSemuPro_Interface.h"
#include "FileIO.h"

#include <list>

struct freezeData
{
	s32 size;
	u8 *data;
};

struct keyEvent
{
	u32 key;
	u32 evt;
};

struct _emuStuff
{
	HWND hWnd;
	WNDPROC WndProc;
} emuStuff;

std::list <keyEvent> keyEventList;

PlayStationDevice * controller[2] = {NULL, NULL};

wchar_t  settingsDirectory[1024] = {0}; // for PCSX2

u32 bufferCount = 0, curPort = 0;
u8 multitap = 0;