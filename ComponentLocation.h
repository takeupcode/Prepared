#ifndef COMPONENTLOCATION_H
#define COMPONENTLOCATION_H

#include "Component.h"
#include "Direction.h"
#include "Point.h"

class ComponentLocation : public Component
{
public:
    static std::string const PermanentId;

    ComponentLocation () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    Point2i location (
        GameItem const * gameItem,
        Point2i const & defaultValue = {0, 0}) const;

    void setLocation (
        GameItem * gameItem,
        Point2i const & value);

    Direction direction (
        GameItem const * gameItem,
        Direction defaultValue = Direction::North) const;

    void setDirection (
        GameItem * gameItem,
        Direction value);

private:
    ComponentLocation (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTLOCATION_H
