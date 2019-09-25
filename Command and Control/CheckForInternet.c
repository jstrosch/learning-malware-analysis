#pragma comment (lib, "wininet.lib")

#include <windows.h>
#include <wininet.h>

int main(void)
{

	int sleepBetweenQueriesInMS = 10000;
    LPDWORD connectionDescription;
	int isActive = 0;
    isActive = InternetGetConnectedState(connectionDescription,0);

    while (isActive == 0){

    	printf ("No active connection, sleeping now for %d seconds\n", (sleepBetweenQueriesInMS / 1000));
    	
    	Sleep(sleepBetweenQueriesInMS);
    	isActive = InternetGetConnectedState(connectionDescription,0);
    }

    printf ("You gotz internet!!!\n");

    return 0;
}