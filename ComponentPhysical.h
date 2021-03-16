#ifndef COMPONENTPHYSICAL_H
#define COMPONENTPHYSICAL_H

#include "Component.h"
#include "Dimensions.h"

#include <string>

class ComponentPhysical : public Component
{
public:
    static std::string const PermanentId;

    enum class Form
    {
        Solid,
        Liquid,
        Gas,
        Powder
    };

    ComponentPhysical () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    Dimensions dimensions (
        GameItem const * gameItem,
        Dimensions const & defaultValue = {0, 0, 0}) const;

    void setDimensions (
        GameItem * gameItem,
        Dimensions const & value);

    double weight (
        GameItem const * gameItem,
        double defaultValue = 0) const;

    void setWeight (
        GameItem * gameItem,
        double value);

    Form form (
        GameItem const * gameItem,
        Form defaultValue = Form::Solid) const;

    void setForm (
        GameItem * gameItem,
        Form value);

private:
    ComponentPhysical (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTPHYSICAL_H
