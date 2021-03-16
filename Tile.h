#ifndef TILE_H
#define TILE_H

#include "Resource.h"

#include <vector>

class Tile
{
public:
    Tile (char symbol, bool obstruct);

    char symbol () const;

    bool isObstruction () const;

    std::vector<Resource> & resources ();
    std::vector<Resource> const & resources () const;

private:
    char mSymbol;
    bool mObstruct;
    std::vector<Resource> mResources;
};

#endif // TILE_H
