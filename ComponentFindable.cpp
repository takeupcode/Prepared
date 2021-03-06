#include "ComponentFindable.h"

#include "GameItem.h"

std::string const ComponentFindable::PermanentId = "findable";

unsigned int ComponentFindable::targetCount (
    GameItem const * gameItem,
    unsigned int defaultValue) const
{
    return getChainedValue<unsigned int, int>(gameItem, "targetCount", defaultValue);
}

void ComponentFindable::setTargetCount (
    GameItem * gameItem,
    unsigned int value)
{
    setDirectValue<unsigned int, int>(gameItem, "targetCount", value);
}

unsigned int ComponentFindable::chanceOfFinding (
    GameItem const * gameItem,
    unsigned int defaultValue) const
{
    return getChainedValue<unsigned int, int>(gameItem, "chanceOfFinding", defaultValue);
}

void ComponentFindable::setChanceOfFinding (
    GameItem * gameItem,
    unsigned int value)
{
    setDirectValue<unsigned int, int>(gameItem, "chanceOfFinding", value);
}

std::unique_ptr<Component> ComponentFindable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentFindable(id));
}
