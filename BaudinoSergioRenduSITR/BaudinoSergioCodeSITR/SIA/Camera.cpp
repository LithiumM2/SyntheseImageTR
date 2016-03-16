#include "Camera.h"


Camera::Camera(glm::vec3 _position, glm::vec3 _lookAt, glm::vec3 _up, float _fov, float _near, float _far, float _speed, float _mouseSpeed)
	: position(_position), lookAt(_lookAt), fov(_fov), near(_near), far(_far), speed(_speed), mouseSpeed(_mouseSpeed)
{
	forward = glm::normalize(_lookAt - _position);
	up = glm::normalize(_up);
	right = glm::cross(forward, up);
}
void Camera::ProcessDirectionVector()
{
	forward = glm::normalize(lookAt - position);
	lookAt = position + forward;
	right = glm::cross(forward, up);
}


void Camera::MoveForward()
{
	Translate(forward * speed);
}

void Camera::MoveBackward()
{
	Translate(-forward * speed);
}

void Camera::MoveRight()
{
	Translate(right * speed);
}

void Camera::MoveLeft()
{
	Translate(- right * speed);
}

void Camera::Translate(glm::vec3 & t)
{
	position += t;
	lookAt += t;
	ProcessDirectionVector();
}

void Camera::MoveLookAt(double mouse_x, double mouse_y, int width, int height)
{
	float x = (float(mouse_x) - (width * 0.5f)) / (width * 0.5f);
	float y = (float(mouse_y) - (height * 0.5f)) / (height * 0.5f);
	float angle_x = mouseSpeed * x;
	float angle_y = mouseSpeed * y;


	glm::mat4x4 rotation_x = glm::rotate(glm::mat4x4(1.f), angle_x, up);
	glm::vec4 tmpForward = glm::vec4(forward.x, forward.y, forward.z, 1.f) * rotation_x;
	forward = glm::normalize(glm::vec3(tmpForward.x, tmpForward.y, tmpForward.z) / tmpForward.w);
	right = glm::cross(forward, up);

	 tmpForward = glm::vec4(forward.x, forward.y, forward.z, 1.f) * glm::rotate(glm::mat4x4(1.f), angle_y, right);
	forward = glm::normalize(glm::vec3(tmpForward.x, tmpForward.y, tmpForward.z) / tmpForward.w);

	lookAt = position + forward;

}


glm::mat4x4 Camera::GetCamView() const
{
	return glm::lookAt(position,
		position + forward,
		up);
}


glm::mat4x4 Camera::GetCamProj(int width, int height) const
{

	return glm::perspective(fov * Constantes::PI / 180.f, (float)width / height, near, far);
}