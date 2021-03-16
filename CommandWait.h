#ifndef COMMANDWAIT_H
#define COMMANDWAIT_H

#include "Command.h"

#include <memory>
#include <optional>
#include <string>

class CommandWait : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandWait ();
};

#endif // COMMANDWAIT_H
