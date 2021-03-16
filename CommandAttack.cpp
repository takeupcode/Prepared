#include "CommandAttack.h"

#include "Character.h"
#include "Direction.h"
#include "Game.h"
#include "Location.h"

#include <vector>

CommandAttack::CommandAttack (
    std::optional<int> characterId)
: mCharacterId(characterId)
{ }

std::unique_ptr<Command> CommandAttack::tryCreate (
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

        if (inputString[1] != 'X')
        {
            return nullptr;
        }

        characterId = std::stoi(std::string(1, inputString[0]));
    }
    else
    {
        if (inputString[0] != 'X')
        {
            return nullptr;
        }
    }

    return std::unique_ptr<Command>(
        new CommandAttack(characterId));
}

std::vector<Location> attackLocations (
    Location const & location,
    Direction direction)
{
    std::vector<Location> result;

    switch (direction)
    {
    case Direction::North:
        result.push_back({
            location.x - 1,
            location.y - 1});
        result.push_back({
            location.x,
            location.y - 1});
        result.push_back({
            location.x + 1,
            location.y - 1});
        break;

    case Direction::South:
        result.push_back({
            location.x - 1,
            location.y + 1});
        result.push_back({
            location.x,
            location.y + 1});
        result.push_back({
            location.x + 1,
            location.y + 1});
        break;

    case Direction::West:
        result.push_back({
            location.x - 1,
            location.y - 1});
        result.push_back({
            location.x - 1,
            location.y});
        result.push_back({
            location.x - 1,
            location.y + 1});
        break;

    case Direction::East:
        result.push_back({
            location.x + 1,
            location.y - 1});
        result.push_back({
            location.x + 1,
            location.y});
        result.push_back({
            location.x + 1,
            location.y + 1});
        break;

    default:
        break;
    }

    return result;
}

GameState::StateAction CommandAttack::execute (Game * game) const
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

    auto attacks = attackLocations(
        character->location(),
        character->direction());

    for (auto & creature: game->creatures())
    {
        for (auto const & attack: attacks)
        {
            if (creature.location() == attack)
            {
                if (game->randomPercent() > 80)
                {
                    continue;
                }

                int damage = -character->attackDamage();
                creature.setCurrentHealth(damage);
                creature.setAttackerId(characterId);

                game->addEvent(CreatureHit {
                    creature.id(),
                    damage});
            }
        }
    }

    return GameState::Keep {};
}
