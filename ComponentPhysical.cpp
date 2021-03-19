#include "ComponentPhysical.h"

#include "GameItem.h"

std::string const ComponentPhysical::PermanentId = "physical";

Dimensions ComponentPhysical::dimensions (
    GameItem const * gameItem,
    Dimensions const & defaultValue) const
{
    Dimensions result;

    result.length = getRegisteredValue<double, double>(
        gameItem, "length", defaultValue.length);
    result.width = getRegisteredValue<double, double>(
        gameItem, "width", defaultValue.width);
    result.height = getRegisteredValue<double, double>(
        gameItem, "height", defaultValue.height);

    return result;
}

void ComponentPhysical::setDimensions (
    GameItem * gameItem,
    Dimensions const & value)
{
    setRegisteredValue<double, double>(gameItem, "length", value.length);
    setRegisteredValue<double, double>(gameItem, "width", value.width);
    setRegisteredValue<double, double>(gameItem, "height", value.height);
}

double ComponentPhysical::weight (
    GameItem const * gameItem,
    double defaultValue) const
{
    return getRegisteredValue<double, double>(gameItem, "weight", defaultValue);
}

void ComponentPhysical::setWeight (
    GameItem * gameItem,
    double value)
{
    setRegisteredValue<double, double>(gameItem, "weight", value);
}

ComponentPhysical::Form ComponentPhysical::form (
    GameItem const * gameItem,
    Form defaultValue) const
{
    return getRegisteredValue<Form, int>(gameItem, "form", defaultValue);
}

void ComponentPhysical::setForm (
    GameItem * gameItem,
    Form value)
{
    setRegisteredValue<Form, int>(gameItem, "form", value);
}

std::unique_ptr<Component> ComponentPhysical::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentPhysical(id));
}
