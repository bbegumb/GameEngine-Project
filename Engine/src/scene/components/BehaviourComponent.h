#pragma once

#include <scene/components/Component.h>
#include <property/Property.h>
#include <property/SerializeMacro.h>

class Scene;

class BehaviourComponent : public Component, public PropertyHolder {
public:
    friend class Scene;

    virtual ~BehaviourComponent() = default;
    virtual void onStart() {}
    virtual void onUpdate(float dt) {}

    void serialize(nlohmann::json& j) const override;
    void deserialize(const nlohmann::json& j) override;

private:
    void tick(float dt) {
        if (!started) {
            onStart();
            started = true;
        }
        onUpdate(dt);
    }

    bool started = false;
};