# XOR Encrypted Shellcode Loader

Extends the plain loader with XOR encryption to defeat static signature
detection. The payload bytes on disk are unrecognizable noise — decryption
happens in memory at runtime.

## How it works

1. `encrypt.py` reads `payload.bin` and XORs every byte with a single-byte
   key (`0xAA`), writing the result as a C header (`encrypted_payload.h`).
2. `loader.c` maps `RW` memory, decrypts the payload byte-by-byte with the
   same key (XOR is symmetric: `byte ^ key ^ key == byte`), flips to `RX`,
   executes.

## Key concepts

**XOR encryption** — symmetric single-byte XOR is the simplest form of
payload obfuscation. The encrypted bytes bear no resemblance to the original
shellcode, defeating signature-based AV that scans bytes on disk.

**Weakness of single-byte XOR** — null bytes (`0x00`) in the original
produce the key itself in the ciphertext (`0x00 ^ 0xAA == 0xAA`). Repeating
`0xAA` bytes in the output reveal the key to any analyst. Real implementations
use multi-byte rolling keys or stream ciphers (RC4, ChaCha20) to avoid this.

**Runtime-only plaintext** — the decrypted shellcode exists in memory only
for the duration of execution. A memory forensics tool (like `volatility`)
could still capture it, but static AV sees nothing.

## Detection (how a defender catches this)

- Same `mmap` → `mprotect` behavioral pattern as v1.
- The decryption loop (XOR over a buffer before marking it executable) is
  itself a behavioral signature that sandbox analysis catches.
- Memory scanning at runtime still finds the decrypted payload.

## Build and run

```bash
make
./loader
```

## Requirements

- Linux x86-64
- `nasm`, `gcc`, `xxd`, `python3`