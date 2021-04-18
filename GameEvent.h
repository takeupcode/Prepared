#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include "Direction.h"
#include "Point.h"

#include <variant>

struct ComponentAdded
{
    int itemInstanceId;
    int compId;
};

struct ComponentRemoved
{
    int itemInstanceId;
    int compId;
};

struct GameItemAttack
{
    int itemInstanceId;
};

struct GameItemControlDirection
{
    int itemInstanceId;
    Direction direction;
};

struct GameItemControlMovement
{
    int itemInstanceId;
    Direction direction;
};

struct GameItemControlSelected
{
    int itemInstanceId;
};

struct GameItemDamaged
{
    int itemInstanceId;
    int damage;
};

struct GameItemMoved
{
    int itemInstanceId;
};

struct GameItemRemoved
{
    int itemInstanceId;
};

struct GameItemSpawned
{
    int itemInstanceId;
};

using GameEvent = std::variant<
    ComponentAdded,
    ComponentRemoved,
    GameItemAttack,
    GameItemControlDirection,
    GameItemControlMovement,
    GameItemControlSelected,
    GameItemDamaged,
    GameItemMoved,
    GameItemRemoved,
    GameItemSpawned
>;

#endif // GAMEEVENT_H
