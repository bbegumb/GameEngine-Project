#pragma once

#include <core/Script.h>

SCRIPT(Rotator) {

public:
    SERIALIZE(float, speed, 45.0f);
    SERIALIZE(glm::vec3, axis, glm::vec3(0, 1, 0));
    SERIALIZE(bool, enabled, true);

    void onUpdate(float dt) override;
};

END_SCRIPT(Rotator);