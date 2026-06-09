#pragma once

#include <core/Script.h>

// Start the script
SCRIPT(KinematicMover) {

public:
    // Serialized fields, these are saved with the scene
    SERIALIZE(float, speed, 3.0f);
    SERIALIZE(float, range, 15.0f); 

    // Called once
    void onStart() override {
        startPos = owner->transform.getPosition();
    }

    // Called per frame
    void onUpdate(float dt) override {
        // Periodic movement
        accumulatedTime += dt * speed;
        if (accumulatedTime > glm::two_pi<float>())
            accumulatedTime -= glm::two_pi<float>();

        float x = startPos.x + std::sin(accumulatedTime) * range;

        // Set position through Transform component
        owner->transform.setPosition(glm::vec3(x, startPos.y, startPos.z));
    }

private:
    // Private, unserialized fields that are NOT saved with the scene
    float accumulatedTime = 0.0f;
    glm::vec3 startPos = glm::vec3(0.0f);
};

END_SCRIPT(KinematicMover);
// End the script