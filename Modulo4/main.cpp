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

// Função para gerar objetos de cena com base no número especificado, posicionando-os alternadamente ao longo do eixo horizontal.
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

// Função para inicializar os buffers de vértices (VBO) e o array de vértices (VAO) para um objeto OpenGL.
bool initializeBuffers(GLuint& VBO, GLuint& VAO, const std::vector<GLfloat>& vbuffer, int stride) {
    // Geração do buffer de vértices
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

    // Geração e configuração do array de vértices
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Configuração dos atributos de vértices (posição, cor, coordenadas de textura e normal)
    // Atributo 0: posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Atributo 2: coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Atributo 3: normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    // Limpeza dos bindings para evitar alterações acidentais
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

// Função para ler um arquivo OBJ, extrair dados de vértices, texturas, normais e materiais.
bool readOBJFile(const std::string& filepath, std::vector<GLuint>& indices, std::vector<GLfloat>& vbuffer, string& materialFileName, string& materialName) {
    // Cor inicial para os vértices
    glm::vec3 color = glm::vec3(1.0, 0.0, 1.0);

    // Vetores para armazenar coordenadas de textura, vértices e normais
    vector <glm::vec2> textureCoordinates;
    vector <glm::vec3> vertices;
    vector <glm::vec3> normals;

    // Abrindo o arquivo OBJ
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo OBJ: " << filepath << std::endl;
        return false;
    }

    // Iteração pelas linhas do arquivo
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream ssline(line);
        std::string word;
        ssline >> word;

        // Processamento de cada tipo de linha do arquivo OBJ
        if (word == "mtllib") {
            ssline >> materialFileName;
        }
        else if (word == "usemtl") {
            ssline >> materialName;
        }
        else if (word == "v") {
            // Leitura de vértices
            glm::vec3 v;
            ssline >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (word == "vt") {
            // Leitura de coordenadas de textura
            glm::vec2 vt;
            ssline >> vt.s >> vt.t;
            textureCoordinates.push_back(vt);
        }
        else if (word == "vn") {
            // Leitura de normais
            glm::vec3 vn;
            ssline >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        }
        else if (word == "f") {
            // Leitura de faces (triângulos)
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

                // Adicionando informações de vértice (posição, cor, textura e normal) ao buffer
                vbuffer.push_back(vertices[index].x);
                vbuffer.push_back(vertices[index].y);
                vbuffer.push_back(vertices[index].z);
                vbuffer.push_back(color.r);
                vbuffer.push_back(color.g);
                vbuffer.push_back(color.b);

                // Movendo para a próxima parte da string para obter o índice da textura
                tokens[i] = tokens[i].substr(pos + 1);
                pos = tokens[i].find("/");
                token = tokens[i].substr(0, pos);
                index = atoi(token.c_str()) - 1;

                // Adicionando coordenadas da textura ao buffer
                vbuffer.push_back(textureCoordinates[index].s);
                vbuffer.push_back(textureCoordinates[index].t);

                // Recuperando os índices de normais
                tokens[i] = tokens[i].substr(pos + 1);
                index = atoi(tokens[i].c_str()) - 1;
                vbuffer.push_back(normals[index].x);
                vbuffer.push_back(normals[index].y);
                vbuffer.push_back(normals[index].z);
            }
        }
    }

    inputFile.close();
    return true;
}

// Função para carregar um arquivo OBJ simples e inicializar buffers de vértices
int loadSimpleOBJ(const std::string& filepath, int& numVertices, string& materialFileName, string& materialName) {
	std::vector<GLuint> indices;
	std::vector<GLfloat> vbuffer;
	int stride = 11;

	if (!readOBJFile(filepath, indices, vbuffer, materialFileName, materialName)) {
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

// Função para carregar o nome do arquivo de textura associado ao material especificado em um arquivo MTL
string loadSimpleMTL(const std::string& filepath, string materialName)
{
	string textureFileName;
	std::ifstream inputFile(filepath); // Abre o arquivo MTL especificado

	if (!inputFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo MTL: " << filepath << std::endl;
		return "";
	}

	string line;
	bool materialFound = false;

	// Itera pelas linhas do arquivo MTL em busca do material especificado
	while (getline(inputFile, line))
	{
		istringstream ssline(line);
		string word;
		ssline >> word;

		// Verifica se encontrou um novo material no arquivo MTL
		if (word == "newmtl")
		{
			string currentMaterialName;
			ssline >> currentMaterialName;
			materialFound = (currentMaterialName == materialName);
		} 
		// Se encontrou o nome do material desejado, procura pela linha que contém a textura (map_Kd)
		else if (word == "map_Kd" && materialFound)
		{ 
			ssline >> textureFileName;
			break;
		}
	}

	inputFile.close();
	return textureFileName;
}

// Função para carregar uma textura a partir de um arquivo de imagem especificado
int loadTexture(string filepath)
{
	GLuint texID;

	// Gera o identificador da textura na memória e a associa ao contexto atual
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Define os parâmetros de wrapping e filtragem da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carrega a imagem a partir do arquivo especificado usando a biblioteca stb_image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	// Verifica se o carregamento da imagem foi bem-sucedido
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

		// Carrega os dados da imagem para a textura e gera os níveis de mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		// Exibe uma mensagem de erro caso ocorra falha no carregamento da textura
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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa Modulo 4 - Guilherme Dorr!", nullptr, nullptr);
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
    GLuint VAO = loadSimpleOBJ("../Modelos/Cube/cube.obj", numVertices, materialFileName, materialName);

	// Carregamento do arquivo MTL para obter as informações do material
	string textureFileName = loadSimpleMTL(materialFileName, materialName);

	//Carregando uma textura e armazenando o identificador na memória
	GLuint textureId = loadTexture(textureFileName);

	int quantidadeObjetos = 4;
	std::vector<SceneObject> sceneObjects = generateSceneObjects(quantidadeObjetos, VAO, numVertices, &shader, textureId);

	//Atualizando o shader com a posição da câmera
	shader.setVec3("camera_pos", 0.0, 0.0, 4.0);
	
	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.2);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("light_pos", 0.0, 10.0, 0.0);
	shader.setVec3("light_color", 1.0, 1.0, 0.8);


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
