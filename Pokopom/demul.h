/* Copyright (c) 2015 KrossX <krossx@live.com>
* License: http://www.opensource.org/licenses/mit-license.html  MIT License
*/

#pragma once

#include "General.h"

struct demul_interface
{
	u8(*func13)(); // 552
	int(*func12)(); // 556
	void(*func11)(); // 560
	int(*func10)(); // 564
	int(__fastcall *func09)(int); // 568
	void(__fastcall *func08)(int, int); // 572
	void(*func07)(); // 576
	int(__fastcall *func06)(u8); // 580
	int(*func05)(); // 584
	void(*func04)(); // 588
	int(__fastcall *func03)(int, int, int, int); // 592
	int(*func02)(); // 596 
	int(*func01)(); // 600
};

// old nullDC stuff

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

		unsigned VA : 4;
		unsigned OWF : 1;
		unsigned PD : 1;
		unsigned CV : 1;
		unsigned PF : 1;
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
		unsigned VN : 4; // Number of sources [1..F]

		unsigned Mpow : 3; // Backward intensity
		unsigned EXH : 1; // Divergency
		unsigned Ppow : 3; // Forward intensity
		unsigned INH : 1; // Convergency
	};
};

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
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len) {
		return 0;
	};
};

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