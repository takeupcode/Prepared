#ifndef COMPONENT_H
#define COMPONENT_H

#include "GameItem.h"

#include <optional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class Display;

class Component
{
public:
    virtual ~Component () = default;

    int id () const
    {
        if (!mId)
        {
            throw std::logic_error("Component not registered.");
        }

        return mId.value();
    }

    virtual std::string permanentId () const = 0;

    virtual void draw (GameItem const *, Display *) const
    { }

protected:
    Component ()
    { }

    Component (int id)
    : mId(id)
    { }

    bool isValidGameItem (GameItem const * gameItem) const
    {
        if (gameItem == nullptr)
        {
            return false;
        }

        return gameItem->hasComponent(id());
    }

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        bool value);

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        char value);

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        int value);

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        double value);

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        std::string const & value);

    bool setValue (
        GameItem * gameItem,
        std::string const & name,
        char const * value);

    bool removeValue (
        GameItem * gameItem,
        std::string const & name);

    bool removeValueCollection (
        GameItem * gameItem,
        std::string const & name);

    virtual void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        bool & value) const
    { }

    virtual void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        char & value) const
    { }

    virtual void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        int & value) const
    { }

    virtual void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        double & value) const
    { }

    virtual void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        std::string & value) const
    { }

    void adjustValue (
        GameItem const * gameItem,
        std::string const & name,
        bool & value) const;

    void adjustValue (
        GameItem const * gameItem,
        std::string const & name,
        char & value) const;

    void adjustValue (
        GameItem const * gameItem,
        std::string const & name,
        int & value) const;

    void adjustValue (
        GameItem const * gameItem,
        std::string const & name,
        double & value) const;

    void adjustValue (
        GameItem const * gameItem,
        std::string const & name,
        std::string & value) const;

    template<typename T>
    std::optional<T> getValue (
        GameItem const * gameItem,
        std::string const & name) const
    {
        std::optional<T> result;
        auto valueVariant = findValue(gameItem, name);
        if (valueVariant != nullptr)
        {
            T value = std::get<T>(*valueVariant);
            adjustValue(gameItem, name, value);
            result = value;
        }

        return result;
    }

    template<typename OutsideT, typename InsideT>
    std::vector<OutsideT> getValueCollection (
        GameItem const * gameItem,
        std::string const & name,
        OutsideT const & defaultValue) const
    {
        std::vector<OutsideT> result;
        auto valueVariant = findValue(gameItem, name + "Count");
        if (valueVariant != nullptr)
        {
            int count = std::get<int>(*valueVariant);
            for (int i = 0; i < count; ++i)
            {
                valueVariant = findValue(gameItem, name + std::to_string(i));
                if (valueVariant != nullptr)
                {
                    result.push_back(static_cast<OutsideT>(
                        std::get<InsideT>(*valueVariant)));
                }
                else
                {
                    result.push_back(defaultValue);
                }
            }
        }

        return result;
    }

    template<typename OutsideT, typename InsideT>
    OutsideT getDirectValue (
        GameItem const * gameItem,
        std::string const & propertyName,
        OutsideT const & defaultValue) const
    {
        OutsideT result = defaultValue;
        if (isValidGameItem(gameItem))
        {
            auto resultOpt = getValue<InsideT>(gameItem, propertyName);
            if (resultOpt)
            {
                result = static_cast<OutsideT>(resultOpt.value());
            }
        }

        return result;
    }

    template<typename OutsideT, typename InsideT>
    std::vector<OutsideT> getDirectValueCollection (
        GameItem const * gameItem,
        std::string const & propertyName,
        OutsideT const & defaultValue) const
    {
        std::vector<OutsideT> result;
        if (isValidGameItem(gameItem))
        {
            result = getValueCollection<OutsideT, InsideT>(
                gameItem, propertyName, defaultValue);
        }

        return result;
    }

    template<typename OutsideT, typename InsideT>
    void setDirectValue (
        GameItem * gameItem,
        std::string const & propertyName,
        OutsideT const & value)
    {
        if (isValidGameItem(gameItem))
        {
            setValue(gameItem, propertyName, static_cast<InsideT>(value));
        }
    }

    template<typename OutsideT, typename InsideT>
    void setDirectValueCollection (
        GameItem * gameItem,
        std::string const & propertyName,
        std::vector<OutsideT> const & values)
    {
        if (isValidGameItem(gameItem))
        {
            setValue(gameItem, propertyName + "Count",
                static_cast<int>(values.size()));

            for (unsigned int i = 0; i < values.size(); ++i)
            {
                setValue(gameItem, propertyName + std::to_string(i),
                    static_cast<InsideT>(values[i]));
            }
        }
    }

    template<typename OutsideT, typename InsideT>
    OutsideT getRegisteredValue (
        GameItem const * gameItem,
        std::string const & propertyName,
        OutsideT const & defaultValue) const
    {
        OutsideT result = defaultValue;
        if (gameItem == nullptr)
        {
            return result;
        }

        auto registeredItem = GameItemRegistry::find(gameItem->id());
        if (!isValidGameItem(registeredItem))
        {
            return result;
        }

        auto resultOpt = getValue<InsideT>(registeredItem, propertyName);
        if (resultOpt)
        {
            InsideT value = resultOpt.value();
            adjustValue(gameItem, propertyName, value);
            result = static_cast<OutsideT>(value);
        }

        return result;
    }

    template<typename OutsideT, typename InsideT>
    std::vector<OutsideT> getRegisteredValueCollection (
        GameItem const * gameItem,
        std::string const & propertyName,
        OutsideT const & defaultValue) const
    {
        std::vector<OutsideT> result;
        if (gameItem == nullptr)
        {
            return result;
        }

        auto registeredItem = GameItemRegistry::find(gameItem->id());
        if (!isValidGameItem(registeredItem))
        {
            return result;
        }

        result = getValueCollection<OutsideT, InsideT>(
            registeredItem, propertyName, defaultValue);

        return result;
    }

    template<typename OutsideT, typename InsideT>
    void setRegisteredValue (
        GameItem * gameItem,
        std::string const & propertyName,
        OutsideT const & value)
    {
        if (gameItem == nullptr)
        {
            return;
        }

        auto registeredItem = GameItemRegistry::find(gameItem->id());
        if (!isValidGameItem(registeredItem))
        {
            return;
        }

        setValue(registeredItem, propertyName, static_cast<InsideT>(value));
    }

    template<typename OutsideT, typename InsideT>
    void setRegisteredValueCollection (
        GameItem * gameItem,
        std::string const & propertyName,
        std::vector<OutsideT> const & values)
    {
        if (gameItem == nullptr)
        {
            return;
        }

        auto registeredItem = GameItemRegistry::find(gameItem->id());
        if (!isValidGameItem(registeredItem))
        {
            return;
        }

        setValue(registeredItem, propertyName + "Count",
            static_cast<int>(values.size()));

        for (unsigned int i = 0; i < values.size(); ++i)
        {
            setValue(registeredItem, propertyName + std::to_string(i),
                static_cast<InsideT>(values[i]));
        }
    }

    GameItem::Value * findValue (
        GameItem * gameItem,
        std::string const & name)
    {
        for (auto & property: gameItem->mProperties)
        {
            if (property.mName == name)
            {
                return &property.mValue;
            }
        }

        return nullptr;
    }

    GameItem::Value const * findValue (
        GameItem const * gameItem,
        std::string const & name) const
    {
        for (auto & property: gameItem->mProperties)
        {
            if (property.mName == name)
            {
                return &property.mValue;
            }
        }

        return nullptr;
    }

private:
    friend class ComponentRegistry;

    virtual std::unique_ptr<Component> clone (int id) const = 0;

    template<typename T>
    bool setValueImpl (
        GameItem * gameItem,
        std::string const & name,
        T const & value)
    {
        auto valueVariant = findValue(gameItem, name);
        if (valueVariant == nullptr)
        {
            gameItem->mProperties.push_back({name, value});
            return true;
        }
        else
        {
            *valueVariant = value;
        }

        return false;
    }

    std::optional<int> mId;
};

#endif // COMPONENT_H
