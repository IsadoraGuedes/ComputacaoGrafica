/* mÓDULO 2 - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void resetAllRotate();

glm::mat4 rotateAll(glm::mat4);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 400\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX = false, rotateY = false, rotateZ = false;
bool reverseRotateX = false, reverseRotateY = false, reverseRotateZ = false;

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Scale parameters
float scaleLevel = 0.5f;

// Translation values
GLfloat translateX = 0.0f;
GLfloat translateY = 0.0f;
GLfloat translateZ = 0.0f;

int main()
{
	// Inicializa��o da GLFF
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criacao da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 2", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();


	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	//
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);


	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);
		
		model = rotateAll(model);

		model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

		model = glm::scale(model, glm::vec3(scaleLevel, scaleLevel, scaleLevel));

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 72);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		glDrawArrays(GL_POINTS, 0, 72);
		glBindVertexArray(0);

		model = glm::translate(model, glm::vec3(1.5, 0, 0));

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 72);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		glDrawArrays(GL_POINTS, 0, 72);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execucao da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

glm::mat4 rotateAll(glm::mat4 model) {
	float angle = (GLfloat)glfwGetTime();

	model = glm::mat4(1);

	if (rotateX)
	{
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));

	}
	if (reverseRotateX)
	{
		model = glm::rotate(model, -angle, glm::vec3(1.0f, 0.0f, 0.0f));

	}
	else if (rotateY)
	{
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	}
	else if (reverseRotateY)
	{
		model = glm::rotate(model, -angle, glm::vec3(0.0f, 1.0f, 0.0f));

	}
	else if (rotateZ)
	{
		model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

	}
	else if (reverseRotateZ)
	{
		model = glm::rotate(model, -angle, glm::vec3(0.0f, 0.0f, 1.0f));

	}

	return model;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	//SCALE ---------------
	
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		// Aumenta scale
		scaleLevel += 0.1f;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		// Diminui scale
		scaleLevel -= 0.1f;
		//zoomLevel = glm::max(zoomLevel, 0.5f);
	}

	//Rotation----------------------

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

	if ((key == GLFW_KEY_Z)&& action == GLFW_PRESS)
	{
		resetAllRotate();
		rotateZ = true;
	}
	/*
	//novos comandos
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		resetAllRotate();
		reverseRotateX = true;
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		resetAllRotate();
		reverseRotateY = true;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		resetAllRotate();
		reverseRotateZ = true;
	}*/

	// transladar -----------------
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_A:
			translateX -= 0.1f;
			break;
		case GLFW_KEY_D:
			translateX += 0.1f;
			break;
		case GLFW_KEY_W:
			translateY += 0.1f;
			break;
		case GLFW_KEY_S:
			translateY -= 0.1f;
			break;
		case GLFW_KEY_I:
			translateZ += 0.1f;
			break;
		case GLFW_KEY_J:
			translateZ -= 0.1f;
			break;
		default:
			break;
		}
	}
}

void resetAllRotate() {
	reverseRotateX = false;
	reverseRotateY = false;
	reverseRotateZ = false;
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
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//lado verde 6 front face
		-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //amarelo A
		 0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //preto C
		 0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //roxo E

		-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //amarelo A
		 0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //roxo E
		-0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //amarelo G
		
		//lado laranja 1 right face
		//x    y    z    r    g    b
		 0.5, -0.5, -0.5, 1.0, 0.5, 0.25, // C
		 0.5, -0.5,  0.5, 1.0, 0.5, 0.25, // D
		 0.5,  0.5,  0.5, 1.0, 0.5, 0.25, // F

		 0.5, -0.5, -0.5, 1.0, 0.5, 0.25, // C
		 0.5,  0.5,  0.5, 1.0, 0.5, 0.25, // F
		 0.5,  0.5, -0.5, 1.0, 0.5, 0.25, // E
		 
		 //lado preto e verde escuro 2 back face
		  0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto D
		 -0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto B
		 -0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto H
		  
		  0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto D
		 -0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto H
		  0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto F
		  
		  //lado roxo 3 left face
		 -0.5, -0.5,  0.5, 1.0, 0.0, 1.0, //roxo B
		 -0.5, -0.5, -0.5, 1.0, 0.0, 1.0, //roxo a
		 -0.5,  0.5, -0.5, 1.0, 0.0, 1.0, //roxo g

		 -0.5, -0.5,  0.5, 1.0, 0.0, 1.0, //roxo B
		 -0.5,  0.5, -0.5, 1.0, 0.0, 1.0, //roxo G
		 -0.5,  0.5,  0.5, 1.0, 0.0, 1.0, //roxo H
		  
		  //lado amarelo 4 top face
		 -0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo G
		  0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo E
		  0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo F
		 
		 -0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo G
		  0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo F
		 -0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo H
		 
		 //lado azul 5 bottom face
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0, //amarelo B
		 0.5, -0.5,  0.5, 0.0, 1.0, 1.0, //roxo d
		 0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //verde cinza C

		 -0.5, -0.5, 0.5, 0.0, 1.0, 1.0, //amarelo B
		  0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //roxo C
		 -0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //amarelo a

		 /*--------------------------------------------*/
		/*
		 -0.5, 1.0, -0.5, 0.0, 1.0, 0.0, //amarelo A
		 0.5, 1.0, -0.5, 0.0, 1.0, 0.0, //preto C
		 0.5, 2.0, -0.5, 0.0, 1.0, 0.0, //roxo E

		-0.5, 1.0, -0.5, 0.0, 1.0, 0.0, //amarelo A
		 0.5, 2.0, -0.5, 0.0, 1.0, 0.0, //roxo E
		-0.5, 2.0, -0.5, 0.0, 1.0, 0.0, //amarelo G

		//lado laranja 1 right face
		//x    y    z    r    g    b
		 0.5, 1.0, -0.5, 1.0, 0.5, 0.25, // C
		 0.5, 2.0,  0.5, 1.0, 0.5, 0.25, // D
		 0.5, 2.0,  0.5, 1.0, 0.5, 0.25, // F

		 0.5, 1.0, -0.5, 1.0, 0.5, 0.25, // C
		 0.5, 2.0,  0.5, 1.0, 0.5, 0.25, // F
		 0.5, 2.0, -0.5, 1.0, 0.5, 0.25, // E

		 //lado preto e verde escuro 2 back face
		  0.5, 1.0,  0.5, 0.0, 0.0, 0.0, //preto D
		 -0.5, 1.0,  0.5, 0.0, 0.0, 0.0, //preto B
		 -0.5, 2.0,  0.5, 0.0, 0.0, 0.0, //preto H

		  0.5, 1.0,  0.5, 0.0, 0.0, 0.0, //preto D
		 -0.5, 2.0,  0.5, 0.0, 0.0, 0.0, //preto H
		  0.5, 2.0,  0.5, 0.0, 0.0, 0.0, //preto F

		  //lado roxo 3 left face
		 -0.5, 1.0,  0.5, 1.0, 0.0, 1.0, //roxo B
		 -0.5, 1.0, -0.5, 1.0, 0.0, 1.0, //roxo a
		 -0.5, 2.0, -0.5, 1.0, 0.0, 1.0, //roxo g

		 -0.5, 1.0,  0.5, 1.0, 0.0, 1.0, //roxo B
		 -0.5, 2.0, -0.5, 1.0, 0.0, 1.0, //roxo G
		 -0.5, 2.0,  0.5, 1.0, 0.0, 1.0, //roxo H

		 //lado amarelo 4 top face
		-0.5,  2.0, -0.5, 1.0, 1.0, 0.0, //amarelo G
		 0.5,  2.0, -0.5, 1.0, 1.0, 0.0, //amarelo E
		 0.5,  2.0,  0.5, 1.0, 1.0, 0.0, //amarelo F

		-0.5,  2.0, -0.5, 1.0, 1.0, 0.0, //amarelo G
		 0.5,  2.0,  0.5, 1.0, 1.0, 0.0, //amarelo F
		-0.5,  2.0,  0.5, 1.0, 1.0, 0.0, //amarelo H

		//lado azul 5 bottom face
	   -0.5, 1.0,  0.5, 0.0, 1.0, 1.0, //amarelo B
		0.5, 1.0,  0.5, 0.0, 1.0, 1.0, //roxo d
		0.5, 1.0, -0.5, 0.0, 1.0, 1.0, //verde cinza C

		-0.5, 1.0, 0.5, 0.0, 1.0, 1.0, //amarelo B
		 0.5, 1.0, -0.5, 0.0, 1.0, 1.0, //roxo C
		-0.5, 1.0, -0.5, 0.0, 1.0, 1.0, //amarelo a*/
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}
