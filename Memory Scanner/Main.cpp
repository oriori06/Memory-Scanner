#include "Main.h"
#include "MemFunctions.h"
#include "CMemoryScanner.h"
#include "CDialogUtils.h"
#include "FMemScan.h"

CMemFunctions *MemFunctions = NULL;
//----------------------------------------------------------------------------------------------------------------------

void _stdcall MainThread()
{
	DialogBoxParamA(hCurrentModule, MAKEINTRESOURCEA(IDD_DIALOG1), NULL, DLGPROC(DialogProc), NULL);
}

//----------------------------------------------------------------------------------------------------------------------

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hDll);

		if (MemFunctions == nullptr)
			MemFunctions = new CMemFunctions();

		hCurrentModule = hDll;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, NULL, NULL, NULL);
		break;

	case DLL_PROCESS_DETACH:
		if (MemFunctions)
		{
			MemFunctions->~CMemFunctions();
			delete MemFunctions;
		}
		break;
	}

	return TRUE;
}
