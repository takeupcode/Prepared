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

bool GameItem::addItem (int instanceId)
{
    return mItems.insert(instanceId).second;
}

bool GameItem::removeItem (int instanceId)
{
    return mItems.erase(instanceId) == 1;
}

bool GameItem::addTag (Game * game, std::string const & tag)
{
    bool tagAdded = mTags.insert(tag).second;

    if (tagAdded && instanceId() != 0)
    {
        game->addEvent(TagAdded {
            mInstanceId,
            tag});
    }

    return tagAdded;
}

bool GameItem::removeTag (Game * game, std::string const & tag)
{
    bool tagRemoved = mTags.erase(tag) == 1;

    if (tagRemoved && instanceId() != 0)
    {
        game->addEvent(TagRemoved {
            mInstanceId,
            tag});
    }

    return tagRemoved;
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
