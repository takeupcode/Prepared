#include "GameMap.h"

#include "Lerp.h"
#include "Math.h"
#include "Noise.h"
#include "Point.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
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

std::vector<Point2i> getStartPoints (double start)
{
    std::vector<Point2i> result;

    // Return points in the following order where
    // start is the positive x axis zero crossing
    // and should line up between 1 and 4. Taking
    // the floor should give the point 1 x value.
    // 0 1 2
    // 3 4 5

    int startFloor = dtoiflr(start);
    result.emplace_back(startFloor - 1,  0);
    result.emplace_back(startFloor    ,  0);
    result.emplace_back(startFloor + 1,  0);
    result.emplace_back(startFloor - 1, -1);
    result.emplace_back(startFloor    , -1);
    result.emplace_back(startFloor + 1, -1);

    return result;
}

double calculateDifference (
    Noise const & noise,
    unsigned int targetRadius,
    unsigned int layers,
    Point2i const & point)
{
    // Get the center of the point as a Point2d
    Point2d calcPoint {point.x + 0.5, point.y + 0.5};
    double calcLengthSquared =
        calcPoint.x * calcPoint.x + calcPoint.y * calcPoint.y;
    double calcAngle = ptoa(calcPoint);

    double noiseRadius = getNoiseRadius(
        noise,
        targetRadius,
        layers,
        calcAngle);
    double noiseRadiusSquared = noiseRadius * noiseRadius;

    double difference =
        std::abs(noiseRadiusSquared - calcLengthSquared);

    return difference;
};

Point2i getStartPoint (
    Noise const & noise,
    unsigned int targetRadius,
    unsigned int layers,
    std::unordered_map<Point2i, double> & differences)
{
    // Begin at 0 degrees and figure out the closest point.
    double startRadius = getNoiseRadius(
        noise,
        targetRadius,
        layers,
        0.0);

    Point2i result;
    double minDiff = std::numeric_limits<double>::max();
    auto startPoints = getStartPoints(startRadius);
    for (auto const & point: startPoints)
    {
        double difference = calculateDifference(noise,
        targetRadius,
        layers,
        point);

        differences[point] = difference;
        if (difference < minDiff)
        {
            result = point;
            minDiff = difference;
        }
    }

    return result;
}

std::vector<Point2i> getPath (
    Noise const & noise,
    unsigned int targetRadius,
    unsigned int layers)
{
    std::vector<Point2i> path;
    std::unordered_set<Point2i> pathSet;
    std::unordered_map<Point2i, double> differences;
    auto startPoint = getStartPoint (
        noise,
        targetRadius,
        layers,
        differences);
    path.push_back(startPoint);
    pathSet.insert(startPoint);

    while (true)
    {
        Point2i nextPoint;
        double minDiff = std::numeric_limits<double>::max();
        int dx[] = { 0,  0,  1, -1};
        int dy[] = {-1,  1,  0,  0};
        for (unsigned int i = 0; i < 4; ++i)
        {
            Point2i point(
                path.back().x + dx[i],
                path.back().y + dy[i]);
            if (pathSet.find(point) != pathSet.end())
            {
                continue;
            }

            double difference;
            auto iter = differences.find(point);
            if (iter == differences.end())
            {
                difference = calculateDifference(
                    noise,
                    targetRadius,
                    layers,
                    point);
                differences[point] = difference;
            }
            else
            {
                difference = iter->second;
            }

            if (difference < minDiff)
            {
                nextPoint = point;
                minDiff = difference;
            }
        }
        if (nextPoint == startPoint)
        {
            break;
        }
        path.push_back(nextPoint);
        pathSet.insert(nextPoint);
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
        result[point.y + borderWidth][point.x + borderWidth] = true;
    }

    // Flood fill the outlined bools.
    std::stack<Point2i> fillPoints;
    fillPoints.emplace(width / 2, height / 2);
    result[height / 2][width / 2] = true;
    while (!fillPoints.empty())
    {
        auto point = fillPoints.top();
        fillPoints.pop();

        if (!result[point.y - 1][point.x])
        {
            fillPoints.emplace(point.y - 1, point.x);
            result[point.y - 1][point.x] = true;
        }
        if (!result[point.y + 1][point.x])
        {
            fillPoints.emplace(point.y + 1, point.x);
            result[point.y + 1][point.x] = true;
        }
        if (!result[point.y][point.x - 1])
        {
            fillPoints.emplace(point.y, point.x - 1);
            result[point.y][point.x - 1] = true;
        }
        if (!result[point.y][point.x + 1])
        {
            fillPoints.emplace(point.y, point.x + 1);
            result[point.y][point.x + 1] = true;
        }
    }

    std::string fileName = "./mask";
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
            // Some rows could have fewer values.
            if (result[y][x])
            {
                n = 255;
            }
            ofs << n << n << n;
        }
    }
    ofs.close();

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

            if (n >= 240)
            {
                ofs << static_cast<unsigned char>(255)
                    << static_cast<unsigned char>(255)
                    << static_cast<unsigned char>(255);

                resultRow.push_back(Terrain::Ice);
            }
            else if (n >= 150)
            {
                ofs << static_cast<unsigned char>(0)
                    << static_cast<unsigned char>(100)
                    << static_cast<unsigned char>(0);

                resultRow.push_back(Terrain::Tree);
            }
            else if (n >= 75)
            {
                ofs << static_cast<unsigned char>(34)
                    << static_cast<unsigned char>(139)
                    << static_cast<unsigned char>(34);

                resultRow.push_back(Terrain::Grass);
            }
            else if (n >= 20)
            {
                ofs << static_cast<unsigned char>(157)
                    << static_cast<unsigned char>(127)
                    << static_cast<unsigned char>(97);

                resultRow.push_back(Terrain::Sand);
            }
            else if (n >= 1)
            {
                ofs << static_cast<unsigned char>(0)
                    << static_cast<unsigned char>(0)
                    << static_cast<unsigned char>(160);

                resultRow.push_back(Terrain::Marsh);
            }
            else
            {
                ofs << static_cast<unsigned char>(0)
                    << static_cast<unsigned char>(0)
                    << static_cast<unsigned char>(0);

                resultRow.push_back(Terrain::Water);
            }

            ++i;
        }
    }
    ofs.close();

    delete[] noiseMap;

    return result;
}
