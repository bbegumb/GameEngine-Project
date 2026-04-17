#pragma once

#include <renderer/Mesh.h>
#include <memory>
#include <string>
#include <vector>

struct Vertex;

class ObjLoader {
public:
	static std::shared_ptr<Mesh> load(const std::string& filePath, bool forceLoadNew = false);
private:
	static void saveCache(const std::string& objName, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	static bool loadCache(const std::string& objName, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};