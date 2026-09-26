#!/usr/bin/env python3

INPUT  = "./payload.bin"
OUTPUT = "./encrypted_payload.h"
KEY    = 0xAA  # single byte XOR key, change this to anything 0x01-0xFF

with open(INPUT, "rb") as f:
    raw = f.read()

encrypted = bytes(b ^ KEY for b in raw)

# write as a C header, same format as xxd -i
with open(OUTPUT, "w") as f:
    f.write("/* XOR key: 0x{:02X} */\n".format(KEY))
    f.write("unsigned char encrypted_payload[] = {\n    ")

    hex_values = ["0x{:02x}".format(b) for b in encrypted]

    # 12 bytes per line, same style as xxd -i
    for i, val in enumerate(hex_values):
        f.write(val)
        if i != len(hex_values) - 1:
            f.write(", ")
            if (i + 1) % 12 == 0:
                f.write("\n    ")

    f.write("\n};\n")
    f.write("unsigned int encrypted_payload_len = {};\n".format(len(encrypted)))