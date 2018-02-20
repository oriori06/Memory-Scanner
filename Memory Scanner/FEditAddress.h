#include "resource.h"
#include "MemUtils.h"

extern CMemoryScanner MemoryScanner;

enum
{
	TIMER3
};

//----------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK EditAddress(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf1[9], buf2[1024], buf3[100];

	switch(uMsg)
	{
		case WM_INITDIALOG:
			for(int i = 0; i <= 9; i++)
			{
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_ADDSTRING, 0, (LPARAM)"Byte");
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_ADDSTRING, 0, (LPARAM)"2 Byte");
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_ADDSTRING, 0, (LPARAM)"4 Byte");
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_ADDSTRING, 0, (LPARAM)"Text");
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_ADDSTRING, 0, (LPARAM)"Array Of Byte");
				SendDlgItemMessageA(hDlg, IDC_CBXEDITVAL1 + i, CB_SETCURSEL, 0, 0);
			}

			SetTimer(hDlg, TIMER3, 50, 0);
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;

		case WM_TIMER:
			switch(LOWORD(wParam))
			{
				case TIMER3:
					{
						for(int i = 0; i <= 9; i++)
						{
							if(IsDlgButtonChecked(hDlg, IDC_CHKFRZ1 + i))
							{
								GetDlgItemTextA(hDlg, IDC_CBXEDITVAL1 + i, buf3, 100);
								GetDlgItemTextA(hDlg, IDC_EDITADDRVALUE1 + i, buf1, 9);
								GetDlgItemTextA(hDlg, IDC_EDITADDRENEWVAL1 + i, buf2, 1024);

								if(strcmp(buf3, "Byte") == 0)
									MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 1);
								else if(strcmp(buf3, "2 Byte") == 0)
									MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 2);
								else if(strcmp(buf3, "4 Byte") == 0)
									MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 4);
								else if(strcmp(buf3, "Text") == 0)
									MemoryScanner.EditString(hextoint(buf1), buf2);
							}
						}
					}
					break;
			}
			break;

		case WM_COMMAND:
			{
				for(int i = 0; i <= 9; i++)
				{
					if(LOWORD(wParam) == IDC_BTNCHGVAL1 + i)
					{
						GetDlgItemTextA(hDlg, IDC_CBXEDITVAL1 + i, buf3, 100);
						GetDlgItemTextA(hDlg, IDC_EDITADDRVALUE1 + i, buf1, 9);
						GetDlgItemTextA(hDlg, IDC_EDITADDRENEWVAL1 + i, buf2, 1024);

						if(strcmp(buf3, "Byte") == 0)
							MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 1);
						else if(strcmp(buf3, "2 Byte") == 0)
							MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 2);
						else if(strcmp(buf3, "4 Byte") == 0)
							MemoryScanner.EditMemory(hextoint(buf1), str2int(buf2), 4);
						else if(strcmp(buf3, "Text") == 0)
							MemoryScanner.EditString(hextoint(buf1), buf2);
					}
					
					if(LOWORD(wParam) == IDC_CHKFRZ1 + i)
					{
						if(IsDlgButtonChecked(hDlg, IDC_CHKFRZ1 + i))
						{
							EnableWindow(GetDlgItem(hDlg, IDC_EDITADDRVALUE1 + i), FALSE);
							EnableWindow(GetDlgItem(hDlg, IDC_EDITADDRENEWVAL1 + i), FALSE);
						}
						else
						{
							EnableWindow(GetDlgItem(hDlg, IDC_EDITADDRVALUE1 + i), TRUE);
							EnableWindow(GetDlgItem(hDlg, IDC_EDITADDRENEWVAL1 + i), TRUE);
						}
					}
				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return FALSE;
}