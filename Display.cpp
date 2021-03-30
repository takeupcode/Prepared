#include "Display.h"

#include "ComponentDrawable.h"
#include "ComponentHealth.h"
#include "ComponentLocateable.h"
#include "ComponentRegistry.h"
#include "Game.h"

#include <algorithm>
#include <iomanip>

Display::Display (
    Game * game,
    unsigned int screenHeight)
: mGame(game), mScreenHeight(screenHeight),
  mMapScrollOrigin(0, 0)
{ }

Game * Display::game () const
{
    return mGame;
}

void Display::clear ()
{
    std::string emptyLines(mScreenHeight, '\n');
    mGame->output() << emptyLines;
}

void Display::update ()
{
    for (auto const & line: mMapOutputLines)
    {
        mGame->output() << line << std::endl;
    }

    mGame->output() << "-------------------------------" << std::endl;

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    mGame->output() << "Character ";
    for (auto const & character: mGame->characters())
    {
        mGame->output() << std::setw(5) << drawable->symbol(&character);
    }
    mGame->output() << std::endl;

    auto health = ComponentRegistry::find<ComponentHealth>();
    mGame->output() << "Health    ";
    for (auto const & character: mGame->characters())
    {
        mGame->output() << std::setw(5) << health->health(&character);
    }
    mGame->output() << std::endl;

    auto locateable = ComponentRegistry::find<ComponentLocateable>();
    mGame->output() << "Direction ";
    for (auto const & character: mGame->characters())
    {
        char symbol = directionSymbol(locateable->direction(&character));
        mGame->output() << std::setw(5) << symbol;
    }
    mGame->output() << std::endl;

    mGame->output() << "-------------------------------" << std::endl;

    for (auto const & line: mDialogOutputLines)
    {
        mGame->output() << line << std::endl;
    }

    mGame->output() << "-------------------------------" << std::endl;

    mMapOutputLines.clear();
    mDialogOutputLines.clear();
    mImportantLocations.clear();
}

void Display::beginStreamingToMap ()
{
    mMapBuffer.str("");
    mMapBuffer.clear();
}

void Display::endStreamingToMap ()
{
    std::string line;
    while (getline(mMapBuffer, line))
    {
        mMapOutputLines.push_back(line);
    }
}

void Display::setMapSymbol (
    char symbol,
    Point const & location,
    bool important)
{
    Point scrolledLoc = location;
    scrolledLoc.x -= mMapScrollOrigin.x;
    scrolledLoc.y -= mMapScrollOrigin.y;

    if (scrolledLoc.x < 0 || scrolledLoc.x >= MapDisplayWidth ||
        scrolledLoc.y < 0 || scrolledLoc.y >= MapDisplayHeight)
    {
        // Point coordinates are signed but they should never
        // be negative at this point. They're allowed to go
        // negative briefly so that we can test for valid
        // movement. If any point coordinate is outside of the
        // visible scrolled bounds, then we return without
        // updating the display.
        return;
    }

    if (std::find(mImportantLocations.begin(),
        mImportantLocations.end(),
        location) != mImportantLocations.end())
    {
        return;
    }

    while (mMapOutputLines.size()
        <= static_cast<unsigned int>(scrolledLoc.y))
    {
        mMapOutputLines.push_back("");
    }

    //  g g l <- tiles with leading spaces
    //  0 1 2 <- location x coordinate of tile
    // 012345 <- display position indices including empty spaces
    //
    // The tile location 0 needs to map to the display index 1,
    // the tile at location 1 needs to map to display index 3,
    // etc. We need to double the location coordinate and then
    // add one.
    int spacedX = scrolledLoc.x * 2 + 1;

    if (mMapOutputLines[scrolledLoc.y].size()
        <= static_cast<unsigned int>(spacedX))
    {
        unsigned int length = spacedX -
            mMapOutputLines[scrolledLoc.y].size() + 1;
        mMapOutputLines[scrolledLoc.y] += std::string(length, ' ');
    }

    mMapOutputLines[scrolledLoc.y][spacedX] = symbol;

    if (important)
    {
        mImportantLocations.push_back(location);
    }
}

Point Display::mapScrollOrigin ()
{
    return mMapScrollOrigin;
}

void Display::ensureVisibleInMap (
    Point const & location,
    int mapWidth,
    int mapHeight)
{
    // To scroll, we check each coordinate separately.
    // There's three possibilities for each coordinate.
    // We handle two of them and leave the middle condition
    // alone.
    if (location.y < mMapScrollOrigin.y + ScrollingMargin)
    {
        mMapScrollOrigin.y = location.y - ScrollingMargin;
    }
    else if (location.y >= mMapScrollOrigin.y + MapDisplayHeight - ScrollingMargin)
    {
        mMapScrollOrigin.y = location.y + ScrollingMargin - MapDisplayHeight + 1;
    }

    // Then check if this would put the display out of bounds.
    if (mMapScrollOrigin.y < 0)
    {
        mMapScrollOrigin.y = 0;
    }
    else if (mMapScrollOrigin.y + MapDisplayHeight > mapHeight)
    {
        mMapScrollOrigin.y = mapHeight - MapDisplayHeight;
    }

    // This is the same except with x and width instead of y and height.
    if (location.x < mMapScrollOrigin.x + ScrollingMargin)
    {
        mMapScrollOrigin.x = location.x - ScrollingMargin;
    }
    else if (location.x >= mMapScrollOrigin.x + MapDisplayWidth - ScrollingMargin)
    {
        mMapScrollOrigin.x = location.x + ScrollingMargin - MapDisplayWidth + 1;
    }

    // Then check if this would put the display out of bounds.
    if (mMapScrollOrigin.x < 0)
    {
        mMapScrollOrigin.x = 0;
    }
    else if (mMapScrollOrigin.x + MapDisplayWidth > mapWidth)
    {
        mMapScrollOrigin.x = mapWidth - MapDisplayWidth;
    }
}

void Display::beginStreamingToDialog ()
{
    mDialogBuffer.str("");
    mDialogBuffer.clear();
}

void Display::endStreamingToDialog ()
{
    std::string line;
    while (getline(mDialogBuffer, line))
    {
        mDialogOutputLines.push_back(line);
    }
}

std::ostream & Display::mapBuffer ()
{
    return mMapBuffer;
}

std::ostream & Display::dialogBuffer ()
{
    return mDialogBuffer;
}
