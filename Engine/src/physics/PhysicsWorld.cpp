#include "PhysicsWorld.h"

void PhysicsWorld::init() {
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	PxTolerancesScale scale;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale);

	PxInitExtensions(*gPhysics, nullptr);

	PxSceneDesc sceneDesc(scale);
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
}

void PhysicsWorld::shutdown() {
	gScene->release();
	gDispatcher->release();
	PxCloseExtensions();
	gPhysics->release();
	gFoundation->release();
}

void PhysicsWorld::step(float dt) {
	gScene->simulate(dt);
	gScene->fetchResults(true);
}

void PhysicsWorld::addActor(PxActor& actor) {
	gScene->addActor(actor);
}

void PhysicsWorld::removeActor(PxActor& actor) {
	gScene->removeActor(actor);
}