// ui/wiki-pages.hpp -- The in-game documentation, stored in a separate file from the wiki rendering code (wiki.cpp).
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_WIKI_PAGES_HPP_
#define UI_WIKI_PAGES_HPP_

#include <map>
#include <string>


namespace invictus
{

class WikiPage
{
public:
    static std::string  get_page(const std::string &page_name, unsigned int paragraph_number);  // Gets a given paragraph from a page in the wiki.

private:
    static std::map<std::string, std::string>   wiki_data_; // The pages in this wiki.
};

}       // namespace invictus
#endif  // UI_WIKI_PAGES_HPP_
