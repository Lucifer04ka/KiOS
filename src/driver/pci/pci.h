#ifndef PCI_H
#define PCI_H

#include <stdint.h>

// ============================================================================
// PCI Configuration Space
// ============================================================================

#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA   0xCFC

// PCI Device Classes
#define PCI_CLASS_MASS_STORAGE  0x01
#define PCI_CLASS_NETWORK      0x02
#define PCI_CLASS_DISPLAY     0x03
#define PCI_CLASS_BRIDGE      0x06

// Mass Storage Subclasses
#define PCI_SUBCLASS_IDE       0x01
#define PCI_SUBCLASS_SATA     0x06

// ============================================================================
// PCI Functions
// ============================================================================

// Read PCI config register
uint32_t pci_read(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);

// Write PCI config register
void pci_write(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t val);

// Get vendor ID
uint16_t pci_get_vendor(uint8_t bus, uint8_t dev, uint8_t func);

// Get device ID
uint16_t pci_get_device(uint8_t bus, uint8_t dev, uint8_t func);

// Get class code
uint32_t pci_get_class(uint8_t bus, uint8_t dev, uint8_t func);

// Get BAR (Base Address Register)
uint32_t pci_get_bar(uint8_t bus, uint8_t dev, uint8_t func, uint8_t bar_num);

// Scan for devices
void pci_scan(void (*callback)(uint8_t bus, uint8_t dev, uint8_t func, uint16_t vendor, uint16_t device, uint32_t class_code));

// Find SATA controller
int pci_find_sata(uint32_t* bar0);

#endif // PCI_H
