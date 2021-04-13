#include "Noise.h"

#include "Easing.h"
#include "Hash.h"
#include "Math.h"
#include "NoiseUtil.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <random>

Noise::Noise (
    int seed,
    double frequency,
    double amplitude,
    double lacunarity,
    double persistence)
: mSeed(seed),
  mFrequency(frequency),
  mAmplitude(amplitude),
  mLacunarity(lacunarity),
  mPersistence(persistence)
{
    if (mSeed == 0)
    {
        mSeed = std::chrono::system_clock::now().
            time_since_epoch().count();
    }
}

int Noise::seed () const
{
    return mSeed;
}

double Noise::generate (double x, unsigned int layers,
    double cycle) const
{
    std::mt19937 rng;
    rng.seed(mSeed);
    // An int distribution seems to be more reliable.
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    double frequency = mFrequency;
    double amplitude = mAmplitude;
    double originalX = x;
    double noise = 0.0;

    for (unsigned int currentLayer = 0; currentLayer < layers; ++currentLayer)
    {
        x = originalX * frequency + dist(rng);
        if (cycle != 0)
        {
            x = std::fmod(x, cycle);
        }

        // First, calculate the nearest whole grid coordinates next to each
        // coordinate of the noise point.
        int ix0 = dtoiflr(x);
        int ix1 = ix0 + 1;

        // Then calculate individual dimension vectors to the noise point
        // from each grid coordinate.
        double x0 = x - ix0;
        double x1 = x - ix1;

        // Define and calculate smooth blends starting with the letter s.
        // Only one blend is needed for each dimension.
        double s = easeInOutPerlin(x0);

        // Get hashes for each grid point using the grid coordinates. One dimension is
        // easier because we can combine the hash of the dimension and the hash of the
        // grid point because they're the same.
        int hp0 = static_cast<int>(hashStart(ix0));
        int hp1 = static_cast<int>(hashStart(ix1));

        // Define and calculate nodes based on the grid points.
        double a, b;
        a = node(hp0, x0);
        b = node(hp1, x1);

        // lerp is a method to interpolate a value between two nodes, n1 and n2,
        // where r is used to define a scale of 0 to 1 that specifies how far
        // between n1 and n2 to go. When r = 0, then lerp returns n1. When r = 1,
        // then lerp returns n2. And other values of r will mix n1 and n2 together.
        //   lerp (n1, n2, r) = (n1 * (1 - r) + n2 * r)
        // By using easeInOut values for r, we get a smooth curve. We're not calling
        // lerp directly. Instead, we expand the formulas so that we can calculate
        // the various constants. This will help later if we ever want to perform
        // additional calculations.
        //
        // This method returns:
        //   lerp(a, b, s)
        // which substitutes to create:
        //   a(1 - s) + bs
        // which becomes:
        //   a - as + bs
        // and after regrouping:
        // ==>  a + s(b - a)

        // Define and calculate the constants needed.
        double c0 = b - a;

        noise += amplitude * (a + s * c0);

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return noise;
}

double Noise::generate (double x, double y, unsigned int layers) const
{
    std::mt19937 rng;
    rng.seed(mSeed);
    // An int distribution seems to be more reliable.
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

    double frequency = mFrequency;
    double amplitude = mAmplitude;
    double originalX = x;
    double originalY = y;
    double noise = 0.0;

    for (unsigned int currentLayer = 0; currentLayer < layers; ++currentLayer)
    {
        x = originalX * frequency + dist(rng);
        y = originalY * frequency + dist(rng);

        // First, calculate the nearest whole grid coordinates next to each
        // coordinate of the noise point.
        int ix0 = dtoiflr(x);
        int ix1 = ix0 + 1;
        int iy0 = dtoiflr(y);
        int iy1 = iy0 + 1;

        // Then calculate individual dimension vectors to the noise point
        // from each grid coordinate.
        double x0 = x - ix0;
        double x1 = x - ix1;
        double y0 = y - iy0;
        double y1 = y - iy1;

        // Define and calculate smooth blends starting with the letter s.
        // Only one blend is needed for each dimension.
        double s = easeInOutPerlin(x0);
        double t = easeInOutPerlin(y0);

        // Get hashes for each grid point dimension.
        int hx0 = static_cast<int>(hashStart(ix0));
        int hx1 = static_cast<int>(hashStart(ix1));
        int hy0 = static_cast<int>(hashStart(iy0));
        int hy1 = static_cast<int>(hashStart(iy1));

        // Get hashes for each grid point using the grid coordinates.
        int hp00 = hx0 + hy0;
        int hp10 = hx1 + hy0;
        int hp01 = hx0 + hy1;
        int hp11 = hx1 + hy1;

        // Define and calculate nodes based on the grid points.
        double a, b, c, d;
        a = node(hp00, x0, y0);
        b = node(hp10, x1, y0);
        c = node(hp01, x0, y1);
        d = node(hp11, x1, y1);

        // lerp is a method to interpolate a value between two nodes, n1 and n2,
        // where r is used to define a scale of 0 to 1 that specifies how far
        // between n1 and n2 to go. When r = 0, then lerp returns n1. When r = 1,
        // then lerp return s n2. And other values of r will mix n1 and n2 together.
        //   lerp (n1, n2, r) = (n1 * (1 - r) + n2 * r)
        // By using easeInOut values for r, we get a smooth curve. We're not calling
        // lerp directly. Instead, we expand the formulas so that we can calculate
        // the various constants. This will help later if we ever want to perform
        // additional calculations.
        //
        // This method returns:
        //   lerp(
        //     lerp(a, b, s),
        //     lerp(c, d, s),
        //     t)
        // which substitutes to create:
        //   (a(1 - s) + bs))(1 - t) +
        //   (c(1 - s) + ds))(t)
        // which becomes:
        //   a - as + bs - (a - as + bs)t +
        //   (c - cs + ds)t
        // which becomes:
        //   a - as + bs - at + ast - bst +
        //   ct - cst + dst
        // and after regrouping:
        // ==>  a + s(b - a) + t(c - a) + st(a + d - b - c)

        // Define and calculate the constants needed.
        double c0 = b - a;
        double c1 = c - a;
        double c2 = a + d - b - c;

        noise += amplitude * (a + s * c0 + t * c1 + s * t * c2);

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return noise;
}
