#ifndef NOISEUTIL_H
#define NOISEUTIL_H

// Predefined gradients for one dimension noise.
extern double gradients1[];

// Predefined unit-vector gradients for two dimension noise.
extern double unitVectors2[][2];

inline double dot (double unitVector[], double x, double y)
{
    double dotX = unitVector[0] * x;
    double dotY = unitVector[1] * y;

    return dotX + dotY;
}

// For the node methods, the hash is passed in as an int
// but since we only have a few values to choose from in
// either the gradients or the unit vectors, we cut off
// everything except the smallest byte. This is then used
// to select a gradient or a unit vector.
//
// The double values should be between 0 and 1. These
// values represent the distances between the point and
// the floor and ceiling of that point.
inline double node (int hash, double x)
{
    hash &= 0xff;
    return gradients1[hash % 13] * x;
}

inline double node (int hash, double x, double y)
{
    hash &= 0xff;
    return dot(unitVectors2[hash % 13], x, y);
}

#endif // NOISEUTIL_H
