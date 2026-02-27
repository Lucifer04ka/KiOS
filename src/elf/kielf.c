#include "kielf.h"
#include <string.h>

// ============================================================================
// Validate KiELF Header
// ============================================================================

int kielf_validate(void* data) {
    if (!data) return 0;
    
    kielf_header_t* hdr = (kielf_header_t*)data;
    
    // Check magic
    if (hdr->magic != KIELF_MAGIC) return 0;
    
    // Check version
    if (hdr->version != 1) return 0;
    
    // Check architecture
    if (hdr->arch != 0x40) return 0;
    
    return 1;
}

// ============================================================================
// Get Entry Point
// ============================================================================

uint64_t kielf_get_entry(void* data) {
    if (!data) return 0;
    
    kielf_header_t* hdr = (kielf_header_t*)data;
    return hdr->entry;
}

// ============================================================================
// Load Program Segments (simplified)
// ============================================================================

void* kielf_load(void* data, uint64_t* load_addr) {
    if (!data || !kielf_validate(data)) return NULL;
    
    kielf_header_t* hdr = (kielf_header_t*)data;
    
    // Find first PT_LOAD segment
    kielf_phdr_t* phdr = (kielf_phdr_t*)((uint8_t*)data + hdr->phoff);
    
    for (int i = 0; i < hdr->phnum; i++) {
        if (phdr[i].type == PT_LOAD) {
            if (load_addr) *load_addr = phdr[i].vaddr;
            // In real implementation, would copy segments to memory
            return (void*)phdr[i].vaddr;
        }
    }
    
    return NULL;
}

// ============================================================================
// Get Section by Name
// ============================================================================

void* kielf_get_section(void* data, const char* name) {
    if (!data || !name) return NULL;
    
    kielf_header_t* hdr = (kielf_header_t*)data;
    
    // Get string table
    kielf_shdr_t* shdr = (kielf_shdr_t*)((uint8_t*)data + hdr->shoff);
    char* strtab = (char*)data + shdr[hdr->shstrndx].offset;
    
    // Find section by name
    for (int i = 0; i < hdr->shnum; i++) {
        char* secname = strtab + shdr[i].name;
        if (strcmp(secname, name) == 0) {
            return (void*)((uint8_t*)data + shdr[i].offset);
        }
    }
    
    return NULL;
}

// ============================================================================
// Get Sections Count
// ============================================================================

int kielf_get_sections_count(void* data) {
    if (!data) return 0;
    kielf_header_t* hdr = (kielf_header_t*)data;
    return hdr->shnum;
}
