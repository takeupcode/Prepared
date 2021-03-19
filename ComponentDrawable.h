#ifndef COMPONENTDRAWABLE_H
#define COMPONENTDRAWABLE_H

#include "Component.h"

class ComponentDrawable : public Component
{
public:
    static std::string const PermanentId;

    ComponentDrawable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    void draw (GameItem const * gameItem, Display * display) const override;

    char symbol (
        GameItem const * gameItem,
        char defaultValue = '!') const;

    void setSymbol (
        GameItem * gameItem,
        char value);

private:
    ComponentDrawable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTDRAWABLE_H
