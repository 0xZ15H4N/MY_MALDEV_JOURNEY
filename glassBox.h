#include<windows.h>

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

    inline HMODULE GetMod(
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







   