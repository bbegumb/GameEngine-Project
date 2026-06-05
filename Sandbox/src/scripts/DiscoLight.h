#pragma once

#include <core/Script.h>

SCRIPT(DiscoLight) {

public:
    SERIALIZE(float, speed, 3.0f);
    SERIALIZE(float, intensity, 1.5f);
    SERIALIZE(float, radius, 3.0f);

    float timer = 0.0f;

    void onUpdate(float dt) override;
};

END_SCRIPT(DiscoLight);