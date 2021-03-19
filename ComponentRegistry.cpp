#include "ComponentRegistry.h"

#include "Component.h"

int ComponentRegistry::mNextId = 0;
std::unordered_map<std::string, int> ComponentRegistry::mComponentIds;
std::unordered_map<int, std::unique_ptr<Component>> ComponentRegistry::mComponents;

bool ComponentRegistry::add (Component const & newComponent)
{
    auto componentIdsMapResult = mComponentIds.find(newComponent.permanentId());
    if (componentIdsMapResult == mComponentIds.end())
    {
        mComponentIds.try_emplace(newComponent.permanentId(), mNextId);
        mComponents.try_emplace(mNextId, newComponent.clone(mNextId));
        ++mNextId;

        return true;
    }

    return false;
}

void ComponentRegistry::clear ()
{
    mComponentIds.clear();
    mComponents.clear();
}
