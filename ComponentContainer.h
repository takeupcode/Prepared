#ifndef COMPONENTCONTAINER_H
#define COMPONENTCONTAINER_H

#include "Component.h"

#include <string>

class ComponentContainer : public Component
{
public:
    static std::string const PermanentId;

    ComponentContainer () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

private:
    ComponentContainer (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTCONTAINER_H
