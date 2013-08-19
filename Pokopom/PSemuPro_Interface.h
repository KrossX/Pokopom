/*
	PSEmu Plugin Developer Kit Header definition

	(C)1998 Vision Thing

	This file can be used only to develop PSEmu Plugins
	Other usage is highly prohibited.
*/


// IMPORTANT!!!
// This file will be used by PSEmu version >= 1.0.3
// so, please, develop Plugins w/o using this header file now
// if you want to add return codes (any) just drop mail to
// duddie@psemu.com

// 2011 - Messed up by KrossX.

#pragma once

namespace emupro
{
	// header version
	const s32 _PPDK_HEADER_VERSION = 1;
	const s32 PLUGIN_VERSION	 = 1;

	// plugin type returned by PSEgetLibType (types can be merged if plugin is multi type!)
	const s32 LT_CDR = 1;
	const s32 LT_GPU = 2;
	const s32 LT_SPU = 4;
	const s32 LT_PAD = 8;

	// every function in DLL if completed sucessfully should return this value
	const s32 ERR_SUCCESS = 0;
	// undefined error but fatal one, that kills all functionality
	const s32 ERR_FATAL = -1;


	// XXX_Init return values
	// Those return values apply to all libraries

	// initialization went OK
	const s32 INIT_ERR_SUCCESS = 0;

	// this driver is not configured
	const s32 INIT_ERR_NOTCONFIGURED = -2;

	// this driver can not operate properly on this hardware or hardware is not detected
	const s32 INIT_ERR_NOHARDWARE = -3;


	/*         GPU PlugIn          */
	namespace gpu
	{

		//  Test return values

		// sucess, everything configured, and went OK.
		const s32 ERR_SUCCESS = 0;

		// this driver is not configured
		const s32 ERR_NOTCONFIGURED = 1;

		//  Query		- will be implemented in v2

		typedef struct
		{
			u32		flags;
			u32		status;
			HWND	window;
			u8		reserved[100];
		} QueryS;

		// gpuQueryS.flags
		// this driver requests windowed mode,
		const s32 FLAGS_WINDOWED = 1;

		// gpuQueryS.status
		// this driver cannot operate in this windowed mode
		const s32 STATUS_WINDOWWRONG = 1;

		//  Query	End	- will be implemented in v2
	}


	/*         CDR PlugIn          */
	namespace cdr
	{
		//	Test return values

		// success, everything configured, and went OK.
		const s32 ERR_SUCCESS = 0;

		// ERRORS
		const s32 ERR = -40;
		// this driver is not configured
		const s32 ERR_NOTCONFIGURED = ERR - 0;
		// if this driver is unable to read data from medium
		const s32 ERR_NOREAD = ERR - 1;

		// WARNINGS
		const s32 WARN = 40;
		// if this driver emulates lame mode ie. can read only 2048 tracks and sector header is emulated
		// this might happen to CDROMS that do not support RAW mode reading - surely it will kill many games
		const s32 WARN_LAMECD = WARN + 0;
	}

	/*         SPU PlugIn          */
	namespace spu
	{
		// some info restricted (now!)

		// success, everything configured, and went OK.
		const s32 ERR_SUCCESS = 0;

		// ERRORS
		// this error might be returned as critical error but none of below
		const s32 ERR = -60;

		// this driver is not configured
		const s32 ERR_NOTCONFIGURED	 = ERR - 1;
		// this driver failed Init
		const s32 ERR_INIT = ERR - 2;


		// WARNINGS
		// this warning might be returned as undefined warning but allowing driver to continue
		const s32 WARN = 60;
	}

	/*         PAD PlugIn          */
	namespace pad
	{
		// PADquery responses (notice - values ORed)
		// PSEmu will use them also in PADinit to tell Plugin which Ports will use
		// notice that PSEmu will call PADinit and PADopen only once when they are from
		// same plugin

		// might be used in port 1 (must support PADreadPort1() function)
		const s32 USE_PORT1 = 1;
		// might be used in port 2 (must support PADreadPort2() function)
		const s32 USE_PORT2 = 2;



		// MOUSE SCPH-1030
		const s32 TYPE_MOUSE = 1;
		// NEGCON - 16 button analog controller SLPH-00001
		const s32 TYPE_NEGCON	 = 2;
		// GUN CONTROLLER - gun controller SLPH-00014 from Konami
		const s32 TYPE_GUN = 3;
		// STANDARD PAD SCPH-1080, SCPH-1150
		const s32 TYPE_STANDARD = 4;
		// ANALOG JOYSTICK SCPH-1110
		const s32 TYPE_ANALOGJOY = 5;
		// GUNCON - gun controller SLPH-00034 from Namco
		const s32 TYPE_GUNCON	 = 6;
		// ANALOG CONTROLLER SCPH-1150
		const s32 TYPE_ANALOGPAD = 7;


		// success, everything configured, and went OK.
		const s32 ERR_SUCCESS = 0;
		// general plugin failure (undefined error)
		const s32 ERR_FAILURE = -1;


		// ERRORS
		// this error might be returned as critical error but none of below
		const s32 ERR = -80;
		// this driver is not configured
		const s32 ERR_NOTCONFIGURED = ERR - 1;
		// this driver failed Init
		const s32 ERR_INIT = ERR - 2;


		// WARNINGS
		// this warning might be returned as undefined warning but allowing driver to continue
		const s32 WARN = 80;


		typedef struct
		{
			// controller type - fill it withe predefined values above
			u8 controllerType;

			// status of buttons - every controller fills this field
			u16 buttonStatus;

			// for analog pad fill those next 4 bytes
			// values are analog in range 0-255 where 128 is center position
			u8 rightJoyX, rightJoyY, leftJoyX, leftJoyY;

			// for mouse fill those next 2 bytes
			// values are in range -128 - 127
			u8 moveX, moveY;

			u8 reserved[91];

		} DataS;
	}

	//Savestates stuff
	namespace Savestate
	{
		const s32 LOAD = 0;
		const s32 SAVE = 1;
		const s32 QUERY_SIZE = 2;
	}

}