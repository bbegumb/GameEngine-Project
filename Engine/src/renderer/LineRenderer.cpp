#include "LineRenderer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

LineRenderer::LineRenderer(): shader("basic") {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(float) * 6,nullptr,GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

LineRenderer::~LineRenderer(){
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void LineRenderer::drawLine(const glm::vec3& start,const glm::vec3& end,const glm::vec3& color,const glm::mat4& view,const glm::mat4& projection) {
    
    float vertices[] = {
        start.x, start.y, start.z,
        end.x,   end.y,   end.z
    };

    shader.use();

    shader.setMat4("uModel", glm::mat4(1.0f));
    shader.setMat4("uView", view);
    shader.setMat4("uProj", projection);
    shader.setVec3("uObjectColor", color);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices),vertices);

    glLineWidth(4.0f);
    
    glDisable(GL_DEPTH_TEST);
    glLineWidth(5.0f);
    
    glDrawArrays(GL_LINES, 0, 2);

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);

    glBindVertexArray(0);
}
