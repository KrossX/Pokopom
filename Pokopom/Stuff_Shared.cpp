#include "General.h"

std::list <keyEvent> keyEventList;

bool SwapPortsEnabled = false;
bool SwapSticksEnabled = false;
bool bPriority = false;
bool isPs2Emulator = false;
bool isPSemulator = false;
bool bKeepAwake = false; // Screensaver and stuff
s32 INIversion = 3; // INI version
_Settings settings[4];


