#pragma once

#include <scene/components/Component.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DirectionalLightComponent : public Component {
public:
	glm::vec3 direction = glm::vec3{ -0.2f, -1.0f, -0.3f };
	glm::vec3 color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	float ambientStrength = 0.2f;
	float diffuseStrength = 1.0f;
	float specularStrength = 0.5f;

    float shadowDistance = 30.0f;
	float shadowOrthoSize = 40.0f;
	float shadowNear = 0.1f;
	float shadowFar = 200.0f;

    glm::mat4 getLightSpaceMatrix(const CameraComponent* camera) const {
        glm::vec3 camPos = camera->getEntity()->getTransform().getPosition();
        glm::vec3 camForward = camera->getEntity()->getTransform().forward();
        glm::vec3 center = camPos + camForward * shadowDistance * 0.5f;

        glm::vec3 lightDir = glm::normalize(direction);
        glm::vec3 lightPos = center - lightDir * shadowFar * 0.5f;

        glm::mat4 view = glm::lookAt(lightPos, center, glm::vec3(0, 1, 0));
        glm::mat4 proj = glm::ortho(
            -shadowOrthoSize, shadowOrthoSize,
            -shadowOrthoSize, shadowOrthoSize,
            shadowNear, shadowFar
        );
        return proj * view;
    }
};