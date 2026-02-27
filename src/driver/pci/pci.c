#include "pci.h"

// ============================================================================
// Read PCI Configuration Register
// ============================================================================

uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    // Enable bit 31, set bus, device, function, and offset
    uint32_t address = (1U << 31) | ((uint32_t)bus << 16) | 
                      ((uint32_t)dev << 11) | ((uint32_t)func << 8) | 
                      (offset & 0xFC);
    
    // Write address to config address register
    outl(PCI_CONFIG_ADDRESS, address);
    
    // Read data from config data register
    uint32_t data = inl(PCI_CONFIG_DATA);
    
    // Shift to align the data (for 8-bit and 16-bit reads)
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
            // Check if device exists (vendor != 0xFFFF)
            uint16_t vendor = pci_get_vendor(bus, dev, 0);
            if (vendor == 0xFFFF || vendor == 0) continue;
            
            // Check all functions (in case of multi-function device)
            for (uint8_t func = 0; func < 8; func++) {
                vendor = pci_get_vendor(bus, dev, func);
                if (vendor == 0xFFFF) {
                    if (func == 0) break;  // No functions on this device
                    continue;
                }
                
                uint16_t device = pci_get_device(bus, dev, func);
                uint32_t class_code = pci_get_class(bus, dev, func);
                
                callback(bus, dev, func, vendor, device, class_code);
                
                // If not a multi-function device, break
                if (func == 0 && !(pci_read(bus, dev, 0, 0x0E) & 0x80)) {
                    break;
                }
            }
        }
    }
}

// ============================================================================
// Find SATA Controller
// ============================================================================

static void pci_find_sata_callback(uint8_t bus, uint8_t dev, uint8_t func, 
                                   uint16_t vendor, uint16_t device, uint32_t class_code) {
    (void)vendor;
    (void)device;
    
    uint8_t class = (class_code >> 16) & 0xFF;
    uint8_t subclass = (class_code >> 8) & 0xFF;
    
    // Check for SATA controller (class 01, subclass 06)
    if (class == PCI_CLASS_MASS_STORAGE && subclass == PCI_CLASS_SATA) {
        uint32_t bar0 = pci_get_bar(bus, dev, func, 0);
        // Save BAR0 somewhere for AHCI driver
    }
}

int pci_find_sata(uint32_t* bar0) {
    // Simple implementation - would need to store found controller
    (void)bar0;
    return 0;
}
