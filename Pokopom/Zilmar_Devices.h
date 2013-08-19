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
