#include "ComponentLocateable.h"

#include "GameItem.h"

std::string const ComponentLocateable::PermanentId = "locateable";

Point ComponentLocateable::location (
    GameItem const * gameItem,
    Point const & defaultValue) const
{
    Point result = defaultValue;

    result.x = getDirectValue<int, int>(gameItem, "x", defaultValue.x);
    result.y = getDirectValue<int, int>(gameItem, "y", defaultValue.y);

    return result;
}

void ComponentLocateable::setLocation (
    GameItem * gameItem,
    Point const & value)
{
    setDirectValue<int, int>(gameItem, "x", value.x);
    setDirectValue<int, int>(gameItem, "y", value.y);
}

Direction ComponentLocateable::direction (
    GameItem const * gameItem,
    Direction defaultValue) const
{
    Direction result = defaultValue;

    result = getDirectValue<Direction, int>(gameItem, "direction", defaultValue);

    return result;
}

void ComponentLocateable::setDirection (
    GameItem * gameItem,
    Direction value)
{
    setDirectValue<Direction, int>(gameItem, "direction", value);
}

std::unique_ptr<Component> ComponentLocateable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentLocateable(id));
}
