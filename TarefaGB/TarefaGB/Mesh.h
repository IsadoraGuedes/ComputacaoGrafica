#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader* shader, 
		glm::vec3 position = glm::vec3(0.0, 0.0, 0.0),
		float angle = 0.0, 
		glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void draw(GLuint texId);
	void update(glm::vec3 pointOnCurve, bool rotateX, bool rotateY, bool rotateZ, float scale);
	void setupTransformacoes(glm::mat4& model, glm::vec3 pointOnCurve, bool rotateX, bool rotateY, bool rotateZ, float scale);
	void setTexId(GLuint texId);
	GLuint getTexId();
	void setPaths(string textFilePath, string objFilePath, string mtlFilePath);
	string getTexturePath();
	string getObjPath();
	string getMtlPath();
	void setDataVertices(vector<GLfloat> vertices);
	vector<GLfloat> getDataVertices();
	void setObjFilePath(string path);
	void setMtlFilePath(string path);
	void setTextureFilePath(string path);

protected:
	GLuint VAO; //Identificador do Vertex Array Object - Vértices e seus atributos
	int nVertices;

	//Informações sobre as transformações a serem aplicadas no objeto
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;

	//Referência (endereço) do shader
	Shader* shader;
	
	GLuint texId;
	string textFilePath;
	string objFilePath;
	string mtlFilePath;

	vector<GLfloat> dataVertices;
};

