// ui/ui.cpp -- The UI overlay that displays additional information on top of the game's map, using other UI classes.
// Copyright © 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include "core/core.hpp"
#include "core/guru.hpp"
#include "terminal/terminal.hpp"
#include "ui/ui.hpp"


namespace invictus
{

// Constructor, sets up UI elements.
UI::UI() : cleanup_done_(false)
{ core()->guru()->log("User interface manager ready!"); }

// Destructor, calls cleanup function.
UI::~UI() { cleanup(); }

// Cleans up any sub-elements.
void UI::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    core()->guru()->log("Cleaning up user interface.");
}

// Renders the UI elements, if needed.
void UI::render() { }

// Returns the visible area not covered by the UI.
void UI::visible_area(int *x, int *y)
{
    auto terminal = core()->terminal();
    *x = terminal->get_cols() - NEARBY_BAR_WIDTH;
    *y = terminal->get_rows() - MESSAGE_LOG_HEIGHT - 2;
}

}   // namespace invictus
