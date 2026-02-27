#include "pci.h"

// ============================================================================
// Inline Assembly for I/O Ports
// ============================================================================

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t val;
    asm volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

// ============================================================================
// Read PCI Configuration Register
// ============================================================================

uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | 
                      ((uint32_t)dev << 11) | ((uint32_t)func << 8) | 
                      (offset & 0xFC);
    
    outl(PCI_CONFIG_ADDRESS, address);
    uint32_t data = inl(PCI_CONFIG_DATA);
    
    return data >> ((offset & 2) * 8);
}

// ============================================================================
// Write PCI Configuration Register
// ============================================================================

void pci_write(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t val) {
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | 
                      ((uint32_t)dev << 11) | ((uint32_t)func << 8) | 
                      (offset & 0xFC);
    
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, val);
}

// ============================================================================
// Get Vendor ID
// ============================================================================

uint16_t pci_get_vendor(uint8_t bus, uint8_t dev, uint8_t func) {
    return (uint16_t)pci_read(bus, dev, func, 0);
}

// ============================================================================
// Get Device ID
// ============================================================================

uint16_t pci_get_device(uint8_t bus, uint8_t dev, uint8_t func) {
    return (uint16_t)(pci_read(bus, dev, func, 2) >> 16);
}

// ============================================================================
// Get Class Code
// ============================================================================

uint32_t pci_get_class(uint8_t bus, uint8_t dev, uint8_t func) {
    return pci_read(bus, dev, func, 8) & 0xFFFFFF;
}

// ============================================================================
// Get BAR
// ============================================================================

uint32_t pci_get_bar(uint8_t bus, uint8_t dev, uint8_t func, uint8_t bar_num) {
    if (bar_num > 5) return 0;
    return pci_read(bus, dev, func, 0x10 + bar_num * 4);
}

// ============================================================================
// Scan PCI Bus
// ============================================================================

void pci_scan(void (*callback)(uint8_t bus, uint8_t dev, uint8_t func, uint16_t vendor, uint16_t device, uint32_t class_code)) {
    for (uint8_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            uint16_t vendor = pci_get_vendor(bus, dev, 0);
            if (vendor == 0xFFFF || vendor == 0) continue;
            
            for (uint8_t func = 0; func < 8; func++) {
                vendor = pci_get_vendor(bus, dev, func);
                if (vendor == 0xFFFF) {
                    if (func == 0) break;
                    continue;
                }
                
                uint16_t device = pci_get_device(bus, dev, func);
                uint32_t class_code = pci_get_class(bus, dev, func);
                
                callback(bus, dev, func, vendor, device, class_code);
                
                if (func == 0 && !(pci_read(bus, dev, 0, 0x0E) & 0x80)) {
                    break;
                }
            }
        }
    }
}

// ============================================================================
// Find SATA Controller (stub)
// ============================================================================

int pci_find_sata(uint32_t* bar0) {
    (void)bar0;
    return 0;
}
