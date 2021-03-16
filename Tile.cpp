#include "Tile.h"

Tile::Tile (char symbol, bool obstruct)
: mSymbol(symbol), mObstruct(obstruct)
{ }

char Tile::symbol () const
{
    return mSymbol;
}

bool Tile::isObstruction () const
{
    return mObstruct;
}

std::vector<Resource> & Tile::resources ()
{
    return mResources;
}

std::vector<Resource> const & Tile::resources () const
{
    return mResources;
}
