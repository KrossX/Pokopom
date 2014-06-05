/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

#include <math.h>

namespace Input
{
	enum
	{
		DS_SELECT = 0,
		DS_L3,
		DS_R3,
		DS_START,
		DS_UP,
		DS_RIGHT,
		DS_DOWN,
		DS_LEFT,
		DS_L2,
		DS_R2,
		DS_L1,
		DS_R1,
		DS_TRIANGLE,
		DS_CIRCLE,
		DS_CROSS,
		DS_SQUARE
	};

	enum
	{
		DC_B = 1,
		DC_A,
		DC_START,
		DC_UP,
		DC_DOWN,
		DC_LEFT,
		DC_RIGHT,
		DC_Y = 9,
		DC_X
	};

	enum
	{
		NAOMI_BUTTON2 = 0,
		NAOMI_BUTTON1,

		NAOMI_DRIGHT,
		NAOMI_DLEFT,
		NAOMI_DDOWN,
		NAOMI_DUP,

		NAOMI_TEST,
		NAOMI_START,

		NAOMI_BUTTON6 = 12,
		NAOMI_BUTTON5,
		NAOMI_BUTTON4,
		NAOMI_BUTTON3,
	};

	enum
	{
		N64_RIGHT = 0,
		N64_LEFT,
		N64_DOWN,
		N64_UP,
		N64_START,
		N64_TRIGGERZ,
		N64_B,
		N64_A,
		N64_CRIGHT,
		N64_CLEFT,
		N64_CDOWN,
		N64_CUP,
		N64_TRIGGERR,
		N64_TRIGGERL
	};

	enum
	{
		ANALOGD_XP = 0x01,
		ANALOGD_XN = 0x02,
		ANALOGD_YP = 0x04,
		ANALOGD_YN = 0x08
	};

	enum
	{
		X360_DUP = 0,
		X360_DDOWN,
		X360_DLEFT,
		X360_DRIGHT,
		X360_START,
		X360_BACK,
		X360_LS,
		X360_RS,
		X360_LB,
		X360_RB,
		X360_A,
		X360_B,
		X360_X,
		X360_Y,
		X360_BIGX,
		X360_BUTTONS
	};

	enum
	{
		X360_STICKLX = 0,
		X360_STICKLY,
		X360_STICKRX,
		X360_STICKRY,
		X360_TRIGGERL,
		X360_TRIGGERR,
		X360_ANALOGS
	};

	struct _Stick 
	{
		s32 X, Y;
		f64 radius;
		f64 angle;

		_Stick()
		{ 
			X = Y = 0;
			radius = 0;
			angle = 0;
		}
	};

	struct _Pad
	{
		u8 buttons[X360_BUTTONS];
		s32 analog[X360_ANALOGS];

		_Stick stickL, stickR;
		_Stick modL, modR;
	};

	void FASTCALL TriggerDeadzone(_Pad& pad, _Settings &set);
	bool FASTCALL InputGetState(_Pad& pad, _Settings &set);
	void FASTCALL GetRadius(_Stick& stick);

	inline WORD Clamp(f64 input) { return input > 0xFFFF ? 0xFFFF : (WORD)input; }
	inline s32 ClampAnalog(f64 input) {	return (s32)(input < -32768? -32768 : input > 32767 ? 32767 : input); }
}

