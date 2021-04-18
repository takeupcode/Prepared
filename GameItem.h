#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "ComponentRegistry.h"
#include "GameItemRegistry.h"

#include <bitset>
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class Display;
class Game;

class GameItem
{
public:
    static int constexpr FirstInstanceId = 100;

    using Value = std::variant<bool, char, int, double, std::string>;

    GameItem (int id)
    : mId(id)
    {
        static int nextInstanceId = FirstInstanceId;

        mInstanceId = nextInstanceId++;
    }

    GameItem (int id, int)
    : mId(id),
      mInstanceId(0)
    { }

    GameItem (GameItem const & src) = default;
    GameItem (GameItem && src) = default;

    GameItem & operator = (GameItem const & src) = default;
    GameItem & operator = (GameItem && src) = default;

    int id () const
    {
        return mId;
    }

    int instanceId () const
    {
        return mInstanceId;
    }

    bool hasComponent (int componentId) const
    {
        if (componentId < 0 ||
            componentId >= ComponentRegistry::MaxComponentCount)
        {
            return false;
        }

        if (mComponents[componentId])
        {
            return true;
        }

        if (instanceId() == 0)
        {
            return false;
        }

        auto registeredItem = GameItemRegistry::find(id());
        if (registeredItem == nullptr)
        {
            throw std::logic_error("GameItem not registered.");
        }

        return registeredItem->mComponents[componentId];
    }

    bool addComponent (Game * game, int componentId);

    bool removeComponent (Game * game, int componentId);

    std::vector<GameItem> & items ()
    {
        return mItems;
    }

    std::vector<GameItem> const & items () const
    {
        return mItems;
    }

    bool hasDirectTag (std::string const & tag) const
    {
        if (mTags.find(tag) != mTags.end())
        {
            return true;
        }

        return false;
    }

    bool hasTag (std::string const & tag) const
    {
        if (hasDirectTag(tag))
        {
            return true;
        }

        auto registeredItem = GameItemRegistry::find(id());
        if (registeredItem == nullptr)
        {
            throw std::logic_error("GameItem not registered.");
        }

        return registeredItem->hasDirectTag(tag);
    }

    bool addTag (std::string const & tag);

    bool removeTag (std::string const & tag);

    std::vector<std::string> tags () const
    {
        return std::vector<std::string>(mTags.begin(), mTags.end());
    }

    void draw (Display * display) const;

private:
    friend class Component;

    struct Property
    {
        std::string mName;
        Value mValue;
    };

    bool hasDirectComponent (int componentId) const
    {
        if (componentId < 0 ||
            componentId >= ComponentRegistry::MaxComponentCount)
        {
            return false;
        }

        if (mComponents[componentId])
        {
            return true;
        }

        return false;
    }

    int mId;
    int mInstanceId;
    std::bitset<ComponentRegistry::MaxComponentCount> mComponents;
    std::vector<Property> mProperties;
    std::set<std::string> mTags;

    std::vector<GameItem> mItems;
};

#endif // GAMEITEM_H
