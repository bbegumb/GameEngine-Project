#include "Material.h"

#include <renderer/ShaderProgram.h>
#include <renderer/Texture.h>

void Material::apply() const {
	if (!shader) return;

	shader->use();
	shader->setVec3("uMaterial.albedo", albedo);
	shader->setFloat("uMaterial.ambientReflectance", ambientReflectance);
	shader->setFloat("uMaterial.specularReflectance", specularReflectance);
	shader->setFloat("uMaterial.shininess", shininess);

	if (diffuseTexture) {
		diffuseTexture->bind(0);
		shader->setBool("uHasTexture", true);
		shader->setInt("uDiffuseTexture", 0);
	}
	else {
		shader->setBool("uHasTexture", false);
	}
}