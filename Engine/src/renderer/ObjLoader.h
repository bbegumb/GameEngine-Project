#pragma once

#include <renderer/Mesh.h>
#include <memory>
#include <string>

class ObjLoader {
public:
	static std::shared_ptr<Mesh> load(const std::string& filePath);
};