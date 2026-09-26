# Process Injection via ptrace

Demonstrates injecting shellcode into a running process using the `ptrace`
syscall — the same mechanism used by debuggers like gdb. The victim process
has no knowledge of the injection.

## How it works

1. `victim.c` — a process that loops, printing a message every 2 seconds.
   It prints its own PID so the injector can target it.
2. `injector.c` — attaches to the victim by PID and:
   - Calls `ptrace(PTRACE_ATTACH)` — sends `SIGSTOP` to freeze the victim
   - Reads the victim's registers with `PTRACE_GETREGS` to get current `RIP`
   - Opens `/proc/<pid>/mem` and seeks to the `RIP` address
   - Writes decrypted shellcode bytes directly into the victim's memory at
     that address
   - Calls `ptrace(PTRACE_DETACH)` — victim resumes, but now executes our
     shellcode instead of its own code

## Key concepts

**ptrace** — a Linux syscall that allows one process to observe and control
another. Used by debuggers, strace, and system call tracers. Requires the
attaching process to be root or a parent of the target (controlled by
`/proc/sys/kernel/yama/ptrace_scope`).

**RIP hijack** — we do not change the RIP register. Instead we overwrite the
bytes *at* the address RIP points to. When the victim resumes, RIP still
holds the same value but the instructions there are now our shellcode.

**/proc/pid/mem** — a file representing the live virtual memory of a running
process. Writing to it at a given offset modifies that address in the
process's address space directly. `ptrace` attachment is required first to
gain write access.

## Limitation

Writing at RIP overwrites whatever instructions were there. If the shellcode
is larger than the available space before the next important code boundary,
it corrupts the victim's code. Production injectors solve this by:
- Finding a code cave (padding region of null bytes large enough for the
  payload) and redirecting RIP there
- Injecting a `mmap` syscall into the victim to allocate fresh memory, then
  writing the payload there

Our shellcode also ends with `exit(0)`, terminating the victim after the
payload runs. A real injector would save the original bytes, restore them
post-execution, and return control to the victim via a trampoline.

## Detection (how a defender catches this)

- `ptrace(PTRACE_ATTACH)` on an unrelated process is a strong signal. EDRs
  alert on this immediately unless the attaching process is a known debugger.
- `/proc/<pid>/mem` writes are visible to audit frameworks (`auditd`).
- The victim's memory map will show a modified `.text` section if compared
  against the original binary on disk.

## Build and run

Terminal 1 — start the victim:
```bash
make
./victim.elf
```

Terminal 2 — inject (note the PID from terminal 1):
```bash
sudo ./injector.elf <pid>
```

Watch terminal 1 for the shellcode output.

## Requirements

- Linux x86-64
- `nasm`, `gcc`, `xxd`, `python3`
- Root privileges (`sudo`) for `ptrace` across unrelated processes