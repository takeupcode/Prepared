#ifndef COMMANDOPTIONS_H
#define COMMANDOPTIONS_H

#include "Command.h"

#include <memory>
#include <string>

class CommandOptions : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandOptions ();
};

#endif // COMMANDOPTIONS_H
