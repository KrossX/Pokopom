/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "General.h"
#include "ConfigDialog.h"
#include "FileIO.h"

#ifdef _WIN32
#include <Uxtheme.h>
#include <CommCtrl.h>

#include "resource.h"

HWND hChild, hParent = NULL;
//extern u8 multitap;
u8 multitap = NULL;
extern HINSTANCE h_instance;
//extern u8 dcPlatform;

f64 SliderUpdate(HWND hDialog, s32 sliderID, s32 textID, bool Linearity = false)
{
	wchar_t text[8];
	f64 value = 0.0;

	LRESULT res = SendMessage(GetDlgItem(hDialog, sliderID),TBM_GETPOS,0,0);

	if(Linearity)
	{
		res = res < 0? res -10 : res > 0? res +10: res;
		value =  res / 10.00;

		swprintf(text, 6, L"%1.2f", value);
	}
	else
	{
		swprintf(text, 5, L"%d%%", res);
		value = res / 100.00;
	}
	
	SetDlgItemText(hDialog, textID, text);

	return value;
}

void SliderSet(HWND hDialog, s32 sliderID, f64 value, bool Linearity = false)
{
	LRESULT position;

	if(Linearity)
	{
		position = (LRESULT)(value * 10.1);
		position = position > 0? position - 10 : position < 0 ? position +10 : position;
	}
	else
	{
		position = (LRESULT)(value * 100.1);
	}
	
	SendMessage(GetDlgItem(hDialog, sliderID), TBM_SETPOS, TRUE, position);
	SendMessage(hDialog, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hDialog, sliderID));
}

void UpdateControls(HWND hDialog, s32 port)
{
	for(s32 i = 0; i<4; i++)
	{
		SendMessage(GetDlgItem(hDialog, i + IDC_COMBO_LX), CB_SETCURSEL, settings[port].axisRemap[i], 0);
		CheckDlgButton(hDialog, i + IDC_INVERT_LX, settings[port].axisInverted[i] ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hDialog, i + IDC_XINPUT1, settings[port].xinputPort == i ? BST_CHECKED : BST_UNCHECKED);
	}

	CheckDlgButton(hDialog, IDC_DISABLED, settings[port].disabled ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDialog, IDC_GUITAR, settings[port].isGuitar ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDialog, IDC_ANALOG_GREEN, settings[port].greenAnalog ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDialog, IDC_LOCKSLIDERS, settings[port].sticksLocked ? BST_CHECKED : BST_UNCHECKED);
	//CheckDlgButton(hDialog, IDC_4WAYSTICK, settings[port].b4wayStick ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDialog, IDC_SWAPSTICKS, settings[port].SwapSticksEnabled ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hDialog, IDC_SWAPXO, settings[port].SwapXO ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(hDialog, settings[port].defaultAnalog ? IDC_MODE_ANALOG : IDC_MODE_DIGITAL, BST_CHECKED);
	CheckDlgButton(hDialog, settings[port].defaultAnalog ? IDC_MODE_DIGITAL : IDC_MODE_ANALOG, BST_UNCHECKED);

	EnableWindow(GetDlgItem(hDialog, IDC_4WAYSTICK), FALSE); // Disabled for now.

	EnableWindow(GetDlgItem(hDialog, IDC_MODE_DIGITAL), isPSemulator);
	EnableWindow(GetDlgItem(hDialog, IDC_MODE_ANALOG), isPSemulator);
	EnableWindow(GetDlgItem(hDialog, IDC_MULTITAP), isPSemulator);
	EnableWindow(GetDlgItem(hDialog, IDC_ANALOG_GREEN), isPSemulator && !isPs2Emulator);
	EnableWindow(GetDlgItem(hDialog, IDC_GUITAR), isPs2Emulator);
	EnableWindow(GetDlgItem(hDialog, IDC_SWAPSTICKS), !isPSemulator);
	EnableWindow(GetDlgItem(hDialog, IDC_SWAPXO), isPSemulator);
	
	SliderSet(hDialog, IDC_SLIDER_RUMBLE, settings[port].rumble);

	SliderSet(hDialog, IDC_SLIDER_DEADZONE2, settings[port].stickR.deadzone);
	SliderSet(hDialog, IDC_SLIDER_ANTIDEADZONE2, settings[port].stickR.antiDeadzone);
	SliderSet(hDialog, IDC_SLIDER_LINEARITY2, settings[port].stickR.linearity, true);

	SliderSet(hDialog, IDC_SLIDER_DEADZONE, settings[port].stickL.deadzone);
	SliderSet(hDialog, IDC_SLIDER_ANTIDEADZONE, settings[port].stickL.antiDeadzone);
	SliderSet(hDialog, IDC_SLIDER_LINEARITY, settings[port].stickL.linearity, true);

	EnableWindow(GetDlgItem(hDialog, IDC_SLIDER_DEADZONE2), !settings[port].sticksLocked);
	EnableWindow(GetDlgItem(hDialog, IDC_SLIDER_ANTIDEADZONE2), !settings[port].sticksLocked);
	EnableWindow(GetDlgItem(hDialog, IDC_SLIDER_LINEARITY2), !settings[port].sticksLocked);
}


INT_PTR CALLBACK DialogProc2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static s32 port;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			port = TabCtrl_GetCurSel(GetDlgItem(GetParent(hwndDlg), IDC_TAB1));

			for(s32 control = IDC_COMBO_LX; control <= IDC_COMBO_RY; control++)
			{
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis LX");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis LY");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis RX");
				SendMessage(GetDlgItem(hwndDlg, control), CB_ADDSTRING, 0, (LPARAM)L"Axis RY");
			}

			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_RUMBLE), TBM_SETRANGE, TRUE, MAKELONG(0, 200));
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ANTIDEADZONE), TBM_SETRANGE, TRUE, MAKELONG(0, 90));
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_LINEARITY), TBM_SETRANGE, TRUE, MAKELONG(-30, 30));

			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE2), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ANTIDEADZONE2), TBM_SETRANGE, TRUE, MAKELONG(0, 90));
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_LINEARITY2), TBM_SETRANGE, TRUE, MAKELONG(-30, 30));

			UpdateControls(hwndDlg, port);

			ShowWindow(hwndDlg, SW_SHOW);
		} break;

	case WM_HSCROLL:
		{
			if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_RUMBLE))
			{
				settings[port].rumble = (f32)SliderUpdate(hwndDlg, IDC_SLIDER_RUMBLE, IDC_TEXT_RUMBLE);
			}
			else  if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE))
			{
				settings[port].stickL.deadzone =  (f32)SliderUpdate(hwndDlg, IDC_SLIDER_DEADZONE, IDC_TEXT_DEADZONE);
				if(settings[port].sticksLocked) SliderSet(hwndDlg, IDC_SLIDER_DEADZONE2, settings[port].stickL.deadzone);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_ANTIDEADZONE))
			{
				settings[port].stickL.antiDeadzone =   (f32)SliderUpdate(hwndDlg, IDC_SLIDER_ANTIDEADZONE, IDC_TEXT_ANTIDEADZONE);
				if(settings[port].sticksLocked) SliderSet(hwndDlg, IDC_SLIDER_ANTIDEADZONE2, settings[port].stickL.antiDeadzone);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_LINEARITY))
			{
				settings[port].stickL.linearity =  SliderUpdate(hwndDlg, IDC_SLIDER_LINEARITY, IDC_TEXT_LINEARITY, true);
				if(settings[port].sticksLocked) SliderSet(hwndDlg, IDC_SLIDER_LINEARITY2, settings[port].stickL.linearity, true);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE2))
			{
				settings[port].stickR.deadzone =  (f32)SliderUpdate(hwndDlg, IDC_SLIDER_DEADZONE2, IDC_TEXT_DEADZONE2);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_ANTIDEADZONE2))
			{
				settings[port].stickR.antiDeadzone =   (f32)SliderUpdate(hwndDlg, IDC_SLIDER_ANTIDEADZONE2, IDC_TEXT_ANTIDEADZONE2);
			}
			else if((HWND)lParam == GetDlgItem(hwndDlg, IDC_SLIDER_LINEARITY2))
			{
				settings[port].stickR.linearity =  SliderUpdate(hwndDlg, IDC_SLIDER_LINEARITY2, IDC_TEXT_LINEARITY2, true);
			}

		} break;

	case WM_COMMAND:
		{
			s16 command = LOWORD(wParam);

			switch(command)
			{
			case IDC_XINPUT1: settings[port].xinputPort = 0; break;
			case IDC_XINPUT2: settings[port].xinputPort = 1; break;
			case IDC_XINPUT3: settings[port].xinputPort = 2; break;
			case IDC_XINPUT4: settings[port].xinputPort = 3; break;

			case IDC_MODE_DIGITAL: settings[port].defaultAnalog = false; break;
			case IDC_MODE_ANALOG: settings[port].defaultAnalog = true; break;

			case IDC_COMBO_LX: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_LX] = (u8)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_LX), CB_GETCURSEL, 0, 0);
				break;
			case IDC_COMBO_LY: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_LY] = (u8)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_LY), CB_GETCURSEL, 0, 0);
				break;
			case IDC_COMBO_RX: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_RX] = (u8)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_RX), CB_GETCURSEL, 0, 0);
				break;
			case IDC_COMBO_RY: if(HIWORD(wParam) == CBN_SELCHANGE)
				settings[port].axisRemap[GP_AXIS_RY] = (u8)SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_RY), CB_GETCURSEL, 0, 0);
				break;

			case IDC_INVERT_LX:
				settings[port].axisInverted[GP_AXIS_LX] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_LX) == BST_CHECKED;
				break;
			case IDC_INVERT_LY:
				settings[port].axisInverted[GP_AXIS_LY] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_LY) == BST_CHECKED;
				break;
			case IDC_INVERT_RX:
				settings[port].axisInverted[GP_AXIS_RX] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_RX) == BST_CHECKED;
				break;
			case IDC_INVERT_RY:
				settings[port].axisInverted[GP_AXIS_RY] = IsDlgButtonChecked(hwndDlg, IDC_INVERT_RY) == BST_CHECKED;
				break;

			case IDC_GUITAR:
				settings[port].isGuitar = IsDlgButtonChecked(hwndDlg, IDC_GUITAR) == BST_CHECKED;
				break;

			case IDC_DISABLED:
				settings[port].disabled = IsDlgButtonChecked(hwndDlg, IDC_DISABLED) == BST_CHECKED;
				break;

			case IDC_ANALOG_GREEN:
				settings[port].greenAnalog = IsDlgButtonChecked(hwndDlg, IDC_ANALOG_GREEN) == BST_CHECKED;
				break;

			case IDC_LOCKSLIDERS:
				settings[port].sticksLocked = IsDlgButtonChecked(hwndDlg, IDC_LOCKSLIDERS) == BST_CHECKED;

				EnableWindow(GetDlgItem(hwndDlg, IDC_SLIDER_DEADZONE2), !settings[port].sticksLocked);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SLIDER_ANTIDEADZONE2), !settings[port].sticksLocked);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SLIDER_LINEARITY2), !settings[port].sticksLocked);
				break;

			case IDC_4WAYSTICK:
				//settings[port].b4wayStick = IsDlgButtonChecked(hwndDlg, IDC_4WAYSTICK) == BST_CHECKED;
				break;

			case IDC_SWAPSTICKS:
				settings[port].SwapSticksEnabled = IsDlgButtonChecked(hwndDlg, IDC_SWAPSTICKS) == BST_CHECKED;
				break;

			case IDC_SWAPXO:
				settings[port].SwapXO = IsDlgButtonChecked(hwndDlg, IDC_SWAPXO) == BST_CHECKED;
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

			//if(dcPlatform != 2 && (!isPSemulator || multitap > 0))
			if (!isPSemulator || multitap > 0)
			{
				tci.pszText = L"Controller 3";
				TabCtrl_InsertItem(hTabControl, 2, &tci);

				tci.pszText = L"Controller 4";
				TabCtrl_InsertItem(hTabControl, 3, &tci);
			}

			hChild = CreateDialog((HINSTANCE)lParam, MAKEINTRESOURCE(IDD_INTAB), hwndDlg, DialogProc2);
			EnableThemeDialogTexture(hChild, ETDT_ENABLETAB);

			EnableWindow(GetDlgItem(hwndDlg, IDC_MULTITAP), isPSemulator);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SWAPPORTS), isPSemulator);
			
			CheckDlgButton(hwndDlg, IDC_SWAPPORTS, SwapPortsEnabled ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_PROCPRIORITY, bPriority ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_SCREENSAVER, bKeepAwake ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_MULTITAP, multitap % 4);

			ShowWindow(hwndDlg, SW_SHOW);
		} break;

	case WM_COMMAND:
		{
			s16 command = LOWORD(wParam);

			switch(command)
			{
			case IDC_PROCPRIORITY:
				bPriority = IsDlgButtonChecked(hwndDlg, IDC_PROCPRIORITY) == BST_CHECKED;
				break;

			case IDC_SWAPPORTS:
				SwapPortsEnabled = IsDlgButtonChecked(hwndDlg, IDC_SWAPPORTS) == BST_CHECKED;
				break;



			case IDC_SCREENSAVER:
				bKeepAwake = IsDlgButtonChecked(hwndDlg, IDC_SCREENSAVER) == BST_CHECKED;
				break;

			case IDC_MULTITAP:
				{
					multitap = IsDlgButtonChecked(hwndDlg, IDC_MULTITAP) & 0xFF;
					HWND hTabControl = GetDlgItem(hwndDlg, IDC_TAB1);
					u8 tabs = (u8)TabCtrl_GetItemCount(hTabControl);

					if(multitap > 0)
					{
						if(tabs < 3)
						{
							TCITEM tci;
							tci.mask = TCIF_TEXT | TCIF_IMAGE;
							tci.iImage = -1;

							tci.pszText = L"Controller 3";
							TabCtrl_InsertItem(hTabControl, 2, &tci);

							tci.pszText = L"Controller 4";
							TabCtrl_InsertItem(hTabControl, 3, &tci);
						}
					}
					else
					{
						TabCtrl_SetCurSel(hTabControl, 0);

						TabCtrl_DeleteItem(hTabControl, 3);
						TabCtrl_DeleteItem(hTabControl, 2);
					}

					RedrawWindow(hChild, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
				}
				break;

			case IDRESET:
				{
					s32 port =  TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_TAB1));
					settings[port].SetDefaults();
					SendMessage(hChild, WM_USER, 0xDEADBEEF, port);
				}
				break;

			case ID_OK:
					FileIO::INI_SaveSettings(); // SAVE SETTINGS
			case ID_CANCEL:
					EndDialog(hwndDlg, command); // .. and QUIT
					PostQuitMessage(0);
					break;
			}

		} break;

	case WM_NOTIFY:
			if ( ((LPNMHDR)lParam)->idFrom==IDC_TAB1 && ((LPNMHDR)lParam)->code == TCN_SELCHANGE  )
			{
				s32 port =  TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_TAB1));
				SendMessage(hChild, WM_USER, 0xDEADBEEF, port);
			}
			break;

	default: return FALSE;
	}

	return TRUE;
}

void CreateConfigDialog()
{
	hParent = GetActiveWindow();

	DialogBoxParam(h_instance, MAKEINTRESOURCE(IDD_CONFIG), hParent, DialogProc, (LPARAM)h_instance);
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
#endif // WIN32

#ifdef __linux__

void CreateConfigDialog()
{
	// There is no config dialog, so at least save some settings.
	FileIO::INI_SaveSettings();
}

#endif
