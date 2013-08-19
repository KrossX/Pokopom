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

Controller * controller[2] = {NULL, NULL};
unsigned int bufferCount = 0, curPort = 0;
bool isPs2Emulator = false;

struct freezeData 
{
	int size;
	unsigned char *data;
};

struct keyEvent 
{
	unsigned int key;
	unsigned int evt;
};

struct _emuStuff
{
	HWND hWnd;
	WNDPROC WndProc;
} emuStuff;

std::list <keyEvent> keyEventList;
