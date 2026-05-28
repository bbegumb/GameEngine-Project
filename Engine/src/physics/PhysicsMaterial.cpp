#include "PhysicsMaterial.h"

#include <physics/PhysicsWorld.h>

PhysicsMaterial::~PhysicsMaterial() {
	pxMat = nullptr;
}

physx::PxMaterial* PhysicsMaterial::getOrCreate(PhysicsWorld* world) {
	if (!pxMat && world)
		pxMat = world->getPhysics()->createMaterial(staticFriction, dynamicFriction, restitution);
	else if (!pxMat && !world) return nullptr;
	
	return pxMat;
}