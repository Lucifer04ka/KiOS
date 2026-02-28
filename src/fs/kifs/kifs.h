#ifndef KIFS_H
#define KIFS_H

#include <stdint.h>
#include <string.h>

// ============================================================================
// KiFS Constants
// ============================================================================

#define KIFS_MAGIC         0x4B465332  // "KFS2"
#define KIFS_VERSION        1
#define KIFS_BLOCK_SIZE    4096
#define KIFS_INODE_SIZE     256

// File types
#define KIFS_TYPE_FILE      0x8000
#define KIFS_TYPE_DIR       0x4000
#define KIFS_TYPE_SYMLINK   0xA000

// ============================================================================
// KiFS Structures
// ============================================================================

// Superblock (first block)
typedef struct {
    uint32_t magic;           // 0x4B465332
    uint16_t version;         // 1
    uint16_t block_size;      // 4096
    uint32_t block_count;     // Total blocks
    uint32_t inode_count;     // Total inodes
    uint32_t free_blocks;    // Free blocks
    uint32_t free_inodes;    // Free inodes
    uint32_t first_data;     // First data block
    uint32_t inode_table;     // Inode table start
    uint64_t created;        // Creation time
    uint64_t modified;        // Modification time
    uint32_t root_inode;     // Root directory inode
    uint8_t uuid[16];        // Filesystem UUID
    char label[64];          // Volume label
    uint8_t reserved[1024 - 160]; // Padding
} __attribute__((packed)) kifs_superblock_t;

// Inode
typedef struct {
    uint16_t mode;            // File type + permissions
    uint16_t uid;            // Owner UID
    uint32_t size;           // File size in bytes
    uint32_t atime;          // Last access
    uint32_t mtime;          // Last modified
    uint32_t ctime;          // Last changed
    uint32_t gid;            // Group ID
    uint32_t links;          // Hard links
    uint32_t blocks;          // Blocks allocated
    uint32_t flags;          // Flags
    uint32_t direct[12];     // Direct block pointers
    uint32_t indirect;       // Single indirect
    uint32_t double_ind;     // Double indirect
    uint32_t triple_ind;     // Triple indirect
    uint8_t reserved[256 - 92]; // Padding
} __attribute__((packed)) kifs_inode_t;

// Directory Entry
typedef struct {
    uint32_t inode;          // Inode number
    uint16_t rec_len;        // Record length
    uint8_t name_len;       // Name length
    uint8_t type;            // File type
    char name[255];         // Filename
} __attribute__((packed)) kifs_dirent_t;

// ============================================================================
// KiFS Functions
// ============================================================================

// Format filesystem
int kifs_format(void* storage, uint64_t size);

// Mount filesystem
int kifs_mount(void* storage);

// Unmount filesystem
int kfs_umount(void);

// Open file
int kifs_open(const char* path);

// Close file
int kifs_close(int fd);

// Read from file
int kifs_read(int fd, void* buf, uint64_t count);

// Write to file
int kifs_write(int fd, const void* buf, uint64_t count);

// Create file
int kifs_create(const char* path, uint16_t mode);

// Create directory
int kifs_mkdir(const char* path);

// List directory
int kifs_list(const char* path);

// Remove file
int kifs_unlink(const char* path);

// Get file size
uint64_t kifs_get_size(int fd);

#endif // KIFS_H
