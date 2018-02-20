#include "FEditAddress.h"
#include "FMemViewer.h"
#include "SpeedHack.h"

#define _DONATE_	TRUE
//----------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK DialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	hDlgProc = hDlg;

	switch(Msg)
	{
		case WM_INITDIALOG:
			DialogUtils.SetFont(hDlg, lf, hFont, hDC, IDC_BTNDONATE, L"Lucida Console", 11, true);

			//add list combobox
			SetDlgItemTextA(hDlg, IDC_EDSTARTADDR, "00400000");
			SetDlgItemTextA(hDlg, IDC_EDENDADDR, "1F000000");
			SetDlgItemTextA(hDlg, IDC_EDSPEEDHACK, "1");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_ADDSTRING, 0, (LPARAM)"Byte");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_ADDSTRING, 0, (LPARAM)"2 Byte");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_ADDSTRING, 0, (LPARAM)"4 Byte");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_ADDSTRING, 0, (LPARAM)"Text");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_ADDSTRING, 0, (LPARAM)"Array Of Byte");
			SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_SETCURSEL, 0, 0);
			EnableWindow(GetDlgItem(hDlg, IDC_BTNNEXTSCAN), FALSE);
			break;

		case WM_CLOSE:
			ExitProcess(0);
			EndDialog(hDlg, 0);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BTNSCAN:
					{
#if !_DONATE_
						int iTypeScan = (int)SendDlgItemMessageA(hDlg, IDC_CBXTYPE, CB_GETCURSEL, 0, 0);
						if(iTypeScan == 4)
						{
							MessageBoxA(0, "Untuk menggunakan fitur ini, silahkan donasi terlebih dahulu", "Informasi", MB_ICONINFORMATION);
							break;
						}
#endif
						//Scan
						char buf[10];
						GetDlgItemTextA(hDlg, IDC_BTNSCAN, buf, 10);

						if(!strcmp(buf, "Scan"))//jika text button = scan
						{
							SendDlgItemMessageA(hDlg, IDC_LIST2, LB_RESETCONTENT, 0, 0);
							SendDlgItemMessageA(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);
							CreateThread(0, 0, (LPTHREAD_START_ROUTINE)tScan, 0, 0, 0);
							CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ElapsedScan, 0, 0, 0);
							SetDlgItemTextA(hDlg, IDC_BTNSCAN, "New Scan");
							EnableWindow(GetDlgItem(hDlg, IDC_BTNNEXTSCAN), TRUE);
							EnableWindow(GetDlgItem(hDlg, IDC_CBXTYPE), FALSE);
						}else
						{
							bcancel = true;//buat cancel scan
							SendDlgItemMessageA(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);
							SendDlgItemMessageA(hDlg, IDC_LIST2, LB_RESETCONTENT, 0, 0);
							SetDlgItemTextA(hDlg, IDC_FOUND, "Found : 0");
							SetDlgItemTextA(hDlg, IDC_ELAPSEDSCAN, "Elapsed Scan (second) : 0");
							SetDlgItemTextA(hDlg, IDC_BTNSCAN, "Scan");
							EnableWindow(GetDlgItem(hDlg, IDC_BTNNEXTSCAN), FALSE);
							EnableWindow(GetDlgItem(hDlg, IDC_CBXTYPE), TRUE);
						}
					}
					break;

				case IDC_BTNNEXTSCAN:
					{
						EnableWindow(GetDlgItem(hDlg, IDC_BTNNEXTSCAN), FALSE);
						SendDlgItemMessage(hDlg, IDC_LIST2, LB_RESETCONTENT, 0, 0);
						CreateThread(0, 0, (LPTHREAD_START_ROUTINE)tNextScan, 0, 0, 0);
						CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ElapsedScan, 0, 0, 0);
					}
					break;

				case IDC_BTNPRINTMEM:
					DialogBoxParamA(hCurrentModule, MAKEINTRESOURCEA(IDD_DIALOG2), NULL, DLGPROC(MemViewer), NULL);
					break;

				case IDC_BTNADDRLIST:
					DialogBoxParamA(hCurrentModule, MAKEINTRESOURCEA(IDD_DIALOG4), NULL, DLGPROC(EditAddress), NULL);
					break;

				case IDC_LIST1:
					{
						// Get current selection index in listbox
						int itemIndex = (int)SendDlgItemMessageA(hDlg, IDC_LIST1, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
						if (itemIndex != LB_ERR)
						{
							// Get length of text in listbox
							int textLen = (int)SendDlgItemMessageA(hDlg, IDC_LIST1, LB_GETTEXTLEN, (WPARAM)itemIndex, 0);
							// Allocate buffer to store text (consider +1 for end of string)
							char *textBuffer = new char[textLen + 1];
							// Get actual text in buffer
							SendDlgItemMessageA(hDlg, IDC_LIST1, LB_GETTEXT, (WPARAM)itemIndex, (LPARAM)textBuffer);
							// set to textbox
							SetDlgItemTextA(hDlg, IDC_EDADDRSEL, textBuffer);
							// Free text
							delete []textBuffer;
							// Avoid dangling references
							textBuffer = NULL;
						}
					}
					break;

				case IDC_LIST2:
				{
					// Get current selection index in listbox
					int itemIndex = (int)SendDlgItemMessageA(hDlg, IDC_LIST2, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
					if (itemIndex != LB_ERR)
					{
						// Get length of text in listbox
						int textLen = (int)SendDlgItemMessageA(hDlg, IDC_LIST2, LB_GETTEXTLEN, (WPARAM)itemIndex, 0);
						// Allocate buffer to store text (consider +1 for end of string)
						char *textBuffer = new char[textLen + 1];
						// Get actual text in buffer
						SendDlgItemMessageA(hDlg, IDC_LIST2, LB_GETTEXT, (WPARAM)itemIndex, (LPARAM)textBuffer);
						// set to textbox
						SetDlgItemTextA(hDlg, IDC_EDADDRSEL, textBuffer);
						// Free text
						delete []textBuffer;
						// Avoid dangling references
						textBuffer = NULL;
					}
				}
				break;

				case IDC_BTNCOPY:
					{
						char txtbuf[10];
						HWND hwnd = GetDesktopWindow();
						GetDlgItemTextA(hDlg, IDC_EDADDRSEL, txtbuf, 9);
						DialogUtils.ToClipboard(hwnd, txtbuf);
					}
					break;

				case IDC_BTNDONATE:
					{
						MessageBoxA(0, "Jika anda menyukai JMS ini, anda dapat membantu pengembangan JMS ini dengan donasi.\n"
							"Dengan nominal berapapun anda sudah menjadi donatur JMS ini dan tentunya saya senang sekali.\n\n"
							"Donasi VIA Bank BCA : \n"
							"REK : 2290031551\n"
							"A/N : Jazzy Becker Islam S.\n\n"
							"Atau anda juga dapat donasi VIA PayPal (setelah ini anda akan di tujukan ke url PayPal).\n\n"
							"Terima Kasih.", "Informasi Donasi", MB_ICONINFORMATION);
						ShellExecuteA(NULL, "Open", "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DZ2HRUEBGCY4S", NULL, NULL, SW_SHOWNORMAL);
					}
					break;

				case IDC_CHKSPEEDHACK:
					{
						if(IsDlgButtonChecked(hDlg, IDC_CHKSPEEDHACK))
						{
#if !_DONATE_
							MessageBoxA(0, "Untuk menggunakan fitur ini, silahkan donasi terlebih dahulu", "Informasi", MB_ICONINFORMATION);
							break;
#endif

							char buff[4];
							GetDlgItemTextA(hDlg, IDC_EDSPEEDHACK, buff, 3);
							SpeedHack::SpeedHack(true);
							SpeedHack::SetSpeed((float)atoi(buff));
							EnableWindow(GetDlgItem(hDlg, IDC_EDSPEEDHACK), FALSE);
						}
						else
						{
							SpeedHack::SpeedHack(false);
							EnableWindow(GetDlgItem(hDlg, IDC_EDSPEEDHACK), TRUE); 
						}
					}
					break;
			}
			break;

		default:
			return FALSE;
			break;
	}

	return FALSE;
}