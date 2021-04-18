#include "Display.h"

#include "ASCII.h"
#include "ComponentDrawable.h"
#include "ComponentHealth.h"
#include "ComponentLocation.h"
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
    auto reset = ASCIIEscape::graphicSequence({ASCIIGraphic::ResetAll});
    for (auto const & line: mMapOutput)
    {
        for (auto const & symbol: line)
        {
            mGame->output() << symbol << reset;
        }
        mGame->output() << std::endl;
    }

    mGame->output()
        << "-------------------------------"
        << std::endl;

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    mGame->output() << "Character ";
    for (auto const & item: mGame->items())
    {
        if (item.hasTag("pc"))
        {
            mGame->output() << std::setw(5) << drawable->symbol(&item);
        }
    }
    mGame->output() << std::endl;

    auto health = ComponentRegistry::find<ComponentHealth>();
    mGame->output() << "Health    ";
    for (auto const & item: mGame->items())
    {
        if (item.hasTag("pc"))
        {
            mGame->output() << std::setw(5) << health->health(&item);
        }
    }
    mGame->output() << std::endl;

    auto location = ComponentRegistry::find<ComponentLocation>();
    mGame->output() << "Direction ";
    for (auto const & item: mGame->items())
    {
        if (item.hasTag("pc"))
        {
            char symbol = directionSymbol(location->direction(&item));
            mGame->output() << std::setw(5) << symbol;
        }
    }
    mGame->output() << std::endl;

    mGame->output() << "-------------------------------" << std::endl;

    for (auto const & line: mDialogOutputLines)
    {
        mGame->output() << line << std::endl;
    }

    mGame->output() << "-------------------------------" << std::endl;

    mDialogOutputLines.clear();
    mImportantLocations.clear();
}

void Display::setMapSymbol (
    std::string const & symbol,
    Point2i const & location,
    bool important)
{
    Point2i scrolledLoc = location;
    scrolledLoc.x -= mMapScrollOrigin.x;
    scrolledLoc.y -= mMapScrollOrigin.y;

    if (scrolledLoc.x < 0 || scrolledLoc.x >= MapDisplayWidth ||
        scrolledLoc.y < 0 || scrolledLoc.y >= MapDisplayHeight)
    {
        // Point2i coordinates are signed but they should never
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

    while (mMapOutput.size()
        <= static_cast<unsigned int>(scrolledLoc.y))
    {
        mMapOutput.push_back(std::vector<std::string>());
    }

    while (mMapOutput[scrolledLoc.y].size()
        <= static_cast<unsigned int>(scrolledLoc.x))
    {
        mMapOutput[scrolledLoc.y].push_back("");
    }

    mMapOutput[scrolledLoc.y][scrolledLoc.x] = symbol;

    if (important)
    {
        mImportantLocations.push_back(location);
    }
}

Point2i Display::mapScrollOrigin ()
{
    return mMapScrollOrigin;
}

void Display::ensureVisibleInMap (
    Point2i const & location,
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

std::ostream & Display::dialogBuffer ()
{
    return mDialogBuffer;
}
