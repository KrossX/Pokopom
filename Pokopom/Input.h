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

namespace Input
{
	bool FASTCALL Recheck(u8 port);
	void FASTCALL Pause(bool pewpew);
	void FASTCALL StopRumble(u8 port, bool &gamepadPlugged);

	bool FASTCALL CheckAnalogToggle(u8 port);
	void FASTCALL SetAnalogLed(u8 port, bool digital);

	void FASTCALL DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged);
	void FASTCALL DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged);

	void FASTCALL DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged);
	void FASTCALL DreamcastRumble(s16 intensity, bool freqH, bool freqL, void* thread, _Settings &set, bool &gamepadPlugged);

	void FASTCALL N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged);
	void FASTCALL N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged);
	void FASTCALL N64rumble(bool on, _Settings &set, bool &gamepadPlugged);
}
