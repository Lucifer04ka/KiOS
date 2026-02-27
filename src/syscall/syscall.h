#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// ============================================================================
// System Call Numbers
// ============================================================================

#define SYS_READ        0
#define SYS_WRITE       1
#define SYS_OPEN        2
#define SYS_CLOSE       3
#define SYS_MMAP        4
#define SYS_MUNMAP      5
#define SYS_EXIT        6
#define SYS_GETPID      7
#define SYS_GETTIMEOFDAY 8
#define SYS_BRK         9
#define SYS_NR          10  // Total number of syscalls

// ============================================================================
// File Descriptors
// ============================================================================

#define FD_STDIN        0
#define FD_STDOUT       1
#define FD_STDERR       2

// ============================================================================
// mmap flags
// ============================================================================

#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20

#define PROT_READ       0x01
#define PROT_WRITE      0x02
#define PROT_EXEC       0x04

// ============================================================================
// System Call API
// ============================================================================

// Initialize syscall interface
void syscall_init(void);

// Main syscall handler
uint64_t syscall_handler(uint64_t syscall_nr, uint64_t arg1, uint64_t arg2, uint64_t arg3);

// Syscall implementations
int64_t sys_read(int fd, void* buf, uint64_t count);
int64_t sys_write(int fd, const void* buf, uint64_t count);
void* sys_mmap(void* addr, uint64_t length, int prot, int flags, int fd, uint64_t offset);
int sys_munmap(void* addr, uint64_t length);
void sys_exit(int code);
uint64_t sys_getpid(void);
int sys_brk(uint64_t brk);

// Helper to register syscall
void syscall_register(uint64_t num, void* handler);

#endif // SYSCALL_H
