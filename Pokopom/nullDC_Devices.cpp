#include "nullDC_Devices.h"
#include "General.h"

////////////////////////////////////////////////////////////////////////
// General and constructors
////////////////////////////////////////////////////////////////////////

nullDC_Device::nullDC_Device(unsigned int _port, _Settings &config) : port(_port), set(config)
{
}

DreamcastController::DreamcastController(unsigned int _port, _Settings &config) : nullDC_Device(_port, config)
{
}

PuruPuruPack::PuruPuruPack(unsigned int _port, _Settings &config) : nullDC_Device(_port, config)
{
}

////////////////////////////////////////////////////////////////////////
// Dreamcast Controller
////////////////////////////////////////////////////////////////////////

struct controllerID
{
	unsigned int ID0[4];
	unsigned char ID1[2];
	unsigned char ID2_Name[30];
	unsigned char ID3_Brand[60];
	unsigned short ID4[2];
};

unsigned int ID0[4] = {0x01000000, 0xFE060F00, 0, 0};
unsigned char ID1[2] = {0xFF, 0};
unsigned char ID2_Name[30] = "Dreamcast Controller\0";
unsigned char ID3_Brand[60] = "Produced By or Under License From SEGA ENTERPRISES,LTD.\0";
unsigned short ID4[2] = {0x01AE, 0x01F4};

unsigned int __fastcall DreamcastController::DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len)
{
	switch(command)
	{
	case 0x01:
		{
			memcpy(buffer_out, ID0, 16);		buffer_out+=16;
			memcpy(buffer_out, ID1, 2);			buffer_out+=2;
			memcpy(buffer_out, ID2_Name, 30);	buffer_out+=30;
			memcpy(buffer_out, ID3_Brand, 60);	buffer_out+=60;
			memcpy(buffer_out, ID4, 4);			buffer_out+=4;
			buffer_out_len += 112;
		}
		return 0x05;

	case 0x09:
		PollOut(buffer_out);
		buffer_out_len += 12;
		return 0x08;
	
	default:
		printf("Pokopom -> Unknown MAPLE command: %X\n", command);
		return 0x07;
	}

}


unsigned int __fastcall PuruPuruPack::DMA(void* device_instance, unsigned int command, 
		unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len)
{
	return 00;
}
