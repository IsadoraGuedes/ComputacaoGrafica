#include "Mesh.h"

void Mesh::initialize(GLuint VAO, Shader* shader, float angle, glm::vec3 axis,
	bool rotateX, bool rotateY, bool rotateZ, float scale, bool isStatic)
{
	this->VAO = VAO;
	this->shader = shader;
	this->angle = angle;
	this->axis = axis;

	this->rotateX = rotateX;
	this->rotateY = rotateY;
	this->rotateZ = rotateZ;
	this->scaleLevel = scale;
	this->isStatic = isStatic;

}

void Mesh::draw(GLuint texId)
{
	int nVert = dataVertices.size() / 8;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVert);
	glBindVertexArray(0);
}

void Mesh::update(glm::vec3 pointOnCurve) {
	glm::mat4 model = glm::mat4(1);
	setupTransformacoes(model, pointOnCurve, rotateX, rotateY, rotateZ, scaleLevel);
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

void Mesh::setObjFilePath(string path) {
	this->objFilePath = path;
}

void Mesh::setMtlFilePath(string path) {
	this->mtlFilePath = path;
}

void Mesh::setTextureFilePath(string path) {
	this->textFilePath = path;
}

void Mesh::setPosition(glm::vec3 position) {
	this->position = position;
}

void Mesh::setDataVertices(vector<GLfloat> vertices) {
	this->dataVertices = vertices;
}

vector<GLfloat> Mesh::getDataVertices() {
	return dataVertices;
}

void Mesh::setRotateX() {
	rotateX = true;
	rotateY = false;
	rotateZ = false;
}

void Mesh::setRotateY() {
	rotateX = false;
	rotateY = true;
	rotateZ = false;
}

void Mesh::setRotateZ() {
	rotateX = false;
	rotateY = false;
	rotateZ = true;
}

void Mesh::resetRotate() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}

void Mesh::incrementScale(float scaleFactor) {
	scaleLevel += scaleFactor;
}

void Mesh::decrementScale(float scaleFactor) {
	scaleLevel -= scaleFactor;
}

void Mesh::setIsStatic(bool isStatic) {
	isStatic = isStatic;
}

bool Mesh::getIsStatic() {
	return isStatic;
}

glm::vec3 Mesh::getPosition() {
	return position;
}