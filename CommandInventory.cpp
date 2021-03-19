#include "CommandInventory.h"

#include "Game.h"
#include "GameStateInventory.h"

CommandInventory::CommandInventory (
    std::optional<int> characterId)
: mCharacterId(characterId)
{ }

std::unique_ptr<Command> CommandInventory::tryCreate (
    std::string const & inputString)
{
    if (inputString.empty() || inputString.size() > 2)
    {
        return nullptr;
    }

    std::optional<int> characterId;
    if (inputString.size() == 2)
    {
        if (inputString[0] < '0' || inputString[0] > '9')
        {
            return nullptr;
        }

        if (inputString[1] != 'I')
        {
            return nullptr;
        }

        characterId = std::stoi(std::string(1, inputString[0]));
    }
    else if (inputString[0] != 'I')
    {
        return nullptr;
    }

    return std::unique_ptr<Command>(
        new CommandInventory(characterId));
}

GameState::StateAction CommandInventory::execute (
    Game * game) const
{
    std::optional<int> characterId = mCharacterId ? mCharacterId :
        game->defaultCharacterId();
    if (!characterId)
    {
        return GameState::Keep {};
    }

    auto character = game->findCharacter(characterId.value());
    if (character == nullptr)
    {
        return GameState::Keep {};
    }

    return GameState::Push {
        std::unique_ptr<GameState>(new GameStateInventory(
            game,
            characterId.value()))
        };
}
