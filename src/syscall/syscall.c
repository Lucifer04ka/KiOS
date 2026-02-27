#include "syscall.h"
#include "../arch/x86_64/paging/vmm/vmm.h"
#include "../mm/pmm.h"
#include "../mm/heap.h"

// ============================================================================
// Syscall Handlers
// ============================================================================

extern void* get_framebuffer(void);

// ============================================================================
// Syscall: read
// ============================================================================

int64_t sys_read(int fd, void* buf, uint64_t count) {
    (void)fd; (void)buf; (void)count;
    return 0;
}

// ============================================================================
// Syscall: write
// ============================================================================

int64_t sys_write(int fd, const void* buf, uint64_t count) {
    (void)fd; (void)buf;
    return count;
}

// ============================================================================
// Syscall: exit
// ============================================================================

void sys_exit(int code) {
    (void)code;
    for (;;) asm volatile ("hlt");
}

// ============================================================================
// Syscall: getpid
// ============================================================================

uint64_t sys_getpid(void) {
    return 1;
}

// ============================================================================
// Main Syscall Handler
// ============================================================================

uint64_t syscall_handler(uint64_t syscall_nr, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    if (syscall_nr >= SYS_NR) {
        return -1;
    }
    
    switch (syscall_nr) {
        case SYS_READ:
            return sys_read((int)arg1, (void*)arg2, arg3);
        case SYS_WRITE:
            return sys_write((int)arg1, (const void*)arg2, arg3);
        case SYS_EXIT:
            sys_exit((int)arg1);
            return 0;
        case SYS_GETPID:
            return sys_getpid();
        default:
            return -1;
    }
}

// ============================================================================
// Initialize Syscall Interface
// ============================================================================

void syscall_init(void) {
    // TODO: Set up syscall handler in IDT
}
