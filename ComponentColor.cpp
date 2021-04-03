#include "ComponentColor.h"

#include "GameItem.h"

std::string const ComponentColor::PermanentId = "color";

std::vector<std::string> ComponentColor::attributesBasic (
    GameItem const * gameItem,
    std::string defaultValue) const
    {
        return getRegisteredValueCollection<std::string, std::string>(
            gameItem, "colorBasicAttributes", defaultValue);
    }

void ComponentColor::setAttributesBasic (
    GameItem * gameItem,
    std::vector<std::string> values)
    {
        setRegisteredValueCollection<std::string, std::string>(
            gameItem, "colorBasicAttributes", values);
    }

std::vector<std::string> ComponentColor::attributes256 (
    GameItem const * gameItem,
    std::string defaultValue) const
    {
        return getRegisteredValueCollection<std::string, std::string>(
            gameItem, "color256Attributes", defaultValue);
    }

void ComponentColor::setAttributes256 (
    GameItem * gameItem,
    std::vector<std::string> values)
    {
        setRegisteredValueCollection<std::string, std::string>(
            gameItem, "color256Attributes", values);
    }

std::vector<std::string> ComponentColor::attributesTrue (
    GameItem const * gameItem,
    std::string defaultValue) const
    {
        return getRegisteredValueCollection<std::string, std::string>(
            gameItem, "colorTrueAttributes", defaultValue);
    }

void ComponentColor::setAttributesTrue (
    GameItem * gameItem,
    std::vector<std::string> values)
{
    setRegisteredValueCollection<std::string, std::string>(
        gameItem, "colorTrueAttributes", values);
}

std::unique_ptr<Component> ComponentColor::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentColor(id));
}
