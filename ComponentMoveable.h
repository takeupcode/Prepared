#ifndef COMPONENTMOVEABLE_H
#define COMPONENTMOVEABLE_H

#include "Component.h"
#include "Point.h"

class ComponentMoveable : public Component
{
public:
    static std::string const PermanentId;

    ComponentMoveable () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    Point location (
        GameItem const * gameItem,
        Point const & defaultValue = {0, 0}) const;

    void setLocation (
        GameItem * gameItem,
        Point const & value);

private:
    ComponentMoveable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTMOVEABLE_H
