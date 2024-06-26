#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class SceneObject
{
public:
	SceneObject(GLuint vertexArrayObject, int numVertices, Shader* shader, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0),
		float rotationAngle = 0.0, glm::vec3 rotationAxis = glm::vec3(0.0, 0.0, 1.0), float translationSpeed = 0.05)
	{
		this->vertexArrayObject = vertexArrayObject;
		this->numVertices = numVertices;
		this->shader = shader;
		this->position = position;
		this->scale = scale;
		this->rotationAngle = rotationAngle;
		this->rotationAxis = rotationAxis;
		this->translationSpeed = translationSpeed;
	}

	void setScale(const glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0))
	{
		this->scale = scale;
	}

	void updateModelMatrix()
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
		model = glm::scale(model, scale);
		shader->setMat4("model", glm::value_ptr(model));
	}

	void renderObject() const
	{
		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		glBindVertexArray(0);
	}

	void rotateX()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	void rotateY()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void rotateZ()
	{
		this->rotationAngle = (GLfloat)glfwGetTime() * 10;
		this->rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	void translateX(int direction = 1)
	{
		this->position += glm::vec3((direction * translationSpeed), 0.0f, 0.0f);
	}

	void translateY(int direction = 1)
	{
		this->position += glm::vec3(0.0f, (direction * translationSpeed), 0.0f);
	}

	void translateZ(int direction = 1)
	{
		this->position += glm::vec3(0.0f, 0.0f, (direction * translationSpeed));
	}

protected:
	GLuint vertexArrayObject;
	int numVertices;

	glm::vec3 position;
	glm::vec3 scale;
	float rotationAngle;
	glm::vec3 rotationAxis;
	float translationSpeed;

	Shader* shader;
};