#include "GameItem.h"

#include "Component.h"
#include "ComponentRegistry.h"
#include "Game.h"

bool GameItem::addComponent (Game * game, int componentId)
{
    if (componentId < 0 ||
        componentId >= ComponentRegistry::MaxComponentCount)
    {
        return false;
    }

    if (!mComponents[componentId])
    {
        mComponents[componentId] = true;

        if (instanceId() != 0)
        {
            game->addEvent(ComponentAdded {
                mInstanceId,
                componentId});
        }

        return true;
    }

    return false;
}

bool GameItem::removeComponent (Game * game, int componentId)
{
    if (componentId < 0 ||
        componentId >= ComponentRegistry::MaxComponentCount)
    {
        return false;
    }

    if (mComponents[componentId])
    {
        mComponents[componentId] = false;

        if (instanceId() != 0)
        {
            game->addEvent(ComponentRemoved {
                mInstanceId,
                componentId});
        }

        return true;
    }

    return false;
}

bool GameItem::addTag (std::string const & tag)
{
    return mTags.insert(tag).second;
}

bool GameItem::removeTag (std::string const & tag)
{
    return mTags.erase(tag) == 1;
}

void GameItem::draw (Display * display) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
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
