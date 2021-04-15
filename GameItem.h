#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "GameItemRegistry.h"

#include <bitset>
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class Display;

class GameItem
{
public:
    using Value = std::variant<bool, char, int, double, std::string>;

    GameItem (int id)
    : mId(id)
    { }

    GameItem (GameItem const & src) = default;
    GameItem (GameItem && src) = default;

    GameItem & operator = (GameItem const & src) = default;
    GameItem & operator = (GameItem && src) = default;

    int id () const
    {
        return mId;
    }

    bool hasComponent (int componentId) const
    {
        if (componentId < 0 || componentId >= MaxComponentCount)
        {
            return false;
        }

        if (mComponents[componentId])
        {
            return true;
        }

        auto registeredItem = GameItemRegistry::find(mId);
        if (registeredItem == nullptr)
        {
            throw std::logic_error("GameItem not registered.");
        }

        return registeredItem->mComponents[componentId];
    }

    bool addComponent (int componentId);

    bool removeComponent (int componentId);

    std::vector<GameItem> & items ()
    {
        return mItems;
    }

    std::vector<GameItem> const & items () const
    {
        return mItems;
    }

    bool hasTag (std::string const & tag) const
    {
        if (mTags.find(tag) != mTags.end())
        {
            return true;
        }

        return false;
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

    static constexpr int MaxComponentCount = 32;

    bool hasDirectComponent (int componentId) const
    {
        if (componentId < 0 || componentId >= MaxComponentCount)
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
    std::bitset<MaxComponentCount> mComponents;
    std::vector<Property> mProperties;
    std::set<std::string> mTags;

    std::vector<GameItem> mItems;
};

#endif // GAMEITEM_H
