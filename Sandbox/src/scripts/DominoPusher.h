#pragma once

#include <core/Script.h>

SCRIPT(DominoPusher) {
public:
    SERIALIZE(float, pushSpeed, 3.0f);
    SERIALIZE(float, pushDuration, 0.15f);

    void onUpdate(float dt) override {
        if (accum < pushDuration) {
            glm::vec3 pos = owner->transform.getPosition();
            pos.z += pushSpeed * dt;
            owner->transform.setPosition(pos);
            accum += dt;
        }
    }
private:
    float accum = 0.0f;
};
END_SCRIPT(DominoPusher);