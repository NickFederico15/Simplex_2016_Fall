#ifndef APPCLASS_H
#define APPCLASS_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Simplex/Simplex.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Node.h"

#include <iostream>
#include <vector>

class Appclass
{
public:
	GLint init();
	GLint run();
	GLint end();

private:
	// window pointer
	GLFWwindow* window = nullptr;

	static Simplex::CameraManager* cameraManager;
	static Simplex::SystemSingleton* system;

	static std::vector<Node> adjacenyList;

	static GLfloat cameraSpeed;
	static bool runPath, getPath;
	static GLuint currentNode;
	GLint width, height;

	// callbacks
	static void printError(GLint error, const GLchar* errorString);
	static void userInput(GLFWwindow* window, GLint key, GLint code, GLint press, GLint modify);
};
#endif