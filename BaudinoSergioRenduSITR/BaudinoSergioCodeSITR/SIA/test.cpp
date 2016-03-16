#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <list>
#include <ctime>

#include "SOIL.h"


#include "Controller.h"
#include "Mesh.h"
#include "Constantes.h"
#include "Param.h"

void render(GLFWwindow*);
void init();

void drawScene(int width, int height, const glm::vec4 & sunPos, const glm::vec4 & colorMesh, int level, const glm::mat4x4 & model, const int side);
#define glInfo(a) std::cout << #a << ": " << glGetString(a) << std::endl


// This function is called on any openGL API error
void APIENTRY debug(GLenum, // source
		   GLenum, // type
		   GLuint, // id
		   GLenum, // severity
		   GLsizei, // length
		   const GLchar *message,
		   const void *) // userParam
{
	std::cout << "DEBUG: " << message << std::endl;
}
GLFWwindow* window;
int main(void)
{
	
	//GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cerr << "Could not init glfw" << std::endl;
		return -1;
	}

	// This is a debug context, this is slow, but debugs, which is interesting
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Param::screen_width, Param::screen_height, "Hello World", NULL, NULL);
	if (!window)
	{
		std::cerr << "Could not init window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		std::cerr << "Could not init GLEW" << std::endl;
		std::cerr << glewGetErrorString(err) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Now that the context is initialised, print some informations
	glInfo(GL_VENDOR);
	glInfo(GL_RENDERER);
	glInfo(GL_VERSION);
	glInfo(GL_SHADING_LANGUAGE_VERSION);

	// And enable debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	
	glDebugMessageCallback(debug, nullptr);

	// This is our openGL init function which creates ressources
	init();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		render(window);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	
	glfwTerminate();
	return 0;
}

// Build a shader from a string
GLuint buildShader(GLenum const shaderType, std::string const src)
{
	GLuint shader = glCreateShader(shaderType);

	const char* ptr = src.c_str();
	GLint length = src.length();

	glShaderSource(shader, 1, &ptr, &length);

	glCompileShader(shader);

	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if(!res)
	{
		std::cerr << "shader compilation error" << std::endl;

		char message[1000];

		GLsizei readSize;
		glGetShaderInfoLog(shader, 1000, &readSize, message);
		message[999] = '\0';

		std::cerr << message << std::endl;

		glfwTerminate();
		exit(-1);
	}

	return shader;
}

// read a file content into a string
std::string fileGetContents(const std::string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

// build a program with a vertex shader and a fragment shader
GLuint buildProgram(const std::string vertexFile, const std::string fragmentFile)
{
	auto vshader = buildShader(GL_VERTEX_SHADER, fileGetContents(vertexFile));
	auto fshader = buildShader(GL_FRAGMENT_SHADER, fileGetContents(fragmentFile));

	GLuint program = glCreateProgram();

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glLinkProgram(program);
	
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if(!res)
	{
		std::cerr << "program link error" << std::endl;

		char message[1000];

		GLsizei readSize;
		glGetProgramInfoLog(program, 1000, &readSize, message);
		message[999] = '\0';

		std::cerr << message << std::endl;

		glfwTerminate();
		exit(-1);
	}

	return program;
}

/****************************************************************
 ******* INTERESTING STUFFS HERE ********************************
 ***************************************************************/

// Store the global state of your program
struct
{

	int width = Param::screen_width; // screen width
	int height = Param::screen_height; // screen height

	Camera cam = Camera(Param::cam_origin, Param::cam_lookAt, Param::cam_up, 
		Param::cam_fov, Param::cam_near, Param::cam_far,
		Param::move_speed, Param::rotate_speed); // camera

	int maxBounce = Param::MirrorBounce;

	GLsizei texture_width = Param::texture_width; // texture width
	GLsizei texture_height = Param::texture_height; // texture height

	float sun_ray = Param::sun_ray;
	float sun_y = Param::sun_y;

	GLuint mirrorStencil; // texture for stencil
	GLuint depthTexture;// texture for depth
	GLuint skyboxTexture;// texture for depth

	GLuint vao; // a vertex array object
	GLuint vaoMirror; // a vertex array object for rendering mirror
	GLuint vaoMirror2; // a vertex array object for rendering mirror 2
	GLuint vaoMirror3; // a vertex array object for rendering back mirror 1
	GLuint vaoMirror4; // a vertex array object for rendering back mirror 2
	GLuint vaoSun; // a vertex array object for rendering texture
	GLuint vaoSkybox; // a vertex array object for rendering texture

	GLuint fbo; // frame buffer object

	GLuint program; // a shader for rendering scene
	GLuint program2; // a shader for rendering texture
	GLuint program3; // a shader for rendering stencil
	GLuint program4; // a shader for rendering sun
	GLuint program5; // a shader for rendering skybox

	// Mesh vertex number
	int meshSize;
	int meshSizeMirror;
	int meshSizeReflect;
	int meshSizeSun;
	int meshSizeSkybox;

} gs;

/** Init a buffer
* buffer : buffer index
* bufferData : buffer containing vertices
* size : size in bytes of buffer data 
*/
void initBuffer(GLuint & buffer, float * bufferData, int size)
{
	glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, bufferData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/** Init a vao with vertex and normal
* vao : vao index
* buffer : buffer index
* normalbuffer : normalbuffer buffer index
*/
void initVaoBasic(GLuint & vao, GLuint & buffer, GLuint & normalbuffer)
{
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

/** Init a vao mirror with vertex and normal
* vaoMirror : vaoMirror index
* bufferMirror : bufferMirror index
* bufferMirrorNormal : bufferMirrorNormal buffer index
*/
void initVaoMirror(GLuint & vaoMirror, GLuint & bufferMirror, GLuint & bufferMirrorNormal)
{
	glCreateVertexArrays(1, &vaoMirror);
	glBindVertexArray(vaoMirror);
		glBindBuffer(GL_ARRAY_BUFFER, bufferMirror);
			glEnableVertexAttribArray(14);
			glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, bufferMirrorNormal);
			glEnableVertexAttribArray(15);
			glVertexAttribPointer(15, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void init()
{
	/**********************************************************************************************************************/
	/**************************************************** Program *********************************************************/
	/**********************************************************************************************************************/

	gs.program = buildProgram("basic.vsl", "basic.fsl");
	gs.program2 = buildProgram("basic.vsl", "basicTexture.fsl"); // a shader for rendering texture
	gs.program3 = buildProgram("basicStencil.vsl", "basicStencil.fsl"); // a shader for rendering stencil
	gs.program4 = buildProgram("basicSun.vsl", "basicSun.fsl"); // a shader for rendering sun
	gs.program5 = buildProgram("basicSkybox.vsl", "basicSkybox.fsl"); // a shader for rendering skybox
	/**********************************************************************************************************************/
	/***************************************************  Geometry  *******************************************************/
	/**********************************************************************************************************************/
	// Obj 
	Mesh m = Mesh::fromObj(Param::mesh_path, Param::mesh_translate, Param::mesh_scale, Param::mesh_rotation_angle, Param::mesh_rotation_axis);
	// Ground
	m.Merge(Mesh::Quadrangle(glm::vec3(-5.F, -0.5f, -5.F),
		glm::vec3(-5.F, -0.5f, 5.F),
		glm::vec3(5.F, -0.5f, -5.F),
		glm::vec3(5.F, -0.5f, 5.F)
		));
	// Skybox
	Mesh skybox = Mesh::fromObjWithUv("skybox.obj", glm::vec3(0.f, 0.0f, 0.f), glm::vec3(1000.f, 1000.f, 1000.f), 0.f, glm::vec3(0.f, 1.f, 0.f));
	// Sun
	Mesh sun = Mesh::Sphere(1.5f, 300, 300);
	// Mirrors
	Mesh mirror = Mesh::Quadrangle(glm::vec3(-5.f, -.5f, 5.f),
		glm::vec3(-5.f, 2.5f, 5.f),
		glm::vec3(5.f, -.5f, 5.f),
		glm::vec3(5.f, 2.5f, 5.f)
		);

	Mesh mirror2 = Mesh::Quadrangle(glm::vec3(5.f, -.5f, -5.f),
		glm::vec3(5.f, 2.5f, -5.f),
		glm::vec3(-5.f, -.5f, -5.f),
		glm::vec3(-5.f, 2.5f, -5.f)
		);
	Mesh mirror3 = Mesh::Quadrangle(glm::vec3(-5.f, -.5f, 5.001f),
		glm::vec3(-5.f, 2.5f, 5.001f),
		glm::vec3(5.f, -.5f, 5.001f),
		glm::vec3(5.f, 2.5f, 5.001f)
		);

	Mesh mirror4 = Mesh::Quadrangle(glm::vec3(5.f, -.5f, -5.001f),
		glm::vec3(5.f, 2.5f, -5.001f),
		glm::vec3(-5.f, -.5f, -5.001f),
		glm::vec3(-5.f, 2.5f, -5.001f)
		);

	/**********************************************************************************************************************/
	/****************************************************   VBOs   ********************************************************/
	/**********************************************************************************************************************/
    // Mesh VBO
	gs.meshSize = m.getVBODataSize();
	float * data = new float[gs.meshSize];
	float * normal = new float[gs.meshSize];
	m.ExportVBOData(data, normal);

	int meshSizeBytes = gs.meshSize * sizeof(float);

	// Sun VBO
	gs.meshSizeSun = sun.getVBODataSize();
	float * dataSun = new float[gs.meshSizeSun];
	float * normalSun = new float[gs.meshSizeSun];
	sun.ExportVBOData(dataSun, normalSun);

	int meshSizeSunBytes = gs.meshSizeSun * sizeof(float);

	// Mirrors vbo
	gs.meshSizeMirror = mirror.getVBODataSize();
	float * dataMirror = new float[gs.meshSizeMirror];
	float * normalMirror = new float[gs.meshSizeMirror];

	float * dataMirror2 = new float[gs.meshSizeMirror];
	float * normalMirror2 = new float[gs.meshSizeMirror];

	float * dataMirror3 = new float[gs.meshSizeMirror];
	float * normalMirror3 = new float[gs.meshSizeMirror];

	float * dataMirror4 = new float[gs.meshSizeMirror];
	float * normalMirror4 = new float[gs.meshSizeMirror];
	mirror.ExportVBOData(dataMirror, normalMirror);

	mirror2.ExportVBOData(dataMirror2, normalMirror2);
	mirror3.ExportVBOData(dataMirror3, normalMirror3);
	mirror4.ExportVBOData(dataMirror4, normalMirror4);
	int meshSizeMirrorBytes = gs.meshSizeMirror * sizeof(float);
	// Skybox vbo
	gs.meshSizeSkybox = skybox.getVBODataSize();
	int uvSizeSkybox = skybox.getVBOUvSize();
	float * dataSkybox = new float[gs.meshSizeSkybox];
	float * normalSkybox = new float[gs.meshSizeSkybox];
	int meshSizeSkyboxBytes = gs.meshSizeSkybox * sizeof(float);
	skybox.ExportVBOData(dataSkybox, normalSkybox);

	// Mesh / 4 because drawing use vector 4
	gs.meshSize = (int)(gs.meshSize * 0.25f);
	gs.meshSizeMirror = (int)(gs.meshSizeMirror * 0.25f);
	gs.meshSizeSun = (int)(gs.meshSizeSun * 0.25f);
	gs.meshSizeSkybox = (int)(gs.meshSizeSkybox * 0.25f);

	/**********************************************************************************************************************/
	/**************************************************   Textures   ******************************************************/
	/**********************************************************************************************************************/
	// Depth texture
	glGenTextures(1, &gs.depthTexture);
	glBindTexture(GL_TEXTURE_2D, gs.depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, gs.texture_width, gs.texture_height);
	// Skybox cubemap
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &gs.skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gs.skyboxTexture);
	// loading texture image																		
	gs.skyboxTexture = SOIL_load_OGL_cubemap
		(
			"skyrender0001.tga",
			"skyrender0004.tga",
			"skyrender0003.tga",
			"skyrender0006.tga",
			"skyrender0005.tga",
			"skyrender0002.tga",
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);

	/**********************************************************************************************************************/
	/****************************************************   FBOs   ********************************************************/
	/**********************************************************************************************************************/
	// Frame buffer object for depth texture
	glGenFramebuffers(1, &gs.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gs.fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gs.depthTexture, 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Texture bind and activate parameters
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gs.depthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gs.skyboxTexture);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	/**********************************************************************************************************************/
	/***************************************************   Buffers   ******************************************************/
	/**********************************************************************************************************************/
	// Declaration buffer

	GLuint bufferTest;
	GLuint normalbuffer;
	GLuint bufferMirror;
	GLuint bufferMirrorNormal;
	GLuint bufferMirror2;
	GLuint bufferMirrorNormal2;
	GLuint bufferMirror3;
	GLuint bufferMirrorNormal3;
	GLuint bufferMirror4;
	GLuint bufferMirrorNormal4;
	GLuint bufferSun;
	GLuint bufferSunNormal;
	GLuint bufferSkybox;
	GLuint bufferSkyboxNormal;

	// Mesh + Ground vertex data
	initBuffer(bufferTest, data, meshSizeBytes);
	
	// Mesh + Ground normal data
	initBuffer(normalbuffer, normal, meshSizeBytes);

	// Mirrors buffers
	initBuffer(bufferMirror, dataMirror, meshSizeMirrorBytes);
	initBuffer(bufferMirrorNormal, normalMirror, meshSizeMirrorBytes);
	initBuffer(bufferMirror2, dataMirror2, meshSizeMirrorBytes);
	initBuffer(bufferMirrorNormal2, normalMirror2, meshSizeMirrorBytes);
	initBuffer(bufferMirror3, dataMirror3, meshSizeMirrorBytes);
	initBuffer(bufferMirrorNormal3, normalMirror3, meshSizeMirrorBytes);
	initBuffer(bufferMirror4, dataMirror4, meshSizeMirrorBytes);
	initBuffer(bufferMirrorNormal4, normalMirror4, meshSizeMirrorBytes);

	// Sun buffers
	initBuffer(bufferSun, dataSun, meshSizeSunBytes);
	initBuffer(bufferSunNormal, normalSun, meshSizeSunBytes);

	// Skybox buffers
	initBuffer(bufferSkybox, dataSkybox, meshSizeSkyboxBytes);
	initBuffer(bufferSkyboxNormal, normalSkybox, meshSizeSkyboxBytes);
	/**********************************************************************************************************************/
	/****************************************************   VAOs   ********************************************************/
	/**********************************************************************************************************************/
	// VAO Mesh + ground
	initVaoBasic(gs.vao, bufferTest, normalbuffer);
	initVaoBasic(gs.vaoSun, bufferSun, bufferSunNormal);
	initVaoBasic(gs.vaoSkybox, bufferSkybox, bufferSkyboxNormal);

	// VAO mirrors
	initVaoMirror(gs.vaoMirror, bufferMirror, bufferMirrorNormal);
	initVaoMirror(gs.vaoMirror2, bufferMirror2, bufferMirrorNormal2);
	initVaoMirror(gs.vaoMirror3, bufferMirror3, bufferMirrorNormal3);
	initVaoMirror(gs.vaoMirror4, bufferMirror4, bufferMirrorNormal4);

	/**********************************************************************************************************************/
	/***********************************************   OpenGL Config   ****************************************************/
	/**********************************************************************************************************************/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetCursorPos(window, gs.width * 0.5f, gs.height * 0.5f);
}

/* Draw shadow map in fbo and bind texture
* mLight : mvp light matrix
*/
void drawShadowMap(const glm::mat4x4 &mLight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gs.fbo);

	glViewport(0, 0, gs.texture_width, gs.texture_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gs.program2);
	glProgramUniformMatrix4fv(gs.program2, 2, 1, GL_FALSE, &mLight[0][0]);

	glBindVertexArray(gs.vao);
	{
		glDrawArrays(GL_TRIANGLES, 0, gs.meshSize);
	}

	glBindVertexArray(0);
	glUseProgram(0);

	glViewport(0, 0, gs.width, gs.height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

/* Draw mesh in main frame
* sunPos : sun position in world space
* m : mvp camera
*/
void drawSun(const glm::vec4 & sunPos, const glm::mat4x4 & m)
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(gs.program4);

	glProgramUniform4fv(gs.program4, 3, 1, &sunPos[0]);
	glProgramUniformMatrix4fv(gs.program4, 2, 1, GL_FALSE, &m[0][0]);

	glBindVertexArray(gs.vaoSun);
	{
		glDrawArrays(GL_TRIANGLES, 0, gs.meshSizeSun);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}


/* Draw mesh in main frame
* m : mvp camera
*/
void drawSkybox(const glm::mat4x4 & m)
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(gs.program5);
	glProgramUniformMatrix4fv(gs.program5, 2, 1, GL_FALSE, &m[0][0]);


	glBindTexture(GL_TEXTURE_CUBE_MAP, gs.skyboxTexture);

	glProgramUniform1i(gs.program5, 5, 1);
	glBindVertexArray(gs.vaoSkybox);
	{
		glDrawArrays(GL_TRIANGLES, 0, gs.meshSizeSkybox);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

/* Draw mesh in main frame
* color : mesh color
* sunPos : sun position in world space
* m : mvp camera
* mLight : mvp light
*/
void drawMesh(const glm::vec4 &color, const glm::vec4 & sunPos, const glm::mat4x4 & m, const glm::mat4x4 & mLight)
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(gs.program);

	glBindTexture(GL_TEXTURE_2D, gs.depthTexture);

	glGenerateMipmap(GL_TEXTURE_2D);
	glProgramUniform4fv(gs.program, 6, 1, &color[0]);
	glProgramUniform1i(gs.program, 5, 0);
	glProgramUniform4fv(gs.program, 3, 1, &sunPos[0]);
	glProgramUniformMatrix4fv(gs.program, 2, 1, GL_FALSE, &m[0][0]);

	glProgramUniformMatrix4fv(gs.program, 4, 1, GL_FALSE, &mLight[0][0]);

	glBindVertexArray(gs.vao);
	{
		glDrawArrays(GL_TRIANGLES, 0, gs.meshSize);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

/* draw mirror mesh
* m : mvp camera
* vao : vao mirror index
*/
void drawMirror(const glm::mat4x4 & m, GLuint vao)
{
	glUseProgram(gs.program3);

	glProgramUniformMatrix4fv(gs.program3, 17, 1, GL_FALSE, &m[0][0]);
	glBindVertexArray(vao);
	{
		glDrawArrays(GL_TRIANGLES, 0, gs.meshSizeMirror);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

/* check if level is fist bounce for mirrors camera
*  level : level to check
* return : true if level is first bounce
*/
bool isFirstBounce(int level)
{
	return level == gs.maxBounce;
}

/** Process mirror model with current reflection model
* model : curent reflection model matrix
* return : mirror model in the model parameter
*/
glm::mat4x4 getMirrorModel(const glm::mat4x4 & model)
{
	glm::mat4x4 modelTranslate = glm::translate(glm::mat4x4(1), glm::vec3(0.f, 0.f, 10.f));
	glm::mat4x4 modelScale = glm::scale(glm::mat4x4(1), glm::vec3(-1.f, 1.f, 1.f));
	glm::mat4x4 modelRotate = glm::rotate(glm::mat4x4(1), Constantes::PI, glm::vec3(0.f, 1.f, 0.f));
	return modelTranslate * modelRotate * modelScale * model;
}

/** Process mirror2 model with current reflection model
* model : curent reflection model matrix
* return : mirror2 model in the model parameter
*/
glm::mat4x4 getMirrorModel2(const glm::mat4x4 & model)
{
	glm::mat4x4 modelTranslate = glm::translate(glm::mat4x4(1), glm::vec3(0.f, 0.f, -10.f));
	glm::mat4x4 modelScale = glm::scale(glm::mat4x4(1), glm::vec3(1.f, 1.f, -1.f));
	glm::mat4x4 modelRotate = glm::rotate(glm::mat4x4(1), 0.f, glm::vec3(0.f, 1.f, 0.f));
	return modelTranslate * modelRotate * modelScale * model;
}

/** Process light view matrix for shadow map
* sunPos : sun position in world space
* return light view matrix 
*/
glm::mat4x4 getLightView(const glm::vec4 & sunPos)
{
	return glm::lookAt(glm::vec3(sunPos),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f));
}

/** draw skybox in stencil buffer
* width, height : windows size
* level : recursion level
* model :  current recursion model 
*/
void drawSceneSkybox(int width, int height, int level, const glm::mat4x4 & model)
{
	glm::mat4x4 camView = gs.cam.GetCamView();
	glm::mat4x4 projection = gs.cam.GetCamProj(width, height);
	glm::mat4x4 m = projection * camView * model;

	int currentLevel = (level + 1);

	glStencilMask(0x00);
	glDepthMask(GL_TRUE); // Write to depth buffer
	glStencilFunc(GL_EQUAL, currentLevel, 0xFF);

	drawSkybox(m);
}

/** draw scene in stencil buffer
* width, height : windows size
* sunPos : sun pos in world space
* colorMesh : mesh color
* level : recursion level
* model :  current recursion model
*/
void drawSceneRecMesh(int width, int height, const glm::vec4 & sunPos, const glm::vec4 & colorMesh, int level, const glm::mat4x4 & model)
{
	glm::mat4x4 camView = gs.cam.GetCamView();
	glm::mat4x4 projection = gs.cam.GetCamProj(width, height);
	glm::mat4x4 m = projection * camView * model;
	glm::mat4x4 lightView = getLightView(sunPos);
	glm::mat4x4 mLight = projection * lightView;

	mLight = Constantes::biasMatrix * mLight;

	glm::mat4x4 mirrorModel2 = getMirrorModel2(model);
	glm::mat4x4 mirrorModel = getMirrorModel(model);

	int currentLevel = (level + 1);

	glStencilMask(0x00);
	glDepthMask(GL_TRUE); // Write to depth buffer
	glStencilFunc(GL_EQUAL, currentLevel, 0xFF);

	drawSun(sunPos, m);
	drawMesh(colorMesh, sunPos, m, mLight);
}


/** draw mirror in stencil buffer
* width, height : windows size
* level : recursion level
* model :  current recursion model
* side : mirror side (0 or 1)
*/
glm::mat4x4 drawSceneRecMirror(int width, int height, int level, const glm::mat4x4 & model, const int side)
{
	glm::mat4x4 camView = gs.cam.GetCamView();
	glm::mat4x4 projection = gs.cam.GetCamProj(width, height);
	glm::mat4x4 m = projection * camView * model;

	glm::mat4x4 mirrorModel2 = getMirrorModel2(model);
	glm::mat4x4 mirrorModel = getMirrorModel(model);

	int currentLevel = (level + 1);

	glStencilMask(0xFF); // Write to stencil buffer

	glDepthMask(GL_FALSE); // Write to depth buffer

	glStencilFunc(GL_LESS, currentLevel, 0xFF); // Set any stencil to 1;
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawMirror(m, gs.vaoMirror2);
	drawMirror(m, gs.vaoMirror);


	if (level % 2 == side)
		return mirrorModel;
	else
		return mirrorModel2;
}


/** draw scene
* width, height : windows size
* sunPos : sun pos in world space
* colorMesh : mesh color
* level : recursion level
* model :  current recursion model
* side : max mirror recursion % 2
*/
void drawScene(int width, int height, const glm::vec4 & sunPos, const glm::vec4 & colorMesh, int level, const glm::mat4x4 & model, const int side)
{
	glm::mat4x4 camView = gs.cam.GetCamView();
	glm::mat4x4 projection = gs.cam.GetCamProj(width, height);
	glm::mat4x4 m = projection * camView * model;
	glm::mat4x4 lightView = getLightView(sunPos);
	glm::mat4x4 mLight = projection * lightView;

	
	glm::mat4x4 mirrorModel2 = getMirrorModel2(model);
	glm::mat4x4 mirrorModel = getMirrorModel(model);

	int currentLevel = (level + 1);
	// draw main skybox
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glDepthMask(GL_TRUE); 

	drawSkybox(m);

	// Write first stencil mirrors
	glEnable(GL_STENCIL_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	glDepthMask(GL_FALSE);

	drawMirror(m, gs.vaoMirror2);

	drawMirror(m, gs.vaoMirror);
	
	int invSide = 1 - side;
	std::vector<glm::mat4x4> models_1(level + 1);
	std::vector<glm::mat4x4> models_2(level + 1);
	models_1[level] = mirrorModel;
	models_2[level] = mirrorModel2;

	// draw mirrors stencil and part of the scenes affected in it
	for (int i = level - 1; i >= 0; i--)
	{
		models_2[i] = drawSceneRecMirror(width, height, i, models_2[i + 1], invSide);
		drawSceneRecMesh(width, height, sunPos, colorMesh, i, models_2[i + 1]);
		models_1[i] = drawSceneRecMirror(width, height, i, models_1[i + 1], side);
		drawSceneRecMesh(width, height, sunPos, colorMesh, i, models_1[i + 1]);
	}
	// draw skybox in the inverse iterations level to avoid overlap problem
	for (int i = 0; i < level; i++)
	{
		drawSceneSkybox(width, height, i, models_2[i + 1]);
		drawSceneSkybox(width, height, i, models_1[i + 1]);
	}

	// write first mirrors in depth buffer
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	drawMirror(m, gs.vaoMirror2);
	drawMirror(m, gs.vaoMirror);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glDepthFunc(GL_LESS);
	drawShadowMap(mLight);
	mLight =  Constantes::biasMatrix * mLight;
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, currentLevel, 0xFF);

	drawMesh(colorMesh, sunPos, m, mLight);
	drawMirror(m, gs.vaoMirror3);
	drawMirror(m, gs.vaoMirror4);
	drawSun(sunPos, m);
}

/* Drawing function
*/
void render(GLFWwindow* window)
{
	glm::vec4 color({ 1.f, 1.f, 1.f, 1.f });
	glfwGetWindowSize(window, &gs.width, &gs.height);

	// get frame size
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// get time
	float timePos = (float)(clock() * 0.001f);

	glm::vec4 sunPos = glm::vec4(sin(timePos)*  gs.sun_ray, gs.sun_y, cos(timePos) *  gs.sun_ray, 1.f);

	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);
	drawScene(width, height, sunPos, color, gs.maxBounce, glm::mat4x4(1.f), gs.maxBounce % 2);
	Controller::Instance().ProcessInput(window, gs.cam, gs.width, gs.height);
}
