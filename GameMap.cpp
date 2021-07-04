#include "GameMap.h"

#include "Lerp.h"
#include "Math.h"
#include "Noise.h"
#include "Point.h"

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
    unsigned int layers,
    double degrees)
{
    double const cycleDivisor = 40.0;
    double noiseValue = 0;

    noiseValue = static_cast<double>(targetRadius) + noise.generate(
        degrees / cycleDivisor,
        layers,
        360.0 / cycleDivisor);

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
    unsigned int layers)
{
    std::vector<Point2i> path;

    double degrees = 0.0;
    auto radius = getNoiseRadius (
        noise,
        targetRadius,
        layers,
        degrees);
    auto point = artop(degrees, radius);
    path.push_back(point);

    for (degrees = 1.0; degrees <= 360.0; ++degrees)
    {
        radius = getNoiseRadius (
            noise,
            targetRadius,
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
    int startX,
    int startY,
    int minX,
    int minY,
    int maxX,
    int maxY,
    std::vector<std::vector<bool>> & result)
{
    std::stack<Point2i> fillPoints;
    fillPoints.emplace(startX, startY);
    result[startY][startX] = true;
    while (!fillPoints.empty())
    {
        Point2i point = fillPoints.top();
        fillPoints.pop();

        if (point.y != minY && !result[point.y - 1][point.x])
        {
            fillPoints.emplace(point.x, point.y - 1);
            result[point.y - 1][point.x] = true;
        }
        if (point.y != maxY && !result[point.y + 1][point.x])
        {
            fillPoints.emplace(point.x, point.y + 1);
            result[point.y + 1][point.x] = true;
        }
        if (point.x != minX && !result[point.y][point.x - 1])
        {
            fillPoints.emplace(point.x - 1, point.y);
            result[point.y][point.x - 1] = true;
        }
        if (point.x != maxX && !result[point.y][point.x + 1])
        {
            fillPoints.emplace(point.x + 1, point.y);
            result[point.y][point.x + 1] = true;
        }
    }
}

std::vector<std::vector<bool>> createMask (
    int seed,
    unsigned int targetRadius,
    unsigned int borderWidth,
    unsigned int layers)
{
    Noise noise(seed, 1, 4);

    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();

    auto path = getPath (
        noise,
        targetRadius,
        layers);

    for (auto const & point: path)
    {
        adjustMinMaxValues(point, minX, maxX, minY, maxY);
    }

    int width = maxX - minX + 1 + borderWidth * 2;
    int height = maxY - minY + 1 + borderWidth * 2;

    std::vector<std::vector<bool>> result;
    for (int y = 0; y < height; ++y)
    {
        auto & resultRow = result.emplace_back();
        resultRow = std::vector<bool>(width);
    }

    for (auto const & point: path)
    {
        int resultY = point.y - minY + borderWidth;
        int resultX = point.x - minX + borderWidth;
        result[resultY][resultX] = true;
    }

    std::string fileName = "./outline";
    fileName += std::to_string(seed);
    fileName += ".ppm";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char n = 0;
            if (result[y][x])
            {
                n = 255;
            }
            ofs << n << n << n;
        }
    }
    ofs.close();

    // Flood fill the outlined bools from a known point
    // inside the outline. This is not as easy as it seems.
    // The only points we know for certain are the outside
    // and the border points. We can first flood fill a
    // portion of the outside. Don't need to do the entire
    // border. And then use those identified outside points
    // to make sure that we don't accidentally select an
    // outside point when trying to flood fill the inside.
    int outsideStartFillY = height / 2;
    int outsideFillMargin = 5;
    int outsideFillMinY = outsideStartFillY - outsideFillMargin;
    std::vector<std::vector<bool>> outsideResult;
    for (int y = 0; y < (outsideFillMargin * 2 + 1); ++y)
    {
        auto & outsideRow = outsideResult.emplace_back();
        outsideRow = result[outsideFillMinY + y];
    }
    floodFill (
        0,
        0,
        0,
        0,
        width - 1,
        outsideFillMargin * 2,
        outsideResult);

    std::stack<Point2i> fillPoints;
    int startFillY = height / 2;
    int startFillX = -1;
    bool foundEdge = false;
    while (true)
    {
        ++startFillX;
        if (!foundEdge && result[startFillY][startFillX])
        {
            // We found the outline edge. Keep looking
            // until we find an inside point that is
            // false;
            foundEdge = true;
        }
        else if (foundEdge && !result[startFillY][startFillX])
        {
            // Stop looking for an inside point only when
            // we are sure it really is inside. Check the
            // point against the known outside points to
            // be sure.
            int outsideY = startFillY - outsideFillMinY;
            if (!outsideResult[outsideY][startFillX])
            {
                break;
            }
        }
    }
    floodFill (
        startFillX,
        startFillY,
        0,
        0,
        width - 1,
        height - 1,
        result);

    fileName = "./mask";
    fileName += std::to_string(seed);
    fileName += ".ppm";
    std::ofstream ofs2;
    ofs2.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs2 << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char n = 0;
            if (result[y][x])
            {
                n = 255;
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
    Noise noise(seed);
    seed = noise.seed();

    if (targetRadius < MinRadius)
    {
        targetRadius = MinRadius;
    }

    if (borderWidth < MinBorderWidth)
    {
        // We need at least 1 border to know for sure
        // how to identify an inside point for filling.
        borderWidth = MinBorderWidth;
    }

    std::vector<std::vector<bool>> mask =
        createMask(seed, targetRadius, borderWidth, layers);

    const unsigned int height =
        static_cast<unsigned int>(mask.size());

    // All the rows in the mask are the same width. Use the
    // first to get the width.
    const unsigned int width =
        static_cast<unsigned int>(mask[0].size());

    double *noiseMap = new double[width * height];

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            double noiseValue = noise.generate(
                static_cast<double>(x) / 64,
                static_cast<double>(y) / 64,
                layers);
            noiseMap[y * width + x] = noiseValue;

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

    double minMaxShift = -min;
    double minMaxRange = max - min;

    std::string fileName = "./noise";
    fileName += std::to_string(seed);
    fileName += ".ppm";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    std::vector<std::vector<GameMap::Terrain>> result;

    std::mt19937 rng;
    rng.seed(seed);
    std::uniform_int_distribution<int> dist(0, 100);

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
    auto addMarsh = [&ofs] (std::vector<GameMap::Terrain> & row)
    {
        ofs << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(0)
            << static_cast<unsigned char>(160);

        row.push_back(Terrain::Marsh);
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
            unsigned char n = static_cast<unsigned char>(
                (noiseMap[i] + minMaxShift) / minMaxRange * 255
                );

            if (!mask[y][x])
            {
                n = 0;
            }

            auto roll = dist(rng);
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
                    else
                    {
                        addDirt(resultRow);
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
                else
                {
                    addDirt(resultRow);
                }
            }
            else if (n >= 115)
            {
                double range = 150 - 115;
                double shifted = n - 115;
                double scaled = shifted / range * 100;
                if (scaled > roll)
                {
                    if (roll > 25)
                    {
                        addTree(resultRow);
                    }
                    else
                    {
                        addDirt(resultRow);
                    }
                }
                else
                {
                    addGrass(resultRow);
                }
            }
            else if (n >= 75)
            {
                if (roll > 15)
                {
                    addGrass(resultRow);
                }
                else
                {
                    addTree(resultRow);
                }
            }
            else if (n >= 20)
            {
                if (roll > 5)
                {
                    addSand(resultRow);
                }
                else
                {
                    addTree(resultRow);
                }
            }
            else if (n >= 1)
            {
                if (roll > 10)
                {
                    addMarsh(resultRow);
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

    delete[] noiseMap;

    return result;
}
