#include "ObjLoader.h"

#include <codeanalysis/warnings.h>
#pragma warning(push)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_object_loader.h>

#pragma warning(pop)

#include <renderer/Vertex.h>
#include <iostream>
#include <unordered_map>

struct VertexKey {
    int pos, norm, tex;
    bool operator==(const VertexKey& other) const {
        return pos == other.pos && norm == other.norm && tex == other.tex;
    }
};

struct VertexKeyHash {
    size_t operator()(const VertexKey& k) const {
        size_t h = std::hash<int>()(k.pos);
        h ^= std::hash<int>()(k.norm) << 1;
        h ^= std::hash<int>()(k.tex) << 2;
        return h;
    }
};

std::shared_ptr<Mesh> ObjLoader::load(const std::string& filepath) {


    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (std::string(MODEL_PATH) + filepath).c_str())) {
        std::cerr << "OBJ load failed: " << err << std::endl;
        return nullptr;
    }

    if (!warn.empty()) std::cerr << "OBJ warning: " << warn << std::endl;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<VertexKey, unsigned int, VertexKeyHash> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            VertexKey key{ index.vertex_index, index.normal_index, index.texcoord_index };

            if (uniqueVertices.count(key)) {
                indices.push_back(uniqueVertices[key]);
                continue;
            }

            Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            } else {
                vertex.normal = { 0.0f, 1.0f, 0.0f };
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            } else {
                vertex.texCoord = { 0.0f, 0.0f };
            }

            unsigned int newIndex = static_cast<unsigned int>(vertices.size());
            uniqueVertices[key] = newIndex;
            indices.push_back(newIndex);
            vertices.push_back(vertex);
        }
    }

    glm::vec3 minBounds(FLT_MAX);
    glm::vec3 maxBounds(-FLT_MAX);

    for (const auto& v : vertices) {
        minBounds = glm::min(minBounds, v.position);
        maxBounds = glm::max(maxBounds, v.position);
    }

    glm::vec3 center = (minBounds + maxBounds) * 0.5f;

    for (auto& v : vertices) {
        v.position -= center;
    }

    return std::make_shared<Mesh>(vertices, indices);
}