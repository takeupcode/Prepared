#include "Geology.h"

#include "Overloaded.h"

#include <stack>

bool isTooCloseToOcean (
    int startX,
    int startY,
    std::vector<unsigned char> const & heightMap,
    int mapWidth,
    int mapHeight,
    int minRiverLength)
{
    // Check all around the potential location to make
    // sure the location isn't too close to the ocean.
    int yMin = startY - minRiverLength;
    int yMax = startY + minRiverLength;
    if (yMin < 0 || yMax >= mapHeight)
    {
        return true;
    }

    int xMin = startX - minRiverLength;
    int xMax = startX + minRiverLength;
    if (xMin < 0 || xMax >= mapWidth)
    {
        return true;
    }

    unsigned int index;
    for (int x = xMin; x <= xMax; ++x)
    {
        Point2i ptMin(x, yMin);
        index = pointToRowMajorIndex(ptMin, mapWidth);
        if (heightMap[index] == 0)
        {
            return true;
        }
        Point2i ptMax(x, yMax);
        index = pointToRowMajorIndex(ptMax, mapWidth);
        if (heightMap[index] == 0)
        {
            return true;
        }
    }
    for (int y = yMin; y <= yMax; ++y)
    {
        Point2i ptMin(xMin, y);
        index = pointToRowMajorIndex(ptMin, mapWidth);
        if (heightMap[index] == 0)
        {
            return true;
        }
        Point2i ptMax(xMax, y);
        index = pointToRowMajorIndex(ptMax, mapWidth);
        if (heightMap[index] == 0)
        {
            return true;
        }
    }

    return false;
}

#if 0
void widenRiver (
    Point2i const & point,
    Point2i const & next,
    std::vector<GeoType> const & geoMap,
    int mapWidth)
{
    Direction dir;
    if (next.x > point.x)
    {
        dir = Direction::East;
    }
    else if (next.x < point.x)
    {
        dir = Direction::West;
    }
    else if (next.y > point.y)
    {
        dir = Direction::South;
    }
    else (next.y < point.y)
    {
        dir = Direction::North;
    }
}
#endif

LakeCenter * lakeCenter (
    LakeCenter const & lc,
    std::vector<GeoType> & geoMap,
    int mapWidth)
{
    unsigned int index;
    index = pointToRowMajorIndex(lc.mTrueCenter, mapWidth);
    // This might result in the same center. But
    // it doesn't hurt to explicitly get the true center.
    return std::get_if<LakeCenter>(&geoMap[index]);
}

LakeCenter * lakeCenter (
    LakeFill const & lf,
    std::vector<GeoType> & geoMap,
    int mapWidth)
{
    unsigned int index;
    index = pointToRowMajorIndex(lf.mCenter, mapWidth);
    auto * center = std::get_if<LakeCenter>(&geoMap[index]);
    // This might result in the same center. But
    // it doesn't hurt to explicitly get the true center.
    index = pointToRowMajorIndex(center->mTrueCenter, mapWidth);
    return std::get_if<LakeCenter>(&geoMap[index]);
}

LakeCenter * joinLakeCenters (
    LakeCenter * from,
    LakeCenter * to,
    std::vector<GeoType> & geoMap,
    int mapWidth)
{
    unsigned int index;
    if (from->mTrueCenter != from->mSelf)
    {
        index = pointToRowMajorIndex(from->mTrueCenter, mapWidth);
        auto * lc = std::get_if<LakeCenter>(&geoMap[index]);
        return joinLakeCenters (
            lc,
            to,
            geoMap,
            mapWidth);
    }

    from->mTrueCenter = to->mSelf;
    for (auto & joinedCenter: from->mJoinedCenters)
    {
        index = pointToRowMajorIndex(joinedCenter, mapWidth);
        auto & lc = std::get<LakeCenter>(geoMap[index]);
        lc.mTrueCenter = to->mSelf;
    }

    to->mEdges.insert(from->mEdges.begin(), from->mEdges.end());
    to->mJoinedCenters.insert(
        from->mJoinedCenters.begin(),
        from->mJoinedCenters.end());
    to->mJoinedCenters.insert(from->mSelf);

    from->mEdges.clear();
    from->mJoinedCenters.clear();

    return to;
}

unsigned char elevationFromLakeCenter (
    Point2i const & point,
    std::vector<GeoType> const & geoMap,
    int mapWidth)
{
    unsigned int index = pointToRowMajorIndex(point, mapWidth);
    auto & lc = std::get<LakeCenter>(geoMap[index]);
    if (lc.mTrueCenter != lc.mSelf)
    {
        return elevationFromLakeCenter(
            lc.mTrueCenter,
            geoMap,
            mapWidth);
    }

    return lc.mElevation;
}

unsigned char elevation (
    Point2i const & point,
    std::vector<unsigned char> const & heightMap,
    std::vector<GeoType> const & geoMap,
    int mapWidth)
{
    unsigned char elevation {0};
    unsigned int index = pointToRowMajorIndex(point, mapWidth);
    std::visit(
        Overloaded
        {
            [](auto & v) {},
            [&elevation, index, &heightMap](DryLand & v)
            {
                elevation = heightMap[index];
            },
            [&elevation](Ocean & v)
            {
                elevation = 0;
            },
            [&elevation, index, &heightMap](RiverCenter & v)
            {
                elevation = heightMap[index];
            },
            [&elevation, &geoMap, mapWidth](LakeCenter & v)
            {
                elevation = elevationFromLakeCenter(
                    v.mTrueCenter,
                    geoMap,
                    mapWidth);
            },
            [&elevation, &geoMap, mapWidth](LakeFill & v)
            {
                elevation = elevationFromLakeCenter(
                    v.mCenter,
                    geoMap,
                    mapWidth);
            }
        },
        geoMap[index]);

    return elevation;
}

Point2i lowestNeighbor (
    Point2i const & point,
    std::vector<unsigned char> const & heightMap,
    std::vector<GeoType> const & geoMap,
    int mapWidth,
    int mapHeight)
{
    std::vector<Point2i> nearbyPoints;
    if (point.y != 0)
    {
        nearbyPoints.push_back({point.x, point.y - 1});
    }
    if (point.y != mapHeight)
    {
        nearbyPoints.push_back({point.x, point.y + 1});
    }
    if (point.x != 0)
    {
        nearbyPoints.push_back({point.x - 1, point.y});
    }
    if (point.x != mapWidth)
    {
        nearbyPoints.push_back({point.x + 1, point.y});
    }

    unsigned char lowestElevation = elevation(
        point,
        heightMap,
        geoMap,
        mapWidth);
    Point2i lowestPoint(point);

    for (Point2i const & neighborPoint: nearbyPoints)
    {
        unsigned char neighborElevation = elevation(
            neighborPoint,
            heightMap,
            geoMap,
            mapWidth);
        if (neighborElevation < lowestElevation)
        {
            lowestElevation = neighborElevation;
            lowestPoint = neighborPoint;
        }
    }

    return lowestPoint;
}

Point2i fillLake (
    Point2i centerPoint,
    std::vector<unsigned char> const & heightMap,
    std::vector<GeoType> & geoMap,
    int mapWidth,
    int mapHeight,
    std::unordered_set<Point2i> & visited)
{
    // Check what type of geo is at the center point. If it's
    // a river, then the river couldn't go any further and needs
    // to turn into a lake so it can start filling up. If it's
    // already a lake, then the river ran into a lake. If we have
    // already visited this lake, then the river turned out to be
    // a false drain and we need to keep filling the lake to look
    // for another drain. If the lake has not yet been visited,
    // then the river ran into an existing lake that should already
    // have a drain going to the ocean.
    Point2i drainPoint;
    bool drained = false;
    unsigned int index =
        pointToRowMajorIndex(centerPoint, mapWidth);
    std::visit(
        Overloaded
        {
            [&drained](auto & v)
            {
                // This should not happen. If it does, we'll
                // return a drain point of 0, 0 which will be
                // a reasonable result.
                drained = true;
            },
            [&centerPoint, &geoMap, &index](RiverCenter & v)
            {
                // Create a new lake starting at the center point.
                // The true center will change if this lake joins with
                // another lake. The center point will already have
                // been added to the visited set.
                geoMap[index] = LakeCenter {
                    centerPoint,
                    centerPoint,
                    0,
                    {centerPoint}};
            },
            [&drained, &centerPoint, &index, &visited,
            mapWidth](LakeCenter & v)
            {
                auto iter = visited.find(v.mSelf);
                if (iter == visited.end())
                {
                    drained = true;
                    return;
                }
                centerPoint = v.mTrueCenter;
                index = pointToRowMajorIndex(centerPoint, mapWidth);
            },
            [&drained, &centerPoint, &geoMap, &index, &visited,
            mapWidth](LakeFill & v)
            {
                auto iter = visited.find(v.mSelf);
                if (iter == visited.end())
                {
                    drained = true;
                    return;
                }
                auto * vc = lakeCenter(v, geoMap, mapWidth);
                centerPoint = vc->mTrueCenter;
                index = pointToRowMajorIndex(centerPoint, mapWidth);
            }
        },
        geoMap[index]);

    // Get the lake center as a pointer so it can be updated
    // in case the lake gets linked to another lake.
    auto * lc = std::get_if<LakeCenter>(&geoMap[index]);

    std::stack<Point2i> fillPoints;
    unsigned char elevation = heightMap[index];
    while (!drained)
    {
        ++elevation;
        lc->mElevation = elevation;
        
        for (auto & edgePoint: lc->mEdges)
        {
            fillPoints.emplace(edgePoint);
        }
        while (!fillPoints.empty())
        {
            Point2i point = fillPoints.top();
            fillPoints.pop();
            std::vector<unsigned int> nearbyIndices;
            if (point.y != 0)
            {
                nearbyIndices.push_back((point.y - 1) * mapWidth + point.x);
            }
            if (point.y != mapHeight)
            {
                nearbyIndices.push_back((point.y + 1) * mapWidth + point.x);
            }
            if (point.x != 0)
            {
                nearbyIndices.push_back(point.y * mapWidth + point.x - 1);
            }
            if (point.x != mapWidth)
            {
                nearbyIndices.push_back(point.y * mapWidth + point.x + 1);
            }

            bool stillEdge = false;
            for (unsigned int i: nearbyIndices)
            {
                std::visit(
                    Overloaded
                    {
                        [&drained, &drainPoint, &centerPoint, &geoMap,
                        &heightMap, &elevation, &fillPoints, &lc, &i,
                        &stillEdge, &visited](DryLand & v)
                        {
                            if (heightMap[i] == elevation)
                            {
                                // This lets the lake expand out into level
                                // ground without treating it as a drain.
                                Point2i pt = v.mSelf;
                                fillPoints.emplace(pt);
                                geoMap[i] = LakeFill {centerPoint, pt};
                                lc->mEdges.insert(pt);
                                visited.insert(pt);
                            }
                            else if (heightMap[i] < elevation)
                            {
                                drainPoint = v.mSelf;
                                drained = true;
                                stillEdge = true;
                            }
                            else
                            {
                                stillEdge = true;
                            }
                        },
                        [&drained, &drainPoint, &stillEdge](Ocean & v)
                        {
                            drainPoint = v.mSelf;
                            drained = true;
                            stillEdge = true;
                        },
                        [&drained, &drainPoint, &centerPoint, &geoMap,
                        &heightMap, &elevation, &fillPoints, &lc, &i,
                        &stillEdge, &visited](RiverCenter & v)
                        {
                            if (heightMap[i] > elevation)
                            {
                                stillEdge = true;
                                return;
                            }

                            // The lake is expanding into a
                            // river. It might be a river that
                            // supplies water to this lake and
                            // that we've already visited. Or it
                            // might be a completely different river.
                            // If it is a new river, then it should
                            // already drain to the ocean.
                            auto iter = visited.find(v.mSelf);
                            if (iter == visited.end())
                            {
                                drainPoint = v.mSelf;
                                drained = true;
                                stillEdge = true;
                                return;
                            }

                            Point2i pt = v.mSelf;
                            fillPoints.emplace(pt);
                            geoMap[i] = LakeFill {centerPoint, pt};
                            lc->mEdges.insert(pt);
                        },
                        [&drained, &drainPoint, &centerPoint, &geoMap,
                        &elevation, &fillPoints, &lc, &stillEdge, &visited,
                        mapWidth](LakeCenter & v)
                        {
                            if (v.mTrueCenter == centerPoint)
                            {
                                return;
                            }
                            // We found a different lake. It might
                            // already be joined with this one.
                            auto iter = lc->mJoinedCenters.find(v.mSelf);
                            if (iter != lc->mJoinedCenters.end())
                            {
                                return;
                            }

                            // It is a new lake that is not part of the
                            // current lake. Make sure to deal with the
                            // true center of the new lake.
                            auto * vc = lakeCenter(v, geoMap, mapWidth);

                            // Add the new edges to explore since we
                            // might start filling the new lake too.
                            for (auto & edgePoint: vc->mEdges)
                            {
                                fillPoints.emplace(edgePoint);
                            }

                            // Join the current lake with the new one.
                            lc = joinLakeCenters (
                                lc,
                                vc,
                                geoMap,
                                mapWidth);
                            centerPoint = lc->mSelf;

                            // Set the current elevation to match the
                            // new lake.
                            elevation = lc->mElevation;

                            // If we haven't visited this new lake,
                            // then it should already have its own
                            // drain all the way to the ocean. If we
                            // have visited it, then we need to keep
                            // filling it because all we've done is
                            // made a bigger lake without a drain.
                            auto iter2 = visited.find(v.mSelf);
                            if (iter2 == visited.end())
                            {
                                drainPoint = v.mSelf;
                                drained = true;
                                stillEdge = true;
                                return;
                            }
                        },
                        [&drained, &drainPoint, &centerPoint, &geoMap,
                        &elevation, &fillPoints, &lc, &stillEdge, &visited,
                        mapWidth](LakeFill & v)
                        {
                            if (v.mCenter == centerPoint)
                            {
                                return;
                            }
                            // We found a different lake. It might
                            // already be joined with this one.
                            auto iter = lc->mJoinedCenters.find(v.mCenter);
                            if (iter != lc->mJoinedCenters.end())
                            {
                                return;
                            }

                            // It is a new lake that is not part of the
                            // current lake. Get the center and proceed
                            // similar to the previous case where we
                            // found a new lake center directly.
                            auto * vc = lakeCenter(v, geoMap, mapWidth);

                            // Add the new edges to explore since we
                            // might start filling the new lake too.
                            for (auto & edgePoint: vc->mEdges)
                            {
                                fillPoints.emplace(edgePoint);
                            }

                            // Join the current lake with the new one.
                            lc = joinLakeCenters (
                                lc,
                                vc,
                                geoMap,
                                mapWidth);
                            centerPoint = lc->mSelf;

                            // Set the current elevation to match the
                            // new lake.
                            elevation = lc->mElevation;

                            // If we haven't visited this new lake,
                            // then it should already have its own
                            // drain all the way to the ocean. If we
                            // have visited it, then we need to keep
                            // filling it because all we've done is
                            // made a bigger lake without a drain.
                            auto iter2 = visited.find(v.mSelf);
                            if (iter2 == visited.end())
                            {
                                drainPoint = v.mSelf;
                                drained = true;
                                stillEdge = true;
                                return;
                            }
                        }
                    },
                    geoMap[i]);
            }

            if (!stillEdge)
            {
                lc->mEdges.erase(point);
            }
        }
    }

    return drainPoint;
}

bool addRiver (
    unsigned int startX,
    unsigned int startY,
    std::vector<unsigned char> const & heightMap,
    std::vector<GeoType> & geoMap,
    unsigned int mapWidth,
    unsigned int mapHeight,
    unsigned int minRiverLength)
{
    if (isTooCloseToOcean(
        startX,
        startY,
        heightMap,
        mapWidth,
        mapHeight,
        minRiverLength))
    {
        return false;
    }

    Point2i point(startX, startY);
    unsigned int index = pointToRowMajorIndex(point, mapWidth);
    bool startOnWater = false;
    std::visit(
        Overloaded
        {
            [&startOnWater](auto & v) { startOnWater = true; },
            [](DryLand & v) { }
        },
        geoMap[index]);

    if (startOnWater)
    {
        return false;
    }

    // Watch for river and lake cycles.
    std::unordered_set<Point2i> visited;

    geoMap[index] = RiverCenter {point};
    visited.insert(point);

    //unsigned int length {0};
    bool done = false;
    while (!done)
    {
        auto nextPoint = lowestNeighbor (
            point,
            heightMap,
            geoMap,
            mapWidth,
            mapHeight);
        if (nextPoint == point)
        {
            auto drain = fillLake(
                point,
                heightMap,
                geoMap,
                mapWidth,
                mapHeight,
                visited);

            std::visit(
                Overloaded
                {
                    [&done](auto & v) { done = true; },
                    [&point, &geoMap, &visited, mapWidth](DryLand & v)
                    {
                        // Treat this just like starting a new
                        // river at the drain position. It might
                        // immediately join back into the lake or
                        // it might take off in a new direction.
                        point = v.mSelf;
                        unsigned int index =
                            pointToRowMajorIndex(point, mapWidth);
                        geoMap[index] = RiverCenter {point};
                        visited.insert(point);
                    }
                },
                geoMap[pointToRowMajorIndex(drain, mapWidth)]);

            continue;
        }

        unsigned int nextIndex = pointToRowMajorIndex(nextPoint, mapWidth);
        std::visit(
            Overloaded
            {
                [&done](auto & v)
                {
                    done = true;
                },
                [&point, &geoMap, &visited, nextIndex](DryLand & v)
                {
                    point = v.mSelf;
                    geoMap[nextIndex] = RiverCenter {point};
                    visited.insert(point);
                },
                [&done, &point, &visited, &geoMap, &heightMap, mapWidth,
                mapHeight](LakeCenter & v)
                {
                    point = fillLake(
                        v.mTrueCenter,
                        heightMap,
                        geoMap,
                        mapWidth,
                        mapHeight,
                        visited);

                    std::visit(
                        Overloaded
                        {
                            [&done](auto & v) { done = true; },
                            [&point, &geoMap, &visited, mapWidth](DryLand & v)
                            {
                                // Treat this just like starting a new
                                // river at the drain position. It might
                                // immediately join back into the lake or
                                // it might take off in a new direction.
                                unsigned int index =
                                    pointToRowMajorIndex(point, mapWidth);
                                geoMap[index] = RiverCenter {point};
                                visited.insert(point);
                            }
                        },
                        geoMap[pointToRowMajorIndex(point, mapWidth)]);
                },
                [&done, &point, &visited, &geoMap, &heightMap, mapWidth,
                mapHeight](LakeFill & v)
                {
                    point = fillLake(
                        v.mCenter,
                        heightMap,
                        geoMap,
                        mapWidth,
                        mapHeight,
                        visited);

                    std::visit(
                        Overloaded
                        {
                            [&done](auto & v) { done = true; },
                            [&point, &geoMap, &visited, mapWidth](DryLand & v)
                            {
                                // Treat this just like starting a new
                                // river at the drain position. It might
                                // immediately join back into the lake or
                                // it might take off in a new direction.
                                unsigned int index =
                                    pointToRowMajorIndex(point, mapWidth);
                                geoMap[index] = RiverCenter {point};
                                visited.insert(point);
                            }
                        },
                        geoMap[pointToRowMajorIndex(point, mapWidth)]);
                }
            },
            geoMap[nextIndex]);
    }

    return true;
}
