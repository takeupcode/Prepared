#include "ComponentLocation.h"

#include "GameItem.h"

std::string const ComponentLocation::PermanentId = "location";

Point2i ComponentLocation::location (
    GameItem const * gameItem,
    Point2i const & defaultValue) const
{
    Point2i result = defaultValue;

    result.x = getDirectValue<int, int>(gameItem, "x", defaultValue.x);
    result.y = getDirectValue<int, int>(gameItem, "y", defaultValue.y);

    return result;
}

void ComponentLocation::setLocation (
    GameItem * gameItem,
    Point2i const & value)
{
    setDirectValue<int, int>(gameItem, "x", value.x);
    setDirectValue<int, int>(gameItem, "y", value.y);
}

Direction ComponentLocation::direction (
    GameItem const * gameItem,
    Direction defaultValue) const
{
    Direction result = defaultValue;

    result = getDirectValue<Direction, int>(gameItem, "direction", defaultValue);

    return result;
}

void ComponentLocation::setDirection (
    GameItem * gameItem,
    Direction value)
{
    setDirectValue<Direction, int>(gameItem, "direction", value);
}

std::unique_ptr<Component> ComponentLocation::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentLocation(id));
}
