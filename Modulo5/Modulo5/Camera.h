#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


class Camera
{
public:
	Camera() {}
	~Camera() {}
	void initialize(Shader* shader, 
        glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0),
        glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0),
        glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0));
	void update(glm::vec3 front);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)

protected:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 front;

	//Refer�ncia (endere�o) do shader
	Shader* shader;
};
