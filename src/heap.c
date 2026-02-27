#include "heap.h"
#include "pmm.h"

static uint64_t heap_start_addr = 0;
static uint64_t heap_current = 0;
static uint64_t heap_end = 0;
static uint64_t hhdm_off = 0;

void heap_init(uint64_t hhdm_offset) {
    hhdm_off = hhdm_offset;
    void* phys_page = pmm_alloc_page();
    if (phys_page == NULL) return;

    heap_start_addr = (uint64_t)phys_page + hhdm_off;
    heap_current = heap_start_addr;
    heap_end = heap_current + PAGE_SIZE;
}

void* kmalloc(size_t size) {
    // Выравнивание по 8 байт для стабильности
    size = (size + 7) & ~7;

    // Если места не хватает — расширяем кучу!
    while (heap_current + size > heap_end) {
        void* new_page = pmm_alloc_page();
        if (new_page == NULL) return NULL; // Совсем кончилась память в ПК

        // В текущей реализации (без VMM) мы просто надеемся, 
        // что PMM выдаст нам следующую физическую страницу подряд.
        // Для простого ядра на старте это работает.
        heap_end += PAGE_SIZE;
    }

    void* ptr = (void*)heap_current;
    heap_current += size;
    return ptr;
}

void kfree(void* ptr) {
    (void)ptr; // Всё еще заглушка, пока не перейдем на сложный аллокатор
}

uint64_t heap_get_used() { return heap_current - heap_start_addr; }
uint64_t heap_get_total() { return heap_end - heap_start_addr; }