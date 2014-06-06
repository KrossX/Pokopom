/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

enum GAMEPAD_AXES
{
	GP_AXIS_LX,
	GP_AXIS_LY,
	GP_AXIS_RX,
	GP_AXIS_RY
};

struct StickSettings
{
	bool b4wayDAC;
	bool DACenabled;
	f64 DACthreshold;

	f64 extThreshold, extMult;
	f32 deadzone, antiDeadzone;
	f64 linearity;

	void SetDefaults();
};

struct _Settings
{
	void SetDefaults();

	bool greenAnalog;
	bool defaultAnalog;
	bool sticksLocked;

	u8 xinputPort;
	f32 rumble;
	
	StickSettings stickL, stickR;

	u8 axisInverted[4];
	u8 axisRemap[4];
	s32 axisValue[4];

	u16 pressureRate;
	u8 triggerDeadzone;

	bool SwapDCBumpers;
	bool SwapXO;
	bool SwapSticksEnabled;
	bool isGuitar;
	bool disabled;

	_Settings();
	_Settings(const _Settings &);
	_Settings& operator=(const _Settings &);
};

extern _Settings settings[4];
