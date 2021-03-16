#include "Resource.h"

Resource::Resource (
    std::string const & name,
    unsigned int value,
    unsigned int possible,
    unsigned int chance,
    std::optional<unsigned int> found,
    unsigned int count)
: mName(name),
  mValueOfEach(value),
  mPossibleCount(possible),
  mChanceOfFinding(chance),
  mFoundCount(found),
  mCount(count)
{ }

std::string Resource::name () const
{
    return mName;
}

unsigned int Resource::valueOfEach () const
{
    return mValueOfEach;
}

unsigned int Resource::possibleCount () const
{
    return mPossibleCount;
}

unsigned int Resource::chanceOfFinding () const
{
    return mChanceOfFinding;
}

std::optional<unsigned int> Resource::foundCount () const
{
    return mFoundCount;
}

void Resource::setFoundCount (
    std::optional<unsigned int> const & found)
{
    mFoundCount = found;
}

unsigned int Resource::count () const
{
    return mCount;
}

void Resource::setCount (unsigned int count)
{
    mCount = count;
}
