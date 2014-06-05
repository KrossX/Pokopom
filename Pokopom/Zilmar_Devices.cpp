/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "Zilmar_Devices.h"
#include "Input.h"
#include "FileIO.h"

#ifdef _WIN32

////////////////////////////////////////////////////////////////////////
// General and constructors, etc...
////////////////////////////////////////////////////////////////////////

Zilmar_Device::Zilmar_Device(_Settings &settings, Zilmar::CONTROL &control, u8 port) :
	set(settings), zControl(control), zPort(port)
{
	gamepadPlugged = false;
};

N64mempak::N64mempak(u8 port) : zPort(port)
{
	if(!Load())
		memset(MEMPAK.RAW8, 0xFF, 0x8000);
};

N64controller::N64controller(_Settings &settings, Zilmar::CONTROL &control, u8 port) :
	Zilmar_Device(settings, control, port), mempak(port)
{
	// 0x00 : No plugin
	// 0x01 : Plugin ... plugged
	// 0x02 : Plugin uninitialized
	// 0x04 : Invalid last Pack I/O address
	// 0x80 : EEPROM busy
	status.Plugin = 0x01;

	// 0x80 : EEPROM present
	// 0x00 : No EEPROM
	status.EEPROM = 0x00;

	// 0x01 : Absolute
	// 0x02 : Relative
	// 0x04 : Gamepad
	status.Mode = 0x04 | 0x01;

	bPolled = false;
	bRumble = true;
};

////////////////////////////////////////////////////////////////////////
// Generic
////////////////////////////////////////////////////////////////////////

void Zilmar_Device::Recheck()
{
	gamepadPlugged = Input::Recheck(set.xinputPort);
}

////////////////////////////////////////////////////////////////////////
// N64 Controller
////////////////////////////////////////////////////////////////////////


// DataCRC and basis for READ/WRITE_PACK stuff from Mupen64Plus
// http://code.google.com/p/mupen64plus/
// -. I was getting mad trying to make it work, and I already had
//    the CRC wrong. So yay Mupen and OpenSource-ness! =D


u8 N64mempak::CRC(u8 *data, s32 iLenght)
{
    u8 remainder = data[0];

    s32 iByte = 1;
    u8 bBit = 0;

    while( iByte <= iLenght )
    {
        s32 hBit = ((remainder & 0x80) != 0);
        remainder = remainder << 1;

        remainder += ( iByte < iLenght && data[iByte] & (0x80 >> bBit )) ? 1 : 0;

        remainder ^= (hBit) ? 0x85 : 0;

        bBit++;
        iByte += bBit/8;
        bBit %= 8;
    }

    return remainder;
}

void FASTCALL N64mempak::ReadBlock(u8 *data, u16 address, bool rumble)
{
	u32 *dest = (u32*)data;

	if(rumble)
	{
		dest[0] = dest[1] = dest[2] = dest[3] = //
		dest[4] = dest[5] = dest[6] = dest[7] = address < 0x400 ? 0 : 0x80808080;
	}
	else if(address < 0x400)
	{
		u32 *src = (u32*)MEMPAK.BLOCK[address];

		dest[0] = src[0];	dest[1] = src[1];
		dest[2] = src[2];	dest[3] = src[3];
		dest[4] = src[4];	dest[5] = src[5];
		dest[6] = src[6];	dest[7] = src[7];
	}
	else
	{
		dest[0] = dest[1] = dest[2] = dest[3] = //
		dest[4] = dest[5] = dest[6] = dest[7] = 0;
	}

	data[32] = CRC(data, 32);
}

void FASTCALL N64mempak::WriteBlock(u8 *data, u16 address, bool rumble)
{
	u32 *src = (u32*)data;

	if(rumble)
	{
		// Nothing here!
	}
	else if(address < 0x400)
	{
		u32 *dest = (u32*)MEMPAK.BLOCK[address];

		dest[0] = src[0];	dest[1] = src[1];
		dest[2] = src[2];	dest[3] = src[3];
		dest[4] = src[4];	dest[5] = src[5];
		dest[6] = src[6];	dest[7] = src[7];

		Save();
	}

	data[32] = CRC(data, 32);
}

bool N64mempak::Load() { return FileIO::LoadMempak(MEMPAK.RAW8, zPort); }
void N64mempak::Save() { FileIO::SaveMempak(MEMPAK.RAW8, zPort); }

////////////////////////////////////////////////////////////////////////

void FASTCALL N64controller::Command(u8 *cmd) // Input ?
{
	// cmd[0] - input length in bytes
	// cmd[1] - output length in bytes | also return errors here?
	// cmd[2] - actual command

	if(!gamepadPlugged)
	{
		Recheck();

		if(!gamepadPlugged)
		{
			zControl.Present = FALSE;
			return;
		}
		else
			zControl.Present = TRUE;
	}

	switch(cmd[2]) // in / out, expected length
	{
	case RAW_GET_STATUS: // 1 / 3
		if(cmd[1] != 3) return;
		GetStatus();
		break;

	case RAW_READ_KEYS: // 1 / 4
		if(cmd[1] != 4) return;
		Poll();
		bPolled = true; // for PJ64
		break;

	case RAW_READ_PACK: break; // 3 / 33 ? // Handled on Read

	case RAW_WRITE_PACK: // 35 / 1 ?
		{
			u16 blockAddress = (cmd[3] << 3) | (cmd[4] >> 5);
			//u16 crcAddress = cmd[4] & 0x1F; // Wut for?

			//Debug("Pokopom -> WP\t%4X %4X\n", blockAddress, crcAddress);
			if (bRumble && blockAddress == 0x600) RumbleIt(cmd[5] != 0);
			mempak.WriteBlock(&cmd[5], blockAddress, bRumble);
		}
		break;

	case RAW_READ_ROM: break; // Handled by the emu?
	case RAW_WRITE_ROM: break;// Handled by the emu?

	case RAW_RESET: break;

	default:
		//Debug("Pokopom(%d) -> Command\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		break;
	}

}

void FASTCALL N64controller::Read(u8 *cmd) // Output ?
{
	// cmd[0] - input length in bytes
	// cmd[1] - output length in bytes | also return errors here?
	// cmd[2] - actual command

	if(!gamepadPlugged)
	{
		Recheck();
		if(!gamepadPlugged)
		{
			zControl.Present = FALSE;
			cmd[1] |= RAW_RET_ERROR;
			return;
		}
		else
			zControl.Present = TRUE;
	}

	switch(cmd[2])
	{
	case RAW_GET_STATUS:
		if(cmd[1] != 3) { cmd[1] |= RAW_RET_WRONG_SIZE; return; }
		cmd[3] = status.Mode;
		cmd[4] = status.EEPROM;
		cmd[5] = status.Plugin;
		break;

	case RAW_READ_KEYS:
		if(cmd[1] != 4) { cmd[1] |= RAW_RET_WRONG_SIZE; return; }

		Input::N64rumbleSwitch(set.xinputPort, bRumble, gamepadPlugged);

		if(!bPolled) Poll();	// Let's do a Poll here if there wasn't a command() before.
		bPolled = false;		// For PJ64 that doesn't seem to use command() much.

		cmd[3] = poll.RAW8[0];
		cmd[4] = poll.RAW8[1];
		cmd[5] = poll.RAW8[2];
		cmd[6] = poll.RAW8[3];
		break;

	case RAW_READ_PACK:
		{
			u16 blockAddress = (cmd[3] << 3) | (cmd[4] >> 5);
			//u16 crcAddress = cmd[4] & 0x1F; // Wut for?

			//Debug("Pokopom -> RP\t%4X %4X\n", blockAddress, crcAddress);
			mempak.ReadBlock(&cmd[5], blockAddress, bRumble);
		}break;

	case RAW_WRITE_PACK: break; // Handled on Command

	case RAW_READ_ROM: break; // Handled by the emu?
	case RAW_WRITE_ROM: break;// Handled by the emu?

	case RAW_RESET:
		cmd[3] = status.Mode;
		cmd[4] = status.EEPROM;
		cmd[5] = status.Plugin;
		break;

	default:
		//Debug("Pokopom(%d) -> Read\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		cmd[1] |= RAW_RET_WRONG_SIZE;
	}
}

extern void FASTCALL N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged);

void N64controller::Poll()
{
	Input::N64controllerPoll(poll.RAW8, set, gamepadPlugged);
}

void N64controller::GetStatus()
{
}

void N64controller::RumbleIt(bool on)
{
	Input::N64rumble(on, set, gamepadPlugged);
}

#endif //WIN32
