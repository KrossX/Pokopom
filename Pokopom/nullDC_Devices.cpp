#include "General.h"
#include "nullDC_Devices.h"
#include "Input_Backend.h"

////////////////////////////////////////////////////////////////////////
// General and constructors
////////////////////////////////////////////////////////////////////////

nullDC_Device::nullDC_Device(u32 _port, _Settings &config) : port(_port), set(config)
{
}

DreamcastController::DreamcastController(u32 _port, _Settings &config) : nullDC_Device(_port, config)
{
}

PuruPuruPack::PuruPuruPack(u32 _port, _Settings &config) : nullDC_Device(_port, config),
	thread(hVibrationThread, Watchdog_ms, set.xinputPort, gamepadPlugged)
{
	AST = 0x13; // 5 seconds in 0.25s units
	AST_ms = AST * 250 + 250; // 5000ms

	Watchdog_ms = 150; // 150ms cycle... enough?

	rSettings.RAW = 0x3B07E010;
	rConfig.RAW = 0;

	hVibrationThread = NULL;

	FreqM = (rSettings.FM1 + rSettings.FM0) >> 1;
	FreqL = (u8)(FreqM * 2.0f/3.0f);
	FreqH = (u8)(FreqM * 1.5f);

	StopVibration();
}

PuruPuruPack::~PuruPuruPack()
{
	if(hVibrationThread)
	{
		TerminateThread(hVibrationThread, 0);
		CloseHandle(hVibrationThread);
		hVibrationThread = NULL;
	}

	StopVibration();
}

////////////////////////////////////////////////////////////////////////
// Dreamcast Controller
////////////////////////////////////////////////////////////////////////

Dreamcast_DeviceInfo ControllerID =
{
	{0x01000000, 0xFE060F00, 0, 0},
	{0xFF, 0},
	"Dreamcast Controller\0",
	"Produced By or Under License From SEGA ENTERPRISES,LTD.\0",
	{0x01AE, 0x01F4},
	"",
};

u32 __fastcall DreamcastController::DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	switch(command)
	{
	case GET_STATUS:
		memcpy(buffer_out, &ControllerID, 112);
		buffer_out_len += 112;
		return RET_STATUS;

	case GET_CONDITION:
		PollOut(buffer_out);
		buffer_out_len += 12;
		return RET_DATA_TRANSFER;

	default:
		printf("Pokopom Controller -> Unknown MAPLE command: %X\n", command);
		return RET_UNKNOWN_COMMAND;
	}

}

void DreamcastController::PollOut(u32 *buffer_out)
{
	Input::DreamcastPoll(buffer_out, set, gamepadPlugged);
}

////////////////////////////////////////////////////////////////////////
// PuruPuru Pack Rumble
////////////////////////////////////////////////////////////////////////

Dreamcast_DeviceInfo RumbleID =
{
	{0x00010000, 0x00000101, 0, 0},
	{0xFF, 0},
	"Puru Puru Pack\0",
	"Produced By or Under License From SEGA ENTERPRISES,LTD.\0",
	{0x00C8, 0x0640},
	"Version 1.000,1998/11/10,315-6211-AH\0",
};

u32 __fastcall PuruPuruPack::DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	switch(command)
	{
	case GET_STATUS:
		memcpy(buffer_out, &RumbleID, 112);
		buffer_out_len += 112;
		return RET_STATUS;

	case GET_STATUS_EX:
		memcpy(buffer_out, &RumbleID, 152);
		buffer_out_len += 152;
		return RET_STATUS_EX;

	case GET_CONDITION:
		buffer_out[0] = 0x00010000;
		buffer_out[1] = rSettings.RAW;
		buffer_out_len += 8;
		return RET_DATA_TRANSFER;

	case GET_MEDIA_INFO:
		buffer_out[0] = 0x00010000;
		buffer_out[1] = rSettings.RAW;
		buffer_out_len += 8;
		return RET_DATA_TRANSFER;

	case BLOCK_READ:
		buffer_out[0] = 0x00010000;
		buffer_out[1] = 0;
		buffer_out[2] = (0x0200 << 16) | (AST << 8);
		buffer_out_len += 12;
		return RET_DATA_TRANSFER;

	case BLOCK_WRITE:
		AST = (buffer_in[2] >> 16) & 0xFF;
		AST_ms = AST * 250 + 250;
		return RET_DEVICE_REPLY;

	case SET_CONDITION:
		rConfig.RAW = buffer_in[1];

		if(rConfig.EXH & rConfig.INH)
			return RET_TRAMSMIT_AGAIN;

		UpdateVibration();
		return RET_DEVICE_REPLY;

	default:
		printf("Pokopom Rumble -> Unknown MAPLE command: %X\n", command);
		return RET_UNKNOWN_COMMAND;
	}
}

void PuruPuruPack::StopVibration()
{
	Input::StopRumble(set.xinputPort, gamepadPlugged);
}

void PuruPuruPack::UpdateVibration()
{
	s16 intensity = (s16)(rConfig.Mpow - rConfig.Ppow);

	if(intensity == 0)
		StopVibration();
	else
		Input::DreamcastRumble(intensity, rConfig.FREQ > FreqH, rConfig.FREQ < FreqL,
			(LPVOID)&thread, set, gamepadPlugged);

}