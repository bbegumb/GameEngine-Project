#pragma once

#include <scene/components/Component.h>
#include <memory>

class Material;

class MaterialComponent : public Component {
public:
	MaterialComponent(std::shared_ptr<Material> material = nullptr) : material(material) {}

	std::shared_ptr<Material> material;

	void setMaterial(std::shared_ptr<Material> material) { this->material = material; }
};