#pragma once

#include <scene/components/Component.h>
#include <memory>

class Material;

class MaterialComponent : public Component {
public:
    MaterialComponent(std::shared_ptr<Material> material = nullptr) {
        if (material) materials.push_back(material);
    }

    MaterialComponent(std::vector<std::shared_ptr<Material>> mats) : materials(std::move(mats)) {}

    std::vector<std::shared_ptr<Material>> materials;

    Material* getMaterial(int index = 0) const {
        if (index < 0 || index >= static_cast<int>(materials.size()))
            return nullptr;
        return materials[index].get();
    }

	void serialize(nlohmann::json& j) const override;
	void deserialize(const nlohmann::json& j) override;
};
