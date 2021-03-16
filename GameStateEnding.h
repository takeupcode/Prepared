#ifndef GAMESTATEENDING_H
#define GAMESTATEENDING_H

#include "GameState.h"

class GameStateEnding : public GameState
{
public:
    GameStateEnding (Game * game);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }

private:
    bool mGameOver;
};

#endif // GAMESTATEENDING_H
