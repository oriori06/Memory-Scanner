#include "Main.h"
#include "CMemoryScanner.h"
#include "MemUtils.h"

using namespace std;
//----------------------------------------------------------------------------------------------------------------------

string str;

void CMemoryScanner::InitAddr(HWND hDlg, DWORD dwAddr, DWORD dwOffs)
{
	BYTE bVal;
	//if(ReadProcessMemory(GetCurrentProcess(), (void*)(dwAddr+dwOffs), &bVal, 1, NULL))
	if (IsBadReadPtr((PDWORD)(dwAddr + dwOffs), 1) == 0)
	{
		bVal = *(PBYTE)(dwAddr + dwOffs);
		SetDlgItemTextA(hDlg, dwOffs, IntToHex(bVal).c_str());
	}
	else
	{
		str += ". ";
		SetDlgItemTextA(hDlg, dwOffs, "??");
	}
}

void CMemoryScanner::Value2Char(HWND hDlg, int nIDDlgItem, DWORD dwAddr)
{
	char buf[10];
	string str;
	BYTE bVal;
	int j = 0;

	//SetDlgItemTextA(hDlg, ID, ". . . . . . . . . . . . . . . .");
	for (int IndexChar = 0; IndexChar <= 255; IndexChar += 16)
	{
		for (int i = 0; i <= 15; i++)
		{
			//if(ReadProcessMemory(GetCurrentProcess(), (void*)(dwAddr+IndexChar+i), &bVal, 1, NULL))
			if (IsBadReadPtr((PDWORD)(dwAddr + IndexChar + i), 1) == 0)
			{
				bVal = *(PBYTE)(dwAddr + IndexChar + i);
				if (bVal == 20)str += " ";
				else if (bVal < 33)str += ". ";
				else
				{
					sprintf_s(buf, "%c", bVal);
					str += buf;
				}
			}
		}

		SetDlgItemTextA(hDlg, nIDDlgItem + j, str.c_str());
		str = "";
		j++;
	}
}

void CMemoryScanner::EditMemory(DWORD dwAddress, BYTE bVal)
{
	DWORD dwOld;
	if (VirtualProtect((void*)dwAddress, 1, 0x40, &dwOld))//jika sukses
	{
		if (IsBadReadPtr((PDWORD)dwAddress, 1) == 0)
		{
			//memcpy((void*)dwAddress, (void*)&bVal, 1);
			*(PBYTE)dwAddress = bVal;
			VirtualProtect((void*)dwAddress, 1, dwOld, &dwOld);
		}
	}
}

void CMemoryScanner::EditMemory(DWORD dwAddress, DWORD dwVal, DWORD type)
{
	DWORD dwOld;
	if (VirtualProtect((void*)dwAddress, 1, 0x40, &dwOld))//jika sukses
	{
		if (IsBadReadPtr((PDWORD)dwAddress, type) == 0)
		{
			memcpy((void*)dwAddress, (void*)&dwVal, type);
			VirtualProtect((void*)dwAddress, 1, dwOld, &dwOld);
		}
	}
}

void CMemoryScanner::EditString(DWORD dwAddress, string strVal)
{
	DWORD dwOld;
	if (VirtualProtect((void*)dwAddress, strVal.length(), 0x40, &dwOld))//jika sukses
	{
		for (DWORD i = 0; i <= strVal.length(); i++)
			memcpy((void*)(dwAddress + i), (void*)&strVal[i], 1);

		VirtualProtect((void*)dwAddress, strVal.length(), dwOld, &dwOld);
	}
}