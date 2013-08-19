#pragma once
#include "nullDC_Devices.h"

////////////////////////////////////////////////////////////////////////
// Interface stuff
////////////////////////////////////////////////////////////////////////

struct ChankastInputInterface
{
  int Version;
  char *Description;
  int (__cdecl *Init)(void* hWnd);
  int (__cdecl *GetNumControllers)();
  void (__cdecl *End)();
  int (__cdecl *Configure)(void* hWnd);
  void (__cdecl *Update)();
  void (__cdecl *GetStatus)(int iNumPad, void* _pContCond);  
};


////////////////////////////////////////////////////////////////////////
// Device structure
////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct ChankastPadData
{
  WORD buttons;			/* buttons bitfield */
  BYTE rtrig;			/* right trigger */
  BYTE ltrig;			/* left trigger */
  BYTE joyx;			/* joystick X */
  BYTE joyy;			/* joystick Y */
  BYTE joy2x;			/* second joystick X */
  BYTE joy2y;			/* second joystick Y */
};
#pragma pack(0)

////////////////////////////////////////////////////////////////////////
// Device.. stuff
////////////////////////////////////////////////////////////////////////

class ChankastController : public DreamcastController
{
	ChankastController & operator = (const ChankastController & other) {}

public:
	void PollData(ChankastPadData &Data);
	
	ChankastController(unsigned int _port, _Settings &config) :
		DreamcastController(_port, config) {}
};
