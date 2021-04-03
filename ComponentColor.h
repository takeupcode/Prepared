#ifndef COMPONENTCOLOR_H
#define COMPONENTCOLOR_H

#include "Component.h"

#include <string>
#include <vector>

class ComponentColor : public Component
{
public:
    static std::string const PermanentId;

    ComponentColor () = default;

    std::string permanentId () const override
    {
        return PermanentId;
    }

    std::vector<std::string> attributesBasic (
        GameItem const * gameItem,
        std::string defaultValue = "") const;

    void setAttributesBasic (
        GameItem * gameItem,
        std::vector<std::string> values);

    std::vector<std::string> attributes256 (
        GameItem const * gameItem,
        std::string defaultValue = "") const;

    void setAttributes256 (
        GameItem * gameItem,
        std::vector<std::string> values);

    std::vector<std::string> attributesTrue (
        GameItem const * gameItem,
        std::string defaultValue = "") const;

    void setAttributesTrue (
        GameItem * gameItem,
        std::vector<std::string> values);

private:
    ComponentColor (int id)
    : Component(id)
    { }

    std::unique_ptr<Component> clone (int id) const override;
};

#endif // COMPONENTCOLOR_H
