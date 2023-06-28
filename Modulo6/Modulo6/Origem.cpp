/* 
*	Isadora Soares Guedes
*	Computação Gráfica
*	Módulo 4
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include <random>

#include "Shader.h"

#include "Mesh.h"
#include "Camera.h"
#include "Bezier.h"

//window configuration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void setupWindow(GLFWwindow*& window);
void setupTransformacoes(glm::mat4& model, glm::vec3 pointOnCurve);
void setupShader(Shader shader);

//geometry configuration
void readFromObj(string path);
void readFromMtl(string path);
int setupGeometry();
int loadTexture(string path);

// Parametric curves
vector <glm::vec3> generateControlPointsSet(int nPoints);
vector <glm::vec3> generateControlPointsSet();
std::vector<glm::vec3> generateTranslatePoints();
GLuint generateControlPointsBuffer(vector <glm::vec3> controlPoints);

//obj values
vector<GLfloat> totalvertices;
vector<GLfloat> vertices;
vector<GLfloat> textures;
vector<GLfloat> normais;

//file values
string mtlFilePath = "";
string textureFilePath = "";
string basePath = "../../Arquivos/";
string objFileName = "CuboTextured.obj";

//iluminacao values
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05f;
float pitch = 0.0, yaw = -90.0;

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Transformation parameters
bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;

GLfloat translateX = 0.0f;
GLfloat translateY = 0.0f;
GLfloat translateZ = 0.0f;
float scale = 1.0f;

Camera camera;

int main()
{
	GLFWwindow* window;

	setupWindow(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");
	readFromObj(basePath + objFileName);
	readFromMtl(basePath + "/mtl/" + mtlFilePath);
	GLuint textureID = loadTexture(basePath + "/textures/" + textureFilePath);
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	Mesh object;
	object.initialize(VAO, (totalvertices.size() / 8), &shader);

	setupShader(shader);

	glEnable(GL_DEPTH_TEST);

	camera.initialize(&shader, width, height);

	std::vector<glm::vec3> basePoints = generateTranslatePoints();

	Bezier bezier;
	bezier.setControlPoints(basePoints);
	bezier.setShader(&shader);
	bezier.generateCurve(30);

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;
	int speed = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		camera.update();

		glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);

		glm::mat4 model = glm::mat4(1);
		setupTransformacoes(model, pointOnCurve);
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

		object.draw(textureID);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (speed++ == 50) {
			i = (i + 1) % nbCurvePoints;
			speed = 0;
		}

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void setupShader(Shader shader) {
	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.7f);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
}

void readFromMtl(string path)
{
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." << std::endl;
	}

	std::string line;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "map_Kd")
			{
				iss >> textureFilePath;
			}
			else if (prefix == "Ka")
			{
				glm::vec3 temp_ka;
				iss >> temp_ka.x >> temp_ka.y >> temp_ka.z;

				ka.push_back(temp_ka.x);
				ka.push_back(temp_ka.y);
				ka.push_back(temp_ka.z);
			}
			else if (prefix == "Ks")
			{
				glm::vec3 temp_ks;
				iss >> temp_ks.x >> temp_ks.y >> temp_ks.z;

				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
			}
			else if (prefix == "Ns")
			{
				iss >> ns;
			}
		}
	}

	file.close();
}

int setupGeometry()
{
	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, totalvertices.size() * sizeof(GLfloat), totalvertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	// coordenadas posição - x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// coordenadas de textura - s, t (ou u, v)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// coordenadas normal - x, y, z
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

void readFromObj(string path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." << std::endl;
	}

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_textures;
	std::vector<glm::vec3> temp_normais;

	std::string line;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);

			std::string prefix;
			iss >> prefix;

			if (prefix == "v") {

				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_vertices.push_back(values);
			}
			else if (prefix == "vt")
			{
				glm::vec2 values;
				iss >> values.x >> values.y;
				temp_textures.push_back(values);
			}
			else if (prefix == "vn")
			{
				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_normais.push_back(values);
			}
			else if (prefix == "f")
			{
				unsigned int vertexIndex, textIndex, normalIndex;
				char slash;

				for (int i = 0; i < 3; ++i)
				{
					iss >> vertexIndex >> slash >> textIndex >> slash >> normalIndex;

					glm::vec3 verticess = temp_vertices[vertexIndex - 1];
					glm::vec3 normaiss = temp_normais[normalIndex - 1];
					glm::vec2 texturess = temp_textures[textIndex - 1];

					totalvertices.push_back(verticess.x);
					totalvertices.push_back(verticess.y);
					totalvertices.push_back(verticess.z);

					totalvertices.push_back(texturess.x);
					totalvertices.push_back(texturess.y);

					totalvertices.push_back(normaiss.x);
					totalvertices.push_back(normaiss.y);
					totalvertices.push_back(normaiss.z);
				}
			}
			else if (prefix == "mtllib")
			{
				iss >> mtlFilePath;
			}
		}
	}

	file.close();
}

int loadTexture(string path)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void setupWindow(GLFWwindow*& window) {
	glfwInit();

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 4: Iluminacao - Isadora Guedes", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	//Desabilita o desenho do cursor 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.mouseCallback(window, xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;

	}
	camera.setCameraPos(key);
}

void setupTransformacoes(glm::mat4& model, glm::vec3 pointOnCurve)
{
	float angle = (GLfloat)glfwGetTime();

	/*if (rotateX)
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

	// Translação
	model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

	// Escala
	model = glm::scale(model, glm::vec3(scale, scale, scale));*/
	model = glm::translate(model, pointOnCurve);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
}

std::vector<glm::vec3> generateTranslatePoints()
{
	float vertices[] = {
-1, 0,0,
-0.866, -0.5,0,
-0.5, -0.866,0,
0, -1,0,
0.5, -0.866,0,
0.866, -0.5,0,
1, 0,0,
0.866, 0.5,0,
0.5, 0.866,0,
0, 1,0,
-0.5, 0.866,0,
-0.866, 0.5,0,
-0.951, -0.259,0,
-0.707, -0.707,0,
-0.259, -0.951,0,
0.259, -0.951,0,
0.707, -0.707,0,
0.951, -0.259,0,
0.951, 0.259,0,
0.707, 0.707,0,
0.259, 0.951,0,
-0.259, 0.951,0,
-0.707, 0.707,0,
-0.951, 0.259,0
	};

	vector <glm::vec3> uniPoints;

	for (int i = 0; i < 24 * 3; i += 3)
	{
		glm::vec3 point;
		point.x = vertices[i];
		point.y = vertices[i + 1];
		point.z = vertices[i + 2];

		uniPoints.push_back(point);
	}

	return uniPoints;
}
/*(-1, 0)
(-0.866, -0.5)
(-0.5, -0.866)
(0, -1)
(0.5, -0.866)
(0.866, -0.5)
(1, 0)
(0.866, 0.5)
(0.5, 0.866)
(0, 1)
(-0.5, 0.866)
(-0.866, 0.5)
(-0.951, -0.259)
(-0.707, -0.707)
(-0.259, -0.951)
(0.259, -0.951)
(0.707, -0.707)
(0.951, -0.259)
(0.951, 0.259)
(0.707, 0.707)
(0.259, 0.951)
(-0.259, 0.951)
(-0.707, 0.707)
(-0.951, 0.259)*/

/*
-1, 0,0,

0, -1,0,

1, 0,0,

0, 1,0,


-0.707, -0.707,0,

0.707, -0.707,0,

0.707, 0.707,0,

-0.707, 0.707,0,
*/

/*
(-1, 0)
(-0.866, -0.5)
(-0.5, -0.866)
(0, -1)
(0.5, -0.866)
(0.866, -0.5)
(1, 0)
(0.866, 0.5)
(0.5, 0.866)
(0, 1)
(-0.5, 0.866)
(-0.866, 0.5)
(-0.951, -0.259)
(-0.707, -0.707)
(-0.259, -0.951)
(0.259, -0.951)
(0.707, -0.707)
(0.951, -0.259)
(0.951, 0.259)
(0.707, 0.707)
(0.259, 0.951)
(-0.259, 0.951)
(-0.707, 0.707)
(-0.951, 0.259)
*/


