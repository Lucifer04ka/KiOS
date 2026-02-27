#include "syscall.h"
#include "../vmm/vmm.h"
#include "../pmm.h"
#include "../heap.h"
#include <string.h>

// ============================================================================
// Syscall Handlers Table
// ============================================================================

typedef uint64_t (*syscall_handler_t)(uint64_t, uint64_t, uint64_t, uint64_t);

static syscall_handler_t syscall_table[SYS_NR] = { NULL };

// ============================================================================
// Internal I/O Buffers (for now - stdin from keyboard, stdout to framebuffer)
// ============================================================================

extern void* get_framebuffer(void);

// ============================================================================
// Syscall: read
// ============================================================================

int64_t sys_read(int fd, void* buf, uint64_t count) {
    if (fd != FD_STDIN) {
        return -1;  // Not supported yet
    }
    
    // For now, just return 0 (no input available)
    return 0;
}

// ============================================================================
// Syscall: write
// ============================================================================

int64_t sys_write(int fd, const void* buf, uint64_t count) {
    if (fd != FD_STDOUT && fd != FD_STDERR) {
        return -1;
    }
    
    // Write to framebuffer
    void* fb = get_framebuffer();
    if (!fb) return -1;
    
    // For now, simple implementation - just return count
    // In full implementation, we'd render text to framebuffer
    
    return count;
}

// ============================================================================
// Syscall: mmap - memory mapping
// ============================================================================

void* sys_mmap(void* addr, uint64_t length, int prot, int flags, int fd, uint64_t offset) {
    // Allocate physical pages
    uint64_t pages = (length + 4095) / 4096;
    
    // For MAP_ANONYMOUS, just allocate pages
    if (flags & MAP_ANONYMOUS) {
        // In a full implementation, we'd create a new mapping
        // For now, return NULL (not fully implemented)
        return NULL;
    }
    
    // Not supported yet for file-backed mappings
    return NULL;
}

// ============================================================================
// Syscall: munmap
// ============================================================================

int sys_munmap(void* addr, uint64_t length) {
    // Would free the mapping
    // Not implemented yet
    return 0;
}

// ============================================================================
// Syscall: exit
// ============================================================================

void sys_exit(int code) {
    // Hang the system (infinite halt)
    for (;;) asm volatile ("hlt");
}

// ============================================================================
// Syscall: getpid
// ============================================================================

uint64_t sys_getpid(void) {
    return 1;  // First (and only) process
}

// ============================================================================
// Syscall: brk (change data segment size)
// ============================================================================

int sys_brk(uint64_t brk) {
    // Not implemented - just return current brk
    return 0;
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
        case SYS_MMAP:
            return (uint64_t)sys_mmap((void*)arg1, arg2, (int)arg3, 
                                       (int)((arg3 >> 32) & 0xFFFFFFFF), 
                                       0, 0);
        case SYS_MUNMAP:
            return sys_munmap((void*)arg1, arg2);
        case SYS_EXIT:
            sys_exit((int)arg1);
            return 0;  // Never returns
        case SYS_GETPID:
            return sys_getpid();
        case SYS_BRK:
            return sys_brk(arg1);
        default:
            return -1;
    }
}

// ============================================================================
// Register a syscall handler
// ============================================================================

void syscall_register(uint64_t num, void* handler) {
    if (num < SYS_NR) {
        syscall_table[num] = (syscall_handler_t)handler;
    }
}

// ============================================================================
// Initialize Syscall Interface
// ============================================================================

void syscall_init(void) {
    // Set up syscall handler in IDT
    // This would be done through idt.c in a full implementation
    
    // Register default syscalls
    // In a full implementation, we'd set up the syscall table here
}
