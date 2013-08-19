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

namespace XInput
{
	bool __fastcall Recheck(u8 port);
	void __fastcall Pause(bool pewpew);
	void __fastcall StopRumble(u8 port, bool &gamepadPlugged);

	void __fastcall DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged);
	void __fastcall DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged);	
	
	void __fastcall DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged);
	void __fastcall DreamcastRumble(s16 intensity, bool freqH, bool freqL, LPVOID thread, _Settings &set, bool &gamepadPlugged);

	void __fastcall N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged);
	void __fastcall N64rumble(bool on, _Settings &set, bool &gamepadPlugged);
}
