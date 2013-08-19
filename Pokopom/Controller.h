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
#include "Settings.h"

class Controller
{
public: 	
	Controller(_Settings &config, unsigned short bsize = 9);		

public:			
	unsigned char command(const unsigned int counter, const unsigned char data);		
	bool gamepadPlugged;

protected:
	virtual void Cmd0(); // To use the analog toggle or whatever before command
	virtual void Cmd1(const unsigned char data); // Requires only main command byte to be known
	virtual void Cmd4(const unsigned char data); // Requires bytes 3 and 4 to be known
	virtual void Cmd8(const unsigned char data); // Requires 8 bytes from the command to be known

protected:	
	void poll();
	void vibration(unsigned char smalldata, unsigned char bigdata);
	void ReadInput(unsigned char *buffer);
	void SetVibration();
	void Reset();
	void Recheck();

protected:
	_Settings &settings;
	unsigned char *dataBuffer, *cmdBuffer; 
	const unsigned short sizeBuffer;
	
protected:	
	unsigned short buttons, buttonsStick, analogL, analogR;
	unsigned char padID;	
	bool bConfig, bModeLock;	
	unsigned char motorMapS, motorMapL;
	unsigned char triggerL, triggerR;

private:
	Controller(const Controller &);
    Controller& operator=(const Controller &);
};

class Controller2 : public Controller
{
public:
	Controller2(_Settings &config);		

public:
	unsigned char command(const unsigned int counter, const unsigned char data);	

protected:	
	void Cmd1(const unsigned char data);
	void Cmd4(const unsigned char data);
	void Cmd8(const unsigned char data);

protected:
	void ReadInputPressure(unsigned char *buffer);
	unsigned char pollMask[6];
	unsigned short pressureButton[12];

private:
	Controller2(const Controller2 &);
    Controller2& operator=(const Controller2 &);
};

class ControllerGuitar : public Controller2
{
public:
	ControllerGuitar(_Settings &config);

private:
	void Cmd1(const unsigned char data);
	void Cmd4(const unsigned char data);

private:
	void ReadInputGuitar(const bool bConfig, unsigned char *buffer);
};
