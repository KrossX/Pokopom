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

#pragma once

enum
{
	KEEPAWAKE_INIT = 0,
	KEEPAWAKE_CLOSE
};

void GetDisplay(void* pDisplay);
void ShowDialog(const wchar_t* message, const wchar_t* title);
void KeepAwake(u8 mode);

void KeyboardOpen();
void KeyboardClose();
void KeyboardCheck();

struct keyEvent
{
	u32 key;
	u32 evt;
};

extern std::list <keyEvent> keyEventList;

extern bool isPs2Emulator;
extern bool isPSemulator;
extern bool bKeepAwake;
extern s32 INIversion;

#ifdef _WIN32
extern HWND hDisplay;
#endif

#ifdef __linux__
extern Display *hDisplay;
#endif
