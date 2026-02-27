#include "ahci.h"
#include "../../mm/pmm.h"
#include <string.h>

// ============================================================================
// Global Variables
// ============================================================================

static ahci_hba_t* ahci_base = NULL;
static int ahci_port_count = 0;
static int ahci_ports[32] = {0};
static int ahci_initialized = 0;

// ============================================================================
// Helper Functions
// ============================================================================

// Read from AHCI register
static uint32_t ahci_read_reg(uint32_t reg) {
    if (!ahci_base) return 0;
    return *(volatile uint32_t*)((uint64_t)ahci_base + reg);
}

// Write to AHCI register
static void ahci_write_reg(uint32_t reg, uint32_t val) {
    if (!ahci_base) return;
    *(volatile uint32_t*)((uint64_t)ahci_base + reg) = val;
}

// Wait for bit to be set
static int ahci_wait_for(volatile uint32_t* reg, uint32_t mask, uint32_t timeout) {
    uint32_t t = 0;
    while (t < timeout) {
        if (*reg & mask) return 1;
        t++;
    }
    return 0;
}

// ============================================================================
// Initialize AHCI Controller
// ============================================================================

int ahci_init(void* pci_bar) {
    if (ahci_initialized) return 0;
    
    ahci_base = (ahci_hba_t*)pci_bar;
    if (!ahci_base) {
        // Use default address if no PCI BAR provided
        ahci_base = (ahci_hba_t*)0xFE000000;
    }
    
    // Read capabilities
    uint32_t cap = ahci_read_reg(AHCI_CAP);
    ahci_port_count = (cap & 0x1F) + 1;  // Number of ports (0-31 + 1)
    
    // Read ports implemented
    uint32_t pi = ahci_read_reg(AHCI_PI);
    
    // Find available ports
    int port_idx = 0;
    for (int i = 0; i < 32; i++) {
        if (pi & (1 << i)) {
            ahci_ports[port_idx++] = i;
        }
    }
    
    // Enable AHCI
    uint32_t ghc = ahci_read_reg(AHCI_GHC);
    ghc |= AHCI_GHC_AE;
    ahci_write_reg(AHCI_GHC, ghc);
    
    ahci_initialized = 1;
    
    return ahci_port_count;
}

// ============================================================================
// Check if Device is Present
// ============================================================================

int ahci_check_device(int port) {
    if (!ahci_base || port < 0 || port >= 32) return 0;
    
    // Get port base
    ahci_port_t* port_base = (ahci_port_t*)((uint64_t)ahci_base + 0x100 + (port * 0x80));
    
    // Read SATA status
    uint32_t ssts = port_base->ssts;
    
    // Check detection and power management
    uint8_t det = ssts & 0x0F;
    uint8_t ipm = (ssts >> 8) & 0x0F;
    
    return (det == 3 && ipm == 1) ? 1 : 0;
}

// ============================================================================
// Initialize Port
// ============================================================================

int ahci_port_init(int port) {
    if (!ahci_base || port < 0 || port >= 32) return -1;
    
    // Get port base
    ahci_port_t* port_base = (ahci_port_t*)((uint64_t)ahci_base + 0x100 + (port * 0x80));
    
    // Stop command engine
    uint32_t cmd = port_base->cmd;
    cmd &= ~AHCI_PxCMD_ST;
    port_base->cmd = cmd;
    
    // Wait for command engine to stop
    for (volatile int i = 0; i < 1000; i++) {
        if (!(port_base->cmd & AHCI_PxCMD_ST)) break;
    }
    
    // Disable FIS receive
    cmd = port_base->cmd;
    cmd &= ~AHCI_PxCMD_FRE;
    port_base->cmd = cmd;
    
    // Wait for FIS receive to stop
    for (volatile int i = 0; i < 1000; i++) {
        if (!(port_base->cmd & AHCI_PxCMD_FRE)) break;
    }
    
    // Clear any pending interrupts
    port_base->is = 0xFFFFFFFF;
    
    // Allocate command list and FIS buffer
    void* cmd_list = pmm_alloc_page();
    void* fis_buf = pmm_alloc_page();
    
    if (!cmd_list || !fis_buf) {
        return -1;
    }
    
    // Clear memory
    memset(cmd_list, 0, 1024);
    memset(fis_buf, 0, 256);
    
    // Set command list base
    port_base->clb = (uint32_t)(uint64_t)cmd_list;
    port_base->clbu = 0;
    
    // Set FIS base
    port_base->fb = (uint32_t)(uint64_t)fis_buf;
    port_base->fbu = 0;
    
    // Enable FIS receive
    cmd = port_base->cmd;
    cmd |= AHCI_PxCMD_FRE;
    port_base->cmd = cmd;
    
    // Start command engine
    cmd = port_base->cmd;
    cmd |= AHCI_PxCMD_ST;
    port_base->cmd = cmd;
    
    return 0;
}

// ============================================================================
// Read Sectors
// ============================================================================

int ahci_read(int port, uint64_t lba, uint32_t count, void* buffer) {
    if (!ahci_base || !buffer) return -1;
    
    // For now, just return -1 (not fully implemented)
    // This requires setting up PRD table, command FIS, etc.
    (void)port;
    (void)lba;
    (void)count;
    
    return -1;
}

// ============================================================================
// Write Sectors
// ============================================================================

int ahci_write(int port, uint64_t lba, uint32_t count, void* buffer) {
    if (!ahci_base || !buffer) return -1;
    
    // Not implemented yet
    (void)port;
    (void)lba;
    (void)count;
    (void)buffer;
    
    return -1;
}

// ============================================================================
// Get Port Count
// ============================================================================

int ahci_get_port_count(void) {
    return ahci_port_count;
}

// ============================================================================
// Get Port Base
// ============================================================================

void* ahci_get_port_base(int port) {
    if (!ahci_base || port < 0 || port >= 32) return NULL;
    return (void*)((uint64_t)ahci_base + 0x100 + (port * 0x80));
}
