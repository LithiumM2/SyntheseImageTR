#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
/*
* Program input, key and mouse, singleton.
*/
class Controller
{
public:
	static Controller& Instance();
	void ProcessInput(GLFWwindow *window, Camera & cam, int width, int height);
private:
	static Controller& instance;
	Controller();
	void ProcessMouse(GLFWwindow *window, Camera & cam, int width, int height);
	void ProcessKey(GLFWwindow *window, Camera & cam);
};