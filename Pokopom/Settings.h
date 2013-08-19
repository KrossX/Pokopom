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

enum GAMEPAD_AXES
{
	GP_AXIS_LX,
	GP_AXIS_LY,
	GP_AXIS_RX,
	GP_AXIS_RY
};

struct _Settings
{
	void SetDefaults();

	bool greenAnalog;
	bool defaultAnalog;
	u8 xinputPort;
	f32 deadzone, rumble, antiDeadzone;
	f64 extThreshold, extMult;

	bool axisInverted[4];
	s16 axisRemap[4];
	s32 axisValue[4];

	u16 pressureRate;
	f64 linearity;

	bool isGuitar;
	bool disabled;

	_Settings();
	_Settings(const _Settings &);
	_Settings& operator=(const _Settings &);
};

extern _Settings settings[4];