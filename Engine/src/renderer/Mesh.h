#pragma once

#include <vector>
#include <renderer/Vertex.h>

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();

	void draw() const;
private:
	void setupMesh();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
};