#include "Mesh.h"

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis)
{
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->position = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;
}

void Mesh::draw(GLuint texId)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
}
