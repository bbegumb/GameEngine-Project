#pragma once

#include <string>
#include <vector>
#include <renderer/Vertex.h>

enum class MeshPrimitive { None, Box, Sphere, Plane };

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();

	void draw() const;

	const std::vector<Vertex>& getVertices() const { return vertices; }

	MeshPrimitive primitive = MeshPrimitive::None;

	std::string getName() const { return name; }
	void setName(const std::string& name) { this->name = name; }

private:
	void setupMesh();

	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
};