#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <cstddef>
#include <functional>
#include <scene/components/Component.h>

class Entity;

enum class FieldType {
    Float,
    Int,
    Bool,
    Vec3,
    Unknown
};

struct FieldInfo {
    std::string name;
    FieldType type;
    std::function<void*(Component*)> getAddress;
};

struct TypeInfo {
    std::string name;
    std::vector<FieldInfo> fields;
    std::function<void(Entity*)> addToEntity;
};

class ReflectionRegistry {
public:
    static std::unordered_map<std::string, TypeInfo>& getTypes() {
        static std::unordered_map<std::string, TypeInfo> types;
        return types;
    }

    static std::unordered_map<std::type_index, std::string>& getTypeNames() {
        static std::unordered_map<std::type_index, std::string> names;
        return names;
    }

    template<typename T>
    static void registerType(const TypeInfo& info) {
        getTypes()[info.name] = info;
        getTypeNames()[std::type_index(typeid(T))] = info.name;
    }

    static TypeInfo* getType(const std::string& name) {
        auto& types = getTypes();
        auto it = types.find(name);
        if (it == types.end()) return nullptr;
        return &it->second;
    }

    static TypeInfo* getType(Component* component) {
        auto& names = getTypeNames();
        auto it = names.find(std::type_index(typeid(*component)));

        if (it == names.end()) return nullptr;

        return getType(it->second);
    }
};
