#ifndef COMPONENTLOCATEABLE_H
#define COMPONENTLOCATEABLE_H

#include "Component.h"
#include "Direction.h"
#include "Point.h"

class ComponentLocateable : public Component
{
public:
    static std::string const PermanentId;

    ComponentLocateable () = default;

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

    Direction direction (
        GameItem const * gameItem,
        Direction defaultValue = Direction::North) const;

    void setDirection (
        GameItem * gameItem,
        Direction value);

private:
    ComponentLocateable (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTLOCATEABLE_H
