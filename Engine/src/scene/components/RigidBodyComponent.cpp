#include "RigidBodyComponent.h"

#include <scene/Entity.h>
#include <physics/PhysicsWorld.h>
#include <glm/glm.hpp>

RigidBodyComponent::~RigidBodyComponent() {
	cleanup();
}

void RigidBodyComponent::init() {
	PhysicsWorld& physicsWorld = owner->getScene().getPhysicsWorld();

	PxPhysics* physics = physicsWorld.getPhysics();
	mat = physics->createMaterial(0.5f, 0.3f, 0.3f);

	glm::vec3 scale = owner->getTransform().getScale();
	PxBoxGeometry boxGeometry = PxBoxGeometry(toPx(scale * 0.5f));

	glm::vec3 pos = owner->getTransform().getPosition();
	glm::vec3 rot = owner->getTransform().getRotation();
	glm::quat q = glm::angleAxis(rot.z, glm::vec3(0, 0, 1))
		* glm::angleAxis(rot.y, glm::vec3(0, 1, 0))
		* glm::angleAxis(rot.x, glm::vec3(1, 0, 0));
	PxTransform pxTransform(toPx(pos), toPx(q));

	if (type == Static) {
		actor = PxCreateStatic(*physics, pxTransform, boxGeometry, *mat);
	}
	else if (type == Dynamic) {
		actor = PxCreateDynamic(*physics, pxTransform, boxGeometry, *mat, 1.0f);
	}
	else {
		actor = PxCreateDynamic(*physics, pxTransform, boxGeometry, *mat, 1.0f);
		static_cast<PxRigidDynamic*>(actor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	}

	physicsWorld.addActor(*actor);
}

void RigidBodyComponent::cleanup() {
	if (!actor) return;

	PhysicsWorld& physicsWorld = owner->getScene().getPhysicsWorld();
	physicsWorld.removeActor(*actor);
	actor->release();
	actor = nullptr;

	if (mat) {
		mat->release();
		mat = nullptr;
	}
}

void RigidBodyComponent::sync() {
	if (type == Dynamic) {
		PxTransform pxTransform = actor->getGlobalPose();
		owner->getTransform().setPosition(toGlm(pxTransform.p));
		owner->getTransform().setRotation(glm::eulerAngles(toGlm(pxTransform.q)));
	}
	else if (type == Kinematic) {
		glm::vec3 pos = owner->getTransform().getPosition();
		glm::vec3 rot = owner->getTransform().getRotation();
		glm::quat q(rot);
		static_cast<PxRigidDynamic*>(actor)->setKinematicTarget(PxTransform(toPx(pos), toPx(q)));
	}
}

void RigidBodyComponent::onAttach() {
	init();
}

void RigidBodyComponent::onDetach() {
	cleanup();
}