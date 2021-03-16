#include "ComponentGroupable.h"

#include "GameItem.h"

std::string const ComponentGroupable::PermanentId = "groupable";

bool ComponentGroupable::areGroupedTogether (
    GameItem const * gameItem1,
    GameItem const * gameItem2) const
{
    if (gameItem1 == nullptr || gameItem2 == nullptr)
    {
        return false;
    }

    if (gameItem1->id() != gameItem2->id())
    {
        return false;
    }

    auto names = propertyNames(gameItem1);
    for (auto const & name: names)
    {
        auto value1 = findValue(gameItem1, name);
        auto value2 = findValue(gameItem2, name);

        if (value1 == nullptr || value2 == nullptr)
        {
            return false;
        }

        if (*value1 != *value2)
        {
            return false;
        }
    }

    return true;
}

std::vector<std::string> ComponentGroupable::propertyNames (
    GameItem const * gameItem,
    std::string defaultValue) const
{
    return getRegisteredValueCollection<std::string, std::string>(
        gameItem, "groupPropertyNames", defaultValue);
}

void ComponentGroupable::setPropertyNames (
    GameItem * gameItem,
    std::vector<std::string> values)
{
    setRegisteredValueCollection<std::string, std::string>(
        gameItem, "groupPropertyNames", values);
}

std::unique_ptr<Component> ComponentGroupable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentGroupable(id));
}
