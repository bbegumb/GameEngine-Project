#pragma once

#include <unordered_map>
#include <string>
#include <json.hpp>

#include <scene/Entity.h>

#define REGISTER(Type) \
    inline bool _autoReg_##Type = []() { \
        ComponentFactory::registerType(#Type, [](Entity& e, const nlohmann::json& j) { \
            auto& c = e.addComponentDeferred<Type>(); \
            c.deserialize(j); \
            c.onAttach(); \
        }); \
        return true; \
    }()

class ComponentFactory {
public:
    using Creator = std::function<void(Entity&, const nlohmann::json&)>;

    static void registerType(const std::string& name, Creator creator);
    static void create(const std::string& name, Entity& entity, const nlohmann::json& j);

private:
    static std::unordered_map<std::string, Creator>& getRegistry();
};