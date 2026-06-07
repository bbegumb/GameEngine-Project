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