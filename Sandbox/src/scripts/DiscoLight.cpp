#include "DiscoLight.h"

#include <scene/Entity.h>
#include <scene/components/PointLightComponent.h>
#include <scene/components/MaterialComponent.h>
#include <cmath>

void DiscoLight::onUpdate(float dt) {
    timer += dt * speed;

    auto* light = owner->getComponent<PointLightComponent>();
    if (!light) return;

    // Cycle RGB with offset phases
    float r = (std::sin(timer) + 1.0f) * 0.5f;
    float g = (std::sin(timer + 2.094f) + 1.0f) * 0.5f;  // +120 degrees
    float b = (std::sin(timer + 4.189f) + 1.0f) * 0.5f;  // +240 degrees

    light->color = glm::vec3(r, g, b);
    light->diffuseStrength = intensity;

    auto matc = owner->getComponent<MaterialComponent>();
    if (matc) {
        int i = 0;
        for (; i < matc->getMaterialCount(); i++) {
            auto mat = matc->getMaterial(i);
            mat->albedo = glm::vec3(r, g, b);
            mat->emission = glm::vec3(r, g, b);
        }
    }
}