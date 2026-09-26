# Shellcode Loader

A minimal shellcode loader in C demonstrating how raw machine code can be
mapped into memory and executed at runtime on Linux x86-64.

## How it works

1. `payload.asm` — hand-written x86-64 assembly that calls the `write` syscall
   to print a message, then calls `exit`. Assembled with `nasm -f bin` to
   produce raw bytes with no ELF headers.
2. `xxd -i` converts the raw bytes into a C byte array (`payload.h`).
3. `loader.c` maps a region of memory with `mmap`, copies the shellcode in,
   flips permissions from `RW` to `RX` with `mprotect`, then jumps to it via
   a function pointer.

## Key concepts

**W^X (Write XOR Execute)** — modern operating systems enforce that a memory
page cannot be writable and executable at the same time. This loader respects
that policy: it writes the shellcode while the page is `RW`, then switches to
`RX` before executing. Skipping `mprotect` may still work on default Ubuntu
but will fail on hardened systems enforcing strict W^X.

**Position-independent shellcode** — the payload uses RIP-relative addressing
(`lea rsi, [rel msg]`) so it works regardless of where the loader maps it in
virtual memory. No hardcoded addresses.

**Raw syscalls** — the shellcode bypasses libc entirely and invokes the kernel
directly via the `syscall` instruction. `rax` holds the syscall number, `rdi`,
`rsi`, `rdx` hold the arguments.

## Detection (how a defender catches this)

- `mmap` followed immediately by `mprotect` on the same region is a strong
  behavioral signal. EDR tools like Falco flag this pattern.
- The shellcode bytes in the binary are a static signature. AV scanners
  recognize known shellcode byte sequences.
- No legitimate program maps anonymous memory and then executes it.

## Build and run

```bash
make
./loader
```

## Requirements

- Linux x86-64
- `nasm`, `gcc`, `xxd`