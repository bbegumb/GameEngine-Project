#pragma once

#include <glm/glm.hpp>
#include <renderer/ShaderProgram.h>

class LineRenderer {
public:
    LineRenderer();
    ~LineRenderer();

    void drawLine(
        const glm::vec3& start,
        const glm::vec3& end,
        const glm::vec3& color,
        const glm::mat4& view,
        const glm::mat4& projection
    );

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    ShaderProgram shader;
};
