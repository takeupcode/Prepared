#include "CommandMove.h"

#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentLocateable.h"
#include "ComponentRegistry.h"
#include "Display.h"
#include "Game.h"

Point calculateProposedLocation (Point current,
    Direction direction,
    int distance = 1)
{
    switch (direction)
    {
    case Direction::North:
        current.y -= distance;
        break;

    case Direction::South:
        current.y += distance;
        break;

    case Direction::West:
        current.x -= distance;
        break;

    case Direction::East:
        current.x += distance;
        break;

    default:
        break;
    }

    return current;
}

CommandMove::CommandMove (
    Direction direction,
    bool turn,
    std::optional<int> characterId)
: mDirection(direction),
  mTurn(turn),
  mCharacterId(characterId)
{ }

std::unique_ptr<Command> CommandMove::tryCreate (
    std::string const & inputString)
{
    if (inputString.empty() || inputString.size() > 3)
    {
        return nullptr;
    }

    std::optional<int> characterId;
    char directionChar;
    bool turn = false;
    if (inputString.size() == 3)
    {
        if (inputString[2] != 'T')
        {
            return nullptr;
        }

        if (inputString[0] < '0' || inputString[0] > '9')
        {
            return nullptr;
        }
        characterId = std::stoi(std::string(1, inputString[0]));
        directionChar = inputString[1];
        turn = true;
    }
    else if (inputString.size() == 2)
    {
        if (inputString[1] == 'T')
        {
            directionChar = inputString[0];
            turn = true;
        }
        else
        {
            if (inputString[0] < '0' || inputString[0] > '9')
            {
                return nullptr;
            }
            characterId = std::stoi(std::string(1, inputString[0]));
            directionChar = inputString[1];
        }
    }
    else
    {
        directionChar = inputString[0];
    }

    Direction direction;
    switch (directionChar)
    {
    case 'A':
        direction = Direction::West;
        break;

    case 'D':
        direction = Direction::East;
        break;

    case 'S':
        direction = Direction::South;
        break;

    case 'W':
        direction = Direction::North;
        break;

    default:
        return nullptr;
    }

    return std::unique_ptr<Command>(
        new CommandMove(direction, turn, characterId));
}

GameState::StateAction CommandMove::execute (Game * game) const
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

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto locateable = ComponentRegistry::find<ComponentLocateable>();

    auto layerId = layer->layerId(character);
    auto currentLocation = locateable->location(character);
    auto proposedLocation = calculateProposedLocation(currentLocation, mDirection);
    auto moveLocation = game->level()->calculateMoveLocation(
        currentLocation, proposedLocation, layerId);

    if (currentLocation != moveLocation)
    {
        locateable->setLocation(character, moveLocation);

        game->display()->ensureVisibleInMap(
            moveLocation,
            game->level()->width(),
            game->level()->height());

        int instanceId = identifiable->instanceId(character);
        game->addEvent(CharacterMoved {instanceId, currentLocation, moveLocation});
    }

    return GameState::Keep {};
}
