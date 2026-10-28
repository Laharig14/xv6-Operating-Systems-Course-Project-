# xv6 Operating Systems Course Project

This repository contains our Operating Systems course project using the x86-based `xv6-public` operating system. Each assignment part will be documented separately as the project progresses.

## Part 1: Periodic User-Level Alarms

### 1. Project Overview

The goal of Part 1 is to implement periodic user-level alarms in xv6.

The implementation introduces two system calls:

- `sigalarm(interval, handler)` — Runs a user-defined handler after every specified number of timer ticks.
- `sigreturn()` — Restores the process's execution state after the handler finishes.

An interval of `0` disables future alarms. Nested alarm handlers are prevented while an existing handler is executing.

### 2. Files Modified

The following existing xv6 files were modified:

| File | Purpose |
|---|---|
| `proc.h` | Added alarm-related fields to `struct proc`. |
| `proc.c` | Initialized alarm fields when allocating a process. |
| `user.h` | Declared `sigalarm()` and `sigreturn()`. |
| `syscall.h` | Assigned system call numbers. |
| `syscall.c` | Registered the new system calls. |
| `usys.S` | Added user-space system call stubs. |
| `sysproc.c` | Implemented `sys_sigalarm()` and `sys_sigreturn()`. |
| `trap.c` | Added timer-interrupt logic to trigger the alarm handler. |
| `Makefile` | Added the alarm test programs to `UPROGS`. |

### 3. Process Structure Changes — `proc.h`

The following fields were added to `struct proc`:

```c
int interval, ticks, active;
uint handler;
struct trapframe *store_copy;
```

Their purposes are:

- `interval`: Number of timer ticks between alarms.
- `ticks`: Number of ticks counted toward the next alarm.
- `active`: Prevents nested alarm handlers.
- `handler`: Address of the user-defined handler function.
- `store_copy`: Stores a copy of the interrupted process's trap frame.

### 4. Process Initialization — `proc.c`

The alarm fields are initialized in `allocproc()`:

```c
p->interval = 0;
p->ticks = 0;
p->handler = 0;
p->active = 0;
p->store_copy = 0;
```

This ensures that a newly allocated process starts with alarms disabled and no active handler.

### 5. System Call Declarations and Registration

#### `user.h`

The user-space declarations allow programs to invoke the new system calls:

```c
int sigalarm(int interval, void (*handler)(void));
int sigreturn(void);
```

#### `syscall.h`

System call numbers were assigned to the new calls:

```c
#define SYS_sigalarm 22
#define SYS_sigreturn 23
```

#### `syscall.c`

The system call handler functions were declared:

```c
extern int sys_sigalarm(void);
extern int sys_sigreturn(void);
```

Both handlers were registered in the system call table:

```c
[SYS_sigalarm] sys_sigalarm,
[SYS_sigreturn] sys_sigreturn,
```

#### `usys.S`

User-space system call stubs were added:

```asm
SYSCALL(sigalarm)
SYSCALL(sigreturn)
```

These entries allow user programs to invoke the corresponding kernel system call handlers.

### 6. Implementing `sigalarm()` — `sysproc.c`

The `sys_sigalarm()` function retrieves the interval and handler address supplied by the user program.

Its responsibilities include:

1. Reading the interval and handler arguments.
2. Rejecting negative intervals.
3. Storing the alarm configuration in the current process.
4. Resetting the tick counter when the alarm is configured.
5. Disabling future alarms when the interval is set to `0`.

The timer interrupt handler uses this configuration to determine when an alarm should fire.

### 7. Implementing `sigreturn()` — `sysproc.c`

The `sys_sigreturn()` function restores the interrupted process's execution state after the alarm handler finishes.

Its main steps are:

1. Check that a saved trap frame exists.
2. Save the original `eax` return value from the saved trap frame.
3. Copy the saved trap frame back into the current trap frame.
4. Free the allocated saved copy.
5. Clear the saved pointer and reset the active-handler flag.
6. Return the original `eax` value.

Preserving the original `eax` value allows the interrupted program to resume without losing its previous register state due to the system call's return value.

### 8. Timer Interrupt Handling — `trap.c`

The timer interrupt logic checks whether:

- A process is running in user mode.
- Its alarm interval is greater than zero.
- No alarm handler is currently active.

For eligible processes, the tick counter is incremented. When the counter reaches the configured interval:

1. Memory is allocated for a saved trap frame.
2. The current trap frame is copied.
3. The process is marked as having an active handler.
4. The tick counter is reset.
5. The instruction pointer (`tf->eip`) is updated to redirect execution to the user-defined handler.

When the handler calls `sigreturn()`, the saved execution state is restored so that the interrupted process can resume.

### 9. Test Programs

#### `alarmtest.c` — Basic Periodic Alarm Test

This test configures an alarm with an interval of 10 timer ticks. The handler increments a counter, prints a message, and calls `sigreturn()`.

**Reported output:**

```text
Alarm fired! count=1
Alarm fired! count=2
Alarm fired! count=3
Alarm test finished!
```

**Reported result:** Basic periodic alarm behavior passed.

#### `alarmdisable.c` — Disabling Alarms

This test waits for an alarm, disables it using `sigalarm(0, handler)`, and checks whether another alarm fires during a subsequent loop.

**Reported output:**

```text
PASS: Alarm disabled
```

**Reported result:** The alarm-disable test was reported successful.

#### `alarmcheck.c` — Nested Alarm and Resumption Diagnostic

This test uses a short alarm interval and keeps the handler busy to check whether nested handler execution occurs. It also checks whether the main loop continues to execute.

**Reported output:**

```text
Handler count: 3
Nested count: 0
Main loop progress: 8275212
Basic alarm checks passed
```

**Reported result:** The diagnostic test reported no nested handler entry, and the main loop continued executing.

These tests provide evidence of basic alarm behavior, alarm disabling, nested-handler prevention, and continued execution. They do not independently establish that every register is restored correctly under every possible condition.

### 10. Building and Running xv6

Run the following commands from the xv6 project directory.

**Build the operating system:**

```bash
make clean
make
```

**Start xv6:**

```bash
make qemu-nox
```

**Run the tests** at the xv6 shell prompt:

```text
alarmtest
alarmdisable
alarmcheck
```

Make sure the corresponding test programs are included in the `UPROGS` list in the `Makefile`:

```makefile
_alarmtest\
_alarmdisable\
_alarmcheck\
```

The entries should follow the formatting used by the existing Makefile.

### 11. Implementation Summary

Part 1 introduces periodic user-level alarms through timer interrupts, process-level alarm state, saved trap frames, and two new system calls.

The key mechanisms are:

- **Timer tracking:** Counts ticks until the configured interval is reached.
- **Handler invocation:** Redirects execution to the user-defined alarm handler.
- **Nested-handler prevention:** Uses an active-handler flag to prevent another alarm from interrupting an existing handler.
- **Execution restoration:** Uses a saved trap frame to resume the interrupted process through `sigreturn()`.
- **Alarm disabling:** Allows a process to disable future alarms by setting its interval to zero.

The reported tests cover basic periodic alarms, alarm disabling, nested-handler prevention, and continued execution. A final clean build and verification against all assignment requirements should be completed before submission.

## Subsequent Project Parts

Additional sections will be added here as the remaining parts of the Operating Systems course project are implemented.

## Project Environment

- **Operating system:** xv6-public
- **Architecture:** x86
- **Languages:** C and x86 assembly
- **Build system:** Make
- **Emulator:** QEMU
- **Course:** Operating Systems

