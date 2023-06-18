/* Atividade Vicencial 1
 *
 * Por Isadora Soares Guedes
 * Código adapatado de repositório da cadeira
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image
#include "stb_image.h"

// Nossa classe que armazena as infos dos shaders
#include "Shader.h"

// Function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void resetAllRotate();
void rotateAll(glm::mat4& model);
void setupTransformacoes(glm::mat4& model);
int loadTexture(string path);
GLuint setupGeometry(string);
void setupWindow(GLFWwindow*& window);
void readFromObj(std::string path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_textures, std::vector<glm::vec3>& out_normais);

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Rotation parameters
bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;

// Scale parameter
float scaleLevel = 0.5f;

// Number of vertices
int verticesSize = 0;
std::vector<GLfloat> vertices;

// Translation parameters
GLfloat translateX = 0.0f;
GLfloat translateY = 0.0f;
GLfloat translateZ = 0.0f;

int main()
{
	GLFWwindow* window;

	setupWindow(window);

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

	//Carregando uma textura e armazenando o identificador na memória
	GLuint texID = loadTexture("../../Arquivos/textures/Cube.png");


	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry("../../Arquivos/CuboTextured.obj");

	glUseProgram(shader.ID);

	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");

	model = glm::ortho(0.0, 800.0, 0.0, 600.0, -1000.0, 1000.0);
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		setupTransformacoes(model);

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		//Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		glDrawArrays(GL_TRIANGLES, 0, vertices.size()/8);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

void setupTransformacoes(glm::mat4& model) {
	rotateAll(model);

	model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

	model = glm::scale(model, glm::vec3(scaleLevel, scaleLevel, scaleLevel));
}

void setupWindow(GLFWwindow*& window) {
	glfwInit();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 3", nullptr, nullptr);
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

void rotateAll(glm::mat4& model) {
	float angle = (GLfloat)glfwGetTime();

	model = glm::mat4(1);

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
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	const float scaleStep = 0.1f;
	const float translateStep = 0.01f;


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
		//zoomLevel = glm::max(zoomLevel, 0.5f);
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
		translateX = 0.0f;
		translateY = 0.0f;
		translateZ = 0.0f;
		scaleLevel = 0.5f;
	}
}

void resetAllRotate() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}


GLuint setupGeometry(string filePath)
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	std::vector<glm::vec3> vetVertices;
	std::vector<glm::vec3> vetNormal;
	std::vector<glm::vec2> vetTexturas;
	

	readFromObj(filePath, vetVertices, vetTexturas, vetNormal);


	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glEnable(GL_DEPTH_TEST);

	return VAO;
}

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void readFromObj(string path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_textures,
	std::vector<glm::vec3>& out_normais) {

	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the file." << std::endl;
	}

	std::vector<unsigned int> verticesIndices, texturesIndices, normalIndices;
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
				unsigned int vertexIndex[3], textIndex[3], normalIndex[3];
				char slash;

				for (int i = 0; i < 3; ++i)
				{
					iss >> vertexIndex[i] >> slash >> textIndex[i] >> slash >> normalIndex[i];

					out_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
					out_textures.push_back(temp_textures[textIndex[i] - 1]);
					out_normais.push_back(temp_normais[normalIndex[i] - 1]);
				}
			}
		}
	}

	file.close();


	std::cout << temp_vertices.size() << std::endl;

	//mapeamento dos triângulos
	for (unsigned int i = 0; i < out_vertices.size(); i++)
	{
		vertices.push_back(out_vertices[i].x);
		vertices.push_back(out_vertices[i].y);
		vertices.push_back(out_vertices[i].z);

		vertices.push_back(out_normais[i].r);
		vertices.push_back(out_normais[i].g);
		vertices.push_back(out_normais[i].b);

		vertices.push_back(out_textures[i].x);
		vertices.push_back(out_textures[i].y);
	}
}
