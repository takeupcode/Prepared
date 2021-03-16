#include "CommandMove.h"

#include "Character.h"
#include "Game.h"

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

    if (character->move(game->level(), mDirection))
    {
        game->addEvent(CharacterMoved {
            character->id()});
    }

    if (mTurn)
    {
        character->setDirection(mDirection);
    }

    return GameState::Keep {};
}
