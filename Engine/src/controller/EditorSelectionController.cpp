#include "EditorSelectionController.h"

#include <controller/SceneController.h>
#include <controller/EntityController.h>

#include <scene/Scene.h>
#include <scene/Entity.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/MeshComponent.h>
#include <renderer/Mesh.h>

#include <glm/gtc/matrix_transform.hpp>

#include <limits>
#include <cmath>

void EditorSelectionController::selectEntityFromViewport(
    float mouseX,
    float mouseY,
    float viewportWidth,
    float viewportHeight
) {
    if (viewportWidth <= 0.0f || viewportHeight <= 0.0f)
        return;

    Scene* scene = SceneController::getScene();
    if (!scene || !scene->getActiveCamera())
        return;

    Entity* pickedEntity = pickEntityWithRay(mouseX, mouseY, viewportWidth, viewportHeight);
    EntityController::setSelectedEntity(pickedEntity);
}

Entity* EditorSelectionController::pickEntityWithRay(
    float mouseX,
    float mouseY,
    float viewportWidth,
    float viewportHeight
) {
    Scene* scene = SceneController::getScene();
    CameraComponent* camera = scene->getActiveCamera();

    glm::mat4 projection = camera->getProjectionMatrix();
    glm::mat4 view       = camera->getViewMatrix();
    glm::mat4 viewProj   = projection * view;

    const float pixelRadius = 1000.0f;

    Entity* closestEntity = nullptr;
    float   closestDist   = std::numeric_limits<float>::max();

    for (Entity* entity : SceneController::getEntityPointers()) {
        if (!entity) continue;

        auto mc = entity->getComponent<MeshComponent>();
        if (!mc) continue;

        TransformComponent& transform = entity->transform;
        
        glm::vec3 localCenter(0.0f);

        if (mc && mc->mesh)
            localCenter = mc->mesh->computeCentroid();

        glm::mat4 model    = transform.getMatrix();
        glm::vec4 worldPos = model * glm::vec4(localCenter, 1.0f);

        glm::vec4 clip = viewProj * worldPos;
        if (clip.w <= 0.0f) continue;

        glm::vec3 ndc   = glm::vec3(clip) / clip.w;
        float screenX   = (ndc.x * 0.5f + 0.5f) * viewportWidth;
        float screenY   = (1.0f - (ndc.y * 0.5f + 0.5f)) * viewportHeight;

        float dx   = screenX - mouseX;
        float dy   = screenY - mouseY;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= pixelRadius && dist < closestDist) {
            closestDist   = dist;
            closestEntity = entity;
        }
    }

    return closestEntity;
}
