#include<Windows.h>
#include<important.h>
#include <psapi.h>  // enum are here in this 


BOOL PrintProcesses() {

	DWORD adwProcesses[1024 * 2] = { 0 },
		dwReturnLen1 = NULL,
		dwReturnLen2 = NULL,
		dwNumberOfPids = NULL;



	HANDLE hProcess = NULL;
	HMODULE hModule = NULL;

	WCHAR szProc[MAX_PATH] = { 0 };


	if (!EnumProcesses(adwProcesses, sizeof(adwProcesses), &dwReturnLen1)) {
		warn("Enumeration failed! %d ", GetLastError());
		return 0;

	}

	dwNumberOfPids = dwReturnLen1 / sizeof(DWORD);

	info("Number of process Detected are %d \n", dwNumberOfPids);

	info("\t\tNAME\t\t| PID");
	for (int i = 0; i < dwNumberOfPids; i++) {

		if (adwProcesses[i] != NULL) {
			if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, adwProcesses[i])) != NULL) {
				if (!EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwReturnLen2)) {
					warn("MODULE ENUMERATION FAILED ! %d, ", GetLastError());
				}
				else {
					// If EnumProcessModules succeeded
					// Get the name of 'hProcess' and save it in the 'szProc' variable
					if (!GetModuleBaseName(hProcess, hModule, szProc, sizeof(szProc) / sizeof(WCHAR))) {
						warn("GetModuleBaseName Failed [ At Pid: %d ] With Error : %d \n", adwProcesses[i], GetLastError());
					}
					else {
						// Printing the process name & its PID
						wprintf(L"Process \"%s\"  %d \n",szProc, adwProcesses[i]);
					}
				}


				CloseHandle(hProcess);
			}


		}
	}
	return 1;
}

	int main(int argc, char** argv) {
	
		info("calling the all process name and pids of them");
		PrintProcesses();
		return 0;	
	
	}