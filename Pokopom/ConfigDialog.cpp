/*  Pokopom - Input Plugin for PSX/PS2 Emulators
 *  Copyright (C) 2012  KrossX
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Settings.h"
#include "FileIO.h"

#include <Windows.h>
#include <Uxtheme.h>
#include <CommCtrl.h>
#include <stdio.h>

#include "resource.h"
#include "ConfigDialog.h"

_Settings settings[2];
HWND hChild = NULL;

void UpdateControls(HWND hDialog, int port)
{		
	for(int i = 0; i<4; i++)
	{
		SendMessage(GetDlgItem(hDialog, i + 1027), CB_SETCURSEL, settings[port].axisRemap[i], 0);
		CheckDlgButton(hDialog, i + 1031, settings[port].axisInverted[i] ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hDialog, i + 1039, settings[port].xinputPort == i ? BST_CHECKED : BST_UNCHECKED);	
	}
		
	CheckDlgButton(hDialog, settings[port].defaultAnalog ? 1044:1043, BST_CHECKED);
	CheckDlgButton(hDialog, settings[port].defaultAnalog ? 1043:1044, BST_UNCHECKED);

	int position = (int)(settings[port].deadzone * 100);
	wchar_t text[8] = {0};					
	
	SendMessage(GetDlgItem(hDialog, IDC_SLIDER_DEADZONE), TBM_SETPOS, TRUE, (LONG)position);
	swprintf(text, 5, L"%d%%", position);
	SetDlgItemText(hDialog, IDC_TEXT_DEADZONE_P, text);

	position = (int)(settings[port].rumble * 100);
						
	SendMessage(GetDlgItem(hDialog, IDC_SLIDER_RUMBLE), TBM_SETPOS, TRUE, (LONG)position);
	swprintf(text, 5, L"%d%%", position);
	SetDlgItemText(hDialog, IDC_TEXT_RUMBLE_P, text);
}


INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	static int port;
	
	switch(uMsg)
	{
	case WM_INITDIALOG: 
		{																																
			port = TabCtrl_GetCurSel(GetDlgItem(GetParent(hwndDlg), IDC_TAB1));
			
			for(int control = 1027; control < 1033; control++)
			{
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis LX");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis LY");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis RX");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis RY");
			}		
												
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE), TBM_SETRANGE, TRUE, MAKELONG(0, 100));			
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_RUMBLE), TBM_SETRANGE, TRUE, MAKELONG(0, 200));
			
			UpdateControls(hwndDlg, port);
				
			ShowWindow(hwndDlg, SW_SHOW);
		} break;  

	case WM_HSCROLL:	
		{											
			if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE)) 
			{	
				wchar_t text[8] = {0};

				int deadzone = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE),TBM_GETPOS,0,0);
				settings[port].deadzone =  deadzone / 100.0f;
				
				swprintf(text, 5, L"%d%%", deadzone);
				SetDlgItemText(hwndDlg, IDC_TEXT_DEADZONE_P, text);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_RUMBLE)) 
			{
				wchar_t text[8] = {0};

				int rumble = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_RUMBLE),TBM_GETPOS,0,0);
				settings[port].rumble =  rumble / 100.0f;
								
				swprintf(text, 5, L"%d%%", rumble);
				SetDlgItemText(hwndDlg, IDC_TEXT_RUMBLE_P, text);
			}	
		} break;

	case WM_COMMAND:
		{
			short command = LOWORD(wParam);

			switch(command)
			{				
			case IDC_XINPUT1: settings[port].xinputPort = 0; break;
			case IDC_XINPUT2: settings[port].xinputPort = 1; break;
			case IDC_XINPUT3: settings[port].xinputPort = 2; break;
			case IDC_XINPUT4: settings[port].xinputPort = 3; break;
			
			case IDC_MODE_DIGITAL: settings[port].defaultAnalog = false; break;
			case IDC_MODE_ANALOG: settings[port].defaultAnalog = true; break;

			case IDC_COMBO_LX: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_LX] = (short)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_LX), CB_GETCURSEL, 0, 0);								
				break;
			case IDC_COMBO_LY: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_LY] = (short)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_LY), CB_GETCURSEL, 0, 0);								
				break;
			case IDC_COMBO_RX: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_RX] = (short)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_RX), CB_GETCURSEL, 0, 0);								
				break;
			case IDC_COMBO_RY: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_RY] = (short)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_RY), CB_GETCURSEL, 0, 0);								
				break;

			case IDC_INVERT_LX:
				settings[port].axisInverted[GP_AXIS_LX] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_LX) == BST_CHECKED? true:false;
				break;
			case IDC_INVERT_LY:
				settings[port].axisInverted[GP_AXIS_LY] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_LY) == BST_CHECKED? true:false;
				break;
			case IDC_INVERT_RX:
				settings[port].axisInverted[GP_AXIS_RX] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_RX) == BST_CHECKED? true:false;
				break;
			case IDC_INVERT_RY:
				settings[port].axisInverted[GP_AXIS_RY] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_RY) == BST_CHECKED? true:false;
				break;
			}

		} break;

	case WM_USER: if(wParam == 0xDEADBEEF) 
		{			
			port = lParam;	
			UpdateControls(hChild, port);
		}
		break;

	default: return FALSE;
	}

	return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG: 
		{																				
			HWND hTabControl = GetDlgItem(hwndDlg, IDC_TAB1);
			
			TCITEM tci; 
			tci.mask = TCIF_TEXT | TCIF_IMAGE;
			tci.iImage = -1; 

			tci.pszText = L"Controller 1";
			TabCtrl_InsertItem(hTabControl, 0, &tci); 

			tci.pszText = L"Controller 2";
			TabCtrl_InsertItem(hTabControl, 1, &tci); 

			hChild = CreateDialog((HINSTANCE)lParam, MAKEINTRESOURCE(IDD_INTAB), hwndDlg, DialogProc2);
			EnableThemeDialogTexture(hChild, ETDT_ENABLETAB);
						
			ShowWindow(hwndDlg, SW_SHOW);
		} break;    

	case WM_COMMAND:
		{
			short command = LOWORD(wParam);

			switch(command)
			{				
			case ID_OK: INI_SaveSettings(); PostQuitMessage(0); break; // SAVE SETTINGS and QUIT			
			case ID_CANCEL: PostQuitMessage(0); break;
			}

		} break;

	case WM_NOTIFY:		
			if ( ((LPNMHDR)lParam)->idFrom==IDC_TAB1 && ((LPNMHDR)lParam)->code == TCN_SELCHANGE  )
			{								
				int port =  TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_TAB1));
				SendMessage(hChild, WM_USER, 0xDEADBEEF, port);				
			}
			break;

	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_CLOSE: DestroyWindow(hwndDlg); break;
	
	default: return FALSE;
	}

	return TRUE;
}


void CreateDialogs (HINSTANCE hInstance)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CONFIG), 0, DialogProc, (LPARAM)hInstance);
	MSG message;

	while(GetMessage(&message, NULL, NULL, NULL))
	{		
		if(hChild == NULL || !IsDialogMessage(hChild, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	} 
}