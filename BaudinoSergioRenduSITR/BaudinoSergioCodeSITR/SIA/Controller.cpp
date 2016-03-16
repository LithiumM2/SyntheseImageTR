#include "Controller.h"
/**
* Singleton stuff
*/
Controller& Controller::instance = Controller();
Controller&	Controller::Instance()
{
	return instance;
}

Controller::Controller()
{
}


void Controller::ProcessInput(GLFWwindow *window, Camera & cam, int width, int height)
{
	ProcessKey(window, cam);
	ProcessMouse(window, cam, width, height);
}

void Controller::ProcessMouse(GLFWwindow *window, Camera & cam, int width, int height)
{
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	cam.MoveLookAt(mouse_x, mouse_y, width, height);
	glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
}

void Controller::ProcessKey(GLFWwindow *window, Camera & cam)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cam.MoveForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam.MoveBackward();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam.MoveRight();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam.MoveLeft();
	}
}