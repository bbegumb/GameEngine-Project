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
    if (viewportWidth <= 0.0f || viewportHeight <= 0.0f) return;

    Scene* scene = SceneController::getScene();
    if (!scene) return;

    auto& picker = getInstance().getColourPicker();
    picker.resize((int)viewportWidth, (int)viewportHeight);

    Entity* clicked = picker.pickEntity(mouseX, mouseY);
    EntityController::setSelectedEntity(clicked);
}
