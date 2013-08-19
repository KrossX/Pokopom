#pragma once

#ifdef _WIN32

namespace nullDC_103
{
	typedef void FASTCALL MapleDeviceDMAFP(void* device_instance,unsigned int Command,unsigned int* buffer_in,unsigned int buffer_in_len,unsigned int* buffer_out,unsigned int& buffer_out_len, unsigned int& response);

	struct maple_subdevice_instance
	{
		//port
		unsigned char port;
		//user data
		void* data;
		//MapleDeviceDMA
		MapleDeviceDMAFP* dma;
		bool connected;
		unsigned int reserved;	//reserved for the emu , DO NOT EDIT
	};

	struct maple_device_instance
	{
		//port
		unsigned char port;
		//user data
		void* data;
		//MapleDeviceDMA
		MapleDeviceDMAFP* dma;
		bool connected;

		maple_subdevice_instance subdevices[5];
	};

	struct maple_plugin_if
	{
		//*Main functions are ignored if no main devices are exported
		//*Sub functions are ignored if no main devices are exported
		//Create Instance
		nullDC::MapleCreateInstanceFP*    CreateMain;
		nullDC::MapleSubCreateInstanceFP* CreateSub;

		nullDC::MapleDestroyInstanceFP* DestroyMain;
		nullDC::MapleDestroyInstanceFP* DestroySub;

		nullDC::MapleInitInstanceFP*	InitMain;
		nullDC::MapleInitInstanceFP*	InitSub;

		nullDC::MapleTermInstanceFP*	TermMain;
		nullDC::MapleTermInstanceFP*	TermSub;

		nullDC::maple_device_definition devices[32];	//Last one must be of type MDT_EndOfList , unless all 32 are used
	};

	struct common_info
	{
		wchar_t			Name[128];			//plugin name
		unsigned int	InterfaceVersion;	//Note : this version is of the interface for this type of plugin :)
		unsigned int	Type;				//plugin type
		unsigned int	Unknown;			// 1 ...
		nullDC::PluginInitFP*	Load;				//Init
		nullDC::PluginTermFP*	Unload;				//Term
		nullDC::EventHandlerFP* EventHandler;		//Event Handler
	};

	struct plugin_interface
	{
		unsigned int InterfaceVersion;

		common_info common;
		union
		{
			nullDC::pvr_plugin_if			pvr;
			nullDC::gdr_plugin_if			gdr;
			nullDC::aica_plugin_if			aica;
			nullDC::arm_plugin_if			arm;
			maple_plugin_if					maple;
			nullDC::ext_device_plugin_if	ext_dev;

			unsigned int pad[4096];//padding & reserved space for future expantion :)
		};
	};


} // End namespace nullDC_103

#endif // WIN32
