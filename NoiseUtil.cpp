#include "NoiseUtil.h"

constexpr double sqrt2d2 = 0.70710678118654752440084436210484904; // sqrt(2) / 2
constexpr double sqrt3d2 = 0.86602540378443864676372317075293618; // sqrt(3) / 2

// With one dimension, we can only define 2 unit-vectors (1 and -1) and that's
// not enough. So one dimension doesn't use unit-length vectors. Three gradients
// have been omitted to give 13 which seems to help prevent cycles from
// appearing in the output.
double gradients1[] =
{
    -8.0, -7.0, -5.0, -4.0, -3.0, -2.0, -1.0,
     7.0,  6.0,  5.0,  4.0,  3.0,  2.0
};

// With two dimensions, we can only define 4 unit-vectors if we limit the x
// and y values to be just 0, 1, and -1. That's not enough either.
//  1,  0
//  0,  1
// -1,  0
// -1, -1
// But unlike one dimension, we can use any point on a circle in two dimensions.
// This array uses the sqrt values above to make a total of 16 unit-vectors.
// Three vectors have been commented out to give 13 which seems to help prevent
// cycles from appearing in the output.
double unitVectors2[][2] =
{
    {-0.5,      sqrt3d2},
    { sqrt3d2,  0.5},
    { 0.5,      sqrt3d2},
    //{-sqrt2d2,  sqrt2d2},
    {-sqrt3d2,  0.5},
    //{ 1.0,      0.0},
    { sqrt2d2,  sqrt2d2},
    { 0.0,      1.0},
    { 0.5,     -sqrt3d2},
    {-1.0,      0.0},
    //{-sqrt3d2, -0.5},
    {-0.5,     -sqrt3d2},
    { sqrt2d2, -sqrt2d2},
    { 0.0,     -1.0},
    { sqrt3d2, -0.5},
    {-sqrt2d2, -sqrt2d2}
};
