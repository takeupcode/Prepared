#include "ComponentEdible.h"

#include "GameItem.h"
#include "GameItemRegistry.h"

std::string const ComponentEdible::PermanentId = "edible";

bool ComponentEdible::isEdible (
    GameItem const * gameItem) const
{
    auto registeredItem = GameItemRegistry::find(gameItem->id());
    return isValidGameItem(registeredItem);
}

int ComponentEdible::nutrition (
    GameItem const * gameItem,
    int defaultValue) const
{
    return getRegisteredValue<int, int>(gameItem, "nutrition", defaultValue);
}

void ComponentEdible::setNutrition (
    GameItem * gameItem,
    int value)
{
    setRegisteredValue<int, int>(gameItem, "nutrition", value);
}

std::unique_ptr<Component> ComponentEdible::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentEdible(id));
}
