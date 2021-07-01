#ifndef COMPONENTTRADEABLE_H
#define COMPONENTTRADEABLE_H

#include "Component.h"

class ComponentTradeable : public Component
{
public:
    static std::string const PermanentId;

    ComponentTradeable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    bool isCoin (
        GameItem const * gameItem,
        bool defaultValue = false) const;

    void setIsCoin (
        GameItem * gameItem,
        bool value);

    unsigned int value (
        GameItem const * gameItem,
        unsigned int defaultValue = 0) const;

    void setValue (
        GameItem * gameItem,
        unsigned int value);

private:
    ComponentTradeable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTTRADEABLE_H
