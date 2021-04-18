#ifndef GAMESTATEEXPLORING_H
#define GAMESTATEEXPLORING_H

#include "GameEvent.h"
#include "GameState.h"

class GameStateExploring : public GameState
{
public:
    GameStateExploring (Game * game);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    void operator () (
        GameItemMoved const & moved) const;

    void operator () (
        GameItemDamaged const & damaged) const;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }
};

#endif // GAMESTATEEXPLORING_H
