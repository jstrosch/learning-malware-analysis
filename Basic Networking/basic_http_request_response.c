#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <winsock.h>

#pragma comment(lib,"wininet.lib")

#define SERVER_PORT 80
#define REQUEST_METHOD "GET"
#define REQUEST_PATH "/"
#define USER_AGENT "Mozilla/4.0 (TheCyberYeti)"

/* This is a simple XOR decrypt, used primarily to 'hide' strings in the binary */
void decrypt(char*src, size_t key, size_t size);

char domains[3][25] = {{"\xe3\xff\xf2\xf4\xee\xf5\xf2\xe5\xee\xf2\xe3\xfe\xb9\xf4\xf8\xfa\x97"},{"\xe3\xff\xf2\xf4\xee\xf5\xf2\xe5\xee\xf2\xe3\xfe\xb9\xf4\xf8\xfa\x97"},{"\xe3\xff\xf2\xf4\xee\xf5\xf2\xe5\xee\xf2\xe3\xfe\xb9\xf4\xf8\xfa\x97"}};

const unsigned int xor_key = 0x97;

void main (void) {
    DWORD LastError, dwBytesRead = 0;
    char buffer[8192];
    BOOL result;
	size_t i = 0;
	HINTERNET hInternet;
    HINTERNET hConnect;
    HINTERNET hRequest;

	hInternet = InternetOpenA(USER_AGENT,INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);

	for(i = 0; i < 3; i++) {
		decrypt(domains[i], xor_key, strlen(domains[i]));
       
		if(hInternet != NULL){
            
			hConnect = InternetConnectA(hInternet, domains[i], 0x50, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
            
            // Build the HTTP request string
            char request[1024];
            sprintf_s(request, sizeof(request), "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n",
                    REQUEST_METHOD, REQUEST_PATH, domains[i], USER_AGENT);

            // Open the HTTP request
            hRequest = HttpOpenRequestA(hConnect, REQUEST_METHOD, REQUEST_PATH, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
            
            if (hRequest == NULL) {
                printf("Error opening HTTP request: %d\n", GetLastError());
            } else {
                result = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);

                if (!result) {
                    printf("Error sending HTTP request: %d\n", GetLastError());
                } else {
                    InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &dwBytesRead);
                    while (dwBytesRead > 0) {
                        
                        if (dwBytesRead > 0) {
                            buffer[dwBytesRead] = '\x00';
                            printf("%s", buffer);
                        }
                        InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &dwBytesRead);
                    }
                }
            }

            // Close handles
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);  
		}
		Sleep(2000);
	}
	InternetCloseHandle(hInternet);

}

void decrypt(char *src,size_t key, size_t size) {
	int i = 0;

	for (i = 0; i < size; i++) {
		src[i] ^= key;
	}
}