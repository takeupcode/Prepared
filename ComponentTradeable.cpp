#include "ComponentTradeable.h"

#include "GameItem.h"

std::string const ComponentTradeable::PermanentId = "tradeable";

bool ComponentTradeable::isCoin (
    GameItem const * gameItem,
    bool defaultValue) const
{
    return getChainedValue<bool, bool>(gameItem, "isCoin", defaultValue);
}

void ComponentTradeable::setIsCoin (
    GameItem * gameItem,
    bool value)
{
    setDirectValue<bool, bool>(gameItem, "isCoin", value);
}

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
