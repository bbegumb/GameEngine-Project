#include "RigidBodyComponent.h"

#include <scene/Entity.h>
#include <scene/components/MeshComponent.h>

#include <physics/body/StaticPhysicsBody.h>
#include <physics/body/DynamicPhysicsBody.h>

void RigidBodyComponent::onAttach() {
    if (!material)
        material = std::make_shared<PhysicsMaterial>();

    PhysicsWorld& world = owner->getScene().getPhysicsWorld();
    glm::vec3 pos = owner->getTransform().getPosition();
    glm::quat rot = owner->getTransform().getRotationQuat();

    glm::vec3 scale = owner->getTransform().getScale();
    lastScale = scale;
    
    if (!shape) {
        auto* mc = owner->getComponent<MeshComponent>();
        if (mc && mc->mesh)
            shape = world.getOrCreateShape(mc->mesh.get(), scale);
        else
            shape = std::make_shared<CollisionShape>(
                CollisionShape::boxMesh(owner->getTransform().getScale() * 0.5f));
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
}

void RigidBodyComponent::onDetach() {
    body.reset();
}

void RigidBodyComponent::pushToWorld() {
    glm::vec3 currentScale = owner->getTransform().getScale();

    if (currentScale != lastScale) {
        lastScale = currentScale;
        recookShape();
    }

    glm::vec3 pos = owner->getTransform().getPosition();
    glm::quat rot = owner->getTransform().getRotationQuat();

    if (type == RigidBodyType::Dynamic) {
        static_cast<DynamicPhysicsBody*>(body.get())->setGlobalPose(pos, rot);
    }
    else if (type == RigidBodyType::Kinematic) {
        static_cast<DynamicPhysicsBody*>(body.get())->setKinematicTarget(pos, rot);
    }
}

void RigidBodyComponent::pullFromWorld() {
    if (type == RigidBodyType::Dynamic) {
        owner->getTransform().setPosition(body->getPosition());
        owner->getTransform().setRotation(body->getRotation());
    }
}

void RigidBodyComponent::recookShape() {
    body->clearShapes();

    auto* mc = owner->getComponent<MeshComponent>();
    if (mc && mc->mesh) {
        PhysicsWorld& world = owner->getScene().getPhysicsWorld();
        shape = world.getOrCreateShape(mc->mesh.get(), owner->getTransform().getScale());
    }

    body->attachShape(shape, owner->getTransform().getScale());
}
