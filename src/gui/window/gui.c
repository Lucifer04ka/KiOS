#include "gui.h"
#include "../../mm/heap.h"
#include <string.h>

// ============================================================================
// Global Variables
// ============================================================================

static window_t* windows = NULL;
static int window_counter = 0;
static int gui_initialized = 0;

// ============================================================================
// Initialize GUI
// ============================================================================

void gui_init(void* fb, uint32_t width, uint32_t height) {
    if (gui_initialized) return;
    
    gfx_init(fb, width, height, width * 4);
    windows = NULL;
    window_counter = 0;
    gui_initialized = 1;
}

// ============================================================================
// Create Window
// ============================================================================

window_t* gui_create_window(const char* title, int x, int y, int width, int height) {
    window_t* win = kmalloc(sizeof(window_t));
    if (!win) return NULL;
    
    win->id = window_counter++;
    strncpy(win->title, title, 63);
    win->title[63] = '\0';
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->bg_color = RGB(45, 45, 50);
    win->border_color = RGB(70, 70, 80);
    win->title_color = RGB(30, 30, 40);
    win->visible = 1;
    win->focused = 1;
    win->next = windows;
    windows = win;
    
    return win;
}

// ============================================================================
// Close Window
// ============================================================================

void gui_close_window(int id) {
    window_t** curr = &windows;
    
    while (*curr) {
        if ((*curr)->id == id) {
            window_t* to_free = *curr;
            *curr = (*curr)->next;
            kfree(to_free);
            return;
        }
        curr = &(*curr)->next;
    }
}

// ============================================================================
// Show Window
// ============================================================================

void gui_show_window(int id) {
    window_t* win = windows;
    while (win) {
        if (win->id == id) {
            win->visible = 1;
            return;
        }
        win = win->next;
    }
}

// ============================================================================
// Hide Window
// ============================================================================

void gui_hide_window(int id) {
    window_t* win = windows;
    while (win) {
        if (win->id == id) {
            win->visible = 0;
            return;
        }
        win = win->next;
    }
}

// ============================================================================
// Focus Window
// ============================================================================

void gui_focus_window(int id) {
    window_t* win = windows;
    while (win) {
        win->focused = (win->id == id);
        win = win->next;
    }
}

// ============================================================================
// Check if point in rectangle
// ============================================================================

int gui_contains(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px < rx + rw && py >= ry && py < ry + rh);
}

// ============================================================================
// Draw Window
// ============================================================================

void gui_draw_window(window_t* win) {
    if (!win->visible) return;
    
    // Draw background
    gfx_fill_rect(win->x, win->y, win->width, win->height, win->bg_color);
    
    // Draw title bar
    gfx_fill_rect(win->x, win->y, win->width, 24, win->title_color);
    
    // Draw border
    gfx_draw_rect(win->x, win->y, win->width, win->height, win->border_color);
    
    // Draw title text
    gfx_draw_text(win->title, win->x + 5, win->y + 5, RGB(200, 200, 200));
    
    // Draw close button
    gfx_draw_rect(win->x + win->width - 20, win->y + 3, 14, 14, RGB(150, 50, 50));
    gfx_draw_text("X", win->x + win->width - 17, win->y + 5, RGB(200, 100, 100));
}

// ============================================================================
// Render Button
// ============================================================================

void gui_render_button(button_t* btn) {
    uint32_t bg = btn->bg_color;
    if (btn->hovered) {
        bg = RGB(80, 80, 90);
    }
    
    gfx_fill_rect(btn->x, btn->y, btn->width, btn->height, bg);
    gfx_draw_rect(btn->x, btn->y, btn->width, btn->height, RGB(100, 100, 110));
    gfx_draw_text(btn->text, btn->x + 5, btn->y + 5, btn->fg_color);
}

// ============================================================================
// Render All Windows
// ============================================================================

void gui_render(void) {
    // Render windows in reverse order (top to bottom)
    window_t* win = windows;
    while (win) {
        gui_draw_window(win);
        win = win->next;
    }
}
