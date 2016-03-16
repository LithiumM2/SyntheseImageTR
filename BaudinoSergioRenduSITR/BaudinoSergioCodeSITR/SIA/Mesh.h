#pragma once
#include <vector>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Constantes.h"
class Mesh
{
	/*
	* Numbre of vertices by triangle
	*/
	static const int SIZE_BY_TRIANGLE = 12;
	/*
	* Meshes vertices coord 
	*/
	std::vector<glm::vec3> points;

	/*
	* Meshes faces coord containing points index
	*/
	std::vector<glm::vec3> faces;

	/*
	* Meshes faces texture coord containing texture points index
	*/
	std::vector<glm::vec3> facesTextures;

	/*
	* Meshes faces normal coord containing normal points index
	*/
	std::vector<glm::vec3> facesNormalesIndex;

	/*
	* Meshes textures coord 
	*/
	std::vector<glm::vec2> textures;

	/*
	* Meshes normal faces coord 
	*/
	std::vector<glm::vec3> facesNormales;

	/*
	* Meshes normal verteces coord 
	*/
	std::vector<glm::vec3> verticesNormales;

public:
	/*
	* Constructor
	*/
	Mesh(std::vector<glm::vec3> points_, std::vector<glm::vec3> faces_, std::vector<glm::vec3> facesTextures_, std::vector<glm::vec3> facesNormales_, std::vector<glm::vec2> textures_, std::vector<glm::vec3> normales_);
	Mesh(std::vector<glm::vec3> points_, std::vector<glm::vec3> faces_, std::vector<glm::vec3> facesTextures_, std::vector<glm::vec2> textures_);

	/* Merge current mes with another in one object
	* m : Mesh
	*/
	void Merge(const Mesh & m);

	/* Get size of current mesh for VBO
	* return : number of verteces
	*/
	int getVBODataSize() const;
	/* Get size of current mesh for VBO
	* return : number of verteces
	*/
	int getVBOUvSize() const;
	/* Export Mesh in VBO format
	*  data, normal : preallocated float array to fill with vertex and normal information
	*/
	void ExportVBOData(float * data, float * normal) const;

	/* Export Mesh in VBO format
	*  data, normal : preallocated float array to fill with vertex and normal information
	*/
	void ExportVBODataWithUv(float * data, float * normal, float * uv) const;

	/* Rotate point list
	*  points : point list
	*  angle : rotation angle in degrees
	*  axe : rotation axes
	*/
	static void rotate(std::vector<glm::vec3> & points, float angle, const glm::vec3 & axe);

	/* Translate point list
	*  points : point list
	*  t : translation vector
	*/
	static void translate(std::vector<glm::vec3> & points, const glm::vec3 & t);

	/* Scale point list
	*  points : point list
	*  s : scaling vector
	*/
	static void scale(std::vector<glm::vec3> & points, const glm::vec3 & s);

	/* Rotate current mesh
	*  angle : rotation angle in degrees
	*  axe : rotation axes
	*/
	void rotate(float angle, const glm::vec3 & axe);

	/* Translate current mesh
	*  t : translation vector
	*/
	void translate(const glm::vec3 & t);
	
	/* Scaling current mesh
	*  s : scale vector
	*/
	void scale(const glm::vec3 & s);

	/* Normalize point list in unitary size and center in 0, 0, 0
	*  points : points list
	*/
	static void normalizePointList(std::vector<glm::vec3> & points);

	/* Create mesh from obj
	* filename : string containing file name
	* t : translation vector
	* s : scale vector
	* angle : rotation angle in degrees
	* axe : rotation axes
	* return Mesh created from obj
	*/
	static Mesh fromObj(const char * filename, glm::vec3 t, glm::vec3 s, float angle, glm::vec3 axe);
	static Mesh fromObjWithUv(const char * filename, glm::vec3 t, glm::vec3 s, float angle, glm::vec3 axe);

	/* Create mesh from quadrangle
	* p0, p1, p2, p3 : mesh verteces
	* return Mesh created from quadrangle
	*/
	static Mesh Quadrangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	static Mesh Sphere(const float, const int, const int);

	static glm::vec3 calcNormal(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
};

