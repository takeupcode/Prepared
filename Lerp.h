#ifndef LERP_H
#define LERP_H

template <typename N, typename T>
N lerp (N n0, N n1, T t)
{
    return static_cast<N>(n0 * (1 - t) + n1 * t);
}

#endif // LERP_H
