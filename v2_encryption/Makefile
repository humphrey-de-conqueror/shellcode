CC      = gcc
NASM    = nasm
PYTHON  = python3
TARGET  = ./loader

all: $(TARGET)

# step 1: assemble raw shellcode bytes
./payload.bin: ./payload.asm
	$(NASM) -f bin -o ./payload.bin ./payload.asm

# step 2: encrypt the payload and generate the C header
./encrypted_payload.h: ./payload.bin ./encrypt.py
	$(PYTHON) ./encrypt.py

# step 3: compile the loader
$(TARGET): ./loader.c ./encrypted_payload.h
	$(CC) -o $(TARGET) ./loader.c

clean:
	rm -f ./payload.bin ./encrypted_payload.h $(TARGET)