#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <property/Property.h>
#include <json.hpp>

using json = nlohmann::json;

namespace PropertySerializer {
    inline void serialize(const PropertyHolder& holder, json& j) {
        for (const auto& prop : holder.getProperties()) {
            switch (prop.type) {
            case PropertyType::Bool:
                j[prop.name] = *static_cast<bool*>(prop.ptr);
                break;
            case PropertyType::Char:
                j[prop.name] = std::string(1, *static_cast<char*>(prop.ptr));
                break;
            case PropertyType::Int:
                j[prop.name] = *static_cast<int*>(prop.ptr);
                break;
            case PropertyType::Float:
                j[prop.name] = *static_cast<float*>(prop.ptr);
                break;
            case PropertyType::Double:
                j[prop.name] = *static_cast<double*>(prop.ptr);
                break;
            case PropertyType::String:
                j[prop.name] = *static_cast<std::string*>(prop.ptr);
                break;
            case PropertyType::Vec2: {
                auto& v = *static_cast<glm::vec2*>(prop.ptr);
                const float* p = glm::value_ptr(v);
                j[prop.name] = std::vector<float>(p, p + 2);
                break;
            }
            case PropertyType::Vec3: {
                auto& v = *static_cast<glm::vec3*>(prop.ptr);
                const float* p = glm::value_ptr(v);
                j[prop.name] = std::vector<float>(p, p + 3);
                break;
            }
            case PropertyType::Vec4: {
                auto& v = *static_cast<glm::vec4*>(prop.ptr);
                const float* p = glm::value_ptr(v);
                j[prop.name] = std::vector<float>(p, p + 4);
                break;
            }
            case PropertyType::Mat3: {
                auto& m = *static_cast<glm::mat3*>(prop.ptr);
                const float* p = glm::value_ptr(m);
                j[prop.name] = std::vector<float>(p, p + 9);
                break;
            }
            case PropertyType::Mat4: {
                auto& m = *static_cast<glm::mat4*>(prop.ptr);
                const float* p = glm::value_ptr(m);
                j[prop.name] = std::vector<float>(p, p + 16);
                break;
            }
            }
        }
    }

    inline void deserialize(PropertyHolder& holder, const json& j) {
        for (auto& prop : holder.getProperties()) {
            if (!j.contains(prop.name)) continue;
            switch (prop.type) {
            case PropertyType::Bool:
                *static_cast<bool*>(prop.ptr) = j[prop.name];
                break;
            case PropertyType::Char:
                *static_cast<char*>(prop.ptr) = j[prop.name].get<std::string>()[0];
                break;
            case PropertyType::Int:
                *static_cast<int*>(prop.ptr) = j[prop.name];
                break;
            case PropertyType::Float:
                *static_cast<float*>(prop.ptr) = j[prop.name];
                break;
            case PropertyType::Double:
                *static_cast<double*>(prop.ptr) = j[prop.name];
                break;
            case PropertyType::String:
                *static_cast<std::string*>(prop.ptr) = j[prop.name];
                break;
            case PropertyType::Vec2: {
                auto v = j[prop.name].get<std::vector<float>>();
                *static_cast<glm::vec2*>(prop.ptr) = glm::make_vec2(v.data());
                break;
            }
            case PropertyType::Vec3: {
                auto v = j[prop.name].get<std::vector<float>>();
                *static_cast<glm::vec3*>(prop.ptr) = glm::make_vec3(v.data());
                break;
            }
            case PropertyType::Vec4: {
                auto v = j[prop.name].get<std::vector<float>>();
                *static_cast<glm::vec4*>(prop.ptr) = glm::make_vec4(v.data());
                break;
            }
            case PropertyType::Mat3: {
                auto m = j[prop.name].get<std::vector<float>>();
                *static_cast<glm::mat3*>(prop.ptr) = glm::make_mat3(m.data());
                break;
            }
            case PropertyType::Mat4: {
                auto m = j[prop.name].get<std::vector<float>>();
                *static_cast<glm::mat4*>(prop.ptr) = glm::make_mat4(m.data());
                break;
            }
            }
        }
    }
}