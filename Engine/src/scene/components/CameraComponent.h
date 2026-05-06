#pragma once
#include <glm/glm.hpp>

#include <scene/components/Component.h>
 

class CameraComponent : public Component {
public:
	CameraComponent(float fov, float aspect, float nearPlane, float farPlane) 
		: fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane) {}

	float fov = 45.0f;
	float aspect = 1.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	void onAttach();
	void onDetach();
};