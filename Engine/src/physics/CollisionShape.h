#pragma once 

#include <glm/glm.hpp>
#include <vector>

#include <renderer/Vertex.h>

class PhysicsWorld;
class PhysicsMaterial;

enum class CollisionShapeType { BoxMesh, SphereMesh, ConvexMesh, TriangleMesh };

namespace physx {
	class PxShape;
}

class CollisionShape {
public:
	~CollisionShape();

	CollisionShapeType type;

	glm::vec3 halfExtents = glm::vec3(0.5f);

	float radius = 0.5f;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	static CollisionShape boxMesh(const glm::vec3& halfExtents);
	static CollisionShape sphereMesh(float radius);
	static CollisionShape convexMesh(const std::vector<Vertex>& meshVertices);
	static CollisionShape triangleMesh(const std::vector<Vertex>& meshVertices,
									   const std::vector<unsigned int>& meshIndices);

	physx::PxShape* getOrCreatePxShape(PhysicsWorld* world, PhysicsMaterial* mat, const glm::vec3& scale = glm::vec3(1.0f));

private:
	physx::PxShape* pxShape = nullptr;
};