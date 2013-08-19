/**********************************************************************************
Based on...

Common Controller plugin spec, version #1.1 maintained by
zilmar (zilmar@emulation64.com)

All questions or suggestions should go through the emutalk plugin forum.
http://www.emutalk.net/cgi-bin/ikonboard/ikonboard.cgi?s=3bd272222f66ffff;act=SF;f=20
**********************************************************************************/

#include "General.h"
#include "Zilmar_Devices.h"
#include "FileIO.h"
#include "ConfigDialog.h"
//#include "ConsoleOutput.h"
#include "Input.h"

#ifdef _WIN32

Zilmar::CONTROL_INFO * zilmarInfo = NULL;
Zilmar_Device * zController[4] = {NULL, NULL, NULL, NULL};

/************************************************************************************
Function: GetDllInfo

Purpose: This function allows the emulator to gather information about the dll by
		 filling in the PluginInfo structure.

input: A pointer to a PLUGIN_INFO stucture that needs to be filled by the function.
*************************************************************************************/
DllExport void CALL GetDllInfo(Zilmar::PLUGIN_INFO *PluginInfo)
{
	//printf("Pokopom -> GetDLLInfo\n");

	strcpy_s(PluginInfo->Name, "Pokopom XInput Plugin v2.0");
	PluginInfo->Type = Zilmar::PLUGIN_TYPE_CONTROLLER;
	PluginInfo->Version = Zilmar::INTERFACE_VERSION;

	//GimmeConsole();
}

/************************************************************************************
Function: InitiateControllers

Purpose: This function initialises how each of the controllers should be handled.

input: - The handle to the main window. (wut? oh 1.0 stuff)
	   - A controller structure that needs to be filled for the emulator to know
	     how to handle each controller.
*************************************************************************************/
DllExport void CALL InitiateControllers(HWND hMain, Zilmar::CONTROL Controls[4])
{
	// 1.0 seems to be the supported version around.

	//printf("Pokopom -> InitControllers\n");

	zilmarInfo = new Zilmar::CONTROL_INFO;
	zilmarInfo->hMainWindow = hMain;
	zilmarInfo->Controls = Controls;

	FileIO::INI_LoadSettings();

	for(u8 i = 0; i < 4; i++)
	{
		zilmarInfo->Controls[i].Plugin = Zilmar::PLUGIN_RAW;
		zilmarInfo->Controls[i].Present = TRUE;
		zilmarInfo->Controls[i].RawData = TRUE;

		zController[i] = new N64controller(settings[i], zilmarInfo->Controls[i], i);
	}

}

/************************************************************************************
Function: CloseDLL

Purpose: This function is called when the emulator is closing down allowing the dll
		 to de-initialise.
*************************************************************************************/

DllExport void CALL CloseDLL()
{
	//printf("Pokopom -> CloseDLL\n");

	if(zilmarInfo)
	{
		delete zilmarInfo;
		zilmarInfo = NULL;
	}

	for(u8 i = 0; i < 4; i++) if(zController[i])
	{
		delete zController[i];
		zController[i] = NULL;
	}
}

/************************************************************************************
Function: DllConfig

Purpose: This function is optional function that is provided to allow the user to
		 configure the dll.

input: A handle to the window that calls this function.
*************************************************************************************/
DllExport void CALL DllConfig(HWND hParent)
{
	//printf("Pokopom -> DLLConfig\n");

	FileIO::INI_LoadSettings();
	CreateConfigDialog();
}

/************************************************************************************
Function: DllAbout

Purpose: This function is optional function that is provided to give further
		 information about the DLL.

input: A handle to the window that calls this function.
*************************************************************************************/
DllExport void CALL DllAbout(HWND hParent)
{
	MessageBox(hParent, L"Pokopom XInput Plugin - KrossX © 2012", L"About...", MB_OK);
}

/************************************************************************************
Function: DllTest

Purpose: This function is optional function that is provided to allow the user to
		 test the dll.

input: A handle to the window that calls this function.
*************************************************************************************/
DllExport void CALL DllTest(HWND hParent)
{
	MessageBox(hParent, L"Message box test successful!", L"Test...", MB_OK);
}

/************************************************************************************
Function: RomOpen

Purpose:  This function is called when a rom is open. (from the emulation thread)
*************************************************************************************/
DllExport void CALL RomOpen()
{
	//printf("Pokopom -> RomOpen\n");
	//GimmeConsole();
	Input::Pause(false);
	KeepAwake(KEEPAWAKE_INIT);
}

/************************************************************************************
Function: RomClosed

Purpose:  This function is called when a rom is closed.
*************************************************************************************/
DllExport void CALL RomClosed()
{
	//printf("Pokopom -> RomClosed\n");
	Input::Pause(true);
	KeepAwake(KEEPAWAKE_CLOSE);
}


/************************************************************************************
Function: GetKeys

Purpose: To get the current state of the controllers buttons.

input: - Controller Number (0 to 3).
	   - A pointer to a BUTTONS structure to be filled with the controller state.
*************************************************************************************/
DllExport void CALL GetKeys(s32 port, Zilmar::BUTTONS *keys)
{
	// Only raw here...
	//printf("Pokopom -> GetKeys\n");
}

/************************************************************************************
Function: ControllerCommand

Purpose:  To process the raw data that has just been sent to a specific controller.

input: - Controller Number (0 to 3) and -1 signalling end of processing the pif ram.
	   - Pointer of data to be processed.

note: This function is only needed if the DLL is allowing raw data, or the plugin
	  is set to raw. The data that is being processed looks like this:

		initilize controller: 01 03 00 FF FF FF
		read controller:      01 04 01 FF FF FF FF
*************************************************************************************/

DllExport void CALL ControllerCommand(s32 port, u8 *cmd)
{
	if(port < 0 || cmd == NULL) return;
	//printf("Pokopom -> ControllerCommand\t(%2d) | %02X\n", port, cmd[2]);
	zController[port]->Command(cmd);
}

/************************************************************************************
Function: ReadController

Purpose: To process the raw data in the pif ram that is about to be read.

input: - Controller Number (0 to 3) and -1 signalling end of processing the pif ram.
	   - Pointer of data to be processed.

note: This function is only needed if the DLL is allowing raw data.
*************************************************************************************/
DllExport void CALL ReadController(s32 port, u8 *cmd)
{
	if(port < 0 || cmd == NULL) return;
	//printf("Pokopom -> ReadController\t(%2d) | %02X\n", port, cmd[2]);
	zController[port]->Read(cmd);
}

/************************************************************************************
Function: WM_KeyDown

Purpose: To pass the WM_KeyDown message from the emulator to the plugin.

input: wParam and lParam of the WM_KEYDOWN message.
*************************************************************************************/
DllExport void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam)
{
}

/************************************************************************************
Function: WM_KeyUp

Purpose:  To pass the WM_KEYUP message from the emulator to the plugin.

input: wParam and lParam of the WM_KEYDOWN message.
*************************************************************************************/
DllExport void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam)
{
}

#endif //WIN32
