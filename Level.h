#ifndef LEVEL_H
#define LEVEL_H

#include "GameItem.h"
#include "Point.h"

#include <unordered_map>
#include <vector>

class Game;

class Level
{
public:
    Level (Game * game, int seed = 0);

    void generate ();

    void draw () const;

    std::vector<Point2i> entryLocations (
        unsigned int count) const;

    void spawnCreatures () const;

    Point2i calculateMoveLocation (
        Point2i const & current,
        Point2i const & proposed,
        int moveLayerId) const;

    GameItem * findTile (Point2i const & location) const;

    int width ();
    int height ();

    std::vector<int> collidingLayerIds (int layerId);

    void setCollidingLayerIds (
        int layerId,
        std::vector<int> const & collidingLayers);

    Point2i findRandomLocationOnLand () const;

private:
    GameItem * createRat () const;

    Game * mGame;
    int mSeed;
    unsigned int mWidth;
    unsigned int mHeight;
    std::vector<GameItem *> mTiles;
    std::vector<Point2i> mEntryLocations;
    std::unordered_map<int, GameItem *> mLayeredTiles;
    std::unordered_map<int, std::vector<int>> mCollidingLayerIds;
};

#endif // LEVEL_H
