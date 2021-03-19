#include "ComponentAbilityAdjustment.h"

#include "GameItem.h"

std::string const ComponentAbilityAdjustment::PermanentId = "adjustment";

std::unique_ptr<Component> ComponentAbilityAdjustment::clone (int id) const
{
    return std::unique_ptr<Component>(new ComponentAbilityAdjustment(id));
}
