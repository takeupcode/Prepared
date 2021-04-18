#include "Component.h"

#include "ComponentRegistry.h"
#include "GameItem.h"

#include <algorithm>

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    bool value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    char value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    int value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    double value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    std::string const & value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::setValue (
    GameItem * gameItem,
    std::string const & name,
    char const * value)
{
    return setValueImpl(gameItem, name, value);
}

bool Component::removeValue (
    GameItem * gameItem,
    std::string const & name)
{
    auto foundProperty = std::find_if(
        gameItem->mProperties.begin(), gameItem->mProperties.end(),
        [&name] (auto const & property) { return property.mName == name; }
        );

    if (foundProperty != gameItem->mProperties.end())
    {
        gameItem->mProperties.erase(foundProperty);
        return true;
    }

    return false;
}

bool Component::removeValueCollection (
    GameItem * gameItem,
    std::string const & name)
{
    std::string modifiedName;
    auto namePredicate = [&modifiedName] (auto const & property)
    {
        return property.mName == modifiedName;
    };

    modifiedName = name + "Count";
    auto foundProperty = std::find_if(
        gameItem->mProperties.begin(),
        gameItem->mProperties.end(),
        namePredicate);

    bool result = false;
    int count = 0;
    if (foundProperty != gameItem->mProperties.end())
    {
        count = std::get<int>(foundProperty->mValue);
        gameItem->mProperties.erase(foundProperty);

        // The count is what determines whether the method returns true.
        result = true;
    }

    for (int i = 0; i < count; ++i)
    {
        std::string modifiedName = name + std::to_string(i);
        foundProperty = std::find_if(
            gameItem->mProperties.begin(),
            gameItem->mProperties.end(),
            namePredicate);

        if (foundProperty != gameItem->mProperties.end())
        {
            gameItem->mProperties.erase(foundProperty);
        }
    }

    return result;
}

void Component::adjustValue (
    GameItem const * gameItem,
    std::string const & name,
    bool & value) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
    {
        if (gameItem->hasDirectComponent(i))
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->adjustValueHook(gameItem, name, value);
            }
        }
    }
}

void Component::adjustValue (
    GameItem const * gameItem,
    std::string const & name,
    char & value) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
    {
        if (gameItem->hasDirectComponent(i))
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->adjustValueHook(gameItem, name, value);
            }
        }
    }
}

void Component::adjustValue (
    GameItem const * gameItem,
    std::string const & name,
    int & value) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
    {
        if (gameItem->hasDirectComponent(i))
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->adjustValueHook(gameItem, name, value);
            }
        }
    }
}

void Component::adjustValue (
    GameItem const * gameItem,
    std::string const & name,
    double & value) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
    {
        if (gameItem->hasDirectComponent(i))
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->adjustValueHook(gameItem, name, value);
            }
        }
    }
}

void Component::adjustValue (
    GameItem const * gameItem,
    std::string const & name,
    std::string & value) const
{
    for (int i = 0; i < ComponentRegistry::MaxComponentCount; ++i)
    {
        if (gameItem->hasDirectComponent(i))
        {
            Component * component = ComponentRegistry::find(i);
            if (component != nullptr)
            {
                component->adjustValueHook(gameItem, name, value);
            }
        }
    }
}
