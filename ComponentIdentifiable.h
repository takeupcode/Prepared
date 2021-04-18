#ifndef COMPONENTIDENTIFIABLE_H
#define COMPONENTIDENTIFIABLE_H

#include "Component.h"

#include <string>

class ComponentIdentifiable : public Component
{
public:
    static std::string const PermanentId;

    ComponentIdentifiable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    int shortcutId (
        GameItem const * gameItem,
        int defaultValue = 0) const;

    void setShortcutId (
        GameItem * gameItem,
        int value);

    unsigned int count (
        GameItem const * gameItem,
        unsigned int defaultValue = 1) const;

    void setCount (
        GameItem * gameItem,
        unsigned int value);

    std::string name (
        GameItem const * gameItem,
        std::string const & defaultValue = "") const;

    void setName (
        GameItem * gameItem,
        std::string const & value);

private:
    ComponentIdentifiable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTIDENTIFIABLE_H
