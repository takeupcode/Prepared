#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Point.h"

#include <vector>

class GameMap
{
public:
    static unsigned int constexpr MinBorderWidth = 25;
    static unsigned int constexpr MinRadius = 25;

    enum class Terrain
    {
        Water,
        Dirt,
        Sand,
        Tree,
        DeadTree,
        Grass,
        Ice
    };

    std::vector<std::vector<Terrain>> create (
        int seed = 0,
        unsigned int targetRadius = 100,
        unsigned int borderWidth = MinBorderWidth,
        unsigned int layers = 2);

private:
    std::vector<std::vector<int>> createMask ();

    int mSeed = 0;
    unsigned int mTargetRadius = 0;
    unsigned int mBorderWidth = 0;
    unsigned int mFadeWidth = 0;
    unsigned int mLayers = 0;
    std::vector<Point2i> mPath;
};

#endif // GAMEMAP_H
