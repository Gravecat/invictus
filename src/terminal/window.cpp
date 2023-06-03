// terminal/window.cpp -- The Window class, part of the Terminal subsystem, allowing easier management of Curses windows and panels.
// Copyright © 2019, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <curses.h>
#include <panel.h>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"


namespace invictus
{

Window::Window(int width, int height, int new_x, int new_y)
{
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    if (new_x < 0) new_x = 0;
    if (new_y < 0) new_y = 0;

    height_ = height;
    width_ = width;
    x_ = new_x;
    y_ = new_y;
    window_ptr_ = newwin(height, width, new_y, new_x);
    panel_ptr_ = new_panel(window_ptr_);
}

Window::~Window()
{
    del_panel(panel_ptr_);
    delwin(window_ptr_);
}

// Read-only access to the Window's height.
uint16_t Window::get_height() const { return height_; }

// Read-only access to the Window's width.
uint16_t Window::get_width() const { return width_; }

// Moves this Window's underlying panel to new coordinates.
void Window::move(int new_x, int new_y)
{
    x_ = new_x;
    y_ = new_y;
    move_panel(panel_ptr_, y_, x_);
}

// Set this Window's panel as visible or invisible.
void Window::set_visible(bool vis)
{
    if (vis) show_panel(panel_ptr_);
    else hide_panel(panel_ptr_);
}

// Returns a pointer to the WINDOW struct.
WINDOW* Window::win() const { return window_ptr_; }

}   // namespace invictus
