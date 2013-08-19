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
	const int _PPDK_HEADER_VERSION = 1;
	const int PLUGIN_VERSION	 = 1;

	// plugin type returned by PSEgetLibType (types can be merged if plugin is multi type!)
	const int LT_CDR = 1;
	const int LT_GPU = 2;
	const int LT_SPU = 4;
	const int LT_PAD = 8;

	// every function in DLL if completed sucessfully should return this value
	const int ERR_SUCCESS = 0;
	// undefined error but fatal one, that kills all functionality
	const int ERR_FATAL = -1;


	// XXX_Init return values
	// Those return values apply to all libraries

	// initialization went OK
	const int INIT_ERR_SUCCESS = 0;

	// this driver is not configured
	const int INIT_ERR_NOTCONFIGURED = -2;

	// this driver can not operate properly on this hardware or hardware is not detected
	const int INIT_ERR_NOHARDWARE = -3;


	/*         GPU PlugIn          */
	namespace gpu
	{

		//  Test return values

		// sucess, everything configured, and went OK.
		const int ERR_SUCCESS = 0;

		// this driver is not configured
		const int ERR_NOTCONFIGURED = 1;

		//  Query		- will be implemented in v2

		typedef struct
		{
			unsigned long	flags;
			unsigned long	status;
			HWND			window;
			unsigned char reserved[100];
		} QueryS;

		// gpuQueryS.flags
		// this driver requests windowed mode,
		const int FLAGS_WINDOWED = 1;

		// gpuQueryS.status
		// this driver cannot operate in this windowed mode
		const int STATUS_WINDOWWRONG = 1;

		//  Query	End	- will be implemented in v2
	}


	/*         CDR PlugIn          */
	namespace cdr
	{
		//	Test return values

		// sucess, everything configured, and went OK.
		const int ERR_SUCCESS = 0;

		// ERRORS
		const int ERR = -40;
		// this driver is not configured
		const int ERR_NOTCONFIGURED = ERR - 0;
		// if this driver is unable to read data from medium
		const int ERR_NOREAD = ERR - 1;

		// WARNINGS
		const int WARN = 40;
		// if this driver emulates lame mode ie. can read only 2048 tracks and sector header is emulated
		// this might happen to CDROMS that do not support RAW mode reading - surelly it will kill many games
		const int WARN_LAMECD = WARN + 0;
	}

	/*         SPU PlugIn          */
	namespace spu
	{
		// some info retricted (now!)

		// sucess, everything configured, and went OK.
		const int ERR_SUCCESS = 0;

		// ERRORS
		// this error might be returned as critical error but none of below
		const int ERR = -60;

		// this driver is not configured
		const int ERR_NOTCONFIGURED	 = ERR - 1;
		// this driver failed Init
		const int ERR_INIT = ERR - 2;


		// WARNINGS
		// this warning might be returned as undefined warning but allowing driver to continue
		const int WARN = 60;
	}	

	/*         PAD PlugIn          */
	namespace pad
	{
		// PADquery responses (notice - values ORed)
		// PSEmu will use them also in PADinit to tell Plugin which Ports will use
		// notice that PSEmu will call PADinit and PADopen only once when they are from
		// same plugin

		// might be used in port 1 (must support PADreadPort1() function)
		const int USE_PORT1 = 1;
		// might be used in port 2 (must support PADreadPort2() function)
		const int USE_PORT2 = 2;



		// MOUSE SCPH-1030
		const int TYPE_MOUSE = 1;
		// NEGCON - 16 button analog controller SLPH-00001
		const int TYPE_NEGCON	 = 2;
		// GUN CONTROLLER - gun controller SLPH-00014 from Konami
		const int TYPE_GUN = 3;
		// STANDARD PAD SCPH-1080, SCPH-1150
		const int TYPE_STANDARD = 4;
		// ANALOG JOYSTICK SCPH-1110
		const int TYPE_ANALOGJOY = 5;
		// GUNCON - gun controller SLPH-00034 from Namco
		const int TYPE_GUNCON	 = 6;
		// ANALOG CONTROLLER SCPH-1150
		const int TYPE_ANALOGPAD = 7;


		// sucess, everything configured, and went OK.
		const int ERR_SUCCESS = 0;
		// general plugin failure (undefined error)
		const int ERR_FAILURE = -1;


		// ERRORS
		// this error might be returned as critical error but none of below
		const int ERR = -80;
		// this driver is not configured
		const int ERR_NOTCONFIGURED = ERR - 1;
		// this driver failed Init
		const int ERR_INIT = ERR - 2;


		// WARNINGS
		// this warning might be returned as undefined warning but allowing driver to continue
		const int WARN = 80;


		typedef struct
		{
			// controler type - fill it withe predefined values above
			unsigned char controllerType;
	
			// status of buttons - every controller fills this field
			unsigned short buttonStatus;
	
			// for analog pad fill those next 4 bytes
			// values are analog in range 0-255 where 128 is center position
			unsigned char rightJoyX, rightJoyY, leftJoyX, leftJoyY;

			// for mouse fill those next 2 bytes
			// values are in range -128 - 127
			unsigned char moveX, moveY;

			unsigned char reserved[91];

		} DataS;
	}

	//Savestates stuff
	namespace Savestate
	{
		const int LOAD = 0;
		const int SAVE = 1;
		const int QUERY_SIZE = 2;
	}

}