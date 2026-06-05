#pragma once

#include <glm/glm.hpp>

class Entity;

enum class GizmoMode {
    Translate,
    Rotate,
    Scale
};

class Gizmo {
public:
    Gizmo();

    void draw(
        Entity* selectedEntity,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    static void setMode(GizmoMode newMode);
    static GizmoMode getMode();

private:
    static GizmoMode currentMode;
};