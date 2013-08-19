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
#include "Input.h"

#ifdef _WIN32

u32 ndcVersion = 0;
u8 dcPlatform = 0; // Default Dreamcast

wchar_t pluginName[] = L"Pokopom XInput Plugin v2.0 by KrossX ";
wchar_t dcController[] = L"Pokopom Dreamcast Controller";
wchar_t naomiJAMMA[] = L"Pokopom NAOMI JAMMA Controller";
wchar_t puruPakku[] = L"Pokopom Rumble Pack";

nullDC_Device * ndcDevice[4] = {NULL};
nullDC_Device * ndcSubDevice[4][5] = {NULL};

nullDC::emu_info nullDCemu;

static inline u32 Pokopom_MakeVersion(u8 platform, u8 major, u8 minor, u8 build)
{
	 return (((platform)<<24)|((build)<<16)|((minor)<<8)|(major));
}

static u32 FASTCALL dcGetInterfaceVersion()
{
	HINSTANCE drkPlugin = LoadLibrary(L"drkMapleDevices_Win32.dll");

	if(drkPlugin)
	{	
		typedef void (CALLBACK* drkPlugin_t)(void* info);
		drkPlugin_t drkInterface = (drkPlugin_t) GetProcAddress(drkPlugin, "dcGetInterface");

		u32 drkInfo[1024];
		drkInterface(drkInfo);
		FreeLibrary(drkPlugin);

		FILE *drkdump = fopen("drkdump.txt", "w");

		if(drkdump)
		{
			fwrite(drkInfo, 1, 1024 * 4, drkdump);
			fclose(drkdump);
		}

		if(drkInfo[0]) return drkInfo[0];
	}

	return PLUGIN_I_F_VERSION;
}

static void ndcInterfaceDevices(nullDC::maple_device_definition *devices)
{
	switch(dcPlatform)
	{
	case DC_PLATFORM_NORMAL:
		wcscpy_s(devices[NDC0_DREAMCAST_CONTROLLER].Name, dcController);
		devices[NDC0_DREAMCAST_CONTROLLER].Type = nullDC::MDT_Main;
		devices[NDC0_DREAMCAST_CONTROLLER].Flags = nullDC::MDTF_Sub0 | nullDC::MDTF_Sub1 | nullDC::MDTF_Hotplug;

		wcscpy_s(devices[NDC0_PURUPURU_PAKKU].Name, puruPakku);
		devices[NDC0_PURUPURU_PAKKU].Type = nullDC::MDT_Sub;
		devices[NDC0_PURUPURU_PAKKU].Flags = nullDC::MDTF_Hotplug;

		devices[NDC0_ENDOFLIST].Type = nullDC::MDT_EndOfList;
		break;

	case DC_PLATFORM_NAOMI:
		wcscpy_s(devices[NDC2_NAOMI_JAMMA].Name, naomiJAMMA);
		devices[NDC2_NAOMI_JAMMA].Type = nullDC::MDT_Main;
		devices[NDC2_NAOMI_JAMMA].Flags = 0;

		devices[NDC2_ENDOFLIST].Type = nullDC::MDT_EndOfList;
		break;

	default:
		printf("Pokopom -> Error! Unknown Platform: %d", dcPlatform);
	}
}

static void ndcInterface103(nullDC_103::plugin_interface *info)
{
	wcscpy_s(info->common.Name, pluginName);

	info->common.Load = Load;
	info->common.Unload = Unload;
	info->common.Type = nullDC::Plugin_Maple;
	info->maple.CreateMain = CreateMain;
	info->maple.CreateSub = CreateSub;
	
	info->InterfaceVersion = Pokopom_MakeVersion(dcPlatform, 1, 0, 1);
	info->common.InterfaceVersion = Pokopom_MakeVersion(dcPlatform, 1, 0, 0);
	info->common.Unknown = Pokopom_MakeVersion(dcPlatform, 1, 0, 0);

	info->maple.InitMain = InitMain;
	info->maple.TermMain = TermMain;
	info->maple.DestroyMain = DestroyMain;

	info->maple.InitSub = InitMain;
	info->maple.TermSub = TermMain;
	info->maple.DestroySub = DestroyMain;

	ndcInterfaceDevices(info->maple.devices);
};

static void ndcInterface104(nullDC::plugin_interface *info)
{
	wcscpy_s(info->common.Name, pluginName);
	info->common.Load = Load;
	info->common.Unload = Unload;
	info->common.Type = nullDC::Plugin_Maple;

	info->InterfaceVersion = Pokopom_MakeVersion(dcPlatform, 1, 0, 2);
	info->common.InterfaceVersion = Pokopom_MakeVersion(dcPlatform, 1, 0, 0);

	info->maple.CreateMain = CreateMain;
	info->maple.CreateSub = CreateSub;
	info->maple.Init = Init;
	info->maple.Term = Term;
	info->maple.Destroy = Destroy;

	ndcInterfaceDevices(info->maple.devices);
};

/*
Pokopom -> dcGetInterface: 01000001 // 1.0.0 Dreamcast
Pokopom -> dcGetInterface: 00010001 // 1.0.3 Dreamcast
Pokopom -> dcGetInterface: 02010001 // 1.0.3 NAOMI
Pokopom -> dcGetInterface: 00020001 // 1.0.4 Dreamcast
Pokopom -> dcGetInterface: 02020001 // 1.0.4 NAOMI
*/

DllExport void CALLBACK dcGetInterface(void* data)
{
	//Debug("Pokopom -> GetInterface\n");
	u32 ndcData = dcGetInterfaceVersion();
	dcPlatform = (ndcData >> 24) & 0xFF;
	ndcVersion = (ndcData >> 16) & 0xFF;

	printf("Pokopom -> %s: %08X\n", __FUNCTION__, ndcData);
	
	switch(ndcVersion)
	{
	case nullDC_VER_103: ndcInterface103((nullDC_103::plugin_interface*)data); break;
	case nullDC_VER_104: ndcInterface104((nullDC::plugin_interface*)data); break;
	
	default:
		printf("Pokopom -> Error! Unknown nullDC Version: %08X", ndcData);
	};


}

////////////////////////////////////////////////////////////////////////
// Common functions
////////////////////////////////////////////////////////////////////////

s32 FASTCALL Load(nullDC::emu_info* emu)
{
	DebugFunc();

	if(emu == NULL) return nullDC::rv_error;
	memcpy(&nullDCemu, emu, sizeof(nullDCemu));

	FileIO::INI_LoadSettings();
	FileIO::INI_SaveSettings();

	return nullDC::rv_ok;
}

void FASTCALL Unload()
{
	DebugFunc();

	for(u8 i = 0; i < 4; i++)
	{
		if(ndcDevice[i] != NULL)
		{
			delete ndcDevice[i];
			ndcDevice[i] = NULL;
		}

		for(u8 j = 0; j < 5; j++)
		{
			if(ndcSubDevice[i][j] != NULL)
			{
				delete ndcSubDevice[i][j];
				ndcSubDevice[i][j] = NULL;
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////
// Create devices
////////////////////////////////////////////////////////////////////////

s32 FASTCALL CreateMain(nullDC::maple_device_instance* inst, u32 id, u32 flags, u32 rootmenu)
{
	DebugPrint("[%X|%X]", inst->port >> 6, id);

	u32 port = (inst->port >> 6);

	switch(dcPlatform)
	{
	case DC_PLATFORM_NORMAL:
		switch(id)
		{
		case NDC0_DREAMCAST_CONTROLLER:
			ndcDevice[port] = new DreamcastController(port, settings[port]);
			nullDCemu.AddMenuItem(rootmenu, -1, dcController, 0, 0);
			break;

		default:
			ndcDevice[port] = NULL;
			printf("Pokopom -> Warning! Unknown Device: %d [%d]", id, port);
			return nullDC::rv_serror;
		}
		break;

	case DC_PLATFORM_NAOMI:
		switch(id)
		{
		case NDC2_NAOMI_JAMMA:
			ndcDevice[port] = new NaomiController(port, settings[port]);
			nullDCemu.AddMenuItem(rootmenu, -1, naomiJAMMA, 0, 0);
			break;

		default:
			ndcDevice[port] = NULL;
			printf("Pokopom -> Warning! Unknown Device: %d [%d]", id, port);
			return nullDC::rv_serror;
		}
		break;

	default:
			ndcDevice[port] = NULL;
			printf("Pokopom -> Warning! Unknown Platform: %d", dcPlatform);
			return nullDC::rv_serror;
	}

	inst->data = inst;

	switch(ndcVersion)
	{
	case nullDC_VER_103: 
		((nullDC_103::maple_device_instance*)inst)->dma = MainDMA_103;
		break;

	case nullDC_VER_104: 
		inst->dma  = MainDMA;
		break;
	};


	WCHAR temp[512];
	swprintf(temp, sizeof(temp), L"Player %d settings...", (inst->port >> 6) + 1);
	u32 hMenu = nullDCemu.AddMenuItem(rootmenu, -1, temp, ConfigMenuCallback, 0);

	nullDC::MenuItem menuItem;
	menuItem.PUser = inst;
	nullDCemu.SetMenuItem(hMenu, &menuItem, nullDC::MIM_PUser);

	return nullDC::rv_ok;
}

u8 GetSubport(u8 port)
{
	switch(port)
	{
	case 1: return 0;
	case 2: return 1;
	case 4: return 2;
	case 8: return 3;
	case 10: return 4; // bug?
	case 16: return 4;
	default: return 0xFF;
	}
}

s32 FASTCALL CreateSub(nullDC::maple_subdevice_instance* inst, u32 id, u32 flags, u32 rootmenu)
{
	u8 port = inst->port>>6;
	u8 subport = GetSubport(inst->port);
	DebugPrint("[%X|%X|%X]", port, subport, id);

	switch(dcPlatform)
	{
	case DC_PLATFORM_NORMAL:
		switch(id)
		{
		case NDC0_PURUPURU_PAKKU:
			if (subport != 1)
				printf("Pokopom -> Better plug the Rumble in Slot 2 only!\n");

			ndcSubDevice[port][subport] = new PuruPuruPack(port, settings[port]);
			nullDCemu.AddMenuItem(rootmenu, -1, puruPakku, 0, 0);
			break;

		default:
			ndcSubDevice[port][subport] = NULL;
			printf("Pokopom -> Warning! Unknown SubDevice: %d [%d|%d]", id, port, subport);
			return nullDC::rv_serror;
		}
		break;

	case DC_PLATFORM_NAOMI: 
		break;

	default:
			ndcSubDevice[port][subport] = NULL;
			printf("Pokopom -> Warning! Unknown Platform: %d", dcPlatform);
			return nullDC::rv_serror;
	}

	inst->data = inst;

	switch(ndcVersion)
	{
	case nullDC_VER_103: 
		{
			nullDC_103::maple_subdevice_instance *inst103 = (nullDC_103::maple_subdevice_instance*)inst;
			inst103->dma  = SubDMA_103;
		}
		break;

	case nullDC_VER_104: inst->dma  = SubDMA; break;
	};

	return nullDC::rv_ok;
}

////////////////////////////////////////////////////////////////////////
// Emulation start, stop... quit?
////////////////////////////////////////////////////////////////////////

s32 FASTCALL Init(void* data, u32 id, nullDC::maple_init_params* params)
{
	DebugPrint("[%d]\n", ((nullDC::maple_device_instance*)data)->port >> 6);

	Input::Pause(false);
	KeepAwake(KEEPAWAKE_INIT);

	return nullDC::rv_ok;
}

void FASTCALL Term(void* data, u32 id)
{
	DebugPrint("[%d]\n", ((nullDC::maple_device_instance*)data)->port >> 6);

	Input::Pause(true);
	KeepAwake(KEEPAWAKE_CLOSE);
}

void FASTCALL Destroy(void* data, u32 id)
{
	DebugPrint("[%d]\n", ((nullDC::maple_device_instance*)data)->port >> 6);
}

//103////////////////////////////////////////////////////////////////////

s32 FASTCALL InitMain(void* data, u32 id, nullDC::maple_init_params* params)
{
	return Init(data, id, params);
}

void FASTCALL TermMain(void* data, u32 id) { Term(data, id); }
void FASTCALL DestroyMain(void* data, u32 id) {}

s32 FASTCALL InitSub(void* data, u32 id, nullDC::maple_init_params* params)
{
	return Init(data, id, params);
}

void FASTCALL TermSub(void* data, u32 id) { Term(data, id); }
void FASTCALL DestroySub(void* data, u32 id) {}

////////////////////////////////////////////////////////////////////////
// Commands and stuff
////////////////////////////////////////////////////////////////////////

u32 FASTCALL MainDMA(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	DebugFunc();

	u32 port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	return ndcDevice[port]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

u32 FASTCALL SubDMA(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len)
{
	DebugFunc();

	u8 port=((nullDC::maple_device_instance*)device_instance)->port>>6;
	u8 subport = GetSubport(((nullDC::maple_device_instance*)device_instance)->port);
	return ndcSubDevice[port][subport]->DMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

//103////////////////////////////////////////////////////////////////////

void FASTCALL MainDMA_103(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len, u32& response)
{
	DebugFunc();

	response = MainDMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

void FASTCALL SubDMA_103(void* device_instance, u32 command,
	u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len, u32& response)
{
	DebugFunc();

	response = SubDMA(device_instance, command, buffer_in, buffer_in_len, buffer_out, buffer_out_len);
}

////////////////////////////////////////////////////////////////////////
// The Config Menu call... figures.
////////////////////////////////////////////////////////////////////////

void EXPORT_CALL ConfigMenuCallback(u32 id, void* w, void* p)
{
	FileIO::INI_LoadSettings();
	CreateConfigDialog();
}

#endif // WIN32
