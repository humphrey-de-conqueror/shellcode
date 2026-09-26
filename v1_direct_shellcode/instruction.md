### create payload
write the payload in assembly, remember to use relative address

### compile the assembly to raw binary 
nasm ./payload.asm -f bin -o ./payload.bin 

### turns the raw binary to C array for latter use 
xxd -i ./payload.bin > ./payload.h 
