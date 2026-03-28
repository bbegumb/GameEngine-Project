#include "Renderer.h"

#include <scene/Scene.h>
#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/PointLightComponent.h>

#include <renderer/ShaderProgram.h>
#include <renderer/Mesh.h>
#include <renderer/Material.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Renderer::render(const Scene& scene) const {
	glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CameraComponent* activeCamera = scene.getActiveCamera();
	if (!activeCamera) return;

	const DirectionalLightComponent* dirLight = nullptr;
	const PointLightComponent* pointLight = nullptr;

	for (const auto& entityPtr : scene.getEntities()) {
		const Entity& entity = *entityPtr;

		if (!dirLight)
			dirLight = entity.getComponent<DirectionalLightComponent>();

		if (!pointLight)
			pointLight = entity.getComponent<PointLightComponent>();
	}

	for (const auto& entityPtr : scene.getEntities()) {
		const Entity& entity = *entityPtr;

		const MeshComponent* meshComponent = entity.getComponent<MeshComponent>();
		const MaterialComponent* materialComponent = entity.getComponent<MaterialComponent>();
		if (!meshComponent || !materialComponent) continue;

		drawEntity(entity, meshComponent, materialComponent, activeCamera, dirLight, pointLight);
	}
}

void Renderer::drawEntity(const Entity& entity,
	const MeshComponent* meshComponent,
	const MaterialComponent* materialComponent,
	const CameraComponent* camera,
	const DirectionalLightComponent* dirLight,
	const PointLightComponent* pointLight) const {

	if (!meshComponent->mesh || !materialComponent->material->shader) return;

	const glm::mat4 model = entity.getTransform().getMatrix();
	const glm::mat4 view = camera->getViewMatrix();
	const glm::mat4 proj = camera->getProjectionMatrix();

	glm::vec3 cameraPosition = camera->getEntity()->getTransform().getPosition();

	Material& material = *(materialComponent->material);
	ShaderProgram& shader = *(material.shader);

	material.apply();
	shader.use();

	shader.setMat4("uModel", model);
	shader.setMat4("uView", view);
	shader.setMat4("uProj", proj);
	shader.setVec3("uViewPos", cameraPosition);

	if (dirLight) {
		shader.setBool("uHasDirectionalLight", true);
		shader.setVec3("uDirectionalLight.direction", dirLight->direction);
		shader.setVec3("uDirectionalLight.color", dirLight->color);
		shader.setFloat("uDirectionalLight.ambientIntensity", dirLight->ambientStrength);
		shader.setFloat("uDirectionalLight.diffuseIntensity", dirLight->diffuseStrength);
		shader.setFloat("uDirectionalLight.specularIntensity", dirLight->specularStrength);
	}
	else {
		shader.setBool("uHasDirectionalLight", false);
	}

	if (pointLight) {
		glm::vec3 pointLightPos = pointLight->getEntity()->getTransform().getPosition();

		shader.setBool("uHasPointLight", true);
		shader.setVec3("uPointLight.position", pointLightPos);
		shader.setVec3("uPointLight.color", pointLight->color);
		shader.setFloat("uPointLight.ambientIntensity", pointLight->ambientStrength);
		shader.setFloat("uPointLight.diffuseIntensity", pointLight->diffuseStrength);
		shader.setFloat("uPointLight.specularIntensity", pointLight->specularStrength);
		shader.setFloat("uPointLight.constant", pointLight->constant);
		shader.setFloat("uPointLight.linear", pointLight->linear);
		shader.setFloat("uPointLight.quadratic", pointLight->quadratic);
	}
	else {
		shader.setBool("uHasPointLight", false);
	}

	meshComponent->mesh->draw();
}