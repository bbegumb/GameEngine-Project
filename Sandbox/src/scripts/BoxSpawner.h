#pragma once

#include <core/Script.h>

SCRIPT(BoxSpawner) {

public:
    int count = 0;

    void onUpdate(float dt) override;
};

END_SCRIPT(BoxSpawner);