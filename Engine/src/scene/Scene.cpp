#include "Scene.h"

#include <core/Input.h>
#include <scene/Entity.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/BehaviourComponent.h>
#include <scene/components/RigidBodyComponent.h>
#include <persistance/SceneSerializer.h>

void Scene::createDefaultScene(Scene& scene) {
	Entity& cameraEntity = scene.createEntityImmediate("Main Camera");
	cameraEntity.transform.setPosition(glm::vec3(0, 2, 6));
	auto& cam = cameraEntity.addComponent<CameraComponent>();
	scene.setActiveCamera(&cam);

	Entity& lightEntity = scene.createEntityImmediate("Directional Light");
	lightEntity.transform.setRotation(glm::vec3(-45.0f, -45.0f, 0.0f));
	auto& light = lightEntity.addComponent<DirectionalLightComponent>();
	light.color = glm::vec3(1.0f);
	light.ambientStrength = 0.2f;
	light.diffuseStrength = 0.8f;
	light.specularStrength = 0.5f;
}

Scene::Scene(const std::string& sceneName) : sceneName(sceneName) {
	physicsWorld.init();
}

Scene::~Scene() {
	entities.clear();
	physicsWorld.shutdown();
}

void Scene::newScene(const std::string& sceneName) {
	clear();
	this->sceneName = sceneName;

	createDefaultScene(*this);
}

void Scene::openScene(const std::string& sceneName) {
	clear();
	this->sceneName = sceneName;

	SceneSerializer::load(*this);
}

Entity& Scene::createEntity(const std::string& name, Entity* parent) {
	auto entity = std::unique_ptr<Entity>(new Entity(this, name, parent));
	Entity& ref = *entity;

	entitiesToAdd.push_back(std::move(entity));
	return ref;
}

Entity& Scene::createEntityImmediate(const std::string& name, Entity* parent) {
	auto entity = std::unique_ptr<Entity>(new Entity(this, name, parent));
	Entity& ref = *entity;
	entities.push_back(std::move(entity));
	return ref;
}

void Scene::removeEntity(Entity* entity) {
	if (onEntityRemoved)
		onEntityRemoved(entity);
	entitiesToRemove.push_back(entity);
}

void Scene::removeEntity(const std::string& name) {
	for (auto it = entities.begin(); it != entities.end(); it++) {
		if (it->get()->name.compare(name) == 0) {
			if (onEntityRemoved)
				onEntityRemoved(it->get());
			entitiesToRemove.push_back(it->get());
			return;
		}
	}
}

Entity* Scene::findEntity(const std::string& name) {
	for (auto& entity : entities) {
		if (entity->getName() == name)
			return entity.get();
	}
	return nullptr;
}

void Scene::setActiveCamera(CameraComponent* camera) {
	if (camera) activeCamera = camera;
	else {
		for (auto& entity : entities) {
			auto cam = entity->getComponent<CameraComponent>();
			if (cam) {
				activeCamera = cam;
				break;
			}
		}
	}
}

void Scene::onCameraAdded(CameraComponent* camera)
{
	if (activeCamera == nullptr)
		activeCamera = camera;
}

void Scene::onUpdate(float dt) {
	removeDeadEntities();
	addNewEntities();

	if (Input::isKeyPressed(Key::F5)) {
		if (!isPlaying) {
			SceneSerializer::save(*this, "_temp.json");
			isPlaying = true;
		}
		else {
			isPlaying = false;
			clear();
			SceneSerializer::load(*this, "_temp.json");
		}
	}

	if (!isPlaying) return;

	if (dt <= 0.0f || dt > 0.1f)
		dt = 1.0f / 60.0f;

	const float fixedPhysicsStep = 1.0f / 60.0f;
	physicsAccumulator += dt;
	colliderCacheTimer += dt;

	if (colliderCacheTimer > 1.0f) {
		physicsWorld.cleanShapeCache();
		colliderCacheTimer -= 1.0f;
	}

	bool isPhysicsLoop = physicsAccumulator >= fixedPhysicsStep;

	if (isPhysicsLoop)
		for (auto& entity : entities) {
			auto* rb = entity->getComponent<RigidBodyComponent>();
			if (rb) rb->pushToWorld();
		}

	while (physicsAccumulator >= fixedPhysicsStep) {
		physicsWorld.step(fixedPhysicsStep);
		physicsAccumulator -= fixedPhysicsStep;
	}

	if (isPhysicsLoop)
		for (auto& entity : entities) {
			auto* rb = entity->getComponent<RigidBodyComponent>();
			if (rb) rb->pullFromWorld();
		}

	for (const auto& entity : entities) {
		for (const auto& component : entity->getComponents()) {
			if (auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get())) {
				behaviour->tick(dt);
			}
		}
	}
}

void Scene::addNewEntities() {
	for (auto& entity : entitiesToAdd)
		entities.push_back(std::move(entity));
	entitiesToAdd.clear();
}

void Scene::removeDeadEntities() {
	for (auto* dead : entitiesToRemove) {
		for (auto it = entities.begin(); it != entities.end(); it++) {
			if (it->get() == dead) {
				entities.erase(it);
				break;
			}
		}
	}
	entitiesToRemove.clear();
}

void Scene::clear() {
	entities.clear();
	entitiesToAdd.clear();
	entitiesToRemove.clear();
	activeCamera = nullptr;
}