#include "Scene.h"

#include <algorithm>
#include <scene/Entity.h>
#include <scene/components/BehaviourComponent.h>

Entity& Scene::createEntity(const std::string& name) {
	auto entity = std::unique_ptr<Entity>(new Entity(this, name));
	Entity& ref = *entity;

	ref.addComponent<TransformComponent>();

	entities.push_back(std::move(entity));
	return ref;
}

void Scene::removeEntity(Entity* entity) {
    if (!entity) return;

    auto& entities_s = entities;

    entities_s.erase(
        std::remove_if(entities_s.begin(), entities_s.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity;
            }),
        entities_s.end()
    );
}

void Scene::onCameraAdded(CameraComponent* camera)
{
	if (activeCamera == nullptr)
		activeCamera = camera;
}

void Scene::onUpdate(float dt) {
	for (const auto& entity : entities) {
		for (const auto& component : entity->getComponents()) {
			if (auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get())) {
				behaviour->onUpdate(dt);
			}
		}
	}
}

