#pragma once

#include <unordered_map>
#include <functional>
#include <string>

#include <scene/Entity.h>

class Archive;

// For registering ONLY built-in components
#define REGISTER(Type) \
    inline bool _autoReg_##Type = []() { \
        ComponentFactory::registerBuiltIn(#Type, [](Entity& e, const Archive& j) { \
            auto& c = e.addComponentDeferred<Type>(); \
            c.deserialize(j); \
            c.onAttach(); \
        }); \
        return true; \
    }()

class ComponentFactory {
public:
    using Creator = std::function<void(Entity&, const Archive&)>;

    static void registerBuiltIn(const std::string& name, Creator creator);
    static void registerScript(const std::string& name, Creator creator);
    static void create(const std::string& name, Entity& entity, const Archive& arch);
    static void clearScriptRegistry() { getScriptRegistry().clear(); }

private:
    static std::unordered_map<std::string, Creator>& getScriptRegistry() {
        static std::unordered_map<std::string, Creator> scriptRegistry;
        return scriptRegistry;
    }

    static std::unordered_map<std::string, Creator>& getBuiltInRegistry() { 
        static std::unordered_map<std::string, Creator> builtInRegistry;
        return builtInRegistry;
    }
};