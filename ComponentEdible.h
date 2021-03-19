#ifndef COMPONENTEDIBLE_H
#define COMPONENTEDIBLE_H

#include "Component.h"

#include <string>

class ComponentEdible : public Component
{
public:
    static std::string const PermanentId;

    ComponentEdible () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    bool isEdible (
        GameItem const * gameItem) const;

    int nutrition (
        GameItem const * gameItem,
        int defaultValue = 0) const;

    void setNutrition (
        GameItem * gameItem,
        int value);

private:
    ComponentEdible (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTEDIBLE_H
