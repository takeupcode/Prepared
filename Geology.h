#ifndef GEOLOGY_H
#define GEOLOGY_H

#include "Direction.h"
#include "Point.h"

#include <unordered_set>
#include <vector>
#include <variant>

struct DryLand
{
    Point2i mSelf;
};

struct Ocean
{
    Point2i mSelf;
};

struct RiverCenter
{
    Point2i mSelf;
};

//struct RiverOverflow
//{
//    Point2i mSelf;
//};

struct LakeCenter
{
    Point2i mTrueCenter;
    Point2i mSelf;
    //double mInFlow;
    //double mVolume;
    unsigned char mElevation;
    std::unordered_set<Point2i> mEdges;
    std::unordered_set<Point2i> mJoinedCenters;
};

struct LakeFill
{
    Point2i mCenter;
    Point2i mSelf;
};

using GeoType = std::variant<
    DryLand,
    Ocean,
    RiverCenter,
//    RiverOverflow,
    LakeCenter,
    LakeFill
>;

bool addRiver (
    unsigned int startX,
    unsigned int startY,
    std::vector<unsigned char> const & heightMap,
    std::vector<GeoType> & geoMap,
    unsigned int mapWidth,
    unsigned int mapHeight,
    unsigned int minRiverLength);

#endif // GEOLOGY_H
