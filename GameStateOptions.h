#ifndef GAMESTATEOPTIONS_H
#define GAMESTATEOPTIONS_H

#include "GameState.h"

#include <vector>

class GameStateOptions : public GameState
{
public:
    GameStateOptions (Game * game);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }

private:
};

#endif // GAMESTATEOPTIONS_H
