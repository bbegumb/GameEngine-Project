#pragma once

#include <scene/components/Component.h>
#include <glm/glm.hpp>

class DirectionalLightComponent : public Component {
public:
	glm::vec3 direction = glm::vec3{ -0.2f, -1.0f, -0.3f };
	glm::vec3 color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	float ambientStrength = 0.2f;
	float diffuseStrength = 1.0f;
	float specularStrength = 0.5f;
};