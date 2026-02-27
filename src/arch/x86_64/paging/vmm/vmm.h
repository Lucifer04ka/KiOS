#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Paging Constants (x86_64 - 4-level paging)
// ============================================================================

#define PAGE_SHIFT      12
#define PAGE_SIZE       4096
#define PAGE_MASK       0xFFFFFFFFFFFFF000

#define PML4_INDEX(vaddr) (((vaddr) >> 39) & 0x1FF)
#define PDPT_INDEX(vaddr) (((vaddr) >> 30) & 0x1FF)
#define PD_INDEX(vaddr)   (((vaddr) >> 21) & 0x1FF)
#define PT_INDEX(vaddr)   (((vaddr) >> 12) & 0x1FF)

// ============================================================================
// Page Table Entry Flags
// ============================================================================

#define PTE_PRESENT     0x001   // Page is present
#define PTE_WRITABLE   0x002   // Writable
#define PTE_USER       0x004   // User accessible
#define PTE_WRITE_THROUGH 0x008 // Write-through caching
#define PTE_NO_CACHE   0x010   // Cache disabled
#define PTE_ACCESSED   0x020   // Accessed
#define PTE_DIRTY      0x040   // Dirty (for PTEs)
#define PTE_HUGE       0x080   // Huge page (1GB/2MB)
#define PTE_GLOBAL     0x100   // Global (not flushed on CR3 write)

// Kernel virtual address space
#define KERNEL_VMA      0xFFFF800000000000ULL
#define KERNEL_SIZE     0x40000000ULL  // 1GB for kernel

// ============================================================================
// Structures
// ============================================================================

typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) pml4_t;

typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) pdpt_t;

typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) pd_t;

typedef struct {
    uint64_t entries[512];
} __attribute__((aligned(4096))) pt_t;

// ============================================================================
// VMM Functions
// ============================================================================

// Initialize virtual memory manager
void vmm_init(void);

// Create new address space
pml4_t* vmm_create_address_space(void);

// Switch to different address space
void vmm_switch(pml4_t* pml4);

// Map virtual page to physical page
bool vmm_map(pml4_t* pml4, uint64_t virt, uint64_t phys, uint64_t flags);

// Unmap virtual page
bool vmm_unmap(pml4_t* pml4, uint64_t virt);

// Get page table entry (for debugging)
uint64_t* vmm_get_pte(pml4_t* pml4, uint64_t virt, bool create);

// Check if virtual address is mapped
bool vmm_is_mapped(pml4_t* pml4, uint64_t virt);

// Identity map region (for early boot)
void vmm_identity_map(uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags);

// Enable paging
void vmm_enable_paging(uint64_t cr3);

// Get current CR3 value
uint64_t vmm_get_cr3(void);

// Allocate a page table
void* vmm_alloc_pt(void);

// Free a page table  
void vmm_free_pt(void* pt);

#endif // VMM_H
