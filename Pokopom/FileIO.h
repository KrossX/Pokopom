/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

namespace FileIO
{
	void INI_LoadSettings();
	void INI_SaveSettings();

	bool FASTCALL LoadMempak(u8 *data, u8 port);
	void FASTCALL SaveMempak(u8 *data, u8 port);

	bool FASTCALL LoadEEPROM(u8 *data);
	void FASTCALL SaveEEPROM(u8 *data);
}
