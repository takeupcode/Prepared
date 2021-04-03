#include "CommandOptions.h"

#include "Game.h"
#include "GameStateOptions.h"

CommandOptions::CommandOptions ()
{ }

std::unique_ptr<Command> CommandOptions::tryCreate (
    std::string const & inputString)
{
    if (inputString.size() != 1 || inputString[0] != 'O')
    {
        return nullptr;
    }

    return std::unique_ptr<Command>(new CommandOptions());
}

GameState::StateAction CommandOptions::execute (Game * game) const
{
    return GameState::Push {std::unique_ptr<GameState>(
        new GameStateOptions(game))};
}
