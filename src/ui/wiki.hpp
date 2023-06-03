// ui/wiki.hpp -- Interactive documentation system, with links between pages.
// Copyright © 2017-2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_WIKI_HPP_
#define UI_WIKI_HPP_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>


namespace invictus
{

class Window;   // defined in terminal/window.hpp


class Wiki
{
public:
    static void wiki(const std::string &page);  // Displays a specific wiki window.

private:
    static void create_wiki_window();   // (re)creates the wiki render window.
    static std::vector<std::string> get_page(const std::string &page_name); // Gets a given page from the wiki.
    static void process_key(int key);   // Processes input in the wiki window.
    static void process_wiki_buffer();  // Processes the wiki buffer after an update or screen resize.
    static void render_wiki();          // Redraws in the in-game wiki.
    static void reset_buffer_pos();     // Resets the wiki buffer position.

    static constexpr int    WIKIBUF_MAX =   256;    // Maximum size of the wiki buffer.

    static unsigned int             buffer_pos_;    // The position of the console buffer.
    static std::vector<std::pair<unsigned short, unsigned short>>  link_coords_;   // Coordinates of the wiki links on the page.
    static std::vector<bool>        link_good_;     // Is this a valid link?
    static unsigned int             link_selected_; // The current active wiki link.
    static std::vector<std::string> link_str_;      // Strings for the links.
    static std::map<std::string, std::vector<std::string>>  wiki_data_; // The pages in this wiki.
    static std::vector<std::string> wiki_history_;  // Previous wiki pages viewed.
    static std::vector<std::string> wiki_prc_;      // The nicely processed wiki buffer, ready for rendering.
    static std::vector<std::string> wiki_raw_;      // The raw, unprocessed wiki buffer.
    static std::shared_ptr<Window>  wiki_window_;   // The wiki's render window.
};

}       // namespace invictus
#endif  // UI_WIKI_HPP_
