#include "ComponentLayer.h"

#include "GameItem.h"

std::string const ComponentLayer::PermanentId = "layer";

int ComponentLayer::layerId (
    GameItem const * gameItem,
    int defaultValue) const
{
    int result = defaultValue;

    result = getChainedValue<int, int>(gameItem, "layerId", defaultValue);

    return result;
}

void ComponentLayer::setLayerId (
    GameItem * gameItem,
    int value)
{
    setDirectValue<int, int>(gameItem, "layerId", value);
}

std::unique_ptr<Component> ComponentLayer::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentLayer(id));
}
