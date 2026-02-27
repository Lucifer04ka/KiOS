#ifndef VFS_H
#define VFS_H

#include <stdint.h>

// ============================================================================
// VFS Types
// ============================================================================

#define VFS_MAX_PATH    256
#define VFS_MAX_NAME    128

// File types
#define VFS_TYPE_FILE       0x8000
#define VFS_TYPE_DIR        0x4000
#define VFS_TYPE_SYMLINK    0xA000
#define VFS_TYPE_DEVICE     0x2000

// Open flags
#define VFS_O_RDONLY    0x0001
#define VFS_O_WRONLY    0x0002
#define VFS_O_RDWR      0x0004
#define VFS_O_CREATE    0x0010
#define VFS_O_TRUNC     0x0020

// Seek modes
#define VFS_SEEK_SET    0
#define VFS_SEEK_CUR    1
#define VFS_SEEK_END    2

// ============================================================================
// VFS Structures
// ============================================================================

typedef struct vfs_inode vfs_inode_t;
typedef struct vfs_file vfs_file_t;
typedef struct vfs_mount vfs_mount_t;

// File descriptor
typedef struct {
    vfs_inode_t* inode;
    uint32_t flags;
    uint64_t position;
    uint32_t refcount;
} vfs_fd_t;

// Mount point
typedef struct vfs_mount {
    char device[64];
    char mountpoint[64];
    void* fs_data;
    int (*read)(void* fs_data, uint64_t lba, uint32_t count, void* buf);
    int (*write)(void* fs_data, uint64_t lba, uint32_t count, void* buf);
    struct vfs_mount* next;
} vfs_mount_t;

// ============================================================================
// VFS Functions
// ============================================================================

// Initialize VFS
void vfs_init(void);

// Mount filesystem
int vfs_mount(const char* device, const char* mountpoint, void* fs_data, 
              int (*read)(void*, uint64_t, uint32_t, void*), 
              int (*write)(void*, uint64_t, uint32_t, void*));

// Open file
int vfs_open(const char* path, uint32_t flags);

// Close file
int vfs_close(int fd);

// Read from file
int vfs_read(int fd, void* buf, uint64_t count);

// Write to file
int vfs_write(int fd, const void* buf, uint64_t count);

// Seek
int vfs_seek(int fd, int64_t offset, uint32_t mode);

// Get file size
uint64_t vfs_get_size(int fd);

// List directory
int vfs_list(const char* path);

// Create file
int vfs_create(const char* path, uint16_t mode);

// Create directory
int vfs_mkdir(const char* path);

// Remove file
int vfs_unlink(const char* path);

#endif // VFS_H
