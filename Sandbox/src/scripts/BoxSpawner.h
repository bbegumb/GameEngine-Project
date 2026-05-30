#pragma once

#include <core/Script.h>

SCRIPT(BoxSpawner) {

public:
    void onUpdate(float dt) override;
};

END_SCRIPT(BoxSpawner);