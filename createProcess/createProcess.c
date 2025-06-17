#include<windows.h>


/*

BOOL CreateProcessW(
  [in, optional]      LPCWSTR               lpApplicationName,   // name of proces to start
  [in, out, optional] LPWSTR                lpCommandLine,     // arguements to starts with the process
  [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,  // return hanlde to child process
  [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes, 
  [in]                BOOL                  bInheritHandles,
  [in]                DWORD                 dwCreationFlags,
  [in, optional]      LPVOID                lpEnvironment,
  [in, optional]      LPCWSTR               lpCurrentDirectory,
  [in]                LPSTARTUPINFOW        lpStartupInfo,
  [out]               LPPROCESS_INFORMATION lpProcessInformation
); 

*/

STARTUPINFO si={0};
PROCESS_INFORMATION pi={0};


int main(void){
    CreateProcessW(
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

    );
    return EXIT_SUCCESS;
}