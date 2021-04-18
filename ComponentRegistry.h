#ifndef COMPONENTREGISTRY_H
#define COMPONENTREGISTRY_H

#include <memory>
#include <string>
#include <unordered_map>

class Component;

class ComponentRegistry
{
public:
    static constexpr int MaxComponentCount = 32;

    static bool add (Component const & newComponent);

    static Component * find (int componentId)
    {
        auto componentsMapResult = mComponents.find(componentId);
        if (componentsMapResult == mComponents.end())
        {
            return nullptr;
        }

        return componentsMapResult->second.get();
    }

    template<typename T>
    static T * find ()
    {
        auto componentIdsMapResult = mComponentIds.find(T::PermanentId);
        if (componentIdsMapResult == mComponentIds.end())
        {
            return nullptr;
        }
        auto componentId = componentIdsMapResult->second;

        Component * component = find(componentId);
        if (component == nullptr)
        {
            return nullptr;
        }

        return static_cast<T *>(component);
    }

    static void clear ();

private:
    static int mNextId;
    static std::unordered_map<std::string, int> mComponentIds;
    static std::unordered_map<int, std::unique_ptr<Component>> mComponents;
};

#endif // COMPONENTREGISTRY_H
