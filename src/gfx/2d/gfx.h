#ifndef GFX_2D_H
#define GFX_2D_H

#include <stdint.h>

// ============================================================================
// Color
// ============================================================================

typedef uint32_t color_t;

#define RGB(r, g, b)       ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a)  (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

// ============================================================================
// 2D Functions
// ============================================================================

// Initialize graphics
void gfx_init(void* framebuffer, uint32_t width, uint32_t height);

// Clear screen  
void gfx_clear(color_t color);

// Draw pixel
void gfx_putpixel(int x, int y, color_t color);

// Draw line
void gfx_draw_line(int x1, int y1, int x2, int y2, color_t color);

// Draw rectangle
void gfx_draw_rect(int x, int y, int w, int h, color_t color);

// Draw filled rectangle
void gfx_fill_rect(int x, int y, int w, int h, color_t color);

// Draw circle
void gfx_draw_circle(int cx, int cy, int radius, color_t color);

// Draw filled circle  
void gfx_fill_circle(int cx, int cy, int radius, color_t color);

// Draw text
void gfx_draw_text(const char* text, int x, int y, color_t color);

#endif
