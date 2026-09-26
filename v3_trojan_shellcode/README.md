# Trojan

Demonstrates how malicious payload execution can be hidden inside a program
that appears to perform legitimate behavior. The trojan forks a child process
that runs a real victim program, waits for it to finish, then executes
shellcode in the parent — invisible to the user.

## How it works

1. `victim.c` — an innocent program that prints a message. Represents any
   legitimate binary.
2. `loader.c` — the trojan. On execution:
   - Forks a child process
   - Child calls `execvp` to replace itself with `victim.elf` (the legitimate
     program)
   - Parent waits for the child to finish with `waitpid`
   - Parent then decrypts and executes the shellcode payload
3. From the user's perspective, they ran a program, got expected output, and
   it exited cleanly. The shellcode ran silently in the parent.

## Key concepts

**fork + execvp** — `fork` creates a child process that is an exact copy of
the parent. `execvp` replaces the child's memory image with a new program.
This is the standard Unix process model; the trojan exploits it to run two
things from one execution.

**Separation of visible and hidden behavior** — the legitimate output comes
from a real child process running a real binary. This makes behavioral
analysis harder — the child process is genuinely innocent.

## Limitation

After the shellcode runs, the process terminates because our payload ends with
`exit(0)`. A more sophisticated trojan would use a trampoline — save the
original bytes at the execution address, restore them after the payload runs,
and jump back so the host process continues normally.

## Detection (how a defender catches this)

- A process that forks, execs a child, waits, then maps anonymous executable
  memory is an unusual behavioral pattern.
- The parent and child have different memory layouts post-exec — the parent
  retains the trojan's full image including the encrypted payload.
- File hash of the trojan binary will not match the legitimate program it
  impersonates.

## Build and run

```bash
make
./loader.elf
```

## Requirements

- Linux x86-64
- `nasm`, `gcc`, `xxd`, `python3`