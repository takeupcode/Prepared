#ifndef LERP_H
#define LERP_H

template <typename N, typename T>
N lerp (N n0, N n1, T t)
{
    return static_cast<N>(n0 * (1 - t) + n1 * t);
}

// range should be = n1 - n0
template <typename N, typename T>
N lerpBack (N n0, N range, T t)
{
    return static_cast<N>((t - n0) / range);
}

#endif // LERP_H
