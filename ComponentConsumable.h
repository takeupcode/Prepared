#ifndef COMPONENTCONSUMABLE_H
#define COMPONENTCONSUMABLE_H

#include "Component.h"

#include <string>

class ComponentConsumable : public Component
{
public:
    static std::string const PermanentId;

    enum class RemainingEstimate
    {
        Empty,
        OneQuarter,
        Half,
        ThreeQuarters,
        Full
    };

    ComponentConsumable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    RemainingEstimate percentageRemainingEstimate (
        GameItem const * gameItem,
        RemainingEstimate defaultValue = RemainingEstimate::Empty) const;

    double percentageRemainingExact (
        GameItem const * gameItem,
        double defaultValue = 0) const;

    void setPercentageRemaining (
        GameItem * gameItem,
        double value);

protected:
    void adjustValueHook (
        GameItem const * gameItem,
        std::string const & name,
        std::string & value) const override;

private:
    ComponentConsumable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTCONSUMABLE_H
