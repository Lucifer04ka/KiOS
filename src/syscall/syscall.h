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
#define SYS_NR          10

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

void syscall_init(void);
uint64_t syscall_handler(uint64_t syscall_nr, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif // SYSCALL_H
