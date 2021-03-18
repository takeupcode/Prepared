#ifndef GAMESTATESTARTING_H
#define GAMESTATESTARTING_H

#include "GameEvent.h"
#include "GameItem.h"
#include "GameState.h"

#include <vector>

class GameStateStarting : public GameState
{
public:
    GameStateStarting (Game * game);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }

private:
    GameItem createCharacter (char symbol) const;

    std::vector<GameItem> mCharacters;
};

#endif // GAMESTATESTARTING_H
