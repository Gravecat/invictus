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
std::map<std::string, std::vector<std::string>> Wiki::wiki_data_ = {
    { "C", { "The following documentation pages start with the letter C: [Controls]" } },

    { "CONTROLS",
        { "The game is controlled entirely with the keyboard. You can naviate menus and [move] around the game world using your choice of {C}arrow keys{w}, "
        "the {C}numeric keypad {w}(if you have one), or {C}vi keys{w}. Navigation in the game world is done via {C}cardinal directions {w}- north, east, south,"
        " west, and the diagonals.",

        "The following grids of keys shows any of the valid controls used to move in the four compass directions, or the four diagonals (northeast, northwest, "
        "southeast, southwest). The {C}arrow keys {w}are represented here by {C}^ v < > {w}characters, and are not these literal keys.",

        "",
        "{0}{e}{Y}Cardinal     {R}\"vi\"     {G}Numeric    {U}Arrow Keys  ",
        "{e}{Y}Directions   {R}keys     {G}Keypad     {U}+ Navigation",
        "{0}{e}{Y}NW N NE      {R}y k u     {G}7 8 9     {U}Home ^ PgUp ",
        "{0}{e}  {Y}\\|/         \\|/       \\|/          \\|/     ",
        "{0}{e} {Y}W-+-E       {R}h{Y}-+-{R}l     {G}4{Y}-+-{G}6        {U}<{Y}-+-{U}>    ",
        "{0}{e}  {Y}/|\\         /|\\       /|\\          /|\\     ",
        "{e}{Y}SW S SE      {R}b j n     {G}1 2 3      {U}End v PgDn ",

        "{R}Please note: {w}Some combinations of these keys may not work on certain terminals, particularly if you are playing over an {W}ssh {w}connection. "
        "This is largely a limitation of Curses.",

        "The four primary directional keys are also used to navigate menus. The {C}Enter {w}key selects items in menus, and the {C}Space Bar {w}cancels "
        "actions or closes menus.",

        "During your adventure, you will find yourself needing to travel vertically - descending into deep [dungeons], or ascending to higher levels or the "
        "surface world above. For this, you can use the {C}< {w}and {C}> {w}keys (not the arrow keys - the actual angle-bracket keys) to travel upward and "
        "downward, when in a suitable place to do so.",

        "The other keys used in the game are listed below. {Y}Please remember, you can always use the {C}? {Y}key during gameplay to open this in-game "
        "documentation again.",

        "{0}{C}, {w}or {C}numpad 5 {w}- do nothing for one turn, causing a little [time] to pass.",
        "{0}{C}. {w}- interact with items on the ground.",
        "{0}{C}= {w}- opens the main menu.",
        "{0}{C}c {w}- close a nearby door.",
        "{0}{C}e {w}- check your equipped gear.",
        "{0}{C}g {w}- pick up items on the ground.",
        "{0}{C}i {w}- check your carried items.",
        "{0}{C}o {w}- open a nearby door.",
        "{0}{C}Shift-R {w}- rest for a while, causing a longer period of [time] to pass.",
        "{C}Shift-S {w}- save the game.",
        } },

    { "D", { "The following documentation pages start with the letter D: [Debug_Options] [Dungeon] [Dungeon_View]" } },

    { "DEBUG_OPTIONS", {
        "Some hidden debug options are available if launching the game with certain special parameters. They are as follows:",

        "{C}-acs-display {w}- Renders the Curses ACS glyphs, including the line-drawing characters and others. These may or may not be supported on your "
        "system, and are grouped into sets which can be disabled entirely via special flags in [prefs.txt].",

        "{C}-keycode-check {w}- Displays either the keycodes or escape sequences returned from Curses for any keys that are pressed. This can be useful for "
        "debugging or adding escape codes from a terminal not yet supported by the game."
    } },

    { "DUNGEON_VIEW",
        { "The [dungeon] view is the main mode in which you'll be playing the game. You'll see that it's divided into three distinct sections:",

        "The top-left section is the {W}dungeon view{w}. This displays a top-down view of your surroundings, in the form of {C}ASCII symbols {w}on a grid. The "
        "large white {W}@ {w}represents your character in the game world. You can only see what your character is capable of seeing, though - see the "
        "[line_of_sight] and [light] pages for more on this.",

        "To the right of this view is the overview window, which will show you what the symbols on the screen all represent, as well as listing any nearby "
        "hostile foes or items you can see on the ground. The coloured bar with an [enemy's] name is {R}red {w}if that enemy is 'awake' and aware of your "
        "presence, or {U}blue {w}if they are still unaware of you.",

        "Just below the dungeon view are some coloured bars - these represent your {R}Hit Points {w}(your general health and life-force), {G}Stamina Points "
        "{w}(used to perform special attacks and physical feats), and your {U}Mana Points {w}(used to cast spells and use magical items).",

        "You may also see a gray number just above these bars - this represents your current depth below ground, in feet. To the left of this bar, you will be "
        "notified of any temporary status conditions (known as buffs and debuffs).",

        "Finally, the bottom box is the {W}message log{w}, which displays important messages about what's happening in the game world."
    } },

    { "DUNGEON",
        { "The world is filled with many dangerous places, which are colloquially known as \"dungeons\". Dungeons might take the form of caves, ancient tombs, "
        "lairs of evil wizards, or even more deadly locales.",

        "A dungeon usually has a number of vertical levels, which can be traversed via staircases. The entrances to many dungeons are located at the "
        "overworld, but you may even find dungeon entrances hidden within other dungeons!"
    } },

    { "DUNGEONS", { "#DUNGEON" } },

    { "E", { "The following documentation pages start with the letter E: [Enemy]" } },

    { "ENEMY",
        { "The world of Morior Invictus is a hostile one, and it is filled with both people and creatues that wish to do you harm. Some enemies are weaker or "
        "stronger than others, and it's up to you to figure out how to deal with the foes you face.",

        "When an enemy is within sight, you will see their name and health bar listed on the [overview_window]. Many foes will seek to do you harm, but you "
        "can attack in return and try to slay them first."
    } },

    { "ENEMY'S", { "#ENEMY" } },

    { "EXAMPLE", { "This was just an example, to show you how links work. You can press the {C}Space Bar {w}to return to your previously-viewed page." } },

    { "G", { "The following documentation pages start with the letter G: [Getting_Started] [GitHub]" } },

    { "GETTING_STARTED",
        { "{W}Morior Invictus {w}is a {C}traditional roguelike game {w}- this means that it has less in common with more mainstream games bearing the "
        "'roguelike' genre, and is much more like the more oldschool 'classic' roguelike games such as {W}Angband{w}, {W}Nethack{w}, {W}ADOM{w}, {W}Crawl{w}, "
        "{W}Omega{w}, {W}Larn {w}or {W}ToME{w}.",

        "This game is {W}ASCII{w}-based, which means everything in the game world is represented by a grid of letters on the screen, with each letter "
        "representing something in the game world -- for example, the letter {C}z {w}might represent a zombie, or a {C}D {w}might be a fearsome dragon. As is "
        "tradition for classic roguelikes, the player character - your avatar in the game world - is represented by the {C}@ {w}symbol.",

        "The game is controlled entirely with the {W}keyboard{w}. Various keys perform different actions, or [move] you around the game world, in a turn-based "
        "loop - the game will always wait for you to make a move before reacting. You can see a list of all the controls on the [Controls] page.",

        "Unlike many other roguelike games, there is no 'levelling up' and no skills system. Your character is a capable adventurer from the get go, but in "
        "order to get stronger, you'll need to find more powerful and useful equipment and items, and learn the most effective way to use them.",

        "{R}The world of Morior Invictus is not a fair one! {w}Most everything is out to get you in one form or another, and it's up to you to rely on your "
        "wits, knowledge and luck to survive. With that said, you'll likely die early and often - and that's okay! The fun of a roguelike is practicing, "
        "learning,  and getting better with each new attempt.",

        "When you begin the game, you will find yourself in the [dungeon_view]. This shows an overview of your character - the {W}@ {w}symbol - and everything "
        "surrounding you from a top-down, tile-based view. Your goal in this version of the game is to retrieve the fabled relic, {C}The Crown of Kings{w}, "
        "which has been lost deep in the zombie-infested {C}Tomb of Forgotten Kings{w}.",

        "The crown is hidden deep within the tomb, but if you are able to find it and return to the surface world with the crown, you will be celebrated as "
        "the true hero you were always destined to be!" } },

    { "GITHUB",
        { "The official GitHub page for Morior Invictus is located here: {U}https://github.com/Gravecat/invictus",

        "You can find the latest builds and downloads there, as well as discussions, bug reports, and more. If you notice anything broken with the game, "
        "please head to the GitHub page and open an issue - the game's developer will deal with it as soon as possible.",

        "If you are unable or unwilling to use GitHub, you can contact the game's developer via email, at {U}gc@gravecat.com" } },

    { "I", { "The following documentation pages start with the letter I: [Index]" } },

    { "INDEX",
        { "This index contains all the pages available on the in-game documentation. Please choose a letter below to view the pages (letters marked as red "
        "currently contain no pages):",

        "[A] [B] [C] [D] [E] [F] [G] [H] [I] [J] [K] [L] [M] [N] [O] [P] [Q] [R] [S] [T] [U] [V] [W] [X] [Y] [Z]" } },

    { "L", { "The following documentation pages start with the letter L: [Line_of_Sight]" } },

    { "LINE_OF_SIGHT",
        { "You are not omnisicent, and your view of the [dungeon] (or other area where you are exploring) is limited by your line-of-sight. Areas that are "
        "unexplored will be simply shown as empty black space, and areas you have previously visited but cannot currently see will be shown in the "
        "[dungeon_view] as {u}dark blue{w}."
    } },

    { "LINKS", { "#EXAMPLE" } },

    { "M", { "The following documentation pages start with the letter M: [Monster] [Movement]" } },

    { "MONSTER", { "#ENEMY" } },

    { "MOVE", { "#MOVEMENT" } },

    { "MOVEMENT",
        { "All movement in the game is done across a two-dimensional grid, in one of eight directions - the four cardinal compass directions, and the four "
        "diagonals. For the movement controls, see the [controls] page.",

        "{0}{e}{Y}Cardinal  ",
        "{e}{Y}Directions",
        "{0}{e} {Y}NW N NE   ",
        "{0}{e}   {Y}\\|/     ",
        "{0}{e}  {Y}W-+-E    ",
        "{0}{e}   {Y}/|\\     ",
        "{e} {Y}SW S SE   "
        } },

    { "O", { "The following documentation pages start with the letter O: [Overview_Window]" } },

    { "OVERVIEW_WINDOW", { "#DUNGEON_VIEW" } },

    { "P", { "The following documentation pages start with the letter P: [prefs.txt]" } },

    { "PREFS.TXT", { 
        "When the game has been run at least once, it will create a {C}userdata {w}folder, with a {C}prefs.txt {w}file located within. This file can be used "
        "to tweak some settings in the game. They are as follows:",

#ifdef INVICTUS_TARGET_LINUX
        "{C}acs_flags {w}(default: 11)"
#else
        "{C}acs_flags {w}(default: 15)"        
#endif
        " - Which Curses ACS glyphs to use (see [Debug_Options]). Unused glyphs are replaced with the closest similar ASCII symbols.",

        "{C}pathfind_euclidean {w}(default: true) - If set to true, pathfinding will use the Euclidean method, which is more computationally expensive but "
        "more accurate. If set to false, it will use the faster, less-accurate Manhattan method.",

        "{C}use_colour {w}(default: true) - If set to false, will disable the use of most ANSI colour codes (invert and bold effects are still used)."
    } },

    { "T", { "The following documentation pages start with the letter T: [Time]" } },

    { "THIS_ONE", { "#EXAMPLE" } },

    { "TIME",
        { "Most every action you take in-game will take a certain amount of time, usually in a predictable mannner - donning a suit of armour takes longer "
        "than opening a door, for example.",

        "While the game is turn-based and will always wait for the player to make a move, the actions taken by the player determine how much time passes "
        "in-game before the player's next turn."
    } },

    { "WIKI", {
        "{R}PLEASE NOTE: {Y}This game is currently in VERY early stages of development, and the gameplay is currently limited. There are many, many more "
        "features planned for future releases!",

        "Welcome to the {W}interactive documentation {w}for Morior Invictus. You can scroll up and down through a page with the {C}up and down arrow keys{w}, "
        "the {C}Page Up and Page Down keys{w}, or {C}vi keys (j and k){w}.",

        "Throughout this interactive documentation, you will see [links] like [this_one]. You can select a link with the {C}left and right arrow keys{w} or "
        "{C}vi keys (h and l){w}, and visit that link to see a new documentation page by pressing {C}Enter{w}.",

        "Sometimes, a link might show up as red, like [this_link]. This indicates that the documentation page is missing, and has likely just not been "
        "written yet. If you think this is in error, please head to the [GitHub] page and post a bug report.",

        "At any time, you can press the {C}Space Bar {w}to move back to the previous page you came from, or if you're on the front page, the {C}Space Bar "
        "{w}will close the documentation.",

        "If you are new to this game, I recommend reading the [Getting_Started], [Controls] and [Dungeon_View] pages. You can also see an alphabetical "
        "[Index] of all available documentation pages." } },

    { "WIKI_HEADER", {
        "{g}.~{r}* {R}MORIOR INVICTUS: INTERACTIVE DOCUMENTATION {r}*{g}~.",
        "{R}__________________________________________________" } },
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

// Gets a given page from the wiki.
std::vector<std::string> Wiki::get_page(const std::string &page_name)
{
    auto result = wiki_data_.find(page_name);
    if (result == wiki_data_.end())
    {
        std::vector<std::string> vec;
        return vec;
    }
    return result->second;
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
            const std::vector<std::string> page_check = get_page(StrX::str_toupper(found));
            if (page_check.size() && page_check.at(0).size())
            {
                if (page_check.at(0)[0] == '#')
                {
                    const std::vector<std::string> page_check_b = get_page(page_check.at(0).substr(1));
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

void Wiki::wiki()
{
    if (!wiki_window_) create_wiki_window();
    wiki("WIKI");
    wiki_window_ = nullptr;
    core()->terminal()->flip();
}

// Displays a specific wiki window.
void Wiki::wiki(const std::string &page)
{
    wiki_raw_.clear();

    // Process link pages.
    std::vector<std::string> wpage = get_page(page);
    if (wpage.size() && wpage.at(0).size() && wpage.at(0)[0] == '#')
    {
        wiki(wpage.at(0).substr(1));
        return;
    }

    wiki_history_.push_back(page);

    std::string line;

    std::vector<std::string> header = get_page("WIKI_HEADER");

    for (auto line : header)
        wiki_raw_.push_back("{e}" + line);
    wiki_raw_.push_back(" ");

    std::vector<std::string> page_data = get_page(page);
    for (auto line : page_data)
    {
        if (line.size())
        {
            bool zero_tag = (line.size() > 3 && line.substr(0, 3) == "{0}");
            wiki_raw_.push_back(line.substr(zero_tag ? 3 : 0));
            if (!zero_tag) wiki_raw_.push_back(" ");
        }
    }
    wiki_raw_.erase(wiki_raw_.end() - 1);

    process_wiki_buffer();
    buffer_pos_ = 0;
    render_wiki();
    while(true)
    {
        process_key(core()->game()->get_key());
        if (!wiki_history_.size()) break;
        if (wiki_history_.at(wiki_history_.size() - 1) != page) break;
    }
}

}   // namespace invictus
