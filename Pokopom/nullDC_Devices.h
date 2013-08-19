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

////////////////////////////////////////////////////////////////////////

class nullDC_Device
{
	nullDC_Device & operator = (const nullDC_Device & other) {}

protected:	
	bool isPlugged;
	unsigned int port;
	_Settings &set;	

public:
	nullDC_Device(unsigned int _port, _Settings &config);
	bool gamepadPlugged;

public:	
	virtual unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len) { return 0; };
};

////////////////////////////////////////////////////////////////////////

class DreamcastController : public nullDC_Device
{
	DreamcastController & operator = (const DreamcastController & other) {}

protected:
	void PollOut(unsigned int* buffer_out);

public:
	DreamcastController(unsigned int _port, _Settings &config);

public:
	unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);
};

////////////////////////////////////////////////////////////////////////

union RumbleSettings
{
	unsigned int RAW;

	struct
	{
		unsigned char VSET0;
		unsigned char VSET1;
		unsigned char FM0;
		unsigned char FM1;
	};
		
	struct 
	{
		unsigned VD : 2;
		unsigned VP : 2;
		unsigned VN : 4;
		
		unsigned VA  : 4;
		unsigned OWF : 1;
		unsigned PD  : 1;
		unsigned CV  : 1;
		unsigned PF  : 1;
	};
};

union RumbleConfig
{
	unsigned int RAW;
		
	struct
	{
		unsigned char CTRL;
		unsigned char POW;
		unsigned char FREQ; // Frequency
		unsigned char INQ;  // Inclination
	};

	struct 
	{
		unsigned CNT : 1; // Continuous mode
		unsigned Res : 3; // ...
		unsigned VN  : 4; // Number of sources [1..F]

		unsigned Mpow : 3; // Backward intensity
		unsigned EXH  : 1; // Divergency
		unsigned Ppow : 3; // Forward intensity
		unsigned INH  : 1; // Convergency		
	};
};

////////////////////////////////////////////////////////////////////////

class PuruPuruPack : public nullDC_Device
{
	PuruPuruPack & operator = (const PuruPuruPack & other) {}

	RumbleSettings rSettings;
	RumbleConfig rConfig;
	
	unsigned char AST;
	unsigned char FreqM, FreqL, FreqH;

	void Update();
	
public:
	PuruPuruPack(unsigned int _port, _Settings &config);

public:
	unsigned int __fastcall DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);
};

////////////////////////////////////////////////////////////////////////

struct Dreamcast_DeviceInfo
{
	unsigned int   ID0[4];
	unsigned char  ID1[2];
	unsigned char  ID2_Name[30];
	unsigned char  ID3_Brand[60];
	unsigned short ID4[2];
	unsigned char  ID5_Extra[40];
};

enum DMA_COMMAND
{
	GET_STATUS = 1,
	GET_STATUS_EX,
	GET_CONDITION = 9,
	GET_MEDIA_INFO,
	BLOCK_READ,
	BLOCK_WRITE,
	SET_CONDITION = 14
};

enum DMA_RETURN
{
	RET_STATUS = 0x05,
	RET_STATUS_EX,
	RET_DEVICE_REPLY = 0x07,
	RET_DATA_TRANSFER,
	RET_TRAMSMIT_AGAIN = 0xFC,
	RET_UNKNOWN_COMMAND,
};

