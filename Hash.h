#ifndef HASH_H
#define HASH_H

#include <cstddef>

template <typename T>
inline std::size_t hashStart (T const & value)
{
    static constexpr std::size_t prime = 1099511628211ULL;
    static constexpr std::size_t offset = 14695981039346656037ULL;

    const unsigned char *bp = reinterpret_cast<const unsigned char *>(&value);
    const unsigned char *be = bp + sizeof(value);

    std::size_t result = offset * prime;

    // FNV-1a hash each octet in the buffer
    while (bp < be)
    {
        // xor the bottom with the current octet
        result ^= static_cast<std::size_t>(*bp++);

        // multiply by the 64-bit FNV magic prime mod 2^64
        result *= prime;
    }

    return result;
}

template <typename T>
inline std::size_t hashContinue (T const & value, std::size_t seed)
{
    static constexpr std::size_t prime = 1099511628211ULL;

    const unsigned char *bp = reinterpret_cast<const unsigned char *>(&value);
    const unsigned char *be = bp + sizeof(value);

    std::size_t result = seed;

    // FNV-1a hash each octet in the buffer
    while (bp < be)
    {
        // xor the bottom with the current octet
        result ^= static_cast<std::size_t>(*bp++);

        // multiply by the 64-bit FNV magic prime mod 2^64
        result *= prime;
    }

    return result;
}

#endif // HASH_H
