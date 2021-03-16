#ifndef COMMAND_H
#define COMMAND_H

#include "GameState.h"

class Game;

class Command
{
public:
    virtual ~Command () = default;

    virtual GameState::StateAction execute (Game * game) const = 0;

protected:
    Command () = default;
};

#endif // COMMAND_H
