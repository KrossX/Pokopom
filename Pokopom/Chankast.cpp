#include "General.h"
#include "FileIO.h"
#include "ConfigDialog.h"
#include "Chankast.h"

extern HINSTANCE hInstance;
extern _Settings settings[4];

ChankastController * chankastPad[2] = {NULL, NULL};
ChankastPadData chankastData[2];

////////////////////////////////////////////////////////////////////////
// On start?
////////////////////////////////////////////////////////////////////////

int InitPads(void* hWnd)
{
	INI_LoadSettings();
	INI_SaveSettings();

	chankastPad[0] = new ChankastController(0, settings[0]);
	chankastPad[1] = new ChankastController(1, settings[1]);

	memset(chankastData, 0, sizeof(chankastData));
	
	return 0;
}

////////////////////////////////////////////////////////////////////////
// ???
////////////////////////////////////////////////////////////////////////

int GetNumControllers()
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
}

////////////////////////////////////////////////////////////////////////
// Config!
////////////////////////////////////////////////////////////////////////

int ConfigurePads(void* hWnd)
{
	INI_LoadSettings();
	CreateDialogs(hInstance, GetActiveWindow());
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Poll
////////////////////////////////////////////////////////////////////////

void  UpdateInput()
{
	chankastPad[0]->PollData(chankastData[0]);
	chankastPad[1]->PollData(chankastData[1]);
}

void GetStatusPads(int iNumPad, void* _pContCond)
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

extern "C" ChankastInputInterface __declspec(dllexport) *GetInputInterface()
{
  return &ChankastInterface;
}