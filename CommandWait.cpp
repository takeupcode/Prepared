#include "CommandWait.h"

CommandWait::CommandWait ()
{ }

std::unique_ptr<Command> CommandWait::tryCreate (
    std::string const & inputString)
{
    if (inputString.size() != 1 || inputString[0] != 'Z')
    {
        return nullptr;
    }

    return std::unique_ptr<Command>(new CommandWait());
}

GameState::StateAction CommandWait::execute (Game * game) const
{
    return GameState::Keep {};
}
