// Author: Josh Stroschein @jstrosch
// Date: 23 December 2021
// Description: A small utility that demonstrates how to perform a DNS query.  

#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")

int main(int argc, char *argv[]) {

  WSADATA wsdata;

  WSAStartup(0x0101, &wsdata); // WSAStartup function initiates use of the Winsock DLL by a process

  struct hostent *he; //The hostent structure is used by functions to store information about a given host, such as host name, IPv4 address, and so forth.

  if (argc != 2)
  {

     printf("Usage: %s hostname\n",argv[0]);

     return -1;

  }

  if ((he = gethostbyname(argv[1])) == NULL)
  {
     printf("gethostbyname() error\n");

     return -1;
  }

  printf("Hostname : %s\n",he->h_name);

  printf("IP Address: %s\n",inet_ntoa(*((struct in_addr *)he->h_addr)));

  WSACleanup();

  return 0;
}