#ifndef HASH_H
#define HASH_H

inline int hash (int value)
{
    static constexpr unsigned int prime = 16777619u;
    static constexpr unsigned int offset = 2166136261u;

    const unsigned char *bp = reinterpret_cast<const unsigned char *>(&value);
    const unsigned char *be = bp + 4;

    int result = offset * prime;

    // FNV-1a hash each octet in the buffer
    while (bp < be)
    {
        // xor the bottom with the current octet
        result ^= static_cast<int>(*bp++);

        // multiply by the 32 bit FNV magic prime mod 2^32
        result *= prime;
    }

    return result;
}

#endif // HASH_H
