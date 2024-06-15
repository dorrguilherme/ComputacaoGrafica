/**
 * 
 * Código desenvolvido por Guilherme Dorr
 * 
**/
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <Shader.h>
#include <SceneObject.cpp>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

//Callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//Dimensoes de janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Variáveis de controle de rotacao, translacao, escala e direcao
bool rotateX = false, rotateY = false, rotateZ = false;        
bool translateX = false, translateY = false, translateZ = false;
int translateDirection = 0;
float scale = 1.0;      

int setupGeometry();

// Funcao de ajuste de escala
void adjustScale(int key, int action)
{
    // Verifica se a tecla de escala foi pressionada
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        scale -= 0.1f;
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        scale += 0.1f; // Marca a escala como aplicada
    }
}

// Funcao de ajuste de rotacao
void adjustRotation(int key, int action) {
    switch (key) {
        case(GLFW_KEY_X):
            if (action == GLFW_PRESS) {
                rotateX = true; 
                rotateY = false;
                rotateZ = false;
            }
            break;
        case(GLFW_KEY_Y):
            if (action == GLFW_PRESS) {
                rotateX = false;
                rotateY = true;
                rotateZ = false;
            }
            break;
        case(GLFW_KEY_Z):
            if (action == GLFW_PRESS) {
                rotateX = false;
                rotateY = false;
                rotateZ = true;
            }
            break;
        default:
            break;
    }
}

// Funcao de ajuste de translacao
void adjustTranslation(int key)
{
	switch (key)
	{
	case(GLFW_KEY_W):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_S):
		translateX = true;
		translateY = false;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_I):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = 1;
		break;
	case(GLFW_KEY_J):
		translateX = false;
		translateY = true;
		translateZ = false;
		translateDirection = -1;
		break;
	case(GLFW_KEY_A):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = 1;
		break;
	case(GLFW_KEY_D):
		translateX = false;
		translateY = false;
		translateZ = true;
		translateDirection = -1;
		break;
	default:
		break;
	}
}

// Funcao de retorno ao estado original de variaveis de controle
void resetTranslationVariables() {
	translateX = false;
	translateY = false;
	translateZ = false;
	translateDirection = 0;
}

// Cria e retorna um vetor de objetos da cena, representando cubos, distribuídos horizontalmente, com base no número fornecido (numCubes)

std::vector<SceneObject> gerarCubos(int numCubes, GLuint vertexArrayObject, int numVertices, Shader* shader) {
	std::vector<SceneObject> cubos;

	const float espacamento = 2.0f;

	for (int i = 0; i < numCubes; ++i)
	{
		float posX = 0.0f;

		if (i % 2 == 0)
			posX = (-espacamento) * (i / 2);
		else
			posX = (espacamento) * ((i / 2) + 1);

		cubos.push_back(SceneObject(vertexArrayObject, numVertices, shader, glm::vec3(posX, 0.0, 0.0)));
	}

	return cubos;
}

bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Especificando os atributos do vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool readOBJFile(const std::string& filepath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer) {
	// Vetor de cores para mapeamento de cores normais
	std::vector<glm::vec3> colors = {
		glm::vec3(1.0f, 0.0f, 0.0f),   // Vermelho
		glm::vec3(0.0f, 1.0f, 0.0f),   // Verde
		glm::vec3(0.0f, 0.0f, 1.0f),   // Azul
		glm::vec3(1.0f, 1.0f, 0.0f),   // Amarelo
		glm::vec3(1.0f, 0.0f, 1.0f),   // Magenta
		glm::vec3(0.0f, 1.0f, 1.0f)    // Ciano
	};

	// Abrindo o arquivo OBJ
	std::ifstream inputFile(filepath);
	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo OBJ: " << filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inputFile, line)) {
		std::istringstream ssline(line);
		std::string word;
		ssline >> word;

		if (word == "v") {
			glm::vec3 v;
			ssline >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (word == "f") {
			std::string tokens[3];
			ssline >> tokens[0] >> tokens[1] >> tokens[2];

			for (int i = 0; i < 3; ++i) {
				int posLastValue = tokens[i].find_last_of('/');
				std::string lastValue = tokens[i].substr(posLastValue + 1);
				int normal = std::stoi(lastValue);
				glm::vec3 normalColors = colors[normal - 1];

				int pos = tokens[i].find("/");
				std::string token = tokens[i].substr(0, pos);
				int index = std::atoi(token.c_str()) - 1;
				indices.push_back(index);

				vbuffer.push_back(vertices[index].x);
				vbuffer.push_back(vertices[index].y);
				vbuffer.push_back(vertices[index].z);

				vbuffer.push_back(normalColors.r);
				vbuffer.push_back(normalColors.g);
				vbuffer.push_back(normalColors.b);
			}
		}
	}

	inputFile.close();
	return true;
}

// Função principal para carregar um arquivo OBJ e inicializar os buffers de vértices e arrays de vértices (VAO e VBO)
int loadSimpleOBJ(const std::string& filepath, int& numVertices) {
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vbuffer;

	if (!readOBJFile(filepath, vertices, indices, vbuffer)) {
		std::cerr << "Erro ao ler o arquivo OBJ: " << filepath << std::endl;
		return -1;
	}

	numVertices = vbuffer.size() / 6;

	GLuint VBO, VAO;
	if (!initializeBuffers(VBO, VAO, vbuffer)) {
		std::cerr << "Erro ao inicializar os buffers de vértices e arrays de vértices." << std::endl;
		return -1;
	}

	return VAO;
}

int main()
{
	//Inicializacao da GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    //Configuracoes do contexto OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Necessario para Mac
#endif

    //Criacao da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa Modulo 2 -- Guilherme Dorr!", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Registro da funcao de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    //GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Obtendo as informações de versao
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    //Definindo as dimensoes da viewport com as mesmas dimensoes da janela da aplicacao
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //Compilando e buildando o programa de shader
    Shader shader("vertexshader.vs", "fragmentshader.fs");
    glUseProgram(shader.ID);

    int numVertices;
    GLuint VAO = loadSimpleOBJ("../Cube/cube.obj", numVertices);

    //Ativando o programa de shader
    glUseProgram(shader.ID);

    // Configurando matrizes de modelo, visualização e projeção
    glm::mat4 model = glm::mat4(1.0f); // Matriz identidade
    glm::mat4 view = glm::lookAt(glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    GLint modelLoc = glGetUniformLocation(shader.ID, "model");
    GLint viewLoc = glGetUniformLocation(shader.ID, "view");
    GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

    int quantidadeCubos = 4;
    std::vector<SceneObject> cubes = gerarCubos(quantidadeCubos, VAO, numVertices, &shader);

    while (!glfwWindowShouldClose(window))
	    {
		    resetTranslationVariables();

            // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
            glfwPollEvents();

            // Limpa o buffer de cor
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glLineWidth(10);
            glPointSize(20);

            for (int i = 0; i < cubes.size(); ++i)
            {
                if (rotateX)
                    cubes[i].rotateX();
                else if (rotateY)
                    cubes[i].rotateY();
                else if (rotateZ)
                    cubes[i].rotateZ();

                if (translateX)
                    cubes[i].translateX(translateDirection);
                else if (translateY)
                    cubes[i].translateY(translateDirection);
                else if (translateZ)
                    cubes[i].translateZ(translateDirection);

                cubes[i].setScale(glm::vec3(scale, scale, scale));
                cubes[i].updateModelMatrix();
                cubes[i].renderObject();
            }

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

    //Limpeza e desalocacao
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader.ID);
    glfwTerminate();

    return 0;
}

// Função callback acionada quando há interação com o teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	adjustScale(key, action);
	adjustRotation(key, action);
	adjustTranslation(key);
}
