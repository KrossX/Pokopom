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

#include "Settings.h"

_Settings::_Settings()
{
	defaultAnalog = false;
	isGuitar = false;
	xinputPort = 0;
	deadzone = 0.0;
	rumble = 1.0;
	pressureRate = 10;
	
	for(short i = 0; i < 4; i++)
	{
		axisInverted[i] = false;
		axisRemap[i] = i;
		axisValue[i] = 0;	
	}
}