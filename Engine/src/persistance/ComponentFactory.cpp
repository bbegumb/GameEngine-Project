#include <persistance/ComponentFactory.h>

#include <persistance/Archive.h>
#include <scene/Entity.h>

using Creator = std::function<void(Entity&, const Archive&)>;

void ComponentFactory::registerBuiltIn(const std::string& name, Creator creator) {
    ComponentFactory::getBuiltInRegistry()[name] = creator;
}

void ComponentFactory::registerScript(const std::string& name, Creator creator) {
    ComponentFactory::getScriptRegistry()[name] = creator;
}

void ComponentFactory::create(const std::string& name, Entity& entity, const Archive& arch) {
    auto& builtInRegistry = getBuiltInRegistry();
    auto it = builtInRegistry.find(name);
    if (it != builtInRegistry.end())
        it->second(entity, arch);
    else {
        auto& scriptRegistry = getScriptRegistry();
        auto it = scriptRegistry.find(name);
        if (it != scriptRegistry.end())
            it->second(entity, arch);
        else
            printf("Unknown component type: %s\n", name.c_str());
    }
}
