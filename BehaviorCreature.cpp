#include "BehaviorCreature.h"

#include "Character.h"
#include "Game.h"

Direction randomDirection(
    Game * game,
    Character * character)
{
    auto direction = character->direction();
    auto percent = game->randomPercent();

    if (percent > 80)
    {
        direction = Direction::North;
    }
    else if (percent > 60)
    {
        direction = Direction::South;
    }
    else if (percent > 40)
    {
        direction = Direction::West;
    }
    else if (percent > 20)
    {
        direction = Direction::East;
    }
    // The last 20% will keep the same direction.

    return direction;
}

void BehaviorCreature::onUpdate (
    Game * game,
    Character * character)
{
    Character * attacker = nullptr;
    auto attackerId = character->attackerId();
    if (attackerId)
    {
        attacker = game->findCharacter(attackerId.value());
    }

    if (attacker == nullptr || !attacker->isConscious())
    {
        auto direction = randomDirection(game, character);

        character->setDirection(direction);
        if (!character->move(game->level(), direction))
        {
            // Try again. We might try the same direction
            // and that's okay.
            direction = randomDirection(game, character);
            character->setDirection(direction);
            character->move(game->level(), direction);
        }
    }
    else
    {
        auto attackerLocation = attacker->location();
        auto characterLocation = character->location();

        int xDelta = attackerLocation.x - characterLocation.x;
        int yDelta = attackerLocation.y - characterLocation.y;

        Direction direction;
        if (xDelta > 1 || xDelta < -1)
        {
            // We're too far away. Need to move.
            direction = (xDelta > 0) ?
                Direction::East :
                Direction::West;

            if (!character->move(game->level(), direction))
            {
                // If we can't move in the intended direction,
                // then try going north.
                character->move(game->level(), Direction::North);
            }
        }
        else if (yDelta > 1 || yDelta < -1)
        {
            // We're too far away. Need to move.
            direction = (yDelta > 0) ?
                Direction::South :
                Direction::North;
            character->move(game->level(), direction);

            if (!character->move(game->level(), direction))
            {
                // If we can't move in the intended direction,
                // then try going west.
                character->move(game->level(), Direction::West);
            }
        }
        else
        {
            // There's no need to move. But we might
            // be facing the wrong direction. The first
            // two cases handle direct alignment and
            // the direction to face is clear.
            if (xDelta == 0)
            {
                direction = (yDelta > 0) ?
                    Direction::South :
                    Direction::North;
            }
            else if (yDelta == 0)
            {
                direction = (xDelta > 0) ?
                    Direction::East :
                    Direction::West;
            }
            else
            {
                // This one is not as clear because
                // each remaining location has two
                // directions to face that can both
                // make sense. We'll base the choice
                // on the current direction unless
                // the character is facing away. If
                // the character is facing away, then
                // it will be turned around.
                Direction currentDirection = character->direction();
                if (xDelta > 0 && yDelta > 0)
                {
                    // The attacker is to the southeast.
                    switch (currentDirection)
                    {
                    case Direction::North:
                        direction = Direction::South;
                        break;

                    case Direction::West:
                        direction = Direction::East;
                        break;

                    default:
                        direction = currentDirection;
                        break;
                    }
                }
                else if (xDelta < 0 && yDelta > 0)
                {
                    // The attacker is to the southwest.
                    switch (currentDirection)
                    {
                    case Direction::North:
                        direction = Direction::South;
                        break;

                    case Direction::East:
                        direction = Direction::West;
                        break;

                    default:
                        direction = currentDirection;
                        break;
                    }
                }
                else if (xDelta < 0 && yDelta < 0)
                {
                    // The attacker is to the northwest.
                    switch (currentDirection)
                    {
                    case Direction::South:
                        direction = Direction::North;
                        break;

                    case Direction::East:
                        direction = Direction::West;
                        break;

                    default:
                        direction = currentDirection;
                        break;
                    }
                }
                else if (xDelta > 0 && yDelta < 0)
                {
                    // The attacker is to the northeast.
                    switch (currentDirection)
                    {
                    case Direction::South:
                        direction = Direction::North;
                        break;

                    case Direction::West:
                        direction = Direction::East;
                        break;

                    default:
                        direction = currentDirection;
                        break;
                    }
                }
            }

            // Now that we know what direction to face, turn
            // in that direction and attack.
            character->setDirection(direction);

            if (game->randomPercent() <= 20)
            {
                int damage = -character->attackDamage();
                attacker->setCurrentHealth(damage);
                attacker->setAttackerId(character->id());

                game->addEvent(CharacterHit {
                    attacker->id(),
                    damage});
            }
        }
    }
}
