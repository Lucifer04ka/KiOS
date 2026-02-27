#include "gfx.h"
#include <stddef.h>

extern char font8x8_basic[128][8];

// ============================================================================
// Global Variables
// ============================================================================

static void* gfx_fb = NULL;
static uint32_t gfx_width = 0;
static uint32_t gfx_height = 0;
static uint32_t gfx_pitch = 0;

// ============================================================================
// Initialize
// ============================================================================

void gfx_init(void* framebuffer, uint32_t width, uint32_t height, uint32_t pitch) {
    gfx_fb = framebuffer;
    gfx_width = width;
    gfx_height = height;
    gfx_pitch = pitch;
}

// ============================================================================
// Get Dimensions
// ============================================================================

uint32_t gfx_get_width(void) { return gfx_width; }
uint32_t gfx_get_height(void) { return gfx_height; }

// ============================================================================
// Clear
// ============================================================================

void gfx_clear(color_t color) {
    if (!gfx_fb) return;
    uint32_t* fb = (uint32_t*)gfx_fb;
    for (uint32_t i = 0; i < gfx_width * gfx_height; i++) {
        fb[i] = color;
    }
}

// ============================================================================
// Put Pixel
// ============================================================================

void gfx_putpixel(int32_t x, int32_t y, color_t color) {
    if (!gfx_fb) return;
    if (x < 0 || x >= (int32_t)gfx_width || y < 0 || y >= (int32_t)gfx_height) return;
    
    uint32_t* fb = (uint32_t*)gfx_fb;
    fb[y * (gfx_pitch / 4) + x] = color;
}

// ============================================================================
// Get Pixel
// ============================================================================

color_t gfx_getpixel(int32_t x, int32_t y) {
    if (!gfx_fb) return 0;
    if (x < 0 || x >= (int32_t)gfx_width || y < 0 || y >= (int32_t)gfx_height) return 0;
    
    uint32_t* fb = (uint32_t*)gfx_fb;
    return fb[y * (gfx_pitch / 4) + x];
}

// ============================================================================
// Draw Line (Bresenham's algorithm)
// ============================================================================

void gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color) {
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    int32_t sx = (dx > 0) ? 1 : -1;
    int32_t sy = (dy > 0) ? 1 : -1;
    dx = (dx > 0) ? dx : -dx;
    dy = (dy > 0) ? dy : -dy;
    
    int32_t err = dx - dy;
    
    while (1) {
        gfx_putpixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int32_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// ============================================================================
// Draw Rectangle
// ============================================================================

void gfx_draw_rect(int32_t x, int32_t y, int32_t width, int32_t height, color_t color) {
    gfx_draw_line(x, y, x + width, y, color);
    gfx_draw_line(x + width, y, x + width, y + height, color);
    gfx_draw_line(x + width, y + height, x, y + height, color);
    gfx_draw_line(x, y + height, x, y, color);
}

// ============================================================================
// Fill Rectangle
// ============================================================================

void gfx_fill_rect(int32_t x, int32_t y, int32_t width, int32_t height, color_t color) {
    for (int32_t j = 0; j < height; j++) {
        for (int32_t i = 0; i < width; i++) {
            gfx_putpixel(x + i, y + j, color);
        }
    }
}

// ============================================================================
// Draw Circle
// ============================================================================

void gfx_draw_circle(int32_t cx, int32_t cy, int32_t radius, color_t color) {
    int32_t x = radius;
    int32_t y = 0;
    int32_t err = 0;
    
    while (x >= y) {
        gfx_putpixel(cx + x, cy + y, color);
        gfx_putpixel(cx + y, cy + x, color);
        gfx_putpixel(cx - y, cy + x, color);
        gfx_putpixel(cx - x, cy + y, color);
        gfx_putpixel(cx - x, cy - y, color);
        gfx_putpixel(cx - y, cy - x, color);
        gfx_putpixel(cx + y, cy - x, color);
        gfx_putpixel(cx + x, cy - y, color);
        
        y += 1;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2 * x;
        }
    }
}

// ============================================================================
// Fill Circle
// ============================================================================

void gfx_fill_circle(int32_t cx, int32_t cy, int32_t radius, color_t color) {
    for (int32_t y = -radius; y <= radius; y++) {
        for (int32_t x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius * radius) {
                gfx_putpixel(cx + x, cy + y, color);
            }
        }
    }
}

// ============================================================================
// Draw Text (using font8x8)
// ============================================================================

void gfx_draw_text(const char* text, int32_t x, int32_t y, color_t color) {
    if (!text) return;
    
    int32_t cursor_x = x;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        // Draw 8x8 character
        char* bitmap = font8x8_basic[(uint8_t)c];
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (bitmap[row] & (1 << col)) {
                    gfx_putpixel(cursor_x + col, y + row, color);
                }
            }
        }
        
        cursor_x += 8;
    }
}
