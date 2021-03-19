#ifndef EASING_H
#define EASING_H

// Classic Perlin continuous interpolant.
inline double easeInOutPerlin (double t)
{
    // f(t) = - 2t^3 + 3t^2
    return t * t * (t * -2 + 3);
}

#endif // EASING_H
