#pragma once
#include <stdint.h>
#include <stddef.h>

void* kmalloc(size_t size);
void kfree(void* ptr);
void heap_init(uint64_t hhdm_offset);

// Статистика кучи
uint64_t heap_get_used();
uint64_t heap_get_total();