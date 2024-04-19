// Author: Josh Stroschein @jstrosch
// Date: 19 April 2024
// Description: A small program to help explore tools such as Process Explorer, SystemInformer or Proces Hacker 2

#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <winsock.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")

//prototypes
void copy_self_persist(const char*current_location);
BOOL file_exists(LPCTSTR szPath);

// Global data
char mutex_name[] = "TheCyberYeti";

void main(void) {
    HANDLE h_mutex = NULL;
    char self_path[100] = {0}; //note the compiler will add a function to handle the zeroing of this memory.
    
    HANDLE h_demo_dll = NULL;
    void * rwx_memory = NULL;

    // Check for MUTEX to handle synchronization
    h_mutex = OpenMutex(SYNCHRONIZE,FALSE,mutex_name);
    
    if(!h_mutex) {

        h_mutex = CreateMutex(NULL, TRUE, mutex_name);
        
        //check current location for persistence
        GetModuleFileName(NULL,self_path, 100);

        // If not in AppData, perform persistence
        if (strstr(self_path, "AppData") == 0)
        {
            copy_self_persist(self_path);
        }

        //powershell command: Start-Sleep -seconds 60
        ShellExecute(NULL,NULL,"powershell.exe", "-enc UwB0AGEAcgB0AC0AUwBsAGUAZQBwACAALQBzAGUAYwBvAG4AZABzACAANgAwAA==", NULL,SW_HIDE);

        //Load a library
        h_demo_dll = LoadLibrary("demo_dll.dll");
        
        //Allocate some memory
        rwx_memory = VirtualAlloc(0, 0x100, 0x1000, 0x40);

        for (int i = 0; i<200;i++) {
            
            memmove(((char *)rwx_memory + i),&i,1);
        }

        Sleep(300000);//300 seconds or 5 minutes
    }

    //clean-up
    CloseHandle(h_mutex);
}

void copy_self_persist(const char*current_location) {
    char drop_name[] = "\\FlyingToasters.scr";
    char run_key[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    char sub_key[] = "TheCyberYeti";
    
    HANDLE h_self_path = NULL;
    HANDLE h_target_path = NULL;
    char env_path[100] = {0};
    char*end_env_path = NULL;
    int self_file_size = 0;
    void*read_buffer = 0;

    HKEY hKey;

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	CONTEXT c = {0};

    GetEnvironmentVariable("TMP",env_path, 100);
    
    end_env_path = strrchr(env_path,0x5c); //get end of path string
        
    strncat(end_env_path, drop_name,strlen(drop_name));

    if(!file_exists(env_path)){

        //read self from current location
        h_self_path = CreateFile(current_location,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

        if(h_self_path) {

            h_target_path = CreateFile(env_path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
            printf("%s\n",env_path);
            if (h_target_path) {
                self_file_size = GetFileSize(h_self_path,NULL);

                read_buffer = VirtualAlloc(NULL, self_file_size, MEM_COMMIT, PAGE_READWRITE);

                if (read_buffer) {
                    ReadFile(h_self_path, read_buffer, self_file_size,NULL,NULL);
                    WriteFile(h_target_path,read_buffer,self_file_size,NULL, NULL);
                }
                VirtualFree(read_buffer,0,MEM_RELEASE);
                CloseHandle(h_target_path);
            }
        }

        CloseHandle(h_self_path);

        //Add persistence from registry
        if(RegOpenKeyEx(HKEY_CURRENT_USER, run_key, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
        {
            RegSetValueEx(hKey, sub_key, 0, REG_SZ, (const unsigned char*)env_path, sizeof(env_path));
            RegCloseKey(hKey);
        }
    }
    //now launch
    CreateProcess(env_path,NULL,NULL,NULL,FALSE,0x00000008, NULL, NULL, &si, &pi);
    exit(0);
}

//https://stackoverflow.com/questions/3828835/how-can-we-check-if-a-file-exists-or-not-using-win32-program
BOOL file_exists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}