all: ./loader

./payload.bin: ./payload.asm
	nasm -f bin -o ./payload.bin ./payload.asm

./payload.h: ./payload.bin
	xxd -i ./payload.bin > ./payload.h

./loader: ./loader.c ./payload.h
	gcc -o ./loader ./loader.c

clean:
	rm -f ./payload.bin ./payload.h ./loader