#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include "Location.h"

#include <variant>

struct CharacterSpawned
{
    int characterId;
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
    CharacterMoved,
    CharacterHit,
    CreatureHit
>;

#endif // GAMEEVENT_H
