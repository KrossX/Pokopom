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

#include "Zilmar_Controller_Interface.h"
#include "Settings.h"


class Zilmar_Device
{
	

protected:
	const u8 zPort;
	Zilmar::CONTROL &zControl;	
	_Settings &set;

	bool gamepadPlugged;
	void Recheck();
		
public:
	virtual void __fastcall Read(u8 *cmd) = 0;
	virtual void __fastcall Command(u8 *cmd) = 0;
	
	Zilmar_Device(_Settings &settings, Zilmar::CONTROL &control, u8 port);
	Zilmar_Device(const Zilmar_Device&);
	Zilmar_Device& operator=(const Zilmar_Device&);
};

class N64controller : public Zilmar_Device
{

	struct _status { u8 Mode, EEPROM, Plugin; } status;
	union _poll { u8 RAW8[4]; s16 RAW16[2]; } poll;

	void Poll();
	void GetStatus();
	void RumbleIt(bool on);

public:
	void __fastcall Read(u8 *cmd);
	void __fastcall Command(u8 *cmd);
	
	N64controller(_Settings &settings, Zilmar::CONTROL &control, u8 port);
	N64controller(const N64controller&);
	N64controller& operator=(const N64controller&);
};

////////////////////////////////////////////////////////////////////////

enum RAW_COMMAND
{
	RAW_GET_STATUS	= 0x00,
	RAW_READ_KEYS	= 0x01,
	RAW_READ_PACK	= 0x02,
	RAW_WRITE_PACK	= 0x03,
	RAW_READ_ROM	= 0x04,
	RAW_WRITE_ROM	= 0x05,
	RAW_RESET		= 0xFF
};

enum RAW_RETURN
{
	RAW_RET_OK = 0x00,
	RAW_RET_WRONG_SIZE = 0x40,
	RAW_RET_ERROR = 0x80
};
