/*
* Author: Josh Stroschein @jstrosch
* Date: 20 March 2024
* Compile this in the architecture you want to inject into i.e. 32-bit -> 32 bit processes, 64-bit 64-bit processes.
*/

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string.h>

//  Forward declarations:
void InjectProcess( void );
void toLower(TCHAR *ptrStr);

char* target_process = "notepad.exe";

int main( void )
{
    InjectProcess(); 
    
    return 0;
}

void InjectProcess( )
{
    //HANDLE: https://msdn.microsoft.com/en-us/library/windows/desktop/ms724457%28v=vs.85%29.aspx
    //Handles represent a system resource
    HANDLE hProcessSnap = NULL;
    HANDLE hProcess = NULL;
    PROCESSENTRY32 pe32; //https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32
    DWORD dwPriorityClass;
    int injected = 0;

    HANDLE hTargetProcess = NULL;
    VOID* vpNewMemory = NULL;

    char libPath[] = "C:\\demo_dll.dll"; 

    //https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa?redirectedfrom=MSDN
    if(GetFileAttributesA(libPath) == INVALID_FILE_ATTRIBUTES)
    {
        printf("[!] Failed to find library to inject.\n");
        return;
    }
    HMODULE hKern32 = GetModuleHandle("Kernel32");
    void *loadLib = GetProcAddress(hKern32, "LoadLibraryA");
    int lasterror = 0;

    // Take a snapshot of all processes in the system.
    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms682489%28v=vs.85%29.aspx
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE ) // -1
    {
        puts("ERROR");
        return;
    }

    //The size of the structure, in bytes. 
    //Before calling the Process32First function, set this member to sizeof(PROCESSENTRY32). 
    //If you do not initialize dwSize, Process32First fails.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        puts("ERROR");
        CloseHandle( hProcessSnap );   
        return;
    }

    do
    {
        if (injected) {
            puts("[*] DLL injected, done.");
            break;
        }
        toLower(pe32.szExeFile);
        printf("[*] Checking process name %s\n", pe32.szExeFile);
        // This could be any process name you are looking for
        if(strstr(pe32.szExeFile, target_process))
        {
            printf("[*] Found %s, attempting to get handle... %s\n", target_process, pe32.szExeFile);

            hTargetProcess = OpenProcess( PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, FALSE, pe32.th32ProcessID );

            if (!hTargetProcess) {
                puts("[!] Failed to get process handle...");
            } else {
                puts("[*] Handle obtained");

                vpNewMemory = VirtualAllocEx(hTargetProcess, NULL, 0x220, MEM_COMMIT, 0x40);

                if(!vpNewMemory){
                    puts("[!] Failed to allocate memory...");
                } else {
                    printf("[*] Memory allocated in target process at 0x%p\n", vpNewMemory);

                    if(WriteProcessMemory(hTargetProcess, vpNewMemory, libPath, sizeof(libPath), NULL)) {
                        puts("[*] DLL path copied to new memory... ");
                    }

                    if(CreateRemoteThread(hTargetProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLib, vpNewMemory, 0, NULL)){
                        puts("[*] Remote thread created");
                        injected = 1;
                    } else {
                        // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
                        lasterror = GetLastError();
                        printf("[!] Error: %08x\n", lasterror);
                    }
            
                    //clean-up?
                    if(VirtualFreeEx(hTargetProcess,vpNewMemory,0,MEM_RELEASE)){
                        puts("[*] Memory freed from target process");
                    } else {
                        puts("[!] Memory NOT freed from target process");
                    }
                }    
            } 
        }
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );

    return;
}

void toLower(TCHAR *pStr)
{
  while(*pStr++ = _totlower(*pStr));
}