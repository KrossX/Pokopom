/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

namespace Input
{
	bool FASTCALL Recheck(u8 port);
	void FASTCALL Pause(bool pewpew);
	void FASTCALL StopRumble(u8 port);
	void StopRumbleAll();

	bool FASTCALL CheckAnalogToggle(u8 port);
	void FASTCALL SetAnalogLed(u8 port, bool digital);

	void FASTCALL DualshockPoll(u16 * bufferOut, _Settings &set, bool &gamepadPlugged, const bool digital);
	void FASTCALL DualshockRumble(u8 smalldata, u8 bigdata, _Settings &set, bool &gamepadPlugged);
	bool FASTCALL DualshockPressure(u8 * bufferOut, u32 mask, _Settings &set, bool &gamepadPlugged);

	void FASTCALL DreamcastPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged);
	void FASTCALL DreamcastRumble(s16 intensity, bool freqH, bool freqL, u16 wait, _Settings &set, bool &gamepadPlugged, HANDLE &thread);

	void FASTCALL NaomiPoll(u32* buffer_out, _Settings &set, bool &gamepadPlugged);

	void FASTCALL N64rumbleSwitch(u8 port, bool &rumble, bool &gamepadPlugged);
	void FASTCALL N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged);
	void FASTCALL N64rumble(bool on, _Settings &set, bool &gamepadPlugged);

	void FASTCALL demul_read1(u32* buffer_out, _Settings &set, bool &gamepadPlugged);
}
