#include "resource.h"

#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

HWND hDlgProc = NULL;

extern CMemFunctions *MemFunctions;
//----------------------------------------------------------------------------------------------------------------------

void _stdcall FinishFirstScan()
{
	HWND hWndList1 = GetDlgItem(hDlgProc, IDC_LIST1);
	_ASSERTE(hWndList1 != NULL);
	int itemCount = (int)SendMessageA(hWndList1, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	if (itemCount != LB_ERR)
	{
		string sfound = "Found : ";
		sfound += IntToStr(itemCount);
		SetDlgItemTextA(hDlgProc, IDC_FOUND, sfound.c_str());
	}

	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), TRUE);
}

//----------------------------------------------------------------------------------------------------------------------

void _stdcall FinishNextScan()
{
	SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_RESETCONTENT, 0, 0);
	HWND hWndList2 = GetDlgItem(hDlgProc, IDC_LIST2);
	_ASSERTE(hWndList2 != NULL);
	int itemCount = (int)SendMessageA(hWndList2, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	if (itemCount != LB_ERR)
	{
		string sfound = "Found : ";
		sfound += IntToStr(itemCount);
		SetDlgItemTextA(hDlgProc, IDC_FOUND, sfound.c_str());

		for (int i = 0; i <= itemCount - 1; i++)
		{
			//Get length of text in listbox
			int textLen = (int)SendMessageA(hWndList2, LB_GETTEXTLEN, (WPARAM)i, 0);
			//Allocate buffer to store text (consider +1 for end of string)
			char buffer[256];
			//Get actual text in buffer
			SendMessageA(hWndList2, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);
			//add to list1
			SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)buffer);
		}
	}

	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), TRUE);
}

//----------------------------------------------------------------------------------------------------------------------

bool timer = true, bcancel = false;

void _fastcall Scan(DWORD dwStart, DWORD dwEnd, DWORD dwVal, DWORD dwLength)
{
	MEMORY_BASIC_INFORMATION mbi;
	DWORD temp = 0;
	ULONG BytesReaded = NULL;
	char val[25];

	bcancel = false;
	timer = true;

	while (1)
	{
		if (bcancel)//batalkan scan
			goto cancel;

		VirtualQuery((void*)dwStart, &mbi, sizeof(mbi));
		if (mbi.State & MEM_COMMIT && mbi.Protect & WRITABLE)
		{
			if (bcancel)//batalkan scan
				goto cancel;

			for (DWORD i = dwStart; i <= (DWORD)mbi.BaseAddress + mbi.RegionSize; i += dwLength)
			{
				if (bcancel)//batalkan scan
					goto cancel;

				if (MemFunctions->NtReadVirtualMemory(GetCurrentProcess(), (void*)i, &temp, dwLength, &BytesReaded) == 0)
				{
					if (temp == dwVal)
					{
						sprintf_s(val, "%08X      %d", i, temp);
						SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)val);
					}
				}
			}

			dwStart += mbi.RegionSize;
		}
		else dwStart = (DWORD)mbi.BaseAddress + mbi.RegionSize;

		if (dwStart > dwEnd)
		{
			timer = false;
			FinishFirstScan();
			SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                    Status : Finish!");
			ExitThread(0);
		}
	}

cancel:
	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), TRUE);
	timer = false;
	SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                   Status : Cancel!");
	ExitThread(0);
}

//----------------------------------------------------------------------------------------------------------------------

void _fastcall ScanText(DWORD dwStart, DWORD dwEnd, char *text)
{
	MEMORY_BASIC_INFORMATION mbi;
	ULONG BytesReaded = NULL;
	char val[1024], bchar[1024];
	string temp;

	bcancel = false;
	timer = true;

	while (1)
	{
		if (bcancel)//batalkan scan
			goto cancel;

		VirtualQuery((void*)dwStart, &mbi, sizeof(mbi));
		if (mbi.State & MEM_COMMIT && mbi.Protect & WRITABLE)
		{
			if (bcancel)//batalkan scan
				goto cancel;

			for (DWORD i = dwStart; i <= (DWORD)mbi.BaseAddress + mbi.RegionSize; i++)
			{
				if (bcancel)//batalkan scan
					goto cancel;

				if (MemFunctions->NtReadVirtualMemory(GetCurrentProcess(), (void*)i, &bchar, strlen(text), &BytesReaded) == 0)
				{
					if (strcmp(bchar, text) == 0)
					{
						sprintf_s(val, "%08X      %s", i, text);
						SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)val);
						i += strlen(text) + 1;
					}
				}
			}

			dwStart += mbi.RegionSize;
		}
		else dwStart = (DWORD)mbi.BaseAddress + mbi.RegionSize;

		if (dwStart > dwEnd)
		{
			timer = false;
			FinishFirstScan();
			SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                    Status : Finish!");
			ExitThread(0);
		}
	}

cancel:
	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), TRUE);
	timer = false;
	SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                   Status : Cancel!");
	ExitThread(0);
}

//----------------------------------------------------------------------------------------------------------------------

void _fastcall ScanArrayOfByte(DWORD dwStart, DWORD dwEnd, char *ArrayOfByte)
{
	MEMORY_BASIC_INFORMATION mbi;
	char val[1024];
	const char* pat = ArrayOfByte;
	DWORD firstMatch = NULL;
	BYTE bCur = NULL;

	bcancel = false;
	timer = true;

	while (1)
	{
		if (bcancel)//batalkan scan
			goto cancel;

		VirtualQuery((void*)dwStart, &mbi, sizeof(mbi));
		if (mbi.State & MEM_COMMIT && mbi.Protect & WRITABLE)
		{
			if (bcancel)//batalkan scan
				goto cancel;

			for (DWORD i = dwStart; i <= (DWORD)mbi.BaseAddress + mbi.RegionSize; i++)
			{
				if (bcancel)//batalkan scan
					goto cancel;

				if (!*pat)
				{
					sprintf_s(val, "%08X      %s", firstMatch, ArrayOfByte);
					SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)val);
					i += strlen(ArrayOfByte) + 1;
				}

				if (IsBadReadPtr((PBYTE)i, 1) == 0)
					bCur = *(PBYTE)i;
				else bCur = 0;
				if (*(PBYTE)pat == '\?' || bCur == getByte(pat))
				{
					if (!firstMatch)
						firstMatch = i;
					if (!pat[2])
					{
						sprintf_s(val, "%08X      %s", firstMatch, ArrayOfByte);
						SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)val);
						i += strlen(ArrayOfByte) + 1;
					}

					if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
						pat += 3;
					else pat += 2;
				}
				else
				{
					pat = ArrayOfByte;
					firstMatch = 0;
				}
			}

			dwStart += mbi.RegionSize;
		}
		else dwStart = (DWORD)mbi.BaseAddress + mbi.RegionSize;

		if (dwStart > dwEnd)
		{
			timer = false;
			FinishFirstScan();
			SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                    Status : Finish!");
			ExitThread(0);
		}
	}

cancel:
	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), TRUE);
	timer = false;
	SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                   Status : Cancel!");
	ExitThread(0);
}

//----------------------------------------------------------------------------------------------------------------------

void _fastcall ElapsedScan()
{
	int i = 0;
	char buf[100];
	timer = true;

	while (1)
	{
		if (!timer)
			ExitThread(0);

		sprintf_s(buf, "Elapsed Scan (second) : %d", i);
		SetDlgItemTextA(hDlgProc, IDC_ELAPSEDSCAN, buf);
		i++;

		Sleep(900);
	}
}

//----------------------------------------------------------------------------------------------------------------------

DWORD type = NULL;

void tScan()
{
	char chValue[255], chType[20], chStart[20], chEnd[20];

	EnableWindow(GetDlgItem(hDlgProc, IDC_BTNNEXTSCAN), FALSE);
	SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :           Status : Scanning ...");
	type = 1;

	//dapatkan string pada component
	GetDlgItemTextA(hDlgProc, IDC_EDVALUE, chValue, 11);
	GetDlgItemTextA(hDlgProc, IDC_CBXTYPE, chType, 19);
	GetDlgItemTextA(hDlgProc, IDC_EDSTARTADDR, chStart, 9);
	GetDlgItemTextA(hDlgProc, IDC_EDENDADDR, chEnd, 9);

	DWORD Start = hextoint(chStart);
	DWORD End = hextoint(chEnd);

	//jika string sama
	if (strcmp(chType, "Byte") == 0)
		type = 1;
	else if (strcmp(chType, "2 Byte") == 0)
		type = 2;
	else if (strcmp(chType, "4 Byte") == 0)
		type = 4;

	if (strcmp(chType, "Text") == 0)
		ScanText(Start, End, chValue);
	else if (strcmp(chType, "Array Of Byte") == 0)
		ScanArrayOfByte(Start, End, chValue);
	else Scan(Start, End, str2int(chValue), type);
}

//----------------------------------------------------------------------------------------------------------------------

void NextScan(int iCase)
{
	char buf[20], textBuffer[1024], tempval[1024];
	DWORD dwtemp = NULL;
	string stemp;
	ULONG BytesReaded = NULL;

	//dapatkan string pada component
	GetDlgItemTextA(hDlgProc, IDC_EDVALUE, buf, 11);
	const char* pat = buf;
	DWORD firstMatch = NULL;
	BYTE bCur = NULL;

	HWND hWndList1 = GetDlgItem(hDlgProc, IDC_LIST1);
	_ASSERTE(hwndList1 != NULL);
	int itemCount = (int)SendMessage(hWndList1, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	if (itemCount != LB_ERR)
	{
		for (int i = 0; i < itemCount; i++)
		{
			SendMessageA(hWndList1, LB_GETTEXT, (WPARAM)i, (LPARAM)textBuffer);
			textBuffer[9] = '\0';
			UINT addr = hextoint(textBuffer);//berisi value dari listbox di konversi ke numeric

			switch (iCase)
			{
			case 0: //BYTE, WORD, DWORD
				if (MemFunctions->NtReadVirtualMemory(GetCurrentProcess(), (void*)addr, &dwtemp, type, &BytesReaded) == 0)
				{
					if (dwtemp == str2int(buf))
					{
						sprintf_s(tempval, "%08X      %d", addr, dwtemp);
						SendDlgItemMessageA(hDlgProc, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)tempval);
					}
				}
				break;

			case 1: //TEXT
				if (MemFunctions->NtReadVirtualMemory(GetCurrentProcess(), (void*)addr, &stemp, strlen(buf) + 1, &BytesReaded) == 0)
				{
					if (stemp.length() > strlen(buf))
						stemp[strlen(buf)] = '\0';

					if (strcmp(stemp.c_str(), buf) == 0)
					{
						sprintf_s(tempval, "%08X      %s", addr, buf);
						SendDlgItemMessageA(hDlgProc, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)tempval);
					}
				}
				break;

			case 2:
				if (!*pat)
				{
					sprintf_s(tempval, "%08X      %s", firstMatch, buf);
					SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)tempval);
					i++;
				}

				if (IsBadReadPtr((PBYTE)i, 1) == 0)
					bCur = *(PBYTE)i;
				else bCur = 0;
				if (*(PBYTE)pat == '\?' || bCur == getByte(pat))
				{
					if (!firstMatch)
						firstMatch = i;
					if (!pat[2])
					{
						sprintf_s(tempval, "%08X      %s", firstMatch, buf);
						SendDlgItemMessageA(hDlgProc, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)tempval);
						i++;
					}

					if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
						pat += 3;
					else pat += 2;
				}
				else
				{
					pat = buf;
					firstMatch = 0;
				}
				break;
			}
		}

		SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :                    Status : Finish!");
		timer = false;
		FinishNextScan();
	}
}

//----------------------------------------------------------------------------------------------------------------------

void tNextScan()
{
	char chType[20];
	GetDlgItemTextA(hDlgProc, IDC_CBXTYPE, chType, 19);
	SetDlgItemTextA(hDlgProc, IDC_RESULT, "Result :           Status : Scanning ...");

	if ((strcmp(chType, "Byte") == 0) ||
		(strcmp(chType, "2 Byte") == 0) ||
		(strcmp(chType, "4 Byte") == 0))
		NextScan(0);
	else if (strcmp(chType, "Text") == 0)
		NextScan(1);
	else if (strcmp(chType, "Array Of Byte") == 0)
		NextScan(2);
}
