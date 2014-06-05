/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "Controller.h"
#include "Codes_IDs.h"


extern u8 multitap;

MultiTap::MultiTap(_Settings *config) : PlayStationDevice(config[0], 1)
{
	for(slot = 0; slot < 4; slot++)
	{
		Device[slot] = new DualShock(config[multitap == 2? (slot+1)%4 : slot]);
		Device[slot]->SetPort(multitap == 2? (slot+1)%4 : slot);
	}

	if(multitap == 2) Device[3]->Disable();
}

MultiTap::~MultiTap()
{
	for(int i = 0; i < 4; i++) delete Device[i];
}

u8 MultiTap::command(const u32 _counter, const u8 data)
{
	const u8 counter = _counter & 0xFF;
	//slot = 0;
	u8 output = 0xFF;

	if(counter < 3) switch(counter)
	{
		case 1: output = 0x80; break;
		case 2: output = 0x5A; break;
	}
	else if(counter < 11)
	{
		//slot = 1;
		if(counter == 3) output = Device[0]->command(0, data);
		output = Device[0]->command(counter - 2, data);
	}
	else if(counter < 19)
	{
		//slot = 2;
		if(counter == 11) output = Device[1]->command(0, data);
		output = Device[1]->command(counter - 10, data);
	}
	else if(counter < 27)
	{
		//slot = 3;
		if(counter == 19) output = Device[2]->command(0, data);
		output = Device[2]->command(counter - 18, data);
	}
	else
	{
		//slot = 4;
		if(counter == 27) output = Device[3]->command(0, data);
		output = Device[3]->command(counter - 26, data);
	}

	//if(counter == 0) printf("\n");
	//Debug("[%02d|%d] %02X:%02X\n", counter, slot, data, output);

	return output;
}

void MultiTap::LoadState(PlayStationDeviceState state)
{
}

void MultiTap::SaveState(PlayStationDeviceState &state)
{
}

////////////////////////////////////////////////////////////////////////
// PCSX2 Multitap
////////////////////////////////////////////////////////////////////////

MultiTap2::MultiTap2(_Settings *config) : MultiTap(config)
{}

u8 MultiTap2::command(const u32 counter, const u8 data)
{
	if(counter == 0) slot = data - 1;

	return Device[slot]->command(counter, data);
}