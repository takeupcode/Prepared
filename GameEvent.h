#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include "Point.h"

#include <variant>

struct CharacterSpawned
{
    int characterId;
};

struct CreatureSpawned
{
    int creatureId;
};

struct CharacterMoved
{
    int characterId;
};

struct CharacterHit
{
    int characterId;
    int damage;
};

struct CreatureHit
{
    int creatureId;
    int damage;
};

using GameEvent = std::variant<
    CharacterSpawned,
    CreatureSpawned,
    CharacterMoved,
    CharacterHit,
    CreatureHit
>;

#endif // GAMEEVENT_H
