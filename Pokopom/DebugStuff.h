/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once


#if 0

	bool _DebugOpen();
	void _DebugClose();
	void _DebugFunc(const char* func);
	void _DebugPrint(const char* func, const char* fmt, ...);

	#define DebugOpen()		_DebugOpen()
	#define DebugClose()	_DebugClose()
	#define DebugPrint(...)	_DebugPrint(__FUNCTION__, __VA_ARGS__)
	#define DebugFunc()		_DebugFunc(__FUNCTION__)

#else

	#define DebugOpen()
	#define DebugClose()
	#define DebugPrint(...)
	#define DebugFunc()

#endif