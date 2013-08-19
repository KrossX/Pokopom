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
}
