#pragma once

#include <glm/glm.hpp>

class Entity;

class EditorSelectionController {
public:
    static void selectEntityFromViewport(
        float mouseX,
        float mouseY,
        float viewportWidth,
        float viewportHeight
    );

private:
    static Entity* pickEntityWithRay(
        float mouseX,
        float mouseY,
        float viewportWidth,
        float viewportHeight
    );
};
