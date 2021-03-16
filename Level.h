#ifndef LEVEL_H
#define LEVEL_H

#include "Character.h"
#include "Location.h"
#include "Tile.h"

#include <vector>

class Game;

class Level
{
public:
    Level (Game * game);

    void generate ();

    void draw () const;

    std::vector<Location> entryLocations (
        unsigned int count) const;

    std::vector<Character> spawnCreatures () const;

    Location calculateMoveLocation (
        Location const & current,
        Location const & proposed) const;

    Tile * findTile (Location const & location);
    Tile const * findTile (Location const & location) const;

private:
    Game * mGame;
    unsigned int mWidth;
    unsigned int mHeight;
    std::vector<Tile> mTiles;
    std::vector<Location> mEntryLocations;
};

#endif // LEVEL_H
