#include "pmm.h"
#include <stdbool.h>

static uint8_t *bitmap;
static uint64_t total_pages;
static uint64_t bitmap_size;
static uint64_t hhdm_off;

static void bitmap_set(uint64_t bit) { bitmap[bit / 8] |= (1 << (bit % 8)); }
static void bitmap_clear(uint64_t bit) { bitmap[bit / 8] &= ~(1 << (bit % 8)); }
static bool bitmap_test(uint64_t bit) { return bitmap[bit / 8] & (1 << (bit % 8)); }

void pmm_init(struct limine_memmap_request *request, uint64_t hhdm_offset) {
    hhdm_off = hhdm_offset;
    struct limine_memmap_response *response = request->response;
    uint64_t max_address = 0;

    for (uint64_t i = 0; i < response->entry_count; i++) {
        struct limine_memmap_entry *entry = response->entries[i];
        if (entry->base + entry->length > max_address) {
            max_address = entry->base + entry->length;
        }
    }

    total_pages = max_address / PAGE_SIZE;
    bitmap_size = total_pages / 8;

    bitmap = NULL;
    for (uint64_t i = 0; i < response->entry_count; i++) {
        struct limine_memmap_entry *entry = response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size) {
            bitmap = (uint8_t *)(entry->base + hhdm_off);
            entry->base += bitmap_size;
            entry->length -= bitmap_size;
            break;
        }
    }

    if (!bitmap) return;

    for (uint64_t i = 0; i < bitmap_size; i++) bitmap[i] = 0xFF;

    for (uint64_t i = 0; i < response->entry_count; i++) {
        struct limine_memmap_entry *entry = response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE) {
                bitmap_clear((entry->base + j) / PAGE_SIZE);
            }
        }
    }
}

void *pmm_alloc_page() {
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (void *)(i * PAGE_SIZE);
        }
    }
    return NULL; 
}

void pmm_free_page(void *addr) {
    uint64_t bit = (uint64_t)addr / PAGE_SIZE;
    bitmap_clear(bit);
}

uint64_t pmm_get_free_memory() {
    uint64_t free_pages = 0;
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) free_pages++;
    }
    return free_pages * PAGE_SIZE;
}