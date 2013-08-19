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

#ifdef _WIN32
	#include <Windows.h>

	#define EXPORT_CALL __stdcall
	#define FASTCALL __fastcall
	#define CALL _cdecl

	#ifdef __cplusplus
	#define DllExport extern "C" __declspec(dllexport)
	#else
	#define DllExport __declspec(dllexport)
	#endif
#elif defined __linux__
	#define EXPORT_CALL __stdcall
	#define FASTCALL
	#define CALLBACK
	#define HWND void*
	#define WNDPROC void*
	#define WORD short

	#include <string.h>
	#include <X11/Xlib.h>

	#ifdef __cplusplus
	#define DllExport extern "C"
	#else
	#define DllExport
	#endif
#endif

#include <stdio.h>
#include <list>

#include "../../Common/TypeDefs.h"
#include "Stuff.h"
#include "Settings.h"

#if 0
	#define Debug printf
	#define DebugFunc() printf("Pokopom -> "__FUNCTION__"\n")
#else
	#define Debug(...)
	#define DebugFunc()
#endif