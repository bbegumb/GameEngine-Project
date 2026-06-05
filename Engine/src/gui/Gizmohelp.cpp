#include "Gizmohelp.h"

#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <cmath>
#include <algorithm>

static constexpr float PI = 3.14159265358979323846f;
static constexpr float PICK_RADIUS = 12.0f;

Gizmo::Gizmo() {}

GizmoMode Gizmo::currentMode = GizmoMode::Translate;

void Gizmo::setMode(GizmoMode newMode) { 
    currentMode = newMode;
}
GizmoMode Gizmo::getMode() {
    return currentMode;
}

static void getAxes(Entity* e,
    glm::vec3& pos,
    glm::vec3& axisX,
    glm::vec3& axisY,
    glm::vec3& axisZ)
{
    glm::mat4 model = e->transform.getMatrix();
    pos = glm::vec3(model[3]);

    axisX = glm::normalize(glm::vec3(model[0]));
    axisY = glm::normalize(glm::vec3(model[1]));
    axisZ = glm::normalize(glm::vec3(model[2]));
}

glm::vec2 Gizmo::worldToScreen(glm::vec3 world,
    const glm::mat4& viewProj,
    float vpW, float vpH) const
{
    glm::vec4 clip = viewProj * glm::vec4(world, 1.0f);
    if (std::abs(clip.w) < 1e-6f) return { -9999.f, -9999.f };
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    return {
        (ndc.x * 0.5f + 0.5f) * vpW,
        (1.0f - (ndc.y * 0.5f + 0.5f)) * vpH  
    };
}
float Gizmo::projectMouseOnAxis(glm::vec2 mouse,
    glm::vec3 origin, glm::vec3 axisDir,
    const glm::mat4& viewProj,
    float vpW, float vpH) const
{
    // Sample two points along the axis and project both to screen
    glm::vec2 s0 = worldToScreen(origin, viewProj, vpW, vpH);
    glm::vec2 s1 = worldToScreen(origin + axisDir, viewProj, vpW, vpH);
    glm::vec2 sd = s1 - s0;
    float len2 = glm::dot(sd, sd);
    if (len2 < 1e-6f) return 0.0f;
    // Project mouse onto that screen line, result is world-unit t
    return glm::dot(mouse - s0, sd) / len2;
}

GizmoAxis Gizmo::pickAxis(glm::vec2 mouse,
    glm::vec3 origin,
    glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ,
    const glm::mat4& viewProj,
    float vpW, float vpH) const
{
    float bestDist = PICK_RADIUS;
    GizmoAxis best = GizmoAxis::None;

    auto check = [&](glm::vec3 axis, GizmoAxis candidate) {
        glm::vec2 s0 = worldToScreen(origin, viewProj, vpW, vpH);
        glm::vec2 s1 = worldToScreen(origin + axis * length, viewProj, vpW, vpH);
        glm::vec2 d = s1 - s0;
        float len2 = glm::dot(d, d);
        if (len2 < 1e-6f) return;

        float t = glm::clamp(glm::dot(mouse - s0, d) / len2, 0.0f, 1.0f);
        glm::vec2 closest = s0 + d * t;
        float dist = glm::length(mouse - closest);
        if (dist < bestDist) {
            bestDist = dist;
            best = candidate;
        }
        };

    check(axisX, GizmoAxis::X);
    check(axisY, GizmoAxis::Y);
    check(axisZ, GizmoAxis::Z);
    return best;
}

void Gizmo::draw(
    Entity* selectedEntity,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    if (!selectedEntity) return;

    auto& transform = selectedEntity->transform;
    glm::vec3 pos = transform.getPosition();

    glm::vec3 rotEuler = transform.getRotation();   
    glm::mat4 rotMat = glm::mat4(1.0f);
    rotMat = glm::rotate(rotMat, glm::radians(rotEuler.y), glm::vec3(0, 1, 0));
    rotMat = glm::rotate(rotMat, glm::radians(rotEuler.x), glm::vec3(1, 0, 0));
    rotMat = glm::rotate(rotMat, glm::radians(rotEuler.z), glm::vec3(0, 0, 1));

    switch (currentMode) {
    case GizmoMode::Translate: drawTranslate(pos, rotMat, view, projection); break;
    case GizmoMode::Scale:     drawScale(pos, rotMat, view, projection); break;
    case GizmoMode::Rotate:    drawRotate(pos, rotMat, view, projection); break;
    }
}

bool Gizmo::handleInput(Entity* selectedEntity,
    const glm::mat4& view,
    const glm::mat4& projection,
    float vpX, float vpY,
    float vpW, float vpH)
{
    if (!selectedEntity || vpW <= 0.f || vpH <= 0.f) return false;

    glm::mat4 viewProj = projection * view;

    glm::vec3 pos, axisX, axisY, axisZ;
    getAxes(selectedEntity, pos, axisX, axisY, axisZ);

    ImVec2 imMouse = ImGui::GetMousePos();
    glm::vec2 mouse = { imMouse.x - vpX, imMouse.y - vpY };

    bool lmbDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool lmbClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool lmbReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    if (lmbReleased) {
        isDragging = false;
        activeAxis = GizmoAxis::None;
        return false;
    }

    if (lmbClicked && !isDragging) {
        GizmoAxis hovered = pickAxis(mouse, pos, axisX, axisY, axisZ, viewProj, vpW, vpH);
        if (hovered != GizmoAxis::None) {
            activeAxis = hovered;
            isDragging = true;
            dragStartPos = selectedEntity->transform.getPosition();
            dragStartScale = selectedEntity->transform.getScale();
            dragStartRot = selectedEntity->transform.getRotationQuat();

            switch (hovered) {
            case GizmoAxis::X: dragAxis = axisX; break;
            case GizmoAxis::Y: dragAxis = axisY; break;
            case GizmoAxis::Z: dragAxis = axisZ; break;
            default: break;
            }

            dragStartT = projectMouseOnAxis(mouse, dragStartPos, dragAxis, viewProj, vpW, vpH);
            return true;   
        }
    }

    if (isDragging && lmbDown) {
        float currentT = projectMouseOnAxis(mouse, dragStartPos, dragAxis, viewProj, vpW, vpH);
        float delta = currentT - dragStartT;

        TransformComponent& t = selectedEntity->transform;

        switch (currentMode) {
        case GizmoMode::Translate:
            t.setPosition(dragStartPos + dragAxis * delta);
            break;

        case GizmoMode::Scale: {
            glm::vec3 s = dragStartScale;
            float factor = 1.0f + delta * 0.5f;
            factor = std::max(factor, 0.01f);
            switch (activeAxis) {
            case GizmoAxis::X: s.x *= factor; break;
            case GizmoAxis::Y: s.y *= factor; break;
            case GizmoAxis::Z: s.z *= factor; break;
            default: break;
            }
            t.setScale(s);
            break;
        }

        case GizmoMode::Rotate: {
            float angle = delta * PI;  
            t.setRotation(glm::angleAxis(angle, dragAxis) * dragStartRot);
            break;
        }
        }
        return true;
    }

    return false;
}

void Gizmo::drawTranslate(
    const glm::vec3& pos,
    const glm::mat4& rotMat,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glm::vec3 axisX = glm::vec3(rotMat * glm::vec4(1, 0, 0, 0));
    glm::vec3 axisY = glm::vec3(rotMat * glm::vec4(0, 1, 0, 0));
    glm::vec3 axisZ = glm::vec3(rotMat * glm::vec4(0, 0, 1, 0));

    float shaftLen = length * 0.80f;
    float coneH = length * 0.20f;
    float coneR = length * 0.06f;

    glm::vec3 shaftEndX = pos + axisX * shaftLen;
    glm::vec3 shaftEndY = pos + axisY * shaftLen;
    glm::vec3 shaftEndZ = pos + axisZ * shaftLen;

    glm::vec3 arrowTipX = shaftEndX + axisX * coneH;
    glm::vec3 arrowTipY = shaftEndY + axisY * coneH;
    glm::vec3 arrowTipZ = shaftEndZ + axisZ * coneH;

    lineRenderer.drawLine(pos, shaftEndX, glm::vec3(1, 0, 0), view, projection);
    lineRenderer.drawLine(pos, shaftEndY, glm::vec3(0, 1, 0), view, projection);
    lineRenderer.drawLine(pos, shaftEndZ, glm::vec3(0, 0, 1), view, projection);

    drawCone(arrowTipX, axisX, coneH, coneR, glm::vec3(1, 0, 0), view, projection);
    drawCone(arrowTipY, axisY, coneH, coneR, glm::vec3(0, 1, 0), view, projection);
    drawCone(arrowTipZ, axisZ, coneH, coneR, glm::vec3(0, 0, 1), view, projection);
}

void Gizmo::drawScale(
    const glm::vec3& pos,
    const glm::mat4& rotMat,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glm::vec3 axisX = glm::vec3(rotMat * glm::vec4(1, 0, 0, 0));
    glm::vec3 axisY = glm::vec3(rotMat * glm::vec4(0, 1, 0, 0));
    glm::vec3 axisZ = glm::vec3(rotMat * glm::vec4(0, 0, 1, 0));

    float shaftLen = length * 0.85f;
    float cubeH = length * 0.10f;

    glm::vec3 endX = pos + axisX * shaftLen;
    glm::vec3 endY = pos + axisY * shaftLen;
    glm::vec3 endZ = pos + axisZ * shaftLen;

    lineRenderer.drawLine(pos, endX, glm::vec3(1, 0, 0), view, projection);
    lineRenderer.drawLine(pos, endY, glm::vec3(0, 1, 0), view, projection);
    lineRenderer.drawLine(pos, endZ, glm::vec3(0, 0, 1), view, projection);

    drawCube(endX, cubeH, glm::vec3(1, 0, 0), view, projection);
    drawCube(endY, cubeH, glm::vec3(0, 1, 0), view, projection);
    drawCube(endZ, cubeH, glm::vec3(0, 0, 1), view, projection);
    drawCube(pos, cubeH * 0.75f, glm::vec3(1, 1, 1), view, projection);
}

void Gizmo::drawRotate(
    const glm::vec3& pos,
    const glm::mat4& rotMat,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glm::vec3 axisX = glm::vec3(rotMat * glm::vec4(1, 0, 0, 0));
    glm::vec3 axisY = glm::vec3(rotMat * glm::vec4(0, 1, 0, 0));
    glm::vec3 axisZ = glm::vec3(rotMat * glm::vec4(0, 0, 1, 0));
    drawCircle(pos, axisY, axisZ, length, glm::vec3(1, 0, 0), view, projection);
    drawCircle(pos, axisX, axisZ, length, glm::vec3(0, 1, 0), view, projection);
    drawCircle(pos, axisX, axisY, length, glm::vec3(0, 0, 1), view, projection);
}


void Gizmo::drawCone(
    const glm::vec3& tip,
    const glm::vec3& direction,
    float height,
    float baseRadius,
    const glm::vec3& color,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 base = tip - dir * height;
    glm::vec3 ref = (std::abs(dir.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    glm::vec3 u = glm::normalize(glm::cross(dir, ref));
    glm::vec3 v = glm::cross(dir, u);

    const int segs = 12;
    glm::vec3 prev = base + u * baseRadius;

    for (int i = 1; i <= segs; ++i) {
        float     angle = i * 2.0f * PI / segs;
        glm::vec3 cur = base + (u * std::cos(angle) + v * std::sin(angle)) * baseRadius;

        lineRenderer.drawLine(prev, cur, color, view, projection);

        if (i % 3 == 0)
            lineRenderer.drawLine(cur, tip, color, view, projection);

        prev = cur;
    }
    lineRenderer.drawLine(base + u * baseRadius, tip, color, view, projection);
    lineRenderer.drawLine(base - u * baseRadius, tip, color, view, projection);
    lineRenderer.drawLine(base + v * baseRadius, tip, color, view, projection);
    lineRenderer.drawLine(base - v * baseRadius, tip, color, view, projection);
}

void Gizmo::drawCircle(
    const glm::vec3& center,
    const glm::vec3& axisU,
    const glm::vec3& axisV,
    float radius,
    const glm::vec3& color,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    const int segments = 64;
    glm::vec3 prev = center + axisU * radius;

    for (int i = 1; i <= segments; ++i) {
        float     t = i * 2.0f * PI / segments;
        glm::vec3 cur = center + (axisU * std::cos(t) + axisV * std::sin(t)) * radius;
        lineRenderer.drawLine(prev, cur, color, view, projection);
        prev = cur;
    }
}

void Gizmo::drawCube(
    const glm::vec3& center,
    float halfSize,
    const glm::vec3& color,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    float h = halfSize;
    glm::vec3 c[8] = {
        center + glm::vec3(-h,-h,-h), center + glm::vec3(h,-h,-h),
        center + glm::vec3(h, h,-h), center + glm::vec3(-h, h,-h),
        center + glm::vec3(-h,-h, h), center + glm::vec3(h,-h, h),
        center + glm::vec3(h, h, h), center + glm::vec3(-h, h, h),
    };

    lineRenderer.drawLine(c[0], c[1], color, view, projection);
    lineRenderer.drawLine(c[1], c[2], color, view, projection);
    lineRenderer.drawLine(c[2], c[3], color, view, projection);
    lineRenderer.drawLine(c[3], c[0], color, view, projection);
    lineRenderer.drawLine(c[4], c[5], color, view, projection);
    lineRenderer.drawLine(c[5], c[6], color, view, projection);
    lineRenderer.drawLine(c[6], c[7], color, view, projection);
    lineRenderer.drawLine(c[7], c[4], color, view, projection);
    lineRenderer.drawLine(c[0], c[4], color, view, projection);
    lineRenderer.drawLine(c[1], c[5], color, view, projection);
    lineRenderer.drawLine(c[2], c[6], color, view, projection);
    lineRenderer.drawLine(c[3], c[7], color, view, projection);
}