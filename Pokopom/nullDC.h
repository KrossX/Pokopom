/*  Pokopom - Input Plugin for PSX/PS2 Emulators (now nullDC too)
 *  - 2012  KrossX
 *	
 *	Content of this file is based on code from 
 *	nullDC http://code.google.com/p/nulldc/
 *
 *  Licenced under GNU GPL v3
 *  http://www.gnu.org/licenses/gpl.html
 */

#include "General.h"
#include "nullDC_plugin_types.h"
#include "nullDC_plugin_types_old.h"


enum
{
	nullDC_VER_10B = 0x00,
	nullDC_VER_103,
	nullDC_VER_104
};


#ifdef _WIN32

s32 FASTCALL Load(nullDC::emu_info* emu);
void FASTCALL Unload();

s32 FASTCALL CreateMain(nullDC::maple_device_instance* inst, u32 id, u32 flags, u32 rootmenu);
s32 FASTCALL CreateSub(nullDC::maple_subdevice_instance* inst, u32 id, u32 flags, u32 rootmenu);
s32 FASTCALL Init(void* data, u32 id, nullDC::maple_init_params* params);
void FASTCALL Term(void* data, u32 id);
void FASTCALL Destroy(void* data, u32 id);

u32 FASTCALL MainDMA(void* device_instance, u32 command, u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len);
u32 FASTCALL SubDMA(void* device_instance, u32 command, u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len);

void EXPORT_CALL ConfigMenuCallback(u32 id, void* w, void* p);

INT_PTR CALLBACK OpenConfig( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// nullDC 103
s32 FASTCALL InitMain(void* data, u32 id, nullDC::maple_init_params* params);
void FASTCALL TermMain(void* data, u32 id);
void FASTCALL DestroyMain(void* data, u32 id);

s32 FASTCALL InitSub(void* data, u32 id, nullDC::maple_init_params* params);
void FASTCALL TermSub(void* data, u32 id);
void FASTCALL DestroySub(void* data, u32 id);

void FASTCALL MainDMA_103(void* device_instance, u32 command, u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len, u32& response);
void FASTCALL SubDMA_103(void* device_instance, u32 command, u32* buffer_in, u32 buffer_in_len, u32* buffer_out, u32& buffer_out_len, u32& response);
// end nullDC 103

#endif // WIN32
