#include "ComponentConsumable.h"

#include "GameItem.h"

std::string const ComponentConsumable::PermanentId = "consumable";

ComponentConsumable::RemainingEstimate ComponentConsumable::percentageRemainingEstimate (
    GameItem const * gameItem,
    RemainingEstimate defaultValue) const
{
    return getChainedValue<RemainingEstimate, int>(
        gameItem,
        "percentageRemainingEstimate",
        defaultValue);
}

double ComponentConsumable::percentageRemainingExact (
    GameItem const * gameItem,
    double defaultValue) const
{
    return getChainedValue<double, double>(
        gameItem,
        "percentageRemainingExact",
        defaultValue);
}

void ComponentConsumable::setPercentageRemaining (
    GameItem * gameItem,
    double value)
{
    RemainingEstimate estimate;
    if (value < 25.0)
    {
        estimate = RemainingEstimate::Empty;
    }
    else if (value < 50.0)
    {
        estimate = RemainingEstimate::OneQuarter;
    }
    else if (value < 75.0)
    {
        estimate = RemainingEstimate::Half;
    }
    else if (value < 99.0)
    {
        estimate = RemainingEstimate::ThreeQuarters;
    }
    else
    {
        estimate = RemainingEstimate::Full;
    }

    setDirectValue<double, double>(
        gameItem,
        "percentageRemainingExact",
        value);

    setDirectValue<RemainingEstimate, int>(
        gameItem,
        "percentageRemainingEstimate",
        estimate);
}

void ComponentConsumable::adjustValueHook (
    GameItem const * gameItem,
    std::string const & name,
    std::string & value) const
{
    if (name == "name")
    {
        switch (percentageRemainingEstimate(gameItem))
        {
        case RemainingEstimate::Empty:
            value += ", empty";
            break;

        case RemainingEstimate::OneQuarter:
            value += ", low";
            break;

        case RemainingEstimate::Half:
            value += ", half";
            break;

        case RemainingEstimate::ThreeQuarters:
            value += ", good";
            break;

        case RemainingEstimate::Full:
            value += ", full";
            break;
        }
    }
}

std::unique_ptr<Component> ComponentConsumable::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentConsumable(id));
}
