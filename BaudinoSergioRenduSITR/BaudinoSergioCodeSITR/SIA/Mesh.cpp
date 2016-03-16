#include "Mesh.h"

/*
* Constructor
*/
Mesh::Mesh(std::vector<glm::vec3> points_, std::vector<glm::vec3> faces_, std::vector<glm::vec3> facesTextures_, std::vector<glm::vec3> facesNormales_, std::vector<glm::vec2> textures_, std::vector<glm::vec3> normales_)
	: points(points_), faces(faces_), textures(textures_), facesTextures(facesTextures_), facesNormalesIndex(facesNormales_), facesNormales(normales_) 
{
}

Mesh::Mesh(std::vector<glm::vec3> points_, std::vector<glm::vec3> faces_, std::vector<glm::vec3> facesTextures_, std::vector<glm::vec2> textures_)
	: points(points_), faces(faces_), textures(textures_), facesTextures(facesTextures_) {
}

/* Merge current mes with another in one object
* m : Mesh
*/
void Mesh::Merge(const Mesh & m)
{
	std::vector<glm::vec3> newFaces = m.faces;
	std::vector<glm::vec3> newFacesTextures = m.facesTextures;
	std::vector<glm::vec3> newFacesNormales = m.facesNormalesIndex;

	std::vector<glm::vec3> newPoints = m.points;
	std::vector<glm::vec2> newTextures = m.textures;
	std::vector<glm::vec3> newNormales = m.facesNormales;

	glm::vec3 offset((float)points.size());
	glm::vec3 offsetNormal((float)facesNormales.size());
	for (unsigned int i = 0; i < newFaces.size(); ++i)
		newFaces[i] += offset;
	for (unsigned int i = 0; i < newFacesTextures.size(); ++i)
		newFacesTextures[i] += offset;
	for (unsigned int i = 0; i < newFacesNormales.size(); ++i)
		newFacesNormales[i] += offsetNormal;

	faces.insert(faces.end(), newFaces.begin(), newFaces.end());
	facesTextures.insert(facesTextures.end(), newFacesTextures.begin(), newFacesTextures.end());
	facesNormalesIndex.insert(facesNormalesIndex.end(), newFacesNormales.begin(), newFacesNormales.end());

	points.insert(points.end(), newPoints.begin(), newPoints.end());
	textures.insert(textures.end(), newTextures.begin(), newTextures.end());
	facesNormales.insert(facesNormales.end(), newNormales.begin(), newNormales.end());
}

/* Get size of current mesh for VBO
* return : number of verteces
*/
int Mesh::getVBODataSize() const
{
	const int sizeByTriangle = 12;
	return faces.size() * sizeByTriangle;
}
/* Get size of current mesh for VBO
* return : number of verteces
*/
int Mesh::getVBOUvSize() const
{
	const int sizeByTriangle = 6;
	return facesTextures.size() * sizeByTriangle;
}

/* Export Mesh in VBO format
*  data, normal : preallocated float array to fill with vertex and normal information
*/
void Mesh::ExportVBODataWithUv(float * data, float * normal, float * uv) const
{
	ExportVBOData(data, normal);
	const int sizeByTriangle = 6;
	int size = facesTextures.size() * 6;
	int cmp = 0;
	for (int i = 0; i < size; i += sizeByTriangle)
	{
		uv[i] = textures[(int)facesTextures[cmp].x].x;
		uv[i + 1] = textures[(int)facesTextures[cmp].x].y;

		uv[i + 2] = textures[(int)facesTextures[cmp].y].x;
		uv[i + 3] = textures[(int)facesTextures[cmp].y].y;

		uv[i + 4] = textures[(int)facesTextures[cmp].z].x;
		uv[i + 5] = textures[(int)facesTextures[cmp].z].y;
		++cmp;
	}
}

/* Export Mesh in VBO format
*  data, normal : preallocated float array to fill with vertex and normal information
*/
void Mesh::ExportVBOData(float * data, float * normal) const
{
	const int sizeByTriangle = 12;
	int size = getVBODataSize();

	int cmp = 0;
	for (int i = 0; i < size; i += sizeByTriangle)
	{
 		data[i] = points[(int)faces[cmp].x].x;
		data[i + 1] = points[(int)faces[cmp].x].y;
		data[i + 2] = points[(int)faces[cmp].x].z;
		data[i + 3] = 1.f;
		
		data[i + 4] = points[(int)faces[cmp].y].x;
		data[i + 5] = points[(int)faces[cmp].y].y;
		data[i + 6] = points[(int)faces[cmp].y].z;
		data[i + 7] = 1.f;

		data[i + 8] = points[(int)faces[cmp].z].x;
		data[i + 9] = points[(int)faces[cmp].z].y;
		data[i + 10] = points[(int)faces[cmp].z].z;
		data[i + 11] = 1.f;
		
		normal[i] = facesNormales[(int)facesNormalesIndex[cmp].x].x;
		normal[i + 1] = facesNormales[(int)facesNormalesIndex[cmp].x].y;
		normal[i + 2] = facesNormales[(int)facesNormalesIndex[cmp].x].z;
		normal[i + 3] = 1.f;

		normal[i + 4] = facesNormales[(int)facesNormalesIndex[cmp].y].x;
		normal[i + 5] = facesNormales[(int)facesNormalesIndex[cmp].y].y;
		normal[i + 6] = facesNormales[(int)facesNormalesIndex[cmp].y].z;
		normal[i + 7] = 1.f;

		normal[i + 8] = facesNormales[(int)facesNormalesIndex[cmp].z].x;
		normal[i + 9] = facesNormales[(int)facesNormalesIndex[cmp].z].y;
		normal[i + 10] = facesNormales[(int)facesNormalesIndex[cmp].z].z;
		normal[i + 11] = 1.f;
		++cmp;
	}
}

/* Create mesh from obj
* filename : string containing file name
* t : translation vector
* s : scale vector
* angle : rotation angle in degrees
* axe : rotation axes
* return Mesh created from obj
*/
Mesh Mesh::fromObj(const char * filename, glm::vec3 t, glm::vec3 s, float angle, glm::vec3 axe)
{
	FILE* f;
	fopen_s(&f, filename, "r");
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normales;
	//std::vector<glm::vec3> normalesT;
	std::vector<glm::vec3> facesNormales;
	std::vector<glm::vec3> faces;
	while (!feof(f)) {
		char line[255];
		fgets(line, 255, f);
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 tmp;
			sscanf_s(line, "v %f %f %f\n", &tmp.x, &tmp.y, &tmp.z);
			points.push_back(tmp);
		}
		if (line[0] == 'v' && line[1] == 'n') {
			glm::vec3 tmp;
			sscanf_s(line, "vn %f %f %f\n", &tmp.x, &tmp.y, &tmp.z);
			normales.push_back(tmp);
		}
		if (line[0] == 'f') {
			int x, y, z;
			//int x2, y2, z2;
			int x3, y3, z3;
			//sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &x, &x2, &x3, &y, &y2, &y3, &z, &z2, &z3);
			sscanf_s(line, "f %d//%d %d//%d %d//%d\n", &x, &x3, &y, &y3, &z, &z3);
			faces.push_back(glm::vec3(x - 1, y - 1, z - 1));
			facesNormales.push_back(glm::vec3(x3 - 1, y3 - 1, z3 - 1));
		}
	}
	normalizePointList(points);
	scale(points, s);
	rotate(points, angle, axe);
	
	translate(points, t);
	/*rotate(normales, angle, axe);
	scale(normales, s);
	translate(normales, t);*/

	fclose(f);
	return Mesh(points, faces, std::vector<glm::vec3>(), facesNormales, std::vector<glm::vec2>(), normales);
}

/* Create mesh from obj
* filename : string containing file name
* t : translation vector
* s : scale vector
* angle : rotation angle in degrees
* axe : rotation axes
* return Mesh created from obj
*/
Mesh Mesh::fromObjWithUv(const char * filename, glm::vec3 t, glm::vec3 s, float angle, glm::vec3 axe)
{
	FILE* f;
	fopen_s(&f, filename, "r");
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normales;
	std::vector<glm::vec2> textures;
	std::vector<glm::vec3> facesNormales;
	std::vector<glm::vec3> facesTextures;
	std::vector<glm::vec3> faces;
	while (!feof(f)) {
		char line[255];
		fgets(line, 255, f);
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 tmp;
			sscanf_s(line, "v %f %f %f\n", &tmp.x, &tmp.y, &tmp.z);
			points.push_back(tmp);
		}
		if (line[0] == 'v' && line[1] == 'n') {
			glm::vec3 tmp;
			sscanf_s(line, "vn %f %f %f\n", &tmp.x, &tmp.y, &tmp.z);
			normales.push_back(tmp);
		}
		if (line[0] == 'v' && line[1] == 't') {
			glm::vec2 tmp;
			sscanf_s(line, "vt %f %f\n", &tmp.x, &tmp.y);
			textures.push_back(tmp);
		}
		if (line[0] == 'f') {
			int x, y, z;
			int x2, y2, z2;
			int x3, y3, z3;
			sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &x, &x2, &x3, &y, &y2, &y3, &z, &z2, &z3);
			faces.push_back(glm::vec3(x - 1, y - 1, z - 1));
			facesTextures.push_back(glm::vec3(x2 - 1, y2 - 1, z2 - 1));
			facesNormales.push_back(glm::vec3(x3 - 1, y3 - 1, z3 - 1));
		}
	}
	normalizePointList(points);
	scale(points, s);
	rotate(points, angle, axe);

	translate(points, t);
	/*rotate(normales, angle, axe);
	scale(normales, s);
	translate(normales, t);*/

	fclose(f);
	return Mesh(points, faces, facesTextures, facesNormales, textures, normales);
}

/* Rotate point list
*  points : point list
*  angle : rotation angle in degrees
*  axe : rotation axes
*/
void Mesh::rotate(std::vector<glm::vec3> & points, float angle, const glm::vec3 & axes)
{
	for (unsigned int i = 0; i < points.size(); i++)
	{
		glm::mat4 trans = glm::rotate(glm::mat4(1.0f), angle, axes);
		glm::vec4 tmpPoint = glm::vec4(points[i], 1.f) * trans;
		points[i] = glm::vec3(tmpPoint / tmpPoint.w);
	}
}

/* Translate point list
*  points : point list
*  t : translation vector
*/
void Mesh::translate(std::vector<glm::vec3> & points, const glm::vec3 & t)
{
	for (unsigned int i = 0; i < points.size(); i++)
	{
		points[i] += t;
	}
}

/* Scale point list
*  points : point list
*  s : scaling vector
*/
void Mesh::scale(std::vector<glm::vec3> & points, const glm::vec3 & s)
{
	for (unsigned int i = 0; i < points.size(); i++)
	{
		points[i] *= s;
	}
}

/* Normalize point list in unitary size and center in 0, 0, 0
*  points : points list
*/
void Mesh::normalizePointList(std::vector<glm::vec3> & points)
{
	glm::vec3 shift(0.f);
	for (unsigned int i = 0; i < points.size(); i++)
	{
		shift += points[i];
	}
	shift /= (double)points.size();

	translate(points, -shift);
	float minX, maxX, minY, maxY, minZ, maxZ, diffX, diffY, diffZ;
	maxX = (minX = points[0].x);
	maxY = (minY = points[0].y);
	maxZ = (minZ = points[0].z);
	for (unsigned int i = 1; i < points.size(); i++)
	{
		maxX = std::fmaxf(maxX, points[i].x);
		minX = std::fminf(minX, points[i].x);

		maxY = std::fmaxf(maxY, points[i].y);
		minY = std::fminf(minY, points[i].y);

		maxZ = std::fmaxf(maxZ, points[i].z);
		minZ = std::fminf(minZ, points[i].z);
	}
	diffX = maxX - minX;
	diffY = maxY - minY;
	diffZ = maxZ - minZ;

	for (unsigned int i = 0; i < points.size(); i++)
	{

		points[i].x = ((points[i].x - minX) - diffX * .5f) / (diffX * .5f);
		points[i].y = ((points[i].y - minY) - diffY * .5f) / (diffY * .5f);
		points[i].z = ((points[i].z - minZ) - diffZ * .5f) / (diffZ * .5f);
	}
}

/* Rotate current mesh
*  angle : rotation angle in degrees
*  axe : rotation axes
*/
void Mesh::rotate(float angle, const glm::vec3 & axes)
{
	rotate(points, angle, axes);
	rotate(facesNormales, angle, axes);
}

/* Translate current mesh
*  t : translation vector
*/
void Mesh::translate(const glm::vec3 & t)
{
	translate(points, t);
}

/* Scaling current mesh
*  s : scale vector
*/
void Mesh::scale(const glm::vec3 & s)
{
	scale(points, s);
}

glm::vec3 Mesh::calcNormal(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
{
	return glm::normalize(glm::cross(p1 - p0, p2 - p0));
}
/* Create mesh from quadrangle
* p0, p1, p2, p3 : mesh verteces
* return Mesh created from quadrangle
*/
Mesh Mesh::Quadrangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	std::vector<glm::vec3> points(4);
	points[0] = p0;
	points[1] = p1;
	points[2] = p2;
	points[3] = p3;

	std::vector<glm::vec3> faces(2);
	faces[0] = glm::vec3(0, 1, 2);
	faces[1] = glm::vec3(1, 3, 2);

	glm::vec3 normalT1 = calcNormal(p0, p1, p3);

	glm::vec3 normalT2 = calcNormal(p2, p1, p3);
	glm::vec3 normalMiddle = glm::normalize((normalT1 + normalT2) * 0.5f);
	std::vector<glm::vec3> normals(4);
	normals[0] = normalT1;
	normals[1] = normalMiddle;
	normals[2] = normalMiddle;
	normals[3] = normalT2;

	std::vector<glm::vec3> facesNormals(2);
	facesNormals[0] = glm::vec3(0, 1, 2);
	facesNormals[1] = glm::vec3(1, 3, 2);

	return Mesh(points, faces, std::vector<glm::vec3>(), facesNormals, std::vector<glm::vec2>(), normals);
}

/*
* return : Mesh Sphere
* o : origine de la sphere
* r : rayon de la sphere
*/
Mesh Mesh::Sphere(const float r, const int nbmeridiens, const int nbparalleles) {


	float anglemeridiens = (2 * Constantes::PI) / nbmeridiens;
	float angleparalleles = Constantes::PI / nbparalleles;
	Mesh sphere = Quadrangle(glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3());
	for (float i = 0; i<nbmeridiens; i++) {
		for (float j = 0; j<nbparalleles; j++) {

			glm::vec3 p0 = glm::vec3(r*sin(angleparalleles*j)*cos(anglemeridiens*i), r*sin(angleparalleles*j)*sin(anglemeridiens*i), r*cos(angleparalleles*j));
			glm::vec3 p1 = glm::vec3(r*sin(angleparalleles*j)*cos(anglemeridiens*(i + 1)), r*sin(angleparalleles*j)*sin(anglemeridiens*(i + 1)), r*cos(angleparalleles*j));
			glm::vec3 p2 = glm::vec3(r*sin(angleparalleles*(j + 1))*cos(anglemeridiens*i), r*sin(angleparalleles*(j + 1))*sin(anglemeridiens*i), r*cos(angleparalleles*(j + 1)));
			glm::vec3 p3 = glm::vec3(r*sin(angleparalleles*(j + 1))*cos(anglemeridiens*(i + 1)), r*sin(angleparalleles*(j + 1))*sin(anglemeridiens*(i + 1)), r*cos(angleparalleles*(j + 1)));
			if(i + j == 0)
				sphere = Quadrangle(p1, p3, p0, p2);
			else
				sphere.Merge(Quadrangle(p1, p3, p0, p2));

		}
	}
	return sphere;

}