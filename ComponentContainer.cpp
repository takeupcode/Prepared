#include "ComponentContainer.h"

#include "GameItem.h"

std::string const ComponentContainer::PermanentId = "container";

std::unique_ptr<Component> ComponentContainer::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentContainer(id));
}
