#include "ComponentTradeable.h"

#include "GameItem.h"

std::string const ComponentTradeable::PermanentId = "tradeable";

unsigned int ComponentTradeable::value (
    GameItem const * gameItem,
    unsigned int defaultValue) const
{
    return getChainedValue<unsigned int, int>(gameItem, "value", defaultValue);
}

void ComponentTradeable::setValue (
    GameItem * gameItem,
    unsigned int value)
{
    setDirectValue<unsigned int, int>(gameItem, "value", value);
}

std::unique_ptr<Component> ComponentTradeable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentTradeable(id));
}
