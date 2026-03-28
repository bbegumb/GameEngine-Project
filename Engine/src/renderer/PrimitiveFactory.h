#pragma once

#include <memory>
#include <renderer/Mesh.h>

namespace PrimitiveFactory {
	std::shared_ptr<Mesh> createCube();
	std::shared_ptr<Mesh> createPlane();
}