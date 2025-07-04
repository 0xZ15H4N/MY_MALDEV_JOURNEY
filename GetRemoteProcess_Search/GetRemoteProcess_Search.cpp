#include<Windows.h>
#include<important.h>
#include<psapi.h>



BOOL GetRemoteProcessHandle(LPCWSTR szProcName,DWORD* pdwPid,HANDLE* phProcess) {
	DWORD adwProcesses[1024 * 2],
		dwReturnLen1 = NULL,
		dwReturnLen2 = NULL,
		dwNmbrOfPids = NULL;
	HANDLE hProcess = NULL;
	HMODULE hModule = NULL;
	WCHAR szProc[MAX_PATH];
	// Get the array of PIDs
	if (!EnumProcesses(adwProcesses, sizeof(adwProcesses), &dwReturnLen1)) {
		printf("[!] EnumProcesses Failed With Error : %d \n", GetLastError());
		return FALSE;
	}

	dwNmbrOfPids = dwReturnLen1 / sizeof(DWORD);
	printf("[i] Number Of Processes Detected : %d \n", dwNmbrOfPids);


	for (int i = 0; i < dwNmbrOfPids; i++) {
		// If process is not NULL
		if (adwProcesses[i] != NULL) {
			// Open a process handle
			if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, adwProcesses[i])) != NULL) {
				// If handle is valid
				// Get a handle of a module in the process 'hProcess'.
				// The module handle is needed for 'GetModuleBaseName'
				if (!EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &dwReturnLen2)) {
					printf("[!] EnumProcessModules Failed [ At Pid: %d ] With Error : %d \n", adwProcesses
						[i], GetLastError());
				}
				else {
					// If EnumProcessModules succeeded
					// Get the name of 'hProcess' and save it in the 'szProc' variable
					if (!GetModuleBaseName(hProcess, hModule, szProc, sizeof(szProc) / sizeof(WCHAR))) {
						printf("[!] GetModuleBaseName Failed [ At Pid: %d ] With Error : %d \n", adwProcesses
							[i], GetLastError());
					}
					else {
						// Perform the comparison logic
						if (wcscmp(szProcName, szProc) == 0) {
							wprintf(L"[+] FOUND \"%s\" - Of Pid : %d \n", szProc, adwProcesses[i]);
							// Return by reference
							*pdwPid = adwProcesses[i];
							*phProcess = hProcess;
							break;
						}
					}
				}
				CloseHandle(hProcess);
			}
		}
	}
	// Check if pdwPid or phProcess are NULL
	if (*pdwPid == NULL || *phProcess == NULL)
		return FALSE;
	else
		return TRUE;
}

int main(int argc, char** argv) {

	LPCWSTR procName = L"Notepad.exe";
	DWORD pdwPid = NULL;
	HANDLE phProcess = NULL;


	GetRemoteProcessHandle(procName, &pdwPid, &phProcess);
		

	return 0;
}