/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"

_Settings::_Settings()
{
	SetDefaults();
}

void _Settings::SetDefaults()
{
	SwapXO = false;
	SwapSticksEnabled = false;
	greenAnalog = false;
	defaultAnalog = true;
	isGuitar = false;
	disabled = false;
	sticksLocked = true;
	xinputPort = 0;

	stickL.SetDefaults();
	stickR.SetDefaults();

	rumble = 1.0;
	pressureRate = 10;
	triggerDeadzone = 0;

	for(u8 i = 0; i < 4; i++)
	{
		axisInverted[i] = false;
		axisRemap[i] = i;
		axisValue[i] = 0;
	}
}

void StickSettings::SetDefaults()
{
	b4wayDAC = false;
	DACenabled = true;
	DACthreshold = 28000;

	extThreshold = 32767.0; // 40201 real max radius
	extMult = 1.4142135623730950488016887242097; // sqrt(2)

	deadzone = 0.0;
	antiDeadzone = 0.0;
	linearity = 0.0;
}
