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

#include "General.h"
#include "Input.h"
#include "Input_Shared.h"

namespace Input
{

u16 FASTCALL ConvertAnalog(s32 X, s32 Y, _Settings &set, u8 mode)
{
	// If input is dead, no need to check or do anything else
	if((X == 0) && (Y == 0)) return mode == 0 ? 0x7F7F : 0;

	f64 radius = sqrt((f64)X*X + (f64)Y*Y);
	const f64 deadzone = set.extThreshold * set.deadzone;

	// Input should be effectively dead under deadzone, no need to do more
	if(radius <= deadzone) return mode == 0? 0x7F7F : 0;

	f64 rX = X/radius, rY = Y/radius;

	if(set.linearity != 0)
		radius = pow(radius / set.extThreshold, set.linearity) * set.extThreshold;

	if(deadzone > 0)
		radius =  (radius - deadzone) * set.extThreshold / (set.extThreshold - deadzone);

	//Antideadzone, inspired by x360ce's setting
	if(set.antiDeadzone > 0)
	{
		const f64 antiDeadzone = set.extThreshold * set.antiDeadzone;
		radius = radius * ((set.extThreshold - antiDeadzone) / set.extThreshold) + antiDeadzone;
	}

	if(radius > set.extThreshold) radius *= set.extMult;

	u16 result = mode == 0? 0x7F7F : 0;

	switch(mode)
	{
	case 0: // Dualshock, Dreamcast...
		{
			X = ClampAnalog(rX * radius);
			Y = ClampAnalog(rY * radius);

			Y = 32767 - Y;
			X = X + 32767;

			X = X < 0 ? 0 : X > 0xFFFF ? 0xFFFF : X;
			Y = Y < 0 ? 0 : Y > 0xFFFF ? 0xFFFF : Y;

			X >>= 8;	Y >>= 8;

			result = (u16)((Y << 8) | X);
		} break;

	case 1: // N64
		{
			//radius modifier should go here...

			X = ClampAnalog(rX * radius);
			Y = ClampAnalog(rY * radius);

			s8 res[2];
			res[0] = (s8)(X>>8);
			res[1] = (s8)(Y>>8);

			u16 * r16 = (u16*)&res[0];
			result = r16[0];
		} break;
	}

	return  result;
}

} // End namespace Input
