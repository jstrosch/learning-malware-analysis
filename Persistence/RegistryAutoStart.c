//Provides access to Windows API for registry
#pragma comment(lib, "advapi32.lib")

#include <windows.h>
#include <stdio.h>

char szRunKey[] = {"Software\\Microsoft\\Windows\\CurrentVersion\\Run"};
char szMyAppPath[] = {"C:\\Windows\\system32\\calc.exe"};
char szMyappSubKey[] = {"CSC432"};

int main(void)
{
    HKEY hKey;
    //What are the limits of writing to HKLM?
    //if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRunKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)

    //Let's write to HKCU
    if(RegOpenKeyEx(HKEY_CURRENT_USER, szRunKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        if(RegSetValueEx(hKey, szMyappSubKey, 0,
                REG_SZ, (const unsigned char*)szMyAppPath,
                sizeof(szMyAppPath)) == ERROR_SUCCESS)
            puts("Create Run key successful");
        else
            puts("RegSetValueEx failed");

        RegCloseKey(hKey);
    }
    else
        puts("RegOpenKeyEx key failed");

    return 0;
}