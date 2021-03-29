#ifndef COMPONENTHEALTH_H
#define COMPONENTHEALTH_H

#include "Component.h"

class ComponentHealth : public Component
{
public:
    static std::string const PermanentId;

    ComponentHealth () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    int health (
        GameItem const * gameItem,
        int defaultValue = 0) const;

    void setHealth (
        GameItem * gameItem,
        int value);

    unsigned int maxHealth (
        GameItem const * gameItem,
        unsigned int defaultValue = 0) const;

    void setMaxHealth (
        GameItem * gameItem,
        unsigned int value);

    bool isDead (
        GameItem const * gameItem,
        bool defaultValue = false) const;

    void setIsDead (
        GameItem * gameItem,
        bool value);

    bool isUnconscious (
        GameItem const * gameItem,
        bool defaultValue = false) const;

    void setIsUnconscious (
        GameItem * gameItem,
        bool value);

private:
    ComponentHealth (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTHEALTH_H
