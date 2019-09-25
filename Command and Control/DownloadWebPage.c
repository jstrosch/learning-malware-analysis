#pragma comment (lib, "wininet.lib")

#include <windows.h>
#include <wininet.h>

int main(void)
{
    LPDWORD connectionDescription;

    if(InternetGetConnectedState(connectionDescription,0) == 1)
    {
        DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;

        HINTERNET hINet = InternetOpenA("User-Agent: Internet Explorer 7.6/pma", 
                dwAccessType, NULL,NULL,0);

        if(hINet)
        {
            
            HINTERNET hUrl = InternetOpenUrlA(hINet, "http://www.practicalmalwareanalysis.com/cc.html", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);  

            if(hUrl)
            {

                char holdBuff[4096];
                DWORD bytesRead;

                char* temp = holdBuff;
                while (InternetReadFile(hUrl, temp, 1024, &bytesRead) == TRUE && bytesRead > 0)
                {
                    temp += bytesRead;
                }
                
                *temp = '\0';
                
                char *content = malloc (strlen(holdBuff) + 1);
                strcpy (content, holdBuff);

                //puts(content);

                free(content);

                InternetCloseHandle(hUrl);
            }

            InternetCloseHandle(hINet);

        }else {
            puts("No joy!");
        }


    } else {
        puts("You gotz no internetz!!!");
    }

    return 0;
}



/*

DWORD dwAccessType;
AnsiString szProxySz;
if (getProxyOptions())
{
dwAccessType = INTERNET_OPEN_TYPE_PROXY;
szProxySz = AnsiString("\"\"http=http://" + getProxyAddress() + ":" +
getProxyPortNum() + "\"\"");
}
else
{
dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
szProxySz = NULL;
}

hINetHnd = InternetOpen(TEXT("Live Update"), dwAccessType,
szProxySz.c_str(), NULL, 0);
if (!hINetHnd)
throw EAbort(SysErrorMessage(GetLastError()), 0); 

*/