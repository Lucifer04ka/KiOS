#include "kifs.h"
#include "../vfs/vfs.h"
#include "../../mm/heap.h"
#include "../../mm/pmm.h"
#include <string.h>

// ============================================================================
// Global Variables
// ============================================================================

static void* kifs_storage = NULL;
static kifs_superblock_t* kifs_sb = NULL;
static int kifs_mounted = 0;

// ============================================================================
// Helper Functions
// ============================================================================

static kifs_inode_t* get_inode(uint32_t num) {
    if (!kifs_storage || num == 0 || num > kifs_sb->inode_count) return NULL;
    return (kifs_inode_t*)((uint8_t*)kifs_storage + KIFS_BLOCK_SIZE + (num - 1) * KIFS_INODE_SIZE);
}

static uint32_t allocate_block(void) {
    if (!kifs_sb) return 0;
    if (kifs_sb->free_blocks == 0) return 0;
    
    // Simple allocation - just return next block
    uint32_t block = kifs_sb->first_data + (kifs_sb->block_count - kifs_sb->free_blocks);
    kifs_sb->free_blocks--;
    return block;
}

static void* get_block(uint32_t num) {
    if (!kifs_storage || num == 0) return NULL;
    return (uint8_t*)kifs_storage + num * KIFS_BLOCK_SIZE;
}

// ============================================================================
// Format Filesystem
// ============================================================================

int kifs_format(void* storage, uint64_t size) {
    if (!storage || size < KIFS_BLOCK_SIZE * 10) return -1;
    
    // Zero out
    memset(storage, 0, KIFS_BLOCK_SIZE * 10);
    
    kifs_sb = (kifs_superblock_t*)storage;
    
    // Set superblock
    kifs_sb->magic = KIFS_MAGIC;
    kifs_sb->version = KIFS_VERSION;
    kifs_sb->block_size = KIFS_BLOCK_SIZE;
    kifs_sb->block_count = (size / KIFS_BLOCK_SIZE) / 2; // Half for data
    kifs_sb->inode_count = 64; // Fixed number for simplicity
    kifs_sb->free_blocks = kifs_sb->block_count - 2;
    kifs_sb->free_inodes = kifs_sb->inode_count - 1;
    kifs_sb->first_data = 2;
    kifs_sb->inode_table = 1;
    kifs_sb->root_inode = 1;
    
    // Create root inode
    kifs_inode_t* root = get_inode(1);
    root->mode = KIFS_TYPE_DIR | 0755;
    root->size = KIFS_BLOCK_SIZE;
    root->links = 2;
    root->blocks = 1;
    root->direct[0] = 2;
    
    // Create "." and ".." entries in root directory
    kifs_dirent_t* de = (kifs_dirent_t*)get_block(2);
    de->inode = 1;
    de->name_len = 1;
    de->type = 2; // DT_DIR
    de->name[0] = '.';
    de->rec_len = 16;
    
    de = (kifs_dirent_t*)((uint8_t*)de + 16);
    de->inode = 1;
    de->name_len = 2;
    de->type = 2;
    de->name[0] = '.';
    de->name[1] = '.';
    de->rec_len = KIFS_BLOCK_SIZE - 16;
    
    kifs_storage = storage;
    kifs_mounted = 1;
    
    return 0;
}

// ============================================================================
// Mount
// ============================================================================

int kifs_mount(void* storage) {
    if (!storage) return -1;
    
    kifs_sb = (kifs_superblock_t*)storage;
    
    if (kifs_sb->magic != KIFS_MAGIC) {
        return -1;
    }
    
    kifs_storage = storage;
    kifs_mounted = 1;
    
    return 0;
}

// ============================================================================
// Unmount
// ============================================================================

int kfs_umount(void) {
    kifs_mounted = 0;
    kifs_storage = NULL;
    kifs_sb = NULL;
    return 0;
}

// ============================================================================
// Open File (simple version)
// ============================================================================

static int current_fd = -1;
static kifs_inode_t* current_inode = NULL;
static uint64_t current_pos = 0;

int kifs_open(const char* path) {
    if (!kifs_mounted || !path) return -1;
    
    // Only support root for now
    if (path[0] == '/' && path[1] == '\0') {
        current_inode = get_inode(1);
        current_pos = 0;
        current_fd = 0;
        return current_fd;
    }
    
    return -1;
}

// ============================================================================
// Close
// ============================================================================

int kifs_close(int fd) {
    if (fd != current_fd) return -1;
    current_fd = -1;
    current_inode = NULL;
    current_pos = 0;
    return 0;
}

// ============================================================================
// Read
// ============================================================================

int kifs_read(int fd, void* buf, uint64_t count) {
    if (!kifs_mounted || fd != current_fd || !current_inode) return -1;
    if (current_pos >= current_inode->size) return 0;
    
    // Read from first data block
    if (current_inode->direct[0] == 0) return 0;
    
    void* block = get_block(current_inode->direct[0]);
    if (!block) return 0;
    
    uint64_t to_read = count;
    if (current_pos + to_read > current_inode->size) {
        to_read = current_inode->size - current_pos;
    }
    
    memcpy(buf, (uint8_t*)block + current_pos, to_read);
    current_pos += to_read;
    
    return to_read;
}

// ============================================================================
// Write
// ============================================================================

int kifs_write(int fd, const void* buf, uint64_t count) {
    if (!kifs_mounted || fd != current_fd || !current_inode) return -1;
    
    // Allocate block if needed
    if (current_inode->direct[0] == 0) {
        current_inode->direct[0] = allocate_block();
        if (current_inode->direct[0] == 0) return -1;
        
        void* block = get_block(current_inode->direct[0]);
        if (!block) return -1;
        memset(block, 0, KIFS_BLOCK_SIZE);
    }
    
    void* data_block = get_block(current_inode->direct[0]);
    if (!data_block) return 0;
    
    uint64_t to_write = count;
    if (current_pos + to_write > KIFS_BLOCK_SIZE) {
        to_write = KIFS_BLOCK_SIZE - current_pos;
    }
    
    memcpy((uint8_t*)data_block + current_pos, buf, to_write);
    current_pos += to_write;
    
    if (current_pos > current_inode->size) {
        current_inode->size = current_pos;
    }
    
    return to_write;
}

// ============================================================================
// Create File
// ============================================================================

int kifs_create(const char* path, uint16_t mode) {
    if (!kifs_mounted || !path) return -1;
    if (kifs_sb->free_inodes == 0) return -1;
    
    // Find free inode
    uint32_t ino = 1;
    for (; ino <= kifs_sb->inode_count; ino++) {
        kifs_inode_t* inode = get_inode(ino);
        if (inode->mode == 0) break;
    }
    
    if (ino > kifs_sb->inode_count) return -1;
    
    kifs_inode_t* inode = get_inode(ino);
    inode->mode = KIFS_TYPE_FILE | (mode & 0777);
    inode->size = 0;
    inode->links = 1;
    inode->blocks = 0;
    
    // Add to root directory
    // (simplified - would need proper dir entry management)
    
    kifs_sb->free_inodes--;
    
    return ino;
}

// ============================================================================
// Make Directory
// ============================================================================

int kifs_mkdir(const char* path) {
    if (!kifs_mounted || !path) return -1;
    if (kifs_sb->free_inodes == 0) return -1;
    
    uint32_t ino = 1;
    for (; ino <= kifs_sb->inode_count; ino++) {
        kifs_inode_t* inode = get_inode(ino);
        if (inode->mode == 0) break;
    }
    
    if (ino > kifs_sb->inode_count) return -1;
    
    kifs_inode_t* inode = get_inode(ino);
    inode->mode = KIFS_TYPE_DIR | 0755;
    inode->size = KIFS_BLOCK_SIZE;
    inode->links = 2;
    inode->blocks = 1;
    inode->direct[0] = allocate_block();
    
    // Create . and .. entries
    if (inode->direct[0]) {
        void* block = get_block(inode->direct[0]);
        kifs_dirent_t* de = (kifs_dirent_t*)block;
        de->inode = ino;
        de->name_len = 1;
        de->type = 2;
        de->name[0] = '.';
        de->rec_len = 16;
        
        de = (kifs_dirent_t*)((uint8_t*)de + 16);
        de->inode = 1;
        de->name_len = 2;
        de->type = 2;
        de->name[0] = '.';
        de->name[1] = '.';
        de->rec_len = KIFS_BLOCK_SIZE - 16;
    }
    
    kifs_sb->free_inodes--;
    
    return ino;
}

// ============================================================================
// List Directory
// ============================================================================

int kifs_list(const char* path) {
    if (!kifs_mounted) return -1;
    
    // List root directory
    kifs_inode_t* root = get_inode(1);
    if (!root || root->direct[0] == 0) return -1;
    
    void* block = get_block(root->direct[0]);
    if (!block) return -1;
    
    // Just return count for now
    return (root->size > 0) ? 1 : 0;
}

// ============================================================================
// Get Size
// ============================================================================

uint64_t kifs_get_size(int fd) {
    if (fd != current_fd || !current_inode) return 0;
    return current_inode->size;
}
