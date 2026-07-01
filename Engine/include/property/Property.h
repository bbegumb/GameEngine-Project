#pragma once

#include <string>
#include <vector>

inline std::string camelToWords(const std::string& s) {
    std::string result;
    for (int i = 0; i < s.size(); i++) {
        if (std::isupper(s[i]) && i > 0)
            result += ' ';
        result += s[i];
    }
    result[0] = std::toupper(result[0]);
    return result;
}

enum class PropertyType { Bool, Char, Int, Float, Double, String, Vec2, Vec3, Vec4, Mat3, Mat4 };

struct Property {
    std::string name;
    PropertyType type;
    void* ptr;
};

class PropertyHolder {
public:
    void registerProperty(const std::string& name, PropertyType type, void* ptr) {
        properties.push_back({ camelToWords(name), type, ptr});
    }

    const std::vector<Property>& getProperties() const { return properties; }

    template<typename T>
    T* getPropertyValue(const std::string& name) {
        for (auto& prop : properties) {
            if (prop.name == name)
                return static_cast<T*>(prop.ptr);
        }
        return nullptr;
    }

protected:
    std::vector<Property> properties;
};