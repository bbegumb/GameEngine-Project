#include "ComponentFactory.h"

#include <iostream>

std::unordered_map<std::string, ComponentFactory::Creator>& ComponentFactory::getRegistry() {
    static std::unordered_map<std::string, Creator> registry;
    return registry;
}

void ComponentFactory::registerType(const std::string& name, Creator creator) {
    getRegistry()[name] = creator;
}

void ComponentFactory::create(const std::string& name, Entity& entity, const nlohmann::json& j) {
    auto& registry = getRegistry();
    auto it = registry.find(name);
    if (it != registry.end())
        it->second(entity, j);
    else
        printf("Unknown component type: %s\n", name.c_str());
}