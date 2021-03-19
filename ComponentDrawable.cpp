#include "ComponentDrawable.h"

#include "ComponentIdentifiable.h"
#include "ComponentMoveable.h"
#include "ComponentRegistry.h"
#include "Display.h"
#include "Game.h"
#include "GameItem.h"

std::string const ComponentDrawable::PermanentId = "drawable";

void ComponentDrawable::draw (GameItem const * gameItem, Display * display) const
{
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    auto moveable = ComponentRegistry::find<ComponentMoveable>();
    Point location = moveable->location(gameItem);

    char symbol = ComponentDrawable::symbol(gameItem);

    bool isImportant = false;
    auto defaultCharacterId = display->game()->defaultCharacterId();
    if (defaultCharacterId)
    {
        if (defaultCharacterId.value() < ComponentIdentifiable::FirstInstanceId)
        {
            int shortcutId = identifiable->shortcutId(gameItem);
            isImportant = defaultCharacterId.value() == shortcutId;
        }
        else
        {
            int instanceId = identifiable->instanceId(gameItem);
            isImportant = defaultCharacterId.value() == instanceId;
        }
    }

    display->setMapSymbol(isImportant ? '*' : symbol, location, isImportant);
}

char ComponentDrawable::symbol (
    GameItem const * gameItem,
    char defaultValue) const
{
    return getDirectValue<char, char>(gameItem, "symbol", defaultValue);
}

void ComponentDrawable::setSymbol (
    GameItem * gameItem,
    char value)
{
    setDirectValue<char, char>(gameItem, "symbol", value);
}

std::unique_ptr<Component> ComponentDrawable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentDrawable(id));
}
