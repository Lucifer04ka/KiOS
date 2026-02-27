#pragma once
#include <stdint.h>
#include <stddef.h>
#include "limine.h"

#define PAGE_SIZE 4096

void pmm_init(struct limine_memmap_request *request, uint64_t hhdm_offset);
void *pmm_alloc_page();
void pmm_free_page(void *addr);

// Возвращает количество свободной физической памяти в байтах
uint64_t pmm_get_free_memory();