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

#include "General.h"
#include "Zilmar_Devices.h"
#include "XInput_Backend.h"

////////////////////////////////////////////////////////////////////////
// General and constructors, etc...
////////////////////////////////////////////////////////////////////////

Zilmar_Device::Zilmar_Device(_Settings &settings, Zilmar::CONTROL &control, u8 port) :
	set(settings), zControl(control), zPort(port)
{
	gamepadPlugged = true;
};

Zilmar_Device::Zilmar_Device(const Zilmar_Device& other) :
	set(other.set), zControl(other.zControl), zPort(other.zPort)
{
}

Zilmar_Device& Zilmar_Device::operator=(const Zilmar_Device&) 
{
	return *this;
}

N64controller::N64controller(_Settings &settings, Zilmar::CONTROL &control, u8 port) :
	Zilmar_Device(settings, control, port)
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
};

N64controller::N64controller(const N64controller& other):
	Zilmar_Device(other.set, other.zControl, other.zPort)
{
}

N64controller& N64controller::operator=(const N64controller&) 
{
	return *this;
}

////////////////////////////////////////////////////////////////////////
// Generic
////////////////////////////////////////////////////////////////////////

void Zilmar_Device::Recheck()
{
	gamepadPlugged = XInput::Recheck(set.xinputPort);
}

////////////////////////////////////////////////////////////////////////
// N64 Controller
////////////////////////////////////////////////////////////////////////


// DataCRC and basis for READ/WRITE_PACK stuff from Mupen64Plus
// http://code.google.com/p/mupen64plus/ 
// -. I was getting mad trying to make it work, and I already had 
//    the CRC wrong. So yay Mupen and OpenSource-ness! =D

static u8 DataCRC(u8 *Data, s32 iLenght)
{
    u8 Remainder = Data[0];

    s32 iByte = 1;
    u8 bBit = 0;

    while( iByte <= iLenght )
    {
        s32 HighBit = ((Remainder & 0x80) != 0);
        Remainder = Remainder << 1;

        Remainder += ( iByte < iLenght && Data[iByte] & (0x80 >> bBit )) ? 1 : 0;

        Remainder ^= (HighBit) ? 0x85 : 0;

        bBit++;
        iByte += bBit/8;
        bBit %= 8;
    }

    return Remainder;
}

void __fastcall N64controller::Command(u8 *cmd) // Input ?
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
		break;

	case RAW_READ_PACK: break; // 3 / 33 ? // Handled on Read		

	case RAW_WRITE_PACK: // 35 / 1 ?
		{
			if (cmd[3] == 0xC0) RumbleIt(cmd[5] != 0);
            cmd[32+5] = DataCRC(&cmd[5], 32);
        }
		break;
	
	case RAW_READ_ROM: break; // Handled by the emu?
	case RAW_WRITE_ROM: break;// Handled by the emu?
	
	case RAW_RESET: break;
	
	default:
		//printf("Pokopom(%d) -> Command\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		break;
	}

}

void __fastcall N64controller::Read(u8 *cmd) // Output ?
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
		
		cmd[3] = poll.RAW8[0];
		cmd[4] = poll.RAW8[1];
		cmd[5] = poll.RAW8[2];
		cmd[6] = poll.RAW8[3];		
		break;

	case RAW_READ_PACK: 
		{
            if((cmd[3] >= 0x80) && (cmd[3] < 0x90))
                memset(&cmd[5], 0x80, 32);
            else
                memset(&cmd[5], 0x00, 32);

            cmd[32+5] = DataCRC(&cmd[5], 32);
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
		//printf("Pokopom(%d) -> Read\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		cmd[1] |= RAW_RET_WRONG_SIZE;
	}
}

extern void __fastcall N64controllerPoll(u8 *outBuffer, _Settings &set, bool &gamepadPlugged);

void N64controller::Poll()
{
	XInput::N64controllerPoll(poll.RAW8, set, gamepadPlugged);
}

void N64controller::GetStatus()
{
}

void N64controller::RumbleIt(bool on)
{
	XInput::N64rumble(on, set, gamepadPlugged);
}