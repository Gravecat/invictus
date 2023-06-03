// ui/wiki.cpp -- Interactive documentation system, with links between pages.
// Copyright © 2017-2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/game-manager.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "ui/wiki.hpp"
#include "util/strx.hpp"


namespace invictus
{

// The pages in this wiki.
std::map<std::string, std::string> Wiki::wiki_data_ = {
    { "EXAMPLE1", "This was just an example, to show you how links work. You can press the {C}Space Bar {w}to return to your previously-viewed page." },

    { "G1", "The following documentation pages start with the letter G: [Getting_Started] [GitHub]" },

    { "GETTING_STARTED1", "{W}Morior Invictus {w}is a {C}traditional roguelike game {w}- this means that it has less in common with more mainstream games \
bearing the 'roguelike' genre, and is much more like the more oldschool 'classic' roguelike games such as {W}Angband{w}, {W}Nethack{w}, {W}ADOM{w}, \
{W}Crawl{w}, {W}Omega{w}, {W}Larn {w}or {W}ToME{w}." },
    { "GETTING_STARTED2", "This game is {W}ASCII{w}-based, which means everything in the game world is represented by a grid of letters on the screen, with \
each letter representing something in the game world -- for example, the letter {C}z {w}might represent a zombie, or a {C}D {w}might be a fearsome dragon. \
As is tradition for classic roguelikes, the player character - your avatar in the game world - is represented by the {C}@ {w}symbol." },
    { "GETTING_STARTED3", "The game is controlled entirely with the {W}keyboard{w}. Various keys perform different actions, or move you around the game world, \
in a turn-based loop - the game will always wait for you to make a move before reacting. You can see a list of all the controls on the [Controls] page." },
    { "GETTING_STARTED4", "Unlike many other roguelike games, there is no 'levelling up' and no skills system. Your character is a capable adventurer from the \
get go, but in order to get stronger, you'll need to find more powerful and useful equipment and items, and learn the most effective way to use them." },
    { "GETTING_STARTED5", "{R}The world of Morior Invictus is not a fair one! {w}Most everything is out to get you in one form or another, and it's up to you \
to rely on your wits, knowledge and luck to survive. With that said, you'll likely die early and often - and that's okay! The fun of a roguelike is \
practicing, learning, and getting better with each new attempt."},
    { "GETTING_STARTED6", "When you begin the game, you will find yourself in the [Dungeon_View]. This shows an overview of your character - the {W}@ \
{w}symbol - and everything surrounding you from a top-down, tile-based view. Your goal in this version of the game is to retrieve the fabled relic, {C}The \
Crown of Kings{w}, which has been lost deep in the zombie-infested {C}Tomb of Forgotten Kings{w}." },
    { "GETTING_STARTED7", "The crown is hidden deep within the tomb, but if you are able to find it and return to the surface world with the crown, you will \
be celebrated as the true hero you were always destined to be!" },

    { "GITHUB1", "The official GitHub page for Morior Invictus is located here: {U}https://github.com/Gravecat/invictus" },
    { "GITHUB2", "You can find the latest builds and downloads there, as well as more extneisve documentation in web format, discussions, bug reports, and \
more. If you notice anything broken with the game, please head to the GitHub page and open an issue - the game's developer will deal with it as soon as \
possible."},
    { "GITHUB3", "If you are unable or unwilling to use GitHub, you can contact the game's developer via email, at {U}gc@gravecat.com"},

    { "I1", "The following documentation pages start with the letter I: [Index]" },

    { "INDEX1", "This index contains all the pages available on the in-game documentation. Please choose a letter below to view the pages (letters marked as \
red currently contain no pages):" },
    { "INDEX2", "[A] [B] [C] [D] [E] [F] [G] [H] [I] [J] [K] [L] [M] [N] [O] [P] [Q] [R] [S] [T] [U] [V] [W] [X] [Y] [Z]" },

    { "LINKS1", "#EXAMPLE" },

    { "THIS_ONE1", "#EXAMPLE" },

    { "WIKI1", "{R}PLEASE NOTE: {Y}This game is currently in VERY early stages of development, and the gameplay is currently limited. There are many, many \
more features planned for future releases!" },
    { "WIKI2", "Welcome to the {W}interactive documentation {w}for Morior Invictus. You can scroll up and down through a page with the {C}up and down arrow \
keys{w}, the {C}Page Up and Page Down keys{w}, or {C}vi keys (j and k){w}." },
    { "WIKI3", "Throughout this interactive documentation, you will see [links] like [this_one]. You can select a link with the {C}left and right arrow \
keys{w} or {C}vi keys (h and l){w}, and visit that link to see a new documentation page by pressing {C}Enter{w}." },
    { "WIKI4", "Sometimes, a link might show up as red, like [this_link]. This indicates that the documentation page is missing, and has likely just not been \
written yet. If you think this is in error, please head to the [GitHub] page and post a bug report."},
    { "WIKI5", "At any time, you can press the {C}Space Bar {w}to move back to the previous page you came from, or if you're on the front page, the {C}Space \
Bar {w}will close the documentation." },
    { "WIKI6", "If you are new to this game, I recommend reading the [Getting_Started], [Controls] and [Dungeon_View] pages. You can also see an alphabetical \
[Index] of all available documentation pages." },

    { "WIKI_HEADER1", "{g}.~{r}* {R}MORIOR INVICTUS: INTERACTIVE DOCUMENTATION {r}*{g}~." },
    { "WIKI_HEADER2", "{R}__________________________________________________" },
};

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

// Gets a given paragraph from a page in the wiki.
std::string Wiki::get_page(const std::string &page_name, unsigned int paragraph_number)
{
    auto result = wiki_data_.find(page_name + std::to_string(paragraph_number));
    if (result == wiki_data_.end()) return "";
    else return result->second;
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
    for (unsigned int i = 0; i < wiki_prc_.size(); i++)
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
            const std::string page_check = get_page(StrX::str_toupper(found), 1);
            if (page_check.size())
            {
                if (page_check[0] == '#')
                {
                    const std::string page_check_b = get_page(page_check.substr(1), 1);
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
    std::string paragraph = get_page(page, 1);
    if (paragraph.size() && paragraph[0] == '#')
    {
        wiki(paragraph.substr(1));
        return;
    }

    wiki_history_.push_back(page);

    std::string line;

    for (int i = 1; i <= 2; i++)
    {
        line = "{e}" + get_page("WIKI_HEADER", i);
        wiki_raw_.push_back(line);
    }
    wiki_raw_.push_back("");

    int line_num = 0;
    do
    {
        line = get_page(page, ++line_num);
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
