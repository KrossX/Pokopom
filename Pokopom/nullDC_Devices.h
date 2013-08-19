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

class nullDC_Device
{
protected:	
	bool isPlugged;
	unsigned int port;
	_Settings &set;

public:
	nullDC_Device(unsigned int _port, _Settings &config);

public:	
	virtual unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len) { return 0; };
};

class DreamcastController : public nullDC_Device
{
	void PollOut(unsigned int* buffer_out);

public:
	DreamcastController(unsigned int _port, _Settings &config);

public:
	unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);
};

class PuruPuruPack : public nullDC_Device
{

public:
	PuruPuruPack(unsigned int _port, _Settings &config);

public:
	unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);
};