// Author: Josh Stroschein @jstrosch
// Date: 23 December 2021
// Description: Common Windows API for enumerating processes by name. 

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string.h>

//  Forward declarations:
void GetProcessList( void );
void toLower(TCHAR *ptrStr);

int main( void )
{
  GetProcessList( );
  return 0;
}

void GetProcessList( )
{
  //HANDLE: https://msdn.microsoft.com/en-us/library/windows/desktop/ms724457%28v=vs.85%29.aspx
  //Handles represent a system resource
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32; //https://msdn.microsoft.com/en-us/library/windows/desktop/ms724457%28v=vs.85%29.aspx
  DWORD dwPriorityClass;

  // Take a snapshot of all processes in the system.
  //https://msdn.microsoft.com/en-us/library/windows/desktop/ms682489%28v=vs.85%29.aspx
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
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
    toLower(pe32.szExeFile);
    // This could be any process name you are looking for
    if(strstr(pe32.szExeFile,"vm")){
      printf("Virtualization Detected! - %s\n", pe32.szExeFile);
    }
  
  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );

  return;
}

void toLower(TCHAR *pStr)
{
  while(*pStr++ = _totlower(*pStr));
}