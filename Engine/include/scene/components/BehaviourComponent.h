#pragma once

#include <scene/components/Component.h>
#include <property/Property.h>
#include <property/SerializeMacro.h>

class Scene;
class Archive;

class BehaviourComponent : public Component, public PropertyHolder {
public:
    friend class Scene;

    virtual ~BehaviourComponent() = default;
    virtual void onStart() {}
    virtual void onUpdate(float dt) {}

    void serialize(Archive& arch) const override;
    void deserialize(const Archive& arch) override;

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