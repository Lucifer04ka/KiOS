#include "vmm.h"
#include "../pmm.h"
#include "../heap.h"
#include "../idt.h"
#include <string.h>

// ============================================================================
// Simple string functions (for freestanding)
// ============================================================================

static int hex_to_str(uint64_t val, char* str) {
    const char* hex = "0123456789abcdef";
    int i = 0;
    for (int j = 15; j >= 0; j--) {
        str[i++] = hex[(val >> (j * 4)) & 0xF];
    }
    str[i] = '\0';
    return i;
}

static int int_to_str(int val, char* str) {
    char tmp[32];
    int i = 0;
    if (val == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    while (val > 0) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    for (int j = 0; j < i; j++) {
        str[j] = tmp[i - 1 - j];
    }
    str[i] = '\0';
    return i;
}

// ============================================================================
// Global Variables
// ============================================================================

static pml4_t* kernel_pml4 = NULL;
static bool vmm_initialized = false;

// ============================================================================
// Inline Assembly
// ============================================================================

static inline uint64_t read_cr3(void) {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

static inline void write_cr3(uint64_t cr3) {
    asm volatile("mov %0, %%cr3" :: "r"(cr3));
}

static inline void invalidate_page(uint64_t addr) {
    asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
}

// ============================================================================
// Page Table Helpers
// ============================================================================

static inline bool pte_present(uint64_t pte) {
    return (pte & PTE_PRESENT) != 0;
}

static inline uint64_t pte_get_phys(uint64_t pte) {
    return pte & PAGE_MASK;
}

static inline void pte_set(uint64_t* pte, uint64_t phys, uint64_t flags) {
    *pte = phys | flags | PTE_PRESENT;
}

static inline void pte_clear(uint64_t* pte) {
    *pte = 0;
}

// ============================================================================
// Allocate Page Table
// ============================================================================

void* vmm_alloc_pt(void) {
    void* page = pmm_alloc_page();
    if (page) {
        memset(page, 0, PAGE_SIZE);
    }
    return page;
}

void vmm_free_pt(void* pt) {
    if (pt) {
        pmm_free_page(pt);
    }
}

// ============================================================================
// Get/Create Page Table Entry
// ============================================================================

uint64_t* vmm_get_pte(pml4_t* pml4, uint64_t virt, bool create) {
    // Calculate indices
    size_t pml4_idx = PML4_INDEX(virt);
    size_t pdpt_idx = PDPT_INDEX(virt);
    size_t pd_idx   = PD_INDEX(virt);
    size_t pt_idx   = PT_INDEX(virt);
    
    // Get PML4 entry
    if (!pte_present(pml4->entries[pml4_idx])) {
        if (!create) return NULL;
        
        // Allocate PDPT
        pdpt_t* pdpt = vmm_alloc_pt();
        if (!pdpt) return NULL;
        
        pte_set(&pml4->entries[pml4_idx], (uint64_t)pdpt, PTE_WRITABLE);
    }
    
    pdpt_t* pdpt = (pdpt_t*)pte_get_phys(pml4->entries[pml4_idx]);
    
    // Get PDPT entry
    if (!pte_present(pdpt->entries[pdpt_idx])) {
        if (!create) return NULL;
        
        // Allocate PD
        pd_t* pd = vmm_alloc_pt();
        if (!pd) return NULL;
        
        pte_set(&pdpt->entries[pdpt_idx], (uint64_t)pd, PTE_WRITABLE);
    }
    
    pd_t* pd = (pd_t*)pte_get_phys(pdpt->entries[pdpt_idx]);
    
    // Get PD entry
    if (!pte_present(pd->entries[pd_idx])) {
        if (!create) return NULL;
        
        // Allocate PT
        pt_t* pt = vmm_alloc_pt();
        if (!pt) return NULL;
        
        pte_set(&pd->entries[pd_idx], (uint64_t)pt, PTE_WRITABLE);
    }
    
    pt_t* pt = (pt_t*)pte_get_phys(pd->entries[pd_idx]);
    
    // Return PTE
    return &pt->entries[pt_idx];
}

// ============================================================================
// Map Virtual to Physical
// ============================================================================

bool vmm_map(pml4_t* pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    // Align addresses
    virt = virt & PAGE_MASK;
    phys = phys & PAGE_MASK;
    
    // Get PTE (create page tables if needed)
    uint64_t* pte = vmm_get_pte(pml4, virt, true);
    if (!pte) return false;
    
    // Check if already mapped
    if (pte_present(*pte)) {
        return false;  // Already mapped
    }
    
    // Map it
    pte_set(pte, phys, flags);
    invalidate_page(virt);
    
    return true;
}

// ============================================================================
// Unmap Virtual Address
// ============================================================================

bool vmm_unmap(pml4_t* pml4, uint64_t virt) {
    virt = virt & PAGE_MASK;
    
    uint64_t* pte = vmm_get_pte(pml4, virt, false);
    if (!pte || !pte_present(*pte)) {
        return false;
    }
    
    pte_clear(pte);
    invalidate_page(virt);
    
    return true;
}

// ============================================================================
// Check if Address is Mapped
// ============================================================================

bool vmm_is_mapped(pml4_t* pml4, uint64_t virt) {
    uint64_t* pte = vmm_get_pte(pml4, virt, false);
    return pte && pte_present(*pte);
}

// ============================================================================
// Identity Map (for early boot - map physical = virtual)
// ============================================================================

void vmm_identity_map(uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags) {
    for (uint64_t addr = 0; addr < size; addr += PAGE_SIZE) {
        vmm_map(kernel_pml4, virt + addr, phys + addr, flags);
    }
}

// ============================================================================
// Create New Address Space
// ============================================================================

pml4_t* vmm_create_address_space(void) {
    pml4_t* pml4 = vmm_alloc_pt();
    if (!pml4) return NULL;
    
    memset(pml4, 0, sizeof(pml4_t));
    
    return pml4;
}

// ============================================================================
// Switch Address Space
// ============================================================================

void vmm_switch(pml4_t* pml4) {
    write_cr3((uint64_t)pml4);
}

// ============================================================================
// Enable Paging
// ============================================================================

void vmm_enable_paging(uint64_t cr3) {
    // Set CR3
    write_cr3(cr3);
    
    // Enable PAE (already set by bootloader for long mode)
    // Enable paging by setting CR0.PG
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // PG bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

// ============================================================================
// Get Current CR3
// ============================================================================

uint64_t vmm_get_cr3(void) {
    return read_cr3();
}

// ============================================================================
// Page Fault Handler
// ============================================================================

extern void draw_string(void *fb, const char *str, uint32_t x, uint32_t y, uint32_t color);
extern void* get_framebuffer(void);

void vmm_page_fault_handler(uint64_t error_code, uint64_t rip) {
    // Get faulting address
    uint64_t fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));
    
    void* fb = get_framebuffer();
    
    // Check if it's a present page (protection fault) vs missing page
    bool present = (error_code & 0x01) != 0;
    bool write = (error_code & 0x02) != 0;
    bool user = (error_code & 0x04) != 0;
    
    // For now, just halt on page fault
    if (fb) {
        draw_string(fb, "PAGE FAULT!", 10, 400, 0x00FF0000);
    }
    
    // Hang
    for(;;) asm("hlt");
}

// ============================================================================
// VMM Initialization
// ============================================================================

void vmm_init(void) {
    if (vmm_initialized) return;
    
    // Allocate kernel page tables
    kernel_pml4 = vmm_alloc_pt();
    if (!kernel_pml4) {
        // Panic - no memory
        for(;;) asm("hlt");
    }
    
    memset(kernel_pml4, 0, PAGE_SIZE);
    
    // Map kernel to high memory (0xFFFF800000000000)
    // For now, we'll use identity mapping for simplicity
    // In the future, map kernel to high addresses
    
    // Get memory info from Limine
    // For now, we rely on the bootloader having set up initial mapping
    
    // Enable paging
    vmm_enable_paging((uint64_t)kernel_pml4);
    
    vmm_initialized = true;
}
