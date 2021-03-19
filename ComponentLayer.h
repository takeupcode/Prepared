#ifndef COMPONENTLAYER_H
#define COMPONENTLAYER_H

#include "Component.h"

class ComponentLayer : public Component
{
public:
    static std::string const PermanentId;

    ComponentLayer () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    int layerId (
        GameItem const * gameItem,
        int defaultValue = 0) const;

    void setLayerId (
        GameItem * gameItem,
        int value);

private:
    ComponentLayer (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTLAYER_H
