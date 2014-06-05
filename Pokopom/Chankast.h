/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once
//#include "nullDC_Devices.h"

#if 0
//#ifdef _WIN32

////////////////////////////////////////////////////////////////////////
// Interface stuff
////////////////////////////////////////////////////////////////////////

struct ChankastInputInterface
{
  s32 Version;
  char *Description;
  s32 (__cdecl *Init)(void* hWnd);
  s32 (__cdecl *GetNumControllers)();
  void (__cdecl *End)();
  s32 (__cdecl *Configure)(void* hWnd);
  void (__cdecl *Update)();
  void (__cdecl *GetStatus)(s32 iNumPad, void* _pContCond);  
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

////////////////////////////////////////////////////////////////////////
// Device.. stuff
////////////////////////////////////////////////////////////////////////

class ChankastController : public DreamcastController
{
	ChankastController & operator = (const ChankastController & other) {}

public:
	void PollData(ChankastPadData &Data);
	
	ChankastController(u32 _port, _Settings &config) :
		DreamcastController(_port, config) {}
};

#endif // Win32
