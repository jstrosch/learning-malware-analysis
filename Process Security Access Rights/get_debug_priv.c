// Author: Josh Stroschein @jstrosch
// Date: 20 March 2024
// Description: A small program using the necessary APIs to get DEBUG privileges for the current process.

#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "advapi32.lib")

//  Forward declarations:
int GetDebugPriv( void );

int main( void )
{
    if(GetDebugPriv()){
        puts("[*] Debug privilege added to current process!");
    }
    return 0;
}

int GetDebugPriv(void) {
    HANDLE hToken = NULL;
    LUID lUID;
    TOKEN_PRIVILEGES tp;
    int success = 0;
    int status = 0;

    if (OpenProcessToken (GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        ZeroMemory (&tp, sizeof (tp));

        if(!LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &lUID )){
            puts("[!] Failed to look-up privilege value");
        } else {
            tp.Privileges[0].Luid = lUID;
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if ( !AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
            { 
                printf("[!] AdjustTokenPrivileges error: %u\n", GetLastError() ); 
            } else {
                status = GetLastError();
                if (status == ERROR_NOT_ALL_ASSIGNED) {//0x514
                    puts("[!] Debug privilege not added :(");
                } else {
                    puts("[*] Privilege adjusted");
                    success = 1;
                }
            }
        }        
    } else {
         puts("[!] Could not obtain process token");  
    }
    return success;
}