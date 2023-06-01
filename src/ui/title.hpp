// ui/title.hpp -- The title screen, also handles main-menu options such as loading saved games.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#ifndef UI_TITLE_HPP_
#define UI_TITLE_HPP_

namespace invictus
{

class TitleScreen
{
public:
            TitleScreen();  // Constructor.
    void    title_screen(); // Displays the title screen.

private:
    void    render();   // Renders the title screen.
    bool    save_file_exists() const;   // Check if a saved game exists.

    int     selected_;  // The menu option currently selected.
};

}       // namespace invictus
#endif  // UI_TITLE_HPP_
