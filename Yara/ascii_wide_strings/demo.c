#include <stdio.h>
#include <windows.h>

/* This is a simple XOR decrypt, used primarily to 'hide' strings in the binary */
void decrypt(char*src, size_t key, size_t size);

const unsigned int xor_key = 0x97;

int main(void) {

	wchar_t *string1 = L"this is a wide char string";
	char *string2 = "this is an ascii string";
    char *string3 = "\xe3\xff\xfe\xe4\xb7\xfe\xe4\xb7\xf6\xf9\xb7\xf2\xf9\xf4\xe5\xee\xe7\xe3\xf2\xf3\xb7\xe4\xe3\xe5\xfe\xf9\xf0";

	printf("%ls\n", string1);
	printf("%s\n", string2);
    printf("%s\n", string3);
    decrypt(string3, xor_key, strlen(string3));
    printf("%s\n", string3);

}

void decrypt(char *src,size_t key, size_t size) {
	int i = 0;

	for (i = 0; i < size; i++) {
		src[i] ^= key;
	}
}
