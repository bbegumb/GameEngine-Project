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
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
	initShadowMap();

	depthShader = std::make_unique<ShaderProgram>("depth.vert", "depth.frag");
}

Renderer::~Renderer() {
	if (shadowFBO) glDeleteFramebuffers(1, &shadowFBO);
	if (shadowDepthTexture) glDeleteTextures(1, &shadowDepthTexture);
}

void Renderer::initShadowMap() {
	glGenFramebuffers(1, &shadowFBO);

	glGenTextures(1, &shadowDepthTexture);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							GL_TEXTURE_2D, shadowDepthTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render(const Scene& scene) const {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLint previousFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);

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

	glm::mat4 lightSpaceMatrix(1.0f);

	if (dirLight) {
		lightSpaceMatrix = dirLight->getLightSpaceMatrix(activeCamera);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//glCullFace(GL_FRONT);
		//glEnable(GL_CULL_FACE);

		depthShader->use();

		for (const auto& entiyPtr : scene.getEntities()) {
			const Entity& entity = *entiyPtr;
			const MeshComponent* mc = entity.getComponent<MeshComponent>();
			const MaterialComponent* matc = entity.getComponent<MaterialComponent>();
			if (!mc || !matc || !mc->mesh) continue;

			drawEntityDepth(entity, mc, lightSpaceMatrix);
		}

		//glCullFace(GL_BACK);
		//glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
	}

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& entityPtr : scene.getEntities()) {
		const Entity& entity = *entityPtr;

		const MeshComponent* meshComponent = entity.getComponent<MeshComponent>();
		const MaterialComponent* materialComponent = entity.getComponent<MaterialComponent>();
		if (!meshComponent || !materialComponent) continue;

		drawEntity(entity, meshComponent, materialComponent, activeCamera, dirLight, pointLight, lightSpaceMatrix);
	}
}

void Renderer::drawEntity(const Entity& entity,
						  const MeshComponent* meshComponent,
						  const MaterialComponent* materialComponent,
						  const CameraComponent* camera,
						  const DirectionalLightComponent* dirLight,
						  const PointLightComponent* pointLight,
						  const glm::mat4 lightSpaceMatrix) const {

	if (!meshComponent->mesh || !materialComponent->material->shader) return;

	const glm::mat4 model = entity.getTransform().getMatrix();
	const glm::mat4 view = camera->getViewMatrix();
	const glm::mat4 proj = camera->getProjectionMatrix();

	glm::vec3 cameraPosition = camera->getEntity()->getTransform().getPosition();

	Material& material = *(materialComponent->material);
	ShaderProgram& shader = *(material.shader);

	material.apply();
	shader.use();

	shader.setMat4("uLightSpaceMatrix", lightSpaceMatrix);
	shader.setMat4("uModel", model);
	shader.setMat4("uView", view);
	shader.setMat4("uProj", proj);
	shader.setVec3("uViewPos", cameraPosition);

	if (dirLight) {
		shader.setBool("uHasDirectionalLight", true);
		shader.setVec3("uDirectionalLight.direction", dirLight->getDirection());
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

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
	shader.setInt("uShadowMap", 4);

	meshComponent->mesh->draw();
}

void Renderer::drawEntityDepth(const Entity& entity,
							   const MeshComponent *meshComponent,
							   const glm::mat4& lightSpaceMatrix) const {
	glm::mat4 model = entity.getTransform().getMatrix();
	depthShader->setMat4("uLightSpaceMatrix", lightSpaceMatrix);
	depthShader->setMat4("uModel", model);
	meshComponent->mesh->draw();
}