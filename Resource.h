#ifndef RESOURCE_H
#define RESOURCE_H

#include <optional>
#include <string>

class Resource
{
public:
    Resource (
        std::string const & name,
        unsigned int value,
        unsigned int possible,
        unsigned int chance,
        std::optional<unsigned int> found = std::nullopt,
        unsigned int count = 0);

    std::string name () const;

    unsigned int valueOfEach () const;

    unsigned int possibleCount () const;

    unsigned int chanceOfFinding () const;

    std::optional<unsigned int> foundCount () const;

    void setFoundCount (
        std::optional<unsigned int> const & found);

    unsigned int count () const;

    void setCount (unsigned int count);

private:
    std::string mName;
    unsigned int mValueOfEach;
    unsigned int mPossibleCount;
    unsigned int mChanceOfFinding;
    std::optional<unsigned int> mFoundCount;
    unsigned int mCount;
};

#endif // RESOURCE_H
