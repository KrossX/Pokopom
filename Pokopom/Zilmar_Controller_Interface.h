/**********************************************************************************
Common Controller plugin spec, version #1.1 maintained by
zilmar (zilmar@emulation64.com)

All questions or suggestions should go through the emutalk plugin forum.
http://www.emutalk.net/cgi-bin/ikonboard/ikonboard.cgi?s=3bd272222f66ffff;act=SF;f=20
**********************************************************************************/

// 2012 - Messed up by KrossX, sorry~

#pragma once
#ifdef _WIN32

namespace Zilmar
{
	const u16 INTERFACE_VERSION			= 0x0100; // 0x01000 for 1.0, x0101 for 1.1
	const u8 PLUGIN_TYPE_CONTROLLER		= 4;

	/*** Conteroller plugin's ****/
	const u8 PLUGIN_NONE				= 1;
	const u8 PLUGIN_MEMPAK				= 2;
	const u8 PLUGIN_RUMBLE_PAK			= 3; // not implemeted for non raw data
	const u8 PLUGIN_TANSFER_PAK			= 4; // not implemeted for non raw data
	const u8 PLUGIN_RAW					= 5; // the controller plugin is passed in raw data

	/*********************************************************************************
	 Note about Conteroller plugin's:
	 the rumble pak needs a function for the force feed back joystick and tranfer pak
	 probaly needs a function for the plugin to be able to select the GB rom and
	 eeprom... maybe this should be done by the emu instead of the plugin, but I think
	 it probaly should be done by the plugin. I will see about adding these functions
	 in the next spec
	**********************************************************************************/

	/***** Structures *****/
	typedef struct {
		WORD Version;        /* Should be set to 0x0101 */
		WORD Type;           /* Set to PLUGIN_TYPE_CONTROLLER */
		char Name[100];      /* Name of the DLL */
		BOOL Reserved1;
		BOOL Reserved2;
	} PLUGIN_INFO;

	typedef struct {
		BOOL Present;
		BOOL RawData;
		int  Plugin;
	} CONTROL;

	typedef union {
		DWORD Value;
		struct {
			unsigned R_DPAD       : 1;
			unsigned L_DPAD       : 1;
			unsigned D_DPAD       : 1;
			unsigned U_DPAD       : 1;
			unsigned START_BUTTON : 1;
			unsigned Z_TRIG       : 1;
			unsigned B_BUTTON     : 1;
			unsigned A_BUTTON     : 1;

			unsigned R_CBUTTON    : 1;
			unsigned L_CBUTTON    : 1;
			unsigned D_CBUTTON    : 1;
			unsigned U_CBUTTON    : 1;
			unsigned R_TRIG       : 1;
			unsigned L_TRIG       : 1;
			unsigned Reserved1    : 1;
			unsigned Reserved2    : 1;

			signed   Y_AXIS       : 8;

			signed   X_AXIS       : 8;
		};
	} BUTTONS;

	typedef struct {
		HWND hMainWindow;
		HINSTANCE hinst;

		BOOL MemoryBswaped;		// If this is set to TRUE, then the memory has been pre
								//   bswap on a dword (32 bits) boundry, only effects header.
								//	eg. the first 8 bytes are stored like this:
								//        4 3 2 1   8 7 6 5
		BYTE * HEADER;			// This is the rom header (first 40h bytes of the rom)
		CONTROL *Controls;		// A pointer to an array of 4 controllers .. eg:
								// CONTROL Controls[4];
	} CONTROL_INFO;

} // End namespace
#endif //WIN32

