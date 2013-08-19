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

class NaomiController : public nullDC_Device
{
	NaomiController & operator = (const NaomiController & other) {}

	u8 EEPROM[0x100];
	struct { u8 cmd, mode, node; } State;

	void FASTCALL SetState(u8 *in, u8 mode);

protected:
	void PollOut(u32* buffer_out);

public:
	NaomiController(u32 _port, _Settings &config);

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
	SET_CONDITION = 14,

	STATE_NAME = 0x10,
	STATE_CMD_VER = 0x11,
	STATE_JAMMA_VER = 0x12,
	STATE_COM_VER = 0x13,
	STATE_FEATURES = 0x14,

	JAMMA_EEPROM_READ = 0x03,
	JAMMA_EEPROM_WRITE = 0x0B,
	JAMMA_GET_CAPS = 0x15,
	JAMMA_SUBDEVICE = 0x17,
	JAMMA_TRANSFER_REPEAT = 0x21,
	JAMMA_TRANSFER = 0x27,
	JAMMA_GET_DATA = 0x33,
	JAMMA_ID = 0x82,
	JAMMA_CMD = 0x86
};

enum DMA_RETURN
{
	RET_STATUS = 0x05,
	RET_STATUS_EX,
	RET_DEVICE_REPLY = 0x07,
	RET_DATA_TRANSFER,
	RET_TRAMSMIT_AGAIN = 0xFC,
	RET_UNKNOWN_COMMAND,
	RET_JAMMA_ID = 0x83
};

enum NDC_DEVICES0 // NORMAL
{
	NDC0_DREAMCAST_CONTROLLER = 0,
	NDC0_PURUPURU_PAKKU,
	NDC0_ENDOFLIST
};

enum NDC_DEVICES2 // NAOMI
{
	NDC2_NAOMI_JAMMA = 0,
	NDC2_ENDOFLIST
};

#endif // WIN32

