#include "General.h"
#include "FileIO.h"
#include "ConfigDialog.h"
#include "Chankast.h"
#include "Input.h"

#ifdef _WIN32

ChankastController * chankastPad[2] = {NULL, NULL};
ChankastPadData chankastData[2];

////////////////////////////////////////////////////////////////////////
// On start?
////////////////////////////////////////////////////////////////////////

s32 InitPads(void* hWnd)
{
	FileIO::INI_LoadSettings();
	FileIO::INI_SaveSettings();

	chankastPad[0] = new ChankastController(0, settings[0]);
	chankastPad[1] = new ChankastController(1, settings[1]);

	memset(chankastData, 0, sizeof(chankastData));

	Input::Pause(false);
	KeepAwake(KEEPAWAKE_INIT);

	return 0;
}

////////////////////////////////////////////////////////////////////////
// ???
////////////////////////////////////////////////////////////////////////

s32 GetNumControllers()
{
	return 2;
}

////////////////////////////////////////////////////////////////////////
// On close?
////////////////////////////////////////////////////////////////////////

void EndPads()
{
	delete chankastPad[0];
	delete chankastPad[1];

	chankastPad[0] = NULL;
	chankastPad[1] = NULL;

	Input::Pause(true);
	KeepAwake(KEEPAWAKE_CLOSE);
}

////////////////////////////////////////////////////////////////////////
// Config!
////////////////////////////////////////////////////////////////////////

s32 ConfigurePads(void* hWnd)
{
	FileIO::INI_LoadSettings();
	CreateConfigDialog();
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Poll
////////////////////////////////////////////////////////////////////////

void ChankastController::PollData(ChankastPadData &Data)
{
	u16 buffer[6];
	Input::DreamcastPoll((u32*)buffer, set, gamepadPlugged);
	memcpy(&Data, &buffer[2], 8);
}

void  UpdateInput()
{
	chankastPad[0]->PollData(chankastData[0]);
	chankastPad[1]->PollData(chankastData[1]);
}

void GetStatusPads(s32 iNumPad, void* _pContCond)
{
	memcpy(_pContCond,&chankastData[iNumPad],sizeof(ChankastPadData));
}

////////////////////////////////////////////////////////////////////////
// Interface stuff
////////////////////////////////////////////////////////////////////////

static ChankastInputInterface ChankastInterface =
{
	MAKEWORD(1,0),
	"Pokopom Interface",
	InitPads,
	GetNumControllers,
	EndPads,
	ConfigurePads,
	UpdateInput,
	GetStatusPads,
};

DllExport ChankastInputInterface *GetInputInterface()
{
  return &ChankastInterface;
}

#endif // Win32
