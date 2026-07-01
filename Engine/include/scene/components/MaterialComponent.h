#pragma once

#include <scene/components/Component.h>

#include <memory>
#include <vector>

class Material;
class Archive;

class MaterialComponent : public Component {
public:
    MaterialComponent(std::shared_ptr<Material> material = nullptr);

    MaterialComponent(std::vector<std::shared_ptr<Material>> mats) : materials(std::move(mats)) {}

    size_t getMaterialCount() const { return materials.size(); }

    std::vector<std::shared_ptr<Material>> materials;

    Material* getMaterial(int index = 0) const;

	void serialize(Archive& arch) const override;
	void deserialize(const Archive& arch) override;

    void addDefaultMaterial();

};
