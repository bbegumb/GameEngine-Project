#pragma once

#include <scene/Scene.h>
#include <scene/components/Component.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/CameraComponent.h>

#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>

class Entity {
    friend class Scene;

public:
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    Entity(Entity&&) = delete;
    Entity& operator=(Entity&&) = delete;

    ~Entity() = default;

    const std::string& getName() const { return name; }
    Scene& getScene() const { return *scene; }

    TransformComponent& getTransform();
    const TransformComponent& getTransform() const;

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        T* existing = getComponent<T>();
        if (existing)
            throw std::runtime_error("Entity already has this component type");

        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;

        T* rawPtr = component.get();
        components.push_back(std::move(component));

        if constexpr (std::is_same_v<T, CameraComponent>) {
            scene->onCameraAdded(rawPtr);
        }

        return *rawPtr;
    }

    template<typename T>
    T* getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        for (auto& component : components) {
            if (auto casted = dynamic_cast<T*>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    template<typename T>
    const T* getComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        for (const auto& component : components) {
            if (auto casted = dynamic_cast<const T*>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        return getComponent<T>() != nullptr;
    }

    const std::vector<std::unique_ptr<Component>>& getComponents() const { return components; }
    std::vector<std::unique_ptr<Component>>& getComponents() { return components; }

private:
    Entity(Scene* owningScene, const std::string& entityName)
        : scene(owningScene), name(entityName) {}

private:
    Scene* scene = nullptr;
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};