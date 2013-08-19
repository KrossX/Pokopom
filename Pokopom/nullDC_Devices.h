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