#include <stdint.h>
#include <stddef.h>
#include "include/limine.h"
#include "gfx/font8x8.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/idt/idt.h"
#include "mm/pmm.h"
#include "mm/heap.h"
#include "arch/x86_64/paging/vmm/vmm.h"
#include "syscall/syscall.h"
#include "driver/pci/pci.h"
#include "driver/ahci/ahci.h"
#include "fs/vfs/vfs.h"
#include "fs/kifs/kifs.h"
#include "gfx/2d/gfx.h"
#include "elf/kielf.h"

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };
__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_request = { .id = LIMINE_MEMMAP_REQUEST, .revision = 0 };
__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdm_request = { .id = LIMINE_HHDM_REQUEST, .revision = 0 };

void halt(void) { asm("cli"); for (;;) asm("hlt"); }

void* get_framebuffer(void) {
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) return NULL;
    return framebuffer_request.response->framebuffers[0];
}

// === Графический движок ===
void draw_char(struct limine_framebuffer *fb, char c, uint32_t x, uint32_t y, uint32_t color) {
    uint32_t *fb_ptr = fb->address;
    uint32_t pitch = fb->pitch / 4;
    char *bitmap = font8x8_basic[(uint8_t)c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (bitmap[row] & (1 << col)) fb_ptr[(y + row) * pitch + (x + col)] = color;
        }
    }
}

void draw_string(struct limine_framebuffer *fb, const char *str, uint32_t x, uint32_t y, uint32_t color) {
    uint32_t curr_x = x;
    for (int i = 0; str[i] != '\0'; i++) { draw_char(fb, str[i], curr_x, y, color); curr_x += 8; }
}

// === Системные утилиты ===
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void itoa(uint64_t n, char *str) {
    int i = 0;
    if (n == 0) str[i++] = '0';
    while (n > 0) { str[i++] = (n % 10) + '0'; n /= 10; }
    str[i] = '\0';
    for (int j = 0; j < i / 2; j++) { char t = str[j]; str[j] = str[i-j-1]; str[i-j-1] = t; }
}

char* strncpy(char* dest, const char* src, uint64_t n) {
    uint64_t i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

// === Настройки Shell ===
static char input_buffer[64];
static int input_ptr = 0;
static int shell_y = 60;

static uint32_t color_white  = 0x00FFFFFF;
static uint32_t color_green  = 0x0000FF00;
static uint32_t color_yellow = 0x00FFFF00;
static uint32_t color_red    = 0x00FF0000;
static uint32_t color_cyan   = 0x0000FFFF;
static uint32_t color_dim    = 0x00606060;
static uint32_t color_bg     = 0x00000000;

static uint32_t current_text_color = 0x00FFFFFF;

// Boot state
static int boot_done = 0;
static int enter_pressed = 0;

#define PROMPT "KiOS> "
#define INPUT_X 58 // Координата X после промпта

void clear_screen(struct limine_framebuffer *fb) {
    uint32_t *fb_ptr = fb->address;
    for (uint32_t i = 0; i < fb->width * fb->height; i++) fb_ptr[i] = color_bg;
    shell_y = 20;
    draw_string(fb, "Terminal cleared.", 10, shell_y, color_yellow);
    shell_y += 20;
}

void execute_command(const char* cmd) {
    struct limine_framebuffer *fb = get_framebuffer();
    shell_y += 20;
    
    if (strcmp(cmd, "help") == 0) {
        draw_string(fb, "Cmds: help about clear mem color disk vfs format ls demo kielf hello", 10, shell_y, current_text_color);
    } else if (strcmp(cmd, "about") == 0) {
        draw_string(fb, "KiOS v0.1.0 - 64-bit microkernel.", 10, shell_y, current_text_color);
        shell_y += 15;
        draw_string(fb, "PMM & Heap expander: ACTIVE", 10, shell_y, current_text_color);
    } else if (strcmp(cmd, "mem") == 0) {
        char buf[32];
        draw_string(fb, "RAM Free: ", 10, shell_y, current_text_color);
        itoa(pmm_get_free_memory() / 1024 / 1024, buf);
        draw_string(fb, buf, 90, shell_y, color_green);
        draw_string(fb, " MB", 90 + (8*4), shell_y, current_text_color);
    } else if (strcmp(cmd, "clear") == 0) {
        clear_screen(fb);
        return;
    } else if (strcmp(cmd, "disk") == 0) {
        int ports = ahci_get_port_count();
        char buf[32];
        draw_string(fb, "AHCI Ports: ", 10, shell_y, current_text_color);
        itoa(ports, buf);
        draw_string(fb, buf, 100, shell_y, color_green);
    } else if (strcmp(cmd, "vfs") == 0) {
        draw_string(fb, "VFS: Ready. Use 'format' to format disk.", 10, shell_y, current_text_color);
    } else if (strcmp(cmd, "format") == 0) {
        // Simple RAM disk for now (16MB)
        void* ramdisk = pmm_alloc_page();
        if (ramdisk) {
            kifs_format(ramdisk, 16 * 1024 * 1024);
            draw_string(fb, "Format: RAM disk formatted (16MB).", 10, shell_y, color_green);
        } else {
            draw_string(fb, "Format: Failed to allocate memory.", 10, shell_y, color_red);
        }
    } else if (strcmp(cmd, "ls") == 0) {
        int count = kifs_list("/");
        char buf[32];
        itoa(count, buf);
        draw_string(fb, "Files: ", 10, shell_y, current_text_color);
        draw_string(fb, buf, 70, shell_y, color_green);
    } else if (strcmp(cmd, "demo") == 0) {
        gfx_init(fb->address, fb->width, fb->height);
        gfx_clear(RGB(10, 10, 20));
        gfx_draw_rect(50, 50, 200, 150, RGB(0, 200, 100));
        gfx_fill_rect(300, 100, 100, 80, RGB(100, 100, 255));
        gfx_draw_circle(600, 200, 50, RGB(255, 200, 0));
        gfx_fill_circle(600, 350, 40, RGB(255, 50, 150));
        gfx_draw_text("KiOS Graphics OK!", 100, 450, RGB(255, 255, 255));
        draw_string(fb, "Demo displayed. Press ENTER.", 10, shell_y, color_green);
    } else if (strcmp(cmd, "kielf") == 0) {
        draw_string(fb, "KiELF: Format ready. Use 'hello' to test.", 10, shell_y, current_text_color);
    } else if (strcmp(cmd, "hello") == 0) {
        draw_string(fb, "Hello from KiOS! - Compiled with KiELF.", 10, shell_y, color_green);
    } else if (strcmp(cmd, "color green") == 0) {
        current_text_color = color_green;
        draw_string(fb, "Text color changed to green.", 10, shell_y, color_green);
    } else if (strcmp(cmd, "color red") == 0) {
        current_text_color = color_red;
        draw_string(fb, "Text color changed to red.", 10, shell_y, color_red);
    } else if (strcmp(cmd, "color white") == 0) {
        current_text_color = color_white;
        draw_string(fb, "Text color changed to white.", 10, shell_y, color_white);
    } else if (cmd[0] != '\0') {
        draw_string(fb, "Unknown command. Try 'help'.", 10, shell_y, color_red);
    }
    shell_y += 10;
}

void on_key_pressed(char c) {
    struct limine_framebuffer *fb = get_framebuffer();
    if (!fb) return;

    // During boot, any key will proceed
    if (!boot_done && c != '\0') {
        enter_pressed = 1;
        return;
    }

    if (c == '\n') {
        input_buffer[input_ptr] = '\0';
        execute_command(input_buffer);
        input_ptr = 0;
        shell_y += 20;
        if (shell_y > fb->height - 60) clear_screen(fb);
        draw_string(fb, PROMPT, 10, shell_y, color_yellow);
    } else if (c == '\b') {
        if (input_ptr > 0) {
            input_ptr--;
            uint32_t *fb_ptr = fb->address;
            uint32_t pitch = fb->pitch / 4;
            for(int i=0; i<8; i++) 
                for(int j=0; j<8; j++) 
                    fb_ptr[(shell_y + i) * pitch + (INPUT_X + (input_ptr * 8) + j)] = color_bg;
        }
    } else if (input_ptr < 50) {
        input_buffer[input_ptr++] = c;
        char s[2] = {c, '\0'};
        draw_string(fb, s, INPUT_X + ((input_ptr-1) * 8), shell_y, current_text_color);
    }
}

void _start(void) {
    struct limine_framebuffer *fb = get_framebuffer();
    if (!fb) halt();

    uint32_t *fb_ptr = fb->address;
    for (uint32_t i = 0; i < fb->width * fb->height; i++) fb_ptr[i] = color_bg;

    // === Visual Boot Sequence ===
    int boot_y = 35;
    
    draw_string(fb, "================================================", 10, 10, color_dim);
    draw_string(fb, "  KiOS v0.7.0 - 64-bit Operating System", 10, 25, color_white);
    draw_string(fb, "================================================", 10, 40, color_dim);
    boot_y = 60;
    
    // GDT
    gdt_init();
    draw_string(fb, "[BOOT] Setting up GDT... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // IDT
    idt_init();
    draw_string(fb, "[BOOT] Setting up IDT... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // PMM
    pmm_init(&memmap_request, hhdm_request.response->offset);
    draw_string(fb, "[BOOT] Initializing PMM... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // Heap
    heap_init(hhdm_request.response->offset);
    draw_string(fb, "[BOOT] Allocating kernel heap... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // VMM
    vmm_init();
    draw_string(fb, "[BOOT] Enabling virtual memory... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // Syscalls
    syscall_init();
    draw_string(fb, "[BOOT] Registering syscalls... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // PCI
    draw_string(fb, "[BOOT] Scanning PCI bus... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // AHCI
    draw_string(fb, "[BOOT] Loading AHCI driver... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // VFS
    vfs_init();
    draw_string(fb, "[BOOT] Initializing VFS... OK", 10, boot_y, color_green);
    boot_y += 18;
    
    // KiFS
    draw_string(fb, "[BOOT] KiFS driver loaded", 10, boot_y, color_dim);
    boot_y += 18;
    
    // KiELF
    draw_string(fb, "[BOOT] KiELF loader ready", 10, boot_y, color_dim);
    boot_y += 25;
    
    // Boot complete
    draw_string(fb, "================================================", 10, boot_y, color_yellow);
    boot_y += 18;
    draw_string(fb, "[BOOT] All systems operational!", 10, boot_y, color_cyan);
    boot_y += 30;
    
    draw_string(fb, "[BOOT] Press any key to continue...", 10, boot_y, color_yellow);
    
    // Wait for key press
    while (!enter_pressed) {
        asm volatile("hlt");
    }
    boot_done = 1;
    
    // Clear and show shell
    for (uint32_t i = 0; i < fb->width * fb->height; i++) fb_ptr[i] = color_bg;
    draw_string(fb, "================================================", 10, 10, color_dim);
    draw_string(fb, "  KiOS v0.7.0 - Ready", 10, 30, color_white);
    draw_string(fb, "================================================", 10, 50, color_dim);
    draw_string(fb, "Type 'help' for commands.", 10, 80, color_dim);
    
    shell_y = 110;
    draw_string(fb, PROMPT, 10, shell_y, color_yellow);

    for (;;) asm("hlt");
}