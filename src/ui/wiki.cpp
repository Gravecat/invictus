// ui/wiki.cpp -- Interactive documentation system, with links between pages.
// Copyright © 2017-2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/wiki.hpp"
#include "ui/wiki-pages.hpp"
#include "util/strx.hpp"


namespace invictus
{

unsigned int Wiki::buffer_pos_ = 0;                     // The position of the console buffer.
std::vector<std::pair<unsigned short, unsigned short>> Wiki::link_coords_;  // Coordinates of the wiki links on the page.
std::vector<bool> Wiki::link_good_;                     // Is this a valid link?
unsigned int Wiki::link_selected_ = 0;                  // The current active wiki link.
std::vector<std::string> Wiki::link_str_;               // Strings for the links.
std::vector<std::string> Wiki::wiki_history_;           // Previous wiki pages viewed.
std::vector<std::string> Wiki::wiki_prc_;               // The nicely processed wiki buffer, ready for rendering.
std::vector<std::string> Wiki::wiki_raw_;               // The raw, unprocessed wiki buffer.
std::shared_ptr<Window>  Wiki::wiki_window_ = nullptr;  // The wiki's render window.

// (re)creates the wiki render window.
void Wiki::create_wiki_window()
{
    auto terminal = core()->terminal();
    wiki_window_ = std::make_shared<Window>(terminal->get_cols(), terminal->get_rows(), 0, 0);
    terminal->box(wiki_window_);
}

// Processes input in the wiki window.
void Wiki::process_key(int key)
{
    auto game = core()->game();
    const unsigned int height = wiki_window_->get_height() - 2;

    if (key == Key::HOME)
    {
        buffer_pos_ = 0;
        render_wiki();
    }
    else if (key == Key::END)
    {
        reset_buffer_pos();
        render_wiki();
    }
    else if (game->is_key_north(key) || key == Key::PAGE_UP)
    {
        unsigned int magnitude = (key == Key::PAGE_UP ? 10 : 1);
        if (buffer_pos_ > magnitude) buffer_pos_ -= magnitude; else buffer_pos_ = 0;
        render_wiki();
    }
    else if (game->is_key_south(key) || key == Key::PAGE_DOWN)
    {
        unsigned int magnitude = (key == Key::PAGE_DOWN ? 10 : 1);
        buffer_pos_ += magnitude;
        if (buffer_pos_ >= wiki_prc_.size()) buffer_pos_ = wiki_prc_.size();
        if (wiki_prc_.size() - buffer_pos_ < height || wiki_prc_.size() < height) reset_buffer_pos();
        render_wiki();
    }
    else if ((game->is_key_west(key) && link_selected_ > 0) || (game->is_key_east(key) && link_selected_ < link_coords_.size() - 1))
    {
        if (game->is_key_east(key)) link_selected_++; else link_selected_--;
        if (link_coords_.size())
        {
            while (link_coords_.at(link_selected_).second < buffer_pos_) buffer_pos_--;
            while (link_coords_.at(link_selected_).second > buffer_pos_ + height - 1) buffer_pos_++;
        }
        render_wiki();
    }
    else if (key == Key::RESIZE)
    {
        create_wiki_window();
        process_wiki_buffer();
        buffer_pos_ = 0;
        render_wiki();
    }
    else if ((key == Key::ENTER) && link_str_.size())
    {
        if (link_good_.at(link_selected_))
        {
            wiki(StrX::str_toupper(link_str_.at(link_selected_)));
            render_wiki();
        }
    }
    else if (key == Key::ESCAPE || key == ' ')
    {
        wiki_history_.erase(wiki_history_.end() - 1);
        if (wiki_history_.size())
        {
            std::string page = wiki_history_.at(wiki_history_.size() - 1);
            wiki_history_.erase(wiki_history_.end() - 1);
            wiki(page);
        }
    }
}

// Processes the wiki buffer after an update or screen resize.
void Wiki::process_wiki_buffer()
{
    // Trim the wiki buffer if necessary.
    while (wiki_raw_.size() > WIKIBUF_MAX)
        wiki_raw_.erase(wiki_raw_.begin());

    // Clear the processed buffer.
    wiki_prc_.clear();
    if (!wiki_raw_.size()) return;

    // Process the console buffer.
    for (auto raw : wiki_raw_)
    {
        std::vector<std::string> line_vec = StrX::string_explode_colour(raw, wiki_window_->get_width() - 2);
        for (auto prc : line_vec)
        {
            wiki_prc_.push_back(prc);
        }
    }

    // Process the wiki links.
    link_coords_.clear();
    link_str_.clear();
    link_good_.clear();
    link_selected_ = 0;
    for (unsigned short i = 0; i < wiki_prc_.size(); i++)
    {
        size_t start_pos = 0;
        while(true)
        {
            // Locate opening wiki tags.
            const size_t pos = wiki_prc_.at(i).find("[", start_pos++);
            if (pos == std::string::npos) break;

            // See if there's any ANSI or glyphs affecting the line length.
            const unsigned int ansi_len = StrX::strlen_colour(wiki_prc_.at(i).substr(0, pos));
            const unsigned int len = wiki_prc_.at(i).substr(0, pos).size();
            const int diff = len - ansi_len;

            // Ensure the link is good, record it in the vectors if so.
            const std::pair<unsigned short, unsigned short> coord = { static_cast<uint16_t>(pos - diff), i };
            link_coords_.push_back(coord);
            const size_t pos2 = wiki_prc_.at(i).find("]", pos);
            if (pos2 == std::string::npos) { link_coords_.erase(link_coords_.end() - 1); break; }
            const std::string found = wiki_prc_.at(i).substr(pos + 1, pos2 - pos - 1);
            link_str_.push_back(found);
            start_pos = pos2;
            const std::string page_check = WikiPage::get_page(StrX::str_toupper(found), 1);
            if (page_check.size())
            {
                if (page_check[0] == '#')
                {
                    const std::string page_check_b = WikiPage::get_page(page_check.substr(1), 1);
                    if (page_check_b.size()) link_good_.push_back(true);
                    else link_good_.push_back(false);
                }
                else link_good_.push_back(true);
            }
            else link_good_.push_back(false);
        }
    }
}

// Redraws in the in-game wiki.
void Wiki::render_wiki()
{
    auto terminal = core()->terminal();
    terminal->cls(wiki_window_);
    const unsigned int height = wiki_window_->get_height() - 2;
    const unsigned int width = wiki_window_->get_width();
    terminal->box(wiki_window_);
    if (buffer_pos_ > 0) terminal->put('^', width - 1, 1, Colour::WHITE, PRINT_FLAG_REVERSE, wiki_window_);
    if (wiki_prc_.size() - buffer_pos_ > height) terminal->put('v', width - 1, height, Colour::WHITE, PRINT_FLAG_REVERSE, wiki_window_);
    if (wiki_prc_.size())
    {
        unsigned int end = wiki_prc_.size();
        if (end - buffer_pos_ > height) end = buffer_pos_ + height;
        for (unsigned int i = buffer_pos_; i < end; i++)
        {
            bool centered = (wiki_prc_.at(i).size() > 3 && wiki_prc_.at(i).substr(0, 3) == "{e}");
            terminal->print(wiki_prc_.at(i), centered ? (width / 2 - StrX::strlen_colour(wiki_prc_.at(i)) / 2) : 1, i - buffer_pos_ + 1,
                Colour::WHITE, 0, wiki_window_);
            for (unsigned int j = 0; j < link_coords_.size(); j++)
            {
                if (link_coords_.at(j).second == i)
                {
                    Colour link_col = Colour::BLUE_BOLD;
                    if (!link_good_.at(j)) link_col = Colour::RED_BOLD;
                    int link_print_flags = 0;
                    if (link_selected_ == j) link_print_flags = PRINT_FLAG_REVERSE;
                    terminal->put('[', link_coords_.at(j).first + 1, link_coords_.at(j).second - buffer_pos_ + 1, Colour::WHITE_BOLD, 0, wiki_window_);
                    terminal->put(']', link_coords_.at(j).first + 2 + link_str_.at(j).size(), link_coords_.at(j).second - buffer_pos_ + 1, Colour::WHITE_BOLD,
                        0, wiki_window_);
                    std::string link_string_processed = link_str_.at(j);
                    StrX::find_and_replace(link_string_processed, "_", " ");
                    terminal->print(link_string_processed, link_coords_.at(j).first + 2, link_coords_.at(j).second - buffer_pos_ + 1, link_col,
                        link_print_flags, wiki_window_);
                }
            }
        }
    }
    terminal->flip();
}

// Resets the wiki buffer position.
void Wiki::reset_buffer_pos()
{
    buffer_pos_ = 0;
    const unsigned int height = wiki_window_->get_height() - 2;
    if (wiki_prc_.size() > height) buffer_pos_ = wiki_prc_.size() - height;
}

// Displays a specific wiki window.
void Wiki::wiki(const std::string &page)
{
    if (!wiki_window_) create_wiki_window();
    wiki_raw_.clear();

    // Process link pages.
    std::string paragraph = WikiPage::get_page(page, 1);
    if (paragraph.size() && paragraph[0] == '#')
    {
        wiki(paragraph.substr(1));
        return;
    }

    wiki_history_.push_back(page);

    std::string line;

    for (int i = 1; i <= 2; i++)
    {
        line = "{e}" + WikiPage::get_page("WIKI_HEADER", i);
        wiki_raw_.push_back(line);
    }
    wiki_raw_.push_back("");

    int line_num = 0;
    do
    {
        line = WikiPage::get_page(page, ++line_num);
        if (line.size())
        {
            wiki_raw_.push_back(line);
            wiki_raw_.push_back(" ");
        }
    } while (line.size());
    wiki_raw_.erase(wiki_raw_.end() - 1);

    process_wiki_buffer();
    buffer_pos_ = 0;
    wiki_window_->set_visible(true);
    render_wiki();
    while(true)
    {
        process_key(core()->game()->get_key());
        if (!wiki_history_.size()) break;
        if (wiki_history_.at(wiki_history_.size() - 1) != page) break;
    }
    wiki_window_->set_visible(false);
}

}   // namespace invictus
