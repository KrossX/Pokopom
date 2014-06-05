/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"

#include "Controller.h"
#include "PSemuPro_Interface.h"
#include "FileIO.h"

struct freezeData
{
	s32 size;
	u8 *data;
};

struct _emuStuff
{
	HWND hWnd;
	WNDPROC WndProc;
};

extern _emuStuff emuStuff;

u8 SwapPorts();
