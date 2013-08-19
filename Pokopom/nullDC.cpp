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
#include "Input_Backend.h"

nullDC_Device * ndcDevice[4] = {NULL, NULL, NULL, NULL};
nullDC_Device * ndcSubDevice[4] = {NULL, NULL, NULL, NULL};

nullDC::emu_info nullDCemu;

DllExport void CALLBACK dcGetInterface(nullDC::plugin_interface* info)
{
	//printf("Pokopom -> GetInterface\n");

	info->InterfaceVersion = PLUGIN_I_F_VERSION;
	info->common.InterfaceVersion = MAPLE_PLUGIN_I_F_VERSION;

	wcscpy_s(info->common.Name, L"Pokopom XInput Plugin v2.0 by KrossX");

	// Assign callback functions
	info->common.Load = Load;
	info->common.Unload = Unload;
	info->common.Type = nullDC::Plugin_Maple;
	info->maple.CreateMain = CreateMain;
	info->maple.CreateSub = CreateSub;
	info->maple.Init = Init;
	info->maple.Term = Term;
	info->maple.Destroy = Destroy;

	u8 id = 0;

	wcscpy_s(info->maple.devices[id].Name, L"Pokopom Dreamcast Controller");
	info->maple.devices[id].Type = nullDC::MDT_Main;
	info->maple.devices[id].Flags = nullDC::MDTF_Sub0 | nullDC::MDTF_Sub1 | nullDC::MDTF_Hotplug;
	id++;

	wcscpy_s(info->maple.devices[id].Name, L"Pokopom Rumble Pack");
	info->maple.devices[id].Type = nullDC::MDT_Sub;
	info->maple.devices[id].Flags = nullDC::MDTF_Hotplug;

	id++;//EOL marker
	info->maple.devices[id].Type = nullDC::MDT_EndOfList;
}

////////////////////////////////////////////////////////////////////////
// Common functions
////////////////////////////////////////////////////////////////////////

s32 FASTCALL Load(nullDC::emu_info* emu)
{
	//printf("Pokopom -> Load\n");
	if(emu == NULL) return nullDC::rv_error;
	memcpy(&nullDCemu, emu, sizeof(nullDCemu));

	FileIO::INI_LoadSettings();
	FileIO::INI_SaveSettings();

	return nullDC::rv_ok;
}

void FASTCALL Unload()
{
	for(s32 i = 0; i < 4; i++)
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

s32 FASTCALL CreateMain(nullDC::maple_device_instance* inst, u32 id, u32 flags, u32 rootmenu)
{
	//printf("Pokopom -> CreateMain [%X|%X]\n", inst->port, id);

	u32 port = (inst->port >> 6);

	ndcDevice[port] = new DreamcastController(port, settings[port]);

	inst->data = inst;
	inst->dma = ControllerDMA;

	nullDCemu.AddMenuItem(rootmenu, -1, L"Pokopom Dreamcast Controller", 0, 0);

	WCHAR temp[512];
	swprintf(temp, sizeof(temp), L"Player %d settings...", (inst->port >> 6) + 1);
	u32 hMenu = nullDCemu.AddMenuItem(rootmenu, -1, temp, ConfigMenuCallback, 0);

	nullDC::MenuItem menuItem;
	menuItem.PUser = inst;
	nullDCemu.SetMenuItem(hMenu, &menuItem, nullDC::MIM_PUser);

	return nullDC::rv_ok;
}

s32 FASTCALL CreateSub(nullDC::maple_subdevice_instance* inst, u32 id, u32 flags, u32 rootmenu)
{
	//printf("Pokopom -> CreateSub [%X|%X]\n", inst->port, id);
	s32 port = inst->port>>6;

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

s32 FASTCALL Init(void* data, u32 id, nullDC::maple_init_params* params)
{
	//u32 port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Init [%d]\n", port);

	if(bKeepAwake)
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

	Input::Pause(false);

	return nullDC::rv_ok;
}

void FASTCALL Term(void* data, u32 id)
{
	//u32 port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Term [%d]\n", port);

	if(bKeepAwake)
		SetThreadExecutionState(ES_CONTINUOUS);

	Input::Pause(true);
}

void FASTCALL Destroy(void* data, u32 id)
{
	//u32 port = ((nullDC::maple_device_instance*)data)->port >> 6;
	//printf("Pokopom -> Destroy [%d]\n", port);
}

////////////////////////////////////////////////////////////////////////
// Commands and stuff
////////////////////////////////////////////////////////////////////////

u32 FASTCALL ControllerDMA(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	if(bKeepAwake) mouse_event( MOUSEEVENTF_MOVE, 0, 0, 0, NULL);

	u32 port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	return ndcDevice[port]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

u32 FASTCALL RumbleDMA(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	u32 port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	return ndcSubDevice[port]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

////////////////////////////////////////////////////////////////////////
// The Config Menu call... figures.
////////////////////////////////////////////////////////////////////////

void EXPORT_CALL ConfigMenuCallback(u32 id, void* w, void* p)
{
	FileIO::INI_LoadSettings();
	CreateConfigDialog();
}
