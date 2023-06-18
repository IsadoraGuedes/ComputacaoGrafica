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

// Function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void resetAllRotate();
void rotateAll(glm::mat4& model);
void setupTransformacoes(glm::mat4& model);
int setupShader();
GLuint setupGeometry(string);
void stupWindow(GLFWwindow*& window);
void readFromObj(std::string path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_textures, std::vector<glm::vec3>& out_normais);

// Shader sources
const GLchar* vertexShaderSource = R"(
    #version 400
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 color;
    uniform mat4 model;
    out vec4 finalColor;
    void main()
    {
        // ... additional lines of code here
        gl_Position = model * vec4(position, 1.0);
        finalColor = vec4(color, 1.0);
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 450
    in vec4 finalColor;
    out vec4 color;
    void main()
    {
        color = finalColor;
    }
)";

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

	stupWindow(window);

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry("../../Arquivos/suzanneTri.obj");

	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");

	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesSize / 2);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		glDrawArrays(GL_POINTS, 0, verticesSize / 6);
		glBindVertexArray(0);

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

void stupWindow(GLFWwindow*& window) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Atividade Vicencial 1", nullptr, nullptr);
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

	// Scale
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_T)
		{
			scaleLevel += scaleStep;
		}
		else if (key == GLFW_KEY_R) 
		{
			scaleLevel -= scaleStep;
		}
	}

	// Rotation
	if (action == GLFW_PRESS)
	{
		resetAllRotate();

		if (key == GLFW_KEY_X)
		{
			rotateX = true;
		}
		else if (key == GLFW_KEY_Y)
		{
			rotateY = true;
		}
		else if (key == GLFW_KEY_Z)
		{
			rotateZ = true;
		}
	}

	// Translation
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

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void resetAllRotate() {
	rotateX = false;
	rotateY = false;
	rotateZ = false;
}

//Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
// shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no iniçio deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
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


	verticesSize = vertices.size();

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
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
				iss >> values.s >> values.t;
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
					//1 5 6
					iss >> vertexIndex[i] >> slash >> textIndex[i] >> slash >> normalIndex[i];

					out_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
					out_textures.push_back(temp_textures[textIndex[i] - 1]);
					out_normais.push_back(temp_normais[normalIndex[i] - 1]);
				}
			}
		}
	}

	std::cout << temp_vertices.size() << std::endl;

	file.close();
	

	//inicialização cores
	std::array<GLfloat, 3> cor1 = { 1.000f, 0.647f, 0.694f };  // 255 165 177
	std::array<GLfloat, 3> cor2 = { 0.741f, 0.129f, 0.415f };  // 189 33 106
	std::array<GLfloat, 3> cor3 = { 0.949f, 0.780f, 0.790f };  // 242 199 203
	std::array<GLfloat, 3> cor4 = { 0.972f, 0.407f, 0.584f };  // 248 104 149
	std::array<GLfloat, 3> cor5 = { 1.000f, 0.058f, 0.502f };  // 255 15 128

	std::vector<std::array<GLfloat, 3>> cores;
	cores.push_back(cor1);
	cores.push_back(cor2);
	cores.push_back(cor3);
	cores.push_back(cor4);
	cores.push_back(cor5);

	int cor = 0;

	//mapeamento dos triângulos
	for (unsigned int i = 0; i < out_vertices.size(); i++)
	{
		vertices.push_back(out_vertices[i].x);
		vertices.push_back(out_vertices[i].y);
		vertices.push_back(out_vertices[i].z);

		vertices.push_back(cores[cor][0]);
		vertices.push_back(cores[cor][1]);
		vertices.push_back(cores[cor][2]);

		cor++;

		if (cor == 5) {
			cor = 0;
		}
	}
}
