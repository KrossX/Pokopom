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

#ifdef _WIN32

////////////////////////////////////////////////////////////////////////

class nullDC_Device
{
	nullDC_Device & operator = (const nullDC_Device & other) {}

protected:
	bool gamepadPlugged;
	u32 port;
	_Settings &set;

public:
	nullDC_Device(u32 _port, _Settings &config);

public:
	virtual u32 FASTCALL DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len) { return 0; };
};

////////////////////////////////////////////////////////////////////////

class DreamcastController : public nullDC_Device
{
	DreamcastController & operator = (const DreamcastController & other) {}

protected:
	void PollOut(u32* buffer_out);

public:
	DreamcastController(u32 _port, _Settings &config);

public:
	u32 FASTCALL DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len);
};

////////////////////////////////////////////////////////////////////////

union RumbleSettings
{
	u32 RAW;

	struct
	{
		u8 VSET0;
		u8 VSET1;
		u8 FM0;
		u8 FM1;
	};

	struct
	{
		unsigned VD : 2;
		unsigned VP : 2;
		unsigned VN : 4;

		unsigned VA  : 4;
		unsigned OWF : 1;
		unsigned PD  : 1;
		unsigned CV  : 1;
		unsigned PF  : 1;
	};
};

union RumbleConfig
{
	u32 RAW;

	struct
	{
		u8 CTRL;
		u8 POW;
		u8 FREQ; // Frequency
		u8 INQ;  // Inclination
	};

	struct
	{
		unsigned CNT : 1; // Continuous mode
		unsigned Res : 3; // ...
		unsigned VN  : 4; // Number of sources [1..F]

		unsigned Mpow : 3; // Backward intensity
		unsigned EXH  : 1; // Divergency
		unsigned Ppow : 3; // Forward intensity
		unsigned INH  : 1; // Convergency
	};
};

////////////////////////////////////////////////////////////////////////

class PuruPuruPack : public nullDC_Device
{
	PuruPuruPack & operator = (const PuruPuruPack & other) {}

	RumbleSettings rSettings;
	RumbleConfig rConfig;

	u16 AST_ms;
	u8 AST;
	u8 FreqM, FreqL, FreqH;

	HANDLE hVibrationThread;
	void UpdateVibration();

public:
	struct _thread
	{
		HANDLE &hThread;
		u16 &wait;
		u8 &port;
		bool &gamepadPlugged;

		_thread(HANDLE hT, u16 w, u8 p, bool g) :
			hThread(hT), wait(w), port(p), gamepadPlugged(g)
		{}

		_thread&operator=(const _thread&other) { return *this; }
	} thread;

public:
	u16 Watchdog_ms;
	void StopVibration();

	PuruPuruPack(u32 _port, _Settings &config);
	~PuruPuruPack();

public:
	u32 FASTCALL DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len);
};

////////////////////////////////////////////////////////////////////////

struct Dreamcast_DeviceInfo
{
	u32   ID0[4];
	u8  ID1[2];
	u8  ID2_Name[30];
	u8  ID3_Brand[60];
	u16 ID4[2];
	u8  ID5_Extra[40];
};

enum DMA_COMMAND
{
	GET_STATUS = 1,
	GET_STATUS_EX,
	GET_CONDITION = 9,
	GET_MEDIA_INFO,
	BLOCK_READ,
	BLOCK_WRITE,
	SET_CONDITION = 14
};

enum DMA_RETURN
{
	RET_STATUS = 0x05,
	RET_STATUS_EX,
	RET_DEVICE_REPLY = 0x07,
	RET_DATA_TRANSFER,
	RET_TRAMSMIT_AGAIN = 0xFC,
	RET_UNKNOWN_COMMAND,
};

#endif // WIN32

