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
// Point
// ============================================================================

typedef struct {
    int32_t x;
    int32_t y;
} point_t;

// ============================================================================
// Rectangle
// ============================================================================

typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} rect_t;

// ============================================================================
// 2D Functions
// ============================================================================

// Initialize graphics
void gfx_init(void* framebuffer, uint32_t width, uint32_t height, uint32_t pitch);

// Clear screen
void gfx_clear(color_t color);

// Draw pixel
void gfx_putpixel(int32_t x, int32_t y, color_t color);

// Get pixel
color_t gfx_getpixel(int32_t x, int32_t y);

// Draw line
void gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color);

// Draw rectangle (outline)
void gfx_draw_rect(int32_t x, int32_t y, int32_t width, int32_t height, color_t color);

// Draw filled rectangle
void gfx_fill_rect(int32_t x, int32_t y, int32_t width, int32_t height, color_t color);

// Draw circle
void gfx_draw_circle(int32_t cx, int32_t cy, int32_t radius, color_t color);

// Draw filled circle
void gfx_fill_circle(int32_t cx, int32_t cy, int32_t radius, color_t color);

// Draw text (simple bitmap)
void gfx_draw_text(const char* text, int32_t x, int32_t y, color_t color);

// Get screen dimensions
uint32_t gfx_get_width(void);
uint32_t gfx_get_height(void);

#endif // GFX_2D_H
