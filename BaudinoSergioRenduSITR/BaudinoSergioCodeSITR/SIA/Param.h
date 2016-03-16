#pragma once
#include "Constantes.h"
/* Beginning program parameter
*/
namespace Param
{
	const int screen_width = 1024; // screen width
	const int screen_height = 1024; // screen height

	const int texture_width = 2048; // texture width
	const int texture_height = 2048; // texture height

	const int MirrorBounce = 6; // max mirror reflect

	// cam parameters
	const glm::vec3 cam_origin(0.f, 2.f, 4.f);
	const glm::vec3 cam_lookAt(0.f, 0.f, 0.f);
	const glm::vec3 cam_up(0.f, 1.f, 0.f);

	const float cam_fov = 90.f;
	const float cam_near = 0.1f;
	const float cam_far = 10000.f;

	// input parameters
	const float move_speed = 0.1f;
	const float rotate_speed = 2.f;

	// light parameters
	const float sun_ray = 4.f;
	const float sun_y = 3.f;

	// mesh file path
	//const char * mesh_path = "cube.obj";
	const char * mesh_path = "dragon_smooth.obj";
	// mesh params
	const glm::vec3 mesh_translate = glm::vec3(0.f, 1.f, 0.f);
	const glm::vec3 mesh_scale = glm::vec3(2.f, 1.f, 1.f);
	const float mesh_rotation_angle = Constantes::PI * 0.5f;
	const glm::vec3 mesh_rotation_axis = glm::vec3(0.f, 1.f, 0.f);
}