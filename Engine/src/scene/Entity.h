#pragma once

#include <persistance/Serializable.h>

#include <scene/Scene.h>
#include <scene/components/Component.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/CameraComponent.h>

#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>

class Entity : Serializable {
    friend class Scene;

public:
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    Entity(Entity&&) = delete;
    Entity& operator=(Entity&&) = delete;

    ~Entity() = default;

    int getID() const { return ID; }

    void serialize(nlohmann::json& j) const override;
    void deserialize(const nlohmann::json& j) override {}

    const std::string& getName() const { return name; }
    Scene& getScene() const { return *scene; }

    TransformComponent& getTransform();
    const TransformComponent& getTransform() const;

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        static_assert(!std::is_same_v<T, TransformComponent>,
            "TransformComponent is built-in, use getTransform()");

        T* existing = getComponent<T>();
        if (existing)
            throw std::runtime_error("Entity already has this component type");

        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;

        T* rawPtr = component.get();
        static_cast<Component*>(rawPtr)->setID(nextCompID++);
        components.push_back(std::move(component));

        if (!rawPtr->onAttach()) {
            components.pop_back();
            throw std::runtime_error("Component rejected attachment");
        }

        rawPtr->onAttach();

        return *rawPtr;
    }

    template<typename T, typename... Args>
    T& addComponentDeferred(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        component->owner = this;

        T* rawPtr = component.get();
        static_cast<Component*>(rawPtr)->setID(nextCompID++);
        components.push_back(std::move(component));
        
        return *rawPtr;
    }

    template<typename T>
    void removeComponent() {
        for (auto it = components.begin(); it != components.end(); it++) {
            if (dynamic_cast<T*>(it->get())) {
                (*it)->onDetach();
                components.erase(it);
                return;
            }
        }
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

    TransformComponent transform;

private:
    Entity(Scene* owningScene, const std::string& entityName, Entity* parent = nullptr);
    Entity(Scene* owningScene, const std::string& entityName,
        const glm::vec3& pos, const glm::vec3& rot,
        const glm::vec3& scale, Entity* parent = nullptr);

private:

    void setID(int newID) { ID = newID; }
    int ID;

    int nextCompID = 0;

    Scene* scene = nullptr;
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};