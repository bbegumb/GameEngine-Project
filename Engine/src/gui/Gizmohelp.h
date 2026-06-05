#pragma once

#include <renderer/LineRenderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Entity;

enum class GizmoMode {
    Translate,
    Rotate,
    Scale
};

enum class GizmoAxis {
    None, X, Y, Z
};

class Gizmo {
public:
    Gizmo();

    void draw(
        Entity* selectedEntity,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    bool handleInput(
        Entity* selectedEntity,
        const glm::mat4& view,
        const glm::mat4& projection,
        float vpX, float vpY,       
        float vpW, float vpH        
    );

    static void setMode(GizmoMode newMode);
    static GizmoMode getMode();
    
private:
    static GizmoMode currentMode;
    LineRenderer lineRenderer;
    float length = 2.0f;

    GizmoAxis  activeAxis = GizmoAxis::None;
    bool       isDragging = false;
    float      dragStartT = 0.0f;  
    glm::vec3  dragStartPos = {};
    glm::vec3  dragStartScale = {};
    glm::quat  dragStartRot = {};
    glm::vec3  dragAxis = {};

    float projectMouseOnAxis(
        glm::vec2 mouse,
        glm::vec3 origin, glm::vec3 axisDir,
        const glm::mat4& viewProj,
        float vpW, float vpH) const;

    glm::vec2 worldToScreen(
        glm::vec3 world,
        const glm::mat4& viewProj,
        float vpW, float vpH) const;

    GizmoAxis pickAxis(
        glm::vec2 mouse,
        glm::vec3 origin,
        glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ,
        const glm::mat4& viewProj,
        float vpW, float vpH) const;

    void drawTranslate(
        const glm::vec3& pos,
        const glm::mat4& rotation,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawScale(
        const glm::vec3& pos,
        const glm::mat4& rotation,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawRotate(
        const glm::vec3& pos,
		const glm::mat4& rotation,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawCone(
        const glm::vec3& tip,
        const glm::vec3& direction, 
        float height,
        float baseRadius,
        const glm::vec3& color,
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void drawCircle(
        const glm::vec3& center,
        const glm::vec3& axisU,     
        const glm::vec3& axisV,
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
