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

int FASTCALL Load(nullDC::emu_info* emu);
void FASTCALL Unload();

int FASTCALL CreateMain(nullDC::maple_device_instance* inst, unsigned int id, unsigned int flags, unsigned int rootmenu);
int FASTCALL CreateSub(nullDC::maple_subdevice_instance* inst, unsigned int id, unsigned int flags, unsigned int rootmenu);
int FASTCALL Init(void* data, unsigned int id, nullDC::maple_init_params* params);
void FASTCALL Term(void* data, unsigned int id);
void FASTCALL Destroy(void* data, unsigned int id);

unsigned int FASTCALL ControllerDMA(void* device_instance, unsigned int command, unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);
unsigned int FASTCALL RumbleDMA(void* device_instance, unsigned int command, unsigned int* buffer_in, unsigned int buffer_in_len, unsigned int* buffer_out, unsigned int& buffer_out_len);

void EXPORT_CALL ConfigMenuCallback(unsigned int id, void* w, void* p);

INT_PTR CALLBACK OpenConfig( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
