#include "ComponentIdentifiable.h"

#include "GameItem.h"
#include "GameItemRegistry.h"

std::string const ComponentIdentifiable::PermanentId = "identifiable";

int ComponentIdentifiable::shortcutId (
    GameItem const * gameItem,
    int defaultValue) const
{
    return getDirectValue<int, int>(gameItem, "shortcutId", defaultValue);
}

void ComponentIdentifiable::setShortcutId (
    GameItem * gameItem,
    int value)
{
    setDirectValue<int, int>(gameItem, "shortcutId", value);
}

bool ComponentIdentifiable::isCountable (
    GameItem const * gameItem,
    bool defaultValue) const
{
    return getChainedValue<bool, bool>(gameItem, "isCountable", defaultValue);
}

void ComponentIdentifiable::setIsCountable (
    GameItem * gameItem,
    bool value)
{
    setDirectValue<bool, bool>(gameItem, "isCountable", value);
}

unsigned int ComponentIdentifiable::count (
    GameItem const * gameItem,
    unsigned int defaultValue) const
{
    return getChainedValue<unsigned int, int>(gameItem, "count", defaultValue);
}

void ComponentIdentifiable::setCount (
    GameItem * gameItem,
    unsigned int value)
{
    setDirectValue<unsigned int, int>(gameItem, "count", value);
}

std::string ComponentIdentifiable::name (
    GameItem const * gameItem,
    std::string const & defaultValue) const
{
    return getRegisteredValue<std::string, std::string>(gameItem, "name", defaultValue);
}

void ComponentIdentifiable::setName (
    GameItem * gameItem,
    std::string const & value)
{
    setRegisteredValue<std::string, std::string>(gameItem, "name", value);
}

std::unique_ptr<Component> ComponentIdentifiable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentIdentifiable(id));
}
