#include "Gizmohelp.h"

#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>
#include <cmath>

Gizmo::Gizmo() {
    
}

GizmoMode Gizmo::currentMode = GizmoMode::Translate;

void Gizmo::setMode(GizmoMode newMode) {
    currentMode = newMode;
}

GizmoMode Gizmo::getMode() {
    return currentMode;
}

void Gizmo::draw(
    Entity* selectedEntity,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    if (!selectedEntity)
        return;

    auto* transform = selectedEntity->getComponent<TransformComponent>();

    if (!transform)
        return;

    glm::vec3 pos = transform->getPosition();
    
    if (currentMode == GizmoMode::Translate) {
        drawTranslate(pos, view, projection);
    }
    else if (currentMode == GizmoMode::Scale) {
        drawScale(pos, view, projection);
    }
    else if (currentMode == GizmoMode::Rotate) {
        drawRotate(pos, view, projection);
    }
    
}

void Gizmo::drawTranslate(const glm::vec3& pos,const glm::mat4& view,const glm::mat4& projection) {
    lineRenderer.drawLine(pos, pos + glm::vec3(length, 0, 0), glm::vec3(1, 0, 0), view, projection);
    lineRenderer.drawLine(pos, pos + glm::vec3(0, length, 0), glm::vec3(0, 1, 0), view, projection);
    lineRenderer.drawLine(pos, pos + glm::vec3(0, 0, length), glm::vec3(0, 0, 1), view, projection);
}

void Gizmo::drawScale(const glm::vec3& pos,const glm::mat4& view,const glm::mat4& projection) {
    float scaleLength = length * 0.8f;
    float cubeSize = length * 0.12f;

    lineRenderer.drawLine(pos, pos + glm::vec3(scaleLength, 0, 0), glm::vec3(1, 0, 0), view, projection);
    lineRenderer.drawLine(pos, pos + glm::vec3(0, scaleLength, 0), glm::vec3(0, 1, 0), view, projection);
    lineRenderer.drawLine(pos, pos + glm::vec3(0, 0, scaleLength), glm::vec3(0, 0, 1), view, projection);

    drawCube(pos + glm::vec3(scaleLength, 0, 0), cubeSize, glm::vec3(1, 0, 0), view, projection);
    drawCube(pos + glm::vec3(0, scaleLength, 0), cubeSize, glm::vec3(0, 1, 0), view, projection);
    drawCube(pos + glm::vec3(0, 0, scaleLength), cubeSize, glm::vec3(0, 0, 1), view, projection);
}

void Gizmo::drawCube(const glm::vec3& center, float halfSize, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection) {
    float h = halfSize;

    glm::vec3 corners[8] = {
        center + glm::vec3(-h, -h, -h),
        center + glm::vec3( h, -h, -h),
        center + glm::vec3( h,  h, -h),
        center + glm::vec3(-h,  h, -h),
        center + glm::vec3(-h, -h,  h),
        center + glm::vec3( h, -h,  h),
        center + glm::vec3( h,  h,  h),
        center + glm::vec3(-h,  h,  h),
    };


    lineRenderer.drawLine(corners[0], corners[1], color, view, projection);
    lineRenderer.drawLine(corners[1], corners[2], color, view, projection);
    lineRenderer.drawLine(corners[2], corners[3], color, view, projection);
    lineRenderer.drawLine(corners[3], corners[0], color, view, projection);
    lineRenderer.drawLine(corners[4], corners[5], color, view, projection);
    lineRenderer.drawLine(corners[5], corners[6], color, view, projection);
    lineRenderer.drawLine(corners[6], corners[7], color, view, projection);
    lineRenderer.drawLine(corners[7], corners[4], color, view, projection);
    lineRenderer.drawLine(corners[0], corners[4], color, view, projection);
    lineRenderer.drawLine(corners[1], corners[5], color, view, projection);
    lineRenderer.drawLine(corners[2], corners[6], color, view, projection);
    lineRenderer.drawLine(corners[3], corners[7], color, view, projection);
}

void Gizmo::drawRotate(const glm::vec3& pos,const glm::mat4& view,const glm::mat4& projection) {
    drawCircle(pos, glm::vec3(1, 0, 0), length, glm::vec3(1, 0, 0), view, projection);
    drawCircle(pos, glm::vec3(0, 1, 0), length, glm::vec3(0, 1, 0), view, projection);
    drawCircle(pos, glm::vec3(0, 0, 1), length, glm::vec3(0, 0, 1), view, projection);
}

void Gizmo::drawCircle(const glm::vec3& center,const glm::vec3& normal,float radius,const glm::vec3& color,const glm::mat4& view,const glm::mat4& projection) {
    const int segments = 64;

    glm::vec3 a;
    glm::vec3 b;

    if (normal.x == 1.0f) {
        for (int i = 0; i < segments; i++) {
            float t1 = (float)i / segments * 2.0f * 3.14159f;
            float t2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

            a = center + glm::vec3(0, cos(t1) * radius, sin(t1) * radius);
            b = center + glm::vec3(0, cos(t2) * radius, sin(t2) * radius);

            lineRenderer.drawLine(a, b, color, view, projection);
        }
    }
    else if (normal.y == 1.0f) {
        for (int i = 0; i < segments; i++) {
            float t1 = (float)i / segments * 2.0f * 3.14159f;
            float t2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

            a = center + glm::vec3(cos(t1) * radius, 0, sin(t1) * radius);
            b = center + glm::vec3(cos(t2) * radius, 0, sin(t2) * radius);

            lineRenderer.drawLine(a, b, color, view, projection);
        }
    }
    else {
        for (int i = 0; i < segments; i++) {
            float t1 = (float)i / segments * 2.0f * 3.14159f;
            float t2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

            a = center + glm::vec3(cos(t1) * radius, sin(t1) * radius, 0);
            b = center + glm::vec3(cos(t2) * radius, sin(t2) * radius, 0);

            lineRenderer.drawLine(a, b, color, view, projection);
        }
    }
}
