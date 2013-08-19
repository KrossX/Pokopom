/*  Pokopom - Input Plugin for PSX/PS2 Emulators (now nullDC too)
 *  - 2012  KrossX
 *
 *	Content of this file is from
 *	nullDC http://code.google.com/p/nulldc/
 *
 *  Licenced under GNU GPL v3
 *  http://www.gnu.org/licenses/gpl.html
 */

#pragma once

#ifdef _WIN32

namespace nullDC
{
	//intc function pointer and enums
	enum HollyInterruptType
	{
		holly_nrm = 0x0000,
		holly_ext = 0x0100,
		holly_err = 0x0200,
	};

	enum HollyInterruptID
	{
			// asic9a /sh4 external holly normal [internal]
			holly_RENDER_DONE_vd = holly_nrm | 0,	//bit 0 = End of Render interrupt : Video
			holly_RENDER_DONE_isp = holly_nrm | 1,	//bit 1 = End of Render interrupt : ISP
			holly_RENDER_DONE = holly_nrm | 2,		//bit 2 = End of Render interrupt : TSP

			holly_SCANINT1 = holly_nrm | 3,			//bit 3 = V Blank-in interrupt
			holly_SCANINT2 = holly_nrm | 4,			//bit 4 = V Blank-out interrupt
			holly_HBLank = holly_nrm | 5,			//bit 5 = H Blank-in interrupt

			holly_YUV_DMA = holly_nrm | 6,			//bit 6 = End of Transferring interrupt : YUV
			holly_OPAQUE = holly_nrm | 7,			//bit 7 = End of Transferring interrupt : Opaque List
			holly_OPAQUEMOD = holly_nrm | 8,		//bit 8 = End of Transferring interrupt : Opaque Modifier Volume List

			holly_TRANS = holly_nrm | 9,			//bit 9 = End of Transferring interrupt : Translucent List
			holly_TRANSMOD = holly_nrm | 10,		//bit 10 = End of Transferring interrupt : Translucent Modifier Volume List
			holly_PVR_DMA = holly_nrm | 11,			//bit 11 = End of DMA interrupt : PVR-DMA
			holly_MAPLE_DMA = holly_nrm | 12,		//bit 12 = End of DMA interrupt : Maple-DMA

			holly_MAPLE_VBOI = holly_nrm | 13,		//bit 13 = Maple V blank over interrupt
			holly_GDROM_DMA = holly_nrm | 14,		//bit 14 = End of DMA interrupt : GD-DMA
			holly_SPU_DMA = holly_nrm | 15,			//bit 15 = End of DMA interrupt : AICA-DMA

			holly_EXT_DMA1 = holly_nrm | 16,		//bit 16 = End of DMA interrupt : Ext-DMA1(External 1)
			holly_EXT_DMA2 = holly_nrm | 17,		//bit 17 = End of DMA interrupt : Ext-DMA2(External 2)
			holly_DEV_DMA = holly_nrm | 18,			//bit 18 = End of DMA interrupt : Dev-DMA(Development tool DMA)

			holly_CH2_DMA = holly_nrm | 19,			//bit 19 = End of DMA interrupt : ch2-DMA
			holly_PVR_SortDMA = holly_nrm | 20,		//bit 20 = End of DMA interrupt : Sort-DMA (Transferring for alpha sorting)
			holly_PUNCHTHRU = holly_nrm | 21,		//bit 21 = End of Transferring interrupt : Punch Through List

			holly_GDROM_CMD = holly_ext | 0x00,	//bit 0 = GD-ROM interrupt
			holly_SPU_IRQ = holly_ext | 0x01,	//bit 1 = AICA interrupt
			holly_EXP_8BIT = holly_ext | 0x02,	//bit 2 = Modem interrupt
			holly_EXP_PCI = holly_ext | 0x03,	//bit 3 = External Device interrupt

			holly_PRIM_NOMEM = holly_err | 0x02,	//bit 2 = TA : ISP/TSP Parameter Overflow
			holly_MATR_NOMEM = holly_err | 0x03		//bit 3 = TA : Object List Pointer Overflow
	};



	typedef void FASTCALL HollyRaiseInterruptFP(HollyInterruptID intr);
	typedef void FASTCALL HollyCancelInterruptFP(HollyInterruptID intr);

	struct vram_block
	{
		unsigned int start;
		unsigned int end;
		unsigned int len;
		unsigned int type;

		void* userdata;
	};

	typedef void FASTCALL vramLockCBFP (vram_block* block, unsigned int addr);

	struct VersionNumber
	{
		union
		{
			struct
			{
				unsigned char major:8;
				unsigned char minnor:8;
				unsigned char build:8;
				unsigned char flags:8;
			};
			unsigned int full;
		};
	};

	#define DC_PLATFORM_MASK		7
	#define DC_PLATFORM_NORMAL		0   /* Works, for the most part */
	#define DC_PLATFORM_DEV_UNIT	1	/* This is missing hardware */
	#define DC_PLATFORM_NAOMI		2   /* Works, for the most part */
	#define DC_PLATFORM_NAOMI2		3   /* Needs to be done, 2xsh4 + 2xpvr + custom TNL */
	#define DC_PLATFORM_ATOMISWAVE	4   /* Needs to be done, DC-like hardware with possibly more ram */
	#define DC_PLATFORM_HIKARU		5   /* Needs to be done, 2xsh4, 2x aica , custom vpu */
	#define DC_PLATFORM_AURORA		6   /* Needs to be done, Uses newer 300 mhz sh4 + 150 mhz pvr mbx SoC */


	#define DC_PLATFORM DC_PLATFORM_NORMAL


	#if (DC_PLATFORM==DC_PLATFORM_NORMAL)

		#define BUILD_DREAMCAST 1

		//DC : 16 mb ram, 8 mb vram, 2 mb aram, 2 mb bios, 128k flash
		#define RAM_SIZE (16*1024*1024)
		#define VRAM_SIZE (8*1024*1024)
		#define ARAM_SIZE (2*1024*1024)
		#define BIOS_SIZE (2*1024*1024)
		#define FLASH_SIZE (128*1024)

		#define ROM_PREFIX L"dc_"
		#define ROM_NAMES
		#define NVR_OPTIONAL 0

	#elif  (DC_PLATFORM==DC_PLATFORM_DEV_UNIT)

		#define BUILD_DEV_UNIT 1

		//Devkit : 32 mb ram, 8? mb vram, 2? mb aram, 2? mb bios, ? flash
		#define RAM_SIZE (32*1024*1024)
		#define VRAM_SIZE (8*1024*1024)
		#define ARAM_SIZE (2*1024*1024)
		#define BIOS_SIZE (2*1024*1024)
		#define FLASH_SIZE (128*1024)

		#define ROM_PREFIX L"hkt_"
		#define ROM_NAMES
		#define NVR_OPTIONAL 0

	#elif  (DC_PLATFORM==DC_PLATFORM_NAOMI)

		#define BUILD_NAOMI 1
		#define BUILD_NAOMI1 1

		//Naomi : 32 mb ram, 16 mb vram, 8 mb aram, 2 mb bios, ? flash
		#define RAM_SIZE (32*1024*1024)
		#define VRAM_SIZE (16*1024*1024)
		#define ARAM_SIZE (8*1024*1024)
		#define BIOS_SIZE (2*1024*1024)
		#define BBSRAM_SIZE (8*1024)

		#define ROM_PREFIX L"naomi_"
		#define ROM_NAMES L";epr-21576d.bin"
		#define NVR_OPTIONAL 1

	#elif  (DC_PLATFORM==DC_PLATFORM_NAOMI2)

		#define BUILD_NAOMI 1
		#define BUILD_NAOMI2 1

		//Naomi2 : 32 mb ram, 16 mb vram, 8 mb aram, 2 mb bios, ? flash
		#define RAM_SIZE (32*1024*1024)
		#define VRAM_SIZE (16*1024*1024)
		#define ARAM_SIZE (8*1024*1024)
		#define BIOS_SIZE (2*1024*1024)
		#define BBSRAM_SIZE (8*1024)

		#define ROM_PREFIX L"n2_"
		#define ROM_NAMES
		#define NVR_OPTIONAL 1

	#elif  (DC_PLATFORM==DC_PLATFORM_ATOMISWAVE)

		#define BUILD_ATOMISWAVE 1

		//Atomiswave : 16(?) mb ram, 16 mb vram, 8 mb aram, 64kb bios, 64k flash
		#define RAM_SIZE (16*1024*1024)
		#define VRAM_SIZE (16*1024*1024)
		#define ARAM_SIZE (8*1024*1024)
		#define BIOS_SIZE (64*1024)
		#define FLASH_SIZE (64*1024)

		#define ROM_PREFIX L"aw_"
		#define ROM_NAMES L";bios.ic23_l"
		#define NVR_OPTIONAL 1

	#else
		#error invalid build config
	#endif

	#define RAM_MASK	(RAM_SIZE-1)
	#define VRAM_MASK	(VRAM_SIZE-1)
	#define ARAM_MASK	(ARAM_SIZE-1)
	#define BIOS_MASK	(BIOS_SIZE-1)

	#ifdef FLASH_SIZE
	#define FLASH_MASK	(FLASH_SIZE-1)
	#endif

	#ifdef BBSRAM_SIZE
	#define BBSRAM_MASK	(BBSRAM_SIZE-1)
	#endif

	#define GD_CLOCK 33868800				//GDROM XTAL -- 768fs

	#define AICA_CORE_CLOCK (GD_CLOCK*4/3)		//[45158400]  GD->PLL 3:4 -> AICA CORE	 -- 1024fs
	#define ADAC_CLOCK (AICA_CORE_CLOCK/2)		//[11289600]  44100*256, AICA CORE -> PLL 4:1 -> ADAC -- 256fs
	#define AICA_ARM_CLOCK (AICA_CORE_CLOCK/2)	//[22579200]  AICA CORE -> PLL 2:1 -> ARM
	#define AICA_SDRAM_CLOCK (GD_CLOCK*2)		//[67737600]  GD-> PLL 2 -> SDRAM
	#define SH4_MAIN_CLOCK (200*1000*1000)		//[200000000] XTal(13.5) -> PLL (33.3) -> PLL 1:6 (200)
	#define SH4_RAM_CLOCK (100*1000*1000)		//[100000000] XTal(13.5) -> PLL (33.3) -> PLL 1:6 (100)	, also suplied to HOLLY chip
	#define G2_BUS_CLOCK (25*1000*1000)			//[25000000]  from Holly, from SH4_RAM_CLOCK w/ 2 2:1 plls

	#define DC_MakeVersion(major,minor,build) (((DC_PLATFORM)<<24)|((build)<<16)|((minor)<<8)|(major))

	enum PluginType
	{
		Plugin_PowerVR=1,		//3D ;)
		Plugin_GDRom=2,			//guess it
		Plugin_AICA=3,			//Sound :p
		Plugin_Maple=4,			//controler ,mouse , ect
		Plugin_ExtDevice=5,		//BBA , Lan adapter , other
		Plugin_ARM=6,			//Sound Cpu
	};

	enum ndc_error_codes
	{
		rv_ok = 0,		//no error

		rv_error=-2,	//error
		rv_serror=-1,	//silent error , it has been reported to the user
	};

	//Simple struct to store window rect  ;)
	//Top is 0,0 & numbers are in pixels.
	//Only client size
	struct NDC_WINDOW_RECT
	{
		unsigned int width;
		unsigned int height;
	};

	enum ndc_events
	{
		//gui -> *
		NDE_GUI_RESIZED=0,			//gui was resized, p points to a NDC_WINDOW_RECT with the new size.This event is not guaratneed to have any thread anfinity.The plugin
									//must handle sync. betwen threads to ensure proper operation.Borderless fullscreen use this, not NDC_GUI_REQESTFULLSCREEN

		NDE_GUI_REQESTFULLSCREEN,	//if (l) -> goto fullscreen, else goto window.This event can be safely ingored

		NDE_GUI_WINDOWCHANGE,		//if (l) old window handle is still valid, else it has been replaced with a new one.This event is sent with l!=0 before destructing the window, and then with l==0
									//after creating a new one.It is not sent for the initial or final window creation/destruction.

		//pvr -> *
		NDE_PVR_FULLSCREEN,			//if (l) -> new mode is fullscreen, else its window

		//Misc
		NDE_CUSTOM=0xFF000000,		//Base for custom events.be carefull with how you use these, as all plugins get em ;)
	};
	#define PLUGIN_I_F_VERSION DC_MakeVersion(1,0,2)

	//These are provided by the emu

	//Config
	typedef void EXPORT_CALL ConfigLoadStrFP(const wchar_t * Section, const wchar_t * Key, wchar_t * Return,const wchar_t* Default);
	typedef void EXPORT_CALL ConfigSaveStrFP(const wchar_t * Section, const wchar_t * Key, const wchar_t * Value);
	typedef signed int EXPORT_CALL ConfigLoadIntFP(const wchar_t * Section, const wchar_t * Key,const signed int Default);
	typedef void EXPORT_CALL ConfigSaveIntFP(const wchar_t * Section, const wchar_t * Key, const signed int Value);
	typedef signed int EXPORT_CALL ConfigExistsFP(const wchar_t * Section, const wchar_t * Key);

	//Menus
	enum MenuItemStyles
	{
		MIS_Seperator	=1,
		MIS_Radiocheck	=2,
		MIS_Bitmap		=4,
		MIS_Grayed		=0x40000000,
		MIS_Checked		=0x80000000,
	};

	typedef void EXPORT_CALL MenuItemSelectedFP(unsigned int id,void* WindowHandle,void* user);

	enum MenuItemMask
	{
		MIM_Text=1,
		MIM_Handler=2,
		MIM_Bitmap=4,
		MIM_Style=8,
		MIM_PUser=16,
		MIM_All=0xFFFFFFFF,
	};

	struct MenuItem
	{
		wchar_t* Text;			//Text of the menu item
		MenuItemSelectedFP* Handler;	//called when the menu is clicked
		void* Bitmap;		//bitmap handle
		unsigned int Style;			//MIS_* combination
		void* PUser;		//User defined pointer :)
	};

	enum MsgTarget
	{
		MT_Core=1<<0,
		MT_Gui =1<<1,
		MT_PowerVR=1<<2,
		MT_GDRom=1<<3,
		MT_Maple=1<<4,
		MT_ExtDevice=1<<5,
		MT_All=0xFFFFFFFF,
	};

	enum SyncSourceFlags
	{
		SSF_NeedsSync=1,
	};

	typedef unsigned int EXPORT_CALL AddMenuItemFP(unsigned int parent,signed int pos,const wchar_t* text,MenuItemSelectedFP* handler , unsigned int checked);
	typedef void EXPORT_CALL SetMenuItemStyleFP(unsigned int id,unsigned int style,unsigned int mask);
	typedef void EXPORT_CALL GetMenuItemFP(unsigned int id,MenuItem* info,unsigned int mask);
	typedef void EXPORT_CALL SetMenuItemFP(unsigned int id,MenuItem* info,unsigned int mask);
	typedef void EXPORT_CALL DeleteMenuItemFP(unsigned int id);
	typedef void* EXPORT_CALL GetRenderTargetFP();
	typedef void EXPORT_CALL SendMsgFP(unsigned int target,unsigned int eid,void* pdata,unsigned int ldata);
	typedef void EXPORT_CALL RegisterSyncSourceFP(wchar_t* name,unsigned int id,unsigned int freq,unsigned int flags);

	struct emu_info
	{
		GetRenderTargetFP*	GetRenderTarget;

		ConfigLoadStrFP*	ConfigLoadStr;
		ConfigSaveStrFP*	ConfigSaveStr;
		ConfigLoadIntFP*	ConfigLoadInt;
		ConfigSaveIntFP*	ConfigSaveInt;
		ConfigExistsFP*		ConfigExists;

		AddMenuItemFP*		AddMenuItem;
		SetMenuItemStyleFP*	SetMenuItemStyle;
		SetMenuItemFP*		SetMenuItem;
		GetMenuItemFP*		GetMenuItem;
		DeleteMenuItemFP*	DeleteMenuItem;

		SendMsgFP* BroardcastEvent;
		unsigned int RootMenu;
		unsigned int DebugMenu;

		RegisterSyncSourceFP* RegisterSyncSource;
	};

	//common plugin functions
	//called when plugin is used by emu (you should do first time init here)
	typedef signed int FASTCALL PluginInitFP(emu_info* param);

	//called when plugin is unloaded by emu , olny if dcInit is called (eg , not called to enumerate plugins)
	typedef void FASTCALL PluginTermFP();

	//Unhandled Write Exeption handler
	typedef bool FASTCALL ExeptionHanlderFP(void* addr);
	typedef unsigned int FASTCALL ReadMemFP(unsigned int addr,unsigned int size);
	typedef void FASTCALL WriteMemFP(unsigned int addr,unsigned int data,unsigned int size);
	typedef void FASTCALL UpdateFP(unsigned int cycles);
	typedef void FASTCALL PluginResetFP(bool Manual);
	typedef void EXPORT_CALL EventHandlerFP(unsigned int nid,void* p);

	struct common_info
	{
		unsigned int	InterfaceVersion;	//Note : this version is of the interface for this type of plugin :)
		wchar_t			Name[128];			//plugin name
		unsigned int	Type;				//plugin type
		PluginInitFP*	Load;				//Init
		PluginTermFP*	Unload;				//Term
		EventHandlerFP* EventHandler;		//Event Handler
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//******************************************************
	//*********************** PowerVR **********************
	//******************************************************

	#define PVR_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,0)
	typedef void FASTCALL vramlock_Unlock_blockFP  (vram_block* block);
	typedef vram_block* FASTCALL vramlock_LockFP(unsigned int start_offset,unsigned int end_offset,void* userdata);
	typedef void FASTCALL osdDrawFP();

	struct pvr_init_params
	{
		HollyRaiseInterruptFP*	RaiseInterrupt;
		unsigned char*					vram;
		vramlock_LockFP*		 vram_lock_32;
		vramlock_LockFP*		 vram_lock_64;
		vramlock_Unlock_blockFP* vram_unlock;
		osdDrawFP*				 osdDraw;
	};

	typedef signed int FASTCALL PvrInitFP(pvr_init_params* param);
	typedef void FASTCALL TaDMAFP(unsigned int* data,unsigned int size);
	typedef void FASTCALL TaSQFP(unsigned int* data);
	typedef struct osdTexture_i* osdTexture;

	enum OSDFORMAT
	{
		OSDFMT_A,		//1 bpp
		OSDFMT_RGB,		//3 bpp
		OSDFMT_RGBA,	//4 bpp
	};

	typedef signed int FASTCALL osdTexCreateFP(unsigned int w,unsigned int h,OSDFORMAT fmt,void* data);
	typedef signed int FASTCALL osdTexBindFP(osdTexture tex);
	typedef signed int FASTCALL osdTexDestroyFP(osdTexture tex);
	typedef signed int FASTCALL osdVtxFP(f32 x,f32 y,f32 z,f32 w,f32 r,f32 g,f32 b,f32 a,f32 u,f32 v);

	struct pvr_plugin_if
	{
		PvrInitFP*		Init;
		PluginResetFP*	Reset;
		PluginTermFP*	Term;
		ExeptionHanlderFP* ExeptionHanlder;
		UpdateFP*		UpdatePvr;
		TaDMAFP*		TaDMA;
		TaSQFP*			TaSQ;
		ReadMemFP*		ReadReg;
		WriteMemFP*		WriteReg;
		vramLockCBFP*	LockedBlockWrite;

		struct
		{
			osdVtxFP*				Vtx;
			osdTexCreateFP*			TexCreate;
			osdTexBindFP*			TexBind;
			osdTexDestroyFP*		TexDestroy;
		} osd;
	};
	//******************************************************
	//************************ GDRom ***********************
	//******************************************************
	enum DiscType
	{
		CdDA=0x00,
		CdRom=0x10,
		CdRom_XA=0x20,
		CdRom_Extra=0x30,
		CdRom_CDI=0x40,
		GdRom=0x80,
		NoDisk=0x1,
		Open=0x2,
		Busy=0x3
	};

	enum DiskArea
	{
		SingleDensity,
		DoubleDensity
	};

	enum DriveEvent
	{
		DiskChange=1	//disk ejected/changed
	};

	typedef void FASTCALL DriveNotifyEventFP(unsigned int event,void* param);
	typedef void FASTCALL DriveReadSectorFP(unsigned char * buff,unsigned int StartSector,unsigned int SectorCount,unsigned int secsz);
	typedef void FASTCALL DriveGetTocInfoFP(unsigned int* toc,unsigned int area);
	typedef unsigned int FASTCALL DriveGetDiscTypeFP();
	typedef void FASTCALL DriveGetSessionInfoFP(unsigned char* pout,unsigned char session);
	typedef void FASTCALL DriveReadSubChannelFP(unsigned char * buff, unsigned int format, unsigned int len);

	#define GDR_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,0)

	struct gdr_init_params
	{
		wchar_t* source;
		DriveNotifyEventFP* DriveNotifyEvent;
	};

	typedef signed int FASTCALL GdrInitFP(gdr_init_params* param);

	struct gdr_plugin_if
	{
		GdrInitFP*		Init;
		PluginResetFP*	Reset;
		PluginTermFP*	Term;
		ExeptionHanlderFP* ExeptionHanlder;
		DriveReadSectorFP* ReadSector;
		DriveReadSubChannelFP *ReadSubChannel;
		DriveGetTocInfoFP* GetToc;
		DriveGetDiscTypeFP* GetDiscType;
		DriveGetSessionInfoFP* GetSessionInfo;
	};
	//******************************************************
	//************************ AICA ************************
	//******************************************************
	typedef void FASTCALL ArmInterruptChangeFP(unsigned int bits,unsigned int L);
	typedef void FASTCALL CDDA_SectorFP(signed short* sector);

	#define AICA_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,1)

	struct aica_init_params
	{
		CDDA_SectorFP*	CDDA_Sector;
		unsigned char*	aica_ram;
		unsigned int* SB_ISTEXT;
		HollyRaiseInterruptFP*	RaiseInterrupt;
		HollyCancelInterruptFP* CancelInterrupt;
		ArmInterruptChangeFP*	ArmInterruptChange;
	};

	typedef signed int FASTCALL AicaInitFP(aica_init_params* param);

	struct aica_plugin_if
	{
		AicaInitFP*		Init;
		PluginResetFP*	Reset;
		PluginTermFP*	Term;
		ReadMemFP*  ReadMem_aica_reg;
		WriteMemFP* WriteMem_aica_reg;
		UpdateFP*	Update;
	};
	//******************************************************
	//******************** ARM Sound CPU *******************
	//******************************************************
	#define ARM_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,0)

	struct arm_init_params
	{
		unsigned char*			aica_ram;
		ReadMemFP*  ReadMem_aica_reg;
		WriteMemFP* WriteMem_aica_reg;
	};

	typedef signed int FASTCALL ArmInitFP(arm_init_params* param);


	struct arm_plugin_if
	{
		ArmInitFP*		Init;
		PluginResetFP*	Reset;
		PluginTermFP*	Term;
		UpdateFP* SetResetState;
		ExeptionHanlderFP* ExeptionHanlder;
		UpdateFP*	Update;
		ArmInterruptChangeFP*	ArmInterruptChange;
	};
	//******************************************************
	//****************** Maple devices ******************
	//******************************************************

	#define MAPLE_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,0)

	enum MapleDeviceCreationFlags
	{
		MDCF_None=0,
		MDCF_Hotplug=1
	};

	struct maple_subdevice_instance;
	struct maple_device_instance;

	//buffer_out_len and responce need to be filled w/ proper info by the plugin
	//buffer_in must not be edited (its direct pointer on ram)
	//output buffer must contain the frame data , the frame header is generated by the maple routing code
	//typedef unsigned int FASTCALL MapleSubDeviceDMAFP(void* device_instance,unsigned int Command,unsigned int* buffer_in,unsigned int buffer_in_len,unsigned int* buffer_out,unsigned int& buffer_out_len);
	typedef unsigned int FASTCALL MapleDeviceDMAFP(void* device_instance,unsigned int Command,unsigned int* buffer_in,unsigned int buffer_in_len,unsigned int* buffer_out,unsigned int& buffer_out_len);

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

	//Nothing for now ;)
	struct maple_init_params;

	typedef signed int FASTCALL MapleCreateInstanceFP(maple_device_instance* mdinst,unsigned int id,unsigned int flags,unsigned int rootmenu);
	typedef signed int FASTCALL MapleSubCreateInstanceFP(maple_subdevice_instance* msdinst,unsigned int id,unsigned int flags,unsigned int rootmenu);

	typedef signed int FASTCALL MapleInitInstanceFP(void* inst,unsigned int id,maple_init_params* params);
	typedef void FASTCALL MapleTermInstanceFP(void* inst,unsigned int id);
	typedef void FASTCALL MapleDestroyInstanceFP(void* inst,unsigned int id);

	typedef signed int FASTCALL MapleInitFP(maple_init_params* param);

	enum MapleDeviceType
	{
		MDT_EndOfList=0,
		MDT_Main=1,
		MDT_Sub=2,
	};
	enum MapleDeviceTypeFlags
	{
		MDTF_Sub0=1,		//these
		MDTF_Sub1=2,		//are
		MDTF_Sub2=4,		//ingored
		MDTF_Sub3=8,		//in
		MDTF_Sub4=16,		//subdevices :)

		MDTF_Hotplug=32,	//Can be added/removed at runtime.That means that it may never get the Init/Term calls
	};
	struct maple_device_definition
	{
		wchar_t Name[128];
		unsigned int Type;
		unsigned int Flags;
	};
	struct maple_plugin_if
	{
		//*Main functions are ignored if no main devices are exported
		//*Sub functions are ignored if no main devices are exported
		//Create Instance
		MapleCreateInstanceFP* CreateMain;
		MapleSubCreateInstanceFP* CreateSub;

		MapleDestroyInstanceFP* Destroy;
		MapleInitInstanceFP*	Init;
		MapleTermInstanceFP*	Term;

		maple_device_definition devices[32];	//Last one must be of type MDT_EndOfList , unless all 32 are used
	};

	//******************************************************
	//********************* Ext.Device *********************
	//******************************************************

	#define EXTDEVICE_PLUGIN_I_F_VERSION DC_MakeVersion(1,0,0)

	struct ext_device_init_params
	{
		HollyRaiseInterruptFP*	RaiseInterrupt;
		unsigned int* SB_ISTEXT;
		HollyCancelInterruptFP* CancelInterrupt;
	};

	typedef signed int FASTCALL ExtInitFP(ext_device_init_params* param);

	struct ext_device_plugin_if
	{
		ExtInitFP*		Init;
		PluginResetFP*	Reset;
		PluginTermFP*	Term;
		ExeptionHanlderFP* ExeptionHanlder;
		ReadMemFP*  ReadMem_A0_006;
		WriteMemFP* WriteMem_A0_006;
		ReadMemFP*  ReadMem_A0_010;
		WriteMemFP* WriteMem_A0_010;
		ReadMemFP*  ReadMem_A5;
		WriteMemFP* WriteMem_A5;
		UpdateFP*	UpdateExtDevice;
	};

	//Plugin Exports
	//These are the functions the plugin has to export :)

	struct plugin_interface
	{
		unsigned int InterfaceVersion;

		common_info common;
		union
		{
			pvr_plugin_if			pvr;
			gdr_plugin_if			gdr;
			aica_plugin_if			aica;
			arm_plugin_if			arm;
			maple_plugin_if			maple;
			ext_device_plugin_if	ext_dev;

			unsigned int pad[4096];//padding & reserved space for future expantion :)
		};
	};
} // Namespace nullDC

#endif // WIN32
