#include <stdio.h> 
#include <string.h> 
#include <sys/mman.h> 

#include "payload.h"

int main(void)
{
	/* step 1: allocaate memory region */
	void *mem = mmap(
		NULL,	//let kernel choose the address
		__payload_bin_len,	//how many byte we need 
		PROT_READ | PROT_WRITE,	//writable
		MAP_PRIVATE | MAP_ANONYMOUS, 
		-1, 	// fd -1 as no file is linked
		0	// offset 0  
	); 

	if (mem == MAP_FAILED) {
		perror("mmap");
		return 1; 
	}

	/* step 2: copy the shellcode byte into the region */
	memcpy(mem, __payload_bin, __payload_bin_len);

	/* step 3: change permission, remove w, add x */
	if (mprotect(mem, __payload_bin_len, PROT_READ | PROT_EXEC) == -1) {
		perror("mprotect");
		return 1; 
	}

	/* step 4: cast the address to a function pointer and call it */
	void (*shellcode)(void) = (void (*)(void))mem;
	shellcode(); 

	/* we never reach here, the shelldoce call exit(0) */
	return 0; 
}