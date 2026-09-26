CC     = gcc
NASM   = nasm
PYTHON = python3

all: ./victim.elf ./injector.elf

./payload.bin: ./payload.asm
	$(NASM) -f bin -o ./payload.bin ./payload.asm

./encrypted_payload.h: ./payload.bin ./encrypt.py
	$(PYTHON) ./encrypt.py

./victim.elf: ./victim.c
	$(CC) -o ./victim.elf ./victim.c

./injector.elf: ./injector.c ./encrypted_payload.h
	$(CC) -o ./injector.elf ./injector.c

clean:
	rm -f ./payload.bin ./encrypted_payload.h ./victim.elf ./injector.elf