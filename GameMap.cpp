#include "GameMap.h"

#include "Lerp.h"
#include "Math.h"
#include "Noise.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

void calculateQuadrantOutline (
    Noise const & noise,
    double targetRadius,
    unsigned int layers,
    std::vector<std::vector<bool>> & quadrant,
    double startingDegrees,
    bool anglesIncreaseFromStart,
    int & xMax,
    int & yMax
    )
{
    const int cycleDivisor = 40;
    double noiseValue = 0;
    int y = 0;
    int x = 0;
    while (true)
    {
        auto & newRow = quadrant.emplace_back();

        if (y == 0)
        {
            noiseValue = targetRadius + noise.generate(
                startingDegrees / cycleDivisor,
                layers,
                360 / cycleDivisor);

            // The floor is just inside the outline.
            x = dtoiflr(noiseValue);

            // We'll assume that noise cannot be so deep that it
            // crosses into other quadrants from a higher angle.
            if (x < 0)
            {
                x = 0;
            }

            // Increment to get outside the outline.
            ++x;
            xMax = x;

            std::fill_n(std::back_inserter(newRow), xMax + 1, true);
            newRow[x] = false;
        }
        else
        {
            // We'll be done with this quadrant when we can go an
            // entire row with all the points being outside the
            // outline.
            bool done = true;
            std::fill_n(std::back_inserter(newRow), xMax + 1, true);

            // If the last point is still inside the outline, then we
            // need to start increasing the width. This will affect
            // the higher rows by increasing xMax.
            bool lastPointInside = false;

            x = 0;
            while (x <= xMax)
            {
                double angle = ptoa({x, y});
                if (anglesIncreaseFromStart)
                {
                    angle = startingDegrees + angle;
                }
                else
                {
                    angle = startingDegrees - angle;
                }
                noiseValue = targetRadius + noise.generate(
                    angle / cycleDivisor,
                    layers,
                    360 / cycleDivisor);
                noiseValue *= noiseValue;

                double pointDistanceSquared = x * x + y * y;
                if (pointDistanceSquared > noiseValue)
                {
                    newRow[x] = false;
                    lastPointInside = false;
                }
                else
                {
                    done = false;
                    lastPointInside = true;
                }

                ++x;
            }

            if (lastPointInside)
            {
                while (true)
                {
                    ++xMax;
                    newRow.push_back(true);

                    double angle = ptoa({x, y});
                    if (anglesIncreaseFromStart)
                    {
                        angle = startingDegrees + angle;
                    }
                    else
                    {
                        angle = startingDegrees - angle;
                    }
                    noiseValue = targetRadius + noise.generate(
                        angle / cycleDivisor,
                        layers,
                        360 / cycleDivisor);
                    noiseValue *= noiseValue;

                    double pointDistanceSquared = x * x + y * y;
                    if (pointDistanceSquared > noiseValue)
                    {
                        newRow[x] = false;
                        break;
                    }

                    ++x;
                }
            }

            if (done)
            {
                break;
            }
        }
        ++y;
    }

    yMax = y;
}

std::vector<std::vector<bool>> createOutline (int seed)
{
    Noise noise(seed, 1, 4);
    seed = noise.seed();

    const double targetRadius = 25;
    std::vector<std::vector<bool>> quadrant1;
    std::vector<std::vector<bool>> quadrant2;
    std::vector<std::vector<bool>> quadrant3;
    std::vector<std::vector<bool>> quadrant4;
    unsigned int layers = 2;

    int q1xMax = 0;
    int q1yMax = 0;
    int q2xMax = 0;
    int q2yMax = 0;
    int q3xMax = 0;
    int q3yMax = 0;
    int q4xMax = 0;
    int q4yMax = 0;

    calculateQuadrantOutline(noise, targetRadius, layers,
        quadrant1, 0.0, true, q1xMax, q1yMax);

    calculateQuadrantOutline(noise, targetRadius, layers,
        quadrant2, 180.0, false, q2xMax, q2yMax);

    calculateQuadrantOutline(noise, targetRadius, layers,
        quadrant3, 180.0, true, q3xMax, q3yMax);

    calculateQuadrantOutline(noise, targetRadius, layers,
        quadrant4, 360.0, false, q4xMax, q4yMax);

    int leftxMax = std::max(q2xMax, q3xMax);
    int rightxMax = std::max(q1xMax, q4xMax);
    int topyMax = std::max(q1yMax, q2yMax);
    int bottomyMax = std::max(q3yMax, q4yMax);

    int width = leftxMax + rightxMax + 1;
    int height = topyMax + bottomyMax + 1;

    std::vector<std::vector<bool>> result;

    std::string fileName = "./outline";
    fileName += std::to_string(seed);
    fileName += ".ppm";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    // Q1 and Q2 both print the high y values first.
    for (int y = topyMax; y >= 0; --y)
    {
        auto & resultRow = result.emplace_back();

        // Q2 x values are backwards. We print the high x values first.
        if (y > q2yMax)
        {
            for (int x = leftxMax; x >= 0; --x)
            {
                unsigned char n = 0;
                ofs << n << n << n;

                resultRow.push_back(false);
            }
        }
        else
        {
            for (int x = leftxMax; x >= 0; --x)
            {
                bool resultValue = false;
                unsigned char n = 0;
                // Some rows could have fewer values.
                if (quadrant2[y].size() > static_cast<unsigned int>(x) &&
                    quadrant2[y][x])
                {
                    n = 255;
                    resultValue = true;
                }
                ofs << n << n << n;

                resultRow.push_back(resultValue);
            }
        }

        // Q1 x values are correct. We print the low x values first.
        // But we skip x == 0 because we already printed it from Q2.
        // Q1 and Q2 overlap when x == 0.
        if (y > q1yMax)
        {
            for (int x = 1; x <= rightxMax; ++x)
            {
                unsigned char n = 0;
                ofs << n << n << n;

                resultRow.push_back(false);
            }
        }
        else
        {
            for (int x = 1; x <= rightxMax; ++x)
            {
                bool resultValue = false;
                unsigned char n = 0;
                // Some rows could have fewer values.
                if (quadrant1[y].size() > static_cast<unsigned int>(x) &&
                    quadrant1[y][x])
                {
                    n = 255;
                    resultValue = true;
                }
                ofs << n << n << n;

                resultRow.push_back(resultValue);
            }
        }
    }

    // Q3 and Q4 both print the low y values first. But we skip
    // the points when y == 0 because Q3 and Q4 overlap with Q1
    // and Q2 when y == 0.
    for (int y = 1; y <= bottomyMax; ++y)
    {
        auto & resultRow = result.emplace_back();

        // Q3 x values are backwards. We print the high x values first.
        if (y > q3yMax)
        {
            for (int x = leftxMax; x >= 0; --x)
            {
                unsigned char n = 0;
                ofs << n << n << n;

                resultRow.push_back(false);
            }
        }
        else
        {
            for (int x = leftxMax; x >= 0; --x)
            {
                bool resultValue = false;
                unsigned char n = 0;
                // Some rows could have fewer values.
                if (quadrant3[y].size() > static_cast<unsigned int>(x) &&
                    quadrant3[y][x])
                {
                    n = 255;
                    resultValue = true;
                }
                ofs << n << n << n;

                resultRow.push_back(resultValue);
            }
        }

        // Q4 x values are correct. We print the low x values first.
        // But we skip x == 0 because we already printed it from Q3.
        // Q3 and Q4 overlap when x == 0.
        if (y > q4yMax)
        {
            for (int x = 1; x <= rightxMax; ++x)
            {
                unsigned char n = 0;
                ofs << n << n << n;

                resultRow.push_back(false);
            }
        }
        else
        {
            for (int x = 1; x <= rightxMax; ++x)
            {
                bool resultValue = false;
                unsigned char n = 0;
                // Some rows could have fewer values.
                if (quadrant4[y].size() > static_cast<unsigned int>(x) &&
                    quadrant4[y][x])
                {
                    n = 255;
                    resultValue = true;
                }
                ofs << n << n << n;

                resultRow.push_back(resultValue);
            }
        }
    }

    ofs.close();
    return result;
}

std::vector<std::vector<GameMap::Terrain>> GameMap::create ()
{
    Noise noise;
    int seed = noise.seed();

    std::vector<std::vector<bool>> outline = createOutline (seed);

    const unsigned int height = outline.size();

    // All the rows in the outline are the same width. Use the
    // first to get the width.
    const unsigned int width = outline[0].size();

    double *noiseMap = new double[width * height];
    unsigned int layers = 1;

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
            else if (noiseValue > max)
            {
                max = noiseValue;
            }
        }
    }

    double minMaxShift = -min;
    double minMaxRange = max - min;
    if (minMaxRange == 0)
    {
        minMaxRange = 1;
    }

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

            if (!outline[y][x])
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

                resultRow.push_back(Terrain::Trees);
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
