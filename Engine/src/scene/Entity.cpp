#include "Entity.h"

#include <glm/gtc/type_ptr.hpp>
#include <scene/components/TransformComponent.h>

inline std::string cleanTypeName(const std::string& name) {
    const std::string prefix = "class ";
    if (name.substr(0, prefix.size()) == prefix)
        return name.substr(prefix.size());
    return name;
}

Entity::Entity(Scene* owningScene, const std::string& entityName, Entity* parent) 
: scene(owningScene), name(entityName) {
	transform.owner = this;

	if (parent) {
		transform.setParent(&(parent->transform));
	}
}

Entity::Entity(Scene* owningScene, const std::string& entityName,
	const glm::vec3& pos, const glm::vec3& rot,
	const glm::vec3& scale, Entity* parent) : Entity(owningScene, entityName, parent) {
	transform.setPosition(pos);
	transform.setRotation(rot);
	transform.setScale(scale);
}

void Entity::serialize(nlohmann::json& j) const {
    j["name"] = name;

    TransformComponent* parent = transform.getParent();
    if (parent && parent->owner)
        j["parent"] = parent->owner->getName();
    else
        j["parent"] = nullptr;

    const float* pos = glm::value_ptr(transform.getPosition());
    glm::quat rot = transform.getRotationQuat();
    const float* scl = glm::value_ptr(transform.getScale());

    j["transform"]["position"] = std::vector<float>(pos, pos + 3);
    j["transform"]["rotation"] = { rot.x, rot.y, rot.z, rot.w };
    j["transform"]["scale"] = std::vector<float>(scl, scl + 3);

    j["components"] = nlohmann::json::array();
    for (const auto& comp : components) {
        nlohmann::json cj;
        cj["type"] = cleanTypeName(typeid(*comp).name());
        comp->serialize(cj);
        j["components"].push_back(cj);
    }
}

TransformComponent& Entity::getTransform() {
	return transform;
}

const TransformComponent& Entity::getTransform() const {
	return transform;
}
