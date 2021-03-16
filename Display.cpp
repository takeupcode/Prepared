#include "Display.h"

#include "Game.h"

#include <algorithm>
#include <iomanip>

Display::Display (
    Game * game,
    unsigned int screenHeight)
: mGame(game), mScreenHeight(screenHeight)
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

    mGame->output() << "----------------------------" << std::endl;

    mGame->output() << "Character ";
    for (auto const & character: mGame->characters())
    {
        mGame->output() << std::setw(5) << character.symbol();
    }
    mGame->output() << std::endl;
    mGame->output() << "Health    ";
    for (auto const & character: mGame->characters())
    {
        mGame->output() << std::setw(5) << character.currentHealth();
    }
    mGame->output() << std::endl;
    mGame->output() << "Direction ";
    for (auto const & character: mGame->characters())
    {
        char symbol = directionSymbol(character.direction());
        mGame->output() << std::setw(5) << symbol;
    }
    mGame->output() << std::endl;

    mGame->output() << "----------------------------" << std::endl;

    for (auto const & line: mDialogOutputLines)
    {
        mGame->output() << line << std::endl;
    }

    mGame->output() << "----------------------------" << std::endl;

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
    Location const & location,
    bool important)
{
    if (location.x < 0 || location.y < 0)
    {
        // Coordinates are signed but they should never
        // be negative at this point. They're allowed to go
        // negative briefly so that we can test for valid
        // movement.
        return;
    }

    if (std::find(mImportantLocations.begin(),
        mImportantLocations.end(),
        location) != mImportantLocations.end())
    {
        return;
    }

    while (mMapOutputLines.size()
        <= static_cast<unsigned int>(location.y))
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
    int spacedX = location.x * 2 + 1;

    if (mMapOutputLines[location.y].size()
        <= static_cast<unsigned int>(spacedX))
    {
        unsigned int length = spacedX -
            mMapOutputLines[location.y].size() + 1;
        mMapOutputLines[location.y] += std::string(length, ' ');
    }

    mMapOutputLines[location.y][spacedX] = symbol;

    if (important)
    {
        mImportantLocations.push_back(location);
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
