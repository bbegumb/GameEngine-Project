#include "RigidBodyComponent.h"

#include <persistance/ComponentFactory.h>
#include <scene/Entity.h>
#include <scene/components/MeshComponent.h>

#include <physics/body/StaticPhysicsBody.h>
#include <physics/body/DynamicPhysicsBody.h>

bool RigidBodyComponent::onAttach() {
    if (owner->getTransform().getParent() != nullptr) {
        printf("Warning: RigidBodyComponent not supported on child entities.\n");
        return false;
    }

    if (!material)
        material = std::make_shared<PhysicsMaterial>();

    PhysicsWorld& world = owner->getScene().getPhysicsWorld();
    glm::vec3 pos = owner->transform.getPosition();
    glm::quat rot = owner->transform.getRotationQuat();

    glm::vec3 scale = owner->transform.getScale();
    lastScale = scale;
    
    if (!shape) {
        auto* mc = owner->getComponent<MeshComponent>();
        if (mc && mc->mesh)
            shape = world.getOrCreateShape(mc->mesh.get(), scale, forceConvex);
        else
            shape = std::make_shared<CollisionShape>(
                CollisionShape::boxMesh(owner->transform.getScale() * 0.5f));
    }

    if (type == RigidBodyType::Static) {
        body = std::make_unique<StaticPhysicsBody>(&world, pos, rot, material, shape, scale);
    }
    else if (type == RigidBodyType::Dynamic) {
        body = std::make_unique<DynamicPhysicsBody>(&world, pos, rot, material, shape, scale);
    }
    else if (type == RigidBodyType::Kinematic) {
        body = std::make_unique<DynamicPhysicsBody>(&world, pos, rot, material, shape, scale, true);
    }

    owner->getTransform().onBeforeReparent = []() {
        printf("Warning: Cannot reparent entity with RigidBodyComponent.\n");
        return false;
    };

    return true;
}

void RigidBodyComponent::onDetach() {
    body.reset();

    owner->getTransform().onBeforeReparent = nullptr;
}

void RigidBodyComponent::pushToWorld() {
    glm::vec3 currentScale = owner->transform.getScale();

    if (currentScale != lastScale) {
        lastScale = currentScale;
        recookShape();
    }

    glm::vec3 pos = owner->transform.getPosition();
    glm::quat rot = owner->transform.getRotationQuat();

    if (type == RigidBodyType::Dynamic) {
        static_cast<DynamicPhysicsBody*>(body.get())->setGlobalPose(pos, rot);
    }
    else if (type == RigidBodyType::Kinematic) {
        static_cast<DynamicPhysicsBody*>(body.get())->setKinematicTarget(pos, rot);
    }
}

void RigidBodyComponent::pullFromWorld() {
    if (type == RigidBodyType::Dynamic) {
        owner->transform.setPosition(body->getPosition());
        owner->transform.setRotation(body->getRotation());
    }
}

void RigidBodyComponent::recookShape() {
    body->clearShapes();

    auto* mc = owner->getComponent<MeshComponent>();
    if (mc && mc->mesh) {
        PhysicsWorld& world = owner->getScene().getPhysicsWorld();
        shape = world.getOrCreateShape(mc->mesh.get(), owner->transform.getScale(), forceConvex);
    }

    body->attachShape(shape, owner->transform.getScale());
}

void RigidBodyComponent::serialize(nlohmann::json& j) const {
    switch (type) {
        case Static:    j["bodyType"] = "Static"; break;
        case Dynamic:   j["bodyType"] = "Dynamic"; break;
        case Kinematic: j["bodyType"] = "Kinematic"; break;
    }
    j["forceConvex"] = forceConvex;

    if (material) {
        j["staticFriction"] = material->staticFriction;
        j["dynamicFriction"] = material->dynamicFriction;
        j["restitution"] = material->restitution;
    }
}

void RigidBodyComponent::deserialize(const nlohmann::json& j) {
    std::string bt = j.value("bodyType", "Dynamic");
    if (bt == "Static") type = Static;
    else if (bt == "Kinematic") type = Kinematic;
    else type = Dynamic;

    forceConvex = j.value("forceConvex", false);

    if (j.contains("staticFriction")) {
        material = std::make_shared<PhysicsMaterial>(
            j.value("staticFriction", 0.5f),
            j.value("dynamicFriction", 0.5f),
            j.value("restitution", 0.01f));
    }
}

void RigidBodyComponent::setType(RigidBodyType newType) {
    if (newType == type) return;
    type = newType;
    rebuild();
}

void RigidBodyComponent::rebuild() {
    body.reset();
    onAttach();
}

REGISTER(RigidBodyComponent);