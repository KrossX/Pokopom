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
#include "..\..\Common\TypeDefs.h"
#include "Settings.h"


struct PlayStationDeviceState
{
	char libraryName[25];
	u16 version;
		
	u16 buttons, buttonsStick, analogL, analogR;
	u8 padID;
	bool bConfig, bModeLock;
	u8 motorMapS, motorMapL;
	u8 triggerL, triggerR;

	u8 pollMask[6];
	u16 pressureButton[12];
};

class PlayStationDevice
{
	PlayStationDevice();
	PlayStationDevice(const PlayStationDevice &);
    PlayStationDevice& operator=(const PlayStationDevice &);

	bool disabled;

protected:
	_Settings &settings;

	u8 *dataBuffer, *cmdBuffer; 
	const u16 sizeBuffer;

	bool gamepadPlugged;
	void Recheck();

public:
	void Enable() { disabled = false; }
	void Disable() { disabled = true; }
	
	virtual void LoadState(PlayStationDeviceState state) = 0;
	virtual void SaveState(PlayStationDeviceState &state) = 0;

	virtual u8 command(const u32 counter, const u8 data) = 0;

	PlayStationDevice(_Settings &config, u16 bufferSize); // 9 PS1, 21 PS2
	~PlayStationDevice();
};


class DualShock : public PlayStationDevice
{
	DualShock();
	DualShock(const DualShock &);
    DualShock& operator=(const DualShock &);

protected:
	virtual void Cmd0(); // To use the analog toggle or whatever before command
	virtual void Cmd1(const u8 data); // Requires only main command byte to be known
	virtual void Cmd4(const u8 data); // Requires bytes 3 and 4 to be known
	virtual void Cmd8(const u8 data); // Requires 8 bytes from the command to be known

	void poll();
	void vibration(u8 smalldata, u8 bigdata);
	void ReadInput(u8 *buffer);
	void SetVibration();
	void Reset();

	u16 buttons, buttonsStick, analogL, analogR;
	u8 padID;	
	bool bConfig, bModeLock;
	u8 motorMapS, motorMapL;
	u8 triggerL, triggerR;

public:
	u8 command(const u32 counter, const u8 data);

	void LoadState(PlayStationDeviceState state);
	void SaveState(PlayStationDeviceState &state);

	DualShock(_Settings &config, u16 bsize = 9);
};

class DualShock2 : public DualShock
{
	DualShock2();
	DualShock2(const DualShock2 &);
    DualShock2& operator=(const DualShock2 &);

protected:	
	void Cmd1(const u8 data);
	void Cmd4(const u8 data);
	void Cmd8(const u8 data);

	void ReadInputPressure(u8 *buffer);
	u8 pollMask[6];
	u16 pressureButton[12];

public:
	void LoadState(PlayStationDeviceState state);
	void SaveState(PlayStationDeviceState &state);

	DualShock2(_Settings &config);	
};

class PS2_Guitar : public DualShock2
{
	PS2_Guitar();
	PS2_Guitar(const PS2_Guitar &);
    PS2_Guitar& operator=(const PS2_Guitar &);
	
	void Cmd1(const u8 data);
	void Cmd4(const u8 data);

	void ReadInputGuitar(const bool bConfig, u8 *buffer);

public:
	PS2_Guitar(_Settings &config);
};

class MultiTap : public PlayStationDevice
{
	MultiTap();
	MultiTap(const MultiTap &);
    MultiTap& operator=(const MultiTap &);

	PlayStationDevice * Device[4];	
	u8 slot;

public:
	void LoadState(PlayStationDeviceState state);
	void SaveState(PlayStationDeviceState &state);

	u8 command(const u32 counter, const u8 data);

	MultiTap(_Settings *config);
	~MultiTap();
};