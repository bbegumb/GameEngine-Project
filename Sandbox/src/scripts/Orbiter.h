#pragma once

#include <core/Script.h>

SCRIPT(Orbiter) {
public:
    SERIALIZE(glm::vec3, center, glm::vec3(0));
    SERIALIZE(float, radius, 3.0f);
    SERIALIZE(float, speed, 1.0f);
    SERIALIZE(float, height, 2.0f);
    SERIALIZE(float, startAngle, 0.0f);

    void onUpdate(float dt) override {
        if (!started) { accum = startAngle; started = true; }
        accum += dt * speed;
        float x = center.x + radius * glm::cos(accum);
        float z = center.z + radius * glm::sin(accum);
        owner->transform.setPosition(glm::vec3(x, height, z));
    }
private:
    float accum = 0.0f;
    bool started = false;
};
END_SCRIPT(Orbiter);