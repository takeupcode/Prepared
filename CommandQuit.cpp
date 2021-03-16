#include "CommandQuit.h"

#include "Game.h"
#include "GameStateEnding.h"

CommandQuit::CommandQuit ()
{ }

std::unique_ptr<Command> CommandQuit::tryCreate (
    std::string const & inputString)
{
    if (inputString.size() != 1 || inputString[0] != 'Q')
    {
        return nullptr;
    }

    return std::unique_ptr<Command>(new CommandQuit());
}

GameState::StateAction CommandQuit::execute (Game * game) const
{
    return GameState::Push {std::unique_ptr<GameState>(
        new GameStateEnding(game))};
}
