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

    GameItem (GameItem const & src) = delete;
    GameItem (GameItem && src) = delete;

    GameItem & operator = (GameItem const & src) = delete;
    GameItem & operator = (GameItem && src) = delete;

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

    bool hasItem (int instanceId) const
    {
        if (mItems.find(instanceId) != mItems.end())
        {
            return true;
        }

        return false;
    }

    bool addItem (int instanceId);

    bool removeItem (int instanceId);

    std::vector<int> items () const
    {
        return std::vector<int>(mItems.begin(), mItems.end());
    }

    bool hasTag (std::string const & tag) const
    {
        if (mTags.find(tag) != mTags.end())
        {
            return true;
        }

        return false;
    }

    bool addTag (Game * game, std::string const & tag);

    bool removeTag (Game * game, std::string const & tag);

    std::vector<std::string> tags () const
    {
        return std::vector<std::string>(mTags.begin(), mTags.end());
    }

    void draw (Display * display) const;

private:
    friend class Component;
    friend class Game;
    friend class GameItemRegistry;

    // Only the Game and GameItemRegistry classes can
    // construct GameItem instances.
    GameItem (int id)
    : mId(id)
    {
        auto registeredItem = GameItemRegistry::find(mId);
        if (registeredItem == nullptr)
        {
            throw std::logic_error("GameItem not registered.");
        }

        static int nextInstanceId = FirstInstanceId;
        mInstanceId = nextInstanceId++;

        for (auto const & tag: registeredItem->tags())
        {
            mTags.insert(tag);
        }
    }

    GameItem (int id, int)
    : mId(id),
      mInstanceId(0)
    { }

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

    using Value = std::variant<bool, char, int, double, std::string>;

    struct Property
    {
        std::string mName;
        Value mValue;
    };

    int mId;
    int mInstanceId;
    std::bitset<ComponentRegistry::MaxComponentCount> mComponents;
    std::vector<Property> mProperties;
    std::set<std::string> mTags;
    std::set<int> mItems;
};

#endif // GAMEITEM_H
