#include "idt.h"

extern void draw_string(void *fb, const char *str, uint32_t x, uint32_t y, uint32_t color);
extern void halt(void);
extern void* get_framebuffer(void);
extern void on_key_pressed(char c);

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0
};

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct interrupt_frame {
    uint64_t rip; uint64_t cs; uint64_t rflags; uint64_t rsp; uint64_t ss;
};

__attribute__((aligned(0x10))) 
static struct idt_entry idt[256];
static struct idtr idtr;

__attribute__((interrupt))
void default_handler(struct interrupt_frame *frame) {
    (void)frame;
    outb(0x20, 0x20);
}

__attribute__((interrupt)) 
void isr0_divide_by_zero(struct interrupt_frame *frame) {
    (void)frame;
    void *fb = get_framebuffer();
    if (fb) draw_string(fb, "!!! KERNEL PANIC: DIV BY 0 !!!", 10, 300, 0x00FF0000);
    halt();
}

__attribute__((interrupt)) 
void keyboard_handler(struct interrupt_frame *frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char c = kbd_us[scancode];
        if (c > 0) on_key_pressed(c);
    }
    outb(0x20, 0x20);
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    struct idt_entry *descriptor = &idt[vector];
    uint64_t addr = (uint64_t)isr;
    descriptor->isr_low    = addr & 0xFFFF;
    descriptor->kernel_cs  = 0x08;
    descriptor->ist        = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid    = (addr >> 16) & 0xFFFF;
    descriptor->isr_high   = (addr >> 32) & 0xFFFFFFFF;
    descriptor->reserved   = 0;
}

void idt_init(void) {
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(struct idt_entry) * 256 - 1;
    for (int i = 0; i < 256; i++) idt_set_descriptor(i, default_handler, 0x8E);
    idt_set_descriptor(0, isr0_divide_by_zero, 0x8E);
    idt_set_descriptor(33, keyboard_handler, 0x8E);
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFD); outb(0xA1, 0xFF);
    asm volatile ("lidt %0" : : "m"(idtr));
    asm volatile ("sti"); 
}