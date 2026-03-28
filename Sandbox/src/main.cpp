#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <renderer/renderer_all.h>
#include <scene/scene_all.h>
#include <DemoScene.h>
#include <scripts/Rotator.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1000, 800, "Dev Window", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create window.\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

    glViewport(0, 0, 1000, 800);

	Scene scene;
	Renderer renderer;

	DemoScene::create(scene);

	float last = 0.0f;
	float dt = 0.0f;

	float x = 0.0f, y = 0.0f, z = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float now = (float)glfwGetTime();
		if (last == 0.0f) {
			last = now;
			continue;
		}

		float dt = now - last;
		
		scene.onUpdate(dt);
		renderer.render(scene);

        glfwSwapBuffers(window);
        glfwPollEvents();

		last = now;
    }

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}