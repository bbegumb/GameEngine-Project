#pragma once

#include <glm/glm.hpp>
#include <memory>

class ShaderProgram;
class Scene;
class Entity;
class CameraComponent;
class MeshComponent;
class MaterialComponent;
class DirectionalLightComponent;
class PointLightComponent;

class Renderer {
public:
	Renderer();
	~Renderer();

	void setClearColor(const glm::vec4& color) { clearColor = color; }
	void render(const Scene& scene) const;

private:
	void initShadowMap();

	void drawEntity(const Entity& entity,
					const MeshComponent* meshComponent,
					const MaterialComponent* materialComponent,
					const CameraComponent* camera,
					const DirectionalLightComponent* dirLight,
					const PointLightComponent* pointLight,
					const glm::mat4 lightSpaceMatrix) const;

	void drawEntityDepth(const Entity& entity,
						 const MeshComponent* meshComponent,
						 const glm::mat4& lightSpaceMatrix) const;

private:
	glm::vec4 clearColor{ 0.1f, 0.1f, 0.1f, 1.0f };

	unsigned int shadowFBO = 0;
	unsigned int shadowDepthTexture = 0;
	static const int SHADOW_WIDTH  = 8192;
	static const int SHADOW_HEIGHT = 8192;

	std::unique_ptr<ShaderProgram> depthShader;
};