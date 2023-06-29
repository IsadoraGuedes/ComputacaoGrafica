#include "Mesh.h"

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, glm::vec3 position, float angle, glm::vec3 axis)
{
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->position = position;
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

void Mesh::update(glm::vec3 pointOnCurve, bool rotateX, bool rotateY, bool rotateZ, float scale) {
	glm::mat4 model = glm::mat4(1);
	setupTransformacoes(model, pointOnCurve, rotateX, rotateY, rotateZ, scale);
	GLint modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
}


void Mesh::setupTransformacoes(glm::mat4& model, glm::vec3 pointOnCurve, bool rotateX, bool rotateY, bool rotateZ, float scale)
{
	float angle = (GLfloat)glfwGetTime();

	if (rotateX)
	{
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (rotateY)
	{
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (rotateZ)
	{
		model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	model = glm::translate(model, pointOnCurve);
	model = glm::scale(model, glm::vec3(scale, scale, scale));
}

void Mesh::setTexId(GLuint texId) {
	this->texId = texId;
}

string Mesh::getTexturePath() {
	return textFilePath;
}

string Mesh::getObjPath() {
	return objFilePath;
}

GLuint Mesh::getTexId() {
	return texId;
}

string Mesh::getMtlPath() {
	return mtlFilePath;
}

void Mesh::setPaths(string textFilePath, string objFilePath, string mtlFilePath) {
	this->textFilePath = textFilePath;
	this->objFilePath = objFilePath;
	this->mtlFilePath = mtlFilePath;
}

void Mesh::setObjFilePath(string path) {
	this->objFilePath = path;
}

void Mesh::setMtlFilePath(string path) {
	this->mtlFilePath = path;
}

void Mesh::setTextureFilePath(string path) {
	this->textFilePath = path;
}

void Mesh::setDataVertices(vector<GLfloat> vertices) {
	this->dataVertices = vertices;
}

vector<GLfloat> Mesh::getDataVertices() {
	return dataVertices;
}