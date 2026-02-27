#include "gdt.h"

// Структура одной записи GDT (ровно 8 байт)
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// Структура указателя, которую мы скормим процессору
struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct gdt_entry gdt[5];
struct gdt_ptr gp;

// Вспомогательная функция для заполнения одной строчки таблицы
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init(void) {
    // Настраиваем указатель: размер таблицы и где она лежит
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base = (uint64_t)&gdt;

    // 0: Null-дескриптор (процессор требует, чтобы первая запись была пустой)
    gdt_set_gate(0, 0, 0, 0, 0);
    // 1: Код Ядра (Ring 0, 64-bit). Флаги: 0x9A (исполняемый), 0x20 (64-битный режим)
    gdt_set_gate(1, 0, 0, 0x9A, 0x20);
    // 2: Данные Ядра (Ring 0). Флаги: 0x92 (доступ на чтение/запись)
    gdt_set_gate(2, 0, 0, 0x92, 0x00);
    // 3: Данные Пользователя (Ring 3). Флаги: 0xF2
    gdt_set_gate(3, 0, 0, 0xF2, 0x00);
    // 4: Код Пользователя (Ring 3, 64-bit). Флаги: 0xFA
    gdt_set_gate(4, 0, 0, 0xFA, 0x20);

    // Магия Ассемблера: загружаем новую таблицу и перезагружаем регистры процессора!
    asm volatile(
        "lgdt %0\n\t"           // Говорим процессору "вот новая GDT"
        "push $0x08\n\t"        // Кладем в стек номер нашего сегмента Кода Ядра (0x08)
        "lea 1f(%%rip), %%rax\n\t" 
        "push %%rax\n\t"        // Кладем в стек адрес следующей инструкции
        "lretq\n\t"             // Делаем дальний прыжок (Far Return), чтобы обновить CS
        "1:\n\t"
        "mov $0x10, %%ax\n\t"   // Берем номер нашего сегмента Данных Ядра (0x10)
        "mov %%ax, %%ds\n\t"    // И обновляем все остальные регистры
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        :
        : "m"(gp)
        : "rax", "memory"
    );
}