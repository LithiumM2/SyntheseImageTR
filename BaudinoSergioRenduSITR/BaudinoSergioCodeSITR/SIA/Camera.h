#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Constantes.h"
class Camera
{
	glm::vec3 position;
	glm::vec3 lookAt;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	float fov;
	float near;
	float far;
	float speed;
	float mouseSpeed;
	void ProcessDirectionVector();
	void Translate(glm::vec3 & t);
public:

	Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, float fov, float near, float far, float speed, float mouseSpeed);
	
	// Controlleur Update
	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	void MoveLookAt(double mouse_x, double mouse_y, int width, int height);

	// Model
	glm::mat4x4 GetCamView() const;
	glm::mat4x4 GetCamProj(int width, int height) const;
};