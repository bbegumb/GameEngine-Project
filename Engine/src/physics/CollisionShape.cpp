#include "CollisionShape.h"

#include <physics/PhysicsWorld.h>
#include <physics/PhysicsMaterial.h>
#include <PxPhysicsAPI.h>

using namespace physx;

CollisionShape CollisionShape::boxMesh(const glm::vec3& halfExtents) {
	CollisionShape s;
	s.type = CollisionShapeType::BoxMesh;
	s.halfExtents = halfExtents;
	return s;
}

CollisionShape CollisionShape::sphereMesh(float radius) {
	CollisionShape s;
	s.type = CollisionShapeType::SphereMesh;
	s.radius = radius;
	return s;
}

CollisionShape CollisionShape::convexMesh(const std::vector<Vertex>& meshVertices) {
	CollisionShape s;
	s.type = CollisionShapeType::ConvexMesh;
	s.vertices.reserve(meshVertices.size());
	for (const Vertex& v : meshVertices)
		s.vertices.push_back(v.position);
	return s;
}

CollisionShape CollisionShape::triangleMesh(const std::vector<Vertex>& meshVertices,
	const std::vector<unsigned int>& meshIndices) {
	CollisionShape s;
	s.type = CollisionShapeType::TriangleMesh;
	s.vertices.reserve(meshVertices.size());
	for (const Vertex& v : meshVertices)
		s.vertices.push_back(v.position);
	s.indices = meshIndices;
	return s;
}

PxShape* CollisionShape::getOrCreatePxShape(PhysicsWorld* world, PhysicsMaterial* mat, const glm::vec3& scale) {
	if (pxShape) return pxShape;

	PxPhysics* physics = world->getPhysics();
	PxMaterial* pxMat = mat->getOrCreate(world);

	switch (type) {
		case CollisionShapeType::BoxMesh:
			pxShape = physics->createShape(PxBoxGeometry(toPx(halfExtents)), *pxMat, false);
			break;
		case CollisionShapeType::SphereMesh:
			pxShape = physics->createShape(PxSphereGeometry(radius), *pxMat, false);
			break;
		case CollisionShapeType::ConvexMesh: {
			PxConvexMeshDesc desc;
			desc.points.count = static_cast<PxU32>(vertices.size());
			desc.points.stride = sizeof(PxVec3);
			desc.points.data = vertices.data();
			desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
			desc.vertexLimit = 255;
			desc.polygonLimit = 255;
			desc.quantizedCount = 255;

			PxDefaultMemoryOutputStream buf;
			PxCookingParams params(physics->getTolerancesScale());

			if (!PxCookConvexMesh(params, desc, buf)) {
				printf("Convex cooking failed! Vertex count: %d\n", (int)vertices.size());
				return nullptr;
			}

			PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			PxConvexMesh* convexMesh = physics->createConvexMesh(input);

			if (!convexMesh) {
				printf("createConvexMesh returned null!\n");
				return nullptr;
			}

			PxMeshScale meshScale(toPx(scale));
			pxShape = physics->createShape(PxConvexMeshGeometry(convexMesh, meshScale), *pxMat, false);
			break;
		}
		case CollisionShapeType::TriangleMesh: {
			PxTriangleMeshDesc desc;
			desc.points.count = static_cast<PxU32>(vertices.size());
			desc.points.stride = sizeof(glm::vec3);
			desc.points.data = vertices.data();
			desc.triangles.count = static_cast<PxU32>(indices.size() / 3);
			desc.triangles.stride = 3 * sizeof(unsigned int);
			desc.triangles.data = indices.data();

			PxDefaultMemoryOutputStream buf;
			PxCookingParams params(physics->getTolerancesScale());
			PxCookTriangleMesh(params, desc, buf);

			if (!PxCookTriangleMesh(params, desc, buf)) {
				printf("Triangle mesh cooking failed! Vertex count: %d\n", (int)vertices.size());
				return nullptr;
			}

			PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			PxTriangleMesh* triangleMesh = physics->createTriangleMesh(input);

			if (!triangleMesh) {
				printf("createTriangleMesh returned null!\n");
				return nullptr;
			}

			pxShape = physics->createShape(PxTriangleMeshGeometry(triangleMesh), *pxMat, false);
			break;
		}
	}

	return pxShape;
}