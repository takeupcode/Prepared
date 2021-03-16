#ifndef GAMESTATESTARTING_H
#define GAMESTATESTARTING_H

#include "Character.h"
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
    std::vector<Character> mCharacters;
};

#endif // GAMESTATESTARTING_H
