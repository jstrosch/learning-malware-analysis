#include <stdio.h>
#include <windows.h>

/* This is a simple XOR decrypt, used primarily to 'hide' strings in the binary */
void decrypt_encrypt(char*src, size_t key, size_t size);

char mutex_name[25] = "\xc3\xff\xf2\xd4\xee\xf5\xf2\xe5\xce\xf2\xe3\xfe";

const unsigned int xor_key = 0x97;

int main(void) {
	HANDLE h_mutex = NULL;

	wchar_t *string1 = L"this is a wide char string";
	char *string2 = "this is an ascii string";
    char *string3 = "\xe3\xff\xfe\xe4\xb7\xfe\xe4\xb7\xf6\xf9\xb7\xf2\xf9\xf4\xe5\xee\xe7\xe3\xf2\xf3\xb7\xe4\xe3\xe5\xfe\xf9\xf0";

	decrypt_encrypt(mutex_name, xor_key, strlen(mutex_name));
	h_mutex = OpenMutex(SYNCHRONIZE,FALSE,mutex_name);

	if(!h_mutex) {
		
		h_mutex = CreateMutex(NULL, TRUE, mutex_name);
		decrypt_encrypt(mutex_name, xor_key, strlen(mutex_name));

		printf("%ls\n", string1);
		printf("%s\n", string2);
		printf("%s\n", string3);
		decrypt_encrypt(string3, xor_key, strlen(string3));
		printf("%s\n", string3);

		Sleep(300000);
	}

	CloseHandle(h_mutex);
}

void decrypt_encrypt(char *src,size_t key, size_t size) {
	int i = 0;

	for (i = 0; i < size; i++) {
		src[i] ^= key;
	}
}
