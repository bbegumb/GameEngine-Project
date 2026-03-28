#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {
public:
	ShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~ShaderProgram();

	void setBool(const std::string& uniformName, bool value) const;
	void setFloat(const std::string& uniformName, float value) const;
	void setVec3(const std::string& uniformName, const glm::vec3& value) const;
	void setMat4(const std::string& uniformName, const glm::mat4& value) const;

	void use() const;
	
	unsigned int getID() const;
private:
	unsigned int programID = 0;

	std::string readFile(const std::string& path);
	unsigned int compileShader(unsigned int type, const std::string& source);
};