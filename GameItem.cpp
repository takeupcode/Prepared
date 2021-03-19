#include "GameItem.h"

#include "Component.h"
#include "ComponentRegistry.h"

bool GameItem::addComponent (int componentId)
{
    if (componentId < 0 || componentId >= MaxComponentCount)
    {
        return false;
    }

    if (!mComponents[componentId])
    {
        mComponents[componentId] = true;
        return true;
    }

    return false;
}

bool GameItem::removeComponent (int componentId)
{
    if (componentId < 0 || componentId >= MaxComponentCount)
    {
        return false;
    }

    if (mComponents[componentId])
    {
        mComponents[componentId] = false;
        return true;
    }

    return false;
}

void GameItem::draw (Display * display) const
{
    for (int i = 0; i < MaxComponentCount; ++i)
    {
        if (mComponents[i])
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->draw(this, display);
            }
        }
    }
}
