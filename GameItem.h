#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "GameItemRegistry.h"

#include <bitset>
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
    std::vector<GameItem> mItems;
};

#endif // GAMEITEM_H
