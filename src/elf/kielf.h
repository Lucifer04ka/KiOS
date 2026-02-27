#ifndef KIELF_H
#define KIELF_H

#include <stdint.h>

// ============================================================================
// KiELF Magic
// ============================================================================

#define KIELF_MAGIC     0x4B454C46  // "KELF"

// ============================================================================
// KiELF Types
// ============================================================================

#define KIELF_TYPE_EXE   1
#define KIELF_TYPE_SO    2
#define KIELF_TYPE_OBJ   3

// ============================================================================
// KiELF Header (64 bytes)
// ============================================================================

typedef struct {
    uint32_t magic;         // 0x4B454C46
    uint16_t version;       // 1
    uint16_t type;         // 1=EXE, 2=SO, 3=OBJ
    uint32_t entry;        // Entry point
    uint32_t phoff;        // Program headers offset
    uint32_t shoff;        // Section headers offset
    uint32_t flags;        // Flags
    uint16_t phentsize;    // Program header size
    uint16_t phnum;        // Program header count
    uint16_t shentsize;    // Section header size
    uint16_t shnum;        // Section header count
    uint16_t shstrndx;      // Section string table index
    uint8_t  abi;          // ABI (0 = KiOS)
    uint8_t  arch;         // Architecture (0x40 = x86_64)
    uint16_t reserved;
} __attribute__((packed)) kielf_header_t;

// ============================================================================
// Program Header
// ============================================================================

typedef struct {
    uint32_t type;         // PT_LOAD, PT_DYNAMIC, etc.
    uint32_t flags;       // PF_X, PF_W, PF_R
    uint64_t offset;      // File offset
    uint64_t vaddr;       // Virtual address
    uint64_t paddr;       // Physical address
    uint64_t filesz;      // Size in file
    uint64_t memsz;       // Size in memory
    uint64_t align;       // Alignment
} __attribute__((packed)) kielf_phdr_t;

// ============================================================================
// Section Header
// ============================================================================

typedef struct {
    uint32_t name;        // Section name (string table index)
    uint32_t type;        // Section type
    uint64_t flags;       // Section flags
    uint64_t addr;        // Virtual address
    uint64_t offset;     // File offset
    uint64_t size;        // Section size
    uint32_t link;        // Link to other section
    uint32_t info;        // Additional info
    uint64_t addralign;   // Alignment
    uint64_t entsize;     // Entry size (for rel, symtab)
} __attribute__((packed)) kielf_shdr_t;

// ============================================================================
// Program Header Types
// ============================================================================

#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4

// ============================================================================
// Section Types
// ============================================================================

#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_NOBITS      8
#define SHT_REL         9

// ============================================================================
// Section Names
// ============================================================================

#define SECTION_TEXT    ".text"
#define SECTION_DATA    ".data"
#define SECTION_RODATA ".rodata"
#define SECTION_BSS     ".bss"
#define SECTION_SYMTAB  ".symtab"
#define SECTION_STRTAB ".strtab"

// ============================================================================
// KiELF Functions
// ============================================================================

// Validate KiELF header
int kielf_validate(void* data);

// Get entry point
uint64_t kielf_get_entry(void* data);

// Load program segments
void* kielf_load(void* data, uint64_t* load_addr);

// Get section by name
void* kielf_get_section(void* data, const char* name);

// Get section count
int kielf_get_sections_count(void* data);

#endif // KIELF_H
