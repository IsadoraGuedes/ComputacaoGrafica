#pragma once

#include "Camera.h"

void Camera::initialize(Shader* shader, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp)
{
	this->shader = shader;
	this->cameraPos = cameraPos;
	this->cameraFront = cameraFront;
	this->cameraUp = cameraUp;

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	shader->setMat4("view", glm::value_ptr(view));

	//Atualizando o shader com a posi��o da c�mera
	shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}

void Camera::update(glm::vec3 front)
{
    this->front = front;
    cameraFront = glm::normalize(front);
	/*glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));*/
}

void Camera::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    this->front = front;
    cameraFront = glm::normalize(front);
	/*glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));*/
}

