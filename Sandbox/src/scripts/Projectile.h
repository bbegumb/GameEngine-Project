#pragma once

#include <core/Script.h>

SCRIPT(Projectile) {
public:
    SERIALIZE(glm::vec3, velocity, glm::vec3(0, 0, 10));
    SERIALIZE(float, lifetime, 4.0f);

    void onUpdate(float dt) override {
        if (age < lifetime) {
            owner->transform.setPosition(
                owner->transform.getPosition() + velocity * dt);
        }
        age += dt;
    }
private:
    float age = 0.0f;
};
END_SCRIPT(Projectile);