#include "gfx.h"
#include <stddef.h>

extern char font8x8_basic[128][8];

static void* gfx_fb = NULL;
static uint32_t gfx_w = 0;
static uint32_t gfx_h = 0;
static uint32_t gfx_pitch = 0;

void gfx_init(void* fb, uint32_t w, uint32_t h) {
    gfx_fb = fb;
    gfx_w = w;
    gfx_h = h;
    gfx_pitch = w * 4;
}

void gfx_clear(color_t c) {
    if (!gfx_fb) return;
    uint32_t* p = (uint32_t*)gfx_fb;
    for (uint32_t i = 0; i < gfx_w * gfx_h; i++) p[i] = c;
}

void gfx_putpixel(int x, int y, color_t c) {
    if (!gfx_fb) return;
    if (x < 0 || x >= (int)gfx_w || y < 0 || y >= (int)gfx_h) return;
    uint32_t* p = (uint32_t*)gfx_fb;
    p[y * (gfx_pitch/4) + x] = c;
}

void gfx_draw_line(int x1, int y1, int x2, int y2, color_t c) {
    int dx = x2 - x1, dy = y2 - y1;
    int sx = dx > 0 ? 1 : -1;
    int sy = dy > 0 ? 1 : -1;
    dx = dx > 0 ? dx : -dx;
    dy = dy > 0 ? dy : -dy;
    int err = dx - dy;
    
    while (1) {
        gfx_putpixel(x1, y1, c);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void gfx_draw_rect(int x, int y, int w, int h, color_t c) {
    gfx_draw_line(x, y, x+w, y, c);
    gfx_draw_line(x+w, y, x+w, y+h, c);
    gfx_draw_line(x+w, y+h, x, y+h, c);
    gfx_draw_line(x, y+h, x, y, c);
}

void gfx_fill_rect(int x, int y, int w, int h, color_t c) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            gfx_putpixel(x+i, y+j, c);
        }
    }
}

void gfx_draw_circle(int cx, int cy, int r, color_t c) {
    int x = r, y = 0, err = 0;
    while (x >= y) {
        gfx_putpixel(cx+x, cy+y, c); gfx_putpixel(cx+y, cy+x, c);
        gfx_putpixel(cx-y, cy+x, c); gfx_putpixel(cx-x, cy+y, c);
        gfx_putpixel(cx-x, cy-y, c); gfx_putpixel(cx-y, cy-x, c);
        gfx_putpixel(cx+y, cy-x, c); gfx_putpixel(cx+x, cy-y, c);
        y++;
        err += 1 + 2*y;
        if (2*(err-x)+1 > 0) { x--; err += 1 - 2*x; }
    }
}

void gfx_fill_circle(int cx, int cy, int r, color_t c) {
    for (int j = -r; j <= r; j++) {
        for (int i = -r; i <= r; i++) {
            if (i*i + j*j <= r*r) gfx_putpixel(cx+i, cy+j, c);
        }
    }
}

void gfx_draw_text(const char* s, int x, int y, color_t c) {
    if (!s) return;
    int px = x;
    while (*s) {
        char* bmp = font8x8_basic[(uint8_t)*s];
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (bmp[row] & (1 << col)) gfx_putpixel(px+col, y+row, c);
            }
        }
        px += 8;
        s++;
    }
}
