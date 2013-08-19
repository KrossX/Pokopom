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

#include <math.h>

namespace Input
{
	inline WORD Clamp(f64 input) { return input > 0xFFFF ? 0xFFFF : (WORD)input; }
	inline s32 ClampAnalog(f64 input) {	return (s32)(input < -32768? -32768 : input > 32767 ? 32767 : input); }

	u16 FASTCALL ConvertAnalog(s32 X, s32 Y, _Settings &set, u8 mode);
}
