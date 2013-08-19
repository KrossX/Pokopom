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
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
	#endif

	#include <Windows.h>

	#ifdef __cplusplus
	#define DllExport extern "C" __declspec(dllexport)
	#else
	#define DllExport __declspec(dllexport)
	#endif
#elif defined __linux__
	#define CALLBACK
	#define HWND void*

	#ifdef __cplusplus
	#define DllExport extern "C"
	#else
	#define DllExport
	#endif
#endif

#include <stdio.h>

#include "..\..\Common\TypeDefs.h"

extern bool isPs2Emulator;
extern bool isPSemulator;
extern bool bKeepAwake;
extern s32 INIversion;
