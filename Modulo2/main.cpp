/**
 * 
 * Código desenvolvido por Guilherme Dorr
 * 
**/

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

glm::mat4 model = glm::mat4(1.0f);

//Dimensoes de janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte vertexShader
const GLchar* vertexShaderSource = "#version 410\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 410\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX=false, rotateY=false, rotateZ=false, w = false, a = false, s = false, d = false, i = false, j = false;
bool teclas[6] = {false, false, false, false, false, false};
// 0 - w    1 - a   2 - s   3 - d   4 - i   5 - j

// Declaração das funções
GLuint setupShader();
int setupGeometry();

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
    GLuint shaderID = setupShader();

    //Configurando a geometria (VAO, VBO)
    GLuint VAO = setupGeometry();

    //Ativando o programa de shader
    glUseProgram(shaderID);

// Configurando matrizes de modelo, visualização e projeção
    glm::mat4 model = glm::mat4(1.0f); // Matriz identidade
    glm::mat4 view = glm::lookAt(glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    GLint viewLoc = glGetUniformLocation(shaderID, "view");
    GLint projectionLoc = glGetUniformLocation(shaderID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
        //Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funcoes de callback correspondentes
        glfwPollEvents();

        //Limpa o buffer de cor e de profundidade
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

        //Atualizando a matriz de modelo
        model = glm::mat4(1.0f); 
        const float translationSpeed = 0.1f;

        if (rotateX) {
            
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        } else if (rotateY){
            
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (rotateZ){

            model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        } else if (teclas[0]) { // W

            model = glm::translate(model, glm::vec3(0.0f, 0.0f, translationSpeed));
        } else if (teclas[1]) { // A

            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -translationSpeed));
        } else if (teclas[2]) { // S

            model = glm::translate(model, glm::vec3(-translationSpeed, 0.0f, 0.0f));
        } else if (teclas[3]) { // D

            model = glm::translate(model, glm::vec3(translationSpeed, 0.0f, 0.0f));
        } else if (teclas[4]) { // I

            model = glm::translate(model, glm::vec3(0.0f, translationSpeed, 0.0f));
        } else if (teclas[5]) { // J

            model = glm::translate(model, glm::vec3(0.0f, -translationSpeed, 0.0f));
        }
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Chamada de desenho - drawcall
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
	}

    //Limpeza e desalocacao
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderID);
    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS) {
        switch (key) {

            case GLFW_KEY_X: rotateX = true; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} break;
            case GLFW_KEY_Y: rotateX = false; rotateY = true; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} break;
            case GLFW_KEY_Z: rotateX = false; rotateY = false; rotateZ = true; for(int i = 0; i < 7; i++) {teclas[i] = false;} break;
            case GLFW_KEY_W: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[0] = true; break;
            case GLFW_KEY_A: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[1] = true; break;
            case GLFW_KEY_S: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[2] = true; break;
            case GLFW_KEY_D: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[3] = true; break;
            case GLFW_KEY_I: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[4] = true; break;
            case GLFW_KEY_J: rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;} teclas[5] = true; break;
        }
    }

    if(action == GLFW_RELEASE) {
        
        rotateX = false; rotateY = false; rotateZ = false; for(int i = 0; i < 7; i++) {teclas[i] = false;}
    }

	// if (key == GLFW_KEY_X && action == GLFW_PRESS)
	// {
	// 	rotateX = true;
	// 	rotateY = false;
	// 	rotateZ = false;
	// }

	// if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	// {
	// 	rotateX = false;
	// 	rotateY = true;
	// 	rotateZ = false;
	// }

	// if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	// {
	// 	rotateX = false;
	// 	rotateY = false;
	// 	rotateZ = true;
	// }

    // const float translationSpeed = 0.001f;
    // if (action == GLFW_PRESS || action == GLFW_REPEAT)
    // {
    //     switch (key)
    //     {
    //         case GLFW_KEY_W:
    //             w = true; s = false; a = false; d = false; i = false; j = false;
    //             break;
    //         case GLFW_KEY_S:
    //             w = false; s = true; a = false; d = false; i = false; j = false;
    //             break;
    //         case GLFW_KEY_A:
    //             w = false; s = false; a = true; d = false; i = false; j = false;
    //             break;
    //         case GLFW_KEY_D:
    //             w = false; s = false; a = false; d = true; i = false; j = false;
    //             break;
    //         case GLFW_KEY_I:
    //             w = false; s = false; a = false; d = false; i = true; j = false;
    //             break;
    //         case GLFW_KEY_J:
    //             w = false; s = false; a = false; d = false; i = false; j = true;
    //             break;
    //     }
    // }
}


GLuint setupShader()
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


int setupGeometry()
{
	//Definicao de coordenadas para os vertices dos triangulos e cores
	GLfloat vertices[] = {
    // Face Inferior
    // Triângulo 1
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 2
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

    // Face Superior
    // Triângulo 3
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 4
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

    // Face Frente
    // Triângulo 5
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 6
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

    // Face Traseira
    // Triângulo 7
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 8
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

    // Face Esquerda
    // Triângulo 9
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 10
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

    // Face Direita
    // Triângulo 11
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    // Triângulo 12
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
};

GLuint VBO, VAO;

// Gera e vincula o VAO
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// Gera e vincula o VBO
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);

// Copia os dados do vértice para o buffer
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Atributo posição (x, y, z)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
glEnableVertexAttribArray(0);

// Atributo cor (r, g, b)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
glEnableVertexAttribArray(1);

// Desvincula o VBO
glBindBuffer(GL_ARRAY_BUFFER, 0);

// Desvincula o VAO
glBindVertexArray(0);

return VAO;
}