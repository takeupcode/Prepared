#ifndef GAMESTATEINVENTORY_H
#define GAMESTATEINVENTORY_H

#include "GameState.h"
#include "Resource.h"

#include <vector>

class GameStateInventory : public GameState
{
public:
    GameStateInventory (
        Game * game,
        int characterId);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }

private:
    void initializeResources ();
    void handleTake ();
    void handleDrop ();
    void handleDone ();

    int mCharacterId;
    std::vector<Resource> mResourcesOwned;
    std::vector<Resource> mResourcesAvailable;
};

#endif // GAMESTATEINVENTORY_H
