/* 
*	Isadora Soares Guedes
*	Computação Gráfica
*	Módulo 5
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
#include "Shader.h"

//window configuration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void setupWindow(GLFWwindow*& window);
void resetAllRotate();
void setupTransformacoes(glm::mat4& model);

//geometry configuration
void readFromObj(string path);
void readFromMtl(string path);
int setupGeometry();
int loadTexture(string path);

//obj values
vector<GLfloat> totalvertices;
vector<GLfloat> vertices;
vector<GLfloat> textures;
vector<GLfloat> normais;

//file values
string mtlFilePath = "";
string textureFilePath = "";
string basePath = "../../Arquivos/";
string objFileName = "SuzanneTriTextured.obj";


//iluminacao values
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Transformation parameters
bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;
float scaleLevel = 200.0f;
GLfloat translateX = 400.0f;
GLfloat translateY = 300.0f;
GLfloat translateZ = 100.0f;

int main()
{
	GLFWwindow* window;

	setupWindow(window);

	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");
	readFromObj(basePath + objFileName);
	readFromMtl(basePath + "/mtl/" + mtlFilePath);
	GLuint textureID = loadTexture(basePath + "/textures/" + textureFilePath);
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	glm::mat4 projection = glm::mat4(1);
	projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1000.0, 1000.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.5);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glm::mat4 model = glm::mat4(1);
		setupTransformacoes(model);
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, (vertices.size() / 3));

		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
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
	GLuint VAO, VBO[3];

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normais.size() * sizeof(GLfloat), normais.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

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

					vertices.push_back(verticess.x);
					vertices.push_back(verticess.y);
					vertices.push_back(verticess.z);

					textures.push_back(texturess.x);
					textures.push_back(texturess.y);

					normais.push_back(normaiss.x);
					normais.push_back(normaiss.y);
					normais.push_back(normaiss.z);
				}
			}
			else if (prefix == "mtllib")
			{
				iss >> mtlFilePath;
			}
		}
	}

	file.close();


	std::cout << temp_textures.size() << std::endl;
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

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 5: Cameras - Isadora Guedes", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

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

void setupTransformacoes(glm::mat4& model) {
	float angle = (GLfloat)glfwGetTime() / 10 * 7;

	model = glm::mat4(1);

	model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));
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

	model = glm::scale(model, glm::vec3(scaleLevel, scaleLevel, scaleLevel));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	const float scaleStep = 10.f;
	const float translateStep = 10.01f;

	//Escala ---------------
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		// Aumenta scale
		scaleLevel += scaleStep;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		// Diminui scale
		scaleLevel -= scaleStep;
	}

	//Rotação----------------------
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if ((key == GLFW_KEY_X) && action == GLFW_PRESS)
	{
		resetAllRotate();
		rotateX = true;
	}

	if ((key == GLFW_KEY_Y) && action == GLFW_PRESS)
	{
		resetAllRotate();
		rotateY = true;
	}

	if ((key == GLFW_KEY_Z) && action == GLFW_PRESS)
	{
		resetAllRotate();
		rotateZ = true;
	}

	// Translação -----------------
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_A:
			translateX -= translateStep;
			break;
		case GLFW_KEY_D:
			translateX += translateStep;
			break;
		case GLFW_KEY_W:
			translateY += translateStep;
			break;
		case GLFW_KEY_S:
			translateY -= translateStep;
			break;
		case GLFW_KEY_I:
			translateZ += translateStep;
			break;
		case GLFW_KEY_J:
			translateZ -= translateStep;
			break;
		default:
			break;
		}
	}

	// Resetar visualização
	if ((key == GLFW_KEY_P) && action == GLFW_PRESS)
	{
		resetAllRotate();
		translateX = 400.0f;
		translateY = 300.0f;
		translateZ = 100.0f;
		scaleLevel = 200.5f;
	}
}

void resetAllRotate() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}