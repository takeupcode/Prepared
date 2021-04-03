#include "ComponentHealth.h"

#include "GameItem.h"

std::string const ComponentHealth::PermanentId = "health";

int ComponentHealth::health (
    GameItem const * gameItem,
    int defaultValue) const
{
    return getChainedValue<int, int>(gameItem, "health", defaultValue);
}

void ComponentHealth::setHealth (
    GameItem * gameItem,
    int value,
    bool delta)
{
    if (delta)
    {
        value += health(gameItem, 0);
    }
    setDirectValue<int, int>(gameItem, "health", value);
}

unsigned int ComponentHealth::maxHealth (
    GameItem const * gameItem,
    unsigned int defaultValue) const
{
    return getChainedValue<unsigned int, int>(gameItem, "maxHealth", defaultValue);
}

void ComponentHealth::setMaxHealth (
    GameItem * gameItem,
    unsigned int value,
    bool delta)
{
    if (delta)
    {
        value += maxHealth(gameItem, 0);
    }
    setDirectValue<unsigned int, int>(gameItem, "maxHealth", value);
}

bool ComponentHealth::isDead (
    GameItem const * gameItem,
    bool defaultValue) const
{
    return getChainedValue<bool, bool>(gameItem, "isDead", defaultValue);
}

void ComponentHealth::setIsDead (
    GameItem * gameItem,
    bool value)
{
    setDirectValue<bool, bool>(gameItem, "isDead", value);
}

bool ComponentHealth::isUnconscious (
    GameItem const * gameItem,
    bool defaultValue) const
{
    return getChainedValue<bool, bool>(gameItem, "isUnconscious", defaultValue);
}

void ComponentHealth::setIsUnconscious (
    GameItem * gameItem,
    bool value)
{
    setDirectValue<bool, bool>(gameItem, "isUnconscious", value);
}

std::unique_ptr<Component> ComponentHealth::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentHealth(id));
}
