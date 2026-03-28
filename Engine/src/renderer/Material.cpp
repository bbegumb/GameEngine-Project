#include "Material.h"

#include <renderer/ShaderProgram.h>

void Material::apply() const {
	if (!shader) return;

	shader->use();
	shader->setVec3("uMaterial.albedo", albedo);
	shader->setFloat("uMaterial.ambientReflectance", ambientReflectance);
	shader->setFloat("uMaterial.specularReflectance", specularReflectance);
	shader->setFloat("uMaterial.shininess", shininess);
}