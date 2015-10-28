/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
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

u8 SwapPorts();
void SetPriority();

void KeyboardOpen();
void KeyboardClose();
void KeyboardCheck();

struct keyEvent
{
	u32 key;
	u32 evt;
};

extern std::list <keyEvent> keyEventList;

extern bool SwapPortsEnabled;
extern bool isPs2Emulator;
extern bool isPSemulator;
extern bool bPriority;
extern bool bKeepAwake;
extern s32 INIversion;
