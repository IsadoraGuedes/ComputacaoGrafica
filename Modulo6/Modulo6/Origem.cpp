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
void setupTransformacoes(glm::mat4& model);
void setupShader(Shader shader);

//geometry configuration
void readFromObj(string path);
void readFromMtl(string path);
int setupGeometry();
int loadTexture(string path);

// Parametric curves
vector <glm::vec3> generateControlPointsSet(int nPoints);
vector <glm::vec3> generateControlPointsSet();
std::vector<glm::vec3> generateUnisinosPointsSet();
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

	std::vector<glm::vec3> uniPoints = generateUnisinosPointsSet();
	GLuint VAOUni = generateControlPointsBuffer(uniPoints);

	Bezier bezier;
	bezier.setControlPoints(uniPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(10);

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glm::mat4 model = glm::mat4(1);
		setupTransformacoes(model);
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));

		camera.update();

		object.draw(textureID);

		glBindTexture(GL_TEXTURE_2D, 0);

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

void setupTransformacoes(glm::mat4& model)
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

	// Translação
	model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

	// Escala
	model = glm::scale(model, glm::vec3(scale, scale, scale));
}


std::vector<glm::vec3> generateControlPointsSet(int nPoints) {
	std::vector<glm::vec3> controlPoints;

	// Gerar n�meros aleat�rios
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distribution(-0.9f, 0.9f);  // Intervalo aberto (-0.9, 0.9)

	for (int i = 0; i < nPoints; i++) {
		glm::vec3 point;
		do {
			// Gerar coordenadas x e y aleat�rias
			point.x = distribution(gen);
			point.y = distribution(gen);
		} while (std::find(controlPoints.begin(), controlPoints.end(), point) != controlPoints.end());

		// Definir coordenada z como 0.0
		point.z = 0.0f;

		controlPoints.push_back(point);
	}

	return controlPoints;
}

vector<glm::vec3> generateControlPointsSet()
{
	vector <glm::vec3> controlPoints;

	controlPoints.push_back(glm::vec3(-0.6, -0.4, 0.0));
	controlPoints.push_back(glm::vec3(-0.4, -0.6, 0.0));
	controlPoints.push_back(glm::vec3(-0.2, -0.2, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));
	controlPoints.push_back(glm::vec3(0.2, 0.2, 0.0));
	controlPoints.push_back(glm::vec3(0.4, 0.6, 0.0));
	controlPoints.push_back(glm::vec3(0.6, 0.4, 0.0));

	return controlPoints;
}

GLuint generateControlPointsBuffer(vector <glm::vec3> controlPoints)
{
	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(GLfloat) * 3, controlPoints.data(), GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

std::vector<glm::vec3> generateUnisinosPointsSet()
{
	float vertices[] = {
		-0.262530, 0.376992, 0.000000,
-0.262530, 0.377406, 0.000000,
-0.262530, 0.334639, 0.000000,
-0.262530, 0.223162, 0.000000,
-0.262530, 0.091495, 0.000000,
-0.262371, -0.006710, 0.000000,
-0.261258, -0.071544, -0.000000,
-0.258238, -0.115777, -0.000000,
-0.252355, -0.149133, -0.000000,
-0.242529, -0.179247, -0.000000,
-0.227170, -0.208406, -0.000000,
-0.205134, -0.237216, -0.000000,
-0.177564, -0.264881, -0.000000,
-0.146433, -0.289891, -0.000000,
-0.114730, -0.309272, -0.000000,
-0.084934, -0.320990, -0.000000,
-0.056475, -0.328224, -0.000000,
-0.028237, -0.334170, -0.000000,
0.000000, -0.336873, -0.000000,
0.028237, -0.334170, -0.000000,
0.056475, -0.328224, -0.000000,
0.084934, -0.320990, -0.000000,
0.114730, -0.309272, -0.000000,
0.146433, -0.289891, -0.000000,
0.177564, -0.264881, -0.000000,
0.205134, -0.237216, -0.000000,
0.227170, -0.208406, -0.000000,
0.242529, -0.179247, -0.000000,
0.252355, -0.149133, -0.000000,
0.258238, -0.115777, -0.000000,
0.261258, -0.071544, -0.000000,
0.262371, -0.009704, 0.000000,
0.262530, 0.067542, 0.000000,
0.262769, 0.153238, 0.000000,
0.264438, 0.230348, 0.000000,
0.268678, 0.284286, 0.000000,
0.275462, 0.320338, 0.000000,
0.284631, 0.347804, 0.000000,
0.296661, 0.372170, 0.000000,
0.311832, 0.396628, 0.000000,
0.328990, 0.419020, 0.000000,
0.347274, 0.436734, 0.000000,
0.368420, 0.450713, 0.000000,
0.393395, 0.462743, 0.000000,
0.417496, 0.474456, 0.000000,
0.436138, 0.487056, 0.000000,
0.450885, 0.500213, 0.000000,
0.464572, 0.513277, 0.000000,
0.478974, 0.525864, 0.000000,
0.494860, 0.538133, 0.000000,
0.510031, 0.552151, 0.000000,
0.522127, 0.570143, 0.000000,
0.531124, 0.593065, 0.000000,
0.537629, 0.620809, 0.000000,
0.542465, 0.650303, 0.000000,
0.546798, 0.678259, 0.000000,
0.552959, 0.703513, 0.000000,
0.563121, 0.725745, 0.000000,
0.577656, 0.745911, 0.000000,
0.596563, 0.764858, 0.000000,
0.620160, 0.781738, 0.000000,
0.648302, 0.795385, 0.000000,
0.678670, 0.805057, 0.000000,
0.710336, 0.810741, 0.000000,
0.750111, 0.814914, 0.000000,
0.802994, 0.819945, 0.000000,
0.860771, 0.825435, 0.000000,
	};

	vector <glm::vec3> uniPoints;

	for (int i = 0; i < 67 * 3; i += 3)
	{
		glm::vec3 point;
		point.x = vertices[i];
		point.y = vertices[i + 1];
		point.z = 0.0;

		uniPoints.push_back(point);
	}

	return uniPoints;
}

