// ui/wiki-pages.cpp -- The in-game documentation, stored in a separate file from the wiki rendering code (wiki.cpp).
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "ui/wiki-pages.hpp"


namespace invictus
{

// The pages in this wiki.
std::map<std::string, std::string> WikiPage::wiki_data_ = {
    { "WIKI_HEADER1", "{g}.~{r}* {R}MORIOR INVICTUS: INTERACTIVE DOCUMENTATION {r}*{g}~." },
    { "WIKI_HEADER2", "{R}__________________________________________________" },

    { "EXAMPLE1", "This was just an example, to show you how links work. You can press the {C}Space Bar {w}to return to your previously-viewed page." },

    { "INDEX1", "This index contains all the pages available on the wiki. Please choose a letter below to view the pages (letters marked as red currently \
contain no pages):" },
    { "INDEX2", "[A] [B] [C] [D] [E] [F] [G] [H] [I] [J] [K] [L] [M] [N] [O] [P] [Q] [R] [S] [T] [U] [V] [W] [X] [Y] [Z]" },

    { "G1", "The following documentation pages start with the letter G: [GitHub]" },

    { "GITHUB1", "The official GitHub page for Morior Invictus is located here: {U}https://github.com/Gravecat/invictus" },
    { "GITHUB2", "You can find the latest builds and downloads there, as well as more extneisve documentation in web format, discussions, bug reports, and \
more. If you notice anything broken with the game, please head to the GitHub page and open an issue - the game's developer will deal with it as soon as \
possible."},
    { "GITHUB3", "If you are unable or unwilling to use GitHub, you can contact the game's developer via email, at {U}gc@gravecat.com"},

    { "I1", "The following documentation pages start with the letter I: [Index]" },

    { "LINKS1", "#EXAMPLE" },

    { "THIS_ONE1", "#EXAMPLE" },

    { "WIKI1", "Welcome to the {W}interactive documentation {w}for Morior Invictus. You can scroll up and down through a page with the {C}up and down arrow \
keys{w}, the {C}Page Up and Page Down keys{w}, or {C}vi keys (j and k){w}." },
    { "WIKI2", "Throughout this interactive documentation, you will see [links] like [this_one]. You can select a link with the {C}left and right arrow \
keys{w} or {C}vi keys (h and l){w}, and visit that link to see a new documentation page by pressing {C}Enter{w}." },
    { "WIKI3", "Sometimes, a link might show up as red, like [this_link]. This indicates that the documentation page is missing, and has likely just not been \
written yet. If you think this is in error, please head to the [GitHub] page and post a bug report."},
    { "WIKI4", "At any time, you can press the {C}Space Bar {w}to move back to the previous page you came from, or if you're on the front page, the {C}Space \
Bar {w}will close the documentation." },
    { "WIKI5", "You can see an alphabetical [Index] of all available wiki pages." },

/*
    You can use the left and right arrow keys, numeric keypad, or vi keys \
(h and l) to select a link, the up and down arrow keys, vi keys (j and k) or page-up and page-down keys to scroll through a page." },
*/
};

// Gets a given paragraph from a page in the wiki.
std::string WikiPage::get_page(const std::string &page_name, unsigned int paragraph_number)
{
    auto result = wiki_data_.find(page_name + std::to_string(paragraph_number));
    if (result == wiki_data_.end()) return "";
    else return result->second;
}

}   // namespace invictus
