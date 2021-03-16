#ifndef COMPONENTFINDABLE_H
#define COMPONENTFINDABLE_H

#include "Component.h"

class ComponentFindable : public Component
{
public:
    static std::string const PermanentId;

    ComponentFindable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    unsigned int targetCount (
        GameItem const * gameItem,
        unsigned int defaultValue = 0) const;

    void setTargetCount (
        GameItem * gameItem,
        unsigned int value);

    unsigned int chanceOfFinding (
        GameItem const * gameItem,
        unsigned int defaultValue = 0) const;

    void setChanceOfFinding (
        GameItem * gameItem,
        unsigned int value);

private:
    ComponentFindable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTFINDABLE_H
