#include "Level.h"

#include "Display.h"
#include "Game.h"

unsigned int const width = 8;
unsigned int const height = 8;

Level::Level (Game * game)
: mGame(game), mWidth(0), mHeight(0)
{ }

void Level::generate ()
{
    mTiles.clear();

    mWidth = width;
    mHeight = height;

    // Let's go for a bit larger map 8 by 8
    // It should look like this when displayed.
    // g - grass
    // w - water (blocking)
    // s - sand
    // t - tree (blocking)
    //   - empty
    //
    // g g g g g w w w
    // t g g t g w w w
    // g g g g g g w w
    //     g g   g g g
    //         t     g
    // s           t g
    // s s s       g g
    // s s s s s s   g
    //
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.back().resources().push_back(
        Resource ("gold", 10, 30, 100));
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'w', true});
    mTiles.push_back({'w', true});
    mTiles.push_back({'w', true});

    mTiles.push_back({'t', true});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'t', true});
    mTiles.push_back({'g', false});
    mTiles.push_back({'w', true});
    mTiles.push_back({'w', true});
    mTiles.push_back({'w', true});

    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'w', true});
    mTiles.push_back({'w', true});

    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});

    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'t', true});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'g', false});

    mTiles.push_back({'s', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'t', true});
    mTiles.push_back({'g', false});

    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'g', false});
    mTiles.push_back({'g', false});

    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({'s', false});
    mTiles.push_back({' ', false});
    mTiles.push_back({'g', false});
}

void Level::draw () const
{
    auto display = mGame->display();
    display->beginStreamingToMap();

    int i = 0;
    for (unsigned int y = 0; y < mHeight; ++y)
    {
        for (unsigned int x = 0; x < mWidth; ++x)
        {
            display->mapBuffer() << ' ' << mTiles[i].symbol();
            ++i;
        }

        display->mapBuffer() << std::endl;
    }

    display->endStreamingToMap();
}

std::vector<Location> Level::entryLocations (unsigned int count) const
{
    std::vector<Location> locations;

    for (unsigned int i = 0; i < count; ++i)
    {
        locations.push_back({0, 0});
    }

    return locations;
}

std::vector<Character> Level::spawnCreatures () const
{
    std::vector<Character> creatures;

    Behavior * behavior = mGame->creatureBehavior();
    creatures.push_back(Character(100, 'a', behavior));
    creatures.back().setLocation({7, 3});
    creatures.back().setMaxHealth(10, false);
    creatures.back().setCurrentHealth(10, false);
    creatures.back().setAttackDamage(4);

    creatures.push_back(Character(101, 'a', behavior));
    creatures.back().setLocation({6, 7});
    creatures.back().setMaxHealth(10, false);
    creatures.back().setCurrentHealth(10, false);
    creatures.back().setAttackDamage(4);

    return creatures;
}

Location Level::calculateMoveLocation (
    Location const & current,
    Location const & proposed) const
{
    if (proposed.x < 0 ||
        static_cast<unsigned int>(proposed.x) >= mWidth ||
        proposed.y < 0 ||
        static_cast<unsigned int>(proposed.y) >= mHeight
    )
    {
        return current;
    }

    int proposedIndex = locationToRowMajorIndex(proposed, mWidth);
    if (mTiles[proposedIndex].isObstruction())
    {
        return current;
    }

    return proposed;
}

Tile * Level::findTile (Location const & location)
{
    if (location.x < 0 ||
        static_cast<unsigned int>(location.x) >= mWidth ||
        location.y < 0 ||
        static_cast<unsigned int>(location.y) >= mHeight
        )
    {
        return nullptr;
    }

    return &mTiles[locationToRowMajorIndex(location, mWidth)];
}

Tile const * Level::findTile (Location const & location) const
{
    if (location.x < 0 ||
        static_cast<unsigned int>(location.x) >= mWidth ||
        location.y < 0 ||
        static_cast<unsigned int>(location.y) >= mHeight
        )
    {
        return nullptr;
    }

    return &mTiles[locationToRowMajorIndex(location, mWidth)];
}
