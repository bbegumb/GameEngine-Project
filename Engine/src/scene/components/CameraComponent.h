#pragma once
#include <glm/glm.hpp>

#include <scene/components/Component.h>

class CameraComponent : public Component {
public:
	CameraComponent(float fov = 45.0f, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 100.0f) 
		: fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane) {}

	float fov;
	float aspect;
	float nearPlane;
	float farPlane;

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	bool onAttach();
	void onDetach();

	void serialize(nlohmann::json& j) const override;
	void deserialize(const nlohmann::json& j) override;
};