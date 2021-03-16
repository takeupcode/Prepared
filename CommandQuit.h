#ifndef COMMANDQUIT_H
#define COMMANDQUIT_H

#include "Command.h"

#include <memory>
#include <string>

class CommandQuit : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandQuit ();
};

#endif // COMMANDQUIT_H
