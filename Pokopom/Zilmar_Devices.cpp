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
	status.Plugin = 0x00;

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
			//zControl.Present = FALSE;
			return;
		}
		else
			zControl.Present = TRUE;
	}

	switch(cmd[2]) // in / out, expected length
	{	
	case RAW_GET_STATUS: // 1 / 3
		GetStatus();
		break;

	case RAW_READ_KEYS: // 1 / 4
		Poll();
		break;

	//case RAW_READ_PACK: break;
	//case RAW_WRITE_PACK: break;
	
	case RAW_READ_ROM: break; // Handled by the emu?
	case RAW_WRITE_ROM: break;// Handled by the emu?
	
	case RAW_RESET: break;
	
	default:
		printf("Pokopom(%d) -> Command\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		cmd[1] |= RAW_RET_ERROR;
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
			//zControl.Present = FALSE;	
			return;
		}
		else
			zControl.Present = TRUE;
	}	

	switch(cmd[2])
	{	
	case RAW_GET_STATUS:
		cmd[3] = status.Mode;
		cmd[4] = status.EEPROM;
		cmd[5] = status.Plugin;
		break;

	case RAW_READ_KEYS:
		cmd[3] = poll.RAW8[0];
		cmd[4] = poll.RAW8[1];
		cmd[5] = poll.RAW8[2];
		cmd[6] = poll.RAW8[3];		
		break;

	//case RAW_READ_PACK: break;
	//case RAW_WRITE_PACK: break;

	case RAW_READ_ROM: break; // Handled by the emu?
	case RAW_WRITE_ROM: break;// Handled by the emu?

	case RAW_RESET:
		cmd[3] = status.Mode;
		cmd[4] = status.EEPROM;
		cmd[5] = status.Plugin;
		break;	

	default:
		printf("Pokopom(%d) -> Read\t%2d %2d %02X\n", zPort, cmd[0], cmd[1], cmd[2]);
		cmd[1] |= RAW_RET_ERROR;
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