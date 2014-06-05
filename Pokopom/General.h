/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
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
	#define HANDLE void*
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

#include "TypeDefs.h"
#include "Stuff.h"
#include "Settings.h"

#include "DebugStuff.h"
