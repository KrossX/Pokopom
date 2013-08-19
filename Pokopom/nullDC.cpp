/*  Pokopom - Input Plugin for PSX/PS2 Emulators (now nullDC too)
 *  - 2012  KrossX
 *	
 *	Content of this file is based on code from 
 *	nullDC http://code.google.com/p/nulldc/
 *
 *  Licenced under GNU GPL v3
 *  http://www.gnu.org/licenses/gpl.html
 */

#include "nullDC.h"
#include "nullDC_Devices.h"
#include "FileIO.h"
#include "ConfigDialog.h"

nullDC_Device * ndcDevice[4] = {NULL, NULL, NULL, NULL};
nullDC_Device * ndcSubDevice[4] = {NULL, NULL, NULL, NULL};

nullDC::emu_info nullDCemu;
extern _Settings settings[4];
extern HINSTANCE hInstance;
extern bool bKeepAwake;
bool is_nullDC = false;

void CALLBACK dcGetInterface(nullDC::plugin_interface* info)
{
	//printf("Pokopom -> GetInterface\n");	
	
	info->InterfaceVersion = PLUGIN_I_F_VERSION;
	info->common.InterfaceVersion = MAPLE_PLUGIN_I_F_VERSION;
	
	is_nullDC = true;
	wcscpy_s(info->common.Name, L"Pokopom XInput Plugin");

	// Assign callback functions
	info->common.Load = Load;
	info->common.Unload = Unload;
	info->common.Type = nullDC::Plugin_Maple;
	info->maple.CreateMain = CreateMain;
	info->maple.CreateSub = CreateSub;
	info->maple.Init = Init;
	info->maple.Term = Term;
	info->maple.Destroy = Destroy;
		
	unsigned char id = 0;

	wcscpy_s(info->maple.devices[id].Name, L"Pokopom Dreamcast Controller");	
	info->maple.devices[id].Type = nullDC::MDT_Main;	
	info->maple.devices[id].Flags = nullDC::MDTF_Sub0 | nullDC::MDTF_Sub1 | nullDC::MDTF_Hotplug;
	id++;
	/*
	wcscpy_s(info->maple.devices[id].Name, L"Pokopom Rumble");
	info->maple.devices[id].Type = nullDC::MDT_Sub;
	info->maple.devices[id].Flags = nullDC::MDTF_Hotplug;	
	*/
	id++;//EOL marker
	info->maple.devices[id].Type = nullDC::MDT_EndOfList;
}

////////////////////////////////////////////////////////////////////////
// Common functions
////////////////////////////////////////////////////////////////////////

int FASTCALL Load(nullDC::emu_info* emu)
{	
	//printf("Pokopom -> Load\n");
	if(emu == NULL) return nullDC::rv_error;
	memcpy(&nullDCemu, emu, sizeof(nullDCemu));
	
	INI_LoadSettings();
	INI_SaveSettings();
	
	return nullDC::rv_ok;
}

void FASTCALL Unload()
{
	for(int i = 0; i < 4; i++)
	{
		if(ndcDevice[i] != NULL)
		{
			delete ndcDevice[i];
			ndcDevice[i] = NULL;
		}

		if(ndcSubDevice[i] != NULL)
		{
			delete ndcSubDevice[i];
			ndcSubDevice[i] = NULL;
		}

	}

}

////////////////////////////////////////////////////////////////////////
// Create devices
////////////////////////////////////////////////////////////////////////

int FASTCALL CreateMain(nullDC::maple_device_instance* inst, unsigned int id, unsigned int flags, unsigned int rootmenu)
{
	//printf("Pokopom -> CreateMain [%X|%X]\n", inst->port, id);

	unsigned int port = (inst->port >> 6);

	ndcDevice[port] = new DreamcastController(port, settings[port]);

	inst->data = inst;
	inst->dma = ControllerDMA;

	nullDCemu.AddMenuItem(rootmenu, -1, L"Pokopom Dreamcast Controller", 0, 0);
		
	WCHAR temp[512];
	swprintf(temp, sizeof(temp), L"Player %d settings...", (inst->port >> 6) + 1);	
	unsigned int hMenu = nullDCemu.AddMenuItem(rootmenu, -1, temp, ConfigMenuCallback, 0);

	nullDC::MenuItem menuItem;
	menuItem.PUser = inst;
	nullDCemu.SetMenuItem(hMenu, &menuItem, nullDC::MIM_PUser);
	
	return nullDC::rv_ok;
}

int FASTCALL CreateSub(nullDC::maple_subdevice_instance* inst, unsigned int id, unsigned int flags, unsigned int rootmenu)
{		
	//printf("Pokopom -> CreateSub [%X|%X]\n", inst->port, id);
	int port = inst->port>>6;

	if ((inst->port&3) != 2) 
	{
		printf("Pokopom -> Better plug the Rumble in Slot 2 only!\n", inst->port, id);
	}

	ndcSubDevice[port] = new PuruPuruPack(port, settings[port]);

	inst->data = inst;
	inst->dma  = RumbleDMA;

	nullDCemu.AddMenuItem(rootmenu, -1, L"PuruPuru Rumble Device", 0, 0);
	

	return nullDC::rv_ok;
}

////////////////////////////////////////////////////////////////////////
// Emulation start, stop... quit?
////////////////////////////////////////////////////////////////////////

int FASTCALL Init(void* data, unsigned int id, nullDC::maple_init_params* params)
{
	if(bKeepAwake)
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
	
	//unsigned int port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Init [%d]\n", port);
	return nullDC::rv_ok;
}

void FASTCALL Term(void* data, unsigned int id)
{
	if(bKeepAwake)
		SetThreadExecutionState(ES_CONTINUOUS);
	
	//unsigned int port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Term [%d]\n", port);
}

void FASTCALL Destroy(void* data, unsigned int id)
{
	//unsigned int port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Destroy [%d]\n", port);
}

////////////////////////////////////////////////////////////////////////
// Commands and stuff
////////////////////////////////////////////////////////////////////////

unsigned int FASTCALL ControllerDMA(void* device_instance, unsigned int command, 
	unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len)
{
	if(bKeepAwake) mouse_event( MOUSEEVENTF_MOVE, 0, 0, 0, NULL);
	
	unsigned int port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	return ndcDevice[port]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

unsigned int FASTCALL RumbleDMA(void* device_instance, unsigned int command, 
	unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len)
{
	unsigned int port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	return ndcSubDevice[port]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

////////////////////////////////////////////////////////////////////////
// The Config Menu call... figures.
////////////////////////////////////////////////////////////////////////

void EXPORT_CALL ConfigMenuCallback(unsigned int id, void* w, void* p)
{
	INI_LoadSettings();
	CreateDialogs(hInstance, GetActiveWindow());
}
