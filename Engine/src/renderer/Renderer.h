#pragma once

#include <glm/glm.hpp>

class Scene;
class Entity;
class CameraComponent;
class MeshComponent;
class MaterialComponent;
class DirectionalLightComponent;
class PointLightComponent;

class Renderer {
public:
	void setClearColor(const glm::vec4& color) { clearColor = color; }
	void render(const Scene& scene) const;

private:
	void drawEntity(const Entity& entity,
					const MeshComponent* meshComponent,
					const MaterialComponent* materialComponent,
					const CameraComponent* camera,
					const DirectionalLightComponent* dirLight,
					const PointLightComponent* pointLight) const;

private:
	glm::vec4 clearColor{ 0.1f, 0.1f, 0.1f, 1.0f };
};