#include "Win32.h"

PVOID GetProcessImageBase(IN HANDLE hProcess) {
	PROCESS_BASIC_INFORMATION   pbi = { 0 };
	ULONG                       uReturnLength = 0;
	ULONGLONG                   uImageBase = 0;
	SIZE_T                      sBytesRead = 0;
	ULONGLONG                   uBaseAddress = 0;
	NTSTATUS                    ntStatus = 0;
	NTQIP						pNtQueryInformationProcess = NULL;


	if (hProcess == NULL) {
		uImageBase = *(ULONGLONG*)((PBYTE)NtCurrentTeb()->ProcessEnvironmentBlock + 0x10);
	}
	else {
		/* retreive image base from remote process PEB */
		pNtQueryInformationProcess = GetProcAddress(GetModuleHandle(L"Ntdll"), "NtQueryInformationProcess");
		
		if (!NT_SUCCESS(ntStatus = pNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &uReturnLength))) {
			PRINT("NtQueryInformationProcess() Failed: %d", ntStatus);
			return (NULL);
		}

		uBaseAddress = (ULONGLONG)((PBYTE)pbi.PebBaseAddress + 0x10);

		if (!ReadProcessMemory(hProcess, (LPCVOID)uBaseAddress, &uImageBase, sizeof(ULONGLONG), &sBytesRead)) {
			PRINT("ReadProcessMemory() Failed: %d", GetLastError());
			return (NULL);
		}
	}

	return ((PVOID)uImageBase);
}