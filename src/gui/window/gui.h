#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <stdint.h>
#include <stddef.h>
#include "../../gfx/2d/gfx.h"

// ============================================================================
// Window
// ============================================================================

#define MAX_WINDOWS 8

typedef struct window {
    int id;
    char title[64];
    int x, y;
    int width, height;
    uint32_t bg_color;
    uint32_t border_color;
    uint32_t title_color;
    int visible;
    int focused;
    struct window* next;
} window_t;

// ============================================================================
// Button
// ============================================================================

typedef struct button {
    int x, y;
    int width, height;
    char text[32];
    uint32_t bg_color;
    uint32_t fg_color;
    void (*on_click)(void);
    int hovered;
} button_t;

// ============================================================================
// GUI Functions
// ============================================================================

// Initialize GUI
void gui_init(void* fb, uint32_t width, uint32_t height);

// Create window
window_t* gui_create_window(const char* title, int x, int y, int width, int height);

// Close window
void gui_close_window(int id);

// Show window
void gui_show_window(int id);

// Hide window
void gui_hide_window(int id);

// Focus window
void gui_focus_window(int id);

// Render all windows
void gui_render(void);

// Draw window
void gui_draw_window(window_t* win);

// Add button to window
void gui_window_add_button(window_t* win, button_t* btn);

// Render button
void gui_render_button(button_t* btn);

// Check if point is in rectangle
int gui_contains(int px, int py, int rx, int ry, int rw, int rh);

#endif // GUI_WINDOW_H
