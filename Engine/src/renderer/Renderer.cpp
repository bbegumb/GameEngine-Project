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
#include <controller/EntityController.h>
#include <controller/EditorSelectionController.h>


Renderer::Renderer() {
	initShadowMap();

	depthShader = std::make_unique<ShaderProgram>("depth");
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

void Renderer::render(const Scene& scene) {
	CameraComponent* cam = scene.getActiveCamera();
	if (!cam) return;
	render(scene, cam->getViewMatrix(), cam->getProjectionMatrix());
}

void Renderer::render(const Scene& scene, const glm::mat4& view, const glm::mat4& proj) {
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
	glm::mat4 view = activeCamera->getViewMatrix();
	glm::mat4 projection = activeCamera->getProjectionMatrix();

	auto& picker = EditorSelectionController::getInstance().getColourPicker();
	picker.resize(viewport[2], viewport[3]);
	picker.renderPickingPass(&const_cast<Scene&>(scene), view, projection,
		[&](unsigned int pickShader, Entity* e, const glm::mat4& mvp) {
		glUseProgram(pickShader);
		glUniformMatrix4fv(
			glGetUniformLocation(pickShader, "uMVP"),
			1, GL_FALSE, glm::value_ptr(mvp));
		glm::vec3 col = picker.getPickedColour(e);
		glUniform3fv(
			glGetUniformLocation(pickShader, "uColour"),
			1, glm::value_ptr(col));
		auto* mc = e->getComponent<MeshComponent>();
		if (mc && mc->mesh) mc->mesh->draw();
	});

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& entityPtr : scene.getEntities()) {
		const Entity& entity = *entityPtr;

		const MeshComponent* meshComponent = entity.getComponent<MeshComponent>();
		const MaterialComponent* materialComponent = entity.getComponent<MaterialComponent>();
		if (!meshComponent || !materialComponent) continue;

		drawEntity(entity, meshComponent, materialComponent, view, proj, glm::vec3(glm::inverse(view)[3]), dirLight, pointLight, lightSpaceMatrix);
	}
}

void Renderer::drawEntity(const Entity& entity,
	const MeshComponent* meshComponent,
	const MaterialComponent* materialComponent,
	const glm::mat4& view,
	const glm::mat4& proj,
	const glm::vec3& cameraPosition,
	const DirectionalLightComponent* dirLight,
	const PointLightComponent* pointLight,
	const glm::mat4& lightSpaceMatrix) const {

	if (!meshComponent->mesh) return;

	const glm::mat4 model = entity.getTransform().getMatrix();

	auto drawWithMaterial = [&](Material& material) {
		ShaderProgram& shader = *material.shader;
		shader.use();
		material.apply();

		shader.setMat4("uModel", model);
		shader.setMat4("uView", view);
		shader.setMat4("uProj", proj);
		shader.setMat4("uLightSpaceMatrix", lightSpaceMatrix);
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
	};

	const auto& submeshes = meshComponent->mesh->getSubMeshes();

	if (submeshes.empty()) {
		Material* mat = materialComponent->getMaterial(0);
		if (!mat || !mat->shader) return;
		drawWithMaterial(*mat);
		meshComponent->mesh->draw();
	}
	else {
		for (int i = 0; i < static_cast<int>(submeshes.size()); i++) {
			Material* mat = materialComponent->getMaterial(submeshes[i].materialIndex);
			if (!mat) mat = materialComponent->getMaterial(0);
			if (!mat || !mat->shader) continue;
			drawWithMaterial(*mat);
			meshComponent->mesh->drawSubMesh(i);
		}
	}
}

void Renderer::drawEntityDepth(const Entity& entity,
	const MeshComponent* meshComponent,
	const glm::mat4& lightSpaceMatrix) const {
	glm::mat4 model = entity.getTransform().getMatrix();
	depthShader->setMat4("uLightSpaceMatrix", lightSpaceMatrix);
	depthShader->setMat4("uModel", model);
    meshComponent->mesh->draw();
}
