#include "CommandTurn.h"

#include "ComponentLocation.h"
#include "ComponentRegistry.h"
#include "Game.h"

CommandTurn::CommandTurn (
    Direction direction,
    std::optional<int> characterId)
: mDirection(direction), mCharacterId(characterId)
{ }

std::unique_ptr<Command> CommandTurn::tryCreate (
    std::string const & inputString)
{
    if (inputString.empty() || inputString.size() > 2)
    {
        return nullptr;
    }

    std::optional<int> characterId;
    char directionChar;
    if (inputString.size() == 2)
    {
        if (inputString[0] < '1' || inputString[0] > '9')
        {
            return nullptr;
        }
        characterId = std::stoi(std::string(1, inputString[0]));
        directionChar = inputString[1];
    }
    else
    {
        directionChar = inputString[0];
    }

    Direction direction;
    switch (directionChar)
    {
    case 'H':
        direction = Direction::West;
        break;

    case 'K':
        direction = Direction::East;
        break;

    case 'J':
        direction = Direction::South;
        break;

    case 'U':
        direction = Direction::North;
        break;

    default:
        return nullptr;
    }

    return std::unique_ptr<Command>(
        new CommandTurn(direction, characterId));
}

GameState::StateAction CommandTurn::execute (Game * game) const
{
    std::optional<int> characterId = mCharacterId ? mCharacterId :
        game->defaultCharacterId();
    if (!characterId)
    {
        return GameState::Keep {};
    }

    auto character = game->findItem(characterId.value());
    if (character == nullptr)
    {
        return GameState::Keep {};
    }

    auto location = ComponentRegistry::find<ComponentLocation>();
    location->setDirection(character, mDirection);

    return GameState::Keep {};
}
