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

class Controller
{
public: 	
	Controller(_Settings &config, u16 bsize = 9);		

public:
	struct State
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

	virtual void LoadState(State state);
	virtual void SaveState(State &state);

public:			
	u8 command(const u32 counter, const u8 data);
	bool gamepadPlugged;

protected:
	virtual void Cmd0(); // To use the analog toggle or whatever before command
	virtual void Cmd1(const u8 data); // Requires only main command byte to be known
	virtual void Cmd4(const u8 data); // Requires bytes 3 and 4 to be known
	virtual void Cmd8(const u8 data); // Requires 8 bytes from the command to be known

protected:	
	void poll();
	void vibration(u8 smalldata, u8 bigdata);
	void ReadInput(u8 *buffer);
	void SetVibration();
	void Reset();
	void Recheck();

protected:
	_Settings &settings;
	u8 *dataBuffer, *cmdBuffer; 
	const u16 sizeBuffer;
	
protected:	
	u16 buttons, buttonsStick, analogL, analogR;
	u8 padID;	
	bool bConfig, bModeLock;	
	u8 motorMapS, motorMapL;
	u8 triggerL, triggerR;

private:
	Controller(const Controller &);
    Controller& operator=(const Controller &);
};

class Controller2 : public Controller
{
public:
	Controller2(_Settings &config);		

public:
	u8 command(const u32 counter, const u8 data);
	
	void LoadState(State state);
	void SaveState(State &state);

protected:	
	void Cmd1(const u8 data);
	void Cmd4(const u8 data);
	void Cmd8(const u8 data);

protected:
	void ReadInputPressure(u8 *buffer);
	u8 pollMask[6];
	u16 pressureButton[12];

private:
	Controller2(const Controller2 &);
    Controller2& operator=(const Controller2 &);
};

class ControllerGuitar : public Controller2
{
public:
	ControllerGuitar(_Settings &config);

private:
	void Cmd1(const u8 data);
	void Cmd4(const u8 data);

private:
	void ReadInputGuitar(const bool bConfig, u8 *buffer);
};
