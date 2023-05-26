// ui/menu.cpp -- Menu class definition, for a fairly generic scrollable menu of items.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/menu.hpp"
#include "ui/ui.hpp"
#include "util/strx.hpp"


namespace invictus
{

// Constructor.
Menu::Menu(int fx, int fy) : allow_highlight_(true), fixed_x_(fx), fixed_y_(fy), offset_(0), pos_x_(0), pos_y_(0), return_after_render_(false), selected_(0),
    size_x_(0), size_y_(0), title_x_(0), window_(nullptr) { }

// Adds an item to this Menu.
void Menu::add_item(const std::string &txt, int ch, Colour col , bool arrow)
{
    if (txt.size()) items_.push_back(txt); else items_.push_back("");
    item_chars_.push_back(ch);
    item_x_.push_back(0);
    colour_.push_back(col);
    arrows_.push_back(arrow);
}

// Another option to specify an arrow without all the other stuff.
void Menu::add_item(const std::string &txt, bool arrow) { add_item(txt, '\0', Colour::BLACK, arrow); }

// Checks how many items are in this Menu already.
unsigned int Menu::get_size() { return items_.size(); }

// Renders the menu, returns the chosen menu item (or -1 if none chosen)
int Menu::render()
{
    if (!items_.size()) return -1;
    reposition();
    auto terminal = core()->terminal();
    auto ui = core()->game()->ui();
    int key;    // The user's keyboard input.
    bool redraw = true;

    while(true)
    {
        if (redraw)
        {
            terminal->cls(window_);
            terminal->box(window_, Colour::WHITE);
            terminal->print(" " + title_ + " ", (window_->get_width() / 2) - (title_.size() / 2) - 1, 0, Colour::WHITE, PRINT_FLAG_REVERSE, window_);
            const unsigned int start = offset_;
            unsigned int end = items_.size();
            if (end - offset_ > 20) end = 20 + offset_;
            for (unsigned int i = start; i < end; i++)
            {
                const bool inverse = (selected_ == i && allow_highlight_);
                const bool item_char = (item_chars_.at(i) != '\0');
                terminal->print((item_char ? " " : "") + items_.at(i), item_x_.at(i) - (item_char ? 1 : 0), 2 + i - offset_, Colour::WHITE, inverse ?
                    PRINT_FLAG_REVERSE : 0, window_);
                if (item_chars_.at(i) != '\0')
                    terminal->put(item_chars_.at(i), item_x_.at(i) - 2, 2 + i - offset_, colour_.at(i), inverse ? PRINT_FLAG_REVERSE : 0, window_);
            }
            if (offset_ > 0) terminal->put('^', size_x_ - 1, 1, Colour::GREEN_BOLD, 0, window_);
            if (end < items_.size()) terminal->put('v', size_x_ - 1, size_y_ - 2, Colour::GREEN_BOLD, 0, window_);
            redraw = false;
        }

        ui->render(true);
        key = terminal->get_key();
        if (key == Key::CLOSE) exit(0);
        else if (key == Key::RESIZE)
        {
            redraw = true;
            ui->window_resized();
            reposition();
        }
        else if ((key == Key::ARROW_UP || key == Key::KP8 || key == 'k') && selected_ > 0)
        {
            selected_--;
            while (selected_ > 0 && (!items_.at(selected_).size() || colour_.at(selected_) == Colour::BLACK_BOLD)) selected_--;
            redraw = true;
        }
        else if ((key == Key::ARROW_DOWN || key == Key::KP2 || key == 'j') && selected_ < items_.size() - 1)
        {
            selected_++;
            while (selected_ < items_.size() - 1 && (!items_.at(selected_).size() || colour_.at(selected_) == Colour::BLACK_BOLD)) selected_++;
            if (selected_ > 0 && colour_.at(selected_) == Colour::BLACK_BOLD)
                while (selected_ > 0 && colour_.at(selected_) == Colour::BLACK_BOLD) selected_--;
            redraw = true;
        }
        else if (key == Key::ENTER || key == Key::CR)
        {
            ui->full_redraw();
            return selected_;
        }
        else if (key == ' ' || key == 'q' || key == 'Q' || key == Key::ESCAPE)
        {
            ui->full_redraw();
            return -1;
        }

        if (selected_ > offset_ + 19) offset_++;
        else if (selected_ < offset_) offset_--;

        if (return_after_render_)
        {
            ui->full_redraw();
            return selected_;
        }
    }
    return -1;
}

// Repositions the menu.
void Menu::reposition()
{
    if (!items_.size()) return;
    auto terminal = core()->terminal();
    unsigned int widest = 0;
    bool add_arrows = false;
    for (unsigned int i = 0; i < items_.size(); i++)
    {
        std::string item = items_.at(i);
        unsigned int len = StrX::strlen_colour(item);
        if (arrows_.at(i))
        {
            len += 2;
            if (items_.at(i).at(items_.at(i).size() - 1) != '>') add_arrows = true;
        }
        if (item_chars_.at(i) != '\0') len += 2;
        if (len > widest) widest = len;
    }

    // Now another loop to add the right-aligned > arrrows, if necessary.
    if (add_arrows)
    {
        for (unsigned int i = 0; i < items_.size(); i++)
        {
            if (!items_.at(i).size()) continue;
            int target_len = widest;
            if (arrows_.at(i)) target_len -= 2;
            if (item_chars_.at(i) != '\0') target_len -= 2;
            items_.at(i) = StrX::pad_string(items_.at(i), target_len);
            if (arrows_.at(i)) items_.at(i) += " >";
        }
    }

    if (title_.size() > widest) widest = title_.size();
    size_x_ = widest + 4;
    size_y_ = items_.size() + 4;
    if (size_y_ > 24) size_y_ = 24;
    window_ = std::make_shared<Window>(size_x_, size_y_);

    if (fixed_x_ < 0 || fixed_y_ < 0)
    {
        pos_x_ = terminal->get_midcol() - (size_x_ / 2);
        pos_y_ = terminal->get_midrow() - (size_y_ / 2);
    }
    else
    {
        pos_x_ = fixed_x_;
        pos_y_ = fixed_y_;
        if (pos_x_ + size_x_ >= terminal->get_cols()) pos_x_ = fixed_x_ - size_x_ - 1;
        if (pos_y_ + size_y_ >= terminal->get_rows()) pos_y_ = fixed_y_ - size_y_ - 1;
    }

    for (unsigned int i = 0; i < item_x_.size(); i++)
        item_x_.at(i) = (size_x_ / 2) - (StrX::strlen_colour(items_.at(i)) / 2) + (item_chars_.at(i) == '\0' ? 0 : 1);
    title_x_ = title_.size() / 2;

    window_->move(pos_x_, pos_y_);
}

// Sets the Menu option highlighting on or off.
void Menu::set_highlight(bool highlight) { allow_highlight_ = highlight; }

// Sets the currently-selected item.
void Menu::set_selected(unsigned int pos)
{
    selected_ = pos;
    while (selected_ > offset_ + 19) offset_++;
    while (selected_ < offset_) offset_--;
}

// Sets the Menu's title.
void Menu::set_title(const std::string &str) { title_ = str; }

}   // namespace invictus
