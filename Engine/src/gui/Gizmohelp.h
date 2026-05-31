#pragma once

#include <renderer/LineRenderer.h>
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
    LineRenderer lineRenderer;
    float length = 2.0f;
    void drawTranslate(
        const glm::vec3& pos,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawScale(
        const glm::vec3& pos,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawRotate(
        const glm::vec3& pos,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawCircle(
        const glm::vec3& center,
        const glm::vec3& normal,
        float radius,
        const glm::vec3& color,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawCube(
        const glm::vec3& center,
        float halfSize,
        const glm::vec3& color,
        const glm::mat4& view,
        const glm::mat4& projection
    );
};
