#include "GameMap.h"

#include "Geology.h"
#include "Lerp.h"
#include "Math.h"
#include "Noise.h"
#include "Overloaded.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <random>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>

double getNoiseRadius (
    Noise const & noise,
    unsigned int targetRadius,
    unsigned int minRadius,
    unsigned int layers,
    double degrees)
{
    double const cycleDivisor = 40.0;
    double noiseValue = 0;

    noiseValue = static_cast<double>(targetRadius) + noise.generate(
        degrees / cycleDivisor,
        layers,
        360.0 / cycleDivisor);

    // Make sure that the island always has land at the center.
    if (noiseValue < minRadius)
    {
        noiseValue = minRadius;
    }

    return noiseValue;
}

// Returns a path from the start + 1 to the end.
std::vector<Point2i> getStraightPath (
    Point2i const & start,
    Point2i const & end)
{
    std::vector<Point2i> path;
    if (start == end)
    {
        return path;
    }

    // Calculate path from the middle of the points given.
    Point2d startDbl(0.5 + start.x, 0.5 + start.y);
    Point2d endDbl(0.5 + end.x, 0.5 + end.y);

    // Use DDA ray casting
    Point2d direction = Point2d(
        endDbl.x - startDbl.x,
        endDbl.y - startDbl.y).unit();
    Point2d unitSteps;
    if (direction.x == 0.0 || direction.y == 0.0)
    {
        // Either x or y is zero. This is a straight
        // line along an axis. No need to do ray casting.
        Point2i next = start;
        while (true)
        {
            next.x += direction.x;
            next.y += direction.y;

            path.push_back(next);

            if (next == end)
            {
                return path;;
            }
        }
    }
    else
    {
        // We need to do DDA ray casting. The nice thing
        // about this algorithm is that sqrt is expensive
        // but only needs to be done twice.
        unitSteps.x = std::sqrt(
            1 + (direction.y / direction.x) *
            (direction.y / direction.x));
        unitSteps.y = std::sqrt(
            1 + (direction.x / direction.y) *
            (direction.x / direction.y));
    }

    Point2i steps;
    Point2d rayLengths;
    if (direction.x < 0)
    {
        steps.x = -1;
        rayLengths.x = (startDbl.x - start.x) * unitSteps.x;
    }
    else
    {
        steps.x = 1;
        rayLengths.x = ((start.x + 1) - startDbl.x) * unitSteps.x;
    }

    if (direction.y < 0)
    {
        steps.y = -1;
        rayLengths.y = (startDbl.y - start.y) * unitSteps.y;
    }
    else
    {
        steps.y = 1;
        rayLengths.y = ((start.y + 1) - startDbl.y) * unitSteps.y;
    }

    Point2i next = start;
    while (true)
    {
        if (rayLengths.x < rayLengths.y)
        {
            next.x += steps.x;
            rayLengths.x += unitSteps.x;
        }
        else
        {
            next.y += steps.y;
            rayLengths.y += unitSteps.y;
        }

        path.push_back(next);

        if (next == end)
        {
            break;
        }
    }

    return path;
}

std::vector<Point2i> getPath (
    Noise const & noise,
    unsigned int targetRadius,
    unsigned int minRadius,
    unsigned int layers)
{
    std::vector<Point2i> path;

    double degrees = 0.0;
    auto radius = getNoiseRadius(
        noise,
        targetRadius,
        minRadius,
        layers,
        degrees);
    auto point = artop(degrees, radius);
    path.push_back(point);

    for (degrees = 1.0; degrees <= 360.0; ++degrees)
    {
        radius = getNoiseRadius(
            noise,
            targetRadius,
            minRadius,
            layers,
            degrees);
        point = artop(degrees, radius);

        auto points = getStraightPath(
            path.back(),
            point);

        for (auto const & next: points)
        {
            path.push_back(next);
        }
    }

    return path;
}

void adjustMinMaxValues (
    Point2i const & point,
    int & minX,
    int & maxX,
    int & minY,
    int & maxY)
{
    if (point.x < minX)
    {
        minX = point.x;
    }
    if (point.x > maxX)
    {
        maxX = point.x;
    }
    if (point.y < minY)
    {
        minY = point.y;
    }
    if (point.y > maxY)
    {
        maxY = point.y;
    }
}

void floodFill (
    int searchValue,
    int fillValue,
    int startX,
    int startY,
    int minX,
    int minY,
    int maxX,
    int maxY,
    std::vector<std::vector<int>> & result)
{
    std::stack<Point2i> fillPoints;
    fillPoints.emplace(startX, startY);
    result[startY][startX] = fillValue;
    while (!fillPoints.empty())
    {
        Point2i point = fillPoints.top();
        fillPoints.pop();

        if (point.y != minY && result[point.y - 1][point.x] == searchValue)
        {
            fillPoints.emplace(point.x, point.y - 1);
            result[point.y - 1][point.x] = fillValue;
        }
        if (point.y != maxY && result[point.y + 1][point.x] == searchValue)
        {
            fillPoints.emplace(point.x, point.y + 1);
            result[point.y + 1][point.x] = fillValue;
        }
        if (point.x != minX && result[point.y][point.x - 1] == searchValue)
        {
            fillPoints.emplace(point.x - 1, point.y);
            result[point.y][point.x - 1] = fillValue;
        }
        if (point.x != maxX && result[point.y][point.x + 1] == searchValue)
        {
            fillPoints.emplace(point.x + 1, point.y);
            result[point.y][point.x + 1] = fillValue;
        }
    }
}

void pathFill (
    int searchValue,
    int fillValue,
    int startX,
    int startY,
    std::vector<std::vector<int>> & result)
{
    // Scan out in a straight line until the searchValue is found.
    while (true)
    {
        if (result[startY][startX + 1] == searchValue)
        {
            break;
        }

        ++startX;
    }

    auto isNearSearchValue = [searchValue, &result] (int x, int y)
    {
        for (int nearY = y - 1; nearY <= y + 1; ++nearY)
        {
            for (int nearX = x - 1; nearX <= x + 1; ++nearX)
            {
                if (result[nearY][nearX] == searchValue)
                {
                    return true;
                }
            }
        }
        return false;
    };

    std::stack<Point2i> fillPoints;
    fillPoints.emplace(startX, startY);
    result[startY][startX] = fillValue;
    while (!fillPoints.empty())
    {
        Point2i point = fillPoints.top();
        fillPoints.pop();

        if (result[point.y - 1][point.x] != searchValue &&
            result[point.y - 1][point.x] != fillValue &&
            isNearSearchValue(point.x, point.y - 1))
        {
            fillPoints.emplace(point.x, point.y - 1);
            result[point.y - 1][point.x] = fillValue;
        }
        if (result[point.y + 1][point.x] != searchValue &&
            result[point.y + 1][point.x] != fillValue &&
            isNearSearchValue(point.x, point.y + 1))
        {
            fillPoints.emplace(point.x, point.y + 1);
            result[point.y + 1][point.x] = fillValue;
        }
        if (result[point.y][point.x - 1] != searchValue &&
            result[point.y][point.x - 1] != fillValue &&
            isNearSearchValue(point.x - 1, point.y))
        {
            fillPoints.emplace(point.x - 1, point.y);
            result[point.y][point.x - 1] = fillValue;
        }
        if (result[point.y][point.x + 1] != searchValue &&
            result[point.y][point.x + 1] != fillValue &&
            isNearSearchValue(point.x + 1, point.y))
        {
            fillPoints.emplace(point.x + 1, point.y);
            result[point.y][point.x + 1] = fillValue;
        }
    }
}

std::vector<std::vector<int>> GameMap::createMask ()
{
    Noise noise(mSeed, 1, 4);

    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();

    mPath = getPath(
        noise,
        mTargetRadius,
        MinRadius,
        mLayers);

    for (auto const & point: mPath)
    {
        adjustMinMaxValues(point, minX, maxX, minY, maxY);
    }

    int width = maxX - minX + 1 + mBorderWidth * 2;
    int height = maxY - minY + 1 + mBorderWidth * 2;

    std::vector<std::vector<int>> result;
    for (int y = 0; y < height; ++y)
    {
        auto & resultRow = result.emplace_back();
        // Start out with all zeroes.
        resultRow = std::vector<int>(width);
    }

    for (auto & point: mPath)
    {
        point.y = point.y - minY + mBorderWidth;
        point.x = point.x - minX + mBorderWidth;
        result[point.y][point.x] = 1;
    }

    std::string fileName = "./outline";
    fileName += std::to_string(mSeed);
    fileName += ".ppm";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char n = 255;
            n *= result[y][x];
            ofs << n << n << n;
        }
    }
    ofs.close();

    // Flood fill the outline from the center which must
    // be inside the outline because of the minimum radius.
    // We use -1 to represent land.
    floodFill(
        0,
        -1,
        width / 2,
        height / 2,
        0,
        0,
        width - 1,
        height - 1,
        result);

    // Fade the edges of the mask before returning it. We
    // do this by filling in extra paths inside the existing
    // outline. Each additional path uses an incremental
    // value.
    for (unsigned int fade = 2; fade <= mFadeWidth; ++fade)
    {
        pathFill(
            fade - 1,
            fade,
            width / 2,
            height / 2,
            result);
    }

    fileName = "./mask";
    fileName += std::to_string(mSeed);
    fileName += ".ppm";
    std::ofstream ofs2;
    ofs2.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs2 << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char n = 255;
            int mask = result[y][x];
            if (mask == 0)
            {
                n = 0;
            }
            else if (mask != -1)
            {
                n *= (static_cast<double>(mask) / mFadeWidth);
            }
            ofs2 << n << n << n;
        }
    }
    ofs2.close();

    return result;
}

std::vector<std::vector<GameMap::Terrain>> GameMap::create (
    int seed,
    unsigned int targetRadius,
    unsigned int borderWidth,
    unsigned int layers)
{
    if (targetRadius < MinRadius)
    {
        targetRadius = MinRadius;
    }

    if (borderWidth < MinBorderWidth)
    {
        // We need at least 1 border to know for sure
        // how to identify an inside point for filling.
        // But the minimum is higher because we also use
        // the border width to determine how much to fade
        // the edges. The fade amount will be scaled based
        // on the border width.
        borderWidth = MinBorderWidth;
    }

    Noise noise(seed);
    mSeed = noise.seed();
    mTargetRadius = targetRadius;
    mBorderWidth = borderWidth;
    mFadeWidth = borderWidth * 4 / 5;
    mLayers = layers;

    std::mt19937 rng;
    rng.seed(mSeed);
    std::uniform_int_distribution<int> dist(0, 100);

    int roll;
    std::vector<std::vector<int>> mask;
    unsigned int width {0};
    unsigned int height {0};
    unsigned int craterX {0};
    unsigned int craterY {0};

    while (true)
    {
        mask = createMask();

        height = static_cast<unsigned int>(mask.size());

        // All the rows in the mask are the same width. Use the
        // first to get the width.
        width = static_cast<unsigned int>(mask[0].size());

        // Try a few times to find a crater location that is
        // close to the center. If we can't find a location that
        // is on land, then recreate the mask.
        bool foundCrater = true;
        for (int i = 0; i < 3; ++i)
        {
            foundCrater = true; // Found until proven otherwise.
            roll = dist(rng);
            roll -= 50; // Shift the roll from (0, 100) to (-50, 50)
            craterX = width / 2 +
                static_cast<int>(width) * roll / 500;

            roll = dist(rng);
            roll -= 50;
            craterY = height / 2 +
                static_cast<int>(height) * roll / 500;

            // Check all around the potential location to
            // make sure the location isn't right on the
            // edge of the mask.
            unsigned int yMin = craterY - height / 10;
            unsigned int yMax = craterY + height / 10;
            unsigned int xMin = craterX - width / 10;
            unsigned int xMax = craterX + width / 10;
            for (unsigned int x = xMin; x <= xMax; ++x)
            {
                if (mask[yMin][x] != -1)
                {
                    foundCrater = false;
                    break;
                }
                if (mask[yMax][x] != -1)
                {
                    foundCrater = false;
                    break;
                }
            }
            if (!foundCrater)
            {
                continue;
            }
            for (unsigned int y = yMin; y <= yMax; ++y)
            {
                if (mask[y][xMin] != -1)
                {
                    foundCrater = false;
                    break;
                }
                if (mask[y][xMax] != -1)
                {
                    foundCrater = false;
                    break;
                }
            }
            if (foundCrater)
            {
                break;
            }
        }
        if (foundCrater)
        {
            break;
        }
    }

    // Generate a noise map with enough room to shift
    // it around so that a high point can be aligned
    // with the crater.
    std::vector<double> noiseMap(width * height * 9);
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (unsigned int y = 0; y < height * 3; ++y)
    {
        for (unsigned int x = 0; x < width * 3; ++x)
        {
            double noiseValue = noise.generate(
                static_cast<double>(x) / 64,
                static_cast<double>(y) / 64,
                layers);
            noiseMap[y * width * 3 + x] = noiseValue;

            if (noiseValue < min)
            {
                min = noiseValue;
            }
            if (noiseValue > max)
            {
                max = noiseValue;
            }
        }
    }

    // Find the location of the highest point in the center third.
    double maxCenter = std::numeric_limits<double>::min();
    unsigned int maxX {0};
    unsigned int maxY {0};
    for (unsigned int y = height; y < height * 2; ++y)
    {
        for (unsigned int x = width; x < width * 2; ++x)
        {
            double noiseValue = noiseMap[y * width * 3 + x];
            if (noiseValue > maxCenter)
            {
                maxCenter = noiseValue;
                maxX = x;
                maxY = y;
            }
        }
    }

    double minMaxShift = -min;
    double minMaxRange = max - min;

    std::vector<unsigned char> heightMap(width * height);
    std::vector<GeoType> geoMap(width * height);
    for (unsigned int y = 0; y < height; ++y)
    {
        unsigned int noiseY = maxY - craterY + y;
        for (unsigned int x = 0; x < width; ++x)
        {
            unsigned char n {0};
            if (mask[y][x] != 0)
            {
                unsigned int noiseX = maxX - craterX + x;
                double noiseValue = noiseMap[noiseY * width * 3 + noiseX];
                n = static_cast<unsigned char>(
                    (noiseValue + minMaxShift) / minMaxRange * 255
                    );

                // Make sure the land itself doesn't turn into water
                // or sand. Sand will only be allowed at the coast.
                n = std::max(n, static_cast<unsigned char>(15));

                if (mask[y][x] != -1)
                {
                    n *= (static_cast<double>(mask[y][x]) / mFadeWidth);
                }

                Point2i point(x, y);
                geoMap[y * width + x] = DryLand {point};
            }
            else
            {
                Point2i point(x, y);
                geoMap[y * width + x] = Ocean {point};
            }

            heightMap[y * width + x] = n;
        }
    }

    unsigned int riverCount = dist(rng) / 30 + 1;
    while (true)
    {
        // Choose a possible starting point within the map
        // with a 1/10 border.
        roll = dist(rng);
        auto x = roll * (width / 10 * 8) / 100;
        x += width / 10;
        roll = dist(rng);
        auto y = roll * (height / 10 * 8) / 100;
        y += height / 10;

        // Try to add the river with a minimum length of
        // 1/10 of the width. This is just to make sure
        // that the rivers are not too short.
        if (!addRiver(
            x,
            y,
            heightMap,
            geoMap,
            width,
            height,
            width / 10))
        {
            continue;
        }

        if (--riverCount == 0)
        {
            break;
        }
    }
    for (unsigned int i = 0; i < width * height; ++i)
    {
        std::visit(
            Overloaded {
                [&heightMap, &i](auto & v) { heightMap[i] = 0; },
                [](DryLand & v) { },
                [](Ocean & v) { }
            },
            geoMap[i]);
    }

    std::string fileName = "./noise";
    fileName += std::to_string(mSeed);
    fileName += ".ppm";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    std::vector<std::vector<GameMap::Terrain>> result;

    auto addIce = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(255)
            << static_cast<unsigned char>(255)
            << static_cast<unsigned char>(255);

        row.push_back(Terrain::Ice);
    };
    auto addGrass = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(34)
            << static_cast<unsigned char>(139)
            << static_cast<unsigned char>(34);

        row.push_back(Terrain::Grass);
    };
    auto addDirt = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(150)
            << static_cast<unsigned char>(100)
            << static_cast<unsigned char>(100);

        row.push_back(Terrain::Dirt);
    };
    auto addTree = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(100)
            << static_cast<unsigned char>(0);

        row.push_back(Terrain::Tree);
    };
    auto addDeadTree = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(75)
            << static_cast<unsigned char>(0);

        row.push_back(Terrain::DeadTree);
    };
    auto addSand = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(157)
            << static_cast<unsigned char>(127)
            << static_cast<unsigned char>(97);

        row.push_back(Terrain::Sand);
    };
    auto addWater = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(0);

        row.push_back(Terrain::Water);
    };

    unsigned int i = 0;
    for (unsigned int y = 0; y < height; ++y)
    {
        auto & resultRow = result.emplace_back();

        for (unsigned int x = 0; x < width; ++x)
        {
            unsigned char n = heightMap[y * width + x];

            roll = dist(rng);
            if (n >= 240)
            {
                if (roll > 15)
                {
                    addIce(resultRow);
                }
                else
                {
                    addDirt(resultRow);
                }
            }
            else if (n >= 195)
            {
                double range = 240 - 195;
                double shifted = n - 195;
                double scaled = shifted / range * 100;
                if (scaled > roll)
                {
                    addIce(resultRow);
                }
                else
                {
                    if (roll > 25)
                    {
                        addTree(resultRow);
                    }
                    else if (roll > 0)
                    {
                        addDirt(resultRow);
                    }
                    else
                    {
                        addDeadTree(resultRow);
                    }
                }
            }
            else if (n >= 150)
            {
                if (roll > 25)
                {
                    addTree(resultRow);
                }
                else if (roll > 8)
                {
                    addGrass(resultRow);
                }
                else if (roll > 0)
                {
                    addDirt(resultRow);
                }
                else
                {
                    addDeadTree(resultRow);
                }
            }
            else if (n >= 115)
            {
                double range = 150 - 115;
                double shifted = n - 115;
                double scaled = shifted / range * 100;
                if (scaled > roll)
                {
                    if (roll > 40)
                    {
                        addTree(resultRow);
                    }
                    else if (roll > 0)
                    {
                        addDirt(resultRow);
                    }
                    else
                    {
                        addDeadTree(resultRow);
                    }
                }
                else
                {
                    addGrass(resultRow);
                }
            }
            else if (n >= 15)
            {
                if (roll > 15)
                {
                    addGrass(resultRow);
                }
                else if (roll > 0)
                {
                    addTree(resultRow);
                }
                else
                {
                    addDeadTree(resultRow);
                }
            }
            else if (n >= 1)
            {
                if (roll > 5)
                {
                    addSand(resultRow);
                }
                else if (roll > 0)
                {
                    addTree(resultRow);
                }
                else
                {
                    addDeadTree(resultRow);
                }
            }
            else
            {
                addWater(resultRow);
            }

            ++i;
        }
    }
    ofs.close();

    return result;
}
