#include "ComponentMoveable.h"

#include "GameItem.h"

std::string const ComponentMoveable::PermanentId = "moveable";

Point ComponentMoveable::location (
    GameItem const * gameItem,
    Point const & defaultValue) const
{
    Point result = defaultValue;

    result.x = getDirectValue<int, int>(gameItem, "x", defaultValue.x);
    result.y = getDirectValue<int, int>(gameItem, "y", defaultValue.y);

    return result;
}

void ComponentMoveable::setLocation (
    GameItem * gameItem,
    Point const & value)
{
    setDirectValue<int, int>(gameItem, "x", value.x);
    setDirectValue<int, int>(gameItem, "y", value.y);
}

std::unique_ptr<Component> ComponentMoveable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentMoveable(id));
}
