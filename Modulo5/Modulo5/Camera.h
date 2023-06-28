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
	void initialize(
		Shader* shader,
		int width, int height,
		glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0),
        glm::vec3 cameraFront = glm::vec3(0.0, 0.0, 0.0),
        glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0),
		bool firstMouse = true,
		float lastX = 0, 
		float lastY = 0,
		float sensitivity = 0.05f,
		float pitch = 0.0, 
		float yaw = -90.0
	);
	void update();
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void setCameraPos(int key, float cameraSpeed = 0.5f);

protected:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 front;

	bool firstMouse;
	float lastX;
	float lastY;
	float sensitivity;
	float pitch;
	float yaw;

	Shader* shader;
};
