#pragma once

class CMemoryScanner
{
public:
	void InitAddr(HWND hDlg, DWORD dwAddr, DWORD dwOffs);
	void Value2Char(HWND hDlg, int nIDDlgItem, DWORD dwAddr);
	void EditMemory(DWORD dwAddress, BYTE bVal);
	void EditMemory(DWORD dwAddress, DWORD dwVal, DWORD type);
	void EditString(DWORD dwAddress, std::string strVal);
};
