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
#include "stb_image.h"

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

std::vector<SceneObject> generateSceneObjects(int quantidadeObjetos, GLuint vertexArrayObject, int numVertices, Shader* shader, GLuint textureId) {
	std::vector<SceneObject> objetos;

	const float espacoHorizontal = 2.0f;

	for (int i = 0; i < quantidadeObjetos; ++i)
	{
		float posX = 0.0f;

		if (i % 2 == 0)
			posX = (-espacoHorizontal) * (i / 2);
		else
			posX = (espacoHorizontal) * ((i / 2) + 1);

		objetos.push_back(SceneObject(vertexArrayObject, numVertices, shader, textureId, glm::vec3(posX, 0.0, 0.0)));
	}

	return objetos;
}

bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer, int stride) {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Especificando os atributos do vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

bool readOBJFile(const std::string& filepath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer, std::vector<glm::vec2>& textureCoordinates, string& materialFileName, string& materialName) {

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

		if (word == "mtllib") {
			ssline >> materialFileName;
		}
		else if (word == "usemtl") { 
			ssline >> materialName;
		}
		else if (word == "v") {
			glm::vec3 v;
			ssline >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (word == "vt")
		{
			glm::vec2 vt;
			ssline >> vt.s >> vt.t;
			textureCoordinates.push_back(vt);
		}
		else if (word == "f") {
			std::string tokens[3];
			ssline >> tokens[0] >> tokens[1] >> tokens[2];

			for (int i = 0; i < 3; ++i) {
				int posLastValue = tokens[i].find_last_of('/');
				std::string lastValue = tokens[i].substr(posLastValue + 1);
				int normal = std::stoi(lastValue);

				int pos = tokens[i].find("/");
				std::string token = tokens[i].substr(0, pos);
				int index = std::atoi(token.c_str()) - 1;
				indices.push_back(index);

				vbuffer.push_back(vertices[index].x);
				vbuffer.push_back(vertices[index].y);
				vbuffer.push_back(vertices[index].z);

				vbuffer.push_back(vertices[index].r);
				vbuffer.push_back(vertices[index].g);
				vbuffer.push_back(vertices[index].b);

				
				tokens[i] = tokens[i].substr(pos + 1);
				pos = tokens[i].find("/");
				token = tokens[i].substr(0, pos);
				index = atoi(token.c_str()) - 1;

				
				vbuffer.push_back(textureCoordinates[index].s);
				vbuffer.push_back(textureCoordinates[index].t);
			}
		}
	}

	inputFile.close();
	return true;
}

int loadSimpleOBJ(const std::string& filepath, int& numVertices, string& materialFileName, string& materialName) {
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	vector <glm::vec2> textureCoordinates;
	std::vector<GLfloat> vbuffer;
	int stride = 8;

	if (!readOBJFile(filepath, vertices, indices, vbuffer, textureCoordinates, materialFileName, materialName)) {
		std::cerr << "Erro ao ler o arquivo OBJ: " << filepath << std::endl;
		return -1;
	}

	numVertices = vbuffer.size() / stride;

	GLuint VBO, VAO;
	if (!initializeBuffers(VBO, VAO, vbuffer, stride)) {
		std::cerr << "Erro ao inicializar os buffers de vértices e arrays de vértices." << std::endl;
		return -1;
	}

	return VAO;
}

string loadSimpleMTL(const std::string& filepath, string materialName)
{
	string textureFileName;
	std::ifstream inputFile(filepath);

	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo MTL: " << filepath << std::endl;
		return "";
	}

	string line;
	bool materialFound = false;

	while (getline(inputFile, line))
	{
		istringstream ssline(line);
		string word;
		ssline >> word; 
		
		if (word == "newmtl")
		{
			string currentMaterialName;
			ssline >> currentMaterialName;
			materialFound = (currentMaterialName == materialName);
		} else if (word == "map_Kd" && materialFound) { 
			ssline >> textureFileName;
			break;
		}
	}

	inputFile.close();
	return textureFileName;
}

int loadTexture(string filepath)
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
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		switch (nrChannels) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cerr << "Número de canais não suportado: " << nrChannels << std::endl;
			stbi_image_free(data);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Falha ao carregar a textura" << std::endl;
		return 0;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa Modulo 3-- Guilherme Dorr!", nullptr, nullptr);
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

    //Matriz de view
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

    int numVertices;
	string materialFileName;
	string materialName;
    GLuint VAO = loadSimpleOBJ("../Modelos/Suzane/SuzanneTriTextured.obj", numVertices, materialFileName, materialName);
	//GLuint VAO = loadSimpleOBJ("../Modelos/Suzane/Suzanne2024.obj", numVertices, materialFileName, materialName);
	//GLuint VAO = loadSimpleOBJ("../Modelos/Cube/cube.obj", numVertices, materialFileName, materialName);

	// Carregamento do arquivo MTL para obter as informações do material
	string textureFileName = loadSimpleMTL(materialFileName, materialName);

	std::cerr << textureFileName << std::endl;

	//Carregando uma textura e armazenando o identificador na memória
	GLuint textureId = loadTexture(textureFileName);

	int quantidadeObjetos = 4;
	std::vector<SceneObject> sceneObjects = generateSceneObjects(quantidadeObjetos, VAO, numVertices, &shader, textureId);

	glEnable(GL_DEPTH_TEST);

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

            for (int i = 0; i < sceneObjects.size(); ++i)
            {
                if (rotateX)
                    sceneObjects[i].rotateX();
                else if (rotateY)
                    sceneObjects[i].rotateY();
                else if (rotateZ)
                    sceneObjects[i].rotateZ();

                if (translateX)
                    sceneObjects[i].translateX(translateDirection);
                else if (translateY)
                    sceneObjects[i].translateY(translateDirection);
                else if (translateZ)
                    sceneObjects[i].translateZ(translateDirection);

                sceneObjects[i].setScale(glm::vec3(scale, scale, scale));
                sceneObjects[i].updateModelMatrix();
                sceneObjects[i].renderObject();
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
