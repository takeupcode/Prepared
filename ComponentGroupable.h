#ifndef COMPONENTGROUPABLE_H
#define COMPONENTGROUPABLE_H

#include "Component.h"

class ComponentGroupable : public Component
{
public:
    static std::string const PermanentId;

    ComponentGroupable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    bool areGroupedTogether (
        GameItem const * gameItem1,
        GameItem const * gameItem2) const;

    std::vector<std::string> propertyNames (
        GameItem const * gameItem,
        std::string defaultValue = "") const;

    void setPropertyNames (
        GameItem * gameItem,
        std::vector<std::string> values);

private:
    ComponentGroupable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTGROUPABLE_H
