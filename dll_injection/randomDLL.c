#include<windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            MessageBoxW(NULL,L"DLL injection successfully!",L"DLL injeciton",MB_OK | MB_ICONEXCLAMATION);
            break;

        // case DLL_THREAD_ATTACH:
        //  // Do thread-specific initialization.
        //     break;

        // case DLL_THREAD_DETACH:
        //  // Do thread-specific cleanup.
        //     break;

        // case DLL_PROCESS_DETACH:
        //  // Perform any necessary cleanup.
        //     break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}