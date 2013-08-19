#include "General.h"
#include "nullDC_Devices.h"
#include "Input.h"
#include "FileIO.h"

#ifdef _WIN32

////////////////////////////////////////////////////////////////////////
// General and constructors
////////////////////////////////////////////////////////////////////////

nullDC_Device::nullDC_Device(u32 _port, _Settings &config) : port(_port), set(config)
{}

DreamcastController::DreamcastController(u32 _port, _Settings &config) : nullDC_Device(_port, config)
{}

NaomiController::NaomiController(u32 _port, _Settings &config) : nullDC_Device(_port, config)
{
	FileIO::LoadEEPROM(EEPROM);
}

PuruPuruPack::PuruPuruPack(u32 _port, _Settings &config) : nullDC_Device(_port, config)
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

u32 FASTCALL DreamcastController::DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	switch(command)
	{
	case GET_STATUS:
		memcpy(buffer_out, &ControllerID, 112);
		buffer_out_len = 112;
		return RET_STATUS;

	case GET_CONDITION:
		PollOut(buffer_out);
		buffer_out_len = 12;
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
// NAOMI JAMMA
////////////////////////////////////////////////////////////////////////

const char NAOMI_ID[0x100] = "315-6149    COPYRIGHT SEGA E\x83\x00\x20\x05NTERPRISES CO,LTD.  ";
const char NAOMI_ID2[0x66] = "SEGA ENTERPRISES,LTD.; I/O 838-13683B; Ver1.07; 99/06";

const u8 NAOMI_CHECK[6] = {0x8E, 0x01, 0x00, 0xFF, 0xE0, 0x01};
const u8 NAOMI_FEATURES[16] = {1,2,11,0,  2,2,0,0,  3,8,0,0,  0,0,0,0};

Dreamcast_DeviceInfo NaomiID =
{
	{0x01000000, 0xFE060F00, 0, 0},
	{0xFF, 0},
	"NAOMI Controller\0",
	"Produced By or Under License From SEGA ENTERPRISES,LTD.\0",
	{0x01AE, 0x01F4},
	"",
};

void FASTCALL NaomiController::SetState(u8 *in, u8 mode)
{
	State.mode = mode;
	State.cmd  = in[8];
	State.node = in[9];
}

u32 FASTCALL NaomiController::DMA(void* device_instance, u32 command,
		u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	u8 *bufferIn = (u8*)buffer_in;
	buffer_out_len = 0;

	static long long counter = 0;

	//Debug("Pokopom NAOMI -> Cmd %2X / Jcmd %2X | State cmd %2X, mode %2X, node %2X | %lX\n", 
	//	command, bufferIn[0], State.cmd, State.mode, State.node, counter++);

	switch(command)
	{
	case GET_STATUS:
		memcpy(buffer_out, &NaomiID, 112);
		buffer_out_len = 112;
		return RET_STATUS;

	case GET_CONDITION:
		buffer_out[0] = 0x01000000;
		buffer_out[1] = 0x0000FFFF;
		buffer_out[2] = 0x80808080;
		buffer_out_len = 12;
		return RET_DATA_TRANSFER;

	case JAMMA_ID:
		memcpy(buffer_out, NAOMI_ID, 0x100);
		buffer_out_len = 0x100;
		return RET_JAMMA_ID;

	case JAMMA_CMD:
		{
			u8 subCmd = buffer_in[0] & 0xFF;
			u8 *bufferOut = (u8*)buffer_out;

			switch(subCmd)
			{
			case JAMMA_EEPROM_READ:
				memcpy(bufferOut, &EEPROM[bufferIn[1]], 0x80);
				buffer_out_len = 0x80;
				return RET_DATA_TRANSFER;

			case JAMMA_EEPROM_WRITE:
				memcpy(&EEPROM[bufferIn[1]], &bufferIn[4], bufferIn[2]);
				FileIO::SaveEEPROM(EEPROM);
				return RET_DEVICE_REPLY;

			case JAMMA_GET_CAPS:
			case JAMMA_GET_DATA:
				buffer_out[0] = 0xFFFFFFFF;
				buffer_out[1] = 0xFFFFFFFF;
				buffer_out_len = 8;

				if(GetAsyncKeyState(0x31)) buffer_out[0]&=~(1<<0x1b);
				if(GetAsyncKeyState(0x32)) buffer_out[0]&=~(1<<0x1a);

				if(subCmd == JAMMA_GET_CAPS && State.mode == 0)
				{
					memcpy(&bufferOut[18], NAOMI_CHECK, 5);
					bufferOut[25] = NAOMI_CHECK[5];
					buffer_out_len = 26;
					
					switch(State.cmd)
					{
					case STATE_NAME:
						bufferOut[24] = 57;
						memcpy(&bufferOut[27], NAOMI_ID2, 0x66);
						buffer_out_len = 128;
						break;

					case STATE_CMD_VER:
						bufferOut[27] = 0x11;
						buffer_out_len = 28;
						break;

					case STATE_JAMMA_VER:
						bufferOut[27] = 0x20;
						buffer_out_len = 28;
						break;

					case STATE_COM_VER:
						bufferOut[27] = 0x10;
						buffer_out_len = 28;
						break;

					case STATE_FEATURES: 
						bufferOut[20] = 0;
						bufferOut[26] = 1;

						memcpy(&bufferOut[27], NAOMI_FEATURES, 16);
						buffer_out_len = 44;
						break;

					case 0xF0: break;
					case 0xF1: break;
					case 0xF2: break;

					default:
						printf("Pokopom NAOMI -> Unknown STATE command: %X\n", State.cmd);
						return RET_UNKNOWN_COMMAND;
					}
				}
				else if(State.mode != 0 || subCmd == JAMMA_GET_DATA)
				{
					memcpy(&bufferOut[18], NAOMI_CHECK, 5);
					memset(&bufferOut[38], 0x00, 9);
					bufferOut[25] = NAOMI_CHECK[5];
					PollOut(buffer_out);

					if(State.mode == 1)
					{
						bufferOut[21]=24;
						bufferOut[24]=19;
					}
					else
					{
						bufferOut[21]=16;
						bufferOut[24]=17;
					}

					buffer_out_len = 60;
					//buffer_out_len = 47;
				}
				
				return RET_DATA_TRANSFER;

			case JAMMA_SUBDEVICE:
				SetState(bufferIn, 0);
				return RET_DEVICE_REPLY;

			case JAMMA_TRANSFER_REPEAT:
				SetState(bufferIn, 2);
				return RET_DEVICE_REPLY;

			case JAMMA_TRANSFER:
				SetState(bufferIn, 1);
				return RET_DEVICE_REPLY;

			case 0x31:
				buffer_out[0] = 0xFFFFFFFF;
				buffer_out[1] = 0xFFFFFFFF;
				buffer_out_len = 8;
				return RET_DATA_TRANSFER;

			default:
				printf("Pokopom NAOMI -> Unknown JAMMA command: %X\n", subCmd);
				return RET_UNKNOWN_COMMAND;
			}
		}
		

	default:
		printf("Pokopom NAOMI -> Unknown MAPLE command: %X\n", command);
		return RET_UNKNOWN_COMMAND;
	}

}

void NaomiController::PollOut(u32 *buffer_out)
{
	Input::NaomiPoll(buffer_out, set, gamepadPlugged);
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

u32 FASTCALL PuruPuruPack::DMA(void* device_instance, u32 command,
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
	Input::StopRumble(set.xinputPort);
}

void PuruPuruPack::UpdateVibration()
{
	s16 intensity = (s16)(rConfig.Mpow - rConfig.Ppow);

	if(intensity == 0)
		StopVibration();
	else
	{
		if(rConfig.CNT)
			printf("Pokopom Rumble -> Continuous mode not implemented!\n");

		Input::DreamcastRumble(intensity, rConfig.FREQ > FreqH, rConfig.FREQ < FreqL,
			Watchdog_ms, set, gamepadPlugged, hVibrationThread);
	}
}

#endif // WIN32
