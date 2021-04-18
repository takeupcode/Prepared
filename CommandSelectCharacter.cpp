#include "CommandSelectCharacter.h"

#include "Game.h"

CommandSelectCharacter::CommandSelectCharacter (int characterId)
: mCharacterId(characterId)
{ }

std::unique_ptr<Command> CommandSelectCharacter::tryCreate (
    std::string const & inputString)
{
    if (inputString.size() != 1 ||
        inputString[0] < '1' ||
        inputString[0] > '9')
    {
        return nullptr;
    }

    return std::unique_ptr<Command>(
        new CommandSelectCharacter(stoi(inputString)));
}

GameState::StateAction CommandSelectCharacter::execute (
    Game * game) const
{
    auto character = game->findItem(mCharacterId);
    if (character == nullptr)
    {
        return GameState::Keep {};
    }

    game->setDefaultCharacterId(mCharacterId);

    return GameState::Keep {};
}
