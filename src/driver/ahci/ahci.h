#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>

// ============================================================================
// AHCI Registers (Memory Mapped)
// ============================================================================

#define AHCI_BASE_ADDR     0x400000  // Placeholder - will be set by PCI

// HBA Registers
#define AHCI_CAP          0x00  // Host Capabilities
#define AHCI_GHC          0x04  // Global Host Control
#define AHCI_IS           0x08  // Interrupt Status
#define AHCI_PI           0x0C  // Ports Implemented
#define AHCI_VERSION      0x10  // Version
#define AHCI_CCCCTL      0x14  // Command Completion Coalescing Control
#define AHCI_CCCPT       0x18  // Command Completion Coalescing Ports
#define AHCI_EMLOC       0x1C  // Enclosure Management Location
#define AHCI_EMCTL       0x20  // Enclosure Management Control
#define AHCI_CAP2        0x24  // Host Capabilities Extended
#define AHCI_BOHC        0x28  // BIOS/OS Handoff

// HBA Global Host Control
#define AHCI_GHC_AE      (1 << 31)  // AHCI Enable
#define AHCI_GHC_IE       (1 << 1)   // Interrupt Enable
#define AHCI_GHC_HR       (1 << 0)   // HBA Reset

// Port Registers (offset from port base)
#define AHCI_PxCLB       0x00  // Command List Base Address
#define AHCI_PxCLBU      0x04  // Command List Base Address Upper
#define AHCI_PxFB        0x08  // FIS Base Address
#define AHCI_PxFBU       0x0C  // FIS Base Address Upper
#define AHCI_PxIS        0x10  // Interrupt Status
#define AHCI_PxIE       0x14  // Interrupt Enable
#define AHCI_PxCMD      0x18  // Command
#define AHCI_PxTFD      0x20  // Task File Data
#define AHCI_PxSIG      0x24  // Signature
#define AHCI_PxSSTS     0x28  // SATA Status
#define AHCI_PxSCTL     0x2C  // SATA Control
#define AHCI_PxSERR     0x30  // SATA Error
#define AHCI_PxSACT     0x34  // SATA Active
#define AHCI_PxCI       0x38  // Command Issue

// Port Command Register
#define AHCI_PxCMD_ST    (1 << 0)   // Start
#define AHCI_PxCMD_SUD  (1 << 1)   // Spin-Up Device
#define AHCI_PxCMD_POD  (1 << 2)   // Power On Device
#define AHCI_PxCMD_CLO  (1 << 3)   // Command List Override
#define AHCI_PxCMD_FRE  (1 << 4)   // FIS Receive Enable
#define AHCI_PxCMD_WDE  (1 << 7)   // Write Data Erase
#define AHCI_PxCMD_ALPE (1 << 15)  // Aggressive Link Power Management
#define AHCI_PxCMD_ASP  (1 << 14)  // Aggressive Sleep Management
#define AHCI_PxCMD_ICC  (1 << 4)   // Interface Communication Control

// SATA Status (SSTS)
#define AHCI_SSTS_DET_MASK   0x0F  // Detection
#define AHCI_SSTS_DET_NONE   0x00  // No device
#define AHCI_SSTS_DET_PRESENT 0x01  // Device present (Phy established)
#define AHCI_SSTS_DET_COMM   0x03  // Communication established
#define AHCI_SSTS_IPM_MASK   0xF0  // Interface Power Management
#define AHCI_SSTS_IPM_ACTIVE 0x00   // Active

// Task File Status (TFD)
#define AHCI_TFD_ERR   (1 << 0)   // Error
#define AHCI_TFD_DRQ   (1 << 3)   // Data Request
#define AHCI_TFD_BSY   (1 << 7)   // Busy

// ATA Commands
#define ATA_CMD_READ_DMA_EX    0x25
#define ATA_CMD_WRITE_DMA_EX   0x35
#define ATA_CMD_IDENTIFY       0xEC

// ============================================================================
// Data Structures
// ============================================================================

// HBA Memory (1KB per port)
typedef struct {
    uint32_t clb;        // 0x00 - Command List Base
    uint32_t clbu;       // 0x04 - Command List Base Upper
    uint32_t fb;         // 0x08 - FIS Base
    uint32_t fbu;        // 0x0C - FIS Base Upper
    uint32_t is;         // 0x10 - Interrupt Status
    uint32_t ie;         // 0x14 - Interrupt Enable
    uint32_t cmd;        // 0x18 - Command
    uint32_t rsvd0;      // 0x1C
    uint32_t tfd;        // 0x20 - Task File Data
    uint32_t sig;        // 0x24 - Signature
    uint32_t ssts;       // 0x28 - SATA Status
    uint32_t sctl;       // 0x2C - SATA Control
    uint32_t serr;       // 0x30 - SATA Error
    uint32_t sact;       // 0x34 - SATA Active
    uint32_t ci;         // 0x38 - Command Issue
    uint32_t sntf;       // 0x3C - Notification
    uint32_t rsvd1[11];  // 0x40-0x6F
    uint32_t rsvd2[32];  // 0x70-0xFF
} __attribute__((packed)) ahci_port_t;

// HBA Capabilities
typedef struct {
    uint32_t cap;        // 0x00
    uint32_t ghc;        // 0x04
    uint32_t is;         // 0x08
    uint32_t pi;         // 0x0C
    uint32_t version;   // 0x10
    uint32_t ccc_ctl;   // 0x14
    uint32_t ccc_pt;    // 0x18
    uint32_t em_loc;    // 0x1C
    uint32_t em_ctl;    // 0x20
    uint32_t cap2;      // 0x24
    uint32_t bohc;      // 0x28
} __attribute__((packed)) ahci_hba_t;

// Command Header (32 bytes)
typedef struct {
    uint16_t cfl;        // Command FIS Length (in DWORDS)
    uint16_t a;          // ATAPI
    uint8_t p;           // Prefetchable
    uint8_t r;           // Reset
    uint8_t b;           // BIST
    uint8_t rsvd;        // Reserved
    uint8_t pmpm;        // Port Multiplier Port
    uint16_t rsvd1;      // Reserved
    uint32_t prdtl;      // Physical Region Descriptor Table Length
    uint32_t prdbc;      // PRD Byte Count
    uint32_t ctba;       // Command Table Base Address
    uint32_t ctbau;      // Command Table Base Upper
    uint32_t rsvd2[4];   // Reserved
} __attribute__((packed)) ahci_cmd_header_t;

// PRD Entry (Physical Region Descriptor)
typedef struct {
    uint32_t dba;        // Data Base Address
    uint32_t dbau;       // Data Base Address Upper
    uint32_t rsvd;       // Reserved
    uint32_t dbc;        // Byte Count (0 = 64KB)
} __attribute__((packed)) ahci_prd_t;

// ATA Identify Data (512 bytes)
typedef struct {
    uint16_t rsvd1[1];
    uint16_t caps;       // 1:Capabilities
    uint16_t rsvd2[1];
    uint16_t tfeats;    // 3:Task file features
    uint16_t sectors[5]; // 6-9:Current sectors
    uint16_t rsvd3[20];
    uint8_t  serial[20]; // 10-19:Serial number
    uint16_t rsvd4[3];
    uint8_t  model[40];  // 23-26:Model number
    uint16_t rsvd5[13];
    uint16_t commands1; // 59:Commands supported
    uint16_t commands2; 
    uint32_t sectors_lba; // 100-101:Total sectors
    uint16_t rsvd6[152];
} __attribute__((packed)) ata_identify_t;

// ============================================================================
// AHCI Driver Functions
// ============================================================================

// Initialize AHCI controller
int ahci_init(void* pci_bar);

// Check if device is present
int ahci_check_device(int port);

// Initialize a port
int ahci_port_init(int port);

// Read sectors from disk
int ahci_read(int port, uint64_t lba, uint32_t count, void* buffer);

// Write sectors to disk
int ahci_write(int port, uint64_t lba, uint32_t count, void* buffer);

// Get number of ports
int ahci_get_port_count(void);

// Get port base address
void* ahci_get_port_base(int port);

#endif // AHCI_H
