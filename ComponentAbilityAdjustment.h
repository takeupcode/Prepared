#ifndef COMPONENTABILITYADJUSTMENT_H
#define COMPONENTABILITYADJUSTMENT_H

#include "Component.h"

#include <string>

class ComponentAbilityAdjustment : public Component
{
public:
    static std::string const PermanentId;

    ComponentAbilityAdjustment () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

private:
    ComponentAbilityAdjustment (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTABILITYADJUSTMENT_H
